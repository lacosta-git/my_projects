/*
 * LocalLib_USART.c
 *
 * Created: 2014-02-21 17:40:39
 *  Author: Tomasz Szkudlarek
 */


#include <avr/io.h>
#include "LocalLib_USART.h"

#define RS_UBRR F_CPU / 16 / RS_BAUD - 1

void uart_init(uint16_t ubrr)
{
	// Setting transmition speed
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	
	// Enabling receiving and transmitting hardware
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Data frame setup:
	// 8 bit data, 1 stop bit, no parity
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}