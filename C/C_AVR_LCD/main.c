/*
 * main.c
 *
 *  Created on: 06.05.2017
 *      Author: autoMaster
 */

#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "lcdCharMap.h"

#define		DDR_B()			DDRB = 0xFF; PORTB = 0xFF;  // All outputs
#define		LED_STRING_1_TRG()		PORTB ^= (1<<DDB5) // change state
#define 	RESET_AS_ON()	PORTB &=~(1<<DDB4) // 0
#define 	RESET_AS_OFF()	PORTB |= (1<<DDB4) // 0

#define		DDR_C()			DDRC = 0xFF; PORTC = 0xFF;  // All outputs

//write date
#define AS650_WRITE 0x40

volatile uint8_t i = 0;
volatile uint8_t j = 0;
volatile uint16_t lcdChar = 0;

void clearRAM(){
	i2c_start_wait(AS650_WRITE);
	i2c_write(0x80);  // Address byte
	for (uint8_t i=0; i<=18;i++){
		i2c_write(0x00);
	}
	i2c_stop();
}

int main(){

DDR_B();
DDR_C();

RESET_AS_ON();
//init i2c
i2c_init();
_delay_ms(10);
RESET_AS_OFF();
_delay_ms(100);

i2c_start_wait(AS650_WRITE);
i2c_write(0x00);  // Address byte
i2c_write(0x55);  // SEGCFG1
i2c_write(0x55);  // SEGCFG2
i2c_write(0x55);  // SEGCFG3
i2c_write(0x00);  // SEGCFG4
i2c_write(0x3D);  // LCDCTR
//i2c_write(month);  // P1D
//i2c_write(month);  // P2D
//i2c_write(month);  // P3D
//i2c_write(month);  // P3PUEN
//i2c_write(month);  // ADCFG
//i2c_write(month);  // SETREG
//i2c_write(month);  // ADCTR
//i2c_write(month);  // KSCTR
i2c_stop();
_delay_ms(10);
i2c_start_wait(AS650_WRITE);
i2c_write(0x0A);  // Address byte
i2c_write(0x04);  // SETREG
i2c_stop();
_delay_ms(10);

while(1){
	for (i=42; i<=46; i++){
		lcdChar = pgm_read_word(&lcdCharMap[i]);
		_delay_ms(1000);
		clearRAM();
		_delay_ms(10);
		i2c_start_wait(AS650_WRITE);
		i2c_write(0x80);  // Address byte
		for (j=0; j<=5; j++){
			i2c_write((uint8_t)lcdChar);
			i2c_write((uint8_t)(lcdChar>>8));
		}

		i2c_stop();
	}

	LED_STRING_1_TRG();
}}
