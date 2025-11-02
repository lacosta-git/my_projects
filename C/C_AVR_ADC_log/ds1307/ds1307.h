/*
 * ds1307.h
 *
 *  Created on: 07.01.2019
 *      Author: lapiUser
 */

#include "../main.h"

#define RTC_READING		TRUE
#define RTC_WRITING		FALSE
#define I2C_RTC_ADDRESS 	0xD0

#define NVRAM_READING	TRUE
#define NVRAM_WRITING	FALSE
#define I2C_RAM_ADDRESS		0xD0

// Functions
// RTC
void init_rtc(void);
void rtc_start_reading_date_and_time(void);
void rtc_write_date_and_time(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void rtc_read_date_and_time(uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
uint8_t rtc_is_busy(void);
void rtc_handler(void);

// RAM
void init_nvRAM(void);
uint8_t nvRAM_is_busy(void);
void nvRAM_start_reading_byte(uint8_t);
uint8_t nvRAM_read_data_byte(void);
void nvRAM_handler(void);
void nvRAM_write_data_byte(uint8_t, uint8_t);
