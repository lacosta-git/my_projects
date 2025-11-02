/*
 * main.c
 *
 *  Created on: 12.03.2019
 *      Author: lapiUser
 */

#include "main.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// Variables

// Main loop
int main(void)
{
	// Setup main CPU clock
	// 128 kHz internal oscillator

	// Ports setup
	SET_HiZ_FOR_PINS();
	SET_DDR_OUTPUTS();

	// Disable peripheries
	DISABLE_PERIPHERIES();

	// Variables


  while(1){
  // Main loop code here
	  OUTPUTS_ON();
	  _delay_us(200);
	  OUTPUTS_OFF();
	  _delay_us(500);
  }
}
