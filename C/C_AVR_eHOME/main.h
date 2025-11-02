/*
 * main.h
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

// CPU for which there project is configured
// Use ONLY ONE of defined
#define ATMEGA328P
//#define ATMEGA328P

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  16000000
#endif  // F_CPU

#include <avr/io.h>

#define FALSE 0
#define TRUE 1

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

	// eHOME temp read board
	#define H_MUX_A0_DDR()			DDRC |= (1<<PC5)
	#define H_MUX_A0_ON()			PORTC |= (1<<PC5)
	#define H_MUX_A0_OFF()			PORTC &= ~(1<<PC5)

	#define H_MUX_A1_DDR()			DDRC |= (1<<PC4)
	#define H_MUX_A1_ON()			PORTC |= (1<<PC4)
	#define H_MUX_A1_OFF()			PORTC &= ~(1<<PC4)

	#define H_MUX_A2_DDR()			DDRC |= (1<<PC3)
	#define H_MUX_A2_ON()			PORTC |= (1<<PC3)
	#define H_MUX_A2_OFF()			PORTC &= ~(1<<PC3)

	#define H_MUX_DDR()		H_MUX_A0_DDR(); H_MUX_A1_DDR(); H_MUX_A2_DDR()
	// ADC Inputs

	#define H_MUX_ADC_CH0_DDR()		DDRC &= ~(1<<PC0)
	#define H_MUX_ADC_CH1_DDR()		DDRC &= ~(1<<PC1)
	#define H_MUX_ADC_CH2_DDR()		DDRC &= ~(1<<PC2)

	#define H_MUX_ADC_DDR()		H_MUX_ADC_CH0_DDR(); H_MUX_ADC_CH1_DDR(); H_MUX_ADC_CH2_DDR()

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

