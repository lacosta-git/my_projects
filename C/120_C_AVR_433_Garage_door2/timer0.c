/*
 * timer0.c
 *
 * Created: 2014-12-31 18:24:38
 *  Author: TSzkudl
 */ 
#include <avr/io.h>
#include "timer0.h"

void t0_init(void)
{
	// CTC Mode - Clear Timer on Compare match
	// Settings for ATMEGA328p
	TCCR0A |= (1<<COM0A0)|(1<<WGM01); 	// Toggle OC0A on Compare Match, CTC Mode
	TCCR0B |= (1<<CS01);				// Clock source Clk_io/8
	OCR0A = 99;							// Value to be compared with. Default (99 + 1) => 100 us period

	// Enabling interrupt
	TIMSK0 |= (1<<OCIE0A);				// Interrupt when compare match
}
