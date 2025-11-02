/*
 * Simple Serial communication library
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 */

#include <stdlib.h>
#include "main.h"
#include "rs232/rs232.h"
#include "i2c/i2c.h"
#include "ds1307/ds1307.h"

// Variables
volatile uint32_t led_loop_counetr = 0;
volatile uint16_t rs_counetr = 0;
volatile uint8_t transfer_state_machine = 0;

// Functions
void init_ports(){
	LED_STRING_1_DDR();  // Set LED GPIB port as output
}

// Main
int main (void){
// Initialization
	// variables
	char string[20];
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

	// Ports
	init_ports();

	// RS232
	init_serial_communication(9600);
	rs_send("\r>> Simple Serial Communication v1.0\r");

	// I2C
	init_i2c(100);  // 100 kHz

	// RTC
	init_rtc();
	//rtc_write_date_and_time(0x19, 0x01, 0x13, 0x07, 0x18, 0x25, 0x44);

	// nvRAM
	init_nvRAM();

	// Interrupts

while(1){
	// rs_send_handler() - function must be in main loop
	// Function is non blocking.
	rs_send_handler();

	// I2C main handler - non blocking
	i2c_handler();

	// RTC main handler - non blocking
	rtc_handler();

	// RAM main handler - non blocking
	nvRAM_handler();

	// ********************************* START DEMO *******************************
	//
//	if (!(rtc_is_busy())){  // Wait for RTC to be ready
//		led_loop_counetr++;
//	}

	led_loop_counetr++;

	if (led_loop_counetr == 10000){
		rtc_start_reading_date_and_time();
	}

	if (led_loop_counetr == 40000){
		nvRAM_start_reading_byte(0x02);
	}

	if (led_loop_counetr == 50000){
		nvRAM_write_data_byte(0x02, 0x19);
	}

	if (led_loop_counetr > 100000) {
		led_loop_counetr = 0;

		rtc_read_date_and_time((uint8_t*)&year,
							   (uint8_t*)&month,
							   (uint8_t*)&date,
							   (uint8_t*)&day,
							   (uint8_t*)&hour,
							   (uint8_t*)&minute,
							   (uint8_t*)&second);

		// RS message preparation
		itoa(year, string, 16);
		rs_send("\r>> 20");
		rs_send((char*)&string);

		rs_send(" ");
		itoa(month, string, 16);
		rs_send((char*)&string);

		rs_send(":");
		itoa(date, string, 16);
		rs_send((char*)&string);

		rs_send("  ");
		itoa(day, string, 16);
		rs_send((char*)&string);

		rs_send("  ");
		itoa(hour, string, 16);
		rs_send((char*)&string);

		rs_send(":");
		itoa(minute, string, 16);
		rs_send((char*)&string);

		rs_send(":");
		itoa(second, string, 16);
		rs_send((char*)&string);

		rs_send("  nvRAM:");
		itoa(nvRAM_read_data_byte(), string, 16);
		rs_send((char*)&string);
	}
	// ********************************* END DEMO ******************************
}
}
// ********** INTERRUPT ********
