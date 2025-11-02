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

#define NOT_PRESSED		1
#define BT_DEBOUNCING_COUNTER	10  // *10ms = 100 ms
#define ZERO_DETECTION_FILTER_COUNTER	100

#define BEEP_TIME 10000


#ifdef ATMEGA8
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB1)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB1) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB1)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB1)

	// Tyrystors
	#define		TR1_PORT_OUTPUT_SETUP()		DDRD |= (1<<PD5)
	#define		TR1_ON()					PORTD |= (1<<PD5)
	#define		TR1_OFF()					PORTD &= ~(1<<PD5)

	// BEEP
	#define		BEEP_PORT_OUTPUT_SETUP()	DDRD |= (1<<PD6)
	#define		BEEP_ON()					PORTD |= (1<<PD6)
	#define		BEEP_OFF()					PORTD &= ~(1<<PD6)

	// Interrupts

	// Inputs
	#define BT_B_PIN				PB0
	#define BT_B_DDR()				DDRB &= ~(1<<BT_B_PIN); PORTB |=(1<<BT_B_PIN)  // INPUT with pull up
	#define READ_BT_START_PIN()		(PINB&(1<<BT_B_PIN))>>BT_B_PIN


	#define SETUP_INPUTS()		BT_B_DDR()



#endif  // ATMEGA8
