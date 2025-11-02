/*
 * Main library for serial communication
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 *
 *
 *  Usage:
 *  	...
 *  	rsSend("\r>> Your text");
 *  	...
 *
 *  Than in main loop there must be called handler.
 *  	...
 *  	rsSendHandler();
 *  	...
 */

#include "rs232.h"

volatile uint8_t* rs_data_register;  // USART Data Register
volatile uint8_t* rs_control_register;  // USART Control Register
volatile uint8_t rs_data_register_empty;  // USART Data Register Empty flag

void init_serial_communication(uint16_t baudrate){
	#ifdef ATMEGA328P
		// ATmega328P setup
		//calculate UBRR value
		uint16_t ubrr;
		ubrr = ((F_CPU / (baudrate * 16UL)) - 1);

		/*Set baud rate */
		UBRR0H = (uint8_t)(ubrr>>8);
		UBRR0L = (uint8_t)ubrr;

		//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
		UCSR0C=(0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);

		//Enable Transmitter and Receiver and NO Interrupt on receive complete
		UCSR0B=(1<<RXEN0)|(1<<TXEN0);

		rs_data_register = &UDR0;
		rs_control_register = &UCSR0A;
		rs_data_register_empty = UDRE0;

	#endif  // ATMEGA328P

	#ifdef ATMEGA2560
		// ATmega2560 setup for RSD0
		//calculate UBRR value
		uint16_t ubrr;
		ubrr = ((F_CPU / (baudrate * 16UL)) - 1);

		/*Set baud rate */
		UBRR0H = (uint8_t)(ubrr>>8);
		UBRR0L = (uint8_t)ubrr;

		//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
		UCSR0C=(0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);

		//Enable Transmitter and Receiver and NO Interrupt on receive complete
		UCSR0B=(1<<RXEN0)|(1<<TXEN0);

		rs_data_register = &UDR0;
		rs_control_register = &UCSR0A;
		rs_data_register_empty = UDRE0;

	#endif  // ATMEGA2560

	txBuf[0] = 0;
}

void decreaseTxBuffer(void){
	uint8_t charsToSend = txBuf[0];
	// roll left data in tx buffer
	for (uint8_t i = 1; i <= charsToSend; i++){
		txBuf[i] = txBuf[i+1];
	}

	// Clear last char in buffer
	txBuf[charsToSend] = 0;

	// Decrease number of chars to send by 1
	txBuf[0] = charsToSend - 1;
}

// txBuffer [charsToSend, char1, char2, char3, ...]
void rs_send_handler(void){
	// Check if there are chars to send
	if (txBuf[0] > 0){
		// Check if uC tx buffer is empty
		if ( *rs_control_register & (1<<rs_data_register_empty)){
			// Sending char to uC tx buffer
			*rs_data_register = txBuf[1];
			decreaseTxBuffer();  // Update tx buffer
		}
	}
}

// Message is put to sending buffer
// End of string is detected when 0x00 in ASCII
// Using "Your string" automatically adds 0x00 at the end of string.
void rs_send(char* msg){
	while(*msg){
		txBuf[++txBuf[0]] = *msg;
		msg++;
	}
}
