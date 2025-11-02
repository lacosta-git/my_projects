/*
 * LocalLibT0Settings.c
 *
 * Created: 2014-03-01 18:47:17
 *  Author: Tomasz Szkudlarek
 */ 

#include <avr/io.h>
#include "LocalLibT0Settings.h"

void t0_timer_init(void)
	{
		T0_PRESCLER_1024;
		T0_NormalOperation;
		TCNT0 = T0_20msValue;	//T0 initial value
		TIMSK0 = (1<<TOIE0);	//T0 overflow interrupt enabling
	}