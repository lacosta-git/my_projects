/*
 * i2c.c
 *
 *  Created on: 05.01.2019
 *      Author: lapiUser
 */


#include "i2c.h"
volatile uint8_t transfer_byte_state;  // used in reading byte state machine
volatile uint8_t new_byte_read;	 // reading byte status
volatile uint8_t b_send_start;  // TRUE/FALSE -> START bit sent/not sent
volatile uint8_t b_address;  // i2c device address
volatile uint8_t b_read_mode;  // Transfer mode: TRUE - Master Read, FALSE - Master Write
volatile uint8_t b_transfer;  // If TRUE data are transfered
volatile uint8_t b_ack;  // ACK bit in Master Recive mode
volatile uint8_t b_data;  // i2c in/out data
volatile uint8_t b_send_stop;  // TRUE/FALSE -> STOP bit send/not sent
volatile uint8_t b_stop_bit_transmit_delay;


uint32_t i2c_calculate_bit_rate(uint16_t bit_rate_kHz, uint8_t pr){
	// BR = (F_CPU - 16*SCL)/(2*Prescaler*SCL)
	uint32_t value_1;
	uint32_t value_2;
	value_1 = (F_CPU / 1000) - (16 * 100);
	value_2 = 2 * pr * bit_rate_kHz;
	return value_1 / value_2;
}

void init_i2c(uint16_t scl_rate_kHz){
	// scl_rate_kHz should be between 100 and 400 kHz
	// Expected: 100, 200, 300, 400
	// Bit Rate register settings are calculated to be as close
	// as possible (but lower) with SCL speed.

	uint32_t bit_rate;
	uint8_t prescaler;
	uint8_t prescaler_reg_value;

	prescaler = 1;  // First prescaler value
	prescaler_reg_value = 0;
	bit_rate = i2c_calculate_bit_rate(scl_rate_kHz, prescaler);

	if (bit_rate > 255){
		prescaler = 4;
		prescaler_reg_value = 1;
		bit_rate = i2c_calculate_bit_rate(scl_rate_kHz, prescaler);
	}

	if (bit_rate > 255){
		prescaler = 16;
		prescaler_reg_value = 2;
		bit_rate = i2c_calculate_bit_rate(scl_rate_kHz, prescaler);
	}

	if (bit_rate > 255){
		prescaler = 64;
		prescaler_reg_value = 3;
		bit_rate = i2c_calculate_bit_rate(scl_rate_kHz, prescaler);
	}

	I2C_BIT_RATE_REG = (uint8_t)bit_rate;
	I2C_PRESCALER_REG &= ~prescaler_reg_value;  // Clear old bits
	I2C_PRESCALER_REG |= prescaler_reg_value;

	transfer_byte_state = I2C_IDLE;
	new_byte_read = FALSE;
	b_stop_bit_transmit_delay = 0;
	b_ack = FALSE;
}

// Send START
void i2c_send_start(void){
	//TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	I2C_CONTROL_REG = (1<<I2C_BUSY_FALG)|(1<<I2C_START_BIT)|(1<<I2C_ENABLE_BIT);
}

// Wait for transfer completed
uint8_t i2c_is_transfer_completed(void){
	// if (TWCR & (1<<TWINT)) return TRUE;
	if (I2C_CONTROL_REG & (1<<I2C_BUSY_FALG)) return TRUE;
	else return FALSE;
}

// Read I2C Status Register
uint8_t i2c_read_status(void){
	return I2C_STATUS;
}

// Load sending register
void i2c_load_sending_reg(uint8_t data){
	I2C_DATA_REG = data;
}

// Read data register
uint8_t i2c_read_data_regiser(){
	return I2C_DATA_REG;
}

// Start transfer of data
void i2c_start_data_transfer(uint8_t master_read_ack){
	// TWCR = (1<<TWINT) | (1<<TWEN)
	I2C_CONTROL_REG = (1<<I2C_BUSY_FALG)|(1<<I2C_ENABLE_BIT)| (master_read_ack<<I2C_ENABLE_ACK);
}

// Send STOP
void i2c_send_stop(void){
	// TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO)
	I2C_CONTROL_REG = (1<<I2C_BUSY_FALG)|(1<<I2C_STOP_BIT)|(1<<I2C_ENABLE_BIT);
}


