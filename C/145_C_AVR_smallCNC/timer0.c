/*
 * timer0.c
 *
 * Created: 2014-12-31 18:24:38
 *  Author: TSzkudl
 */ 
#include <avr/io.h>
#include "timer0.h"

void t0initialization(void)
{
	// 100 us overflow time
	// CPU = 16 MHz
	// 16000000/10000 = 1600 [100 us]
	// 1600/8 = 200 -> prescaler = 8
	// 256 - 200 = 56 -> 0x38 - T0_RELOAD

	TCCR0A = 0x0;
	TCCR0B = (0<<WGM02)|(0<<CS02)|(1<<CS01)|(0<<CS00);
	t0reload();
	
	//T0 Interrupt Enable
	TIMSK0 = _BV(TOIE0);
}

void t0reload(void)
{
	//reload timer
	TCNT0 = T0_RELOAD;
}
