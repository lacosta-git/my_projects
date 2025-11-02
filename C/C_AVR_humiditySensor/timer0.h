/*
 * timer0.h
 *
 */ 

#include <avr/io.h>

#ifndef TIMER0_H_
#define TIMER0_H_

// 100 us overflow time
// CPU = 16 MHz
// 16000000/10000 = 1600 [100 us]
// 1600/8 = 200 -> prescaler = 8
// 256 - 200 = 56 -> 0x38 - T0_RELOAD
// T0_RELOAD = 0x58 is empirical value

// reload value
#define T0_RELOAD	0x38

//Timer initialization prototype function
void t0initialization(void);

//Timer reload
void t0reload(void);

#endif /* TIMER0_H_ */
