/*
 * main.c
 *
 *  Created on: 14.03.2017
 *      Author: autoMaster
 */

/*
 * Pinout Arduino Mega2560
 *
 *  Function            Arduino         ATmega
 * bST_MO_ROTATION_L		22			PA0
 * bST_MO_ROTATION_R		23			PA1
 * bMA_MO_REVERSE			24			PA2
 * bMA_MO_FORWADR			25			PA3
 * bSTOP					26			PA4
 * bSTART					27			PA5
 * iBackEnd_PIPE			28			PA6
 * iSLOW_PIPE				29			PA7
 * iFrontEnd_PIPE			34			PC3
 * iBackEnd_SCREW			37			PC0
 * iSLOW_SCREW				36			PC1
 * iFrontEnd_SCREW			35			PC2
 *
 * ST_MO_PULSE				49			PL0
 * ST_MO_DIR				48			PL1
 *
 * DISPALY					A8-A15		PK0-7
 *
 * RELAYS
 * MA_MO_FORWARD			A0			PF0
 * MA_MO_REVERSE			A1			PF1
 * MA_MO_SLOW				A2			PF2
 * MA_MO_ITEM				A3			PF3
 * reserved					A4			PF4
 * reserved					A5			PF5
 * reserved					A6			PF6
 * reserved					A7			PF7
 *
 */


#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/EEPROM.h>
#include "timer0.h"
#import "HD44780.h"

// 1 - Output
// 0 - Input
#define		DDR_A()			DDRA = 0; PORTA = 0xFF // All PINS are inputs + PullUps

#define		DDR_B()			DDRB |= (1<<PB7)
#define		LED_STRING_1_TRG()		PORTB ^= (1<<DDB7) // change state

#define		DDR_C()			DDRC = 0; PORTC = 0xFF // All PINS are inputs + PullUps

#define		DDR_F()			DDRF = 0xFF; PORTF = 0xFF;  // All outputs
#define		MA_MO_FORWARD_ON()	PORTF &=~(1<<DDF0) // 0
#define		MA_MO_FORWARD_OFF()	PORTF |= (1<<DDF0) // 1
#define		MA_MO_REVERSE_ON()	PORTF &=~(1<<DDF1) // 0
#define		MA_MO_REVERSE_OFF()	PORTF |= (1<<DDF1) // 1
#define		MA_MO_SLOW()		PORTF &=~(1<<DDF2) // 0
#define		MA_MO_FAST()		PORTF |= (1<<DDF2) // 1
#define		MA_MO_PIPE()		PORTF &=~(1<<DDF3) // 0
#define		MA_MO_SCREW()		PORTF |= (1<<DDF3) // 1

#define		DDR_L()			DDRL = 0xFF; PORTL = 0xFF;  // All outputs
#define		ST_MO_PULSE_TRG()	PORTL ^= (1<<DDL0) // change state
#define		ST_MO_DIR_LEFT()	PORTL &=~(1<<DDL1) // 0
#define		ST_MO_DIR_RIGHT()	PORTL |= (1<<DDL1) // 1
#define		ST_MO_CLUTCH_OFF()	PORTL &=~(1<<DDL2) // 0
#define		ST_MO_CLUTCH_ON()	PORTL |= (1<<DDL2) // 1

// Iteration
volatile uint8_t i = 0;

// Delay
volatile uint8_t delay8bit = 0;
volatile uint16_t timeOut = 0;
volatile uint16_t time3sec = 30000;

// BOOL
#define FALSE 0
#define TRUE 1
#define SCREW 0
#define PIPE 1
// **********************************************
typedef struct {
	volatile uint8_t counter;		// counter of input state
	volatile uint8_t counterMax;	// max counter value
	volatile uint8_t counterMid;	// Output: mid + his = TRUE; mid - his = FALSE
	volatile uint8_t counterMin;	// min counter value
	volatile uint8_t hysteresis;	// Hysteresis
	volatile uint8_t f_input;		//
	volatile uint8_t ss_input;		// single shot - TRUE for one cycle
} INPUT_FILTER;

INPUT_FILTER inputsPortA[8];  // Buttons
INPUT_FILTER inputsPortC[8];  // Sensors

