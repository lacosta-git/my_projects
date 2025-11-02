/*
 * Command execution
 *
 *  Created on: 10.06.2019
 *      Author: lapiUser
 */

#include <stdlib.h>
#include "cmd_exe.h"
#include "string.h"

extern char pr_program[];

uint16_t ce_i;
char ce_string[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char ce_temp_str[50];

uint8_t ce_detect_line_number (char *dl_line){
	uint16_t dl_line_number = 0;

	for (ce_i=0; ce_i<3; ce_i++){
		if ((*(dl_line+ce_i)) > 0x20){
			if (ce_i >= 1) dl_line_number *= 10;
			dl_line_number += (*(dl_line+ce_i)) & 0x0F;
		} else break;
	}
	if (dl_line_number > 250) dl_line_number = 250;
	return (uint8_t)dl_line_number;
}


uint16_t ce_count_user_program_characters(char *ce_user_pr){
	uint16_t ce_counter = 0;
	while (*ce_user_pr){
		ce_user_pr++;
		ce_counter++;
		if (ce_counter >= PR_RAM_PROGRAM_SIZE) break;
	}
	return ce_counter;
}


uint8_t ce_move_user_program(char *mu_pt_program,
							 uint16_t mu_number_of_characters,
							 uint8_t mu_line_length){
	int mu_i;
	for (mu_i=mu_number_of_characters; mu_i>=0; mu_i--){
		*(mu_pt_program+mu_line_length+1+mu_i) = *(mu_pt_program+mu_i);
	}
	return 0;
}


uint8_t ce_del_user_pogram_line(char *dl_pt_program, uint8_t dl_line){
	uint8_t ce_dl_line_deleted = FALSE;
	uint8_t ce_dl_program_line_number = 0;
	char *ce_dl_end_of_user_line;
	uint16_t ce_dl_len_till_end = 0;
	uint8_t ce_sm_dl = CE_DL_DETECT_LINE_NUMBER;

	ce_dl_end_of_user_line = dl_pt_program;
	while(ce_dl_line_deleted == FALSE){
		switch (ce_sm_dl){
		case CE_DL_DETECT_LINE_NUMBER:
			if (*dl_pt_program == 0) {
				ce_sm_dl = CE_DL_END;
			} else {
				for (ce_i=0; ce_i<3; ce_i++){
					ce_string[ce_i] = *(dl_pt_program + ce_i);
				}
				ce_dl_program_line_number = ce_detect_line_number((char*)&ce_string);
				ce_sm_dl = CE_DL_FIND_END_OF_LINE;
			}
			break;

		case CE_DL_FIND_END_OF_LINE:
			ce_dl_end_of_user_line = dl_pt_program;
			while(*ce_dl_end_of_user_line != '\n'){
				ce_dl_end_of_user_line++;
			}
			ce_sm_dl = CE_DL_COMPARE;
			break;

		case CE_DL_COMPARE:
			if (ce_dl_program_line_number == dl_line){
				ce_sm_dl = CE_DL_DEL_LINE;
			}
			else {
				dl_pt_program = ++ce_dl_end_of_user_line;
				ce_sm_dl = CE_DL_DETECT_LINE_NUMBER;
			}
			break;

		case CE_DL_DEL_LINE:
			ce_dl_len_till_end = ce_count_user_program_characters(++ce_dl_end_of_user_line);
			if (ce_dl_len_till_end == 0){
				while (*dl_pt_program != 0){
					*dl_pt_program = 0;
					dl_pt_program++;
				}
			} else {
				for (ce_i=0; ce_i<ce_dl_len_till_end; ce_i++){
					*(dl_pt_program++) = *(ce_dl_end_of_user_line++);
				}
				while (*dl_pt_program != 0){
					*dl_pt_program = 0;
					dl_pt_program++;
				}
			}
			ce_sm_dl = CE_DL_END;
			break;

		case CE_DL_END:
			ce_dl_line_deleted = TRUE;
			break;

		default:
			ce_sm_dl = CE_DL_END;
			break;
		}
	}
	return 0;
}


uint8_t cmd_exe_parser(char *ce_command){
	char *ce_pt_program;

	uint16_t ce_j;
	uint8_t ce_mode = CE_EXECUTION;
	int ce_rsp = 0;
	uint8_t ce_cmd_len = 0;
	uint8_t ce_uint8_line_number = 0;
	uint8_t ce_line_length = 0;
	uint8_t ce_user_program_updated = FALSE;
	uint8_t ce_program_line_number = 0;
	uint16_t ce_end_of_user_line = 0;
	uint8_t ce_sm_edit_user_program = 0;
	uint16_t ce_number_of_characters = 0;

	ce_cmd_len = strlen(ce_command);

	// Find first characters in cmd
	for (ce_i=0; ce_i<ce_cmd_len; ce_i++){
		if (*(ce_command+ce_i) > 0x20){
			ce_command += ce_i;  // Move pointer to first character
			break;
		}
	}

	// Detect command mode
	// Number = programming mode
	// Char = execution mode
	if ((*ce_command >= 0x30) &
	    (*ce_command <= 0x39)){
		ce_mode = CE_PROGRAMMING;
	} else {
		ce_mode = CE_EXECUTION;
	}

	if (ce_mode == CE_PROGRAMMING){

		// Detect user program line number to update
		ce_uint8_line_number = ce_detect_line_number(ce_command);
		// Detect length of user program line
		ce_line_length = strlen(ce_command);

		// Set starting pointer
		ce_pt_program = (char*)&pr_program;
		ce_sm_edit_user_program = CE_DETECT_LINE_NUMBER;
		while(ce_user_program_updated == FALSE){
			switch (ce_sm_edit_user_program){
			case CE_DETECT_LINE_NUMBER:
				if (*ce_pt_program == 0) {
					ce_sm_edit_user_program = CE_INSERT_NEW_LINE;
				} else {
					for (ce_j=0; ce_j<3; ce_j++){
						ce_string[ce_j] = *(ce_pt_program + ce_j);
					}
					ce_program_line_number = ce_detect_line_number((char*)&ce_string);
					ce_sm_edit_user_program = CE_FIND_END_OF_LINE;
				}
				break;

			case CE_FIND_END_OF_LINE:
				ce_j = 0;
				ce_end_of_user_line = 0;
				while(*(ce_pt_program + ce_j) != '\n'){
					ce_j++;
				}
				ce_end_of_user_line += ce_j;
				ce_sm_edit_user_program = CE_COMPARE_LINE_NUMBERS;
				break;

			case CE_COMPARE_LINE_NUMBERS:
				if (ce_uint8_line_number < ce_program_line_number) {
					ce_number_of_characters = ce_count_user_program_characters((char*)ce_pt_program);

//					itoa(ce_number_of_characters, ce_string, 10);
//					ifc_print((char*)&ce_string);
//					ifc_print("\r\n> ");

					ce_move_user_program((char*)ce_pt_program, ce_number_of_characters, ce_line_length);
					ce_sm_edit_user_program = CE_INSERT_NEW_LINE;
				} else if (ce_uint8_line_number == ce_program_line_number) {
					ce_del_user_pogram_line((char*)&pr_program, ce_uint8_line_number);
					ce_pt_program = (char*)&pr_program;
					ce_sm_edit_user_program = CE_DETECT_LINE_NUMBER;
				} else if (ce_uint8_line_number > ce_program_line_number) {
					ce_pt_program += (ce_end_of_user_line + 1);
					ce_sm_edit_user_program = CE_DETECT_LINE_NUMBER;
				} else
					ce_sm_edit_user_program = CE_END_OF_EDIT;
				break;

			case CE_INSERT_NEW_LINE:
				// Insert new line of user program
				while (*ce_command){
					*ce_pt_program = *ce_command;
					ce_pt_program++;
					ce_command++;
				}
				*ce_pt_program = '\n';

				ce_sm_edit_user_program = CE_END_OF_EDIT;
				break;

			case CE_END_OF_EDIT:
				ce_user_program_updated = TRUE;
				break;

			default:
				ce_sm_edit_user_program = CE_END_OF_EDIT;
				break;
			}
		}
	}

//	itoa(ce_program_line_number, ce_string, 10);
//	ifc_print((char*)&ce_string);
//	ifc_print("\r\n> ");

	if (ce_mode == CE_EXECUTION){

		ce_rsp = strcmp(ce_command, "led\r");
		if (ce_rsp == 0){
			ifc_LED_TRG();
		}

		ce_rsp = strcmp(ce_command, "list\r");
		if (ce_rsp == 0){

			ce_pt_program = (char*)&pr_program;

			ifc_print("\r\n");
			ifc_print((char*)&pr_program);
			ifc_print("\r\n> ");
		}


		// DEL
		ce_rsp = strcmp(ce_command, "del\r");
		if (ce_rsp == 0){

			ce_pt_program = (char*)&pr_program;

			while(*ce_pt_program){
				*ce_pt_program = 0;
				ce_pt_program++;
			}
		}


		// DEL nn
		ce_rsp =(int)strstr(ce_command, "del ");
		if (ce_rsp != 0){

			ce_command += 4;
			ce_program_line_number = ce_detect_line_number(ce_command);
			ifc_LED_TRG();
			ce_del_user_pogram_line((char*)&pr_program, ce_program_line_number);
		}


		// Diagnostic
		ce_rsp = strcmp(ce_command, "li\r");
		if (ce_rsp == 0){

			ce_pt_program = (char*)&pr_program;

			itoa((int)ce_pt_program, ce_string, 10);
			ifc_print((char*)&ce_string);
			ifc_print("\r\n> ");

			for (ce_i=0; ce_i<49; ce_i++){
				if (*ce_pt_program == 0){
					ce_temp_str[ce_i] = '0';
				}
				else if (*ce_pt_program == '\r'){
					ce_temp_str[ce_i] = 'R';
				}
				else if (*ce_pt_program == '\n'){
					ce_temp_str[ce_i] = 'N';
				}

				else {
					ce_temp_str[ce_i] = *ce_pt_program;
				}
				ce_pt_program++;
			}

			ifc_print("\r\n");
			ifc_print((char*)&ce_temp_str);
			ifc_print("\r\n> ");
		}
	}

	return 0;
}

