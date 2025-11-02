/*
 * Simple Numeric Controller
 *
 *  Created on: 15.09.2017
 *      Author: Tomasz Szkudlarek
 */


#include "main.h"
#include "rs232/rs232.h"

volatile uint16_t loop_counter = 0;
volatile uint16_t m_loop_counter = 0;
char string[] = {0, 0, 0, 0, 0, 0, 0, 0};


#define ACE_PROG_IDLE 			0
#define ACE_PROG_LOAD_12V		4
#define ACE_PROG_LOAD_5V		8
#define ACE_PROG_PREPARE_DATA	10
#define ACE_PROG_SEND_BYTE_1	12
#define ACE_PROG_SEND_BYTE_2	14
#define ACE_PROG_SEND_BYTE_3	16
#define ACE_PROG_SEND_BYTE_4	18
#define ACE_PROG_LOAD_0V		20
#define ACE_PROG_TWO_CLK		22
#define ACE_PROG_CHECK_IF_END	24

volatile uint8_t m_acex_prog = ACE_PROG_IDLE;
volatile uint16_t m_delay = 0;
volatile uint8_t m_device_prog_mod = FALSE;

// OPCODE BYTE 1
#define ACE_READ_CODE_MEM	0x11
#define ACE_WRITE_CODE_MEM	0x10
#define ACE_READ_DATA_MEM	0x21
#define ACE_WRITE_DATA_MEM	0x20

// Registers
#define ACE_INIT_REG_1		0xBB
#define ACE_LBD				0xBD

// Command word
volatile uint8_t byte_1 = ACE_READ_DATA_MEM;
volatile uint8_t byte_2 = 0;
volatile uint8_t byte_3 = 0x3F;
volatile uint8_t byte_4 = 0;


void set_load_0v(void);
void set_load_5v(void);
void set_load_12v(void);
uint8_t m_is_sned_ready(void);
void m_send_run(void);


uint8_t m_sned_handler(uint8_t);
#define SEND_IDLE			0
#define SEND_NOT_COMPLETED	2
#define SEND_SET_BIT		4
#define SEND_SET_CLK_HIGH	6
#define SEND_SET_CLK_LOW	8
#define SEND_CHECK_END		10
volatile uint8_t resp;
volatile uint8_t send_state = SEND_IDLE;
volatile uint8_t bit_index = 7;
volatile uint16_t bytes_to_transmit = 2;

uint8_t ace_2_pulse_clk(void);
#define CLK_HIGH_1 		2
#define CLK_LOW_1		4
#define CLK_HIGH_2		6
#define CLK_LOW_2		8
volatile uint8_t pulse_clk = CLK_HIGH_1;


volatile uint8_t read_byte;
void read_shift_out(uint8_t);


void print_byte(uint8_t);

