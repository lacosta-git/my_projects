/*
 * interpreter.c
 *
 *  Created on: 03.10.2017
 *      Author: autoMaster
 */

#include "interpreter.h"
#include "../interrups/interrups.h"
#include "../rs232/rs232.h"
#include "../axis/axis.h"

volatile uint8_t lineCounter = 0;
volatile uint8_t columnCounter = 0;
volatile uint8_t lineCMDExecutionStatus = DONE;

volatile struct stepMotor *interAxis;
static uint8_t cmdLine[MAX_COLUMNS];
static uint8_t lineWithCode = FALSE;
uint32_t dummy;

uint32_t decodeDigits(uint8_t* cmdEntry){
	char sign;
	uint32_t digit = 0;

	while (columnCounter < (MAX_COLUMNS - 1)){
		columnCounter++;
		sign = *(cmdEntry + columnCounter);
		if (sign != 0x20){
			digit = (digit * 10) + (sign & 0x0F);
		} else break;
	}
	return digit;
}

void mainInterpreterHandler(void){
	if (runProgram == FALSE){
		delayCounter1 = 0;
		xAxis.enable = FALSE;
		yAxis.enable = FALSE;
		zAxis.enable = FALSE;
		aAxis.enable = FALSE;
		bAxis.enable = FALSE;
		cAxis.enable = FALSE;
		lineCMDExecutionStatus = DONE;
		lineCounter = 0;
	}

	if ((lineCMDExecutionStatus == DONE)&
		(runProgram == TRUE)){
		// Copy new command line
		lineWithCode = FALSE;
		for (uint8_t i = 0; i < MAX_COLUMNS; i++){
			cmdLine[i] = vPaper[lineCounter][i];
			if (cmdLine[i] != 0x20) lineWithCode = TRUE;  // Code detected in new line
		}
		if (lineWithCode == TRUE){
			columnCounter = 0;
			lineCMDExecutionStatus = PARSING;
		} else runProgram = FALSE;  // END of execution
	}

	// CMD line interpretation
	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'N'){
			dummy = decodeDigits((uint8_t*)&cmdLine);
			dummy++;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'M'){
			dummy = decodeDigits((uint8_t*)&cmdLine);
			dummy++;
			lineCMDExecutionStatus = DONE;
			lineCounter = 0;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'W'){
			delayCounter1 = decodeDigits((uint8_t*)&cmdLine) * 10;  // 1 ms
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'X'){
			if (cmdLine[columnCounter + 1] == '-'){
				columnCounter++;
				xAxis.dir = MINUS;
			} else xAxis.dir = PLUS;
			xAxis.steps = decodeDigits((uint8_t*)&cmdLine);
			interAxis = (struct stepMotor*)&xAxis;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'Y'){
			if (cmdLine[columnCounter + 1] == '-'){
				columnCounter++;
				yAxis.dir = MINUS;
			} else yAxis.dir = PLUS;
			yAxis.steps = decodeDigits((uint8_t*)&cmdLine);
			interAxis = (struct stepMotor*)&yAxis;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'Z'){
			if (cmdLine[columnCounter + 1] == '-'){
				columnCounter++;
				zAxis.dir = MINUS;
			} else zAxis.dir = PLUS;
			zAxis.steps = decodeDigits((uint8_t*)&cmdLine);
			interAxis = (struct stepMotor*)&zAxis;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'A'){
			if (cmdLine[columnCounter + 1] == '-'){
				columnCounter++;
				aAxis.dir = MINUS;
			} else aAxis.dir = PLUS;
			aAxis.steps = decodeDigits((uint8_t*)&cmdLine);
			interAxis = (struct stepMotor*)&aAxis;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'B'){
			if (cmdLine[columnCounter + 1] == '-'){
				columnCounter++;
				bAxis.dir = MINUS;
			} else bAxis.dir = PLUS;
			bAxis.steps = decodeDigits((uint8_t*)&cmdLine);
			interAxis = (struct stepMotor*)&bAxis;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'C'){
			if (cmdLine[columnCounter + 1] == '-'){
				columnCounter++;
				cAxis.dir = MINUS;
			} else cAxis.dir = PLUS;
			cAxis.steps = decodeDigits((uint8_t*)&cmdLine);
			interAxis = (struct stepMotor*)&cAxis;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == 'F'){
			interAxis->maxSpeedDivider = decodeDigits((uint8_t*)&cmdLine);
			interAxis->currentDivider = interAxis->maxSpeedDivider;
		}
	}

	if (lineCMDExecutionStatus == PARSING){
		if (cmdLine[columnCounter] == ';'){  // Comment detected
			columnCounter = (MAX_COLUMNS - 1);
		}
	}
	if (lineCMDExecutionStatus == START){
		delayTimerEnable = TRUE;
		xAxis.enable = TRUE;
		yAxis.enable = TRUE;
		zAxis.enable = TRUE;
		aAxis.enable = TRUE;
		bAxis.enable = TRUE;
		cAxis.enable = TRUE;
		lineCMDExecutionStatus = EXECUTING;
	}

	if ((lineCMDExecutionStatus == PARSING)&
		(columnCounter >= (MAX_COLUMNS - 1))){
		lineCMDExecutionStatus = START;
	}

	if (lineCMDExecutionStatus == PARSING){
		columnCounter++;
	}

	if (lineCMDExecutionStatus == EXECUTING){
		if ((delayCounter1 == 0) &
			(xAxis.enable == FALSE) &
			(yAxis.enable == FALSE) &
			(zAxis.enable == FALSE) &
			(aAxis.enable == FALSE) &
			(bAxis.enable == FALSE) &
			(cAxis.enable == FALSE)){

			lineCMDExecutionStatus = DONE;
			lineCounter++;
		}
	}
}

