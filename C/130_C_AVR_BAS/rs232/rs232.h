/*
 * Header file for serial communication
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include "../interfaces/ifc.h"

#define TX_BUFFER_SIZE 100
volatile unsigned char txBuf[TX_BUFFER_SIZE];

// Functions
void init_serial_communication(uint16_t);
void rs_send(char*);
void rs_send_handler(void);
uint8_t rs_receive_handler(void);
uint8_t rs_read_received_data(char*);

