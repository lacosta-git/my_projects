/*
 * Simple Serial communication library
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

//
// BSD 3-Clause License
//
// Copyright (c) 2017, Tero Saarni
//
//
// This software re-programs ATtiny85 fuses using the high-voltage
// serial programming method.
//
//
// References
//
//   * ATtiny85 datasheet -
//     http://www.atmel.com/images/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
//
//   * Online fuse value calculator -
//     http://www.engbedded.com/fusecalc/
//
//
// High-voltaga serial programming instruction set (from ATtiny85 datasheet)
//
//   Write Fuse Low Bits
//
//         Instr.1/5        Instr.2/6        Instr.3          Instr.4          Operation remark
//   SDI   0_0100_0000_00   0_A987_6543_00   0_0000_0000_00   0_0000_0000_00   Wait after Instr. 4 until SDO
//   SII   0_0100_1100_00   0_0010_1100_00   0_0110_0100_00   0_0110_1100_00   goes high. Write A - 3 = “0” to
//   SDO   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   program the Fuse bit.
//
//   Write Fuse High Bits
//
//         Instr.1/5        Instr.2/6        Instr.3          Instr.4          Operation remark
//   SDI   0_0100_0000_00   0_IHGF_EDCB_00   0_0000_0000_00   0_0000_0000_00   Wait after Instr. 4 until SDO
//   SII   0_0100_1100_00   0_0010_1100_00   0_0111_0100_00   0_0111_1100_00   goes high. Write I - B = “0” to
//   SDO   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   program the Fuse bit.
//
//   Write Fuse Extended Bits
//
//         Instr.1/5        Instr.2/6        Instr.3          Instr.4          Operation remark
//   SDI   0_0100_0000_00   0_0000_000J_00   0_0000_0000_00   0_0000_0000_00   Wait after Instr. 4 until SDO
//   SII   0_0100_1100_00   0_0010_1100_00   0_0110_0110_00   0_0110_1110_00   goes high. Write J = “0” to
//   SDO   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   x_xxxx_xxxx_xx   program the Fuse bit.
//


#include <stdlib.h>
#include "main.h"
#include <util/delay.h>


// ATtiny85 pin mapaping at Arduino nano programmer
#define SDI_PIN 9     // serial data input
#define SII_PIN 10    // serial instruction input
#define SDO_PIN 11    // serial data output
#define SCI_PIN 12    // serial clock input
#define RESET_PIN 13  // reset


void digitalWrite(uint8_t pin, uint8_t bit_value){
	switch(pin){
	case(SDI_PIN):
		if (bit_value > LOW){
			SET_SDI_PIN_HIGH();
		} else {
			SET_SDI_PIN_LOW();
		}
	break;
	case(SII_PIN):
			if (bit_value > LOW){
				SET_SII_PIN_HIGH();
			} else {
				SET_SII_PIN_LOW();
			}
	break;
	case(SDO_PIN):
			if (bit_value == HIGH){
				SET_SDO_PIN_HIGH();
			} else {
				SET_SDO_PIN_LOW();
			}
	break;
	case(SCI_PIN):
				if (bit_value == HIGH){
					SET_SCI_PIN_HIGH();
				} else {
					SET_SCI_PIN_LOW();
				}
	break;
	case(RESET_PIN):
			if (bit_value == HIGH){
				SET_RESET_PIN_HIGH();
			} else {
				SET_RESET_PIN_LOW();
			}
	break;
	}
	_delay_ms(1);
}

uint8_t digitalRead(uint8_t pin){
	pin += 1;
	return READ_SDO_PIN();
}

void avr_hvsp_write(uint8_t sdi, uint8_t sii)
{
   digitalWrite(SDI_PIN, LOW);
   digitalWrite(SII_PIN, LOW);
   digitalWrite(SCI_PIN, HIGH);
   digitalWrite(SCI_PIN, LOW);


   for (uint8_t i = 0; i < 8; i++)
   {
      digitalWrite(SDI_PIN, (sdi & (1 << (7-i))));
      digitalWrite(SII_PIN, (sii & (1 << (7-i))));
      digitalWrite(SCI_PIN, HIGH);
      digitalWrite(SCI_PIN, LOW);
   }

   digitalWrite(SDI_PIN, LOW);
   digitalWrite(SII_PIN, LOW);
   digitalWrite(SCI_PIN, HIGH);
   digitalWrite(SCI_PIN, LOW);
   digitalWrite(SCI_PIN, HIGH);
   digitalWrite(SCI_PIN, LOW);
}

void avr_hvsp_write_lfuse(uint8_t lfuse)
{
   avr_hvsp_write(0b01000000, 0b01001100);
   avr_hvsp_write(lfuse,      0b00101100);
   avr_hvsp_write(0b00000000, 0b01100100);
   avr_hvsp_write(0b00000000, 0b01101100);
   while (digitalRead(SDO_PIN) == LOW) { };
}

void avr_hvsp_write_hfuse(uint8_t hfuse)
{
   avr_hvsp_write(0b01000000, 0b01001100);
   avr_hvsp_write(hfuse,      0b00101100);
   avr_hvsp_write(0b00000000, 0b01110100);
   avr_hvsp_write(0b00000000, 0b01111100);
   while (digitalRead(SDO_PIN) == LOW) { };
}

void avr_hvsp_write_efuse(uint8_t efuse)
{
   avr_hvsp_write(0b01000000, 0b01001100);
   avr_hvsp_write(efuse,      0b00101100);
   avr_hvsp_write(0b00000000, 0b01100110);
   avr_hvsp_write(0b00000000, 0b01101110);
   while (digitalRead(SDO_PIN) == LOW) { };
}


int main()
{

	// setup pin modes
	SET_SDI_PIN_AS_OUTPUT();
	SET_SII_PIN_AS_OUTPUT();
	SET_SDO_PIN_AS_OUTPUT();
	SET_SCI_PIN_AS_OUTPUT();
	SET_RESET_PIN_AS_OUTPUT();
	LED_STRING_1_DDR();

//	for (uint8_t j=0; j<10; j++){
//		_delay_ms(200);
//		LED_STRING_1_TRG();
//	}

   // enter programming mode
   digitalWrite(SDI_PIN, LOW);
   digitalWrite(SII_PIN, LOW);
   digitalWrite(SDO_PIN, LOW);
   digitalWrite(SCI_PIN, LOW);
   digitalWrite(RESET_PIN, HIGH); // pull attiny85 reset pin to ground
   _delay_ms(100);
   digitalWrite(RESET_PIN, LOW);  // attiny85 reset pin to 12V
   _delay_ms(100);
   SET_SDO_PIN_AS_INPUT();
   _delay_ms(300);

   // write  fuse values
   avr_hvsp_write_lfuse(0xe4);  // 128 kHz
   avr_hvsp_write_hfuse(0xdf);
   avr_hvsp_write_efuse(0xff);

   // exit programming mode
   digitalWrite(RESET_PIN, HIGH);  // pull attiny85 reset pin to ground (arduino nano LED will be on)

   while(TRUE)
   {
	   _delay_ms(100);
       //LED_STRING_1_TRG();
   }
}
