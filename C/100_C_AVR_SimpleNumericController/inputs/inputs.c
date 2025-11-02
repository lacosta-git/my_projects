/*
 * inputs.c
 *
 *  Created on: 04.01.2018
 *      Author: autoMaster
 */

#include "../main.h"
#include "inputs.h"

// **********************************************
// **********        FILTER INPUTS       ********
// **********************************************
void filterInput(uint8_t raw_input, INPUT_FILTER* p_input){
	if (raw_input != 0){
		if (p_input->counter < p_input->counterMax) p_input->counter++;
	} else {
		if (p_input->counter > p_input->counterMin) p_input->counter--;
	}

	if ((p_input->f_input == TRUE)&
		(p_input->counter < (p_input->counterMid - p_input->hysteresis))){
		p_input->f_input = FALSE;
	}
	if ((p_input->f_input == FALSE)&
		(p_input->counter > (p_input->counterMid + p_input->hysteresis))){
		p_input->f_input = TRUE;
	}
}

// **********************************************
void filterInputs(uint8_t raw_inputs, INPUT_FILTER portSets[]){
	for (uint8_t i=0; i<=7; i++){
		filterInput((raw_inputs >> i & 0x01), &portSets[i]);
	}
}
// **********************************************
void initializeInputFilter(){
	uint8_t i = 0;
	for (i=0; i<=7; i++){
		inputsPortA[i].counter = 128;
		inputsPortA[i].counterMax = 255;
		inputsPortA[i].counterMid = 128;
		inputsPortA[i].counterMin = 0;
		inputsPortA[i].hysteresis = 100;
		inputsPortA[i].f_input = 0;
		inputsPortA[i].ss_input = FALSE;
	}
	for (i=0; i<=7; i++){
		inputsPortC[i].counter = 128;
		inputsPortC[i].counterMax = 255;
		inputsPortC[i].counterMid = 128;
		inputsPortC[i].counterMin = 0;
		inputsPortC[i].hysteresis = 100;
		inputsPortC[i].f_input = 0;
		inputsPortC[i].ss_input = FALSE;
	}
}

