/*
 * LocalLibADC.h
 *
 * Created: 2014-03-12 10:38:07
 *  Author: Tomasz Szkudlarek
 */ 


#ifndef LOCALLIBADC_H_
#define LOCALLIBADC_H_

#define TEMPERATURE_ADC_CHANEL		1
#define INFRA_RED_ADC_CHANEL		2

#define TEMPERATURE_UPDATE_TIME		49
#define INFRA_RED_UPDATE_TIME		46
#define NUMBER_OF_AVR_SAMPLES		8

volatile uint8_t numberOfSamplesForAverage;
volatile uint8_t adc_update_time;
volatile uint16_t infra_red_raw_value;
volatile uint16_t temperature_raw_value;
volatile uint16_t adc_raw_value;

volatile uint8_t tempADMUX;

void adc_init(void);
void start_conversion(uint8_t);

#endif /* LOCALLIBADC_H_ */