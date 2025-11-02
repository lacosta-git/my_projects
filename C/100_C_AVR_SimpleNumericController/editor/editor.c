/*
 * editor.c
 *
 *  Created on: 24.09.2017
 *      Author: autoMaster
 */

#include "../main.h"
#include "../display/display.h"
#include "../keyboard/keyboard.h"
#include <avr/eeprom.h>

volatile uint8_t eRow = 0;  // Position of cursor on virtual paper
volatile uint8_t eCol = 0;  // Position of cursor on virtual paper
volatile uint8_t dRow = 0;  // LEFT UP CORNER OF DISPLAY
volatile uint8_t dCol = 0;  // LEFT UP CORNER OF DISPLAY
volatile uint8_t xOffset = 0;
volatile uint8_t yOffset = 0;

uint8_t EEMEM ee_vPaper[MAX_LINES][MAX_COLUMNS];
extern uint8_t ee_bkBrightness;

void initEditor(void){
	char text[] = {';', 'E', 'D', 'I', 'T', 'O', 'R'};
	// Clear virtual paper
	for (uint8_t line = 0; line < MAX_LINES; line++){
		for (uint8_t column = 0; column < MAX_COLUMNS; column++){
			vPaper[line][column] = 0x20;  // SPACE
		}
	}
	// Write EDITOR in first line on editor after reset
	for (uint8_t c = 0; c < 7; c++){
		vPaper[0][c] = text[c];
	}

	// Init eeprom
	uint16_t vPaperSize;
	vPaperSize = MAX_LINES * MAX_COLUMNS;
	if (eeprom_read_byte((uint8_t*)&ee_vPaper[0][0]) == 0xFF){
		// EEPROM cleaning
		eeprom_update_block ((const void *)vPaper , (void *)ee_vPaper , vPaperSize);
		eeprom_update_byte((uint8_t*)&ee_bkBrightness, 1);
	}

}

void mainEditorHAndler(char kChar){
	if ((0x20 <= kChar) & (kChar <= 0x7D)){  // Letters and digits

		if (vPaper[eRow][MAX_COLUMNS - 1] == 0x20){  // Check if there is space to shift text right
			for (uint8_t i=(MAX_COLUMNS - 1); i > eCol; i--){
				vPaper[eRow][i] = vPaper[eRow][i-1];
			}

			vPaper[eRow][eCol] = kChar;
			if (eCol < (MAX_COLUMNS - 1)){
				eCol++;
			}
		}
	}

	if (kChar == key_ENTER){  // Enter
		uint8_t i;

		// Check if there is no code in last line which is to be deleted
		uint8_t noCodeInLastLine = TRUE;
		for (i=0; i<MAX_COLUMNS; i++){
			if (vPaper[MAX_LINES-1][i] != 0x20){
				noCodeInLastLine = FALSE;
				break;
			}
		}

		if (eRow < (MAX_LINES - 1)){
			if (noCodeInLastLine == TRUE){  // move down all code by one line
				for (i = (MAX_LINES-1); i>eRow; i--){
					for (uint8_t j=0; j<MAX_COLUMNS; j++){
						vPaper[i][j] = vPaper[i-1][j];
					}
				}
				for (i=0; i<(MAX_COLUMNS-eCol); i++){  // move down middle part of line
					vPaper[eRow+1][i] = vPaper[eRow][eCol+i];
					vPaper[eRow][eCol+i] = 0x20;
				}
				for (i=(MAX_COLUMNS-eCol); i<MAX_COLUMNS; i++){  // clear end of middle part
					vPaper[eRow+1][i] = 0x20;
				}
			}

			eRow++;
		}
		eCol = 0;
	}

	if (kChar == key_BKSP){  // Backspace
		if ((eCol == 0) &
			(eRow > 0)){
			uint8_t i;
			uint8_t j;

			// Check if there is no code i previous line
			uint8_t noCodeInPeviousLine = TRUE;
			uint8_t noCodeInCurrentLine = TRUE;
			for (i=0; i<MAX_COLUMNS; i++){
				if (vPaper[eRow-1][i] != 0x20){
					noCodeInPeviousLine = FALSE;
				}
				if (vPaper[eRow][i] != 0x20){
					noCodeInCurrentLine = FALSE;
				}
			}

			// move up all code by one line
			if ((noCodeInPeviousLine == TRUE)|
				(noCodeInCurrentLine == TRUE)){

				uint8_t temRow;
				if (noCodeInCurrentLine == TRUE) temRow = eRow;
				else temRow = eRow - 1;

				for (i=temRow; i<MAX_LINES; i++){
					for (j=0; j<MAX_COLUMNS; j++){
						vPaper[i][j] = vPaper[i+1][j];
					}
				}
				// add empty line at the end
				for (j=0; j<MAX_COLUMNS; j++){
					vPaper[MAX_LINES-1][j] = 0x20;
				}
			}

			// Move cursor to previous line
			eRow--;

			// find last char in code
			for (j=0; j<MAX_COLUMNS; j++){
				if (vPaper[eRow][j] != 0x20){
					eCol = j;
				}
			}
			// setting cursor after last char only when it is possible
			if (eCol < (MAX_COLUMNS-1)) eCol++;
		}

		// Delete last digit in a row
		else if ((eCol == (MAX_COLUMNS - 1))&
				(vPaper[eRow][eCol]!= 0x20)){
			vPaper[eRow][MAX_COLUMNS-1] = 0x20;
		}

		else if (eCol > 0){
			eCol--;

			// Move text left
			for (uint8_t i = eCol; i < (MAX_COLUMNS-1); i++){
				vPaper[eRow][i] = vPaper[eRow][i+1];
			}
			vPaper[eRow][MAX_COLUMNS-1] = 0x20;
		}
	}

	if (kChar == key_UP){  // UP ARROW
		if (eRow > 0){
			eRow--;
		}
	}

	if (kChar == key_DOWN){  // DOWN ARROW
		if (eRow < (MAX_LINES - 1)){
			eRow++;
		}
	}

	if (kChar == key_LEFT){  // LEFT ARROW
		if (eCol > 0){
			eCol--;
		}
	}

	if (kChar == key_RIGHT){  // RIGHT ARROW
		if (eCol < (MAX_COLUMNS - 1)){
			eCol++;
		}
	}

	if (kChar == key_END){  // END
		eCol = MAX_COLUMNS - 1;
	}

	if (kChar == key_HOME){  // HOME
		eCol = 0;
	}

	// Move display window
	if (eCol > (xOffset + DISPLAY_COLUMNS - 1)){
		xOffset = eCol - DISPLAY_COLUMNS + 1;
	}
	if (eCol < xOffset){
		xOffset = eCol;
	}

	if (eRow > (yOffset + DISPLAY_ROWS - 1)){
		yOffset = eRow - DISPLAY_ROWS + 1;
	}
	if (eRow < yOffset){
		yOffset = eRow;
	}

	// Refresh display
	for (uint8_t line = 0; line < DISPLAY_ROWS; line++){
		for (uint8_t column = 0; column < DISPLAY_COLUMNS; column++){
			dRow = yOffset + line;
			dCol = xOffset + column;
			vDisplay[line][column] = vPaper[dRow][dCol];
		}
	}

	// Update display
	if (kChar) displayUpadate = TRUE;
	cursorRowCol((eRow - yOffset), (eCol - xOffset));
}

