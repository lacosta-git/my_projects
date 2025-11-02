/*
 * main.c
 *
 *  Created on: 30.12.2019
 *      Author: lapiUser
 */

#include <avr/io.h>
#include "main.h"
#include "interrupt.h"
#include "servo.h"


#define SRV_Y_LED 	0


volatile uint8_t y_led_angle;

// **********************************************
// **********       MAIN LOOP            ********
// **********************************************
int main(){
	// Port init
	LED_STRING_1_DDR();

	srv_attach((void*)&PORTB, PB5, SRV_Y_LED);
	// Interrupts
	int_initT1();
	sei();

while(TRUE){
	//LED_ON();

}}
// **********************************************
// **********************************************

// ********** INTERRUPT ********
ISR(INT_T1){
	int_T1_interrupt_handler();
	srv_set_angle(SRV_Y_LED, y_led_angle);
	y_led_angle++;
	if (y_led_angle == 181) y_led_angle = 0;
}
