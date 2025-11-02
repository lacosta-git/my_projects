/*
 * main.h
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

// CPU for which there project is configured
// Use ONLY ONE of defined
//#define ATMEGA2560
#define ATMEGA328P

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  16000000
#endif  // F_CPU

#include <avr/io.h>

#define FALSE 0
#define TRUE 1

#define LOW 0
#define HIGH 1

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

	// SDI
	#define		SET_SDI_PIN_AS_OUTPUT()		DDRB |= (1<<PB0)  // SDI pin as OUTPUT
	#define		SET_SDI_PIN_HIGH();			PORTB |= (1<<PB0)
	#define		SET_SDI_PIN_LOW();			PORTB &= ~(1<<PB0)

	// SDI
	#define		SET_SII_PIN_AS_OUTPUT()		DDRB |= (1<<PB1)  // SII pin as OUTPUT
	#define		SET_SII_PIN_HIGH();			PORTB |= (1<<PB1)
	#define		SET_SII_PIN_LOW();			PORTB &= ~(1<<PB1)

	// SDI
	#define		SET_SDO_PIN_AS_OUTPUT()		DDRB |= (1<<PB2)  // SDO pin as OUTPUT
	#define		SET_SDO_PIN_HIGH();			PORTB |= (1<<PB2)
	#define		SET_SDO_PIN_LOW();			PORTB &= ~(1<<PB2)

	// SDI
	#define		SET_SCI_PIN_AS_OUTPUT()		DDRB |= (1<<PB3)  // SCI pin as OUTPUT
	#define		SET_SCI_PIN_HIGH();			PORTB |= (1<<PB3)
	#define		SET_SCI_PIN_LOW();			PORTB &= ~(1<<PB3)

	// SDI
	#define		SET_RESET_PIN_AS_OUTPUT()		DDRB |= (1<<PB4)  // RESET pin as OUTPUT
	#define		SET_RESET_PIN_HIGH();			PORTB |= (1<<PB4)
	#define		SET_RESET_PIN_LOW();			PORTB &= ~(1<<PB4)

	// Inputs
	#define _SDO_PIN						PB2
	#define SET_SDO_PIN_AS_INPUT()			DDRB &= ~(1<<_SDO_PIN)
	#define READ_SDO_PIN()					(PINB&(1<<_SDO_PIN))>>_SDO_PIN
	// Interrupts

#endif

#ifdef ATMEGA2560
	// Pinout Arduino MEGA2560
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB7)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB7) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB7)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB7)


	// Inputs

	// Interrupts

#endif