int main (void){
// Initialization
	// variables

	// Ports
	LED_STRING_1_DDR();

	ACE_SHIFT_IN_DDR();
	ACE_CLK_DDR();
	ACE_LOAD_0V_DDR();
	ACE_LOAD_12V_DDR();

	// Debug
	DBG_LOAD_12V_DDR();
	DBG_LOAD_0V_DDR();
	DBG_CLK_DDR();
	DBG_SHIFT_IN_DDR();

	DBG_LOAD_12V_LO();
	DBG_LOAD_0V_LO();
	DBG_CLK_LO();
	DBG_SHIFT_IN_LO();

	// Default
	ACE_LOAD_12V_LO();
	ACE_LOAD_0V_LO();
	ACE_CLK_LO();
	ACE_SHIFT_IN_LO();

	// RS232
	init_serial_communication(9600);
	rs_send("\r>> ACEx interface v1.0\r");

while(1){
	loop_counter++;
	if (loop_counter == 0) {
		LED_STRING_1_TRG();
		if (m_loop_counter == 0){
			m_loop_counter = 1;
			if (m_acex_prog == ACE_PROG_IDLE){
				if (m_device_prog_mod == TRUE){
					m_acex_prog = ACE_PROG_LOAD_5V;

					print_byte(byte_3);
					print_byte(read_byte);
					rs_send("\r");
					byte_3++;
				} else {
					m_acex_prog = ACE_PROG_LOAD_12V;
				}
			}
		} else m_loop_counter--;
	}

	rs_send_handler();

	// Delay calculation
	if (m_delay > 0) {
		m_delay--;
	}
	else {
		// Programming state machine
		switch(m_acex_prog){
		case ACE_PROG_IDLE:

			break;

		// Supply a 12V pulse to LOAD
		case ACE_PROG_LOAD_12V:
			set_load_12v();
			m_delay = 0x6F;
			m_device_prog_mod = TRUE;
			m_acex_prog = ACE_PROG_LOAD_5V;
			break;

		// Bring LOAD to Vcc
		case ACE_PROG_LOAD_5V:
			set_load_5v();
			m_delay = 0x4F;
			m_acex_prog = ACE_PROG_PREPARE_DATA;
			break;

		case ACE_PROG_PREPARE_DATA:
			resp = m_is_sned_ready();
			if (resp == TRUE) {
				m_send_run();
				m_acex_prog = ACE_PROG_SEND_BYTE_1;
			}
			break;

		// Send 1st BYTE
		case ACE_PROG_SEND_BYTE_1:
			resp = m_sned_handler(byte_1);
			if (resp == SEND_IDLE){
				m_send_run();
				m_acex_prog = ACE_PROG_SEND_BYTE_2;
			}
			m_delay = 0x0F;  // Clock pulse
			break;

		// Send 2nd BYTE
		case ACE_PROG_SEND_BYTE_2:
			resp = m_sned_handler(byte_2);
			if (resp == SEND_IDLE){
				m_send_run();
				m_acex_prog = ACE_PROG_SEND_BYTE_3;
			}
			m_delay = 0x0F;  // Clock pulse
			break;


		// Send 3rd BYTE
		case ACE_PROG_SEND_BYTE_3:
			resp = m_sned_handler(byte_3);
			if (resp == SEND_IDLE){
				m_send_run();
				m_acex_prog = ACE_PROG_SEND_BYTE_4;
			}
			m_delay = 0x0F;  // Clock pulse
			break;

		// Send 4th BYTE
		case ACE_PROG_SEND_BYTE_4:
			resp = m_sned_handler(byte_4);
			if (resp == SEND_IDLE){
				m_acex_prog = ACE_PROG_LOAD_0V;
			}
			m_delay = 0x0F;  // Clock pulse
			break;

		// Bring LOAD to 0V
		case ACE_PROG_LOAD_0V:
			set_load_0v();
			m_delay = 0x0F;
			m_acex_prog = ACE_PROG_TWO_CLK;
			break;

		// Two CLK pulses
		case ACE_PROG_TWO_CLK:
			resp = ace_2_pulse_clk();
			if (resp == CLK_HIGH_1){
				m_acex_prog = ACE_PROG_CHECK_IF_END;
			}
			m_delay = 0x0F;  // Clock pulse
			break;

		// Check if all bytes transmited
		case ACE_PROG_CHECK_IF_END:
			set_load_5v();
			if (bytes_to_transmit <= 1){
				m_acex_prog = ACE_PROG_IDLE;
			} else {
				bytes_to_transmit--;
				m_acex_prog = ACE_PROG_LOAD_5V;
			}
			m_delay = 0xF0;
			break;

		default:
			m_acex_prog = ACE_PROG_IDLE;
			break;
		}
	}
}
}

void set_load_0v(void){
	ACE_LOAD_0V_HI();
	DBG_LOAD_0V_HI();
	ACE_LOAD_12V_LO();
	DBG_LOAD_12V_LO();
	//rs_send(">> 0V\r");
}

