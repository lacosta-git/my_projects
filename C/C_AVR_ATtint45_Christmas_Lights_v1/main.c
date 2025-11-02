/*
 * main.c
 *
 *  Created on: 12.03.2019
 *      Author: lapiUser
 */

#include "main.h"
#include <avr/interrupt.h>

// Variables
volatile uint16_t m_10ms_counter;

volatile uint8_t m_bt_b_counter = 0;
volatile uint8_t m_bt_d_counter = 0;

volatile uint8_t m_bt_b_pressed = FALSE;
volatile uint8_t m_bt_d_pressed = FALSE;


volatile uint8_t m_led_string_mode[] = {
										M_MODE_0_PWM,
										M_MODE_1_PWM,
										M_MODE_2_PWM,
										M_MODE_3_PWM,
										M_MODE_4_PWM};
volatile uint8_t m_led_string_mode_index = 0;

volatile uint8_t m_mode_pwm_counter = 0;
volatile uint8_t m_leds = OFF;
volatile uint8_t m_leds_string = 0;

// Main loop
int main(void)
{
	// Ports setup
	LED_STRING_1_DDR();
	LED_STRING_2_DDR();

	SETUP_INPUTS();

	// Interrupts
	// CTC Mode - Clear Timer on Compare match
	// Settings for ATTiny45
	TCCR0A |= (1<<WGM01);  // CTC mode
	TCCR0B |= (1<<CS01);   // Clock source Clk_io/8
	OCR0A = 124;  // 1MHz/8 => 125 kHz/(124+1) => 1 kHz => 1 [ms]

	// Enabling interrupt
	TIMSK |= (1<<OCIE0A);				// Interrupt when compare match
	sei();

	// Variables
	m_10ms_counter = M_DIVIDER_10MS;

  while(1){
  // Main loop code here
	  // Button B
	  if (m_bt_b_counter > 8){  // Debouncing
		  if (m_bt_b_pressed == FALSE){
			  m_bt_b_pressed = TRUE;
			  if (m_led_string_mode_index < 4){
				  m_led_string_mode_index++;  // Change mode
			  }
		  }
	  }
	  if (m_bt_b_counter < 2){
		  m_bt_b_pressed = FALSE;
	  }

	  // Button D
	  if (m_bt_d_counter > 8){  // Debouncing
		  if (m_bt_d_pressed == FALSE){
			  m_bt_d_pressed = TRUE;
			  if (m_led_string_mode_index > 0) {
				  m_led_string_mode_index--;  // Change mode
			  }
		  }
	  }
	  if (m_bt_d_counter < 2){
		  m_bt_d_pressed = FALSE;
	  }
  }
}

// 1 msec task
void m_1ms_task(void){
	uint8_t m_pwm;

	//  PWM setting
	m_pwm = m_led_string_mode[m_led_string_mode_index];
	if (m_mode_pwm_counter < m_pwm){
		m_leds = ON;
	} else {
		m_leds = OFF;
	}
	if (m_mode_pwm_counter < M_MAX_PWM_COUNTER){
		m_mode_pwm_counter++;
	} else {
		m_mode_pwm_counter = 0;
	}

	// Alternating enabling of LEDs strings
	if (m_leds_string > 0){
		m_leds_string = 0;
		LED_STRING_1_OFF();
		if (m_leds == ON){
			LED_STRING_2_ON();
		} else LED_STRING_2_OFF();

	} else {
		m_leds_string = 1;
		LED_STRING_2_OFF();
		if (m_leds == ON){
			LED_STRING_1_ON();
		} else LED_STRING_1_OFF();
	}
}

// 10 msec task
void m_10ms_task(void){
	uint8_t m_button = 0;
	m_button = READ_BT_B_PIN();

	if (m_button == TRUE){
		if (m_bt_b_counter < 10) m_bt_b_counter++;
	} else {
		if (m_bt_b_counter > 0) m_bt_b_counter--;
	}

	m_button = READ_BT_D_PIN();

	if (m_button == TRUE){
		if (m_bt_d_counter < 10) m_bt_d_counter++;
	} else {
		if (m_bt_d_counter > 0) m_bt_d_counter--;
	}
}

// ********** INTERRUPT ********
ISR(TIMER0_COMPA_vect){

	m_1ms_task();  // 1 ms task

	if (m_10ms_counter > 0) {
		m_10ms_counter--;
	} else {
		m_10ms_counter = M_DIVIDER_10MS;
		m_10ms_task();  // 10 ms task
	}
}
