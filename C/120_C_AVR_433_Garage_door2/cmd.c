/*
 * cmd.c
 *
 *  Created on: 09.09.2020
 *      Author: lapiUser
 */

#include <ctype.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/interrupt.h>
#include "cmd.h"
#include "rs232/rs232.h"
#include "garage/garage.h"
#include "my_eeprom.h"
#include "main.h"
#include "radio_decode.h"

// RS
#include <stdlib.h>
char string[20];
char c_code[0x40];
uint16_t c_temp_val;

// CMD
volatile unsigned char c_input_buf[C_INPUT_BUF_SIZE];
volatile unsigned char c_cmd[C_CMD_MAX_SIZE];
volatile uint8_t c_cmd_is_executed = FALSE;
volatile uint8_t c_remove_cmd = FALSE;
volatile uint8_t c_parse_input_f = FALSE;

// Timer
volatile uint16_t c_timer_div = 0;
volatile uint16_t c_timer_div2 = 0;


// TEXTS
const __flash char c_p_nq[] = "\n? ";
const __flash char c_p_ns[] = "\n> ";
const __flash char c_p_sec[] = " [s]";
const __flash char c_p_ms[] = " [ms]";
const __flash char c_p_read_x_param[] = "\nr:x;   read x param";
const __flash char c_p_write_y_value_to_x_param[] = "\nw:x:y; write y value to x param";
const __flash char c_p_list_of_params[] = "\np?;    list of params";
const __flash char c_p_description_of_x_param[] = "\np:x;   description of x param";
const __flash char c_p_dot_mct_mot[] = "\n:dot:mct:mot:wt:wbont:wbofft:wlont:wlofft";
const __flash char c_p_wed_wdobont_wdobofft[] = "\n:wed:tbc:wdobont:wdobofft:wdolont:wdolofft";
const __flash char c_p_key_code_r_w[] ="\n:kno:knc:reset";
const __flash char c_p_door_open_time[] = "\n> dot: Door open time [s] before start closing";
const __flash char c_p_max_closing_time[] = "\n> mct: Max closing time [ms]";
const __flash char c_p_max_opening_time[] = "\n> mot: Max opening time [ms]";
const __flash char c_p_warning_time[] = "\n> wt: Warning Time [ms] before next action";
const __flash char c_p_warning_beep_on_time[] = "\n> wbont: Warning Beep ON Time [ms]";
const __flash char c_p_warning_beep_off_time[] = "\n> wbofft: Warning Beep OFF Time [ms]";
const __flash char c_p_warning_light_on_time[] = "\n> wlont: Warning Light ON Time [ms]";
const __flash char c_p_warning_light_off_time[] = "\n> wlofft: Warning Light OFF Time [ms]";
const __flash char c_p_warnings_enabled_delay[] = "\n> wed: Warnings time [ms] after closing";
const __flash char c_p_time_between_commands[] = "\n> tbc: Time [ms] between commands";
const __flash char c_p_warning_door_open_beep_on_time[] = "\n> wdobont: Warning during Door Open - Beep ON Time [ms]";
const __flash char c_p_warning_door_open_beep_off_time[] = "\n> wdobofft: Warning during Door Open - Beep OFF Time [ms]";
const __flash char c_p_warning_door_open_light_on_time[] = "\n> wdolont: Warning during Door Open - Light ON Time [ms]";
const __flash char c_p_warning_door_open_light_off_time[] = "\n> wdolofft: Warning during Door Open - Light OFF Time [ms]";
const __flash char c_p_key_n_opening_code[] = "\n> kno: Key opening code. n - number 1, 2 or 3. Max code length 60 digits";
const __flash char c_p_key_n_closing_code[] = "\n> knc: Key closing code. n - number 1, 2 or 3. Max code length 60 digits";
const __flash char c_p_reset[] = "\n> reset: RESET all settings to factory defaults. Usage  w:reset;";
const __flash char c_p_unknown_param_of_p[] = "\n> Unknown param of p:";
const __flash char c_p_unknown_param_of_r[] = "\n> Unknown param of r:";
const __flash char c_p_unknown_param_of_w[] = "\n> Unknown param of w:";
const __flash char c_p_error[] = "\n> Syntax error";
const __flash char c_p_ok[] = "\n> OK";
const __flash char c_p_err_value_out_of_range[] = "\n> Err: Value is out of range";
const __flash char c_p_err_expected_value[] = "\n> Expected value in range";
const __flash char c_p_wait_for_reset[] = "\n> Wait for RESET ...";

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

