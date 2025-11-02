/*
 * keyboard.h
 *
 *  Created on: 18.09.2017
 *      Author: autoMaster
 */

#include "../main.h"

#define key_ENTER	0x0D
#define key_ESC		0x1B
#define key_SPACE	0x20
#define key_BKSP	0x08
#define key_TAB		0x0B

#define key_UP		0x81
#define key_DOWN	0x82
#define key_LEFT	0x83
#define key_RIGHT	0x84

#define key_END 	0x85
#define key_HOME 	0x86

#define key_F1 		0x87
#define key_F2 		0x88
#define key_F3 		0x89
#define key_F4 		0x8A

#define key_RELEASE	0xF0

void initKeyboard(void);
void interruptKeyboardHandler(void);
void keyboardTimieTick(uint16_t);
char mainKeyboardHandler(void);
char ps2toASCII(uint16_t);
