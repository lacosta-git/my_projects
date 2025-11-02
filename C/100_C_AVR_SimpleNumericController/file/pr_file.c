/*
 * pr_file.c
 *
 *  Created on: 18.03.2018
 *      Author: lapiMaster
 */

#include <avr/eeprom.h>
#include "pr_file.h"

uint8_t EEMEM ee_vFile[1000];

void init_ee_vFile(void){
	uint8_t* ptr;
	ptr = (uint8_t*)&ee_vFile;
	eeprom_update_byte(ptr, ';'); ptr++;
	eeprom_update_byte(ptr, 'e'); ptr++;
	eeprom_update_byte(ptr, 'e'); ptr++;
	eeprom_update_byte(ptr, '_'); ptr++;
	eeprom_update_byte(ptr, 'v'); ptr++;
	eeprom_update_byte(ptr, 'F'); ptr++;
	eeprom_update_byte(ptr, 'i'); ptr++;
	eeprom_update_byte(ptr, 'l'); ptr++;
	eeprom_update_byte(ptr, 'e'); ptr++;
	eeprom_update_byte(ptr, 0x0D);
}

void load_line(uint8_t* ptr_read_data,
			   uint8_t* ptr_data_lenght,
			   uint8_t* ptr_ee_vFile,
			   uint16_t ee_start_index){
	uint8_t index;
	uint8_t read_byte;

	// Reading line till EOL, EOF, or CR char
	for (index=0; index<=MAX_LINE_LENGHT; index++){
		read_byte = eeprom_read_byte((ptr_ee_vFile+index));
		if (read_byte == 0x0D) break;  // CR detected
		if (read_byte == 0x1A) break;  // EOF detected
		if (read_byte == 0x0A) break;  // EOL detected
		*(ptr_read_data + index) = read_byte;
		*ptr_data_lenght = index;
	}
}

void load_program_file(void){
	uint8_t data_line[MAX_LINE_LENGHT];
	uint8_t data_lenght = 0;

	load_line((uint8_t*)&data_line,
			  (uint8_t*)&data_lenght,
			  (uint8_t*)&ee_vFile,
			   0);

	for (uint8_t i=0; i<=data_lenght; i++){
		vPaper[3][i] = data_line[i];
	}

}
