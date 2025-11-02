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
	PC1_OUT;	//  HEATER 1
	PC2_OUT;	//  HEATER 2
	PC3_OUT;	//  HEATER 3
	PC4_OUT;	//  HEATER 4
	HEATER_1_OFF;
	HEATER_2_OFF;
	HEATER_3_OFF;
	HEATER_4_OFF;
	
	PB1_OUT;	//  RED_LED
	RED_LED_OFF;
	
	PD5_OUT;	// BUTTON_LED
	BUTTON_LED_OFF;

}