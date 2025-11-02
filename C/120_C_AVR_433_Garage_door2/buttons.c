/*
 * buttons.c
 *
 *  Created on: 25.12.2020
 *      Author: lapiUser
 */

#include "main.h"
#include "buttons.h"
#include "garage/garage.h"

// Variables
volatile uint8_t b_read_divider;
volatile uint8_t b_sw_up;
volatile uint8_t b_sw_down;

extern s_garage garage;

volatile uint8_t b_sw_up_pressed_old;
volatile uint8_t b_sw_down_pressed_old;
volatile uint8_t b_sw_up_down_pressed;
volatile uint8_t b_sw_up_down_pressed_old;
volatile uint8_t b_sw_up_and_down_filters_counting;

// Functions

void b_buttons_init(){
	b_read_divider = B_READ_DIVIDER;
	b_sw_up = 0;
	b_sw_down = 0;
	garage.i_sw_up_pressed = FALSE;
	b_sw_up_pressed_old = FALSE;
	garage.i_sw_down_pressed = FALSE;
	b_sw_down_pressed_old = FALSE;
	b_sw_up_down_pressed = FALSE;
	b_sw_up_down_pressed_old = FALSE;
	b_sw_up_and_down_filters_counting = FALSE;
}

void b_buttons_handler(void){

	// Input filtering and reading
	if (b_read_divider == 0){  // Update status on every divided time
		b_read_divider = B_READ_DIVIDER;
		if (READ_SW_UP_PIN() == B_PRESSED){
			if (b_sw_up < B_PRESSED_FILTER){
				b_sw_up++;
			}
		} else if (b_sw_up > 0){
			b_sw_up--;
		}

		if (READ_SW_DOWN_PIN() == B_PRESSED){
			if (b_sw_down < B_PRESSED_FILTER){
				b_sw_down++;
			}
		} else if (b_sw_down > 0){
			b_sw_down--;
		}
	}

	// Pressed buttons handling
	// UP
	if (b_sw_up == B_PRESSED_FILTER){
		garage.i_sw_up_pressed = TRUE;
		if ((b_sw_up_pressed_old == FALSE)&&
			(b_sw_up_and_down_filters_counting == FALSE)){
			b_sw_up_pressed_old = TRUE;
			// Rising edge of pressed UP button
			garage_button_pressed(BUTTON_OPEN);  // OPEN button pressed
		}
	} else if (b_sw_up == 0){
		garage.i_sw_up_pressed = FALSE;
		b_sw_up_pressed_old = FALSE;
	}

	// DOWN
	if (b_sw_down == B_PRESSED_FILTER){
		garage.i_sw_down_pressed = TRUE;
		if ((b_sw_down_pressed_old == FALSE)&&
			(b_sw_up_and_down_filters_counting == FALSE)){
			b_sw_down_pressed_old = TRUE;
			// Rising edge of pressed DOWN button
			garage_button_pressed(BUTTON_CLOSE);  // CLOSE button pressed
		}
	} else if (b_sw_down == 0){
		garage.i_sw_down_pressed = FALSE;
		b_sw_down_pressed_old = FALSE;
	}

	// If two buttons pressed than STOP action for first one
	if ((b_sw_up > 0)&&(b_sw_down > 0)){
		b_sw_up_and_down_filters_counting = TRUE;
	} else b_sw_up_and_down_filters_counting = FALSE;

	// UP and DOWN
	if ((garage.i_sw_up_pressed == TRUE)&&
		(garage.i_sw_down_pressed == TRUE)){
		b_sw_up_down_pressed = TRUE;
		if (b_sw_up_down_pressed_old == FALSE){
			b_sw_up_down_pressed_old = TRUE;
			// Rising edge of pressed UP and DOWN button
			garage_toggle_auto_closing();
		}
	} else if ((garage.i_sw_up_pressed == FALSE)&&
			   (garage.i_sw_down_pressed == FALSE)){
		b_sw_up_down_pressed = FALSE;
		b_sw_up_down_pressed_old = FALSE;
	}

}

void b_buttons_time_tick_ms(void){
	if (b_read_divider > 0) b_read_divider--;
}
