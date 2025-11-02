/*
 * interpreter.h
 *
 *  Created on: 03.10.2017
 *      Author: autoMaster
 */

#include "../main.h"

// Line command execution status
#define DONE 		0
#define PARSING 	1
#define EXECUTING 	2
#define START		3

// Interpreter program execution
volatile uint8_t runProgram;

void mainInterpreterHandler(void);
