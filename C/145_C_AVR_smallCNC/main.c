/*
 * main.c
 *
 *  Created on: 14.03.2017
 *      Author: autoMaster
 */

#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"
#include "rotations.h"

// 1 - Output
// 0 - Input
// Port B
// Port D direction
#define		DDR_B()			DDRB = 0; PORTB = 0xFF  // All inputs

// Port C direction
#define		DDR_C()			DDRC = 0xFF  // All outputs
#define		MA_MO_FORWARD_ON()	PORTC &=~(1<<DDC0) // 0
#define		MA_MO_FORWARD_OFF()	PORTC |= (1<<DDC0) // 1
#define		MA_MO_REVERSE_ON()	PORTC &=~(1<<DDC1) // 0
#define		MA_MO_REVERSE_OFF()	PORTC |= (1<<DDC1) // 1
#define		MA_MO_SLOW()		PORTC &=~(1<<DDC2) // 0
#define		MA_MO_FAST()		PORTC |= (1<<DDC2) // 1
#define		MA_MO_SCREW()		PORTC &=~(1<<DDC3) // 0
#define		MA_MO_PIPE()		PORTC |= (1<<DDC3) // 1
#define		ST_MO_DIR_LEFT()	PORTC &=~(1<<DDC4) // 0
#define		ST_MO_DIR_RIGHT()	PORTC |= (1<<DDC4) // 1
#define		ST_MO_PULSE_TRG()	PORTC ^= (1<<DDC5) // change state

// Port D direction
#define		DDR_D()			DDRD = 0; PORTD = 0xFF // All PINS are inputs + PullUps

// Iteration
volatile uint8_t i = 0;

// BOOL
#define FALSE 0
#define TRUE 1

// INPUTS
volatile uint8_t bSTART = FALSE;
volatile uint8_t bSTARTCounter = 0xff;
volatile uint8_t bSTOP = TRUE;
volatile uint8_t bSTOPCounter = 0xff;
volatile uint8_t bMA_MO_FORWARD = FALSE;
volatile uint8_t bMA_MO_FORWARDCounter = 0xff;
volatile uint8_t bMA_MO_REVERSE = FALSE;
volatile uint8_t bMA_MO_REVERSECounter = 0xff;
volatile uint8_t bROTATE_R = FALSE;
volatile uint8_t bROTATE_RCounter = 0xff;
volatile uint8_t bROTATE_L = FALSE;
volatile uint8_t bROTATE_LCounter = 0xff;
volatile uint8_t iBACK_END_PIPE = FALSE;
volatile uint8_t iBACK_END_PIPECounter = 0xff;
volatile uint8_t iBACK_END_SCREW = FALSE;
volatile uint8_t iBACK_END_SCREWCounter = 0xff;
volatile uint8_t iSLOW_PIPE = FALSE;
volatile uint8_t iSLOW_PIPECounter = 0xff;
volatile uint8_t iSLOW_SCREW = FALSE;
volatile uint8_t iSLOW_SCREWCounter = 0xff;
volatile uint8_t iFRONT_END_PIPE = FALSE;
volatile uint8_t iFRONT_END_PIPECounter = 0xff;
volatile uint8_t iFRONT_END_SCREW = FALSE;
volatile uint8_t iFRONT_END_SCREWCounter = 0xff;

// OUTPUTS
#define RIGHT 0
#define LEFT 1
volatile uint8_t ST_MO_DIR = RIGHT;
#define LOW 0
#define HIGH 1
volatile uint8_t MA_MO_SPEED = LOW;
volatile uint8_t MA_MO_FORWARD = FALSE;
volatile uint8_t MA_MO_REVERSE = FALSE;
#define SCREW 0
#define PIPE 1
volatile uint8_t MA_MO_PIPE_SCREW = PIPE;

// Step Motor
#define ST_MO_RAMP_STEPS_MAX 40
volatile uint8_t st_mo_pulseWidthMultiplier = ST_MO_RAMP_STEPS_MAX;
volatile uint8_t st_mo_puseWidth = ST_MO_RAMP_STEPS_MAX;
volatile uint8_t ST_MO_ROTATE = FALSE;
volatile uint8_t ST_MO_SPEED = LOW;
volatile uint16_t st_mo_step_count = 0;
volatile uint8_t st_mo_generatePulse = FALSE;
#define ST_MO_HIGH_SPEED_DIVIDER 1  // speed = max_speed / divider
#define ST_MO_LOW_SPEED_DIVIDER 30
volatile uint8_t st_mo_speedDivider = ST_MO_LOW_SPEED_DIVIDER;
volatile uint8_t pulse = FALSE;

