/*
 * main.c
 *
 *  Created on: 12.02.2017
 *      Author: autoMaster
 */


#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>

const char allOK[] PROGMEM = "00 ACK\0";
const char syntaxError[] PROGMEM = "01 Syntax error\0";
const char unknownCommand[] PROGMEM = "02 Unknown command\0";
const char argumentOutOfrange[] PROGMEM = "03 Argument out of range\0";

PGM_P const errorTable[] PROGMEM = {
		allOK,
		syntaxError,
		unknownCommand,
		argumentOutOfrange
};

// General
#define TRUE  1
#define FALSE 0

// USART
#define USART_ACK   0
#define USART_BUSY	1
#define USART_RX_NO_NEW_DATA 0
#define CMD_INDEX_MAX 100
#define USART_TX_MAX 100
#define SEND_CRC 254
#define END_OF_SENDING 255
#define NO_ERROR	0
#define SYNTAX_ERROR 0x01

// 1 - Output
// 0 - Input
// Port D direction
#define		DDR_D()			DDRD = (1<<DDD7)

#define		LED_RED_OFF()	PORTD &=~(1<<DDD7)
#define		LED_RED_ON()	PORTD |= (1<<DDD7)
#define		LED_RED_TRG()	PORTD ^= (1<<DDD7)


// filter input
#define HIGH 1
#define LOW 0
#define LEVEL_FILTER_MIN 0
#define LEVEL_FILTER_MAX 2
#define NOT_DEFINED 0
#define DATA_VALID 1

// bit [ticks]
#define TSmin 5		// T of Standard space
#define TS	  10
#define TSmax 15
#define T0min 15	// T0
#define T0    20
#define T0max 25
#define T1min 5		// T1
#define T1    10
#define T1max 15
#define IDLE 0
#define LOW_COUNTING 1
#define LOW_EVALUATE 2
#define LOW_ERROR 3

typedef struct {
	volatile uint8_t pinLevel;				// current input port level 0-Low, 1 - High
	volatile uint8_t signal;				// filtered input port level
	volatile uint8_t levelFilter;			// current value of filter
	volatile uint8_t lastBit;				// value of last valid bit
	volatile uint8_t validity;				// validity byte of bit value
	volatile uint8_t bitState;				// bit detection state machine
	volatile uint8_t lowCount;				// counter of LOW level
	volatile uint8_t highCount;				// counter of HIGH level
} INPUT;

#define INPUTS_NO 2			// Number of inputs
INPUT inputs[INPUTS_NO];
#define inputA 0
#define inputB 1


typedef struct {
	volatile uint8_t data;					// data to send on sBus
	volatile uint8_t bitState;				// bit sending state machine
	volatile uint8_t bit;					// bit value
	volatile uint8_t bitNo;					// bit number
	volatile uint8_t count;					// counter of bit time
	volatile uint8_t byteState;				// byte sending state machine
	volatile uint8_t pinLevel;				// current input port level 0-Low, 1 - High
} OUTPUT;

#define OUTPUTS_NO 2		// number of outputs
OUTPUT outputs[OUTPUTS_NO];
#define outputA 0
#define outputB 1
// output state machine
#define START_SENDING 1
#define PREPARE_DATA 2
#define SENDING 3
#define PREPARE_SPACE 4
#define SEND_SPACE 5
#define SPACE_SENT 6


// Variables
volatile uint8_t i = 0;			// general purpose variable
volatile uint8_t err;			//  error from function call
volatile uint16_t counter = 0;
volatile uint8_t txStstus;
volatile uint8_t cmdIndex;
volatile uint8_t newCommadReceived;
volatile uint8_t replayReadyToSend;
volatile uint8_t txIndex;
volatile uint16_t rxCmd = 0;
unsigned char dataChar;
unsigned char rep[USART_TX_MAX + 1];
unsigned char command[CMD_INDEX_MAX + 1];
unsigned char parseCMD[CMD_INDEX_MAX + 1];

/**********************************************************
 **************     FUNCTIONS     *************************
 **********************************************************/

// string to hex
uint8_t str2hex(uint8_t ch){
	uint8_t digit = 0;
	// 0..9
	if ((ch >= 0x30) & (ch <= 0x39)) {
		digit = (ch & 0x0F);
	}

	// A...F
	if ((ch >= 0x41) & (ch <= 0x46)) {
		digit = ((ch & 0x0F) + 0x09);
	}

	// a...f
	if ((ch >= 0x61) & (ch <= 0x66)) {
		digit = ((ch & 0x0F) + 0x09);
	}
	return digit;
}

// **********************************************
// **********         USART              ********
// **********************************************
uint8_t USART0_init(uint16_t baudrate){
	// ATmega328P setup
	//calculate UBRR value
	uint16_t ubrr;
	ubrr = ((F_CPU / (baudrate * 16UL)) - 1);

	/*Set baud rate */
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;

	// Disable double speed
	UCSR0A = (0<<U2X0);

	//Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSR0C=(0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);
	return (NO_ERROR);
}

