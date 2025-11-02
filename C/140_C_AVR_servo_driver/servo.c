/*
 * servo.c
 *
 *  Created on: 30.12.2019
 *      Author: lapiUser
 */

#include "main.h"
#include "servo.h"

// Variables
typedef struct {
	volatile uint8_t connected;  // TRUE, FALSE
	volatile uint8_t phase;		 // 0, 1
	volatile uint8_t *port;		 // PORTB, PORTC ...
	volatile uint8_t pin;		 // PB5, PC3 ...
	volatile uint8_t angle;		 // 0...180
} SRV_CHANEL;

volatile SRV_CHANEL srv_ch_index_mtrx[8];

volatile uint8_t srv_ch_index = 0;				// max 8 possible [0..7]
volatile uint16_t svr_next_reload = RELOAD;		// Default 2,5 ms

// Functions
void srv_attach(uint8_t *svr_port, uint8_t svr_pin, uint8_t assigned_ch){
	srv_ch_index_mtrx[assigned_ch].port = svr_port;
	srv_ch_index_mtrx[assigned_ch].pin = svr_pin;
	srv_ch_index_mtrx[assigned_ch].connected = TRUE;
}


void srv_set_angle(uint8_t srv_ch_indx, uint8_t srv_angle){
	srv_ch_index_mtrx[srv_ch_indx].angle = srv_angle;
}


void srv_handler(void){
	if (srv_ch_index_mtrx[srv_ch_index].connected == TRUE){
		if (srv_ch_index_mtrx[srv_ch_index].phase == 0){
			srv_ch_index_mtrx[srv_ch_index].phase = 1;
			*srv_ch_index_mtrx[srv_ch_index].port |= (1<<srv_ch_index_mtrx[srv_ch_index].pin);  // Set Output
			svr_next_reload = 0xFFFF - (SRV_0_DEGREE + SVR_1_DEGREE * srv_ch_index_mtrx[srv_ch_index].angle);
		} else {
			srv_ch_index_mtrx[srv_ch_index].phase = 0;
			*srv_ch_index_mtrx[srv_ch_index].port &= ~(1<<srv_ch_index_mtrx[srv_ch_index].pin);  // Clear Output
			svr_next_reload = RELOAD + (SRV_0_DEGREE + SVR_1_DEGREE * srv_ch_index_mtrx[srv_ch_index].angle);
			srv_ch_index ++;
		}
	} else {
		svr_next_reload = RELOAD;		// Default timer settings
		srv_ch_index ++;
	}

	// Servo index update
	if (srv_ch_index >= 8) srv_ch_index = 0;

}

// Function returns number of next timer ticks
uint16_t srv_next_reload(void){
	return svr_next_reload;
}
