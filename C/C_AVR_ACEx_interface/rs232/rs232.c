/*
 * Main library for serial communication
 *
 *  Created on: 15.09.2017
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

void init_serial_communication(uint16_t baudrate){
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
		if ( UCSR0A & (1<<UDRE0)){
			// Sending char to uC tx buffer
			UDR0 = txBuf[1];
			decreaseTxBuffer();  // Update tx buffer
		}
	}
}

// Message is put to sending buffer
void rs_send(char* msg){
	while(*msg){
		txBuf[++txBuf[0]] = *msg;
		msg++;
	}
}
