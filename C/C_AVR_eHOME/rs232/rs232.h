/*
 * Header file for serial communication
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include "../main.h"

#define TX_BUFFER_SIZE 100
volatile unsigned char txBuf[TX_BUFFER_SIZE];

// Functions
extern void init_serial_communication(uint16_t);
extern void rs_send(char*);
extern void rs_send_handler(void);
