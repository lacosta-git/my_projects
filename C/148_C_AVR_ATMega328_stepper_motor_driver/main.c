/*
 * Stepper Motor Driver
 *
 *  Created on: 26.12.2023
 *      Author: Tomasz Szkudlarek
 */

#include <avr/interrupt.h>
#include "main.h"
#include "rs232/rs232.h"
#include "stepper_motor_drv/stepper.h"
#include "cmd/cmd.h"

// Variables
volatile uint16_t m_timer1_reload = 0xC1B8; // 1ms = 0xFFFF - 16000us + (correction)  for 16 MHz CPU
volatile uint16_t m_temp_counter_2 = 0;


// Texts
const __flash char m_p_intro[] = "\r>> AVR Stepper Motor Driver";


// T0 setup
void t1_init(void)
{
	// Settings for ATMEGA328p
	// Normal Mode - Interrupt at overflow. No prescaler
	TCCR1B = (1<<CS10);
	TCNT1 = m_timer1_reload;
	TIMSK1 = (1<<TOIE1); 		// Enable interrupt at overflow

}

// Main
int main (void){

	// WDT

	// Init GPIB ports
	//SET_INPUTS_DDR();
	SET_OUTPUTS_DDR();

	// EEPROM

	// Counter

	// Interrupts
	t1_init();
	sei();

	// Init rs232
	init_serial_communication(19200);
	rs_send_P(m_p_intro);

	// Initial setup
	MS1_ON();
	MS2_ON();
	MS3_ON();

	EN_M1_ON();
	EN_M2_ON();
	EN_M3_ON();


while(1){
	// ************************* START MAIN LOOP *******************************

	rs_send_handler();
	rs_get_handler();
	c_cmd_handler();

	// ************************** END MAIN LOOP ********************************
}
}


// ********** INTERRUPT ********
ISR(TIMER1_OVF_vect){
	TCNT1 = m_timer1_reload;
	man_stepper_handler();
	auto_stepper_handler();
	c_cmd_tick_ms();
}

