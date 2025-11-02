/*
 * 433 MHz
 *
 *  Created on: 31.05.2019
 *      Author: Tomasz Szkudlarek
 */

#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "main.h"
#include "timer0.h"
#include "timer1.h"
#include "rs232/rs232.h"
#include "garage/garage.h"
#include "radio_decode.h"
#include "cmd.h"
#include "my_eeprom.h"
#include "buttons.h"

// Variables
volatile uint8_t m_ms_divider = 0;  // 50us multiplier
volatile uint8_t m_ms_timer = 0;	// ms timer tick. Increased by 1 every 1 ms

extern s_garage garage;
volatile uint8_t m_door_status;
volatile uint8_t m_obstacle_detected;

volatile uint8_t m_reset = FALSE;

// Texts
const __flash char m_p_intro[] = "\r>> AVR 433Mhz driver v2.2";


// Reset by stopping Watch Dog resets
void m_reset_cpu(void){
	wdt_reset();
	wdt_enable(WDTO_2S);
	m_reset = TRUE;

}


// Main
int main (void){

	// WDT
	m_reset = FALSE;  // FALSE = NO reset. Normal operation.
	wdt_reset();
	wdt_enable(WDTO_8S);

	// Init GPIB ports
	SET_INPUTS_DDR();
	SET_OUTPUTS_DDR();

	// EEPROM
	ee_init();
	garage_read_eeprom_settings();

	// Counter
	t1_init();

	// Interrupts
	t0_init();
	sei();

	// Init rs232
	init_serial_communication(19200);
	rs_send_P(m_p_intro);

	// Buttons
	b_buttons_init();

	// Garage init
	garage_init();

	// radio decoder init
	rc_init();

while(1){
	// ************************* START MAIN LOOP *******************************

	if (m_reset == FALSE) wdt_reset();
	read_inputs();
	ms_timer_handler();

	data_pin_handler();
	radio_decode_handler();
	b_buttons_handler();
	garage_handler();
	beep_handler();
	light_handler();
	auto_closing_handler();

	set_outputs();
	rs_send_handler();
	rs_get_handler();
	c_cmd_handler();

	// ************************** END MAIN LOOP ********************************
}
}


// ********** INTERRUPT ********
ISR(TIMER0_COMPA_vect){
	m_ms_divider++;
	if (m_ms_divider >= 20){  // 1000us/50us => 20
		m_ms_divider = 0;
		m_ms_timer++;
	}
}


// IO functions
void read_inputs(void){
// Read inputs
	m_door_status = READ_GATE_PIN();
	m_door_status = ~m_door_status & 0x01;		// Required negation due to HW changes

	if (m_door_status == ON) {
		garage.i_door_status = OPEN;
	} else {
		garage.i_door_status = CLOSED;
	}

	m_obstacle_detected = READ_OBSTACLE_PIN();
	m_obstacle_detected = ~m_obstacle_detected & 0x01;		// Required negation due to HW changes
	if (m_obstacle_detected == TRUE) {
		garage.i_obstacle_detected = TRUE;
	} else {
		garage.i_obstacle_detected = FALSE;
	}
}

void set_outputs(void){
	// Outputs update
	if (garage.o_door_close == ON) O_CLOSE_ON();
	else O_CLOSE_OFF();

	if (garage.o_door_open == ON) O_OPEN_ON();
	else O_OPEN_OFF();

	if (garage.o_beep == ON) O_BEEP_ON();
	else O_BEEP_OFF();

	if (garage.o_light == ON) O_LIGHT_ON();
	else O_LIGHT_OFF();
}


void ms_timer_handler(void){
	if (m_ms_timer > 0){
		m_ms_timer = 0;

		// Call 1 ms functions
		garage_time_tick();
		radio_code_time_tick();
		c_cmd_tick_ms();
		b_buttons_time_tick_ms();
	}
}
