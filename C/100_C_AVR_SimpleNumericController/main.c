/*
 * Simple Numeric Controller
 *
 *  Created on: 15.09.2017
 *      Author: Tomasz Szkudlarek
 */


#include "main.h"
#include <avr/interrupt.h>
#include "display/display.h"
#include "editor/editor.h"
#include "interpreter/interpreter.h"
#include "keyboard/keyboard.h"
#include "rs232/rs232.h"
#include "interrups/interrups.h"
#include "axis/axis.h"
#include "file/pr_file.h"

char pressedButton = 0;
char string[] = {0, 0};

int main (void){
// Initialization
	// variables
	free_time = 0;

	// Ports
	LED_STRING_1_DDR();

	// RS232
	init_serial_communication(9600);
	rs_send("\r>> Simple Numeric Controller v1.0\r");

	// EEPROM
	init_ee_vFile();

	// DISPLAY
	initDisplay();
	lcdWrite(0, 5, "SNC v1.0");
	statusBar("F1-NM F2-ST/SP");

	// Init editor
	initEditor();

	// KEYBOARD
	int_initKeyboard();

	// STEP MOTORS
	initAxis();

	// Interrupts
	int_init100us();
	sei();

while(1){
	pressedButton = mainKeyboardHandler();
	if (pressedButton > 0){
		string[0] = pressedButton;
		//rsSend((char*)&string);
	}
	rs_send_handler();
	menuHandler(pressedButton);
	mainInterpreterHandler();
	displayHandler();
	free_time++;
}
}

// ********** INTERRUPT ********
ISR(INT_100us){
	int_100usHandler();
}

ISR(KEYBOARD_CLOCK){
	int_keyboardHndler();
}
