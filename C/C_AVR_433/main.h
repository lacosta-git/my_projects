/*
 * main.h
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  16000000
#endif  // F_CPU

#include <avr/io.h>

#define FALSE 0
#define TRUE 1

// OUTPUTS
#define PB_GND_PIN		PB3
#define	SET_GND_DDR()	DDRB |= (1<<PB_GND_PIN)
#define GND_OFF() 		PORTB |= (1<<PB_GND_PIN)
#define GND_ON()		PORTB &= ~(1<<PB_GND_PIN)

#define PB_PWR_PIN		PB0
#define	SET_PWR_DDR()	DDRB |= (1<<PB_PWR_PIN)
#define PWR_ON() 		PORTB |= (1<<PB_PWR_PIN)
#define PWR_OFF()		PORTB &= ~(1<<PB_PWR_PIN)

#define	DDR_B()			DDRB |= (1<<PB5)
#define	LED_STRING_1_ON()		PORTB |= (1<<PB5)
#define	LED_STRING_1_OFF()		PORTB &= ~(1<<PB5)
#define LED_STRING_1_TRG()		PORTB ^=(1<<PB5)

#define SET_OUTPUTS_DDR()	SET_GND_DDR(); SET_PWR_DDR(); DDR_B()

// INPUTS
#define DATA_1_PIN			PB1
#define DATA_1_DDR()		DDRB &= ~(1<<DATA_1_PIN); PORTB |=(1<<DATA_1_PIN)  // INPUT with pull up
#define READ_DATA_1_PIN()	(PINB&(1<<DATA_1_PIN))>>DATA_1_PIN
#define READ_DATA_PIN() 	READ_DATA_1_PIN()

#define DATA_2_PIN			PB2
#define DATA_2_DDR()		DDRB &= ~(1<<DATA_2_PIN); PORTB |=(1<<DATA_2_PIN)  // INPUT with pull up
#define READ_DATA_2_PIN()	(PINB&(1<<DATA_2_PIN))>>DATA_2_PIN

#define SET_INPUTS_DDR()	DATA_1_DDR(); DATA_2_DDR()
