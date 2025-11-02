/*
 * keyboard.c
 *
 *  Created on: 18.09.2017
 *      Author: autoMaster
 */

#include "keyboard.h"

volatile uint8_t bitIndex = 0;
volatile char keyboardData = 0;
volatile uint8_t newCharInBuffor = FALSE;
volatile char keyboardBuffor[] = {0, 0, 0};
volatile uint8_t bufforIndex = 0;
volatile uint8_t shiftPressed = FALSE;

void initKeyboard(void){
	// Set as input with pull up
	KEYBOARD_DATA_DIR &= ~KEYBOARD_DATA;  // Set as input
	KEYBOARD_DATA_PORT |= KEYBOARD_DATA;  // Set pull up
	KEYBOARD_CLK_DIR &= ~KEYBOARD_CLK;  // Set as input
	KEYBOARD_CLK_PORT |= KEYBOARD_CLK;  // Set pull up
}

void interruptKeyboardHandler(void){
	bitIndex++;
	if (bitIndex == 11){
		keyboardBuffor[bufforIndex] = keyboardData;
		keyboardData = 0;
		bufforIndex++;
	}

	if ((bitIndex >= 2) & (bitIndex <= 9)){
		if (KEYBOARD_DATA_PIN & KEYBOARD_DATA){
			keyboardData |= (1<<(bitIndex - 2));
	}}
}

void keyboardTimieTick(uint16_t timeTicks){
	if (timeTicks > 10){  // > 1ms
		bitIndex = 0;
	}

	if (timeTicks == 50){  // = 5ms
		newCharInBuffor = TRUE;
		bufforIndex = 0;
	}
}

// return 0 if no button pressed
// return ASCII of pressed button
char mainKeyboardHandler(void){
	uint16_t charCode = 0;
	if (newCharInBuffor){
		newCharInBuffor = FALSE;
		charCode = (keyboardBuffor[0] << 8) + keyboardBuffor[1];
		keyboardBuffor[0] = 0;
		keyboardBuffor[1] = 0;
		keyboardBuffor[2] = 0;
	}

	return ps2toASCII(charCode);
}