// LOGIC
#define L_UNDEFINED 0xff
volatile uint8_t step = L_UNDEFINED;
volatile uint8_t oldStep = 0xfe;
volatile uint8_t stepCounter = 0;
volatile uint8_t item = PIPE;  // PIPE, SCREW
volatile uint8_t autoForward = FALSE;
volatile uint8_t vBACK_END = FALSE;
volatile uint8_t vSLOW = FALSE;
volatile uint8_t vFRONT_END = FALSE;
volatile uint8_t prSelProcedure = FALSE;

// ROTATION STEPS
#define STEP_MOTOR_STEPS 20  // divided by 10
#define STEP_MOTOR_STEP_DIVIDER 10
#define MOTO_REDUCTOR_RATIO 75  // multiplied by 10
#define SET_COUNT 6  // Number of sets
volatile uint16_t STEPS_FOR_TURN = STEP_MOTOR_STEPS*STEP_MOTOR_STEP_DIVIDER*MOTO_REDUCTOR_RATIO;
volatile uint16_t stepsForInterval = 2500;
volatile uint8_t loStepCount = 0;
volatile uint8_t hiStepCount = 0;
volatile uint8_t set = 1;
volatile uint16_t rotateTemp_low = 0;
volatile uint16_t rotateTemp_high = 0;
volatile uint16_t rotateSteps [SET_COUNT];
volatile uint16_t rotateStepsScrew [SET_COUNT];
volatile uint16_t rotateStepsPipe [SET_COUNT];

volatile uint16_t timeOut = 0;

// **********************************************
// **********        FUNCTIONS           ********
// **********************************************

// **********************************************
void readInputs(void){
	uint8_t port = 0;
	// BUTTONS
	port = PIND;
	// START
	if ((port & 0x80) == 0){
		if (bSTARTCounter > 0) bSTARTCounter --;
	} else {
		if (bSTARTCounter < 0xff) bSTARTCounter ++;
			}
	if (bSTARTCounter <= 0x80) bSTART = TRUE;
	else bSTART = FALSE;

	// STOP
	if ((port & 0x40) == 0){
		if (bSTOPCounter > 0) bSTOPCounter --;
	} else {
		if (bSTOPCounter < 0xff) bSTOPCounter ++;
			}
	if (bSTOPCounter <= 0x80) bSTOP = FALSE;
	else bSTOP = TRUE;

	// MAIN MOTOR FORWARD
	if ((port & 0x20) == 0){
		if (bMA_MO_FORWARDCounter > 0) bMA_MO_FORWARDCounter --;
	} else {
		if (bMA_MO_FORWARDCounter < 0xff) bMA_MO_FORWARDCounter ++;
			}
	if (bMA_MO_FORWARDCounter <= 0x80) bMA_MO_FORWARD = TRUE;
	else bMA_MO_FORWARD = FALSE;

	// MAIN MOTOR REVERSE
	if ((port & 0x10) == 0){
		if (bMA_MO_REVERSECounter > 0) bMA_MO_REVERSECounter --;
	} else {
		if (bMA_MO_REVERSECounter < 0xff) bMA_MO_REVERSECounter ++;
			}
	if (bMA_MO_REVERSECounter <= 0x80) bMA_MO_REVERSE = TRUE;
	else bMA_MO_REVERSE = FALSE;

	// ROTATE RIGHT
	if ((port & 0x08) == 0){
		if (bROTATE_RCounter > 0) bROTATE_RCounter --;
	} else {
		if (bROTATE_RCounter < 0xff) bROTATE_RCounter ++;
			}
	if (bROTATE_RCounter <= 0x80) bROTATE_R = TRUE;
	else bROTATE_R = FALSE;

	// ROTATE LEFT
	if ((port & 0x04) == 0){
		if (bROTATE_LCounter > 0) bROTATE_LCounter --;
	} else {
		if (bROTATE_LCounter < 0xff) bROTATE_LCounter ++;
			}
	if (bROTATE_LCounter <= 0x80) bROTATE_L = TRUE;
	else bROTATE_L = FALSE;

	// SENSORS
	port = PINB;
	// iBACK_END_SCREW
	if ((port & 0x20) == 0){
		if (iBACK_END_SCREWCounter > 0) iBACK_END_SCREWCounter --;
	} else {
		if (iBACK_END_SCREWCounter < 0xff) iBACK_END_SCREWCounter ++;
			}
	if (iBACK_END_SCREWCounter <= 0x80) iBACK_END_SCREW = TRUE;
	else iBACK_END_SCREW = FALSE;
	// iBACK_END_PIPE
	if ((port & 0x04) == 0){
		if (iBACK_END_PIPECounter > 0) iBACK_END_PIPECounter --;
	} else {
		if (iBACK_END_PIPECounter < 0xff) iBACK_END_PIPECounter ++;
			}
	if (iBACK_END_PIPECounter <= 0x80) iBACK_END_PIPE = TRUE;
	else iBACK_END_PIPE = FALSE;

	// i_SLOW_SCREW
	if ((port & 0x10) == 0){
		if (iSLOW_SCREWCounter > 0) iSLOW_SCREWCounter --;
	} else {
		if (iSLOW_SCREWCounter < 0xff) iSLOW_SCREWCounter ++;
			}
	if (iSLOW_SCREWCounter <= 0x80) iSLOW_SCREW = TRUE;
	else iSLOW_SCREW = FALSE;
	// i_SLOW_PIPE
	if ((port & 0x02) == 0){
		if (iSLOW_PIPECounter > 0) iSLOW_PIPECounter --;
	} else {
		if (iSLOW_PIPECounter < 0xff) iSLOW_PIPECounter ++;
			}
	if (iSLOW_PIPECounter <= 0x80) iSLOW_PIPE = TRUE;
	else iSLOW_PIPE = FALSE;

	// iFRONT_END_SCREW
	if ((port & 0x08) == 0){
		if (iFRONT_END_SCREWCounter > 0) iFRONT_END_SCREWCounter --;
	} else {
		if (iFRONT_END_SCREWCounter < 0xff) iFRONT_END_SCREWCounter ++;
			}
	if (iFRONT_END_SCREWCounter <= 0x80) iFRONT_END_SCREW = TRUE;
	else iFRONT_END_SCREW = FALSE;
	// iFRONT_END_PIPE
	if ((port & 0x01) == 0){
		if (iFRONT_END_PIPECounter > 0) iFRONT_END_PIPECounter --;
	} else {
		if (iFRONT_END_PIPECounter < 0xff) iFRONT_END_PIPECounter ++;
			}
	if (iFRONT_END_PIPECounter <= 0x80) iFRONT_END_PIPE = TRUE;
	else iFRONT_END_PIPE = FALSE;
}

