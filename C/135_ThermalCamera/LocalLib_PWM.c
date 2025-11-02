/*
 * LocalLib_PWM.c
 *
 * Created: 2014-03-03 15:12:44
 *  Author: Tomasz Szkudlarek
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "LocalLib_PWM.h"
void t1_timer_init(void)
{
	//Timer T1 is set to work in Fast PWM mode 14
	//Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at BOTTOM (inverting mode)
	//No clock source (Timer/Counter STOPPED)
	TCCR1A = (1<<COM1A0)|(1<<COM1A1)|(1<<COM1B0)|(1<<COM1B1)|(0<<WGM10)|(1<<WGM11);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(0<<CS12)|(0<<CS11)|(0<<CS10);
	
	//Initial values loading
	//Set TOP value for T1
	ICR1 = T1_TOP_VALUE_2ms;
	
	//OC1A output set PWM ON_TIME
	OCR1A = T1_TOP_VALUE_1ms + 4000;  //1,5 ms
	
	//OC1B output set PWM ON_TIME
	OCR1B = T1_TOP_VALUE_1ms + 4000;  //1,5 ms

	//Clear T1 current value
	TCNT1 = 0;
	
	//Enable interrupt from T1.
	TIMSK1 = (1<<TOIE1);
}

ISR(TIMER1_OVF_vect)
{
	//Disable Timer
	TCCR1B &=~((1<<CS12)|(1<<CS11)|(1<<CS10));
	//Clear T1 current value
	TCNT1 = 0;
}