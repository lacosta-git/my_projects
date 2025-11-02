/*
 * Simple Serial communication library
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include <stdlib.h>
#include "main.h"
#include "rs232/rs232.h"

// Variables
volatile uint32_t led_loop_counetr = 0;
volatile uint16_t rs_counetr = 0;

// Functions
void init_ports(){
	LED_STRING_1_DDR();  // Set LED GPIB port as output
}

// Main
int main (void){
// Initialization
	// variables
	char string[20];

	// Ports
	init_ports();

	// RS232
	init_serial_communication(19200);
	rs_send("\r>> Simple Serial Communication v2.0\r");

	// EEPROM

	// Interrupts

while(1){
	// rs_send_handler() - function must be in main loop
	// Function is non blocking.
	rs_send_handler();
	rs_get_handler();


	// ********************************* START DEMO *******************************
	// LED blinking
	led_loop_counetr++;
	if (led_loop_counetr > 100000) {
		LED_STRING_1_TRG();
		led_loop_counetr = 0;

		// RS message preparation
		itoa(rs_counetr, string, 10);
		rs_send("\r>> rs_counter = ");
		rs_send((char*)&string);
		rs_counetr++;

		rs_get((char*)&string);
		rs_send((char*)&string);

	}
	// ********************************* END DEMO ******************************
}
}
// ********** INTERRUPT ********
