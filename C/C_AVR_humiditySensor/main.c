/*
 * main.c
 *
 *  Created on: 14.08.2017
 *      Author: autoMaster
 */

/*
 * Pinout Arduino UNO R3
 *
 *  Function            Arduino         ATmega
 *  LCD_RS				  D4			  PD4
 *  LDC_RW				  D5			  PD5
 *  LDC_E				  D6			  PD6
 *  LDC_D4				  D8			  PB0
 *  LDC_D5				  D9			  PB1
 *  LDC_D6				  D10			  PB2
 *  LDC_D7				  D11			  PB3
 *
 */


#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"
#import "HD44780.h"

// 1 - Output
// 0 - Input

#define		DDR_B()				DDRB |= (1<<PB5)
#define		LED_STRING_1_TRG()			PORTB ^= (1<<DDB5) // change state

#define		DDR_C()				DDRC = (1<<PC1)|(1<<PC2)|(1<<PC3)	// Set outputs. Rest are inputs
#define		DISCHARGE_CAP()		DIDR0 = (1<<ADC0D); DDRC |=(1<<PC2); PORTC &=~(1<<PC2)  // Set: Out and 0
#define 	ENABLE_CAP()		DIDR0 = (1<<ADC2D)|(1<<ADC0D); DDRC &=~(1<<PC2)				    // Set: Input
#define		RES_33K_LOW()		PORTC &=~(1<<PC1)
#define		RES_33K_HIGH()		PORTC |=(1<<PC1)
#define		RES_HUMI_LOW()		PORTC &=~(1<<PC3)
#define		RES_HUMI_HIGH()		PORTC |=(1<<PC3)

#define		DDR_D()				DDRD |= (1<<PD7)
#define		BACK_LIGHT_TRG()	PORTD ^= (1<<DDD7) // change state
#define		BACK_LIGHT_ON()		PORTD |=(1<<DDD7)
#define		BACK_LIGHT_OFF()	PORTD &=~(1<<DDD7)

// BOOL
#define FALSE 0
#define TRUE 1

// Iteration
volatile uint8_t i = 0;
volatile uint8_t counter = 0;
volatile uint16_t counterHi = 0;
volatile uint16_t counterOld = 0;

// Tasks
volatile uint8_t task1ms = FALSE;
volatile uint8_t task100ms = FALSE;
volatile uint8_t task400ms = FALSE;


// Display
volatile char mTemAndHum[] =
	{'T', 'E', 'M', 'P', ':', ' ', '1', '2', '.', '4', ' ', 223, 'C', ' ', ' ', ' ',
	 'H', 'U', 'M', 'I', ':', ' ', '4', '5', '.', '0', ' ', '%', ' ', ' ', ' ', ' '};

volatile uint8_t decTab[][5] = {
		{0, 0, 0, 0, 1},
		{0, 0, 0, 0, 2},
		{0, 0, 0, 0, 4},
		{0, 0, 0, 0, 8},
		{0, 0, 0, 1, 6},
		{0, 0, 0, 3, 2},
		{0, 0, 0, 6, 4},
		{0, 0, 1, 2, 8},
		{0, 0, 2, 5, 6},
		{0, 0, 5, 1, 2},
		{0, 1, 0, 2, 4},
		{0, 2, 0, 4, 8},
		{0, 4, 0, 9, 6},
		{0, 8, 1, 9, 2},
		{1, 6, 3, 8, 4},
		{3, 2, 7, 6, 8}};

volatile uint32_t highResolutionTemp = 0;
volatile uint32_t highResolutionHumRes = 0;
volatile uint32_t highResolutionHumV = 0;
volatile uint32_t highResolutionHum = 0;
volatile uint8_t humiHigh = FALSE;
volatile uint8_t humiMeasurement = FALSE;

// **********************************************
// **********************************************
// **********        FUNCTIONS           ********
// **********************************************

// **********************************************
// **********    HEX2DEC  & SUM    **************
// **********************************************
// @param dg1, dg2   - Dec didits to add
// @return sum		 - Dec sum with
uint8_t sumDec2digits(uint8_t dg1, uint8_t dg2, uint8_t carry){
	uint8_t decSum = 0;
	decSum = dg1 + dg2 + carry;
	if (decSum > 9){
		decSum = decSum + 6;
	}
	return decSum;
}

// **********************************************
// @param tdg1, tdg2 		- table of dec digits to add
// @param tDecSum			- table of dec sum of digits
void sumDecDigs (uint8_t* tdg1, uint8_t* tdg2, uint8_t* tDecSum){
	uint8_t i = 0;
	uint8_t sum = 0;
	uint8_t crr = 0;
	for (i=5; i>0; i--){
		sum = sumDec2digits(*(tdg1 + i - 1), *(tdg2 + i - 1), crr);
		*(tDecSum + i - 1) = sum & 0x0F;
		crr = sum >> 4;
	}
}

