/*
 * interrupt.c
 *
 *  Created on: 30.03.2018
 *      Author: lapiUser
 */

#include "main.h"
#include "interrupt.h"
#include "servo.h"

volatile uint16_t counter_1 = 0;

void _T1Reload(uint16_t reload)
{
	//reload timer
	#ifdef ATMEGA328P
		TCNT1 = reload;
	#endif
}

void int_initT1(void)
{
	// 2,5 ms overflow time
	// CPU = 16 MHz
	// prescaler = 1
	// 65535 - 40000 = 25535 => 0x63BF
	// Normal PWM mode 0

	#ifdef ATMEGA328P
		TCCR1A = 0;
		TCCR1B = (1<<CS10);
		TCNT1 = RELOAD;

		//T1 Interrupt Enable
		TIMSK1 = _BV(TOIE1);
	#endif
}

// Interrupt handler
void int_T1_interrupt_handler(void){
	// T1 Reload
	srv_handler();
	_T1Reload(srv_next_reload());
}
