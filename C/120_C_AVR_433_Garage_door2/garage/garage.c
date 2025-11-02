/*
 * garage.c
 *
 *  Created on: 12.07.2020
 *      Author: lapiUser
 */

#include <avr/eeprom.h>
#include "../main.h"
#include "garage.h"
#include "../my_eeprom.h"

volatile uint16_t g_time_counter;			// 1 ms counter

volatile uint16_t g_door_open_timer_sec = 0;	// 1 sec counter
volatile uint8_t g_count_door_open_time;
volatile uint8_t g_count_door_open_time_old = TRUE;
volatile uint16_t g_time_tick_divider_sec = G_TIME_DIV_SEC;  // Default tick is every 1ms * 1000 => 1s

volatile s_garage garage;
volatile uint8_t g_button;					// Type of pressed button  OPEN/CLOSE
volatile uint8_t g_last_btn;

volatile uint16_t g_beep_warning_time_ms;
volatile uint16_t g_light_warning_time_ms;
volatile uint8_t g_next_garage_state;

volatile uint8_t g_stop_closing;
volatile uint16_t g_closing_time;
volatile uint8_t g_stop_opening;
volatile uint16_t g_opening_time;

volatile uint16_t g_debouncing_time;
volatile uint16_t g_button_hold_time_ms;
volatile uint16_t g_button_pressed_time_ms;
volatile uint8_t g_button_pressed_and_hold; // TRUE/FALSE

// Confirmation
volatile uint16_t g_conf_tiemr_ms = 0;  // Used for confirmation of new settings

// beep handler
volatile uint16_t g_beep_tiemr_ms;

// light handler
volatile uint16_t g_light_tiemr_ms;

// Warnings enabled delay
volatile uint16_t g_warnings_delay_ms;
volatile uint8_t g_auto_closing_enabled = FALSE;

volatile uint16_t g_ee_max_closing_time_ms;
volatile uint16_t g_ee_max_opening_time_ms;
volatile uint16_t  g_ee_max_door_open_time_sec;
volatile uint16_t g_ee_worning_time_ms;
volatile uint16_t g_ee_worning_beep_on_time_ms;
volatile uint16_t g_ee_worning_beep_off_time_ms;
volatile uint16_t g_ee_worning_light_on_time_ms;
volatile uint16_t g_ee_worning_light_off_time_ms;
volatile uint16_t g_ee_warnings_enabled_delay_ms;
volatile uint16_t g_ee_time_between_commands_ms;
volatile uint16_t g_ee_worning_door_open_beep_on_time_ms;
volatile uint16_t g_ee_worning_door_open_beep_off_time_ms;
volatile uint16_t g_ee_worning_door_open_light_on_time_ms;
volatile uint16_t g_ee_worning_door_open_light_off_time_ms;

// Reading EEPROM settings
void garage_read_eeprom_settings(void){
	eeprom_busy_wait();
	g_ee_max_closing_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_MAX_CLOSING_TIME_MS);
	eeprom_busy_wait();
	g_ee_max_opening_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_MAX_OPENING_TIME_MS);
	eeprom_busy_wait();
	g_ee_max_door_open_time_sec = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_MAX_DOOR_OPEN_TIME_SEC);
	eeprom_busy_wait();
	g_ee_worning_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_beep_on_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_BEEP_ON_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_beep_off_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_BEEP_OFF_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_light_on_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_LIGHT_ON_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_light_off_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_LIGHT_OFF_TIME_MS);
	eeprom_busy_wait();
	g_ee_warnings_enabled_delay_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNINGS_ENABLED_DELAY_MS);
	eeprom_busy_wait();
	g_ee_time_between_commands_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_TIME_BETWEEN_COMMANDS_MS);
	eeprom_busy_wait();
	g_ee_worning_door_open_beep_on_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_door_open_beep_off_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_door_open_light_on_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS);
	eeprom_busy_wait();
	g_ee_worning_door_open_light_off_time_ms = eeprom_read_word((uint16_t*)EE_ADR_WORD__G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS);
}


