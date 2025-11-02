/*
 * EngineHeater1.c
 *
 * Created: 2014-09-19 08:05:32
 *  Author: Tomasz Szkudlarek
 *
 * 	 10 ms TIME_CLK - tick
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "LocalLib_PORTS.h"

#define T0_10ms_prescaler_1024	0xB1   // T0 = 0xFF - 80 000(10 ms)/1024 = 0xFF - 0x4E = 0xB1

#define CHARGING		0
#define BUTTON_ON		1
#define ENGINE_HOT		2

// HEATERS Status
#define HEATER_1		0
#define HEATER_2		1
#define HEATER_3		2
#define HEATER_4		3

// Button LED status
#define OFF				0
#define DISABLING		1
#define ENABLED			5
#define BUT_LED_PATERN_LENGHT   24
#define BUTTON_LED_DIVIDER		25		// *10ms = 250 ms

// SIGNAL
typedef struct {
	volatile uint16_t signal;			// signal variable
	volatile uint16_t minimum;			// Min value that can be reached by signal
	volatile uint16_t maximum;			// Max value that can be reached by signal
	volatile uint16_t start;			// START value for signal
	volatile uint16_t hiLevel;			// if signal >= HI -> output = 1
	volatile uint16_t loLevel;			// if signal <= LO -> output = 0
	volatile uint16_t deltaIncrease;	// signal increase value
	volatile uint16_t deltaDecrease;	// signal decrease value
	volatile uint8_t  timeTick;			// Temporary variable used in clock divider
	volatile uint8_t  clkDivider;		// Time tick divider	
	volatile uint8_t  input;			// signal input state
	volatile uint8_t  output;			// signal output state
	volatile uint8_t  h2l;				// High to Low transition detected
	volatile uint8_t  l2h;				// Low to High transition detected
	} SIG;
SIG signal[3];

volatile uint16_t ignition;			// Time since START of the system. IGNITION enabled.
volatile uint8_t redLED_divider;	// variable used for blinking system LED
volatile uint8_t generalEnable;		// HEATERS all conditions fulfilled. Enabling allowed
volatile uint16_t heaterCounter;	// variable used for delay between en/dis heaters
volatile uint8_t  buttonSignalOn;	// Variable represents BUTTON ON SIG
volatile uint8_t  buttonLedStatus;  // Button LED status - ENABLED, DISABLING, OFF
volatile uint8_t  buttonLedControll[BUT_LED_PATERN_LENGHT];  // Button LED pattern
volatile uint8_t  buttonLedControllIndex;
volatile uint8_t  buttonTimeTick;	// Variable used for time button time tick divider
volatile uint8_t  heaterStatus[4];	// Status of HEATERs ENABLED, OFF

volatile uint16_t tempindex;		// Temporary variable used in iterations
volatile uint8_t tempValue;			// 
volatile uint8_t i;					//

int main(void)
{
    ports_init();  // PROTs and PINs initialization
	
	// Timer 0 settings
	// Prescaler = 1024
	TCCR0 = _BV(CS02)|_BV(CS00);	// Prescaler = 1024
	TCNT0 = T0_10ms_prescaler_1024;
	TIMSK = _BV(TOIE0);				// Enable interrupts for T0
	
	signal[CHARGING].signal =  0;
	signal[CHARGING].minimum = 0;
	signal[CHARGING].maximum = 100;
	signal[CHARGING].start = signal[CHARGING].minimum;
	signal[CHARGING].hiLevel = 80;
	signal[CHARGING].loLevel = 20;
	signal[CHARGING].deltaIncrease = 1;
	signal[CHARGING].deltaDecrease = 2;
	signal[CHARGING].timeTick = 0;
	signal[CHARGING].clkDivider = 10;
	signal[CHARGING].output = 0;
	signal[CHARGING].h2l = 0;
	signal[CHARGING].l2h = 0;
	
	signal[BUTTON_ON].signal =  0;
	signal[BUTTON_ON].minimum = 0;
	signal[BUTTON_ON].maximum = 15;
	signal[BUTTON_ON].start = signal[BUTTON_ON].minimum;
	signal[BUTTON_ON].hiLevel = 10;
	signal[BUTTON_ON].loLevel = 2;
	signal[BUTTON_ON].deltaIncrease = 1;
	signal[BUTTON_ON].deltaDecrease = 1;
	signal[BUTTON_ON].timeTick = 0;
	signal[BUTTON_ON].clkDivider = 1;
	signal[BUTTON_ON].output = 0;
	signal[BUTTON_ON].h2l = 0;
	signal[BUTTON_ON].l2h = 0;
	
	signal[ENGINE_HOT].signal =  0;
	signal[ENGINE_HOT].minimum = 0;
	signal[ENGINE_HOT].maximum = 100;
	signal[ENGINE_HOT].start = signal[ENGINE_HOT].minimum;
	signal[ENGINE_HOT].hiLevel = 80;
	signal[ENGINE_HOT].loLevel = 20;
	signal[ENGINE_HOT].deltaIncrease = 1;
	signal[ENGINE_HOT].deltaDecrease = 2;
	signal[ENGINE_HOT].timeTick = 0;
	signal[ENGINE_HOT].clkDivider = 100;
	signal[ENGINE_HOT].output = 0;
	signal[ENGINE_HOT].h2l = 0;
	signal[ENGINE_HOT].l2h = 0;
	
	ignition = 5000;
	heaterCounter = 0;
	buttonSignalOn = 0;
	buttonLedStatus = OFF;
	for (tempindex = 0; tempindex < BUT_LED_PATERN_LENGHT; tempindex++)
		{
			buttonLedControll[tempindex] = 0;
		}
	for (tempindex = 0; tempindex < 4; tempindex++)
	{
		heaterStatus[tempindex] = OFF;
	}
	
	tempValue = 0;
	buttonTimeTick = 0;
	buttonLedControllIndex = 0;
	sei();
	while(1)
    {
        // All code is done in 10ms INTERRUPT 
    }
}

void outputFiltering(uint8_t VARIABLE, uint8_t input)
{
	// input reading
	signal[VARIABLE].input = input;
	
	// CLK DIVIDER
	signal[VARIABLE].timeTick ++;
	if (signal[VARIABLE].timeTick >= signal[VARIABLE].clkDivider)
		{
			signal[VARIABLE].timeTick = 0;
			
			// MIN MAX
			if (signal[VARIABLE].input == 1)
				{
					if ((signal[VARIABLE].maximum - signal[VARIABLE].signal) <= signal[VARIABLE].deltaIncrease)	
						{
							signal[VARIABLE].signal = signal[VARIABLE].signal + signal[VARIABLE].deltaIncrease;
						}
					else
						{
							signal[VARIABLE].signal = signal[VARIABLE].maximum;
						}
				}
			else
				{
					if ((signal[VARIABLE].signal - signal[VARIABLE].minimum) >= signal[VARIABLE].deltaDecrease)
						{
							signal[VARIABLE].signal = signal[VARIABLE].signal - signal[VARIABLE].deltaDecrease;
						}
					else
						{
							signal[VARIABLE].signal = signal[VARIABLE].minimum;
						}
					
				}
			// Hysteresis
			if (signal[VARIABLE].signal <= signal[VARIABLE].loLevel)
				{
					if (signal[VARIABLE].output == 1)
						{
							signal[VARIABLE].output = 0;
							signal[VARIABLE].h2l = 1;
							signal[VARIABLE].l2h = 0;
						}
				}
			
			if (signal[VARIABLE].signal >= signal[VARIABLE].hiLevel)
				{
					if (signal[VARIABLE].output == 0)
						{
							signal[VARIABLE].output = 1;
							signal[VARIABLE].h2l = 0;
							signal[VARIABLE].l2h = 1;
						}
				}
		}
}

uint8_t readPin(uint8_t VARIABLE)
{
	if (VARIABLE == CHARGING)
		{
			if (PIND & (1<<PIND7)) {return 1;}
			else {return 0;}
		}
		
	if (VARIABLE == BUTTON_ON)
		{
			if (!(PIND & (1<<PIND6))) {return 1;}
			else {return 0;}
		}
	
	if (VARIABLE == ENGINE_HOT)
		{
			if (PINB & (1<<PINB0)) {return 0;}
			else {return 1;}
		}
	return 0;
}

ISR(TIMER0_OVF_vect)
{
	// 10ms INTERRUPT
	TCNT0 = T0_10ms_prescaler_1024;  // T0 Timer reload
	
	outputFiltering(CHARGING, readPin(CHARGING));
	outputFiltering(BUTTON_ON, readPin(BUTTON_ON));
	outputFiltering(ENGINE_HOT, readPin(ENGINE_HOT));
	
	// BUTTON_SIG_ON
	if (signal[BUTTON_ON].l2h >= 1)
		{
			if (buttonSignalOn >= 1)
				{
					// HEATING OFF
					buttonSignalOn = 0;
					signal[BUTTON_ON].l2h = 0;
					buttonLedStatus = DISABLING;
					buttonTimeTick = 0;
					buttonLedControllIndex = 0;
				}	
			else
				{
					// HEATING ON
					buttonSignalOn = 1;
					signal[BUTTON_ON].l2h = 0;
					buttonLedStatus = ENABLED;
					buttonTimeTick = 0;
					buttonLedControllIndex = 0;
				}		
		}
	
	if ((buttonLedStatus == DISABLING) & (heaterCounter < 1000))
		{
			buttonLedStatus = OFF;
		}
		
	
	
	ignition++;						// Time since system START *10ms	
	
	// HEATERS CONTROL
	if (ignition >= 6000)
		{
			//generalEnable = signal[CHARGING].output + buttonSignalOn + signal[ENGINE_HOT].output;
			generalEnable = buttonSignalOn + 2;
			ignition = 6000;		// STOP counting to not over flow counter
		}
		
	if (generalEnable >= 3)
		{
			if (heaterCounter < 4100)
				{
					heaterCounter++;
				}
		}
	else 
		{
			if (heaterCounter > 0) 
				{
					heaterCounter--;
				}
		}
	if (heaterCounter >= 1000){HEATER_1_ON; heaterStatus[HEATER_1] = ENABLED;}
	else {HEATER_1_OFF; heaterStatus[HEATER_1] = OFF;}
	if (heaterCounter >= 2000){HEATER_2_ON; heaterStatus[HEATER_2] = ENABLED;}
	else {HEATER_2_OFF; heaterStatus[HEATER_2] = OFF;}
	if (heaterCounter >= 3000){HEATER_3_ON; heaterStatus[HEATER_3] = ENABLED;}
	else {HEATER_3_OFF; heaterStatus[HEATER_3] = OFF;}
	if (heaterCounter >= 4000){HEATER_4_ON; heaterStatus[HEATER_4] = ENABLED;}
	else {HEATER_4_OFF; heaterStatus[HEATER_4] = OFF;}
	
	
	// BUTTON LED PATTERN
	if (buttonLedStatus == OFF)
		{
			for (tempindex = 0; tempindex < 8; tempindex++)
			{
				buttonLedControll[tempindex] = 0;
			} 
		}
	
	if (buttonLedStatus == DISABLING)
		{
			for (tempindex = 0; tempindex < 8; tempindex = tempindex + 2)
			{
				buttonLedControll[tempindex] = 1;
				buttonLedControll[tempindex+1] = 0;
			}
		}

	if (buttonLedStatus == ENABLED)
	{
		for (tempindex = 0; tempindex < 8; tempindex++)
		{
			buttonLedControll[tempindex] = 1;
		}
	}
	
	// heaters
	for (tempindex = 0; tempindex < 4; tempindex++)
	{
		if (heaterStatus[tempindex] == ENABLED){tempValue = 1;}
		else {tempValue = 0;}
		
		for (i=(8 + tempindex * 4); i < ((8 + tempindex * 4) + 4); i++)
			{
				buttonLedControll[i] = tempValue;
			}
	}
	
	// BUTTON LED CONTROLL
	buttonTimeTick++;
	if (buttonTimeTick >= BUTTON_LED_DIVIDER)
		{
			buttonTimeTick = 0;
			buttonLedControllIndex++;
			if (buttonLedControllIndex >= BUT_LED_PATERN_LENGHT){buttonLedControllIndex = 0;}
		}
	
	if ((buttonLedControll[buttonLedControllIndex] == 1) | (readPin(BUTTON_ON) == 1))
		{
			BUTTON_LED_ON;
		}
	else
		{
			BUTTON_LED_OFF;
		}
	
	redLED_divider++;
	if (redLED_divider >= 25)
		{
			RED_LED_TOGLE;
			redLED_divider = 0;
		}
}