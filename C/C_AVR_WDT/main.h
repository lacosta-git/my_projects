
#include <avr/io.h>

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  1600000
#endif  // F_CPU


#define FALSE 0
#define TRUE 1

// Diagnostic LED
#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

#define		LED_STRING_2_DDR()			DDRB |= (1<<PB3)
#define		LED_STRING_2_TRG()			PORTB ^= (1<<PB3) // change state
#define		LED_STRING_2_ON()			PORTB |= (1<<PB3)
#define		LED_STRING_2_OFF()			PORTB &= ~(1<<PB3)


// inputs

#define DATA_1_PIN			PB1
#define DATA_1_DDR()		DDRB &= ~(1<<DATA_1_PIN); PORTB |=(1<<DATA_1_PIN)  // INPUT with pull up
#define READ_DATA_1_PIN()	(PINB&(1<<DATA_1_PIN))>>DATA_1_PIN