// Function used for time ticks updates
// Default 1 ms
void garage_time_tick(void){

	g_time_tick_divider_sec++;
	if (g_time_tick_divider_sec >= G_TIME_DIV_SEC){
		g_time_tick_divider_sec = 0;

		if (g_count_door_open_time == TRUE){
			g_door_open_timer_sec++;				// 1 sec time tick
		}
	}

	g_time_counter++;					// 1 ms time tick
	g_closing_time++;
	g_opening_time++;
	g_beep_tiemr_ms++;
	g_light_tiemr_ms++;

	// Debouncing
	if (g_debouncing_time < g_ee_time_between_commands_ms){
		g_debouncing_time++;
	}

	// Holding the button
	if (g_button_hold_time_ms > 0){
		g_button_hold_time_ms--;

		if (g_button_pressed_time_ms >= HOLD_TIME_MS){
			g_button_pressed_and_hold = TRUE;
		} else {
			g_button_pressed_time_ms++;
			g_button_pressed_and_hold = FALSE;
		}

	} else {
		g_button_pressed_time_ms = 0;
		g_button_pressed_and_hold = FALSE;
	}

	// Warnings delay
	if (g_warnings_delay_ms > 0) g_warnings_delay_ms--;

	// New settings confirmation delay
	if (g_conf_tiemr_ms > 0) g_conf_tiemr_ms--;
}

// Initialization
void garage_init(void){
	// Enable auto closing
	garage_toggle_auto_closing();
	setup_warnings(g_ee_worning_door_open_beep_on_time_ms,     // Beep ON time ms
				   g_ee_worning_door_open_beep_off_time_ms,    // Beep OFF time ms
				   g_ee_worning_door_open_light_on_time_ms,		// Light ON time ms
				   g_ee_worning_door_open_light_off_time_ms);	// Light OFF time ms
}

// Pressed buttons handler
void garage_button_pressed(uint8_t g_btn){

	if (g_btn != g_button){					// New button pressed
		if(g_debouncing_time >= g_ee_time_between_commands_ms){
			g_debouncing_time = 0;
			g_button = g_btn;
		}
	}

	if (g_btn == g_last_btn){
		if (g_button_hold_time_ms < MAX_HEART_BEAT_TIME){
			g_button_hold_time_ms += MAX_HEART_BEAT_TIME; // add time for detection of holding the button
		}
	} else {
		g_last_btn = g_btn;
	}
}