// Send response to RS
void _c_send_response_to_rs(uint16_t _c_exe_temp, uint8_t _c_unit){
	itoa(_c_exe_temp, string, 10);
	rs_send_P(c_p_ns);  // New line >
	rs_send((char*)&string);
	if (_c_unit==C_SEC) rs_send_P(c_p_sec);
	if (_c_unit==C_MS) rs_send_P(c_p_ms);
}

// Validate if value is in acceptable range min < x < max
// If value is OK than data are saved in EEPROM
// After save RAM data are updated
void c_validate_save_update(int c_value,
					   	    int c_range_min,
							int c_range_max,
							uint8_t c_unit,
							uint16_t c_eeprom_address){
	if ((c_value < c_range_min)||(c_value> c_range_max)){
		rs_send_P(c_p_err_value_out_of_range);
		rs_send_P(c_p_err_expected_value);
		rs_send(" [");
		itoa(c_range_min, string, 10);
		rs_send((char*)&string);
		rs_send(":");
		itoa(c_range_max, string, 10);
		rs_send((char*)&string);
		rs_send("]");
		if (c_unit == C_SEC) rs_send_P(c_p_sec);
		if (c_unit == C_MS) rs_send_P(c_p_ms);
	} else {
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)c_eeprom_address, c_value);
		garage_read_eeprom_settings();
		rs_send_P(c_p_ok);
	}
}


// Function extract key code from cmd and saves it in EEPROM
void c_validate_save_update_code(uint16_t ee_code_start_address, char *c_cmd_start_code_index){
	c_clear_code();
	c_extract_digits_txt(c_cmd_start_code_index, (char*)&c_code);
	ee_clear_block(ee_code_start_address, 0x40);
	eeprom_update_block(c_code, (void *)ee_code_start_address, strlen(c_code));
	eeprom_busy_wait();
	rc_read_key_codes();
	rs_send_P(c_p_ok);
}


void c_clear_code(void){
	uint8_t i=0;
	while (c_code[i]){
		c_code[i] = 0;
		if (i<0x40)	i++;
		else i = 0;
	}
}