// **********************************************
void writeOutputs(void){
	// STEP MOTOR DIRECTION
	if (ST_MO_DIR == RIGHT) ST_MO_DIR_RIGHT();
	if (ST_MO_DIR == LEFT) ST_MO_DIR_LEFT();

	// MAIN MOTOR FORWARD
	if (MA_MO_FORWARD == TRUE) MA_MO_FORWARD_ON();
	else MA_MO_FORWARD_OFF();

	// MAIN MOTOR REVERSE
	if (MA_MO_REVERSE == TRUE) MA_MO_REVERSE_ON();
	else MA_MO_REVERSE_OFF();

	// MAIN MOTOR SPEED
	if (MA_MO_SPEED == LOW) MA_MO_SLOW();
	else MA_MO_FAST();

	// PIPE / SCREW
	if (MA_MO_PIPE_SCREW == PIPE) MA_MO_PIPE();
	if (MA_MO_PIPE_SCREW == SCREW) MA_MO_SCREW();
}

// **********************************************
void st_mo_rotate(uint16_t low_step_count, uint16_t high_step_count){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;
	// Move back
	// On transition
	if ((bMA_MO_REVERSE == TRUE)&
		(st_mo_step_count <= low_step_count)){
		ST_MO_ROTATE = FALSE;
		step--;
	}
	// SLOW reverse rotate on iFRONT_END
	if (bMA_MO_REVERSE == TRUE){
		ST_MO_DIR = LEFT;
		ST_MO_ROTATE = TRUE;
	}

	// Move forward
	// SLOW rotate on iFRONT_END
	if (bMA_MO_FORWARD == TRUE){
		ST_MO_DIR = RIGHT;
		ST_MO_ROTATE = TRUE;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(st_mo_step_count >= high_step_count)){
		ST_MO_ROTATE = FALSE;
		step++;
	}
}

