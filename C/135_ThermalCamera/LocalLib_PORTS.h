/*
 * LocalLib_PORTS.h
 *
 * Created: 2014-02-26 12:21:36
 *  Author: Tomasz Szkudlarek
 */ 


/* Port/Pin settings */
/* Port B */
#define PB0_OUT		DDRB |= (1<<PB0)
#define PB0_IN		DDRB &=~(1<<PB0)
#define PB0_HiZ		DDRB &=~(1<<PB0); PORTB |=(1<<PB0)

#define PB1_OUT		DDRB |= (1<<PB1)
#define PB1_IN		DDRB &=~(1<<PB1)
#define PB1_HiZ		DDRB &=~(1<<PB1); PORTB |=(1<<PB1)

#define PB2_OUT		DDRB |= (1<<PB2)
#define PB2_IN		DDRB &=~(1<<PB2)
#define PB2_HiZ		DDRB &=~(1<<PB2); PORTB |=(1<<PB2)

#define PB3_OUT		DDRB |= (1<<PB3)
#define PB3_IN		DDRB &=~(1<<PB3)
#define PB3_HiZ		DDRB &=~(1<<PB3); PORTB |=(1<<PB3)

#define PB4_OUT		DDRB |= (1<<PB4)
#define PB4_IN		DDRB &=~(1<<PB4)
#define PB4_HiZ		DDRB &=~(1<<PB4); PORTB |=(1<<PB4)

#define PB5_OUT		DDRB |= (1<<PB5)
#define PB5_IN		DDRB &=~(1<<PB5)
#define PB5_HiZ		DDRB &=~(1<<PB5); PORTB |=(1<<PB5)

#define PB_OUT		PB0_OUT; PB1_OUT; PB2_OUT; PB3_OUT; PB4_OUT; PB5_OUT
#define PB_IN		PB0_IN; PB1_IN; PB2_IN; PB3_IN; PB4_IN; PB5_IN
#define PB_HiZ		PB0_HiZ; PB1_HiZ; PB2_HiZ; PB3_HiZ; PB4_HiZ; PB5_HiZ

/* Port C */
#define PC0_OUT		DDRC |= (1<<PC0)
#define PC0_IN		DDRC &=~(1<<PC0)
#define PC0_HiZ		DDRC &=~(1<<PC0); PORTC |=(1<<PC0)

#define PC1_OUT		DDRC |= (1<<PC1)
#define PC1_IN		DDRC &=~(1<<PC1)
#define PC1_HiZ		DDRC &=~(1<<PC1); PORTC |=(1<<PC1)

#define PC2_OUT		DDRC |= (1<<PC2)
#define PC2_IN		DDRC &=~(1<<PC2)
#define PC2_HiZ		DDRC &=~(1<<PC2); PORTC |=(1<<PC2)

#define PC3_OUT		DDRC |= (1<<PC3)
#define PC3_IN		DDRC &=~(1<<PC3)
#define PC3_HiZ		DDRC &=~(1<<PC3); PORTC |=(1<<PC3)

#define PC4_OUT		DDRC |= (1<<PC4)
#define PC4_IN		DDRC &=~(1<<PC4)
#define PC4_HiZ		DDRC &=~(1<<PC4); PORTC |=(1<<PC4)

#define PC5_OUT		DDRC |= (1<<PC5)
#define PC5_IN		DDRC &=~(1<<PC5)
#define PC5_HiZ		DDRC &=~(1<<PC5); PORTC |=(1<<PC5)

#define PC_OUT		PC0_OUT; PC1_OUT; PC2_OUT; PC3_OUT; PC4_OUT; PC5_OUT
#define PC_IN		PC0_IN; PC1_IN; PC2_IN; PC3_IN; PC4_IN; PC5_IN
#define PC_HiZ		PC0_HiZ; PC1_HiZ; PC2_HiZ; PC3_HiZ; PC4_HiZ; PC5_HiZ

/* Port D */
#define PD0_OUT		DDRD |= (1<<PD0)
#define PD0_IN		DDRD &=~(1<<PD0)
#define PD0_HiZ		DDRD &=~(1<<PD0); PORTD |=(1<<PD0)

#define PD1_OUT		DDRD |= (1<<PD1)
#define PD1_IN		DDRD &=~(1<<PD1)
#define PD1_HiZ		DDRD &=~(1<<PD1); PORTD |=(1<<PD1)

#define PD2_OUT		DDRD |= (1<<PD2)
#define PD2_IN		DDRD &=~(1<<PD2)
#define PD2_HiZ		DDRD &=~(1<<PD2); PORTD |=(1<<PD2)

#define PD3_OUT		DDRD |= (1<<PD3)
#define PD3_IN		DDRD &=~(1<<PD3)
#define PD3_HiZ		DDRD &=~(1<<PD3); PORTD |=(1<<PD3)

#define PD4_OUT		DDRD |= (1<<PD4)
#define PD4_IN		DDRD &=~(1<<PD4)
#define PD4_HiZ		DDRD &=~(1<<PD4); PORTD |=(1<<PD4)

#define PD5_OUT		DDRD |= (1<<PD5)
#define PD5_IN		DDRD &=~(1<<PD5)
#define PD5_HiZ		DDRD &=~(1<<PD5); PORTD |=(1<<PD5)

#define PD6_OUT		DDRD |= (1<<PD6)
#define PD6_IN		DDRD &=~(1<<PD6)
#define PD6_HiZ		DDRD &=~(1<<PD6); PORTD |=(1<<PD6)

#define PD7_OUT		DDRD |= (1<<PD7)
#define PD7_IN		DDRD &=~(1<<PD7)
#define PD7_HiZ		DDRD &=~(1<<PD7); PORTD |=(1<<PD7)

#define PD_OUT		PD0_OUT; PD1_OUT; PD2_OUT; PD3_OUT; PD4_OUT; PD5_OUT; PD6_OUT; PD7_OUT
#define PD_IN		PD0_IN; PD1_IN; PD2_IN; PD3_IN; PD4_IN; PD5_IN; PD6_IN; PD7_IN
#define PD_HiZ		PD0_HiZ; PD1_HiZ; PD2_HiZ; PD3_HiZ; PD4_HiZ; PD5_HiZ; PD6_HiZ; PD7_HiZ


/* Pins definition */

#define YelowLED_ON		PORTD &=~(1<<PD7)
#define YelowLED_OFF	PORTD |=(1<<PD7)
#define YelowLED_TOGLE	PORTD ^=(1<<PD7)

#define SERVO_PWR_ON	PORTD &=~(1<<PD4)
#define SERVO_PWR_OFF	PORTD |=(1<<PD4)
#define SERVO_PWR_TOGLE	PORTD ^=(1<<PD4)

#define GreenLED_ON		PORTB &=~(1<<PB1)
#define GreenLED_OFF	PORTB |=(1<<PB1)
#define GreenLED_TOGLE	PORTB ^=(1<<PB1)

#ifndef LOCALLIB_PORTS_H_
#define LOCALLIB_PORTS_H_

void ports_init(void);

#endif /* LOCALLIB_PORTS_H_ */