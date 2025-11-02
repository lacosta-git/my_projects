
#include <avr/io.h>

// Frequency of CPU
#ifndef F_CPU
	#define F_CPU  128000   // Hz
#endif  // F_CPU

#define FALSE 0
#define TRUE 1

#define OFF 0
#define ON 1

#define NO 0
#define YES 1

// Pinout ATtiny45
// Outputs
#define		OUT_1_DDR()			DDRB |= (1<<PB4)
#define		OUT_1_TRG()			PORTB ^= (1<<PB4) // change state
#define		OUT_1_ON()			PORTB |= (1<<PB4)
#define		OUT_1_OFF()			PORTB &= ~(1<<PB4)

#define		OUT_2_DDR()			DDRB |= (1<<PB3)
#define		OUT_2_TRG()			PORTB ^= (1<<PB3) // change state
#define		OUT_2_ON()			PORTB |= (1<<PB3)
#define		OUT_2_OFF()			PORTB &= ~(1<<PB3)

#define		SET_DDR_OUTPUTS()	OUT_1_DDR(); OUT_2_DDR()
#define		OUTPUTS_ON()		OUT_1_ON(); OUT_2_ON()
#define		OUTPUTS_OFF()		OUT_1_OFF(); OUT_2_OFF()

// Inputs
#define HiZ(pin)			DDRB &= ~(1<<pin); PORTB &=~(1<<pin)  // Hi Z
#define SET_HiZ_FOR_PINS()	HiZ(PB0); HiZ(PB1); HiZ(PB2); HiZ(PB5)

// Disable peripheries
#define DISABLE_PERIPHERIES()   PRR = (1<<PRADC)|(1<<PRUSI)|(1<<PRTIM0)|(1<<PRTIM1)
