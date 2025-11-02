/*
 * display.h
 *
 *  Created on: 17.09.2017
 *      Author: autoMaster
 */

#include "hd44780.h"

#define MAIN_MENU 				0x1000

#define MAIN_EDITOR 			0x2000
#define 	EDITOR_SAVE_LOAD 	0x2100

#define MAIN_MANUAL				0x3000

#define MAIN_SETTINGS			0x9000
#define 	LCD_BK_BRIGHTNESS	0x9100


volatile uint8_t displayUpadate;
volatile uint8_t statusBarUpadate;
volatile uint8_t bkBrightness;

void initDisplay(void);
void lcdWrite(uint8_t, uint8_t, char*);
void statusBar(char*s);
void displayHandler(void);
void cursorRowCol(uint8_t, uint8_t);
void writeChar(uint8_t, uint8_t, char);

void menuHandler(char);
void selectAxis(uint8_t);
