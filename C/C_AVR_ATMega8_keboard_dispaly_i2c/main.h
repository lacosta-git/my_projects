/*
 * main.h
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

// CPU for which there project is configured
#define ATMEGA8

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  1000000
#endif  // F_CPU

#include <avr/io.h>

#define FALSE 0
#define TRUE 1

#ifdef ATMEGA8
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB1)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB1) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB1)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB1)
	// Interrupts

#endif  // ATMEGA8
