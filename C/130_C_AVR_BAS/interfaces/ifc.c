/*
 * ATMega328P
 * ifc.c
 *
 *  Created on: 31.05.2019
 *      Author: lapiUser
 */

#include "ifc.h"
#include "../rs232/rs232.h"

// Variables
volatile char ifc_new_line[] = {'\r', '\n', '>', ' ', 0};
volatile char rx_data;
volatile char new_char_received;
uint8_t ret;

// Interfaces init
uint8_t ifc_init(void){
	ifc_LED_DDR();
	init_serial_communication(9600);
	return 0;
}

// Standard output
uint8_t ifc_print(char* string_data){
	rs_send(string_data);
	return 0;
}


uint8_t ifc_print_new_line(void){
	ifc_print((char*)&ifc_new_line);
	return 0;
}



// ifc handler
uint8_t ifc_handler(void){
	rs_send_handler();
	ret = rs_receive_handler();
	if (ret == NEW_DATA_RECEIVED){
		new_char_received = TRUE;
	}
	return 0;
}

// Building command
uint8_t ifc_read_received_data(char* in_char){
	if (new_char_received == TRUE){
		rs_read_received_data(in_char);
		new_char_received = FALSE;
		return TRUE;
	}
	return 0;
}