// Main garage door function
void garage_handler(void){

	// Main state machine
	switch (garage.state){
	case G_IDLE:

		if (g_button == BUTTON_CLOSE){			// CLOSING settings

			setup_warnings(g_ee_worning_beep_on_time_ms,
						   g_ee_worning_beep_off_time_ms,
						   g_ee_worning_light_on_time_ms,
						   g_ee_worning_light_off_time_ms);
			g_beep_warning_time_ms = g_ee_worning_time_ms;
			g_light_warning_time_ms = g_ee_worning_time_ms;
			g_next_garage_state = G_CLOSING;
			g_time_counter = 0;
			g_closing_time = 0;
			garage.state = G_WARNING;
		}

		if (g_button == BUTTON_OPEN) {			// OPENING settings

			setup_warnings(g_ee_worning_beep_on_time_ms,
						   g_ee_worning_beep_off_time_ms,
						   g_ee_worning_light_on_time_ms,
						   g_ee_worning_light_off_time_ms);
			g_beep_warning_time_ms = g_ee_worning_time_ms;
			g_light_warning_time_ms = g_ee_worning_time_ms;
			g_next_garage_state = G_OPENING;
			g_time_counter = 0;
			g_opening_time = 0;
			garage.state = G_WARNING;
		}
		break;

	case G_CLOSING:
		g_stop_closing = FALSE;

		// Checking conditions for NOT CLOSING
		// Conditions for STOP closing with warnings delay
		if (garage.i_door_status == CLOSED){
			g_warnings_delay_ms = g_ee_warnings_enabled_delay_ms;
			garage.state = G_DELAY_AFTER_CLOSING;
		}


		// Checking conditions for NOT CLOSING
		// Conditions for STOP closing immediately

		// Detection if there is no obstacle in garage door
		if (garage.i_obstacle_detected == TRUE){
			g_stop_closing = TRUE;
		}

		// CLOSING button pressed during closing
		if (g_button == BUTTON_CLOSE){
			if (g_button_pressed_time_ms < g_ee_time_between_commands_ms){  // Detection of holding the button
			g_stop_closing = TRUE;
			}
		}

		// OPEN button pressed during closing
		if (g_button == BUTTON_OPEN) {
			g_stop_closing = TRUE;
		}

		// Time for closing elapsed
		if (g_closing_time >= g_ee_max_closing_time_ms){	// Default 15 sec
			g_stop_closing = TRUE;
		}


		// Force closing by pressing and holding CLOSE button
		// Must be just before CLOSING section
		if (g_button_pressed_and_hold == TRUE){
			g_closing_time = g_ee_max_closing_time_ms;  // Setting timer to max will stop action after releasing button
			g_stop_closing = FALSE;
		}

		// CLOSING
		if (g_stop_closing == FALSE){
			// Enabling CLOSING
			setup_warnings(g_ee_worning_beep_on_time_ms,
						   g_ee_worning_beep_off_time_ms,
						   g_ee_worning_light_on_time_ms,
						   g_ee_worning_light_off_time_ms);
			g_beep_warning_time_ms = g_ee_worning_time_ms;
			g_light_warning_time_ms = g_ee_worning_time_ms;
			garage.o_door_close = ON;
		} else {
			// Disabling CLOSING
			garage.beep = OFF;
			garage.light = OFF;
			garage.o_door_close = OFF;
			garage.state = G_IDLE;
		}
		break;

	case G_DELAY_AFTER_CLOSING:
			if(g_warnings_delay_ms == 0){
				// Disabling CLOSING
				garage.beep = OFF;
				garage.light = OFF;
				garage.o_door_close = OFF;
				garage.state = G_IDLE;
			}
		break;

	case G_OPENING:
			g_stop_opening = FALSE;

			// Checking conditions for NOT OPENING
			// CLOSING button pressed during opening
			if (g_button == BUTTON_CLOSE) {
				g_stop_opening = TRUE;
			}

			// OPEN button pressed during opening
			if (g_button == BUTTON_OPEN) {
				if (g_button_pressed_time_ms < g_ee_time_between_commands_ms){  // Detection of holding the button
					g_stop_opening = TRUE;
				}
			}

			// Time for opening elapsed
			if (g_opening_time >= g_ee_max_opening_time_ms){	// Default 15 sec
				g_stop_opening = TRUE;
			}


			// Force opening by pressing and holding OPEN button
			// Must be just before OPENING section
			if (g_button_pressed_and_hold == TRUE){
				g_opening_time = g_ee_max_opening_time_ms;  // Setting timer to max will stop action after releasing button
				g_stop_opening = FALSE;
			}

			// OPENING
			if (g_stop_opening == FALSE){
				// Enabling OPENING
				setup_warnings(g_ee_worning_beep_on_time_ms,
							   g_ee_worning_beep_off_time_ms,
							   g_ee_worning_light_on_time_ms,
							   g_ee_worning_light_off_time_ms);
				g_beep_warning_time_ms = g_ee_worning_time_ms;
				g_light_warning_time_ms = g_ee_worning_time_ms;
				garage.o_door_open = ON;
			} else {
				// Disabling OPENING
				garage.beep = OFF;
				garage.light = OFF;
				garage.o_door_open = OFF;
				garage.state = G_IDLE;
			}
			break;
	case G_WARNING:

		if ((g_time_counter >= g_beep_warning_time_ms) &&
			(g_time_counter >= g_light_warning_time_ms)){
			garage.state = g_next_garage_state;
		}
		break;

	default:
		garage.state = G_IDLE;
		break;
	}
	g_button = BUTTON_NONE;
}


