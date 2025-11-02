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

#define BT_PRESSED 	0


// PORT B
// Inputs
// None

// Outputs
#define STEP_M3_PIN			PB0  // STEP M3
#define STEP_M1_PIN			PB1  // STEP M1
#define DIAG_LED_PIN		PB5
#define SET_OUT_PORT_B		DDRB |= (1<<STEP_M1_PIN)|(1<<STEP_M3_PIN)|(1<<DIAG_LED_PIN)

#define	STEP_M3_ON()		PORTB |= (1<<STEP_M3_PIN)
#define	STEP_M3_OFF()		PORTB &= ~(1<<STEP_M3_PIN)
#define STEP_M3_TOG()		PORTB ^=(1<<STEP_M3_PIN)

#define	STEP_M1_ON()		PORTB |= (1<<STEP_M1_PIN)
#define	STEP_M1_OFF()		PORTB &= ~(1<<STEP_M1_PIN)
#define STEP_M1_TOG()		PORTB ^=(1<<STEP_M1_PIN)

#define	DIAG_LED_ON()		PORTB |= (1<<DIAG_LED_PIN)
#define	DIAG_LED_OFF()		PORTB &= ~(1<<DIAG_LED_PIN)
#define DIAG_LED_TOG()		PORTB ^=(1<<DIAG_LED_PIN)


// Port C
// Inputs
// None

// Outputs
#define MS3_PIN				PC0
#define MS2_PIN				PC1
#define MS1_PIN				PC2
#define EN_M3_PIN			PC3
#define EN_M2_PIN			PC4
#define DIR_PIN				PC5
#define SET_OUT_PORT_C		DDRC |= (1<<MS3_PIN)|(1<<MS2_PIN)|(1<<MS1_PIN)|(1<<EN_M3_PIN)|(1<<EN_M2_PIN)|(1<<DIR_PIN)

#define	MS3_ON()			PORTC |= (1<<MS3_PIN)
#define	MS3_OFF()			PORTC &= ~(1<<MS3_PIN)

#define	MS2_ON()			PORTC |= (1<<MS2_PIN)
#define	MS2_OFF()			PORTC &= ~(1<<MS2_PIN)

#define	MS1_ON()			PORTC |= (1<<MS1_PIN)
#define	MS1_OFF()			PORTC &= ~(1<<MS1_PIN)

#define	EN_M3_OFF()			PORTC |= (1<<EN_M3_PIN)
#define	EN_M3_ON()			PORTC &= ~(1<<EN_M3_PIN)

#define	EN_M2_OFF()			PORTC |= (1<<EN_M2_PIN)
#define	EN_M2_ON()			PORTC &= ~(1<<EN_M2_PIN)

#define	DIR_HI()			PORTC |= (1<<DIR_PIN)
#define	DIR_LO()			PORTC &= ~(1<<DIR_PIN)

// Port D
// Inputs
#define M_DIR_PIN			PD4
#define M_STEP_M2_PIN		PD5
#define M_STEP_M3_PIN		PD6
#define M_STEP_M1_PIN		PD7

#define READ_M_DIR_PIN()		(PIND&(1<<M_DIR_PIN))>>M_DIR_PIN
#define READ_M_STEP_M1_PIN()	(PIND&(1<<M_STEP_M1_PIN))>>M_STEP_M1_PIN
#define READ_M_STEP_M2_PIN()	(PIND&(1<<M_STEP_M2_PIN))>>M_STEP_M2_PIN
#define READ_M_STEP_M3_PIN()	(PIND&(1<<M_STEP_M3_PIN))>>M_STEP_M3_PIN

// Outputs
#define EN_M1_PIN			PD2
#define STEP_M2_PIN			PD3
#define SET_OUT_PORT_D		DDRD |= (1<<EN_M1_PIN)|(1<<STEP_M2_PIN)

#define	EN_M1_OFF()			PORTD |= (1<<EN_M1_PIN)
#define	EN_M1_ON()			PORTD &= ~(1<<EN_M1_PIN)

#define	STEP_M2_ON()		PORTD |= (1<<STEP_M2_PIN)
#define	STEP_M2_OFF()		PORTD &= ~(1<<STEP_M2_PIN)
#define STEP_M2_TOG()		PORTD ^=(1<<STEP_M2_PIN)


#define SET_OUTPUTS_DDR()   SET_OUT_PORT_B; SET_OUT_PORT_C; SET_OUT_PORT_D