// Reading RX buffer
// @return USART_RX_NO_NEW_DATA     if there is no new data in RX buffer
//         char  if there is data in RX buffer
uint8_t USART0_readBuff(void){
	/* Wait for data to be received */
	if (UCSR0A & (1<<RXC0)){
		/* Get and return received data from buffer */
		return UDR0;
		}
	else return USART_RX_NO_NEW_DATA;
}

// Send Byte
// @param  (char)data 	byte to be send
// @return   USART_ACK if char accepted to send
// 			 USART_BUSY if sending buffer busy
uint8_t USART0_sendChar(unsigned char data){
		if ( UCSR0A & (1<<UDRE0)){
			UDR0 = data;
			return USART_ACK;
		}
		else return USART_BUSY;
}

// Compute values
uint16_t compute(uint8_t hByte, uint8_t lByte){
	uint16_t data;
	data = 0;
	data = hByte;
	data = data << 8;
	data = data + lByte;
	return data;
}

//REPLY
void replError(uint8_t errorNo){
	uint8_t strLen;
	char * ptr = (char *) pgm_read_word (&errorTable[errorNo]);
	strcpy_P((char *)rep + 1, ptr);
	strLen = strlen_P(ptr) + 1;
	rep[0] = strLen;
	rep[strLen] = 0x0d;
}

// ADD  ASCII CRC
void addCRC(void){
	uint8_t strLen = 0;
	uint8_t crc = 0;

	strLen = rep[0];
	for (uint8_t i=1; i<strLen; i++){
		crc = crc + rep[i];
	}

	// Add crc to message
	i = rep[strLen];
	rep[strLen] = ':';
	rep[strLen + 1] = (crc >> 4) + 0x41;
	rep[strLen + 2] = (crc & 0x0F) + 0x41;
	rep[strLen + 3] = i;
	rep[0] = strLen + 3;
}


// **********************************************
// **********       SIMPLE BUS           ********
// **********************************************
// **********************************************
// **********          INPUT             ********
// **********************************************
/* if filter >= MAX => signal = High
 * if filter <= Min => signal = Low */
void filterInputs(void){
	for (uint8_t inp = 0; inp <= INPUTS_NO; inp++){
		if (inputs[inp].pinLevel == HIGH) {
			if (inputs[inp].levelFilter < LEVEL_FILTER_MAX) inputs[inp].levelFilter ++;
		}
		if (inputs[inp].pinLevel == LOW) {
			if (inputs[inp].levelFilter > LEVEL_FILTER_MIN) inputs[inp].levelFilter --;
		}
		if (inputs[inp].levelFilter == LEVEL_FILTER_MAX) inputs[inp].signal = HIGH;
		if (inputs[inp].levelFilter == LEVEL_FILTER_MIN) inputs[inp].signal = LOW;
	}
}

void detectBits(void){
	for (uint8_t inp = 0; inp <= INPUTS_NO; inp++){
		// IDLE -> LOW_COUNTING
		if (inputs[inp].bitState == IDLE){
			if (inputs[inp].signal == LOW){
				inputs[inp].bitState = LOW_COUNTING;
				inputs[inp].lowCount = 0;
				inputs[inp].validity = NOT_DEFINED;
			}
		}

		// COUNTING LOW
		if (inputs[inp].bitState == LOW_COUNTING){
			if (inputs[inp].signal == LOW){
				inputs[inp].lowCount++;
			}
			else {
				inputs[inp].bitState = LOW_EVALUATE;
			}
		}

		// EVALUATE LOW
		if (inputs[inp].bitState == LOW_EVALUATE){
			if ((inputs[inp].lowCount >= T0min) &
				(inputs[inp].lowCount <= T0max)){
				inputs[inp].lastBit = 0;
				inputs[inp].validity = DATA_VALID;
			}
			if ((inputs[inp].lowCount >= T1min) &
				(inputs[inp].lowCount <= T1max)){
				inputs[inp].lastBit = 1;
				inputs[inp].validity = DATA_VALID;
			}
			inputs[inp].highCount = 0;
		}

		// LOW ERROR DETECTION
		if (inputs[inp].lowCount > T0max){
			inputs[inp].validity = NOT_DEFINED;
			inputs[inp].bitState = LOW_ERROR;
		}
		if (inputs[inp].lowCount < T1min){
			inputs[inp].validity = NOT_DEFINED;
			inputs[inp].bitState = LOW_ERROR;
		}

		// HIGH counting
		if (inputs[inp].signal == HIGH){
			if (inputs[inp].highCount < TSmax){
				inputs[inp].highCount++;
			}
		}

		// HIGH evaluating
		if (inputs[inp].highCount > TSmin){
			inputs[inp].bitState = IDLE;
		}

	}
}

