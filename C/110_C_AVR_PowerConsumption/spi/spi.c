/*
 * spi.c
 *
 *  Created on: 22 paü 2016
 *      Author: autoMaster
 */

#include <avr/io.h>
#include "spi.h"
#include "../pinConfig/pinConfig.h"

// INIT
void spiMasterInit(void){
	/* Set MOSI and SCK output, all others input */
	SPI_MOSI_AS_OUTPUT;
	SPI_SCK_AS_OUTPUT;
	SPI_MISO_AS_INPUT;

	/* Enable SPI, Master, set clock rate fck/16 */
	// MSB first
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

// WRITE BYTE
void spiWriteByte(uint8_t data){
	/* Start transmission */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

// READ BYTE
uint8_t spiReadByte(void){
	spiWriteByte(0x00);
	return SPDR;
}
