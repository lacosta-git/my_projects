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
	// 10 ms overflow time
	// CPU = 16 MHz
	// 16000000/100 = 1600000 [10 ms]
	// 160000/1024 = 156 -> prescaler = 1024
	// 256 - 156 = 100 -> 0x64 - T0_RELOAD

	TCCR0A = 0x0;
	TCCR0B = (0<<WGM02)|(1<<CS02)|(0<<CS01)|(1<<CS00);
	t0reload();
	
	//T0 Interrupt Enable
	TIMSK0 = _BV(TOIE0);
}

void t0reload(void)
{
	//reload timer
	TCNT0 = T0_RELOAD;
}