// **********************************************
void move_iBACK_END_to_iSLOW(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;
	// Move back
	// On transition
	if ((bMA_MO_REVERSE == TRUE)&
		(vBACK_END == TRUE)){
		MA_MO_REVERSE = FALSE;
		step--;
	}
	// till ZERO position
	if ((bMA_MO_REVERSE == TRUE)&
		(vBACK_END == FALSE)){
		MA_MO_REVERSE = TRUE;
	}

	// Move forward
	// Fast movement to iSLOW
	if ((bMA_MO_FORWARD == TRUE)&
		(vSLOW == FALSE)){
		MA_MO_FORWARD = TRUE;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(vSLOW == TRUE)){
		step++;
	}
}

// **********************************************
void move_iSLOW_to_iFRONT_END(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;
	// Move back
	// On transition
	if ((bMA_MO_REVERSE == TRUE)&
		(vSLOW == FALSE)){
		step--;
	}
	// SLOW movement till iSLOW
	if ((bMA_MO_REVERSE == TRUE)&
		(vSLOW == TRUE)){
		MA_MO_REVERSE = TRUE;
	}

	// Move forward
	// SLOW movement to iFRONT_END
	if ((bMA_MO_FORWARD == TRUE)&
		(vFRONT_END == FALSE)){
		MA_MO_FORWARD = TRUE;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(vFRONT_END == TRUE)){
		step++;
	}
}

// **********************************************
void move_iFRONT_END_to_iSLOW(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;
	// Move back
	// On transition
	if ((bMA_MO_REVERSE == TRUE)&
		(vFRONT_END == TRUE)){
		step--;
	}
	// SLOW movement till iSLOW
	if ((bMA_MO_REVERSE == TRUE)&
		(vFRONT_END == FALSE)){
		MA_MO_FORWARD = TRUE;
	}

	// Move forward
	// SLOW movement to iFRONT_END
	if ((bMA_MO_FORWARD == TRUE)&
		(vSLOW == TRUE)){
		MA_MO_REVERSE = TRUE;
		if (timeOut == 0) MA_MO_SPEED = HIGH;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(vSLOW == FALSE)){
		step++;
	}
}

// **********************************************
void move_iSLOW_to_iBACK_END(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;
	// Move back
	// On transition
	if ((bMA_MO_REVERSE == TRUE)&
		(vSLOW == TRUE)){
		MA_MO_FORWARD = FALSE;
		step--;
	}
	// Fast movement to iSLOW
	if ((bMA_MO_REVERSE == TRUE)&
		(vSLOW == FALSE)){
		MA_MO_FORWARD = TRUE;
	}

	// Move forward
	// Fast movement to iBACK_END
	if ((bMA_MO_FORWARD == TRUE)&
		(vBACK_END == FALSE)){
		MA_MO_REVERSE = TRUE;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(vBACK_END == TRUE)){
		MA_MO_REVERSE = FALSE;
		step = 0;
	}
}

// Load settings from EEPROM
void loadProgramSettings(uint8_t r_set){
	uint8_t loc_i = 0;
	uint16_t* eeStartAddress = 0;
	uint16_t* eeAddress = 0;

	eeStartAddress = (uint16_t*)(r_set * 6 * 2 * 2);  // r_set * paramInSet * uint16 * 2(SCREW+PIPE)
	for (loc_i = 0; loc_i < 6; loc_i++){
		eeAddress = eeStartAddress + (loc_i * 2);
		rotateStepsScrew [loc_i] = eeprom_read_word(eeAddress);
		eeAddress = eeStartAddress + (6 * 2) + (loc_i * 2);
		rotateStepsPipe [loc_i] = eeprom_read_word(eeAddress);
	}
}

