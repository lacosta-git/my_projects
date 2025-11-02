/*
 * at45db0xx.c
 *
 *  Created on: 14 paü 2016
 *      Author: autoMaster
 */

#define PAGE_SIZE	264		// Page size 264 or 256 bytes

#include <avr/io.h>
#include "../pinConfig/pinConfig.h"
#include "../spi/spi.h"
#include "at45db0xx.h"

// Common functions
uint16_t calculatePage(uint32_t adr){
	return (adr/PAGE_SIZE);}

uint16_t calculateByte(uint32_t adr, uint16_t page){
	return (adr - (page * PAGE_SIZE));}

// memory BUSY
uint8_t memBusy(void){
	uint8_t temp;
	MEM_CS_LOW;
	spiWriteByte(COM_STATUS_REGISTER_READ);
	temp = spiReadByte();
	MEM_CS_HIGH;
	return (!(temp & SR_READY_MASK));
}

// Chip erase
void chipErase(void){
	MEM_CS_LOW;
	spiWriteByte(0xc7);
	spiWriteByte(0x94);
	spiWriteByte(0x80);
	spiWriteByte(0x9a);
	MEM_CS_HIGH;
	while(memBusy());
}

void loadMainMemoryPageToBuffer1(uint16_t ui16_pageNumber){
	MEM_CS_LOW;
	spiWriteByte(COM_MAIN_MEM_TO_BUFFER1);
	spiWriteByte((uint8_t)(ui16_pageNumber >> 7));
	spiWriteByte((uint8_t)(ui16_pageNumber << 1));
	spiWriteByte(0x00); 	// Dummy byte
	MEM_CS_HIGH;
	while(memBusy());
}

void saveBuffer1ToMainMemory(uint16_t ui16_pageNumber){
	MEM_CS_LOW;
	spiWriteByte(COM_BUFFER1_TO_MAIN_MEM_WITH_ERASE);
	spiWriteByte((uint8_t)(ui16_pageNumber >> 7));
	spiWriteByte((uint8_t)(ui16_pageNumber << 1));
	spiWriteByte(0x00); 	// Dummy byte
	MEM_CS_HIGH;
	while(memBusy());
}

void buffer1Write_cmd(uint16_t ui16_byteNumber){
	spiWriteByte(COM_BUFFER1_WRITE);
	spiWriteByte(0x00); 	// Dummy byte
	spiWriteByte((uint8_t)(ui16_byteNumber >> 8));
	spiWriteByte((uint8_t)(ui16_byteNumber));
}

void readFlash(char* cptr_data, uint32_t ui32_address, uint8_t ui8_lenght){
	uint16_t ui16_pageNumber = 0;
	uint16_t ui16_byteNumber = 0;

	while(memBusy());

	ui16_pageNumber = calculatePage(ui32_address);
	ui16_byteNumber = calculateByte(ui32_address, ui16_pageNumber);

	MEM_CS_LOW;
	spiWriteByte(COM_CONTINOUS_ARRAY_READ);
	spiWriteByte((uint8_t)(ui16_pageNumber >> 7));
	spiWriteByte((uint8_t)((ui16_pageNumber << 1) + (ui16_byteNumber >> 8)));
	spiWriteByte((uint8_t)(ui16_byteNumber));
	spiWriteByte(0x00); 	// Dummy byte

	for (uint8_t i=0; i<ui8_lenght; i++){
		*cptr_data = spiReadByte();
		cptr_data++;
	}
	MEM_CS_HIGH;
}

void writeFlash(char* cptr_data, uint32_t ui32_address, uint8_t ui8_lenght){
	uint16_t ui16_pageNumber = 0;
	uint16_t ui16_byteNumber = 0;
	uint8_t ui8_index = 1;	// index of written byte
	while(memBusy());

	ui16_pageNumber = calculatePage(ui32_address);
	ui16_byteNumber = calculateByte(ui32_address, ui16_pageNumber);

	// Load Main Memory page to Buffer1
	loadMainMemoryPageToBuffer1(ui16_pageNumber);

	// Open Buffer1 to write
	MEM_CS_LOW;
	buffer1Write_cmd(ui16_byteNumber);

	// Buffer updating
	while(ui8_index <= ui8_lenght){
		// Update byte
		spiWriteByte(*cptr_data); cptr_data++;

		if (ui16_byteNumber + ui8_index == PAGE_SIZE){
			// End of buffer
			MEM_CS_HIGH;  // End of clocking-in data
			while(memBusy());

			// Save Buffer to Main Memory
			saveBuffer1ToMainMemory(ui16_pageNumber);

			// Opening next page
			ui16_pageNumber++;
			ui16_byteNumber = 0;
			loadMainMemoryPageToBuffer1(ui16_pageNumber);
			// Open Buffer1 to write
			MEM_CS_LOW;
			buffer1Write_cmd(ui16_byteNumber);
		}
		ui8_index++;
	}

	// End of data. Closing and saving data.
	MEM_CS_HIGH;  // End of clocking-in data
	while(memBusy());

	// Save Buffer to Main Memory
	saveBuffer1ToMainMemory(ui16_pageNumber);
}
