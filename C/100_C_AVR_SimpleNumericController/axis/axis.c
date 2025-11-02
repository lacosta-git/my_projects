/*
 * axis.c
 *
 *  Created on: 08.10.2017
 *      Author: autoMaster
 */

#include "axis.h"

void initAxis(void){

	// Ports configuration
	ST_MO_1_DIR_DIR |= ST_MO_1_DIR;
	ST_MO_1_PUL_DIR |= ST_MO_1_PUL;
	ST_MO_2_DIR_DIR |= ST_MO_2_DIR;
	ST_MO_2_PUL_DIR |= ST_MO_2_PUL;
	ST_MO_3_DIR_DIR |= ST_MO_3_DIR;
	ST_MO_3_PUL_DIR |= ST_MO_3_PUL;
	ST_MO_4_DIR_DIR |= ST_MO_4_DIR;
	ST_MO_4_PUL_DIR |= ST_MO_4_PUL;
	ST_MO_5_DIR_DIR |= ST_MO_5_DIR;
	ST_MO_5_PUL_DIR |= ST_MO_5_PUL;
	ST_MO_6_DIR_DIR |= ST_MO_6_DIR;
	ST_MO_6_PUL_DIR |= ST_MO_6_PUL;


	// X axis
	xAxis.maxSpeedDivider = 10;
	xAxis.currentDivider = xAxis.maxSpeedDivider;
	xAxis.ramp = RAMP_STEPS;
	xAxis.manualEnable = FALSE;
	xAxis.enable = FALSE;

	// Y axis
	yAxis.maxSpeedDivider = 10;
	yAxis.currentDivider = yAxis.maxSpeedDivider;
	yAxis.ramp = RAMP_STEPS;
	yAxis.manualEnable = FALSE;
	yAxis.enable = FALSE;

	// Z axis
	zAxis.maxSpeedDivider = 10;
	zAxis.currentDivider = zAxis.maxSpeedDivider;
	zAxis.ramp = RAMP_STEPS;
	zAxis.manualEnable = FALSE;
	zAxis.enable = FALSE;

	// A axis
	aAxis.maxSpeedDivider = 10;
	aAxis.currentDivider = aAxis.maxSpeedDivider;
	aAxis.ramp = RAMP_STEPS;
	aAxis.manualEnable = FALSE;
	aAxis.enable = FALSE;

	// B axis
	bAxis.maxSpeedDivider = 10;
	bAxis.currentDivider = bAxis.maxSpeedDivider;
	bAxis.ramp = RAMP_STEPS;
	bAxis.manualEnable = FALSE;
	bAxis.enable = FALSE;

	// C axis
	cAxis.maxSpeedDivider = 10;
	cAxis.currentDivider = cAxis.maxSpeedDivider;
	cAxis.ramp = RAMP_STEPS;
	cAxis.manualEnable = FALSE;
	cAxis.enable = FALSE;


}

void setXAxisDir(uint8_t dir){
	if (dir == PLUS){
		ST_MO_1_DIR_PORT |= ST_MO_1_DIR;
	} else {
		ST_MO_1_DIR_PORT &= ~ST_MO_1_DIR;
	}
}

void setYAxisDir(uint8_t dir){
	if (dir == PLUS){
		ST_MO_2_DIR_PORT |= ST_MO_2_DIR;
	} else {
		ST_MO_2_DIR_PORT &= ~ST_MO_2_DIR;
	}
}

void setZAxisDir(uint8_t dir){
	if (dir == PLUS){
		ST_MO_3_DIR_PORT |= ST_MO_3_DIR;
	} else {
		ST_MO_3_DIR_PORT &= ~ST_MO_3_DIR;
	}
}

void setAAxisDir(uint8_t dir){
	if (dir == PLUS){
		ST_MO_4_DIR_PORT |= ST_MO_4_DIR;
	} else {
		ST_MO_4_DIR_PORT &= ~ST_MO_4_DIR;
	}
}

