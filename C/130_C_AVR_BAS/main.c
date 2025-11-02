/*
 * Basic AVR System - BAS
 *
 *  Created on: 31.05.2019
 *      Author: Tomasz Szkudlarek
 */

#include <stdlib.h>
#include "main.h"
#include "interfaces\ifc.h"
#include "cm\cm.h"

// Variables
volatile uint8_t m_ret;
volatile uint8_t m_in_char;
volatile char pr_program[PR_RAM_PROGRAM_SIZE];


// Functions

// Main
int main (void){
// Initialization
	// variables
	uint32_t led_loop_counetr = 0;

	// Init interfaces;
	ifc_init();
	ifc_print("\r\n> BASIC AVR SYSTEM\r\n");
	ifc_print("> Version 0.1\r\n> ");

while(1){
	// ifc_handler() - function must be in main loop
	// Function is non blocking.
	ifc_handler();

	m_ret = ifc_read_received_data((char*)&m_in_char);
	if (m_ret == TRUE) {
		cm_cmd_builder(m_in_char);
	}

	m_ret = cm_is_new_cmd_ready();
	if (m_ret == TRUE){
		cm_excute_cmd();
	}
	// ************************* START MAIN LOOP *******************************
	// LED blinking
	led_loop_counetr++;
	if (led_loop_counetr > 100000) {
		//ifc_LED_TRG();
		led_loop_counetr = 0;
	}
	// **************************** END MAIN LOOP ******************************
}
}