// I2C Transfer Data State Machine
void i2c_transfer_byte(){
	uint8_t transfer_staus;
	switch (transfer_byte_state){
		case I2C_IDLE:
			break;
		// START BIT
		case I2C_SEND_START_BIT:
			if (b_send_start == TRUE){
				i2c_send_start();
				transfer_byte_state = I2C_WAIT_FOR_TRANSFER_COMPLETED;
			} else {
				transfer_byte_state = I2C_TRANSFER_B_DATA;
			}
			break;

		case I2C_WAIT_FOR_TRANSFER_COMPLETED:
			if (i2c_is_transfer_completed()){
				transfer_byte_state = I2C_READ_TRANSFER_STATUS;
			}
			break;

		case I2C_READ_TRANSFER_STATUS:
			transfer_staus = i2c_read_status();
			if (!((transfer_staus == 0x08)|
				  (transfer_staus == 0x10))){
				transfer_byte_state = I2C_TRANSFER_ERROR;  // Transfer error
			} else {
				transfer_byte_state = I2C_TRANSFER_SLA;
			}
			break;

		// ADDRESS
		case I2C_TRANSFER_SLA:
			if (b_read_mode == TRUE){
				b_address |= 0x01;  // READING - LSb MUST be 1
			} else {
				b_address &= 0xFE;  // WRITING - LSb MUST be 0
			}
			i2c_load_sending_reg(b_address);
			i2c_start_data_transfer(FALSE);
			transfer_byte_state = I2C_WAIT_FOR_SLA_TRANSFER_COMPLETED;
			break;

		case I2C_WAIT_FOR_SLA_TRANSFER_COMPLETED:
			if (i2c_is_transfer_completed()){
				transfer_byte_state = I2C_READ_SLA_TRANSFER_STATUS;
			}
			break;

		case I2C_READ_SLA_TRANSFER_STATUS:
			transfer_staus = i2c_read_status();
			if (!((transfer_staus == 0x40)|
				  (transfer_staus == 0x18))){
				transfer_byte_state = I2C_TRANSFER_ERROR;  // Transfer error
			} else {
				transfer_byte_state = I2C_TRANSFER_B_DATA;
			}

		// TRANSFERING DATA
		case I2C_TRANSFER_B_DATA:
			if (b_transfer == TRUE){
				if (b_read_mode == FALSE){
					i2c_load_sending_reg(b_data);
				}
				i2c_start_data_transfer(b_ack);
				transfer_byte_state = I2C_WAIT_FOR_B_DATA_TRANSFER_COMPLETED;
			} else {
				transfer_byte_state = I2C_SEND_STOP_BIT;
			}
			break;

		case I2C_WAIT_FOR_B_DATA_TRANSFER_COMPLETED:
			if (i2c_is_transfer_completed()){
				transfer_byte_state = I2C_READ_B_DATA_TRANSFER_STATUS;
			}
			break;

		case I2C_READ_B_DATA_TRANSFER_STATUS:
			transfer_staus = i2c_read_status();
			if (!((transfer_staus != 0x50)|
				  (transfer_staus != 0x58)|
				  (transfer_staus != 0x28)|
				  (transfer_staus != 0x30))){
				transfer_byte_state = I2C_TRANSFER_ERROR;  // Transfer error
			} else {
				if (b_read_mode == TRUE){
					b_data = i2c_read_data_regiser();
					new_byte_read = TRUE;
				}
				transfer_byte_state = I2C_SEND_STOP_BIT;
			}

		// STOP BIT
		case I2C_SEND_STOP_BIT:
			if (b_send_stop == TRUE){
				i2c_send_stop();
				transfer_byte_state = I2C_WAIT_FOR_STOP_BIT_TRANSFER_COMPLETED;
			} else {
				transfer_byte_state = I2C_IDLE;
			}
			break;

		case I2C_WAIT_FOR_STOP_BIT_TRANSFER_COMPLETED:
			b_stop_bit_transmit_delay++;
			if (b_stop_bit_transmit_delay==50){
				b_stop_bit_transmit_delay = 0;
				transfer_byte_state = I2C_IDLE;
			}
			break;

			break;
		case I2C_TRANSFER_ERROR:
				LED_STRING_1_TRG();
				transfer_byte_state = I2C_IDLE;
			break;

		default:
			transfer_byte_state = I2C_IDLE;
			break;
	}
}


// Start transfer
void i2c_start_transfer(uint8_t i2c_address,  // I2C device address
						uint8_t read){  // Transfer mode: TRUE - Master Read, FALSE - Master Write

	b_send_start = TRUE;
	b_address = i2c_address;
	b_transfer = FALSE;
	if (read == TRUE){  // READ MODE
		b_read_mode = TRUE;
		new_byte_read = FALSE;
	} else {  // WRITE MODE
		b_read_mode = FALSE;
	}
	b_send_stop = FALSE;
	transfer_byte_state = I2C_SEND_START_BIT;
}

void i2c_start_reading_byte(uint8_t ack){
	b_send_start = FALSE;
	b_transfer = TRUE;
	b_ack = ack;
	b_send_stop = FALSE;
	transfer_byte_state = I2C_SEND_START_BIT;
}

void i2c_start_writing_byte(uint8_t i2c_data){
	b_send_start = FALSE;
	b_transfer = TRUE;
	b_data = i2c_data;
	b_ack = FALSE;
	b_send_stop = FALSE;
	transfer_byte_state = I2C_SEND_START_BIT;
}

void i2c_stop_transfer(void){
	b_send_start = FALSE;
	b_transfer = FALSE;
	b_send_stop = TRUE;
	transfer_byte_state = I2C_SEND_START_BIT;
}

uint8_t i2c_is_busy(void){
	if (transfer_byte_state != I2C_IDLE){
		return TRUE;
	} else return FALSE;
}

uint8_t i2c_is_new_byte_read(void){
	return new_byte_read;
}

uint8_t i2c_read_data_byte(void){
	new_byte_read = FALSE;
	return b_data;
}

// I2C handler
// Should be put in main loop
void i2c_handler(void){
	i2c_transfer_byte();
}
