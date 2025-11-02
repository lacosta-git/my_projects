/*
 *
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include <stdlib.h>
#include "main.h"
#include "keyboard/keyboard.h"

// Variables
volatile uint32_t led_loop_counetr = 0;
volatile uint32_t loop_counetr = 0;
volatile uint16_t pressed_key;

// Functions
void init_ports(){
	LED_STRING_1_DDR();  // Set LED GPIB port as output
}

// Main
int main (void){
// Initialization
	// variables

	// Ports
	init_ports();

	// Keyboard
	init_keyboard();

while(1){

	keyboard_handler();
	pressed_key = keyboard_read_keys();
	if (pressed_key == KEY_F1) led_loop_counetr = 10;
	if (pressed_key == KEY_F2) led_loop_counetr = 20;
	if (pressed_key == KEY_HASH) led_loop_counetr = 30;
	if (pressed_key == KEY_STAR) led_loop_counetr = 40;
	if (pressed_key == KEY_1) led_loop_counetr = 50;
	if (pressed_key == KEY_2) led_loop_counetr = 60;
	if (pressed_key == KEY_3) led_loop_counetr = 70;
	if (pressed_key == KEY_ARROW_UP) led_loop_counetr = 80;
	if (pressed_key == KEY_4) led_loop_counetr = 90;
	if (pressed_key == KEY_5) led_loop_counetr = 100;
	if (pressed_key == KEY_6) led_loop_counetr = 110;
	if (pressed_key == KEY_ARROW_DOWN) led_loop_counetr = 120;
	if (pressed_key == KEY_7) led_loop_counetr = 130;
	if (pressed_key == KEY_8) led_loop_counetr = 140;
	if (pressed_key == KEY_9) led_loop_counetr = 150;
	if (pressed_key == KEY_ESC) led_loop_counetr = 160;
	if (pressed_key == KEY_ARROW_LEFT) led_loop_counetr = 170;
	if (pressed_key == KEY_0) led_loop_counetr = 180;
	if (pressed_key == KEY_ARROW_RIGHT) led_loop_counetr = 190;
	if (pressed_key == KEY_ENT) led_loop_counetr = 200;

	loop_counetr++;
	if (loop_counetr > 210) loop_counetr = 0;

	if (loop_counetr > led_loop_counetr) {
		LED_STRING_1_ON();
	} else LED_STRING_1_OFF();

}
}
