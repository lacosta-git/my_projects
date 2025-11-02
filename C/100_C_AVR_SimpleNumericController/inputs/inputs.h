/*
 * inputs.h
 *
 *  Created on: 04.01.2018
 *      Author: autoMaster
 */

#ifndef INPUTS_INPUTS_H_
#define INPUTS_INPUTS_H_

typedef struct {
	volatile uint8_t counter;		// counter of input state
	volatile uint8_t counterMax;	// max counter value
	volatile uint8_t counterMid;	// Output: mid + his = TRUE; mid - his = FALSE
	volatile uint8_t counterMin;	// min counter value
	volatile uint8_t hysteresis;	// Hysteresis
	volatile uint8_t f_input;		//
	volatile uint8_t ss_input;
} INPUT_FILTER;

INPUT_FILTER inputsPortA[8];  // Buttons
INPUT_FILTER inputsPortC[8];  // Sensors

#endif /* INPUTS_INPUTS_H_ */
