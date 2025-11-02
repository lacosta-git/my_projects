/*
 * main.h
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */


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

#define NO 0
#define YES 1

#define OPEN 0
#define CLOSED 1

// OUTPUTS
#define DATA_GATE_PIN		PD6
#define	DATA_GATE_DDR()		DDRD |= (1<<DATA_GATE_PIN)
#define DATA_GATE_ON() 		PORTD |= (1<<DATA_GATE_PIN)
#define DATA_GATE_OFF()		PORTD &= ~(1<<DATA_GATE_PIN)

#define	DDR_B()			DDRB |= (1<<PB5)
#define	LED_STRING_1_ON()		PORTB |= (1<<PB5)
#define	LED_STRING_1_OFF()		PORTB &= ~(1<<PB5)
#define LED_STRING_1_TRG()		PORTB ^=(1<<PB5)

// Port C
#define DDR_C0			DDRC |= (1<<PC0)
#define DDR_C1			DDRC |= (1<<PC1)
#define DDR_C2			DDRC |= (1<<PC2)
#define DDR_C3			DDRC |= (1<<PC3)
#define DDR_C4			DDRC |= (1<<PC4)
#define DDR_C5			DDRC |= (1<<PC5)
#define DDR_PC_OUTPUT()	DDR_C5; DDR_C4; DDR_C3; DDR_C2

#define O_OPEN_OFF()	PORTC |= (1<<PC5)
#define O_OPEN_ON()		PORTC &= ~(1<<PC5)
#define O_CLOSE_OFF()	PORTC |= (1<<PC4)
#define O_CLOSE_ON()	PORTC &= ~(1<<PC4)
#define O_BEEP_ON()		PORTC |= (1<<PC3)
#define O_BEEP_OFF()	PORTC &= ~(1<<PC3)
#define O_LIGHT_ON()	PORTC |= (1<<PC2)
#define O_LIGHT_OFF()	PORTC &= ~(1<<PC2)
#define O_LIGHT_TRG()	PORTC ^= (1<<PC2)


#define SET_OUTPUTS_DDR()	DATA_GATE_DDR(); DDR_B(); DDR_PC_OUTPUT()

// INPUTS
#define SW_UP_PIN			PB0
#define SW_UP_DDR()			DDRB &= ~(1<<SW_UP_PIN); PORTB |=(1<<SW_UP_PIN)  // INPUT with pull up
#define READ_SW_UP_PIN()	(PINB&(1<<SW_UP_PIN))>>SW_UP_PIN

#define SW_DOWN_PIN			PB1
#define SW_DOWN_DDR()		DDRB &= ~(1<<SW_DOWN_PIN); PORTB |=(1<<SW_DOWN_PIN)  // INPUT with pull up
#define READ_SW_DOWN_PIN()	(PINB&(1<<SW_DOWN_PIN))>>SW_DOWN_PIN

#define SW_REMOTE_PIN		PB2
#define SW_REMOTE_DDR()		DDRB &= ~(1<<SW_DOWN_PIN); PORTB |=(1<<SW_DOWN_PIN)  // INPUT with pull up
#define READ_SW_REMOTE_PIN()	(PINB&(1<<SW_DOWN_PIN))>>SW_DOWN_PIN

#define DATA_1_PIN			PD2
#define DATA_1_DDR()		DDRD &= ~(1<<DATA_1_PIN); PORTD |=(1<<DATA_1_PIN)  // INPUT with pull up
#define READ_DATA_1_PIN()	(PIND&(1<<DATA_1_PIN))>>DATA_1_PIN
#define READ_DATA_PIN() 	READ_DATA_1_PIN()

#define GATE_PIN			PD3
#define GATE_DDR()			DDRD &= ~(1<<GATE_PIN)  // INPUT
#define READ_GATE_PIN()		(PIND&(1<<GATE_PIN))>>GATE_PIN

#define OBSTACLE_PIN		PD7
#define OBSTACLE_DDR()		DDRD &= ~(1<<OBSTACLE_PIN); PORTD |=(1<<OBSTACLE_PIN)  // INPUT with pull up
#define READ_OBSTACLE_PIN()	(PIND&(1<<OBSTACLE_PIN))>>OBSTACLE_PIN

#define SET_INPUTS_DDR()	DATA_1_DDR(); GATE_DDR(); OBSTACLE_DDR(); SW_UP_DDR(); SW_DOWN_DDR(); SW_REMOTE_DDR()

void read_inputs(void);
void set_outputs(void);
void ms_timer_handler(void);
void m_reset_cpu(void);

