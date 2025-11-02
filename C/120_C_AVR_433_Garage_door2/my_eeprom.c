/*
 * my_eeprom.c
 *
 *  Created on: 23.10.2020
 *      Author: lapiUser
 */
#include <avr/eeprom.h>
#include <string.h>
#include "main.h"
#include "my_eeprom.h"
#include "garage/garage.h"
#include "radio_decode.h"


uint8_t EEMEM my_eeprom_array[0x400];

void ee_init(void){
	uint8_t ee_temp_val;
	eeprom_busy_wait();
	ee_temp_val = eeprom_read_byte((uint8_t*)EE_ADR_BYTE__SET_DEFAULT);
	if (ee_temp_val != 0x3A){  // 0x3A is used to detect if EEPROM has been initialized with defaults

		ee_clear();

		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_MAX_CLOSING_TIME_MS, G_MAX_CLOSING_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_MAX_OPENING_TIME_MS, G_MAX_OPENING_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_MAX_DOOR_OPEN_TIME_SEC, G_MAX_DOOR_OPEN_TIME_SEC_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_TIME_MS, G_WARNING_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_BEEP_ON_TIME_MS, G_WARNING_BEEP_ON_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_BEEP_OFF_TIME_MS, G_WARNING_BEEP_OFF_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_LIGHT_ON_TIME_MS, G_WARNING_LIGHT_ON_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_LIGHT_OFF_TIME_MS, G_WARNING_LIGHT_OFF_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNINGS_ENABLED_DELAY_MS, G_WARNINGS_ENABLED_DELAY_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_TIME_BETWEEN_COMMANDS_MS, G_TIME_BETWEEN_COMMANDS_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS, G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS, G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS, G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS_DEF);
		eeprom_busy_wait();
		eeprom_update_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS, G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS_DEF);

		eeprom_busy_wait();
		ee_temp_val = strlen(PILOT_CODE_OPEN);
		eeprom_update_block(PILOT_CODE_OPEN, &my_eeprom_array[EE_ADR_STRING_PILOT_CODE_OPEN], ee_temp_val);
		eeprom_busy_wait();
		ee_temp_val = strlen(PILOT_CODE_CLOSE);
		eeprom_update_block(PILOT_CODE_CLOSE, &my_eeprom_array[EE_ADR_STRING_PILOT_CODE_CLOSE], ee_temp_val);
		eeprom_busy_wait();

		ee_temp_val = strlen(PILOT_CODE_OPEN_2);
		eeprom_update_block(PILOT_CODE_OPEN_2, &my_eeprom_array[EE_ADR_STRING_PILOT_CODE_OPEN_2], ee_temp_val);
		eeprom_busy_wait();
		ee_temp_val = strlen(PILOT_CODE_CLOSE_2);
		eeprom_update_block(PILOT_CODE_CLOSE_2, &my_eeprom_array[EE_ADR_STRING_PILOT_CODE_CLOSE_2], ee_temp_val);
		eeprom_busy_wait();

		ee_temp_val = strlen(PILOT_CODE_OPEN_3);
		eeprom_update_block(PILOT_CODE_OPEN_3, &my_eeprom_array[EE_ADR_STRING_PILOT_CODE_OPEN_3], ee_temp_val);
		eeprom_busy_wait();
		ee_temp_val = strlen(PILOT_CODE_CLOSE_3);
		eeprom_update_block(PILOT_CODE_CLOSE_3, &my_eeprom_array[EE_ADR_STRING_PILOT_CODE_CLOSE_3], ee_temp_val);
		eeprom_busy_wait();

		eeprom_busy_wait();  // 0x3A is used to detect if EEPROM has been initialized with defaults
		eeprom_update_byte((uint8_t*)EE_ADR_BYTE__SET_DEFAULT, 0x3A);
	}
	eeprom_busy_wait();
}


void ee_clear(void){
	// Clear EEPROM
	for (uint16_t i=0; i<0x400; i++){
		eeprom_busy_wait();
		eeprom_write_byte(&my_eeprom_array[i], 0);
	}
	eeprom_busy_wait();
}


void ee_clear_block(uint16_t ee_start_byte, uint16_t ee_len){
	// Clear block
	uint16_t ee_end_byte = ee_start_byte + ee_len;

	if ((ee_end_byte > ee_start_byte) &&
		(ee_end_byte <0x3FF)){

		for (uint16_t i=ee_start_byte; i<ee_end_byte; i++){
			eeprom_busy_wait();
			eeprom_write_byte(&my_eeprom_array[i], 0);
		}
	}
	eeprom_busy_wait();
}
