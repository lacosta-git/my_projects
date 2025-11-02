/*
 * main.c
 *
 *  Created on: 15.05.2018
 *      Author: lapiUser
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// **********************************************
// *********         Defines         ************
// **********************************************
// General
#define FALSE 0
#define TRUE 1

#define ATMEGA328P

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

	// Interrupts
	#define INT_1000us 		TIMER0_OVF_vect
	// Timer reload value for 1000 us
	#define RELOAD	0x05

	// PWM
	#define	OUTPUT_PWM_DDR()	DDRD |= (1<<PD6)
	#define	GND_PWM_DDR()		DDRD |= (1<<PD7)
	#define	GND_PWM_ON()		PORTD |= (1<<PB7)
	#define	GND_PWM_OFF()		PORTD &= ~(1<<PB7)
	#define PWM_REGISTER		OCR0A

	// INPUT
	#define PIN_1				PC0
	#define INPUT_1_DDR()		DDRC &= ~(1<<PIN_1); PORTC |=(1<<PIN_1)  // INPUT with pull up
	#define READ_INPUT_1()		PINC&(1<<PIN_1)>>PIN_1

#endif // ATMEGA328P



// **********************************************
// ***********      Variables    ****************
// **********************************************
volatile uint8_t c_pwm_min;  // % [0..100]
volatile uint8_t c_pwm_max;  // % [0..100]
volatile uint8_t c_pwm_current;  // % [0..100] - current pwm
volatile uint8_t c_pwm_100_factor;  // Register max - min / 100% *100
volatile uint16_t i_int_divider;  // Timer interrupt divider
volatile int i_pwm_step;

volatile uint16_t counter;

// Input
volatile uint8_t in_switch;
volatile uint8_t in_last_value;
volatile uint8_t in_sin_active;
// **********************************************
// ***********      Functions    ****************
// **********************************************

// **********************************************
void var_init(void){
	counter = 0;
	// PWM
	c_pwm_min = 0;
	c_pwm_max = 100;
	c_pwm_current = c_pwm_min;
	c_pwm_100_factor = 0xFF - RELOAD;
	i_pwm_step = 1;

	// Interrupt
	i_int_divider = 0;

	// Input
	in_switch = 0;
	in_last_value = 0;
	in_sin_active = FALSE;
}

// **********************************************
void _1000usReload(void)
{
	//reload timer
	#ifdef ATMEGA328P
		TCNT0 = RELOAD;
	#endif
}

// **********************************************
void int_init1000us(void)
{
	// 1000 us overflow time
	// CPU = 16 MHz
	// 16000000/1000 = 16000 [1000 us = 1 ms]
	// 16000/64 = 250 -> prescaler = 64
	// 256 - 250 = 5 => 0x05  -> RELOAD
	// Fast PWM mode 7 ->  WGM2:0 = 3
	// 0A - inverting output
	// OCR0A <= 200 - Default

	#ifdef ATMEGA328P
		TCCR0A = (1<<COM0A1)|(1<<COM0A0)|(1<<COM0B1)|(1<<COM0B0)|(1<<WGM01)|(1<<WGM00);
		TCCR0B = (0<<WGM02)|(0<<CS02)|(1<<CS01)|(1<<CS00);
		OCR0A = 0xC8;  // 200 DEFAULT
		_1000usReload();

		//T0 Interrupt Enable
		TIMSK0 = _BV(TOIE0);
	#endif
}

// **********************************************
// 1000 us interrupt handler
void int_1000usHandler(void){
	// 1000 us task
	_1000usReload();

	// 1 sec LED blinking
	counter++;
	if (counter>=1000){
		counter = 0;
		LED_STRING_1_TRG();
	}

	i_int_divider++;
	// PWM update every 10 ms (1ms * 10)
	if (i_int_divider >= 10){
		i_int_divider = 0;

		// Reading input
		in_switch = READ_INPUT_1();

		// Falling edge detection
		if ((in_last_value == 1)&
			(in_switch == 0)){
			in_sin_active = TRUE;
			c_pwm_current = c_pwm_min;
			i_pwm_step = 1;
		}
		// Save last value of input signal
		in_last_value = in_switch;

		// PWM changing procedure
		if (in_sin_active == TRUE){
			c_pwm_current += i_pwm_step;
		} else {
			c_pwm_current = c_pwm_min;
			i_pwm_step = 1;
		}

		if (c_pwm_current >= (c_pwm_max - 1)) {
			i_pwm_step = -1;
		}
		if (c_pwm_current <= c_pwm_min) {
			in_sin_active = FALSE;
		}
	}
}

// **********************************************
void pwm_handler(){

	PWM_REGISTER = 0xFF - (uint8_t)((c_pwm_current * c_pwm_100_factor) / 100);
}
// **********************************************
// **********       MAIN LOOP            ********
// **********************************************
int main(){
	// Port init
	LED_STRING_1_DDR();
	OUTPUT_PWM_DDR();
	GND_PWM_DDR();
	GND_PWM_OFF();
	INPUT_1_DDR();

	// Variables init
	var_init();

	// Interrupts
	int_init1000us();
	sei();

while(TRUE){
	pwm_handler();
}}
// **********************************************
// **********************************************

// ********** INTERRUPT ********
ISR(INT_1000us){
	int_1000usHandler();
}
