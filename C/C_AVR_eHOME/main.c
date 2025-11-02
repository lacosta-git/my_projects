/*
 * eHOME
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include <stdlib.h>
#include "main.h"
#include "rs232/rs232.h"
#include "adc/adc.h"

// Variables
volatile uint32_t led_loop_counetr = 0;
volatile uint16_t rs_counetr = 0;

// Functions
void init_ports(){
	LED_STRING_1_DDR();  // Set LED GPIB port as output
	H_MUX_DDR();
	H_MUX_ADC_DDR();
}

// Main
int main (void){
// Initialization
	// variables
	char string[20];

	// Ports
	init_ports();

	// ADC
	init_adc();

	// RS232
	init_serial_communication(9600);
	rs_send("\r>> Simple Serial Communication v1.0\r");

	// EEPROM

	// eHOME
	H_MUX_A0_OFF();
	H_MUX_A1_OFF();
	H_MUX_A2_OFF();

	// Interrupts

while(1){
	// rs_send_handler() - function must be in main loop
	// Function is non blocking.
	rs_send_handler();


	// ********************************* START DEMO *******************************
	// LED blinking
	led_loop_counetr++;
	if (led_loop_counetr > 100000) {
		LED_STRING_1_TRG();
		led_loop_counetr = 0;

		// RS message preparation
		itoa(adc_read(), string, 10);
		rs_send("\r>> adc = ");
		rs_send((char*)&string);
	}
	// ********************************* END DEMO ******************************
}
}
// ********** INTERRUPT ********
