/*
 * webServer.c
 * Created on AVRDUINO based on ATMega329P and Wiznet W5100
 * Created: 2014-12-31 17:05:29
 *  Author: TSzkudl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "pinAsigment.h"
#include "timer0.h"
#include "SPI.h"
#include "wizznetW5100.h"

volatile uint8_t multiplier;
volatile uint32_t highResolutionTemp;
volatile uint32_t avrHighResolutionTemp;
volatile uint32_t adcAvr[] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile uint8_t adcIndex = 0;
extern int tmpOutdor;

/* ADC  */
void adcInit(uint8_t channel){
	// REF = AVCC with external capacitor at AREF pin
	// Right Adjust Result
	ADMUX = ((1<<REFS1)|(1<<REFS0)) + channel;

	// Disable digital inputs for ADC channels 2,1,0
	DIDR0 = (1<<ADC2D)|(1<<ADC1D)|(1<<ADC0D);

	// Free running mode
	ADCSRB = 0;

	// Enable ADC
	// START conversion
	// Auto trigger
	// precaler = Clk/128
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

int main(void)
{
	DDR_B();
	DDR_D();
	t0initialization();
	initializeSPI();
	W5100_Init();
	initializeParametry();
	sei();
	adcInit(2);
    while(1)
    {
       socketTCPStateMachine(0);
       highResolutionTemp = ADCH;
       highResolutionTemp = (highResolutionTemp << 8) + ADCL;
       highResolutionTemp = highResolutionTemp * 1100;
       adcAvr[adcIndex] = (adcAvr[adcIndex] + highResolutionTemp) >> 1;
       // AVR calculation
       avrHighResolutionTemp = 0;
       for (uint8_t i=0; i<=7; i++){
    	   avrHighResolutionTemp = avrHighResolutionTemp + adcAvr[i];
       }
       tmpOutdor = (avrHighResolutionTemp >> 13) - 525;
	    //TODO:: Please write your application code 
	}
}

ISR(TIMER0_OVF_vect)
{
	// 10 ms task
	t0reload();
	multiplier++;
	
	if (multiplier >= 100)
	  {
		LED_Y1_TRG();
		multiplier = 0;
		adcIndex++;
		if (adcIndex >= 8) adcIndex = 0;
	  }
	
}