// **********************************************
// **********       MAIN LOOP            ********
// **********************************************
int main(){
	// Initialization
	// PORTS
	DDR_B();
	DDR_C();
	DDR_D();

	// SCREW/PIPE, SET 1
	item = eeprom_read_byte(&EE_ITEM);
	set = eeprom_read_byte(&EE_SET);
	loadProgramSettings(set);

	// Interrupts
	t0initialization();
	sei();

// ********** MAIN LOOP ********
while (1){
	readInputs();

	// ***********  ITEM  *************
	// SCREW = STOP + MA_MO_FORWARD
	if ((bSTOP == TRUE)&
		(bMA_MO_FORWARD == TRUE)){
		item = SCREW;
		eeprom_update_byte(&EE_ITEM, item);
	}
	// PIPE = STOP + MA_MO_REVERSE
	if ((bSTOP == TRUE)&
		(bMA_MO_REVERSE == TRUE)){
		item = PIPE;
		eeprom_update_byte(&EE_ITEM, item);
	}
	// Load rotations
	if (item == SCREW){
		for (i = 0; i < 6; i++){
			rotateSteps [i] = rotateStepsScrew [i];
		}
		MA_MO_PIPE_SCREW = SCREW;
	}
	if (item == PIPE){
		for (i = 0; i < 6; i++){
			rotateSteps [i] = rotateStepsPipe [i];
		}
		MA_MO_PIPE_SCREW = PIPE;
	}

	// vXX = item & iXX
	vBACK_END = FALSE;
	if ((item == SCREW)&
		(iBACK_END_SCREW == TRUE)) vBACK_END = TRUE;
	if ((item == PIPE)&
		(iBACK_END_PIPE == TRUE)) vBACK_END = TRUE;

	vSLOW = FALSE;
	if ((item == SCREW)&
		(iSLOW_SCREW == TRUE)) vSLOW = TRUE;
	if ((item == PIPE)&
		(iSLOW_PIPE == TRUE)) vSLOW = TRUE;

	vFRONT_END = FALSE;
	if ((item == SCREW)&
		(iFRONT_END_SCREW == TRUE)) vFRONT_END = TRUE;
	if ((item == PIPE)&
		(iFRONT_END_PIPE == TRUE)) vFRONT_END = TRUE;

	// ******* PRORAM SELECTION *******

	// START PROGRAM SELECTION procedure - ROTATION
	if ((bSTOP == TRUE)&
		(bROTATE_R == TRUE)&
		(prSelProcedure == FALSE)){
		prSelProcedure = TRUE;
		st_mo_step_count = 0;  // RESET STEP COUNTER
	}

	// EXIT form PROGRMA SELECTION
	if ((bSTOP == FALSE)&
		(prSelProcedure = TRUE)){
		prSelProcedure = FALSE;
		stepsForInterval = STEPS_FOR_TURN / SET_COUNT;
		for (i = 0; i < SET_COUNT; i++){
			loStepCount = i * stepsForInterval;
			hiStepCount = (i+1) * stepsForInterval;
			if ((st_mo_step_count > loStepCount)&
				(st_mo_step_count <= hiStepCount)){
				loadProgramSettings(i);
				eeprom_update_byte(&EE_SET, i);
			}
		}
	}

	// ********** PRORAM LOGIC ********
	// ZERO position detected
	if (vBACK_END == TRUE){
		autoForward = FALSE;
		st_mo_step_count = 0;
	}

	// latch Auto Forward
	if ((bSTART == TRUE)&
		(vBACK_END == TRUE)){
		autoForward = TRUE;
		step = 0;
	}
	if ((bSTOP == TRUE)|
		(bMA_MO_FORWARD == TRUE)|
		(bMA_MO_REVERSE == TRUE)|
		(bROTATE_R == TRUE)|
		(bROTATE_L == TRUE)){
		autoForward = FALSE;
	}

	// Auto FORWARD
	if (autoForward == TRUE) bMA_MO_FORWARD = TRUE;

	// SLOW detection
	if (vSLOW == TRUE) {
		MA_MO_SPEED = LOW;
		ST_MO_SPEED = LOW;
	}
	else {
		MA_MO_SPEED = HIGH;
		ST_MO_SPEED = HIGH;
	}

	stepCounter = 0;
	// ZERO position
	if (step == stepCounter){
		// Move forward
		if (bMA_MO_FORWARD == TRUE) step++;
	}

	stepCounter++;
	// Movement between iBACK_END and iSLOW
	if (step == stepCounter){
		move_iBACK_END_to_iSLOW();
	}

	// FIRST
	stepCounter++;
	// FIRST Movement between iSLOW and iFRONT_END
	if (step == stepCounter){
		move_iSLOW_to_iFRONT_END();
	}

	stepCounter++;
	if (step == stepCounter){
		timeOut = 20000;
		step++;
	}

	stepCounter++;
	// Movement between iFRONT_END and iSLOW
	if (step == stepCounter){
		move_iFRONT_END_to_iSLOW();
	}

	stepCounter++;
	//Rotation
	if (step == stepCounter){
		rotateTemp_low = 0;
		rotateTemp_high = 5000;
		st_mo_rotate(rotateTemp_low, rotateTemp_high); // rotate set 6
	}

	stepCounter++;
	// FIRST Movement between iSLOW and iFRONT_END
	if (step == stepCounter){
		move_iSLOW_to_iFRONT_END();
	}

	stepCounter++;
	if (step == stepCounter){
		timeOut = 20000;
		step++;
	}

	stepCounter++;
	// Movement between iFRONT_END and iSLOW
	if (step == stepCounter){
		move_iFRONT_END_to_iSLOW();
	}

	stepCounter++;
	//Rotation
	if (step == stepCounter){
		rotateTemp_low = 5000;
		rotateTemp_high = 10000;
		st_mo_rotate(rotateTemp_low, rotateTemp_high); // rotate set 6
	}

	stepCounter++;
	// Movement between iSLOW and iFRONT_END
	if (step == stepCounter){
		move_iSLOW_to_iFRONT_END();
	}

	stepCounter++;
	if (step == stepCounter){
		timeOut = 20000;
		step++;
	}

	stepCounter++;
	// Movement between iFRONT_END and iSLOW
	if (step == stepCounter){
		move_iFRONT_END_to_iSLOW();
	}

	stepCounter++;
	// Movement between iSLOW and iBACK_END
	if (step == stepCounter){
		move_iSLOW_to_iBACK_END();
	}

	// MANUAL MODE
	if (step == L_UNDEFINED){

		if ((bMA_MO_FORWARD == TRUE)&
			(vFRONT_END == FALSE)){
			MA_MO_FORWARD = TRUE;
		} else MA_MO_FORWARD = FALSE;

		if ((bMA_MO_REVERSE == TRUE)&
			(vBACK_END == FALSE)){
			MA_MO_REVERSE = TRUE;
		} else MA_MO_REVERSE = FALSE;

		// STEP motor
		if (bROTATE_R == TRUE){
			ST_MO_DIR = RIGHT;
		}

		// STEP motor
		if (bROTATE_L == TRUE){
			ST_MO_DIR = LEFT;
		}

		if ((bROTATE_L == TRUE)|
			(bROTATE_R == TRUE)){
			ST_MO_ROTATE = TRUE;
		} else ST_MO_ROTATE = FALSE;
	}

	// STOP
	if (bSTOP == TRUE){
		MA_MO_FORWARD = FALSE;
		MA_MO_REVERSE = FALSE;
		if (prSelProcedure != TRUE) ST_MO_ROTATE = FALSE;
		step = L_UNDEFINED;
	}

	// ********* END OF PROGRAM ********
	writeOutputs();
}}

