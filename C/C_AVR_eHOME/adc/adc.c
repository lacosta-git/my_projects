/*
 * adc.c
 *
 *  Created on: 17.02.2019
 *      Author: lapiUser
 */

#include "adc.h"

void init_adc(void){
	#ifdef ATMEGA328P
		// ATmega328P setup

	ADMUX |= (1<<REFS0);  // AVCC with external capacitor at AREF pin

	ADCSRB = 0;  // Free running mode
	ADCSRA |= (1<<ADATE); // ADC Auto Trigger Enable
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);  // 100 - 200 kHz required: 125 kHz for F_CPU = 16 MHz
	ADCSRA |= (1<<ADEN);  // Enable ADC
	ADCSRA |= (1<<ADSC);  // Start Conversion

	#endif  // ATMEGA328P
}

uint16_t adc_read(void){
	return ADCW;
}

