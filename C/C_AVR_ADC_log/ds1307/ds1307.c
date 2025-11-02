/*
 * ds1307.c
 *
 *  Created on: 07.01.2019
 *      Author: lapiUser
 */

#include "ds1307.h"
#include "../i2c/i2c.h"

// RTC
volatile uint8_t transfer_state_machine;
volatile uint8_t transfer_direction;
volatile uint8_t raw_year;
volatile uint8_t raw_month;
volatile uint8_t raw_date;
volatile uint8_t raw_day;
volatile uint8_t raw_hour;
volatile uint8_t raw_minute;
volatile uint8_t raw_second;

// RAM

volatile uint8_t nvRAM_transfer_state_machine;
volatile uint8_t nvRAM_transfer_direction;
volatile uint8_t nvRAM_raw_address;
volatile uint8_t nvRAM_raw_data;



void init_rtc(void){
	transfer_direction = RTC_READING;
	transfer_state_machine = 0;
}

void rtc_start_reading_date_and_time(void){
	transfer_direction = RTC_READING;
	transfer_state_machine = 1;
}


void rtc_start_writing_date_and_time(void){
	transfer_direction = RTC_WRITING;
	transfer_state_machine = 1;
}


void rtc_transfer_date_and_time(void){
	switch (transfer_state_machine){
		case 0:
			break;

		case 1:  // Waiting because rtc or ram state machine is busy
			if ((nvRAM_is_busy() == TRUE) |
				(rtc_is_busy() == TRUE)){
				break;
			} else transfer_state_machine++;
			break;

		case 2:
			if (i2c_is_busy() == FALSE){
				i2c_start_transfer(I2C_RTC_ADDRESS, I2C_WRITE);  // RTC + write
				transfer_state_machine++;
			}
			break;

		case 3:
			if (i2c_is_busy() == FALSE){
				i2c_start_writing_byte(0x00);  // RTC: Time and data start address
				if (transfer_direction == RTC_WRITING){
					transfer_state_machine++;  // skip on step
				}
				transfer_state_machine++;
			}
			break;

		case 4:
			if (i2c_is_busy() == FALSE){
				i2c_start_transfer(I2C_RTC_ADDRESS, I2C_READ);  // RTC + read
				transfer_state_machine++;
			}
			break;

		case 5:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					i2c_start_reading_byte(I2C_ACK_TRUE); // start reading second
				} else {
					i2c_start_writing_byte(raw_second);  // start writing second
				}
				transfer_state_machine++;
			}
			break;

		case 6:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_second = i2c_read_data_byte();  // read second
					i2c_start_reading_byte(I2C_ACK_TRUE); // start reading minutes
				} else {
					i2c_start_writing_byte(raw_minute);  // start writing minutes
				}
				transfer_state_machine++;
			}
			break;

		case 7:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_minute = i2c_read_data_byte();  // read minutes
					i2c_start_reading_byte(I2C_ACK_TRUE); // start reading hours
				} else {
					i2c_start_writing_byte(raw_hour);  // start writing hours
				}
				transfer_state_machine++;
			}
			break;

		case 8:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_hour = i2c_read_data_byte();  // read hours
					i2c_start_reading_byte(I2C_ACK_TRUE); // start reading day of the week
				} else {
					i2c_start_writing_byte(raw_day);  // start writing day of the week
				}
				transfer_state_machine++;
			}
			break;

		case 9:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_day = i2c_read_data_byte();  // read day day of the week
					i2c_start_reading_byte(I2C_ACK_TRUE); // start reading date
				} else {
					i2c_start_writing_byte(raw_date);  // start writing date
				}
				transfer_state_machine++;
			}
			break;

		case 10:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_date = i2c_read_data_byte();  // read date
					i2c_start_reading_byte(I2C_ACK_TRUE); // start reading month
				} else {
					i2c_start_writing_byte(raw_month);  // start writing month
				}
				transfer_state_machine++;
			}
			break;

		case 11:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_month = i2c_read_data_byte();  // read month
					i2c_start_reading_byte(I2C_ACK_FALSE); // start reading year
				} else {
					i2c_start_writing_byte(raw_year);  // start writing year
				}
				transfer_state_machine++;
			}
			break;

		case 12:
			if (i2c_is_busy() == FALSE){
				if (transfer_direction == RTC_READING){
					raw_year = i2c_read_data_byte();  // read year
				}
				i2c_stop_transfer(); // RTC end of transfer
				transfer_state_machine = 0;
			}
			break;

		default:
			transfer_state_machine = 0;
			break;
		}
}

