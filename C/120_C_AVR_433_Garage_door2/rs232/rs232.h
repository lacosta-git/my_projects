/*
 * Header file for serial communication
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include "../main.h"

#define RS_TX_BUFFER_SIZE 200
#define RS_SHORT_RX_BUFFER_SIZE 8

// Functions
void init_serial_communication(uint32_t);
void rs_send(char*);
void rs_send_P(const __flash char*);
void rs_send_handler(void);
void rs_get_handler(void);
void rs_get(char*);
