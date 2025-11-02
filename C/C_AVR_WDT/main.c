/*
 * main.c
 *
 *  Created on: 12.03.2019
 *      Author: lapiUser
 */

#include "main.h"
#include <util/delay.h>
#include <avr/wdt.h>

volatile uint8_t delay = 1;

// Functions
void _delay_ms_var(uint16_t a)
{
  while(a--)
  {
    _delay_ms(100);
    if (READ_DATA_1_PIN() == TRUE) wdt_reset();
  }
}


void wdt_set(void)
{
	wdt_reset();
	wdt_enable(WDTO_120MS);
}


void wdt_off(void)
{
	MCUSR = 0;
	wdt_disable();
}


// Main loop
int main(void)
{
	// WDT OFF
	wdt_off();
	wdt_set();

	// Ports setup
	LED_STRING_1_DDR();
	DATA_1_DDR();

  while(1)
  {
    LED_STRING_1_OFF();
    _delay_ms_var(delay);
    LED_STRING_1_ON();
    _delay_ms_var(delay);

    delay++;
    if (delay >= 30) delay = 1;
  }
}
