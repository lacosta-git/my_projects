/*
 * main.c
 *
 *  Created on: 16 paü 2016
 *      Author: autoMaster
 */

#ifndef F_CPU
# define F_CPU 8000000UL
#endif

#define TRUE  1
#define FALSE 0

#define NO_ERROR	0
#define SYNTAX_ERROR 0x01

#define USART_ACK   0
#define USART_BUSY	1
#define USART_RX_NO_NEW_DATA 0
#define CMD_INDEX_MAX 100
#define USART_TX_MAX 100
#define SEND_CRC 254
#define END_OF_SENDING 255

#define SAVE_DIVIDER 3

#define DS_1307_RAM_MM_ADDRESS	0x10	// DS1307 RAM pointer address used for MM address

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "util/delay.h"
#include "i2cmaster/i2cmaster.h"
#include "ds1307/ds1307.h"
#include "spi/spi.h"
#include "pinConfig/pinConfig.h"
#include "at45db/at45db0xx.h"

const char allOK[] PROGMEM = "00 ACK\0";
const char syntaxError[] PROGMEM = "01 Syntax error\0";
const char unknownCommand[] PROGMEM = "02 Unknown command\0";
const char argumentOutOfrange[] PROGMEM = "03 Argument out of range\0";

PGM_P errorTable[] PROGMEM = {
		allOK,
		syntaxError,
		unknownCommand,
		argumentOutOfrange
};


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

	//Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSR0C=(0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);
	//Enable Transmitter and Receiver and NO Interrupt on receive complete
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
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


// **********************************************
// ****     main loop functions              ****
// **********************************************
// Main variables
uint8_t err;			//  error from function call
uint8_t txStstus;
uint8_t cmdIndex;
uint8_t newCommadReceived;
uint8_t replayReadyToSend;
uint8_t txIndex;
uint8_t i;
uint8_t year = 0;
uint8_t month = 0;
uint8_t day = 0;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t oldMinute = 0;
uint8_t second = 0;
uint8_t oldSecond = 0;
uint16_t rxCmd = 0;
uint16_t msrNumber = 0;
uint16_t loopCounter = 0;
uint8_t currAdcChannel = 0;
uint8_t adcChannel[] = {0, 0, 0};
uint8_t divaider = SAVE_DIVIDER;
unsigned char dataChar;
unsigned char rep[USART_TX_MAX + 1];
unsigned char command[CMD_INDEX_MAX + 1];
unsigned char parseCMD[CMD_INDEX_MAX + 1];

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

// ADD one ASCII character CRC
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

// GET DATE and TIME
void getDateTime(char* timeDate)
{
	ds1307_getdate_BCD(&year, &month, &day, &hour, &minute, &second);
	uint8_t table[] = {year, month, day, hour, minute, second};
	*timeDate = 21; timeDate ++;     // Length
	*timeDate = 0x32; timeDate ++;  // 2
	*timeDate = 0x30; timeDate ++;  // 0

	for (uint8_t i = 0; i <= 5; i++){
		*timeDate = ((table[i] & 0xF0) >> 4) + 0x30; timeDate ++;
		*timeDate = (table[i] & 0x0F) + 0x30; timeDate ++;
		*timeDate = 0x3a; timeDate ++;  // :
	}

	*timeDate = 0x0d;
}

