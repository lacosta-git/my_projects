/*
 * interrupt.h
 *
 *  Created on: 30.03.2018
 *      Author: lapiUser
 */

#include <avr/interrupt.h>

//Timer initialization prototype function
void int_initT1(void);
void int_T1_interrupt_handler(void);
