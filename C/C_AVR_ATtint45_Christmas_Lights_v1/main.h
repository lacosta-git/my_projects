
#include <avr/io.h>

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  1000000
#endif  // F_CPU

#define FALSE 0
#define TRUE 1

#define OFF 0
#define ON 1

#define NO 0
#define YES 1

// Pinout ATtiny45
#define		LED_STRING_1_DDR()			DDRB |= (1<<PB4)
#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB4) // change state
#define		LED_STRING_1_ON()			PORTB |= (1<<PB4)
#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB4)

#define		LED_STRING_2_DDR()			DDRB |= (1<<PB3)
#define		LED_STRING_2_TRG()			PORTB ^= (1<<PB3) // change state
#define		LED_STRING_2_ON()			PORTB |= (1<<PB3)
#define		LED_STRING_2_OFF()			PORTB &= ~(1<<PB3)

// Inputs
#define BT_B_PIN			PB2
#define BT_B_DDR()			DDRB &= ~(1<<BT_B_PIN); PORTB |=(1<<BT_B_PIN)  // INPUT with pull up
#define READ_BT_B_PIN()		(PINB&(1<<BT_B_PIN))>>BT_B_PIN

#define BT_D_PIN			PB1
#define BT_D_DDR()			DDRB &= ~(1<<BT_D_PIN); PORTB |=(1<<BT_D_PIN)  // INPUT with pull up
#define READ_BT_D_PIN()		(PINB&(1<<BT_D_PIN))>>BT_D_PIN

#define SETUP_INPUTS()		BT_B_DDR();BT_D_DDR()


#define M_DIVIDER_10MS		10

#define M_MAX_PWM_COUNTER	10  // Time base
#define M_MODE_0_PWM		0   // LEDs disabled
#define M_MODE_1_PWM		1
#define M_MODE_2_PWM		2
#define M_MODE_3_PWM		6
#define M_MODE_4_PWM		11	// Always ON

