/*
 * display.c
 *
 *  Created on: 17.09.2017
 *      Author: autoMaster
 */

#include "../main.h"
#include "display.h"
#include <avr/eeprom.h>
#include "../rs232/rs232.h"
#include "../keyboard/keyboard.h"
#include "../editor/editor.h"
#include "../interpreter/interpreter.h"
#include "../axis/axis.h"
#include <stdlib.h>
#include "../file/pr_file.h"

volatile uint8_t row = 0;
volatile uint8_t col = 0;
volatile uint8_t cursorRow = 0;
volatile uint8_t cursorCol = 0;

volatile uint16_t menuID = MAIN_MENU;

extern uint8_t* ee_vPaper;
uint8_t EEMEM ee_bkBrightness;

static uint8_t axis_counter = 0;
static struct stepMotor *manualSelectedAxis;

void clrScr(void){
	uint8_t vrow = 0;
	uint8_t vcol = 0;
	for (vrow = 0; vrow < DISPLAY_ROWS; vrow++){
		for (vcol = 0; vcol < DISPLAY_COLUMNS; vcol++){
			vDisplay[vrow][vcol] = ' ';
		}
	}
}

void clrStatusBar(void){
	uint8_t vcol;
	for (vcol = 0; vcol < DISPLAY_COLUMNS; vcol++){
		vStatusBar[0][vcol] = ' ';
	}
}

void initDisplay(void){
	LCD_Initalize();
	clrScr();
	clrStatusBar();
	bkBrightness = eeprom_read_byte((uint8_t*)&ee_bkBrightness);
	displayUpadate = TRUE;
}

void cursorRowCol(uint8_t wRow, uint8_t wCol){
	cursorRow = wRow;
	cursorCol = wCol;
}

void writeChar(uint8_t wRow, uint8_t wCol, char cChar){
	vDisplay[wRow][wCol] = cChar;
	displayUpadate = TRUE;
}

void lcdWrite(uint8_t lcdRow, uint8_t lcdCol, char* text){
	uint8_t counter = 0;
	while(*text){
		vDisplay[lcdRow][lcdCol + counter] = *text;
		text++;
		counter++;
	}
	displayUpadate = TRUE;
}

// Update status bar
void statusBar(char* text){
	uint8_t counter = 0;
	clrStatusBar();
	while(*text){
		vStatusBar[0][counter] = *text;
		text++;
		counter++;
	}
}
void displayHandler(void){

	if (statusBarUpadate == TRUE){
		// Refresh Status Bar
		if (col < DISPLAY_COLUMNS){
			LCD_GoTo(col, row);
			LCD_WriteData(vStatusBar[0][col]);
			col++;
		} else {
			row = 0;
			col = 0;
			statusBarUpadate = FALSE;
			LCD_GoTo(cursorCol, cursorRow);
		}
	}

	if (displayUpadate == TRUE){
		// Refresh screen
		if (row < DISPLAY_ROWS){
			if (col < DISPLAY_COLUMNS){
				LCD_GoTo(col, row);
				LCD_WriteData(vDisplay[row][col]);
				col++;
			}
		}

		// Move from end to beginning
		if (col >= DISPLAY_COLUMNS) {
			col = 0;
			row++;
		}
		if (row >= DISPLAY_ROWS) {
			row = STATUS_BAR_ROW - 1;
			statusBarUpadate = TRUE;
			displayUpadate = FALSE;
		}
	}
}

void onEnterMainMenu(void){
	clrScr();
	statusBar("F1-NM F2-ST/SP");
	lcdWrite(0, 5, "SNC v1.0");
	cursorRowCol(0, 0);
}

void onEnterEditorMenu(void){
	mainEditorHAndler(0);
	statusBar("F1-NM F2-ST/S F3-S/L");
	displayUpadate = TRUE;
}

void onEnterEditorSaveLoad(void){
	clrScr();
	clrStatusBar();
	lcdWrite(0, 0, "F3-LOAD F4-SAVE");
	lcdWrite(1, 4, "F1-CANCEL");
	cursorRowCol(0, 0);
}

void onEnterLoadProgram(void){
	uint16_t vPaperSize;
	clrScr();
	lcdWrite(0, 0, "LOADING...");
	vPaperSize = MAX_LINES * MAX_COLUMNS;
	eeprom_read_block ((void *) vPaper, (const void *)ee_vPaper, vPaperSize) ;
	//load_program_file();  WORKS !
	lcdWrite(0, 0, "LOADING...OK");
	lcdWrite(1, 0, "PRESS F1");
	cursorRowCol(0, 0);
}

void onEnterSaveProgram(void){
	uint16_t vPaperSize;
	clrScr();
	lcdWrite(0, 0, "SAVING...");
	vPaperSize = MAX_LINES * MAX_COLUMNS;
	eeprom_update_block ((const void *)vPaper, (void *)ee_vPaper, vPaperSize);
	lcdWrite(0, 0, "SAVING...OK");
	lcdWrite(1, 0, "PRESS F1");
	cursorRowCol(0, 0);
}

void onEnterMainSettings(void){
	clrScr();
	lcdWrite(0, 0, "SETTINGS");
	lcdWrite(1, 4, "F3-ENTER");
	statusBar("F1-NM F2-ST/S");
	cursorRowCol(0, 0);
}

