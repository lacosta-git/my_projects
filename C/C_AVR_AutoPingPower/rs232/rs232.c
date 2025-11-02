/*
 * Main library for serial communication
 *
 *  Created on: 30.12.2018
 *      Author: Tomasz Szkudlarek
 *
 *
 *  Usage:
 *  Init section
 *  	...
 *      init_serial_communication(9600);
 *  	...
 *
 *  Than in main loop there must be called handler.
 *  	...
 *  	rs_send_handler();
 *  	rs_get_handler();
 *  	...
 *
 *  Somewhere in code there must be called rs_get to read current RS get buffer
 *  	...
 *  	len_since_last_rs_get = rs_get(*get_buff_ptr);
 *  	...
 *
 *  Sending text
 *  	...
 *  	rs_send("\r>> Your text");
 *  	...
 */

#include "rs232.h"

volatile uint8_t* rs_data_register;  // USART Data Register
volatile uint8_t* rs_control_register;  // USART Control Register
volatile uint8_t rs_data_register_empty;  // USART Data Register Empty flag
volatile uint8_t rs_dara_receive_complete;  // USART Control Register flag

volatile unsigned char tx_buf[RS_TX_BUFFER_SIZE];
volatile unsigned char rs_short_rx_buffer[RS_SHORT_RX_BUFFER_SIZE];

void init_serial_communication(uint32_t baudrate){
	#ifdef ATMEGA328P
		// ATmega328P setup
		//calculate UBRR value
		uint16_t ubrr;
		ubrr = ((F_CPU / (baudrate * 16UL)) - 1);

		/*Set baud rate */
		UBRR0H = (uint8_t)(ubrr>>8);
		UBRR0L = (uint8_t)ubrr;

		//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
		UCSR0C=((0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00));

		//Enable Transmitter and Receiver and NO Interrupt on receive complete
		UCSR0B |=((1<<RXEN0)|(1<<TXEN0));

		// Clear double speed of RS
		UCSR0A &= ~(1<<U2X0);

		rs_data_register = &UDR0;
		rs_control_register = &UCSR0A;
		rs_data_register_empty = UDRE0;
		rs_dara_receive_complete = RXC0;

	#endif  // ATMEGA328P

	#ifdef ATMEGA2560
		// ATmega2560 setup for RSD0
		//calculate UBRR value
		uint16_t ubrr;
		ubrr = ((F_CPU / (baudrate * 16UL)) - 1);

		/*Set baud rate */
		UBRR0H = (uint8_t)(ubrr>>8);
		UBRR0L = (uint8_t)ubrr;

		//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
		UCSR0C=(0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);

		//Enable Transmitter and Receiver and NO Interrupt on receive complete
		UCSR0B=(1<<RXEN0)|(1<<TXEN0);

		rs_data_register = &UDR0;
		rs_control_register = &UCSR0A;
		rs_data_register_empty = UDRE0;

	#endif  // ATMEGA2560

	tx_buf[0] = 0;
	rs_short_rx_buffer[0] = 0;
}

void _decrease_tx_buffer(void){
	uint8_t chars_to_send = tx_buf[0];
	// roll left data in tx buffer
	for (uint8_t i=1; i<=chars_to_send; i++){
		tx_buf[i] = tx_buf[i+1];
	}

	// Clear last char in buffer
	tx_buf[chars_to_send] = 0;

	// Decrease number of chars to send by 1
	tx_buf[0] = chars_to_send - 1;
}

// txBuffer [charsToSend, char1, char2, char3, ...]
void rs_send_handler(void){
	// Check if there are chars to send
	if (tx_buf[0] > 0){
		// Check if uC tx buffer is empty
		if ( *rs_control_register & (1<<rs_data_register_empty)){
			// Sending char to uC tx buffer
			*rs_data_register = tx_buf[1];
			_decrease_tx_buffer();  // Update tx buffer
		}
	}
}

// Message is put to sending buffer
// End of string is detected when 0x00 in ASCII
// Using "Your string" automatically adds 0x00 at the end of string.
void rs_send(char* msg){
	while(*msg){
		tx_buf[++tx_buf[0]] = *msg;
		msg++;
	}
}


// Function pulls Receiving Completed for new sign received
void rs_get_handler(void){
	// Check if char received
	if ( *rs_control_register & (1<<rs_dara_receive_complete)){
		// Short RS rx buffer is FIFO type
		for (uint8_t i=RS_SHORT_RX_BUFFER_SIZE-1; i>=1; i--){
			rs_short_rx_buffer[i] = rs_short_rx_buffer[i-1];
		}
		rs_short_rx_buffer[RS_SHORT_RX_BUFFER_SIZE-1] = 0;  // End of string
		rs_short_rx_buffer[0] = *rs_data_register;  // Reading char from rx buffer
	}
}

// Reading data from receiving buffer
void rs_get(char* rs_rx_data){
	for (uint8_t i=0; i<RS_SHORT_RX_BUFFER_SIZE; i++){
		*(rs_rx_data+i) = rs_short_rx_buffer[i];  // Reading data
		rs_short_rx_buffer[i] = 0;	// Clear buffer after reading
	}
}


