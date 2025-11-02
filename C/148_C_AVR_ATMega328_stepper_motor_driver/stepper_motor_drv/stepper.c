/*
 * stepper.c
 *
 *  Created on: 28 gru 2023
 *      Author: lapiUser
 */


#include "../main.h"
#include "stepper.h"


// Global variables
volatile uint8_t m_direction = ST_DIR_IN;
volatile uint16_t m1_step_counter = 0;
volatile uint16_t m2_step_counter = 0;
volatile uint16_t m3_step_counter = 0;



// Manual movement of stepper motors
void man_stepper_handler(void){
	if (READ_M_DIR_PIN() == BT_PRESSED){
		DIR_LO();
	} else {
		DIR_HI();
	}

	if (READ_M_STEP_M1_PIN() == BT_PRESSED){
		STEP_M1_TOG();
	}

	if (READ_M_STEP_M2_PIN() == BT_PRESSED){
		STEP_M2_TOG();
	}

	if (READ_M_STEP_M3_PIN() == BT_PRESSED){
		STEP_M3_TOG();
	}
}


// Auto movement of stepper motors
void auto_stepper_handler(void){
	if (m_direction == ST_DIR_IN){
		DIR_LO();
	} else {
		DIR_HI();
	}

	if (m1_step_counter > 0){
		STEP_M1_TOG();
		m1_step_counter--;
	}

	if (m2_step_counter > 0){
		STEP_M2_TOG();
		m2_step_counter--;
	}

	if (m3_step_counter > 0){
		STEP_M3_TOG();
		m3_step_counter--;
	}
}


// Stepper move by x steps
void stepper_move(uint8_t s_motor, uint8_t s_direction, uint16_t s_steps){

	m_direction = s_direction;
	if (s_motor == 1){
		m1_step_counter = s_steps * 2;
	}

	if (s_motor == 2){
		m2_step_counter = s_steps * 2;
	}

	if (s_motor == 3){
		m3_step_counter = s_steps * 2;
	}
}