// execute comd
void c_exe_cmd(void){
	int c_exe_temp = 0;

	c_cmd_is_executed = TRUE;
	rs_send_P(c_p_nq);  // New line ?
	rs_send((char*)&c_cmd);

	// List of commands
	if (strncmp((char*)&c_cmd[0], "l?", 2 ) == 0){
		rs_send_P(c_p_read_x_param);
		rs_send_P(c_p_write_y_value_to_x_param);
		rs_send_P(c_p_list_of_params);
		rs_send_P(c_p_description_of_x_param);
	}
	else if (strncmp((char*)&c_cmd[0], "p?", 2 ) == 0){
		rs_send_P(c_p_dot_mct_mot);
		rs_send_P(c_p_wed_wdobont_wdobofft);
		rs_send_P(c_p_key_code_r_w);
	}

	// p: - PARAM description
	else if (strncmp((char*)&c_cmd[0], "p:", 2) == 0){
		if (strncmp((char*)&c_cmd[2], "dot", 3) == 0){  // Door Open Time before start closing
			rs_send_P(c_p_door_open_time);
		}
		else if (strncmp((char*)&c_cmd[2], "mct", 3) == 0){  // Max door Closing Time
			rs_send_P(c_p_max_closing_time);
		}
		else if (strncmp((char*)&c_cmd[2], "mot", 3) == 0){  // Max door Opening Time
			rs_send_P(c_p_max_opening_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wt", 2) == 0){  // Warning Time before start closing
			rs_send_P(c_p_warning_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wbont", 5) == 0){  // Warning Beep ON Time
			rs_send_P(c_p_warning_beep_on_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wbofft", 6) == 0){  // Warning Beep OFF Time
			rs_send_P(c_p_warning_beep_off_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wlont", 5) == 0){  // Warming Light ON Time
			rs_send_P(c_p_warning_light_on_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wlofft", 6) == 0){  // Warming Light OFF Time
			rs_send_P(c_p_warning_light_off_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wed", 3) == 0){  // Warnings Enabled Delay
			rs_send_P(c_p_warnings_enabled_delay);
		}
		else if (strncmp((char*)&c_cmd[2], "tbc", 3) == 0){  // Time Between Commands
			rs_send_P(c_p_time_between_commands);
		}
		else if (strncmp((char*)&c_cmd[2], "wdobont", 7) == 0){  // Warning Door Open - Beep ON Time
			rs_send_P(c_p_warning_door_open_beep_on_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wdobofft", 8) == 0){  // Warning Door Open - Beep OFF Time
			rs_send_P(c_p_warning_door_open_beep_off_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wdolont", 7) == 0){  // Warning Door Open - Light ON Time
			rs_send_P(c_p_warning_door_open_light_on_time);
		}
		else if (strncmp((char*)&c_cmd[2], "wdolofft", 8) == 0){  // Warning Door Open - Light OFF Time
			rs_send_P(c_p_warning_door_open_light_off_time);
		}
		else if (strncmp((char*)&c_cmd[2], "kno", 3) == 0){  // Key opening code. n - number [max: 60 digits]
			rs_send_P(c_p_key_n_opening_code);
		}
		else if (strncmp((char*)&c_cmd[2], "knc", 3) == 0){  // Key closing code. n - number [max: 60 digits]
			rs_send_P(c_p_key_n_closing_code);
		}
		else if (strncmp((char*)&c_cmd[2], "reset", 5) == 0){  // RESET to factory defaults.
			rs_send_P(c_p_reset);
		}

		else {
			rs_send_P(c_p_unknown_param_of_p);
		}
	}

	// r: - READ
	else if (strncmp((char*)&c_cmd[0], "r:", 2) == 0){
		if (strncmp((char*)&c_cmd[2], "dot", 3) == 0){  // Door Open Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_MAX_DOOR_OPEN_TIME_SEC);
			_c_send_response_to_rs(c_exe_temp, C_SEC);
		}
		else if (strncmp((char*)&c_cmd[2], "mct", 3) == 0){  // Max Closing Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_MAX_CLOSING_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "mot", 3) == 0){  // Max Opening Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_MAX_OPENING_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wt", 2) == 0){  // Warning Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wbont", 5) == 0){  // Warning Beep ON Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_BEEP_ON_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wbofft", 6) == 0){  // Warning Beep OFF Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_BEEP_OFF_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wlont", 5) == 0){  // Warning Light ON Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_LIGHT_ON_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wlofft", 6) == 0){  // Warning Light OFF Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_LIGHT_OFF_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wed", 3) == 0){  // Warnings Enabled Delay
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNINGS_ENABLED_DELAY_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "tbc", 3) == 0){  // Times Between Commands
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_TIME_BETWEEN_COMMANDS_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdobont", 7) == 0){  // Warning Door Open - Beep On Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdobofft", 8) == 0){  // Warning Door Open - Beep Off Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdolont", 7) == 0){  // Warning Door Open - Light On Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdolofft", 8) == 0){  // Warning Door Open - Light Off Time
			c_exe_temp = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS);
			_c_send_response_to_rs(c_exe_temp, C_MS);
		}

		else if (strncmp((char*)&c_cmd[2], "k1o", 3) == 0){  // Key 1 gate opening code. n - number [max: 60 digits]
			eeprom_read_block(c_code, (const void *)EE_ADR_STRING_PILOT_CODE_OPEN, 0x40);
			rs_send_P(c_p_ns);  // New line >
			rs_send((char*)&c_code);
		}
		else if (strncmp((char*)&c_cmd[2], "k1c", 3) == 0){  // Key 1 gate closing code. n - number [max: 60 digits]
			eeprom_read_block(c_code, (const void *)EE_ADR_STRING_PILOT_CODE_CLOSE, 0x40);
			rs_send_P(c_p_ns);  // New line >
			rs_send((char*)&c_code);
		}

		else if (strncmp((char*)&c_cmd[2], "k2o", 3) == 0){  // Key 2 gate opening code. n - number [max: 60 digits]
			eeprom_read_block(c_code, (const void *)EE_ADR_STRING_PILOT_CODE_OPEN_2, 0x40);
			rs_send_P(c_p_ns);  // New line >
			rs_send((char*)&c_code);
		}
		else if (strncmp((char*)&c_cmd[2], "k2c", 3) == 0){  // Key 2 gate closing code. n - number [max: 60 digits]
			eeprom_read_block(c_code, (const void *)EE_ADR_STRING_PILOT_CODE_CLOSE_2, 0x40);
			rs_send_P(c_p_ns);  // New line >
			rs_send((char*)&c_code);
		}

		else if (strncmp((char*)&c_cmd[2], "k3o", 3) == 0){  // Key 3 gate opening code. n - number [max: 60 digits]
			eeprom_read_block(c_code, (const void *)EE_ADR_STRING_PILOT_CODE_OPEN_3, 0x40);
			rs_send_P(c_p_ns);  // New line >
			rs_send((char*)&c_code);
		}
		else if (strncmp((char*)&c_cmd[2], "k3c", 3) == 0){  // Key 3 gate closing code. n - number [max: 60 digits]
			eeprom_read_block(c_code, (const void *)EE_ADR_STRING_PILOT_CODE_CLOSE_3, 0x40);
			rs_send_P(c_p_ns);  // New line >
			rs_send((char*)&c_code);
		}
		else {
			rs_send_P(c_p_unknown_param_of_r);
		}
	}

	// w: - WRITE
	else if (strncmp((char*)&c_cmd[0], "w:", 2) == 0){
		if (strncmp((char*)&c_cmd[2], "dot:", 4) == 0) {  // Door open time
			c_exe_temp = c_extract_val((char*)&c_cmd[6]);
			c_validate_save_update(c_exe_temp,
								   G_MAX_DOOR_OPEN_TIME_SEC_MIN,
								   G_MAX_DOOR_OPEN_TIME_SEC_MAX,
								   C_SEC,
								   EE_ADR_WORD__G_MAX_DOOR_OPEN_TIME_SEC);
		}
		else if (strncmp((char*)&c_cmd[2], "mct:", 4) == 0) {  // Max Closing Time
			c_exe_temp = c_extract_val((char*)&c_cmd[6]);
			c_validate_save_update(c_exe_temp,
								   G_MAX_CLOSING_TIME_MS_MIN,
								   G_MAX_CLOSING_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_MAX_CLOSING_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "mot:", 4) == 0) {  // Max Opening Time
			c_exe_temp = c_extract_val((char*)&c_cmd[6]);
			c_validate_save_update(c_exe_temp,
								   G_MAX_OPENING_TIME_MS_MIN,
								   G_MAX_OPENING_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_MAX_OPENING_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wt:", 3) == 0) {  // Warning Time
			c_exe_temp = c_extract_val((char*)&c_cmd[5]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_TIME_MS_MIN,
								   G_WARNING_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wbont:", 6) == 0) {  // Warning Beep ON Time
			c_exe_temp = c_extract_val((char*)&c_cmd[8]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_BEEP_ON_TIME_MS_MIN,
								   G_WARNING_BEEP_ON_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_BEEP_ON_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wbofft:", 7) == 0) {  // Warning Beep OFF Time
			c_exe_temp = c_extract_val((char*)&c_cmd[9]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_BEEP_OFF_TIME_MS_MIN,
								   G_WARNING_BEEP_OFF_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_BEEP_OFF_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wlont:", 6) == 0) {  // Warning Light ON Time
			c_exe_temp = c_extract_val((char*)&c_cmd[8]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_LIGHT_ON_TIME_MS_MIN,
								   G_WARNING_LIGHT_ON_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_LIGHT_ON_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wlofft:", 7) == 0) {  // Warning Light OFF Time
			c_exe_temp = c_extract_val((char*)&c_cmd[9]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_LIGHT_OFF_TIME_MS_MIN,
								   G_WARNING_LIGHT_OFF_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_LIGHT_OFF_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wed:", 4) == 0) {  // Warnings Enabled Delay
			c_exe_temp = c_extract_val((char*)&c_cmd[6]);
			c_validate_save_update(c_exe_temp,
								   G_WARNINGS_ENABLED_DELAY_MS_MIN,
								   G_WARNINGS_ENABLED_DELAY_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNINGS_ENABLED_DELAY_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "tbc:", 4) == 0) {  // Time Between Commands
			c_exe_temp = c_extract_val((char*)&c_cmd[6]);
			c_validate_save_update(c_exe_temp,
								   G_TIME_BETWEEN_COMMANDS_MS_MIN,
								   G_TIME_BETWEEN_COMMANDS_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_TIME_BETWEEN_COMMANDS_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdobont:", 8) == 0) {  // Warning Door Open - Beep ON Time
			c_exe_temp = c_extract_val((char*)&c_cmd[10]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS_MIN,
								   G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdobofft:", 9) == 0) {  // Warning Door Open - Beep OFF Time
			c_exe_temp = c_extract_val((char*)&c_cmd[11]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS_MIN,
								   G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdolont:", 8) == 0) {  // Warning Door Open - Light ON Time
			c_exe_temp = c_extract_val((char*)&c_cmd[10]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS_MIN,
								   G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS);
		}
		else if (strncmp((char*)&c_cmd[2], "wdolofft:", 9) == 0) {  // Warning Door Open - Light OFF Time
			c_exe_temp = c_extract_val((char*)&c_cmd[11]);
			c_validate_save_update(c_exe_temp,
								   G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS_MIN,
								   G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS_MAX,
								   C_MS,
								   EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS);
		}


		else if (strncmp((char*)&c_cmd[2], "k1o:", 4) == 0) {  // Key 1 gate opening code. n - number
			c_validate_save_update_code(EE_ADR_STRING_PILOT_CODE_OPEN, (char*)&c_cmd[6]);
		}
		else if (strncmp((char*)&c_cmd[2], "k1c:", 4) == 0) {  // Key 1 gate closing code. n - number
			c_validate_save_update_code(EE_ADR_STRING_PILOT_CODE_CLOSE, (char*)&c_cmd[6]);
		}

		else if (strncmp((char*)&c_cmd[2], "k2o:", 4) == 0) {  // Key 2 gate opening code. n - number
			c_validate_save_update_code(EE_ADR_STRING_PILOT_CODE_OPEN_2, (char*)&c_cmd[6]);
		}
		else if (strncmp((char*)&c_cmd[2], "k2c:", 4) == 0) {  // Key 2 gate closing code. n - number
			c_validate_save_update_code(EE_ADR_STRING_PILOT_CODE_CLOSE_2, (char*)&c_cmd[6]);
		}

		else if (strncmp((char*)&c_cmd[2], "k3o:", 4) == 0) {  // Key 3 gate opening code. n - number
			c_validate_save_update_code(EE_ADR_STRING_PILOT_CODE_OPEN_3, (char*)&c_cmd[6]);
		}
		else if (strncmp((char*)&c_cmd[2], "k3c:", 4) == 0) {  // Key 3 gate closing code. n - number
			c_validate_save_update_code(EE_ADR_STRING_PILOT_CODE_CLOSE_3, (char*)&c_cmd[6]);
		}

		else if (strncmp((char*)&c_cmd[2], "reset", 5) == 0) {  // Reset to factory default
			ee_clear();
			rs_send_P(c_p_wait_for_reset);
			m_reset_cpu();
		}

		else {
			rs_send_P(c_p_unknown_param_of_w);
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
	} else {
		c_timer_div++;
	}


	if (c_timer_div2 > 1000){  // 1s
		c_timer_div2 = 0;

		// Code here
		c_cmd_is_executed = FALSE;

	} else {
		c_timer_div2++;
	}
}


void c_cmd_handler(void){
	c_read_rs_buffer();
	if (c_parse_input_f == TRUE){
		c_parse_input();
	}
}
