/*
 * radio_decode.c
 *
 *  Created on: 03.08.2020
 *      Author: lapiUser
 */

#include "main.h"
#include <string.h>
#include "radio_decode.h"
#include "garage/garage.h"
#include "rs232/rs232.h"
#include <avr/eeprom.h>
#include "my_eeprom.h"

#include <stdlib.h>

volatile uint8_t sm_code_detect = CODE_IDLE;
volatile uint16_t code_valid_timer;
volatile uint8_t gpio_data = TRUE;
volatile uint8_t gpio_data_old = FALSE;
volatile uint8_t falling_edge_detected = FALSE;
volatile uint8_t rising_edge_detected = FALSE;
volatile uint8_t bd_counter = 0;
volatile char code[60];
volatile uint8_t decoding;
volatile uint8_t c_index;

char rc_key_1_open_code[0x40];
char rc_key_1_close_code[0x40];
char rc_key_2_open_code[0x40];
char rc_key_2_close_code[0x40];
char rc_key_3_open_code[0x40];
char rc_key_3_close_code[0x40];


void rc_init(void){
	for (uint8_t i=0; i<0x40; i++){
		rc_key_1_open_code[i] = 0;
		rc_key_1_close_code[i] = 0;
		rc_key_2_open_code[i] = 0;
		rc_key_2_close_code[i] = 0;
		rc_key_3_open_code[i] = 0;
		rc_key_3_close_code[i] = 0;
	}
	rc_read_key_codes();
}


void rc_read_key_codes(void){
	eeprom_read_block(rc_key_1_open_code, (const void *)EE_ADR_STRING_PILOT_CODE_OPEN, 0x40);
	eeprom_read_block(rc_key_1_close_code, (const void *)EE_ADR_STRING_PILOT_CODE_CLOSE, 0x40);
	eeprom_read_block(rc_key_2_open_code, (const void *)EE_ADR_STRING_PILOT_CODE_OPEN_2, 0x40);
	eeprom_read_block(rc_key_2_close_code, (const void *)EE_ADR_STRING_PILOT_CODE_CLOSE_2, 0x40);
	eeprom_read_block(rc_key_3_open_code, (const void *)EE_ADR_STRING_PILOT_CODE_OPEN_3, 0x40);
	eeprom_read_block(rc_key_3_close_code, (const void *)EE_ADR_STRING_PILOT_CODE_CLOSE_3, 0x40);
}


void radio_decode_handler(){
	switch (sm_code_detect){
	case CODE_IDLE:
		if (falling_edge_detected == TRUE){
			sm_code_detect = CODE_COUNT_LOW;
		}
		break;

	case CODE_COUNT_LOW:
		if (rising_edge_detected == TRUE){
			rising_edge_detected = FALSE;

			if (bd_counter >= 90){		// SYNC BIT DETECTED
				sm_code_detect = CODE_BIT_DETECT;
				c_index = 0;

				if (code_valid_timer > 0){
					verify_code();
				}

				for (uint8_t i=0; i<60; i++){
					code[i] = 0;
				}
			} else {
				sm_code_detect = CODE_IDLE;
			}
			bd_counter = 0;
		}
		break;

	case CODE_BIT_DETECT:
		if (rising_edge_detected == TRUE){
			rising_edge_detected = FALSE;
			if ((bd_counter >= 2) &&
				(bd_counter < 5)){			// 0
				code[c_index] = '0';
			} else if ((bd_counter >= 5) &&
					   (bd_counter < 20)){		// 1
					code[c_index] = '1';
			}
			else {
				sm_code_detect = CODE_IDLE;
				code_valid_timer = CODE_VALID_TIME_MS;
			}
			c_index++;
			bd_counter = 0;
		}
		break;
	default:
		sm_code_detect = CODE_IDLE;
		break;
	}
}


void verify_code(void){

	if (strlen((char*)&code) > 23){

		rs_send("\r");
		rs_send((char*)&code);

		if ((strcmp(((char*)&code), (char*)&rc_key_1_open_code)==0)||
			(strcmp(((char*)&code), (char*)&rc_key_2_open_code)==0)||
			(strcmp(((char*)&code), (char*)&rc_key_3_open_code)==0))
		{
			garage_button_pressed(BUTTON_OPEN);				// OPEN button pressed
		}

		if ((strcmp(((char*)&code), (char*)&rc_key_1_close_code)==0)||
			(strcmp(((char*)&code), (char*)&rc_key_2_close_code)==0)||
			(strcmp(((char*)&code), (char*)&rc_key_3_close_code)==0))
		{
			garage_button_pressed(BUTTON_CLOSE);			// CLOSE button pressed
		}
	}
}


void radio_code_time_tick(void){
	if (code_valid_timer > 0){
		code_valid_timer--;
	}
}


void data_pin_handler(void){
	gpio_data = READ_DATA_PIN();
	gpio_data = ~gpio_data & 0x01;		// Required negation due to HW changes

	if ((gpio_data == FALSE) &
		(gpio_data_old == TRUE)){
		falling_edge_detected = TRUE;
		TCNT1 = 0;					// Reset counter
	} else falling_edge_detected = FALSE;

	if ((gpio_data == TRUE) &
		(gpio_data_old == FALSE)){
		rising_edge_detected = TRUE;
		bd_counter = TCNT1L;		// Read MSB of the counter
	} else rising_edge_detected = FALSE;

	gpio_data_old = gpio_data;
}
