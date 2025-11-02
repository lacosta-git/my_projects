/*
 * axis.h
 *
 *  Created on: 08.10.2017
 *      Author: autoMaster
 */

#include "../main.h"

#define READY  0
#define RUNNING 1
#define PLUS	0
#define MINUS	1

#define RAMP_STEPS 10

struct stepMotor{
	uint8_t enable;  	// TRUE, FALSE
	uint8_t manualEnable;	// manual movement
	uint8_t dir;  		// PLUS, MINUS
	uint8_t maxSpeedDivider;
	uint8_t currentDivider;
	uint8_t ramp;
	uint32_t steps;
	uint8_t output;  	// TRUE, FALSE
};

volatile struct stepMotor xAxis;
volatile struct stepMotor yAxis;
volatile struct stepMotor zAxis;
volatile struct stepMotor aAxis;
volatile struct stepMotor bAxis;
volatile struct stepMotor cAxis;

void initAxis(void);
void interruptAxisHandler(void);
void setXAxisDir(uint8_t);
void setYAxisDir(uint8_t);
void setZAxisDir(uint8_t);
void setAAxisDir(uint8_t);
void setBAxisDir(uint8_t);
void setCAxisDir(uint8_t);
