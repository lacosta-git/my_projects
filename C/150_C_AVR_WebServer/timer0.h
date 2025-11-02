/*
 * timer0.h
 *
 */ 

#include <avr/io.h>

#ifndef TIMER0_H_
#define TIMER0_H_

// 10 ms overflow time
// CPU = 16 MHz
// 16000000/100 = 1600000 [10 ms]
// 160000/1024 = 156 -> prescaler = 1024
// 256 - 156 = 100 -> 0x64 - value to T0_RELOAD

// reload value
#define T0_RELOAD	0x64

//Timer initialization prototype function
void t0initialization(void);

//Timer reload
void t0reload(void);

#endif /* TIMER0_H_ */