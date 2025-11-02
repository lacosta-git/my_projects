/*
 * 433 MHz
 *
 *  Created on: 31.05.2019
 *      Author: Tomasz Szkudlarek
 */

#include <avr/interrupt.h>
#include <stdlib.h>
#include "main.h"
#include "timer0.h"
#include "rs232/rs232.h"

// BIT DETECTION
#define CODE_IDLE 	0
#define CODE_COUNT_LOW 	2
#define CODE_BIT_DETECT 10

volatile uint8_t gpio_data;
volatile uint8_t gpio_data_old;
volatile uint8_t bd_counter;
volatile uint8_t sm_code_detect;
volatile uint8_t falling_edge_detected;
volatile uint8_t rising_edge_detected;
volatile char code[40];
volatile uint8_t decoding;
volatile uint8_t c_index;

uint16_t interuptCounter;
char string[20];

// Functions

// Main
int main (void){
// Initialization
	// variables
	sm_code_detect = CODE_IDLE;
	gpio_data = TRUE;
	gpio_data_old = FALSE;
	falling_edge_detected = FALSE;
	rising_edge_detected = FALSE;

	// Init GPIB ports
	SET_INPUTS_DDR();
	SET_OUTPUTS_DDR();
	GND_ON();
	PWR_ON();

	// Interrupts
	t0initialization();
	interuptCounter = 1000;
	sei();

	// Init rs232
	init_serial_communication(9600);
	rs_send("\r>> AVR 433Mhz driver v0.1");

while(1){
	// ************************* START MAIN LOOP *******************************
	rs_send_handler();


	switch (sm_code_detect){
	case CODE_IDLE:
		if (falling_edge_detected == TRUE){
			sm_code_detect = CODE_COUNT_LOW;
		}
		break;

	case CODE_COUNT_LOW:
		if (rising_edge_detected == TRUE){
			if (bd_counter >= 90){		// SYNC BIT DETECTED
				bd_counter = 0;
				sm_code_detect = CODE_BIT_DETECT;

//				rs_send("\r");
//				rs_send((char*)&code);

				for (uint8_t i=0; i<40; i++){
					code[i] = 0;
				}

			} else {
				bd_counter = 0;
				sm_code_detect = CODE_IDLE;
			}
			rising_edge_detected = FALSE;
		}
		break;

	case CODE_BIT_DETECT:
		decoding = TRUE;
		c_index = 0;
		while (decoding == TRUE){
			if (rising_edge_detected == TRUE){
				if ((bd_counter >= 2) &
					(bd_counter < 5)){			// 0
					code[c_index] = '0';
				} else if ((bd_counter >= 7) &
						(bd_counter < 13)){		// 1
						code[c_index] = '1';
				} else {
					decoding = FALSE;
					sm_code_detect = CODE_IDLE;
				}
				c_index++;
				bd_counter = 0;
				rising_edge_detected = FALSE;
			}
		}
		break;
	default:
		sm_code_detect = CODE_IDLE;
		break;
	}


	// ************************** END MAIN LOOP ********************************
//	if (gpio_data == 1) {
//		LED_ON();
//	} else LED_OFF();
}
}

// ********** INTERRUPT ********
ISR(TIMER0_OVF_vect)
{
	// 100 us task
	t0reload();
	interuptCounter--;
	if (interuptCounter == 0){
		//LED_TRG();
		interuptCounter = 10000;

//		// RS message preparation
//		itoa(gpio_data, string, 10);
//		rs_send("\r>> DATA = ");
//		rs_send((char*)&string);
	}

	gpio_data = READ_DATA_PIN();

	if ((gpio_data == FALSE) &
		(gpio_data_old == TRUE)){
		falling_edge_detected = TRUE;
	} else falling_edge_detected = FALSE;

	if ((gpio_data == TRUE) &
		(gpio_data_old == FALSE)){
		rising_edge_detected = TRUE;
	} else rising_edge_detected = FALSE;

	gpio_data_old = gpio_data;

	if (gpio_data == FALSE){
		if (bd_counter < 250){
			bd_counter++;
		}
	}
}
