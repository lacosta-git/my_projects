/*
 *
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include <stdlib.h>
#include <avr/interrupt.h>
#include "main.h"

// Variables
volatile uint32_t led_loop_counetr = 100;
volatile uint32_t loop_counetr = 0;

volatile uint16_t zero_detection_filter = ZERO_DETECTION_FILTER_COUNTER;
volatile uint16_t welting_time_20ms;
volatile uint16_t welting_time_10ms;

volatile uint8_t bt_START = NOT_PRESSED;
volatile uint8_t bt_start_prssed = FALSE;
volatile uint8_t bt_start_deboucing_counter = BT_DEBOUNCING_COUNTER;

volatile uint8_t beep = FALSE;
volatile uint16_t beep_timer = BEEP_TIME;

// Functions
void init_ports(){
	SETUP_INPUTS();
	LED_STRING_1_DDR();  // Set LED GPIB port as output
	TR1_PORT_OUTPUT_SETUP();
	BEEP_PORT_OUTPUT_SETUP();
}


void init_interrupts(){
	MCUCR |= (1<<ISC01)|(1<<ISC00);
	GICR |= (1<<INT0);
	sei();
}


void init_adc(){
	// Ref voltage: AVCC with external capacitor at AREF pin
	// MUX: ADC0
	// Left adjusted result
	ADMUX = (1<<REFS0)|(1<<ADLAR);

	// ADC Enable
	// ADC start conversion
	// ADC free running
	// Prescaler: 16
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADFR)|(1<<ADPS2);

}

// Main
int main (void){
// Initialization
	// variables
	welting_time_20ms = 10;  // 20ms * value

	// Ports
	init_ports();

	// ADC init
	init_adc();

	// Interrupts
	init_interrupts();

while(1){
	if (zero_detection_filter > 0){
		zero_detection_filter--;
	}

	welting_time_20ms = ADCH >> 3;	// Continues update and divide by 8

	// BEEP
	if (beep_timer > 0){
		beep_timer--;
		BEEP_ON();
	} else {
		BEEP_OFF();
	}
}
}


// ********** INTERRUPT ********
ISR(INT0_vect){
	// Zero Detection filtering
	if (zero_detection_filter > 0) {
		return;
	}
	zero_detection_filter = ZERO_DETECTION_FILTER_COUNTER;

	loop_counetr++;
	if (loop_counetr >= 50) {
		loop_counetr = 0;
		LED_STRING_1_TRG();
	}

	// START welding
	if (bt_start_prssed == TRUE){
		// Enable 5 ms timer delay to enable tafo in Uin = max
		TCNT1 = 0xFFFF - 5700;  // Load 5000 us
		// Timer1 - Normal Operation. No prescaler
		TCCR1B = (1<<CS10);
		// Clear old interrupt request
		TIFR = (1<<TOV1);
		// Enable Interrupt for timer 1
		TIMSK = (1<<TOIE1);

		// Set welting time
		welting_time_10ms = (welting_time_20ms << 1) + 1;   // *2

		// Block continues STARTING
		bt_start_prssed = FALSE;
	}

	// Counting welding time
	if (welting_time_10ms > 0){
		welting_time_10ms--;
	}

	if (welting_time_10ms == 0){
		TR1_OFF();
	}

	// Read buttons
	bt_START = READ_BT_START_PIN();

	// START button
	if (bt_START == NOT_PRESSED){
		bt_start_deboucing_counter = BT_DEBOUNCING_COUNTER;
	} else {
		// Button pressed
		// Blocking continues counting down of counter
		if (bt_start_deboucing_counter <= BT_DEBOUNCING_COUNTER){
			bt_start_deboucing_counter--;
		}

		if (bt_start_deboucing_counter == 0){
			bt_start_prssed = TRUE;  // START PRESSED
		}
	}
}

ISR(TIMER1_OVF_vect){
	// Disable Interrupt for timer 1
	TIMSK = 0;
	TR1_ON();
	beep_timer = BEEP_TIME;
}