// Display
volatile uint8_t refreshDisplay = TRUE;
#define MENU_PRACA 0
#define MENU_PRG_MAT_ENT 1
#define MENU_PRG_ENT_SET 2
volatile uint8_t activeMenu = MENU_PRACA;
volatile char menuPrgMatWja[] =
	{'P', ' ', 'M', 'a', 't', ':', 'S', 'R', 'U', 'B', 'Y', ' ', ' ', ' ', ' ', ' ',
	 'P', ' ', 'W', 'j', 'a', ':', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

volatile char menuPrgWjaNr[] =
	{'P', ' ', 'M', 'M', 'M', 'M', 'M', ':', '-', '-', '-', '-', '-', ' ', 'W', '1',
	 'P', ' ', '0', '0', '0', '0', '0', ':', '0', '0', '0', '0', '0', ' ', ' ', ' '};

volatile char menuPraca[] =
	{'M', ' ', 'S', 'R', 'U', 'B', 'Y', ' ', ' ', 'W', 'j', 'a', ':', '1', ' ', ' ',
	 'M', ' ', '0', '0', '0', '0', '0', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

volatile char screws[] = {'S', 'R', 'U', 'B', 'Y'};
volatile char pipes[] = {'R', 'U', 'R', 'K', 'I'};
volatile uint8_t decTab[][5] = {
		{0, 0, 0, 0, 1},
		{0, 0, 0, 0, 2},
		{0, 0, 0, 0, 4},
		{0, 0, 0, 0, 8},
		{0, 0, 0, 1, 6},
		{0, 0, 0, 3, 2},
		{0, 0, 0, 6, 4},
		{0, 0, 1, 2, 8},
		{0, 0, 2, 5, 6},
		{0, 0, 5, 1, 2},
		{0, 1, 0, 2, 4},
		{0, 2, 0, 4, 8},
		{0, 4, 0, 9, 6},
		{0, 8, 1, 9, 2},
		{1, 6, 3, 8, 4},
		{3, 2, 7, 6, 8}};

// **********************************************
volatile uint8_t cursorPosition = 0;
volatile uint8_t bMOVE_CUR_R = FALSE;
volatile uint8_t bINC_DIGIT = FALSE;
// **********************************************
volatile uint8_t currentEntrance = 0;  // 0..8
volatile uint8_t allEntrances = 3;  // 0..8
volatile uint8_t entranceSettings[][10] = {
		{0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 2, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 3, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 4, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 5, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 6, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 7, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 8, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 9, 0, 0, 0}
};

// **********************************************
uint8_t EEMEM ee_eepromProgramed;
uint8_t EEMEM ee_allEntrances;
uint8_t EEMEM ee_item;
uint8_t EEMEM ee_entranceSettings[9][10];

// **********************************************

// INPUTS
volatile uint8_t bSTART = FALSE;
volatile uint8_t bSTOP = TRUE;
volatile uint8_t bMA_MO_FORWARD = FALSE;
volatile uint8_t bMA_MO_REVERSE = FALSE;
volatile uint8_t bROTATE_R = FALSE;
volatile uint8_t bROTATE_L = FALSE;
volatile uint8_t iBACK_END_PIPE = FALSE;
volatile uint8_t iBACK_END_SCREW = FALSE;
volatile uint8_t iSLOW_PIPE = FALSE;
volatile uint8_t iSLOW_SCREW = FALSE;
volatile uint8_t iFRONT_END_PIPE = FALSE;
volatile uint8_t iFRONT_END_SCREW = FALSE;

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
volatile uint16_t rotationSteps = 0;

// LOGIC
#define L_UNDEFINED 0xff
#define L_MANUAL 0xfe
#define L_PROGRAMING 0xfd
volatile uint8_t step = L_UNDEFINED;
volatile uint8_t stepCounter = 0;
volatile uint8_t item = PIPE;  // PIPE, SCREW
volatile uint8_t autoForward = FALSE;
volatile uint8_t vBACK_END = FALSE;
volatile uint8_t vSLOW = FALSE;
volatile uint8_t vFRONT_END = FALSE;
#define NORMAL 0
#define PROGRAMING 1
volatile uint8_t mode = NORMAL;

// **********************************************
// **********************************************
// **********        FUNCTIONS           ********
// **********************************************

// **********************************************
// **********        FILTER INPUTS       ********
// **********************************************
void filterInput(uint8_t raw_input, INPUT_FILTER* p_portSet){
	if (raw_input != 0){
		if (p_portSet->counter < p_portSet->counterMax) p_portSet->counter++;
	} else {
		if (p_portSet->counter > p_portSet->counterMin) p_portSet->counter--;
	}

	if (p_portSet->ss_input == TRUE) p_portSet->ss_input = FALSE;

	if ((p_portSet->f_input == TRUE)&
		(p_portSet->counter < (p_portSet->counterMid - p_portSet->hysteresis))){
		p_portSet->f_input = FALSE;
		p_portSet->ss_input = TRUE;
	}
	if ((p_portSet->f_input == FALSE)&
		(p_portSet->counter > (p_portSet->counterMid + p_portSet->hysteresis))){
		p_portSet->f_input = TRUE;
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
// **********************************************
// **********    HEX2DEC  & SUM    **************
// **********************************************
// @param dg1, dg2   - Dec didits to add
// @return sum		 - Dec sum with
uint8_t sumDec2digits(uint8_t dg1, uint8_t dg2, uint8_t carry){
	uint8_t decSum = 0;
	decSum = dg1 + dg2 + carry;
	if (decSum > 9){
		decSum = decSum + 6;
	}
	return decSum;
}

// **********************************************
// @param tdg1, tdg2 		- table of dec digits to add
// @param tDecSum			- table of dec sum of digits
void sumDecDigs (uint8_t* tdg1, uint8_t* tdg2, uint8_t* tDecSum){
	uint8_t i = 0;
	uint8_t sum = 0;
	uint8_t crr = 0;
	for (i=5; i>0; i--){
		sum = sumDec2digits(*(tdg1 + i - 1), *(tdg2 + i - 1), crr);
		*(tDecSum + i - 1) = sum & 0x0F;
		crr = sum >> 4;
	}
}

// **********************************************
void hex2dec16bit(uint16_t hexVal, uint8_t* decV){
	uint8_t li = 0;
	for (li=0; li<=15; li++){
		if (((hexVal >> li) & 0x01) == 1){
			sumDecDigs(((uint8_t*)&decTab+li*5) , decV, decV);
		}
	}
}
// **********************************************

// **********************************************
// **********    DISPLAY   2x16    **************
// **********************************************
// Display 2 line Menu
void display2x16lineText(char* text){
	char lineOfText [17];

	LCD_GoTo(0 , 0);
	for (i=0; i<=15; i++){
		lineOfText[i] = *(text + i);
	}
	lineOfText[16] = 0;
	LCD_WriteText((char*)&lineOfText);

	LCD_GoTo(0 , 1);
	for (i=16; i<=31; i++){
		lineOfText[i-16] = *(text + i);
	}
	lineOfText[16] = 0;
	LCD_WriteText((char*)&lineOfText);
}
// **********************************************
void updateMenuPraca(uint8_t autoF, uint8_t item, uint8_t entrance,
					 uint16_t stepMotorCounter, char* menu){
	uint8_t ind = 0;
	uint8_t decimalValue[] = {0, 0, 0, 0, 0};

	// AUTO-MANUAL
	if (autoF == TRUE){
		*(menu) = 'A';
		*(menu + 16) = 'A';
	} else {
		*(menu) = 'M';
		*(menu + 16) = 'M';
	}

	// ITEM
	if (item == PIPE){
		for (ind = 0; ind <= 4; ind++){
			*(menu + 2 + ind) = pipes[ind];
		}
	} else {
		for (ind = 0; ind <= 4; ind++){
			*(menu + 2 + ind) = screws[ind];
		}
	}

	// Entrance
	*(menu + 13) = ((entrance + 1) & 0x0F) + 0x30;

	// Step Motor Counter
	hex2dec16bit(stepMotorCounter, (uint8_t*)&decimalValue);
	for (ind = 0; ind <= 4; ind++){
		*(menu + 18 + ind) = decimalValue[ind] + 0x30;
	}
	display2x16lineText(menu);
}

// **********************************************
void updateMenuPrgMatWja(uint8_t item, uint8_t entrance, char* menu){
	uint8_t ind = 0;
	// ITEM
	if (item == PIPE){
		for (ind = 0; ind <= 4; ind++){
			*(menu + 6 + ind) = pipes[ind];
		}
	} else {
		for (ind = 0; ind <= 4; ind++){
			*(menu + 6 + ind) = screws[ind];
		}
	}

	// Entrance
	*(menu + 22) = ((entrance + 1) & 0x0F) + 0x30;

	display2x16lineText(menu);
}

// **********************************************
void updateMenuPrgWjaNr(uint8_t entrance, uint8_t* stepsSettings, char* menu){
	uint8_t ind = 0;

	// Entrance
	*(menu + 15) = ((entrance + 1) & 0x0F) + 0x30;

	// STEPS
	for (ind = 0; ind <= 4; ind++){
		*(menu + 18 + ind) = (*(stepsSettings + (entrance * 10) + ind) + 0x30);
		*(menu + 24 + ind) = (*(stepsSettings + (entrance * 10) + 5 + ind) + 0x30);
	}

	display2x16lineText(menu);
}
// **********************************************

// **********************************************
// **********        CURSOR        **************
// **********************************************
void moveCursor(uint8_t* pos){
	uint8_t xPos = 0;
	uint8_t yPos = 0;
	if (*pos == 0) {xPos = 2; yPos = 1;}
	if (*pos == 1) {xPos = 3; yPos = 1;}
	if (*pos == 2) {xPos = 4; yPos = 1;}
	if (*pos == 3) {xPos = 5; yPos = 1;}
	if (*pos == 4) {xPos = 6; yPos = 1;}
	if (*pos == 5) {xPos = 8; yPos = 1;}
	if (*pos == 6) {xPos = 9; yPos = 1;}
	if (*pos == 7) {xPos = 10; yPos = 1;}
	if (*pos == 8) {xPos = 11; yPos = 1;}
	if (*pos == 9) {xPos = 12; yPos = 1;}
	if (*pos == 10) {xPos = 15; yPos = 0;}
	if (*pos == 11) {xPos = 6; yPos = 0;}
	if (*pos == 12) {xPos = 6; yPos = 1;}

	LCD_GoTo(xPos, yPos);
}
// **********************************************
void moveCursorRight(uint8_t* crPos){
	if (*crPos >= 10) *crPos = 0;
	else {
		*crPos = *crPos + 1;
	}
	moveCursor(crPos);
}
// **********************************************
void moveCursorLeft(uint8_t* crPos){
	if (*crPos == 0) *crPos = 10;
	else {
		*crPos = *crPos - 1;
	}
	if (*crPos > 10) *crPos = 0;
	moveCursor(crPos);
}

// **********************************************
// **********     DIGITs CHANGE    **************
// **********************************************
void incDigitOnSteps(uint8_t entrance, uint8_t crPos, uint8_t* entranceSettings){
	uint8_t temp = 0;
	temp = *(entranceSettings + (entrance * 10) + crPos);
	if (temp >= 9) temp = 0;
	else temp++;
	*(entranceSettings + (entrance * 10) + crPos) = temp;
}

// **********************************************
void decDigitOnSteps(uint8_t entrance, uint8_t crPos, uint8_t* entranceSettings){
	uint8_t temp = 0;
	temp = *(entranceSettings + (entrance * 10) + crPos);
	if (temp == 0) temp = 9;
	else temp--;
	if (temp > 9) temp = 0;
	*(entranceSettings + (entrance * 10) + crPos) = temp;
}

// **********************************************
void incDigitEntrance(uint8_t* digit){
	if (*digit >= 8) *digit = 0;
	else {
		*digit = *digit + 1;
	}
}
// **********************************************
void decDigitEntrance(uint8_t* digit){
	if (*digit == 0) *digit = 8;
	else {
		*digit = *digit - 1;
	}
}

// **********************************************
void updateDisplay(void){
	if (activeMenu == MENU_PRG_ENT_SET){
		updateMenuPrgWjaNr(currentEntrance, (uint8_t*)&entranceSettings, (char*)&menuPrgWjaNr);
		moveCursor((uint8_t*)&cursorPosition);
	}

	if (activeMenu == MENU_PRG_MAT_ENT){
		updateMenuPrgMatWja(item, allEntrances, (char*)&menuPrgMatWja);
		moveCursor((uint8_t*)&cursorPosition);
	}

	if (activeMenu == MENU_PRACA){
		updateMenuPraca(autoForward, item, currentEntrance, st_mo_step_count, (char*)&menuPraca);
	}
}
// **********************************************
void buttonsHandling(){
	if (activeMenu == MENU_PRG_ENT_SET){
		if (bMOVE_CUR_R == TRUE){
			moveCursorRight((uint8_t*)&cursorPosition);
		}

		if (bINC_DIGIT == TRUE){
			if (cursorPosition == 10){
				incDigitEntrance((uint8_t*)&currentEntrance);
			} else {
				incDigitOnSteps(currentEntrance, cursorPosition, (uint8_t*)&entranceSettings);
			}
		}
	}

	if (activeMenu == MENU_PRG_MAT_ENT){
		if (bINC_DIGIT == TRUE){
			if (cursorPosition == 12){
				incDigitEntrance((uint8_t*)&allEntrances);
			} else {
				if (item == PIPE) item = SCREW;
				else item = PIPE;
			}
		}

		if (bMOVE_CUR_R == TRUE){
			if (cursorPosition == 11) cursorPosition = 12;
			else cursorPosition = 11;
		}
	}
}

// **********************************************
// **********       EEPROM         **************
// **********************************************
void initializeEEPROM(void){
	uint8_t eeprom;
	eeprom = eeprom_read_byte(&ee_eepromProgramed);
	if (eeprom != 0x55){
		eeprom_write_byte((uint8_t*)&ee_allEntrances, 8);
		eeprom_write_byte((uint8_t*)&ee_item, PIPE);
		eeprom_write_block((uint8_t*)&entranceSettings, (uint8_t*)&ee_entranceSettings, 90);
		eeprom_write_byte((uint8_t*)&ee_eepromProgramed, 0x55);
	}
}

// **********************************************
void saveSettingsToEEPROM(void){
	eeprom_write_byte((uint8_t*)&ee_allEntrances, allEntrances);
	eeprom_write_byte((uint8_t*)&ee_item, item);
	eeprom_write_block((uint8_t*)&entranceSettings, (uint8_t*)&ee_entranceSettings, 90);
}

// **********************************************
void readSettingsFromEEPROM(void){
	allEntrances = eeprom_read_byte(&ee_allEntrances);
	item = eeprom_read_byte(&ee_item);
	eeprom_read_block((uint8_t*)&entranceSettings, (uint8_t*)&ee_entranceSettings, 90);
}
// **********************************************
// **********************************************

// **********************************************
uint16_t hexMMrotation(uint8_t crrEntrance){
	uint16_t ret = 0;
	ret = entranceSettings[crrEntrance][4];
	ret = ret + (entranceSettings[crrEntrance][3] * 10);
	ret = ret + (entranceSettings[crrEntrance][2] * 100);
	ret = ret + (entranceSettings[crrEntrance][1] * 1000);
	ret = ret + (entranceSettings[crrEntrance][0] * 10000);
	return ret;
}

uint16_t hexRotation(uint8_t crrEntrance){
	uint16_t ret = 0;
	ret = entranceSettings[crrEntrance][9];
	ret = ret + (entranceSettings[crrEntrance][8] * 10);
	ret = ret + (entranceSettings[crrEntrance][7] * 100);
	ret = ret + (entranceSettings[crrEntrance][6] * 1000);
	ret = ret + (entranceSettings[crrEntrance][5] * 10000);
	return ret;
}
// **********************************************
void readInputs(void){

	// BUTTONS
	filterInputs(PINA, inputsPortA);

	if (inputsPortA[0].f_input == FALSE) bROTATE_L = TRUE;
	else bROTATE_L = FALSE;

	if (inputsPortA[1].f_input == FALSE) bROTATE_R = TRUE;
	else bROTATE_R = FALSE;

	if (inputsPortA[2].f_input == FALSE) bMA_MO_REVERSE = TRUE;
	else bMA_MO_REVERSE = FALSE;

	if (inputsPortA[3].f_input == FALSE) bMA_MO_FORWARD = TRUE;
	else bMA_MO_FORWARD = FALSE;

	if (inputsPortA[4].f_input == FALSE) bSTOP = FALSE;
	else bSTOP = TRUE;

	if (inputsPortA[5].f_input == FALSE) bSTART = TRUE;
	else bSTART = FALSE;

	// SENSORS
	if (inputsPortA[6].f_input == FALSE) iBACK_END_PIPE = TRUE;
	else iBACK_END_PIPE = FALSE;

	if (inputsPortA[7].f_input == FALSE) iSLOW_PIPE = TRUE;
	else iSLOW_PIPE = FALSE;


	filterInputs(PINC, inputsPortC);

	if (inputsPortC[0].f_input == FALSE) iBACK_END_SCREW = TRUE;
	else iBACK_END_SCREW = FALSE;

	if (inputsPortC[1].f_input == FALSE) iSLOW_SCREW = TRUE;
	else iSLOW_SCREW = FALSE;

	if (inputsPortC[2].f_input == FALSE) iFRONT_END_SCREW = TRUE;
	else iFRONT_END_SCREW = FALSE;

	if (inputsPortC[3].f_input == FALSE) iFRONT_END_PIPE = TRUE;
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

	// CLUTCH
	if (ST_MO_ROTATE == FALSE) ST_MO_CLUTCH_ON();
	else ST_MO_CLUTCH_OFF();
}

// **********************************************
void move_iBACK_END_to_iSLOW(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;

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

	// SLOW movement to iFRONT_END
	if ((bMA_MO_FORWARD == TRUE)&
		(vFRONT_END == FALSE)){
		MA_MO_FORWARD = TRUE;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(vFRONT_END == TRUE)){
		rotationSteps = st_mo_step_count + hexMMrotation(currentEntrance);  // MMMMM Rotation
		step++;
	}
}

// **********************************************
void move_iFRONT_END_to_iSLOW(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;

	// SLOW movement to iFRONT_END
	if ((bMA_MO_FORWARD == TRUE)&
		(vSLOW == TRUE)){
		MA_MO_REVERSE = TRUE;
		if (timeOut == 0) MA_MO_SPEED = HIGH;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(vSLOW == FALSE)){
		rotationSteps = st_mo_step_count + hexRotation(currentEntrance);  // ----- Rotation
		step++;
	}
}

// **********************************************
void move_iSLOW_to_iBACK_END(){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;

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

// **********************************************
void st_mo_rotate(uint16_t end_step_count){
	MA_MO_FORWARD = FALSE;
	MA_MO_REVERSE = FALSE;
	ST_MO_ROTATE = FALSE;

	// Rotate
	if (bMA_MO_FORWARD == TRUE){
		ST_MO_DIR = RIGHT;
		ST_MO_ROTATE = TRUE;
	}
	// On transition
	if ((bMA_MO_FORWARD == TRUE)&
		(st_mo_step_count >= end_step_count)){
		ST_MO_ROTATE = FALSE;
		step++;
	}
}
// **********************************************
// **********       MAIN PROGRAM         ********
// **********************************************

int main(){
	// Initialization

	// PORTS
	DDR_A();  // in BUTTONS
	DDR_B();  // Out misc
	DDR_C();  // in SENSORS
	DDR_F();  // Out StepMotor, Clutch
  //DDR_K();  // Out DISPLAY. Initialized in HD44780.h
	DDR_L();  // Out Main Motor, Item

	initializeInputFilter();

	initializeEEPROM();
	readSettingsFromEEPROM();

	LCD_Initalize();

	// Interrupts
	t0initialization();
	sei();

	// ********** MAIN LOOP ********
while (1){
	readInputs();

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
		currentEntrance = 0;
	}
	if (((bMA_MO_FORWARD == TRUE)|
		 (bMA_MO_REVERSE == TRUE)|
		 (bROTATE_R == TRUE)|
		 (bROTATE_L == TRUE))&
		(autoForward == TRUE)){
		autoForward = FALSE;
		step = L_UNDEFINED;
	}

	if (mode == PROGRAMING) step = L_PROGRAMING;

	// SLOW detection
	if (vSLOW == TRUE) {
		MA_MO_SPEED = LOW;
		ST_MO_SPEED = LOW;
	}
	else {
		MA_MO_SPEED = HIGH;
		ST_MO_SPEED = HIGH;
	}

	// Auto FORWARD
	if (autoForward == TRUE){

		bMA_MO_FORWARD = TRUE;

		stepCounter = 0;
		// ZERO position
		if (step == stepCounter){
			// Move forward
			if (bMA_MO_FORWARD == TRUE) step++;
		}

		stepCounter = 1;
		// Movement between iBACK_END and iSLOW
		if (step == stepCounter){
			move_iBACK_END_to_iSLOW();
		}

		// Begin of loop
		stepCounter = 2;
		// Movement between iSLOW and iFRONT_END
		if (step == stepCounter){
			move_iSLOW_to_iFRONT_END();
		}

		stepCounter++;
		// MMMMM Rotation
		if (step == stepCounter){
			st_mo_rotate(rotationSteps);
		}

		stepCounter++;
		// Timeout for slow move back for SCREW
		if (step == stepCounter){
			if (item == SCREW) timeOut = 20000;  // t * 100us
			step++;
		}

		stepCounter++;
		// Movement between iFRONT_END and iSLOW
		if (step == stepCounter){
			move_iFRONT_END_to_iSLOW();
		}

		stepCounter++;
		// ----- Rotation
		if (step == stepCounter){
			st_mo_rotate(rotationSteps);
		}

		stepCounter++;
		// Loop
		if (step == stepCounter){
			if (currentEntrance < allEntrances){
				currentEntrance++;
				step = 2;  // jump to begin of loop
			} else {
				step++;  // End of loop
			}
		}


		stepCounter++;
		// Movement between iSLOW and iBACK_END
		if (step == stepCounter){
			move_iSLOW_to_iBACK_END();
		}
	} else {
		MA_MO_FORWARD = FALSE;
		MA_MO_REVERSE = FALSE;
		ST_MO_ROTATE = FALSE;
	}


	// MANUAL MODE
	if (step == L_MANUAL){
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
		ST_MO_ROTATE = FALSE;
		autoForward = FALSE;
		step = L_MANUAL;
	}

	MA_MO_PIPE_SCREW = item;
	writeOutputs();

	//DEBUG
	if (bSTOP == TRUE) LED_STRING_1_TRG();


	// ********* END OF PROGRAM ********

	// DISPLAY
	if (bSTOP == FALSE) time3sec = 30000;  // Reload 3 sec timer

	if ((bSTOP == TRUE)&
		(time3sec == 0)){
		time3sec = 30000;  // Reload 3 sec timer

		if (activeMenu == MENU_PRG_ENT_SET){
			activeMenu = MENU_PRACA;
			cursorPosition = 0;
			saveSettingsToEEPROM();
			currentEntrance = 0;
			mode = NORMAL;
		}
		else if (activeMenu == MENU_PRG_MAT_ENT){
			activeMenu = MENU_PRG_ENT_SET;
			cursorPosition = 0;
			mode = PROGRAMING;
		}
		else if (activeMenu == MENU_PRACA){
			activeMenu = MENU_PRG_MAT_ENT;
			cursorPosition = 11;
			mode = PROGRAMING;
		}
	}

	bMOVE_CUR_R = inputsPortA[3].ss_input;
	bINC_DIGIT = inputsPortA[2].ss_input;
	buttonsHandling();

	if (refreshDisplay == TRUE){
		updateDisplay();
		refreshDisplay = FALSE;
	}
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

	// Timeouts
	if (timeOut > 0) timeOut--;
	if (time3sec > 0) time3sec--;

	delay8bit--;
	if (delay8bit == 0){
		refreshDisplay = TRUE;
	}

}
