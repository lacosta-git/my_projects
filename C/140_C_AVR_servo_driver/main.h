/*
 * main.h
 *
 *  Created on: 30.03.2018
 *      Author: lapiUser
 */
#include <avr/io.h>

// General
#define FALSE 0
#define TRUE 1

#define ATMEGA328P

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

	// Interrupts
	#define INT_T1 		TIMER1_OVF_vect
	// Timer reload value for 2,5 ms
	#define PERIOD  40000
	#define RELOAD	65535 - PERIOD

#endif // ATMEGA328P

