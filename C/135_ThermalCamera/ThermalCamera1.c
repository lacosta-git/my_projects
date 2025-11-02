/*
 * ThermalCamera1.c
 *
 * Created: 2014-02-21 17:40:39
 *  Author: Tomasz Szkudlarek
 */ 

#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include "SystSettings.h"
#include "LocalLib_PORTS.h"
#include "LocalLib_USART.h"
#include "LocalLibT0Settings.h"
#include "LocalLibSendReceive.h"
#include "LocalLib_PWM.h"
#include "LocalLibServo.h"
#include "LocalLibADC.h"

//Global Variables
volatile uint8_t one_sec_counter;

int main(void)
{
	//Variables init
	one_sec_counter = 0;
	servo_off_watchdog = 0;
	adc_update_time = 0;
	
	//System init
	ports_init();
	uart_init();
	t0_timer_init();
	t1_timer_init();
	adc_init();
		
	//Interrupts Enabling
	sei();
	while(1)
    {
		//System is based on interrupts therefore here is no code
    }
}


//Interrupts
ISR(TIMER0_OVF_vect)
	{			
		// 20 ms task
		TCNT0 = T0_20msValue;	//Reload Timer
		
		//Enable T1 Timer
		//No prescaler
		TCCR1B |=(1<<CS10);		
		
		//0,5 Sec Procedure
		one_sec_counter++;
		if (one_sec_counter >= 25)
			{
				YelowLED_TOGLE;
				one_sec_counter = 0;
			}
		
		//Servo power procedure
		if (servo_off_watchdog >= SERVO_OFF_TIME)
			{
				SERVO_PWR_OFF;
			}
		else
			{
				servo_off_watchdog++;
			}
			
		//Temperature update procedure
		if (adc_update_time == INFRA_RED_UPDATE_TIME)
			{
				start_conversion(INFRA_RED_ADC_CHANEL);
			}
		
		if (adc_update_time == TEMPERATURE_UPDATE_TIME)
			{
				start_conversion(TEMPERATURE_ADC_CHANEL);
			}
		if (adc_update_time >= 50)
			{
				adc_update_time = 0;
			}
		else
			{
				adc_update_time++;
			}
	}
