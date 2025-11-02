/*
 * LocalLibADC.c
 *
 * Created: 2014-03-12 10:40:44
 *  Author: Tomasz Szkudlarek
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "LocalLibSendReceive.h"
#include "LocalLibADC.h"

void adc_init()
	{
		//Vref = 1.1V + external capacitor on Vref pin
		//ADC result is right adjusted
		// GND channel selected
		ADMUX = (1<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);
		
		//ADC enable
		//Single run
		//Interrupts enabled
		//Prescaler for ADC = Fcpu/64
		//125 kHz for 8MHz F_CPU
		ADCSRA = (1<<ADEN)|(0<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);
		
		//Disable digital logic on analog inputs
		DIDR0 = (1<<ADC1D)|(1<<ADC2D);
	}

//Start conversion
void start_conversion(uint8_t chanel)
{	
	//Chanel selection
	tempADMUX = ADMUX;
	tempADMUX &= 0xF0;
	ADMUX = tempADMUX + chanel;
	
	//Start first conversion
	numberOfSamplesForAverage = NUMBER_OF_AVR_SAMPLES;
	adc_raw_value = 0;
	ADCSRA |= (1<<ADSC);
}

//Interrupt on Conversion complete
ISR(ADC_vect)
{
	tempADMUX = ADMUX;
	tempADMUX &= 0x0F;
	
	
	// Calculate average
	numberOfSamplesForAverage--;
	if (numberOfSamplesForAverage == 0)
		{
			adc_raw_value = (adc_raw_value + (uint8_t)ADCL+ (uint8_t)ADCH*0x100)/NUMBER_OF_AVR_SAMPLES;
			
			if (tempADMUX == TEMPERATURE_ADC_CHANEL)
				{
					temperature_raw_value = adc_raw_value;
				}
			
			if (tempADMUX == INFRA_RED_ADC_CHANEL)
				{
					infra_red_raw_value = adc_raw_value;
				}
			
			// Send Status
			sendHVPosInfraredThemperature(horizontalPosMinus1, verticalPosMinus1, infra_red_raw_value, temperature_raw_value);
		}
	else
		{
			adc_raw_value = adc_raw_value + (uint8_t)ADCL+ (uint8_t)ADCH*0x100;
			//Start next conversion
			ADCSRA |= (1<<ADSC);
		}
}