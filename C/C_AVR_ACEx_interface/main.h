/*
 * main.h
 *
 *  Created on: 15.09.2017
 *      Author: autoMaster
 */

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  16000000
#endif  // F_CPU

#include <avr/io.h>

#define FALSE 0
#define TRUE 1

	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)


#define		ACE_SHIFT_IN_DDR()			DDRC |= (1<<PC0)
#define		ACE_SHIFT_IN_HI()			PORTC |= (1<<PC0)
#define		ACE_SHIFT_IN_LO()			PORTC &= ~(1<<PC0)

#define		ACE_CLK_DDR()				DDRC |= (1<<PC2)
#define		ACE_CLK_HI()				PORTC |= (1<<PC2)
#define		ACE_CLK_LO()				PORTC &= ~(1<<PC2)

#define		ACE_LOAD_0V_DDR()			DDRC |= (1<<PC3)
#define		ACE_LOAD_0V_HI()			PORTC |= (1<<PC3)
#define		ACE_LOAD_0V_LO()			PORTC &= ~(1<<PC3)

#define		ACE_LOAD_12V_DDR()			DDRC |= (1<<PC4)
#define		ACE_LOAD_12V_HI()			PORTC |= (1<<PC4)
#define		ACE_LOAD_12V_LO()			PORTC &= ~(1<<PC4)

// Debug
#define		DBG_SHIFT_IN_DDR()			DDRD |= (1<<PD7)
#define		DBG_SHIFT_IN_HI()			PORTD |= (1<<PD7)
#define		DBG_SHIFT_IN_LO()			PORTD &= ~(1<<PD7)

#define		DBG_CLK_DDR()				DDRD |= (1<<PD6)
#define		DBG_CLK_HI()				PORTD |= (1<<PD6)
#define		DBG_CLK_LO()				PORTD &= ~(1<<PD6)

#define		DBG_LOAD_0V_DDR()			DDRD |= (1<<PD5)
#define		DBG_LOAD_0V_HI()			PORTD |= (1<<PD5)
#define		DBG_LOAD_0V_LO()			PORTD &= ~(1<<PD5)

#define		DBG_LOAD_12V_DDR()			DDRD |= (1<<PD4)
#define		DBG_LOAD_12V_HI()			PORTD |= (1<<PD4)
#define		DBG_LOAD_12V_LO()			PORTD &= ~(1<<PD4)

// INPUTS
#define DATA_IN_PIN			PC1
#define DATA_IN_PORT		PINC
#define READ_DATA_PIN()		(DATA_IN_PORT&(1<<DATA_IN_PIN))>>DATA_IN_PIN
