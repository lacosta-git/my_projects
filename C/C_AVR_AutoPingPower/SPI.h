/*
 * SPI.h
 *
 */ 

#include <avr/io.h>

//Function prototypes
void initializeSPI(void);

// Write
void SPIwriteByte(uint8_t);
void SPIwriteWord(uint16_t);

// Read 
uint8_t SPIreadByte(void);
uint16_t SPIreadWord(void);