// Setup warnings
void setup_warnings(uint16_t _WORNING_beep_on_time_ms,
					uint16_t _WORNING_beep_off_time_ms,
					uint16_t _WORNING_light_on_time_ms,
					uint16_t _WORNING_light_off_time_ms){

	garage.beep_on_time_ms = _WORNING_beep_on_time_ms;
	garage.beep_off_time_ms = _WORNING_beep_off_time_ms;
	garage.beep = ON;

	garage.light_on_time_ms = _WORNING_light_on_time_ms;
	garage.light_off_time_ms = _WORNING_light_off_time_ms;
	garage.light = ON;

}

// Beep handler
void beep_handler(void){
	uint16_t g_beep_cycle_time_ms;

	if (g_conf_tiemr_ms == 0){
		if (garage.beep == ON){
			g_beep_cycle_time_ms = garage.beep_on_time_ms + garage.beep_off_time_ms;

			if (g_beep_tiemr_ms >= g_beep_cycle_time_ms){
				g_beep_tiemr_ms = 0;	// resting timer to start cycle from beginning
			}

			if (g_beep_tiemr_ms < garage.beep_on_time_ms){
				garage.o_beep = ON;
			} else garage.o_beep = OFF;

		} else {
			garage.o_beep = OFF;
		}
	} else {
		garage.o_beep = ON; // Beep ON during confirmation time
	}
}

// Warning light handler
void light_handler(void){
	uint16_t g_light_cycle_time_ms;

	if (g_conf_tiemr_ms == 0){
		if (garage.light == ON){
			g_light_cycle_time_ms = garage.light_on_time_ms + garage.light_off_time_ms;

			if (g_light_tiemr_ms >= g_light_cycle_time_ms){
				g_light_tiemr_ms = 0;	// resting timer to start cycle from beginning
			}

			if (g_light_tiemr_ms < garage.light_on_time_ms){
				garage.o_light = ON;
			} else garage.o_light = OFF;

		} else {
			garage.o_light = OFF;
		}
	} else {
		garage.o_light = ON; // Light ON during confirmation time
	}
}


// Enable/Disable Auto closing
void garage_toggle_auto_closing(void){
	if (g_auto_closing_enabled == TRUE){
		g_auto_closing_enabled = FALSE;
	} else {
		g_auto_closing_enabled = TRUE;
	}
	// New settings confirmation
	g_conf_tiemr_ms = 1000;
}


// Auto closing
void auto_closing_handler(void){

	if ((garage.state == G_IDLE)&&
		(garage.i_door_status == OPEN)&&
		(g_auto_closing_enabled == TRUE)){
		g_count_door_open_time = TRUE;
	} else {
		g_count_door_open_time = FALSE;
		g_door_open_timer_sec = 0;
	}

	if (g_door_open_timer_sec >= g_ee_max_door_open_time_sec){
		garage_button_pressed(BUTTON_CLOSE);			// CLOSE button pressed
	}

	// Enabling warning
	// Warning when door is OPEN and left OPEN for max_door_open_time_sec
	if ((g_count_door_open_time == TRUE)&&
		(g_count_door_open_time_old == FALSE)){
		setup_warnings(g_ee_worning_door_open_beep_on_time_ms,     // Beep ON time ms
					   g_ee_worning_door_open_beep_off_time_ms,    // Beep OFF time ms
					   g_ee_worning_door_open_light_on_time_ms,		// Light ON time ms
					   g_ee_worning_door_open_light_off_time_ms);	// Light OFF time ms
	}

	if ((g_count_door_open_time == FALSE)&&
		(g_count_door_open_time_old == TRUE)&&
		(garage.state == G_IDLE)){
		garage.beep = OFF;
		garage.light = OFF;
	}
	g_count_door_open_time_old = g_count_door_open_time;
}