void setBAxisDir(uint8_t dir){
	if (dir == PLUS){
		ST_MO_5_DIR_PORT |= ST_MO_5_DIR;
	} else {
		ST_MO_5_DIR_PORT &= ~ST_MO_5_DIR;
	}
}

void setCAxisDir(uint8_t dir){
	if (dir == PLUS){
		ST_MO_6_DIR_PORT |= ST_MO_6_DIR;
	} else {
		ST_MO_6_DIR_PORT &= ~ST_MO_6_DIR;
	}
}

void axisHandler(struct stepMotor *axis){
	if ((axis->enable == TRUE)|
		(axis->manualEnable == TRUE))
		{
		if (axis->steps > 0){
			if (axis->currentDivider > 0) {
				axis->currentDivider--;
			} else {
				axis->steps--;

				// change output state
				if (axis->output > 0) axis->output = 0;
				else axis->output = 1;

				if (axis->steps > RAMP_STEPS) {  // Ramp up
					if (axis->ramp > 0) axis->ramp--;
				} else {  // Ramp down
					axis->ramp++;
				}

				axis->currentDivider = axis->maxSpeedDivider + axis->ramp;
			}
		} else {
			axis->enable = FALSE;
		}
	}
}

void interruptAxisHandler(void){
	axisHandler((void*)&xAxis);
	if (xAxis.dir == PLUS){
		ST_MO_1_DIR_PORT |= ST_MO_1_DIR;
	} else {
		ST_MO_1_DIR_PORT &= ~ST_MO_1_DIR;
	}
	if (xAxis.output > 0) ST_MO_1_PUL_PORT |= ST_MO_1_PUL;
	else ST_MO_1_PUL_PORT &= ~(ST_MO_1_PUL);

	axisHandler((void*)&yAxis);
	if (yAxis.dir == PLUS){
		ST_MO_2_DIR_PORT |= ST_MO_2_DIR;
	} else {
		ST_MO_2_DIR_PORT &= ~ST_MO_2_DIR;
	}
	if (yAxis.output > 0) ST_MO_2_PUL_PORT |= ST_MO_2_PUL;
	else ST_MO_2_PUL_PORT &= ~(ST_MO_2_PUL);

	axisHandler((void*)&zAxis);
	if (zAxis.dir == PLUS){
		ST_MO_3_DIR_PORT |= ST_MO_3_DIR;
	} else {
		ST_MO_3_DIR_PORT &= ~ST_MO_3_DIR;
	}
	if (zAxis.output > 0) ST_MO_3_PUL_PORT |= ST_MO_3_PUL;
	else ST_MO_3_PUL_PORT &= ~(ST_MO_3_PUL);

	axisHandler((void*)&aAxis);
	if (aAxis.dir == PLUS){
		ST_MO_4_DIR_PORT |= ST_MO_4_DIR;
	} else {
		ST_MO_4_DIR_PORT &= ~ST_MO_4_DIR;
	}
	if (aAxis.output > 0) ST_MO_4_PUL_PORT |= ST_MO_4_PUL;
	else ST_MO_4_PUL_PORT &= ~(ST_MO_4_PUL);

	axisHandler((void*)&bAxis);
	if (bAxis.dir == PLUS){
		ST_MO_5_DIR_PORT |= ST_MO_5_DIR;
	} else {
		ST_MO_5_DIR_PORT &= ~ST_MO_5_DIR;
	}
	if (bAxis.output > 0) ST_MO_5_PUL_PORT |= ST_MO_5_PUL;
	else ST_MO_5_PUL_PORT &= ~(ST_MO_5_PUL);

	axisHandler((void*)&cAxis);
	if (cAxis.dir == PLUS){
		ST_MO_6_DIR_PORT |= ST_MO_6_DIR;
	} else {
		ST_MO_6_DIR_PORT &= ~ST_MO_6_DIR;
	}
	if (cAxis.output > 0) ST_MO_6_PUL_PORT |= ST_MO_6_PUL;
	else ST_MO_6_PUL_PORT &= ~(ST_MO_6_PUL);
}
