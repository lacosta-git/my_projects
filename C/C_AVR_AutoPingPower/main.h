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
#define OFF 0
#define ON 1

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

	// SPI
	#define		SPI_PORT		PORTB
	#define		SPI_CS			PORTB2
	#define		SPI_MOSI		PORTB3
	#define		SPI_MISO		PORTB4
	#define		SPI_SCK			PORTB5

	// Set MOSI (PORTB3),SCK (PORTB5) and PORTB2 (SS) as output, others as input
	#define		DDR_B()			DDRB = (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_CS)

	// Inputs

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

