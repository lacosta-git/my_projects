/*
 * timer0.h
 *
 */ 

#include "../main.h"

volatile uint8_t delayTimerEnable;
volatile uint32_t delayCounter1;

// reload value
#define RELOAD	0x38

//Timer initialization prototype function
void int_init100us(void);
void int_100usHandler(void);
void int_initKeyboard(void);
void int_keyboardHndler(void);
