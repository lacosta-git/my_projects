/*
 * i2c.h
 *
 *  Created on: 05.01.2019
 *      Author: lapiUser
 */

#include "../main.h"

#define I2C_READ	TRUE
#define I2C_WRITE	FALSE
#define I2C_ACK_TRUE	TRUE
#define I2C_ACK_FALSE	FALSE

#ifdef ATMEGA328P
	#define I2C_PRESCALER_REG 		TWSR
	#define I2C_BIT_RATE_REG		TWBR
	#define I2C_CONTROL_REG			TWCR
		#define I2C_BUSY_FALG			7
		#define I2C_ENABLE_ACK			6
		#define I2C_START_BIT			5
		#define I2C_STOP_BIT			4
		#define I2C_ENABLE_BIT			2
	#define I2C_STATUS_REG			TWSR
	#define I2C_STATUS				(I2C_STATUS_REG & 0xF8)
	#define I2C_DATA_REG			TWDR

#endif  // ATMEGA328P

// Master Write BYTE State Machine
#define I2C_IDLE									0
#define I2C_SEND_START_BIT							2
#define I2C_WAIT_FOR_TRANSFER_COMPLETED				4
#define I2C_READ_TRANSFER_STATUS					6
#define I2C_TRANSFER_SLA							8
#define I2C_WAIT_FOR_SLA_TRANSFER_COMPLETED			10
#define I2C_READ_SLA_TRANSFER_STATUS				12
#define I2C_TRANSFER_B_DATA							14
#define I2C_WAIT_FOR_B_DATA_TRANSFER_COMPLETED		16
#define I2C_READ_B_DATA_TRANSFER_STATUS				18
#define I2C_SEND_STOP_BIT							20
#define I2C_WAIT_FOR_STOP_BIT_TRANSFER_COMPLETED	22
#define I2C_TRANSFER_ERROR							80


// Functions
void init_i2c(uint16_t);
void i2c_start_transfer(uint8_t, uint8_t);
void i2c_start_reading_byte(uint8_t);
void i2c_start_writing_byte(uint8_t);
void i2c_stop_transfer(void);
uint8_t i2c_is_busy(void);
uint8_t i2c_is_new_byte_read(void);
uint8_t i2c_read_data_byte(void);
void i2c_handler(void);
