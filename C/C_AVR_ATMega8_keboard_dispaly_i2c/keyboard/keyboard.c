/*
 * keyboard.c
 *
 *  Created on: 26.01.2019
 *      Author: lapiUser
 */

#include "keyboard.h"

volatile uint8_t keyboard_state_machine;
volatile uint8_t detected_rows;
volatile uint8_t detected_cols;
volatile uint8_t output_seter;
volatile uint8_t wait_counter;
volatile uint8_t read_couner;
volatile uint8_t inputs[KEY_INPUTS_COUNT];

void keyboard_set_output(uint8_t output){
	COL1_ON();
	COL2_ON();
	COL3_ON();
	COL4_ON();

	if (output == 0x01) COL1_OFF();
	if (output == 0x02) COL2_OFF();
	if (output == 0x04) COL3_OFF();
	if (output == 0x08) COL4_OFF();
}

void keyboard_read_inputs(uint8_t read_no){

	if (read_no == 1){  // Clear old values
		for (uint8_t i=0; i<KEY_INPUTS_COUNT; i++){
			inputs[i] = 0;
		}
	}

	inputs[0] += READ_ROW1();
	inputs[1] += READ_ROW2();
	inputs[2] += READ_ROW3();
	inputs[3] += READ_ROW4();
	inputs[4] += READ_ROW5();

}

// Main State Machine
void keyboard_main(void){
	uint8_t output_to_set;
	switch (keyboard_state_machine){
		case 0:  // IDLE
			keyboard_state_machine++;
			break;

		case 1:  // set output
			output_to_set = (1 << output_seter);
			keyboard_set_output(output_to_set);
			wait_counter = 0;
			keyboard_state_machine++;
			break;

		case 2:  // wait for voltage stabilization
			wait_counter++;
			if (wait_counter > 10) {
				read_couner = 0;
				keyboard_state_machine++;
			}
			break;

		case 3:  // read inputs
			read_couner++;
			if (read_couner <= 3){
				keyboard_read_inputs(read_couner);
			} else {
				keyboard_state_machine++;
			}
			break;

		case 4:  // compute pressed keys
			detected_rows = 0;
			for (uint8_t i=0; i<KEY_INPUTS_COUNT; i++){
				if (inputs[i] <= 1) {
					detected_rows |= (1<<i);
				}
			}

			if (detected_rows > 0){
				detected_cols = (1 << output_seter);
			} else detected_cols = 0;

			keyboard_state_machine++;
			break;

		case 5:  // prepare enabling next output

			output_seter++;
			if (output_seter >= KEY_OUTPUTS_COUNT){
				output_seter = 0;
			}
			keyboard_state_machine++;
			break;

		default:
			keyboard_state_machine = 0;
			break;
	}
}


void init_keyboard(void){
	INIT_KEYBOARD_PORTS();
	detected_rows = 0;
	detected_cols = 0;
	output_seter = 0;
}

void keyboard_handler(void){
	keyboard_main();
}


// Function returns 2 bytes
// MSB = detected_rows, LSB = detected_columns
// one bit for one row or column
uint16_t keyboard_read_keys(void){
	uint16_t pressed_keys;
	pressed_keys = 0;
	pressed_keys = (detected_rows << 8);
	pressed_keys += detected_cols;
	return pressed_keys;
}
