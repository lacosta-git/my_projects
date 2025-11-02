/*
 * Interrupts handling
 *
 * Created: 2014-12-31 18:24:38
 *  Author: Tomasz Szkudlarek
 */ 
#include "interrups.h"
#include "../display/display.h"
#include "../keyboard/keyboard.h"
#include "../axis/axis.h"
#include "../interpreter/interpreter.h"
#include <stdlib.h>

volatile uint16_t int_counter = 5;
volatile uint16_t int_mainCounter = 0;
volatile int i_mainTemp;
volatile char c_mainTemp[] ="      ";

volatile uint16_t timeTicks = 10000;

void _100usReload(void)
{
	//reload timer
	#ifdef ATMEGA328P
		TCNT0 = RELOAD;
	#endif

//reload timer
	#ifdef ATMEGA2560
		TCNT0 = RELOAD;
	#endif
}

void int_init100us(void)
{
	// 100 us overflow time
	// CPU = 16 MHz
	// 16000000/10000 = 1600 [100 us]
	// 1600/8 = 200 -> prescaler = 8
	// 256 - 200 = 56 -> 0x38 - RELOAD

	#ifdef ATMEGA328P
		TCCR0A = 0x0;
		TCCR0B = (0<<WGM02)|(0<<CS02)|(1<<CS01)|(0<<CS00);
		_100usReload();

		//T0 Interrupt Enable
		TIMSK0 = _BV(TOIE0);
	#endif

	#ifdef ATMEGA2560

		TCCR0A = 0x0;
		TCCR0B = (0<<WGM02)|(0<<CS02)|(1<<CS01)|(0<<CS00);
		_100usReload();

		//T0 Interrupt Enable
		TIMSK0 = _BV(TOIE0);
	#endif

	delayTimerEnable = FALSE;
}

// 100 us interrupt handler
void int_100usHandler(void){
	// 100 us task
	_100usReload();

	// LCD Back light brightness
	int_counter--;
	if (int_counter == 0)
	{
		BACK_LIGHT_ON();
		int_counter = bkBrightness;
	}
	else BACK_LIGHT_OFF();

	// Display free time on display
	int_mainCounter++;
	if (int_mainCounter >= 100){
		int_mainCounter = 0;
		itoa(free_time, (char*)&c_mainTemp, 10);
//		lcdWrite(1, 0, (char*)&c_mainTemp);
		free_time = 0;
	}

	// Delay timer
	if (delayTimerEnable == TRUE){
		if (delayCounter1 > 0) delayCounter1--;  // Used by interpreter
		else delayTimerEnable = FALSE;
	}

	// Keyboard
	timeTicks++;
	if (timeTicks > 10000){
		timeTicks = 10000;
	}
	keyboardTimieTick(timeTicks);

	interruptAxisHandler();

}

void int_initKeyboard(){
	#ifdef ATMEGA328P
		// Enable interrupt from external input
		EIMSK |= (1<<INT0);

		// Generate interrupt on falling edge of CLK
		EICRA |= (1<<ISC01);
	#endif

	#ifdef ATMEGA2560
		// Enable interrupt from external input
		EIMSK |= (1<<INT2);

		// Generate interrupt on falling edge of CLK
		EICRA |= (1<<ISC21);
	#endif

	initKeyboard();
}

void int_keyboardHndler(void){
	interruptKeyboardHandler();
	timeTicks = 0;
}