// **********************************************
void hex2dec16bit(uint16_t hexVal, uint8_t* decV){
	uint8_t li = 0;
	for (li=0; li<=15; li++){
		if (((hexVal >> li) & 0x01) == 1){
			sumDecDigs(((uint8_t*)&decTab+li*5) , decV, decV);
		}
	}
}
// **********************************************
// **********************************************
// **********    DISPLAY   2x16    **************
// **********************************************
// Display 2 line Menu
void display2x16lineText(char* text){
	char lineOfText [17];

	LCD_GoTo(0 , 0);
	for (i=0; i<=15; i++){
		lineOfText[i] = *(text + i);
	}
	lineOfText[16] = 0;
	LCD_WriteText((char*)&lineOfText);

	LCD_GoTo(0 , 1);
	for (i=16; i<=31; i++){
		lineOfText[i-16] = *(text + i);
	}
	lineOfText[16] = 0;
	LCD_WriteText((char*)&lineOfText);
}

// **********************************************
void updateTempDigits(uint16_t temp){
	uint8_t decimalValue[] = {0, 0, 0, 0, 0};

	hex2dec16bit(temp, (uint8_t*)&decimalValue);

	*(mTemAndHum + 6) = decimalValue[2] + 0x30;
	*(mTemAndHum + 7) = decimalValue[3] + 0x30;
	*(mTemAndHum + 9) = decimalValue[4] + 0x30;
}

// **********************************************
void updateHumDigits(uint16_t temp){
	uint8_t decimalValue[] = {0, 0, 0, 0, 0};

	hex2dec16bit(temp, (uint8_t*)&decimalValue);

	*(mTemAndHum + 22) = decimalValue[2] + 0x30;
	*(mTemAndHum + 23) = decimalValue[3] + 0x30;
	*(mTemAndHum + 25) = decimalValue[4] + 0x30;
}



/* ADC  */
void adcInit(void){
	// REF = Internal 1.1 V
	// Right Adjust Result
	// Chanel 0
	ADMUX = ((1<<REFS1)|(1<<REFS0));

	// Disable digital inputs for ADC channels 2,0
	DIDR0 = (1<<ADC2D)|(1<<ADC0D);

	// Enable ADC
	// Single conversion mode
	// precaler = Clk/128
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t adcGetChanel(uint8_t channel){
	uint8_t retL = 0;
	uint16_t ret = 0;
	// REF = Internal 1.1 V
	// Right Adjust Result
	ADMUX = ((1<<REFS1)|(1<<REFS0)) + channel;

	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC));

	retL = ADCL;
	ret = ADCH;
	ret = ret << 8;
	ret += retL;

	return ret;
}

// **********************************************
// **********       MAIN PROGRAM         ********
// **********************************************

int main(){
	// Initialization

	// PORTS
	DDR_B();  // Out misc
	DDR_C();
    DDR_D();  // Out DISPLAY. Initialized in HD44780.h

	LCD_Initalize();

	// Interrupts
	t0initialization();
	sei();
	adcInit();
	ENABLE_CAP();

	// ********** MAIN LOOP ********
while (1){

	if (humiMeasurement == TRUE){
		humiHigh = TRUE;
	}

	if (task1ms == TRUE){
		task1ms = FALSE;
		if (humiHigh == TRUE){
			RES_HUMI_HIGH();
			RES_33K_LOW();
			humiHigh = FALSE;
		} else {
			RES_HUMI_LOW();
			RES_33K_HIGH();
			humiHigh = TRUE;
		}
	}

	// ADC
	if (task400ms == TRUE){
		task400ms = FALSE;

		highResolutionTemp = adcGetChanel(0);
		highResolutionTemp = highResolutionTemp * 1090;
		highResolutionTemp = highResolutionTemp >> 10;  // divide by 1024
		highResolutionTemp = highResolutionTemp - 500;  // offset

		if (humiMeasurement == TRUE){
			humiMeasurement = FALSE;
			highResolutionHumV = adcGetChanel(2);
			highResolutionHumV = highResolutionHumV * 1090;
			highResolutionHumV = highResolutionHumV >> 10;  // divide by 1024
			highResolutionHumRes = ((3300 - highResolutionHumV) * 10) / highResolutionHumV;
			highResolutionHumRes = 33 * highResolutionHumRes;
		}

		//highResolutionHum = highResolutionHumV;
		highResolutionHum = highResolutionHumRes;

		// DISPLAY
		updateTempDigits(highResolutionTemp);
		updateHumDigits(highResolutionHum);
		display2x16lineText((char*)&mTemAndHum);

		LED_STRING_1_TRG();
	}
}}

// ********** INTERRUPT ********
ISR(TIMER0_OVF_vect)
{
	// 100 us task
	t0reload();
	counter++;
	if (counter == 5)
	{
		BACK_LIGHT_ON();
		counter = 0;
	}
	else BACK_LIGHT_OFF();

	// 1 ms task
	counterHi++;
	if ((counterHi - counterOld) >= 10){
		task1ms = TRUE;
		counterOld = counterHi;
	}

	if (counterHi >= 3000){
		// Discharge humiCap
		DISCHARGE_CAP();
	}

	if (counterHi >= 3900){
		ENABLE_CAP();
		humiMeasurement = TRUE;
	}

	if (counterHi >= 4000){
		task400ms = TRUE;
		counterHi = 0;
		counterOld = 0;
	}
}