char ps2toASCII(uint16_t ps2code){
	if (shiftPressed == TRUE){
		if (ps2code == 0x1C00) return 'A';
		if (ps2code == 0x3200) return 'B';
		if (ps2code == 0x2100) return 'C';
		if (ps2code == 0x2300) return 'D';
		if (ps2code == 0x2400) return 'E';
		if (ps2code == 0x2B00) return 'F';
		if (ps2code == 0x3400) return 'G';
		if (ps2code == 0x3300) return 'H';
		if (ps2code == 0x4300) return 'I';
		if (ps2code == 0x3B00) return 'J';
		if (ps2code == 0x4200) return 'K';
		if (ps2code == 0x4B00) return 'L';
		if (ps2code == 0x3A00) return 'M';
		if (ps2code == 0x3100) return 'N';
		if (ps2code == 0x4400) return 'O';
		if (ps2code == 0x4D00) return 'P';
		if (ps2code == 0x1500) return 'Q';
		if (ps2code == 0x2D00) return 'R';
		if (ps2code == 0x1B00) return 'S';
		if (ps2code == 0x2C00) return 'T';
		if (ps2code == 0x3C00) return 'U';
		if (ps2code == 0x2A00) return 'V';
		if (ps2code == 0x1D00) return 'W';
		if (ps2code == 0x2200) return 'X';
		if (ps2code == 0x3500) return 'Y';
		if (ps2code == 0x1A00) return 'Z';
		if (ps2code == 0x4500) return ')';
		if (ps2code == 0x1600) return '!';
		if (ps2code == 0x1E00) return '@';
		if (ps2code == 0x2600) return '#';
		if (ps2code == 0x2500) return '$';
		if (ps2code == 0x2E00) return '%';
		if (ps2code == 0x3600) return '^';
		if (ps2code == 0x3D00) return '&';
		if (ps2code == 0x3E00) return '*';
		if (ps2code == 0x4600) return '(';
		if (ps2code == 0x4E00) return '_';
		if (ps2code == 0x5500) return '+';
		if (ps2code == 0x5400) return '{';
		if (ps2code == 0x5B00) return '}';
		if (ps2code == 0x4C00) return ':';
		if (ps2code == 0x5200) return 0x22;
		if (ps2code == 0x4100) return '<';
		if (ps2code == 0x4900) return '>';
		if (ps2code == 0x4A00) return '?';

		if (ps2code == 0x5800) {
			shiftPressed = FALSE;  // CAPS pressed
			return 0;
		}
	} else {
		if (ps2code == 0x1C00) return 'a';
		if (ps2code == 0x3200) return 'b';
		if (ps2code == 0x2100) return 'c';
		if (ps2code == 0x2300) return 'd';
		if (ps2code == 0x2400) return 'e';
		if (ps2code == 0x2B00) return 'f';
		if (ps2code == 0x3400) return 'g';
		if (ps2code == 0x3300) return 'h';
		if (ps2code == 0x4300) return 'i';
		if (ps2code == 0x3B00) return 'j';
		if (ps2code == 0x4200) return 'k';
		if (ps2code == 0x4B00) return 'l';
		if (ps2code == 0x3A00) return 'm';
		if (ps2code == 0x3100) return 'n';
		if (ps2code == 0x4400) return 'o';
		if (ps2code == 0x4D00) return 'p';
		if (ps2code == 0x1500) return 'q';
		if (ps2code == 0x2D00) return 'r';
		if (ps2code == 0x1B00) return 's';
		if (ps2code == 0x2C00) return 't';
		if (ps2code == 0x3C00) return 'u';
		if (ps2code == 0x2A00) return 'v';
		if (ps2code == 0x1D00) return 'w';
		if (ps2code == 0x2200) return 'x';
		if (ps2code == 0x3500) return 'y';
		if (ps2code == 0x1A00) return 'z';
		if (ps2code == 0x4500) return '0';
		if (ps2code == 0x1600) return '1';
		if (ps2code == 0x1E00) return '2';
		if (ps2code == 0x2600) return '3';
		if (ps2code == 0x2500) return '4';
		if (ps2code == 0x2E00) return '5';
		if (ps2code == 0x3600) return '6';
		if (ps2code == 0x3D00) return '7';
		if (ps2code == 0x3E00) return '8';
		if (ps2code == 0x4600) return '9';

		if (ps2code == 0x4E00) return '-';
		if (ps2code == 0x5500) return '=';
		if (ps2code == 0x5400) return '[';
		if (ps2code == 0x5B00) return ']';
		if (ps2code == 0x4C00) return ';';
		if (ps2code == 0x5200) return 0x27;
		if (ps2code == 0x4100) return ',';
		if (ps2code == 0x4900) return '.';
		if (ps2code == 0x4A00) return '/';
	}

	if (ps2code == 0x5A00) return key_ENTER;  // CR - ENTER
	if (ps2code == 0xE05A) return key_ENTER;  // CR - ENTER NUMERICAL
	if (ps2code == 0x7600) return key_ESC;  // ESC
	if (ps2code == 0x2900) return key_SPACE;  // SPACE
	if (ps2code == 0x6600) return key_BKSP;  // BKSP
	if (ps2code == 0x0D00) return key_TAB;  // TAB

	if (ps2code == 0xE075) return key_UP;  // UP ARROW
	if (ps2code == 0xE072) return key_DOWN;  // DOWN ARROW
	if (ps2code == 0xE06B) return key_LEFT;  // LEFT ARROW
	if (ps2code == 0xE074) return key_RIGHT;  // RIGHT ARROW

	if (ps2code == 0xE069) return key_END;  // END
	if (ps2code == 0xE06C) return key_HOME;  // HOME

	if (ps2code == 0x0500) return key_F1;  // F1
	if (ps2code == 0x0600) return key_F2;  // F2
	if (ps2code == 0x0400) return key_F3;  // F3
	if (ps2code == 0x0C00) return key_F4;  // F4

	if ((ps2code == 0x1200) |						// L SHIFT pressed
		(ps2code == 0x5900) |						// R SHIFT pressed
		(ps2code == 0x5800)) { 						// CAPS pressed
		shiftPressed = TRUE;
	}

	if ((ps2code == 0xF012) |						// L SHIFT released
		(ps2code == 0xF059)) shiftPressed = FALSE;  // R SHIFT released

	if (ps2code == 0xE0F0) return key_RELEASE;  // KEY RELEASE !!! Only tree byte coded

	return 0;
}
