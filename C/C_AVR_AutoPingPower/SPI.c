/*
 * SPI.c
 *
 */ 

#include "SPI.h"
#include "main.h"

//Initialize
void initializeSPI(void)
{
	// Enable SPI
	// Master mode
	// Mode 0
	// Fosc/8
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPSR |= (1<<SPI2X);
	
	//Disable CS
	SPI_PORT |= (1<<SPI_CS);
}

// Write
void SPIwriteByte(uint8_t data)
{
	// Load data to sending register
	SPDR = data;
	
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

// Write
void SPIwriteWord(uint16_t data)
{
	// MSB first
	SPIwriteByte(((data & 0xFF00) >> 8));
	// LSB last
	SPIwriteByte((data & 0x00FF));
}


// Read
uint8_t SPIreadByte(void)
{
	SPIwriteByte(0x00);		// Write dummy byte to read
	return (SPDR);
}