void onEnterSettingsBkBrightness(void){
	clrScr();
	lcdWrite(0, 0, "BK BRIGHTNESS");
	lcdWrite(1, 4, "F3-CHANGE");
	lcdWrite(2, 4, "F1-EXIT");
}

void nextBkBrightness(void){
	if (bkBrightness < 10) bkBrightness += 2;
	else bkBrightness = 1;
	eeprom_update_byte((uint8_t*)&ee_bkBrightness, bkBrightness);
}

// MANUAL movement
void onEnterMainManual(void){
	clrScr();
	lcdWrite(0, 0, "MANUAL");
	lcdWrite(0, 11, "NEXT");  // UP ARROW
	lcdWrite(1, 11, "-");  // LEFT ARROW
	lcdWrite(1, 15, "+");  // RIGHT ARROW
	lcdWrite(2, 11, "PREV");  // DOWN ARROW
	axis_counter = 1;	// first axis selected
	selectAxis(axis_counter);
	statusBar("F1-NM   USE ARROWS");
	cursorRowCol(0, 0);
}

void selectAxis(uint8_t axisNo){
	switch (axisNo){
		case 1:
			writeChar(1, 13, 'X');
			manualSelectedAxis = (struct stepMotor*)&xAxis;
		break;

		case 2:
			writeChar(1, 13, 'Y');
			manualSelectedAxis = (struct stepMotor*)&yAxis;
		break;

		case 3:
			writeChar(1, 13, 'Z');
			manualSelectedAxis = (struct stepMotor*)&zAxis;
		break;

		case 4:
			writeChar(1, 13, 'A');
			manualSelectedAxis = (struct stepMotor*)&aAxis;
		break;

		case 5:
			writeChar(1, 13, 'B');
			manualSelectedAxis = (struct stepMotor*)&bAxis;
		break;

		case 6:
			writeChar(1, 13, 'C');
			manualSelectedAxis = (struct stepMotor*)&cAxis;
		break;

	default:
	break;
	}
}
void menuHandler(char kChar){

	if (kChar == key_F2){  // F2 START/STOP PROGRAM
		if (runProgram == FALSE) runProgram = TRUE;
		else runProgram = FALSE;
	}

	if (runProgram == TRUE){
		LED_STRING_1_ON();
	} else LED_STRING_1_OFF();

	switch (menuID) {
	case MAIN_MENU:
		if (kChar == key_F1){  // F1
			onEnterEditorMenu();
			menuID = MAIN_EDITOR;
		}
		break;

	case MAIN_EDITOR:
		if (kChar == key_F1){  // F1 - NEXT MENU
			onEnterMainManual();
			menuID = MAIN_MANUAL;
		}
		else if (kChar == key_F3){  // F3 SAVE/LOAD
			onEnterEditorSaveLoad();
			menuID = EDITOR_SAVE_LOAD;

		} else mainEditorHAndler(kChar);
	break;

		case EDITOR_SAVE_LOAD:
			if (kChar == key_F1){  // F1 BACK TO EDITOR
				onEnterEditorMenu();
				menuID = MAIN_EDITOR;
			}

			else if (kChar == key_F3){  // F3 LOAD
				onEnterLoadProgram();
			}

			else if (kChar == key_F4){  // F4 SAVE
				onEnterSaveProgram();
			}
		break;

	// Manual control
	case MAIN_MANUAL:
			if (kChar == key_F1){  // F1 - NEXT MENU
				onEnterMainSettings();
				menuID = MAIN_SETTINGS;
			}
			else if (kChar == key_RIGHT){  // ENABLE PLUS
				// PLUS
				manualSelectedAxis -> dir = PLUS;
				manualSelectedAxis -> steps = 500;
				manualSelectedAxis -> manualEnable = TRUE;
			}
			else if (kChar == key_LEFT){  // ENABLE MINUS
				// MINUS
				manualSelectedAxis -> dir = MINUS;
				manualSelectedAxis -> steps = 500;
				manualSelectedAxis -> manualEnable = TRUE;
			}
			else if (kChar == key_UP){  // NEXT AXIS
				// NEXT
				axis_counter++;
				if (axis_counter > 6) axis_counter = 1;  // MAX number of axis
				selectAxis(axis_counter);
			}
			else if (kChar == key_DOWN){  // PREVIOUS AXIS
				// PREVIOUS
				axis_counter--;
				if ((axis_counter == 0) | (axis_counter > 6)){
					axis_counter = 6;  // MAX number of axis
				}
				selectAxis(axis_counter);
			}
			else if (kChar == key_RELEASE){
				// STOP
				manualSelectedAxis -> manualEnable = FALSE;
				manualSelectedAxis -> steps = 0;
			}

		break;

	// SETTINGS
	case MAIN_SETTINGS:
		if (kChar == key_F1){  // F1 - Next menu
			onEnterMainMenu();
			menuID = MAIN_MENU;
		}
		else if (kChar == key_F3){  // F3 Back Light brightness
			onEnterSettingsBkBrightness();
			menuID = LCD_BK_BRIGHTNESS;
		}
	break;

		case LCD_BK_BRIGHTNESS:
			if (kChar == key_F1){  // F1 - Next menu
				onEnterMainSettings();
				menuID = MAIN_SETTINGS;
			}
			else if (kChar == key_F3){  // F3 Back Light brightness
				nextBkBrightness();
			}
		break;

	default:
		break;
	}
}