void set_load_5v(void){
	ACE_LOAD_0V_LO();
	DBG_LOAD_0V_LO();
	ACE_LOAD_12V_LO();
	DBG_LOAD_12V_LO();
	//rs_send(">> 5V\r");
}

void set_load_12v(void){
	ACE_LOAD_12V_HI();
	DBG_LOAD_12V_HI();
	ACE_LOAD_0V_LO();
	DBG_LOAD_0V_LO();
	//rs_send(">> 12V\r");
}


void set_shift_out(uint8_t m_bit){
	if (m_bit == 1){
		ACE_SHIFT_IN_HI();
		DBG_SHIFT_IN_HI();
		//rs_send("1");
	} else {
		ACE_SHIFT_IN_LO();
		DBG_SHIFT_IN_LO();
		//rs_send("0");
	}
}


void set_clk(uint8_t m_bit){
	if (m_bit == 1){
		ACE_CLK_HI();
		DBG_CLK_HI();
		//rs_send("-");
	} else {
		ACE_CLK_LO();
		DBG_CLK_LO();
		//rs_send("_");
	}
}


uint8_t m_is_sned_ready(void){
	//rs_send(">> ready\r");
	if (send_state == SEND_IDLE){
		return TRUE;
	} else return FALSE;
}


void m_send_run(void){
	send_state = SEND_SET_BIT;
	//rs_send(">> run\r");
}


// Sending byte on bus
uint8_t m_sned_handler(uint8_t m_byte){
	uint8_t s_bit;
	uint8_t s_resp;

	s_resp = SEND_SET_BIT;
	switch (send_state){
	case SEND_IDLE:
		bit_index = 7;
		s_resp = SEND_IDLE;
		break;

	case SEND_SET_BIT:
		s_bit = m_byte & (1<<bit_index);
		if (s_bit == 0){
			set_shift_out(0);
		} else set_shift_out(1);
		send_state = SEND_SET_CLK_HIGH;
		break;

	case SEND_SET_CLK_HIGH:
		set_clk(1);
		send_state = SEND_SET_CLK_LOW;
		break;

	case SEND_SET_CLK_LOW:
		set_clk(0);
		send_state = SEND_CHECK_END;
		break;

	case SEND_CHECK_END:
		read_shift_out(bit_index);
		if (bit_index == 0){
			send_state = SEND_IDLE;
		} else {
			bit_index--;
			send_state = SEND_SET_BIT;
		}
		break;

	default:
		send_state = SEND_IDLE;
		break;
	}

	return s_resp;
}


uint8_t ace_2_pulse_clk(void){
	switch(pulse_clk){
	// Set CLK HIGH 1
	case CLK_HIGH_1:
		set_clk(1);
		pulse_clk = CLK_LOW_1;
		break;

	// Set CLK LOW 1
	case CLK_LOW_1:
		set_clk(0);
		pulse_clk = CLK_HIGH_2;
		break;

	// Set CLK HIGH 2
	case CLK_HIGH_2:
		set_clk(1);
		pulse_clk = CLK_LOW_2;
		break;

	// Set CLK LOW 2
	case CLK_LOW_2:
		set_clk(0);
		pulse_clk = CLK_HIGH_1;
		break;

	default:
		pulse_clk = CLK_HIGH_1;
		break;
	}
	return pulse_clk;
}


void read_shift_out(uint8_t r_bit_index){
	read_byte &= ~(1<<bit_index);  // clear bit
	read_byte |= ((READ_DATA_PIN())<<bit_index);  // set bit
}


void print_byte(uint8_t p_byte){
	uint8_t digit;
	digit = p_byte & 0xF0;
	digit = digit >> 4;
	if (digit > 9){
		string[0] = (digit - 9) + 0x40;
	} else string[0] = digit + 0x30;

	digit = p_byte & 0x0F;
	if (digit > 9){
		string[1] = (digit - 9) + 0x40;
	} else string[1] = digit + 0x30;

	rs_send("->0x");
	rs_send((char*)&string);
	rs_send(" ");
}