// **********************************************
// **********          OUTPUT            ********
// **********************************************
void outputSBusHandler(void){
	// IDLE -> START_SENDING
	if (outputs[outputA].bitState == START_SENDING){
		outputs[outputA].bitNo = 0;
		outputs[outputA].bitState = PREPARE_DATA;
	}

	// PREPARE_DATA
	if (outputs[outputA].bitState == PREPARE_DATA){
		outputs[outputA].bit = (outputs[outputA].data >> outputs[outputA].bitNo) & 0x01;

		if (outputs[outputA].bit == HIGH){
			outputs[outputA].count = T1;
		}
		else {
			outputs[outputA].count = T0;
		}
		outputs[outputA].bitState = SENDING;
	}

	// SENDING BIT
	if (outputs[outputA].bitState == SENDING){
		outputs[outputA].pinLevel = LOW;

		if (outputs[outputA].count > 0){
			outputs[outputA].count --;
		}
		else {
			outputs[outputA].bitState = PREPARE_SPACE;
		}
	}

	// PREPARE SPACE
	if (outputs[outputA].bitState == PREPARE_SPACE){
		outputs[outputA].count = TS;
		outputs[outputA].pinLevel = HIGH;
		outputs[outputA].bitState = SEND_SPACE;
	}

	// SENDING SPACE
	if (outputs[outputA].bitState == SEND_SPACE){
		if (outputs[outputA].count > 0){
			outputs[outputA].count --;
		}
		else {
			outputs[outputA].bitState = SPACE_SENT;
		}
	}

	if (outputs[outputA].bitState == SPACE_SENT){
		// check if last bit was sent
		if (outputs[outputA].bitNo == 7){
			// all was sent
			outputs[outputA].pinLevel = HIGH;
			outputs[outputA].bitState = IDLE;
		}
		else {
			// there are still bits to send
			outputs[outputA].bitNo ++;
			outputs[outputA].bitState = PREPARE_DATA;
		}
	}
}

// put data into sending buffer
void sendByteOnSBus(uint8_t B_Data){
	outputs[outputA].data = B_Data;
	outputs[outputA].bitState = START_SENDING;
}

// Sending buffer ready for new data
// return TRUE or FALSE
uint8_t isSendBufferReadyForNewData(void){
	if (outputs[outputA].bitState == IDLE) {
		return TRUE;
	}
	else{
		return FALSE;
	}
}
// **********************************************
// **********       MAIN LOOP            ********
// **********************************************
int main(){
// Initialization
// RS232
err = USART0_init(9600);

// PORTS
DDR_D();

// Variables init
cmdIndex = 0;
newCommadReceived = FALSE;
replayReadyToSend = FALSE;
txIndex = END_OF_SENDING;

for (uint8_t inp = 0; inp <= INPUTS_NO; inp++){
	inputs[inp].pinLevel = 1;
	inputs[inp].signal = 1;
	inputs[inp].levelFilter = LEVEL_FILTER_MAX;
	inputs[inp].lastBit = 0;
	inputs[inp].validity = NOT_DEFINED;
	inputs[inp].bitState = IDLE;
	inputs[inp].lowCount = 0;
	inputs[inp].highCount = TSmax;
}

outputs[outputA].pinLevel = HIGH;

filterInputs();
detectBits();

while (1){
	//Command building
	// Command format
	// :cc;
	dataChar = USART0_readBuff();
	if (dataChar != USART_RX_NO_NEW_DATA){
		if (cmdIndex >= CMD_INDEX_MAX - 2) cmdIndex = 0;
		if (dataChar ==';') newCommadReceived = TRUE;
		if (dataChar ==':') cmdIndex = 0;
		else command[cmdIndex] = dataChar;
		cmdIndex += 1;
	}

	// Execute command
	if (newCommadReceived == TRUE){
		newCommadReceived = FALSE;
		for (i = 0; i <= CMD_INDEX_MAX; i++) {
			parseCMD[i] = command[i];
			command[i] = 0; // Clear command
		}

		replError(SYNTAX_ERROR);  // Default response
		//Parse and execute command
		rxCmd = compute(parseCMD[1], parseCMD[2]);

		// RS Commands

		// Replay OK
		//:ro; -> ACK OK;
		if (rxCmd == compute('r', 'o')){
			replError(NO_ERROR);
		}

		// Send BYTE on sBUS
		//:sb.a4; -> ACK OK;
		if (rxCmd == compute('s', 'b')){
			uint8_t data = 0;
			data = str2hex(parseCMD[4]) << 4 ;
			data = data + str2hex(parseCMD[5]);
			sendByteOnSBus(data);
			replError(NO_ERROR);
		}


		replayReadyToSend = TRUE;
	}
	// Sending back replay message
	if (replayReadyToSend == TRUE){
		replayReadyToSend = FALSE;
		txIndex = 1;
		for (i = 0; i <= CMD_INDEX_MAX; i++) parseCMD[i] = 0;  // Clear command
		addCRC();
	}
	if (txIndex <= rep[0]){
		txStstus = USART0_sendChar(rep[txIndex]);
		if (txStstus == USART_ACK) txIndex += 1;
	}
	else txIndex = END_OF_SENDING;



	counter--;
	if (counter == 0) {
		counter = 0x0f00;
		outputSBusHandler();
	}

	if (outputs[outputA].pinLevel == HIGH){
		LED_RED_OFF();
	}
	else {
		LED_RED_ON();
	}
}}