// ********** INTERRUPT ********
ISR(TIMER0_OVF_vect)
{
	// 100 us task
	t0reload();

	st_mo_generatePulse = FALSE;

	if (ST_MO_SPEED == HIGH){
		st_mo_speedDivider = ST_MO_HIGH_SPEED_DIVIDER;
	}
	if (ST_MO_SPEED == LOW){
		st_mo_speedDivider = ST_MO_LOW_SPEED_DIVIDER;
		if (st_mo_pulseWidthMultiplier < st_mo_speedDivider){
			st_mo_pulseWidthMultiplier = st_mo_speedDivider;
		}
	}

	if (ST_MO_ROTATE == TRUE){
		st_mo_puseWidth++;
		if (st_mo_puseWidth >= st_mo_pulseWidthMultiplier){
			if (st_mo_pulseWidthMultiplier > st_mo_speedDivider) {
				st_mo_pulseWidthMultiplier--;  // Ramp up
			}
			// Generate Pulse
			st_mo_generatePulse = TRUE;
		}
	} else {
		if (st_mo_puseWidth >= st_mo_pulseWidthMultiplier){
			if (st_mo_pulseWidthMultiplier < ST_MO_RAMP_STEPS_MAX){
				st_mo_pulseWidthMultiplier++;  // Ramp down
				// Generate Pulse
				st_mo_generatePulse = TRUE;
			}
		} else st_mo_puseWidth++;
	}

	// Generate Pulse
	if (st_mo_generatePulse == TRUE){
		st_mo_generatePulse = FALSE;
		ST_MO_PULSE_TRG();

		if (pulse == TRUE){
			pulse = FALSE;
			if ((ST_MO_DIR == RIGHT)&
				(st_mo_step_count < 0xffff)){
				st_mo_step_count++;
			}
			if ((ST_MO_DIR == LEFT)&
				(st_mo_step_count > 0)){
				st_mo_step_count--;
			}
		} else pulse = TRUE;

		st_mo_puseWidth = 0;
	}
	if (timeOut > 0) timeOut--;
}
