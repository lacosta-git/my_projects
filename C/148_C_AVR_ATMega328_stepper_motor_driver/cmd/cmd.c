/*
 * cmd.c
 *
 *  Created on: 09.09.2020
 *      Author: lapiUser
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cmd.h"
#include "../main.h"
#include "../rs232/rs232.h"
#include "../stepper_motor_drv/stepper.h"

// Global Variables
// RS
char string[20];
char c_code[0x0A];
uint16_t c_temp_val;

// CMD
volatile unsigned char c_input_buf[C_INPUT_BUF_SIZE];
volatile unsigned char c_cmd[C_CMD_MAX_SIZE];
volatile uint8_t c_cmd_is_executed = FALSE;
volatile uint8_t c_remove_cmd = FALSE;
volatile uint8_t c_parse_input_f = FALSE;

// Timer
volatile uint16_t c_timer_div = 0;

// TEXTS
const __flash char c_p_nq[] = "\n? ";
const __flash char c_p_ns[] = "\n> ";
const __flash char c_p_description_of_m_param[] = "\nm:M:D:STEPS; move motor [1-3] in/out [0/1] by STEPS [0-32000]";
const __flash char c_p_error[] = "\n> Syntax error";
const __flash char c_p_ok[] = "\n> OK";
const __flash char c_p_err_value_out_of_range[] = "\n> Err: Value is out of range\n";
const __flash char c_p_dummy[] = "\n> DUMMY Text";
const __flash char c_p_move_motor[] = "\n> Move motor";
const __flash char c_p_m1[] = " M1";
const __flash char c_p_m2[] = " M2";
const __flash char c_p_m3[] = " M3";
const __flash char c_p_in[] = " IN";
const __flash char c_p_out[] = " OUT";
const __flash char c_p_by[] = " by ";
const __flash char c_p_steps_txt[] = " steps";

void c_read_rs_buffer(void){
	unsigned char c_rs_buf[RS_SHORT_RX_BUFFER_SIZE];
	uint16_t c_input_index = 0;
	char* c_rs_buf_ptr = (char*)&c_rs_buf[0];

	for (uint8_t i=0; i<RS_SHORT_RX_BUFFER_SIZE;i++){
		c_rs_buf[i] = 0;
	}

	rs_get((char*)c_rs_buf);  // Reading RS buffer

	// Updating input buffer
	// Finding last char in input buffer
	while(c_input_buf[c_input_index]!=0){
		c_input_index++;
		if (c_input_index >= (C_INPUT_BUF_SIZE - RS_SHORT_RX_BUFFER_SIZE)) break;
	}

	// Adding rs buffer chars at the end of input data
	while(*c_rs_buf_ptr){
		c_input_buf[c_input_index] = *c_rs_buf_ptr;
		c_rs_buf_ptr++;
		if (c_input_index < C_INPUT_BUF_SIZE){
			c_input_index++;
		}
	}
}


void c_parse_input(void){
	uint16_t c_input_index = 0;

	c_parse_input_f = FALSE;
	if (c_cmd_is_executed == FALSE){
		while(c_input_buf[c_input_index]!=0){
			if (c_input_buf[c_input_index] == ';'){

				// Extract single CMD
				for (uint8_t k=0; k<=c_input_index; k++){
					c_cmd[k] = c_input_buf[k];
				}
				c_exe_cmd();
				break;
			} else {
				c_input_index++;
			}
		}
	}


	// Remove executed command
	if (c_remove_cmd == TRUE){
		c_input_index++;  // Move after separation char
		for (uint16_t i=0; i<(C_INPUT_BUF_SIZE-c_input_index); i++){
			c_input_buf[i] = c_input_buf[c_input_index+i];
			c_input_buf[c_input_index+i] = 0;
		}
		c_remove_cmd = FALSE;
	}
}


void c_clear_code(void){
	uint8_t i=0;
	while (c_code[i]){
		c_code[i] = 0;
		if (i<0x0A)	i++;
		else i = 0;
	}
}


// Function return integer value from ASCII string.
// The pointer must point to first digit in string.
int c_extract_val(char* c_str){
	uint8_t C_VAL_LEN = 8;
	char c_cmd_val_str[C_VAL_LEN];
	char* c_cmd_val_str_ptr = (char*)&c_cmd_val_str;

	// Clear old values
	for (uint8_t i=0; i<C_VAL_LEN; i++){
		c_cmd_val_str[i] = 0;
	}

	// Copy ASCII digits to temp string
	while (isdigit(*c_str)){
		*c_cmd_val_str_ptr = *c_str;
		c_cmd_val_str_ptr++;
		c_str++;
	}

	// Convert ASCII to integer and return
	return atoi(c_cmd_val_str);
}


// Function return digits in text from ASCII string.
// The pointer must point to first digit in string.
void c_extract_digits_txt(char* c_str, char* c_cmd_val_str){
	uint8_t c_loop_counter = 0;
	// Copy ASCII digits to temp string
	while ((isdigit(*c_str))&&
		   (c_loop_counter < 0x40)){
		*c_cmd_val_str = *c_str;
		c_cmd_val_str++;
		c_str++;
		c_loop_counter++;
	}
}


// execute cmd
void c_exe_cmd(void){
	uint8_t no_error = TRUE;
	uint8_t c_p_motor = 0;
	uint8_t c_p_dir = 0;
	uint16_t c_p_steps = 0;

	c_cmd_is_executed = TRUE;
	rs_send_P(c_p_nq);  // New line ?
	rs_send((char*)&c_cmd);

	// List of commands
	if (strncmp((char*)&c_cmd[0], "?", 1 ) == 0){
		rs_send_P(c_p_description_of_m_param);
	}

	// m: - move stepper motor
	else if (strncmp((char*)&c_cmd[0], "m:", 2) == 0){
		rs_send_P(c_p_move_motor);

		if (strncmp((char*)&c_cmd[2], "1:", 2) == 0){
			rs_send_P(c_p_m1);
			c_p_motor = 1;
		}
		else if (strncmp((char*)&c_cmd[2], "2:", 2) == 0){
			rs_send_P(c_p_m2);
			c_p_motor = 2;
		}
		else if (strncmp((char*)&c_cmd[2], "3:", 2) == 0){
			rs_send_P(c_p_m3);
			c_p_motor = 3;
		}
		else {
			rs_send_P(c_p_err_value_out_of_range);
			no_error = FALSE;
		}

		if (strncmp((char*)&c_cmd[4], "0:", 2) == 0){
			rs_send_P(c_p_in);
			c_p_dir = ST_DIR_IN;
		} else if (strncmp((char*)&c_cmd[4], "1:", 2) == 0){
			rs_send_P(c_p_out);
			c_p_dir = ST_DIR_OUT;
		} else {
			rs_send_P(c_p_err_value_out_of_range);
			no_error = FALSE;
		}

		rs_send_P(c_p_by);

		c_clear_code();

		c_extract_digits_txt((char*)&c_cmd[6], (char*)&c_code);
		c_p_steps = atoi((char*)&c_code);
		if (c_p_steps > 32000){
			rs_send_P(c_p_err_value_out_of_range);
			no_error = FALSE;
		} else {
			rs_send((char*)&c_code);
		}

		rs_send_P(c_p_steps_txt);

		if (no_error == TRUE){	//  Execute movement
			stepper_move(c_p_motor, c_p_dir, c_p_steps);
		}
	}

	else {
		rs_send_P(c_p_error);
	}

	// Clear CMD text in exe buffer
	for (uint8_t i=0; i<C_CMD_MAX_SIZE; i++){
		c_cmd[i] = 0;
	}
	c_remove_cmd = TRUE;
}


void c_cmd_tick_ms(void){
	if (c_timer_div > 100){  // 100 ms
		c_timer_div = 0;

		// Parse input buffer every 100 ms
		c_parse_input_f = TRUE;
		c_cmd_is_executed = FALSE;
	} else {
		c_timer_div++;
	}

}


void c_cmd_handler(void){
	c_read_rs_buffer();
	if (c_parse_input_f == TRUE){
		c_parse_input();
	}
}