void rtc_read_date_and_time(uint8_t *year,
							uint8_t *month,
							uint8_t *date,
							uint8_t *day,
							uint8_t *hour,
							uint8_t *minute,
							uint8_t *second){
	*year = raw_year;
	*month = raw_month;
	*date = raw_date;
	*day = raw_day;

	if (raw_hour & 0x40){
		// 24h mode detected
		*hour = raw_hour & 0x3F;
	} else {
		// 12h mode detected
		*hour = raw_hour;
	}

	*minute = raw_minute;
	*second = raw_second & 0x7F;  // CH mode masked
}


void rtc_write_date_and_time(uint8_t year,
							 uint8_t month,
							 uint8_t date,
							 uint8_t day,
							 uint8_t hour,
							 uint8_t minute,
							 uint8_t second){
	raw_year = year;
	raw_month = month;
	raw_date = date;
	raw_day = day;
	raw_hour = hour;
	raw_minute = minute;
	raw_second = second;

	rtc_start_writing_date_and_time();
}

uint8_t rtc_is_busy(void){
	if (transfer_state_machine > 1){
		return TRUE;
	} else return FALSE;
}

void rtc_handler(void){
	rtc_transfer_date_and_time();
}


//  nvRAM support
void init_nvRAM(void){
	nvRAM_transfer_state_machine = 0;
	nvRAM_transfer_direction = NVRAM_READING;
}


void nvRAM_transfer_data(void){
	switch (nvRAM_transfer_state_machine){
		case 0:
			break;

		case 1:  // Waiting because rtc or ram state machine is busy
			if ((nvRAM_is_busy() == TRUE) |
				(rtc_is_busy() == TRUE)){
				break;
			} else nvRAM_transfer_state_machine++;
			break;

		case 2:
			if (i2c_is_busy() == FALSE){
				i2c_start_transfer(I2C_RAM_ADDRESS, I2C_WRITE);  // RAM + write
				nvRAM_transfer_state_machine++;
			}
			break;

		case 3:
			if (i2c_is_busy() == FALSE){
				i2c_start_writing_byte(nvRAM_raw_address);  // RAM: set address
				if (nvRAM_transfer_direction == NVRAM_WRITING){
					nvRAM_transfer_state_machine++;  // skip on step
				}
				nvRAM_transfer_state_machine++;
			}
			break;

		case 4:
			if (i2c_is_busy() == FALSE){
				i2c_start_transfer(I2C_RAM_ADDRESS, I2C_READ);  // RAM + read
				nvRAM_transfer_state_machine++;
			}
			break;

		case 5:
			if (i2c_is_busy() == FALSE){
				if (nvRAM_transfer_direction == NVRAM_READING){
					i2c_start_reading_byte(I2C_ACK_FALSE); // start reading second
				} else {
					i2c_start_writing_byte(nvRAM_raw_data);  // start writing data
				}
				nvRAM_transfer_state_machine++;
			}
			break;

		case 6:
			if (i2c_is_busy() == FALSE){
				if (nvRAM_transfer_direction == NVRAM_READING){
					nvRAM_raw_data = i2c_read_data_byte();  // read data
				}
				i2c_stop_transfer(); // RAM end of transfer
				nvRAM_transfer_state_machine = 0;
			}
			break;

		default:
			nvRAM_transfer_state_machine = 0;
			break;
	}
}

uint8_t nvRAM_is_busy(void){
	if (nvRAM_transfer_state_machine > 1){
		return TRUE;
	} else return FALSE;
}

void nvRAM_start_reading_byte(uint8_t b_address){
	nvRAM_raw_address = b_address;
	nvRAM_transfer_direction = NVRAM_READING;
	nvRAM_transfer_state_machine = 1;
}

uint8_t nvRAM_read_data_byte(void){  // Read data byte
	return nvRAM_raw_data;
}

void nvRAM_start_writing_byte(void){
	nvRAM_transfer_direction = NVRAM_WRITING;
	nvRAM_transfer_state_machine = 1;
}

void nvRAM_write_data_byte(uint8_t b_address, uint8_t b_data){
	nvRAM_raw_address = b_address;
	nvRAM_raw_data = b_data;
	nvRAM_start_writing_byte();
}

void nvRAM_handler(void){
	nvRAM_transfer_data();
}
