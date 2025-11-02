/*
 * timer1.c
 *
 *  Created on: 01.08.2020
 *      Author: lapiUser
 */

#include <avr/io.h>
#include "timer1.h"


void t1_init(void){
	// Normal operation mode
	// Timer works as a counter. It counts impulses on T1 input
	// Counter is cleared by application
	// Counting falling edge on T1 input
	TCCR1B |= (1<<CS12)|(1<<CS11);
}
