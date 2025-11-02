/*
 * Command Manager
 *
 *  Created on: 04.06.2019
 *      Author: lapiUser
 */
#include "cm.h"
#include "cmd_exe.h"

volatile uint8_t cm_new_cmd = FALSE;
volatile uint8_t cm_s_cmd_index = 0;
volatile char cm_s_cmd[MAX_CMD_LEN];
volatile char cm_a_in_char[] = {0, 0};

uint8_t cm_cmd_builder(char cm_in_char){
	// Add char to command
	cm_s_cmd[cm_s_cmd_index] = cm_in_char;
	if (cm_s_cmd_index < MAX_CMD_LEN-2){
		cm_s_cmd_index++;
	}

	if (cm_in_char == 0x0d){
		// Enter received. Command should be executed
		cm_new_cmd = TRUE;
		ifc_print_new_line();
	} else {
		// Writing on terminal
		cm_a_in_char[0] = cm_in_char;
		ifc_print((char*)&cm_a_in_char);
	}

	return 0;
}

uint8_t cm_is_new_cmd_ready(void){
	if (cm_new_cmd == TRUE){
		return TRUE;
	} else return FALSE;
}

uint8_t cm_excute_cmd(void){
	uint8_t i;

	// Execute command
	cmd_exe_parser((char*)cm_s_cmd);


	// Clean command buffer
	for (i=0; i<=MAX_CMD_LEN; i++){
		cm_s_cmd[i] = 0;
	}
	cm_s_cmd_index = 0;
	cm_new_cmd = FALSE;
	return 0;
}
