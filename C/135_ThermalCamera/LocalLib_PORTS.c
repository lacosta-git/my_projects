/*
 * LocalLib_PORTS.c
 *
 * Created: 2014-02-26 12:25:13
 *  Author: Tomasz Szkudlarek
 */ 

#include <avr/io.h>
#include "LocalLib_PORTS.h"

void ports_init(void)
{
	PB_IN;
	PC_IN;
	PD_IN;
	
	//Pins direction
	PB1_OUT;	//OC1A
	PB2_OUT;	//OC1B
	PD4_OUT;	//Servo PWR
	SERVO_PWR_OFF;
	PD7_OUT;	//Yellow LED
	YelowLED_OFF;
}