// SET DATE and TIME
void setDateTime(char* timeDate)
{
	uint8_t data[7];
	//:sd-16.10.20.01.22.20.00;
	timeDate += 2;
	for (uint8_t i = 0; i <= 6; i++)
	{
		timeDate += 2;
		data[i] = *timeDate;
		data[i] = (data[i] - 0x30) << 4;
		timeDate ++;
		data[i] = data[i] + ((*timeDate - 0x30) & 0x0F);
	}

	// Set date
	ds1307_setdate_BCD(data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
}

void saveMsrNumber(uint16_t number){
	ds1307_write_RAM_W(number, DS_1307_RAM_MM_ADDRESS);
}

uint16_t loadMsrNumber(void){
	return ds1307_read_RAM_W(DS_1307_RAM_MM_ADDRESS);
}

// Save data to MMemory
void saveDataToMM(uint8_t dataL1, uint8_t dataL2, uint8_t dataL3, uint16_t msrNumber){
	uint8_t data[] = {year, month, day, hour, minute, dataL1, dataL2, dataL3};
	uint32_t addr = (uint32_t)msrNumber;
	addr = addr << 3;
	writeFlash((char*)&data, addr, 8);
}

// Read data batch (8 byte) from MMemory
// DataNumber is 8 byte set of data + measurements
void readDataFromMM(char* ptrData, char* ptrDataNumber){
	uint32_t address = 0;
	uint8_t data[8];
	uint8_t digit = 0;

	for (uint8_t i = 4; i <= 7; i++){
		digit = *(ptrDataNumber + i);
		if ((digit >= 0x30) & (digit <= 0x39)){
			// digit 0...1 detected
			digit = digit & 0x0F;
		}

		if (((digit >= 0x41) & (digit <= 0x46))|
			((digit >= 0x61) & (digit <= 0x66))){
				// digit A...F detected
				// digit a...f detected
				digit = digit & 0x0F;
				digit = digit + 0x09;
		}

		address = address << 4;
		address = address + digit;
	}

	address = address << 3;
	readFlash((char*)&data, address, 8);

	for (uint8_t i = 0; i <= 7; i++){
		digit = data[i];
		digit = (data[i] >> 4) + 0x30;
		if (digit > 0x39) digit = digit + 0x07;
		*ptrData =  digit; ptrData++;
		digit = (data[i] & 0x0F) + 0x30;
		if (digit > 0x39) digit = digit + 0x07;
		*ptrData =  digit; ptrData++;
		*ptrData = 0x3A; ptrData++;
	}
}


/* ADC  */
void adcInit(uint8_t channel){
	// REF = AVCC with external capacitor at AREF pin
	// Left Adjust Result
	// Channel 0 (ADC0) selected
	ADMUX = ((0<<REFS1)|(1<<REFS0)|(1<<ADLAR)) + channel;

	// Disable digital inputs for ADC channels 2,1,0
	DIDR0 = (1<<ADC2D)|(1<<ADC1D)|(1<<ADC0D);

	// Free running mode
	ADCSRB = 0;

	// Enable ADC
	// START conversion
	// Auto trigger
	// precaler = Clk/128
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}


int main (void){
// Initialization
// RS232
	err = USART0_init(9600);

// PORTS
	MEM_CS_AS_OUTPUT;
	MEM_CS_HIGH;
	YELLOW_LED_PIN_AS_OUTPUT;
	YELLOW_LED_OFF;
	RED_LED_PIN_AS_OUTPUT;
	RED_LED_OFF;

// Variables init
	cmdIndex = 0;
	newCommadReceived = FALSE;
	replayReadyToSend = FALSE;
	txIndex = END_OF_SENDING;
	divaider = SAVE_DIVIDER;

// Init DS1307
	ds1307_init();

// SPI
	spiMasterInit();

// Main Memory
	msrNumber = loadMsrNumber();

// Get current date
	ds1307_getdate_BCD(&year, &month, &day, &hour, &minute, &second);
	oldSecond = second;
	oldMinute = minute;

// ADC init
	adcInit(0);

//Main loop
while(1){
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

		// Get date
		//:gd; -> YYYY:MM:DD:HH:MM:SS;
		if (rxCmd == compute('g', 'd')){
			getDateTime((char*)&rep);
		}

		// Set date
		//:sd-16.10.20.01.22.20.00;
		if (rxCmd == compute('s', 'd')){
			setDateTime((char*)&parseCMD);
			replError(NO_ERROR);
		}

		// Read measurement (number) - msrNumber
		// :rm-hhhh; hhhh - hex number of measurement (8 byte data batch)
		// -> YY:MM:DD:HH:MM:L1:L2:L3:
		if (rxCmd == compute('r', 'm')){
			readDataFromMM((char*)&rep + 1, (char*)&parseCMD);
			rep[0] = 25;  // length
			rep[25] = 0x0d;
		}

		// Set USART baudrate
		//:b1; -> 9600;
		//:b2; -> 18200;
		//:b3; -> 38400;
		//:b4; -> 57600;
		if (rxCmd == compute('b', '1')){
			err = USART0_init(9600);
			replError(NO_ERROR);}
		if (rxCmd == compute('b', '2')){
			err = USART0_init(18200);
			replError(NO_ERROR);}
		if (rxCmd == compute('b', '3')){
			err = USART0_init(38400);
			replError(NO_ERROR);}
		if (rxCmd == compute('b', '4')){
			err = USART0_init(57600);
			replError(NO_ERROR);}

		// TESTY
		// Reset settings
		// :re;
		if (rxCmd == compute('r', 'e')){
			saveMsrNumber(0);
			msrNumber = 0;
			replError(NO_ERROR);
		}

		// Read measurement number
		// :rn; -> HHHH
		if (rxCmd == compute('r', 'n')){
			rep[0] = 3;  // length
			rep[1] = (uint8_t)(msrNumber >> 8);
			rep[2] = (uint8_t)(msrNumber);
			rep[3] = 0x0d;
		}

		if (rxCmd == compute('c', 'e')){
			chipErase();
			replError(NO_ERROR);
		}

		// Current measurement read
		// :ar; -> L1L2L3
		if (rxCmd == compute('a', 'r')){
			rep[0] = 4;  // length
			rep[1] = adcChannel[0];
			rep[2] = adcChannel[1];
			rep[3] = adcChannel[2];
			rep[4] = 0x0d;
		}

		// Flash Status Register read
		// :rs; -> BB status register
		if (rxCmd == compute('r', 's')){
			MEM_CS_LOW;
			spiWriteByte(0xd7);
			rep[0] = 2;  // length
			rep[1] = spiReadByte();
			rep[2] = 0x0d;
			MEM_CS_HIGH;
		}

		if (rxCmd == compute('f', 'g')){
			saveDataToMM(minute, second, (uint8_t)msrNumber, msrNumber);
			msrNumber++;
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

	// Current time and date update
	loopCounter++;
	if (loopCounter == 0) ds1307_getdate_BCD(&year, &month, &day, &hour, &minute, &second);

	// Get ADC measurements every second
	if (oldSecond != second){
		oldSecond = second;
		YELLOW_LED_TOGLE;

		// Calculate avr   (curr + last) / 2
		adcChannel[currAdcChannel] = ((adcChannel[currAdcChannel] + ADCH) >> 1);

		if (currAdcChannel == 2){currAdcChannel = 0;}
		else{currAdcChannel++;}
		adcInit(currAdcChannel);
	}

	if (oldMinute != minute){
		oldMinute = minute;

		// Save data to Main Memory every time divider is 0
		divaider--;
		if (divaider == 0){
			divaider = SAVE_DIVIDER;
			saveDataToMM(adcChannel[0], adcChannel[1], adcChannel[2], msrNumber);
			msrNumber++;
			saveMsrNumber(msrNumber);
		}

	}
}}
