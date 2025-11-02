/*
 * wizznetW5100.c
 *
 */ 

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include "main.h"
#include "wizznetW5100.h"
#include "SPI.h"
#include <stdlib.h>
#include "rs232/rs232.h"

volatile char wizz_msg[100];  // Message read from wizz
volatile char up_msg[100];  // Message prepared to send to wizz
volatile uint8_t source_ip[4] = {192, 168, 7, 200};
volatile uint8_t dest_ip[4] = {192, 168, 7, 89};

volatile uint16_t counter_16b;
volatile uint8_t counter_8b;

typedef struct {
	uint8_t ch_active;		// TRUE/FALSE
	uint8_t ip[4];			// Destination IP
	uint16_t id;			// ICMP:: ID
	uint16_t seq;			// ICMP:: SEQENCE
	uint8_t ping_counter;	// Counter down of ping trails. If 0 then no response on ping since MAX_DROPPED_PINGS
	uint8_t max_dropped_pings;	// Number of max allowed pings with no response before pores reset of device
	uint8_t power;			// ON/OFF
} IP_CHANNEL;
volatile IP_CHANNEL ip_channel[8];
volatile uint8_t channel = 0;

volatile uint8_t icmp_ping;	 // Send ping state machine variable

char string[20];
volatile uint8_t rs_sent = FALSE;


void wizz_clear_wizz_msg(void){
	uint8_t i;
	for(i=0; i<100; i++){
		wizz_msg[i] = 0;
	}
}


void wizz_clear_up_msg(void){
	uint8_t i;
	for(i=0; i<100; i++){
		up_msg[i] = 0;
	}
}



void W5100_Init(void)
{
	// TODO: Remove wizz_msg
	wizz_clear_wizz_msg();
	wizz_clear_up_msg();


	// Ethernet Setup
	unsigned char mac_addr[] = {0x00,0x16,0x36,0xDE,0x58,0xF6};
	unsigned char ip_addr[] = {192,168,7,201};
	unsigned char sub_mask[] = {255,255,255,0};
	unsigned char gtw_addr[] = {192,168,7,1};
	// Setting the Wiznet W5100 Mode Register: 0x0000
	wizzWrite(MR, 0x80);
	// Setting the Wiznet W5100 Gateway Address (GAR): 0x0001 to 0x0004
	wizzWrite(GAR + 0,gtw_addr[0]);
	wizzWrite(GAR + 1,gtw_addr[1]);
	wizzWrite(GAR + 2,gtw_addr[2]);
	wizzWrite(GAR + 3,gtw_addr[3]);
	// Setting the Wiznet W5100 Source Address Register (SAR): 0x0009 to 0x000E
	wizzWrite(SAR + 0,mac_addr[0]);
	wizzWrite(SAR + 1,mac_addr[1]);
	wizzWrite(SAR + 2,mac_addr[2]);
	wizzWrite(SAR + 3,mac_addr[3]);
	wizzWrite(SAR + 4,mac_addr[4]);
	wizzWrite(SAR + 5,mac_addr[5]);
	// Setting the Wiznet W5100 Sub Mask Address (SUBR): 0x0005 to 0x0008
	wizzWrite(SUBR + 0,sub_mask[0]);
	wizzWrite(SUBR + 1,sub_mask[1]);
	wizzWrite(SUBR + 2,sub_mask[2]);
	wizzWrite(SUBR + 3,sub_mask[3]);
	// Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
	wizzWrite(SIPR + 0,ip_addr[0]);
	wizzWrite(SIPR + 1,ip_addr[1]);
	wizzWrite(SIPR + 2,ip_addr[2]);
	wizzWrite(SIPR + 3,ip_addr[3]);

	// Setting the Wiznet W5100 RX and TX Memory Size (2KB),
	wizzWrite(RMSR,NET_2K_MEMALLOC);
	wizzWrite(TMSR,NET_2K_MEMALLOC);

	open_icmp_on_socket_0();

	wizz_init_ip_channels();
}


void wizz_init_ip_channels(void){
	ip_channel[0].ch_active = TRUE;
	ip_channel[0].ip[0] = 192;
	ip_channel[0].ip[1] = 168;
	ip_channel[0].ip[2] = 7;
	ip_channel[0].ip[3] = 89;
	ip_channel[0].id = ip_channel[0].ip[3];
	ip_channel[0].seq = 0;
	ip_channel[0].max_dropped_pings = 5;
	ip_channel[0].ping_counter = ip_channel[0].max_dropped_pings;
	ip_channel[0].power = ON;

	//
	ip_channel[1].ch_active = TRUE;
	ip_channel[1].ip[0] = 192;
	ip_channel[1].ip[1] = 168;
	ip_channel[1].ip[2] = 7;
	ip_channel[1].ip[3] = 89;
	ip_channel[1].id = 0x0100 + ip_channel[1].ip[3];
	ip_channel[1].seq = 0;
	ip_channel[1].max_dropped_pings = 8;
	ip_channel[1].ping_counter = ip_channel[1].max_dropped_pings;
	ip_channel[1].power = ON;

	ip_channel[2].ch_active = TRUE;
	ip_channel[2].ip[0] = 192;
	ip_channel[2].ip[1] = 168;
	ip_channel[2].ip[2] = 7;
	ip_channel[2].ip[3] = 89;
	ip_channel[2].id = 0x0200 + ip_channel[2].ip[3];
	ip_channel[2].seq = 0;
	ip_channel[2].max_dropped_pings = 8;
	ip_channel[2].ping_counter = ip_channel[2].max_dropped_pings;
	ip_channel[2].power = ON;

	ip_channel[3].ch_active = TRUE;
	ip_channel[3].ip[0] = 192;
	ip_channel[3].ip[1] = 168;
	ip_channel[3].ip[2] = 7;
	ip_channel[3].ip[3] = 89;
	ip_channel[3].id = 0x0300 + ip_channel[3].ip[3];
	ip_channel[3].seq = 0;
	ip_channel[3].max_dropped_pings = 8;
	ip_channel[3].ping_counter = ip_channel[3].max_dropped_pings;
	ip_channel[3].power = ON;

	ip_channel[4].ch_active = TRUE;
	ip_channel[4].ip[0] = 192;
	ip_channel[4].ip[1] = 168;
	ip_channel[4].ip[2] = 7;
	ip_channel[4].ip[3] = 90;
	ip_channel[4].id = 0x0400 + ip_channel[4].ip[3];
	ip_channel[4].seq = 0;
	ip_channel[4].max_dropped_pings = 8;
	ip_channel[4].ping_counter = ip_channel[4].max_dropped_pings;
	ip_channel[4].power = ON;

	ip_channel[5].ch_active = FALSE;
	ip_channel[6].ch_active = FALSE;
	ip_channel[7].ch_active = FALSE;

}


void wizzWrite(uint16_t addr,uint8_t data)
{
	uint8_t j;
	// Activate the CS pin
	SPI_PORT &= ~(1<<SPI_CS);
	for (j=0; j<=50; j++);
	// Start Wiznet W5100 Write OpCode transmission
	SPIwriteByte(WIZNET_WRITE_OPCODE);
	// Start Wiznet W5100 Address
	SPIwriteWord(addr);
	// Start Data transmission
	SPIwriteByte(data);
	// CS pin is not active
	SPI_PORT |= (1<<SPI_CS);
}


unsigned char wizzRead(uint16_t addr)
{
	uint8_t data;
	uint8_t j;
	// Activate the CS pin
	SPI_PORT &= ~(1<<SPI_CS);
	for (j=0; j<=50; j++);
	// Start Wiznet W5100 Read OpCode transmission
	SPIwriteByte(WIZNET_READ_OPCODE);
	// Start Wiznet W5100 Address
	SPIwriteWord(addr);
	// Read data
	data = SPIreadByte();
	// CS pin is not active
	SPI_PORT |= (1<<SPI_CS);
	return(data);
}

// Read word from addres
uint16_t wizz_read_word(uint16_t addr){
	uint16_t val = 0;
	val = wizzRead(addr);  // MSB
	val = (val << 8);
	val += wizzRead(addr + 1);
	return(val);
}



void disconnectSocket(uint8_t sock)
{
	if (sock != 0) return;

	// Send DISCONNNECT Command
	wizzWrite(S0_CR,CR_DISCON);
	// Waiting until the S0_CR is clear
	while(wizzRead(S0_CR));
}


void close(uint8_t sock)
{
	if (sock != 0) return;

	// Send Close Command
	wizzWrite(S0_CR,CR_CLOSE);
	// Waiting until the S0_CR is clear
	while(wizzRead(S0_CR));
}

void open_icmp_on_socket_0(void)
{

	// Assigned Socket 0 Mode Register
	wizzWrite(S0_MR, MR_IPRAW);

	// Set IP protocol
	wizzWrite(S0_PROTO, 0x01);  // 0x01 - ICMP

	// Now open the Socket 0
	wizzWrite(S0_CR, CR_OPEN);
	
	// Wait for Opening Process
	while(wizzRead(S0_CR));
}

void wizz_memcpy_wizz_to_uP(uint16_t wizz_start_addr, char *up_dest_ptr, uint16_t wizz_size){
	uint8_t i;

	for (i=0; i<wizz_size; i++){
		*(up_dest_ptr + i) = wizzRead(wizz_start_addr + i);
	}
}


void wizz_memcpy_uP_to_wizz(char *up_source_ptr, uint16_t wizz_dest_addr, uint16_t wizz_size){
	uint8_t i;

	for (i=0; i<wizz_size; i++){
		wizzWrite((wizz_dest_addr + i), *(up_source_ptr + i));
	}
}



void wizz_read_message(uint16_t get_size){
	uint16_t get_offset = 0;
	uint16_t get_start_address = 0;
	uint16_t upper_size = 0;
	uint16_t left_size = 0;
	char *wizz_msg_ptr;
	uint16_t s0_rx_rd = 0;

	if (get_size < 100){  // Drop data if to big
		wizz_msg_ptr = (char*)&wizz_msg;

		/* calculate offset address */
		get_offset = wizz_read_word(S0_RX_RD) & RX_BUF_2K_MASK;
		/* calculate start address(physical address) */
		get_start_address = RXBUFADDR + get_offset;

		/* if overflow socket RX memory */
		if ((get_offset + get_size) > (RX_BUF_2K_MASK + 1))
		{
			/* copy upper_size bytes of get_start_address to destination_addr */
			upper_size = (RX_BUF_2K_MASK + 1) - get_offset;

			// Read msg
			wizz_memcpy_wizz_to_uP(get_start_address, wizz_msg_ptr, upper_size);

			/* copy left_size bytes of gSn_RX_BASE to destination_addr */
			left_size = get_size - upper_size;
			wizz_msg_ptr += upper_size;  // offset in wizz_msg /* update destination_addr*/
			wizz_memcpy_wizz_to_uP(RXBUFADDR, wizz_msg_ptr, left_size);
		}
		else
		{
			/* copy get_size bytes of get_start_address to destination_addr */
			wizz_memcpy_wizz_to_uP(get_start_address, wizz_msg_ptr, get_size);

		}
	}
	// Increase the S0_RX_RD value, so it point to the next receive
	s0_rx_rd = wizz_read_word(S0_RX_RD) + get_size;
	wizzWrite(S0_RX_RD,(s0_rx_rd & 0xFF00) >> 8 );
	wizzWrite(S0_RX_RD + 1,(s0_rx_rd & 0x00FF));

	// Now Send the RECV command
	wizzWrite(S0_CR, CR_RECV);
}


void wizz_parse_message(void){
	uint8_t icmp_data_size = 0;
	uint16_t icmp_request_identifier = 0;
	uint16_t icmp_request_sequence = 0;
	uint16_t msg_size;
	uint8_t i;

	if (wizz_msg[6] == 0x08){  //

		// Prepare response to ECHO Request
		icmp_request_identifier = (wizz_msg[10] << 8);
		icmp_request_identifier += wizz_msg[11];
		icmp_request_sequence = (wizz_msg[12] << 8);
		icmp_request_sequence += wizz_msg[13];
		icmp_data_size = wizz_msg[5] - 8;

		for (i=0; i<icmp_data_size;i++){
			up_msg[i+8] = wizz_msg[i+14];  // Data
		}

		// Source -> Destination IP
		for (i=0; i<4; i++){
			dest_ip[i] = wizz_msg[i];
		}

		msg_size = wizz_icmp_echo_msg((uint8_t*)&up_msg, 0x00, icmp_request_identifier, icmp_request_sequence, icmp_data_size);
		wizz_send_msg(msg_size, (uint8_t*)&dest_ip);
	}

	// ECHO Response
	if (wizz_msg[6] == 0x00){

		icmp_request_identifier = (wizz_msg[10] << 8);
		icmp_request_identifier += wizz_msg[11];
		icmp_request_sequence = (wizz_msg[12] << 8);
		icmp_request_sequence += wizz_msg[13];

		// Verify if response match to any of requests
		for (i=0; i<8; i++){  // scan through all IPs
			if ((ip_channel[i].ch_active == TRUE) &&
				(icmp_request_identifier == ip_channel[i].id) &&
				(icmp_request_sequence == ip_channel[i].seq)){
				// response match to request
				rs_send("\r >> match");
				ip_channel[i].ping_counter = ip_channel[i].max_dropped_pings;  // Reset dropped pings counter
			}
		}
	}

	wizz_clear_wizz_msg();
}


void wizz_read_socket_0_handler(void){
	uint16_t s0_rx_rsr = 0;

	// Wait for command execution
	if (wizzRead(S0_CR) == 0){

		// Check if new data in RX buffer
		s0_rx_rsr = wizz_read_word(S0_RX_RSR);
		if (s0_rx_rsr != 0){
			// Read message
			wizz_read_message(s0_rx_rsr);
			wizz_parse_message();
		}
	}
}


void wizz_send_msg(uint16_t send_size, uint8_t *_dest_ip){
	uint16_t get_offset;
	uint16_t get_start_address;
	uint16_t upper_size;
	uint16_t left_size;
	char *msg_ptr;
	uint16_t s0_tx_wr;

	msg_ptr = (char*) &up_msg;

	/* Write the value of remote_ip, Destination IP Address Register(Sn_DIPR)*/
	wizzWrite(S0_DIPR, *_dest_ip); _dest_ip++;
	wizzWrite(S0_DIPR+1, *_dest_ip); _dest_ip++;
	wizzWrite(S0_DIPR+2, *_dest_ip); _dest_ip++;
	wizzWrite(S0_DIPR+3, *_dest_ip);

	/* calculate offset address */
	get_offset = wizz_read_word(S0_TX_WR) & TX_BUF_2K_MASK;
	/* calculate start address(physical address) */
	get_start_address = TXBUFADDR + get_offset;

	/* if overflow socket TX memory */
	if ((get_offset + send_size) > (TX_BUF_2K_MASK + 1))
	{
		/* copy upper_size bytes of get_start_address to destination_addr */
		upper_size = (TX_BUF_2K_MASK + 1) - get_offset;

		// Write msg
		wizz_memcpy_uP_to_wizz(msg_ptr, get_start_address, upper_size);

		/* copy left_size bytes of gSn_TX_BASE to destination_addr */
		left_size = send_size - upper_size;
		//msg_ptr += upper_size;  // offset in wizz_msg /* update source_addr*/
		wizz_memcpy_uP_to_wizz(msg_ptr, TXBUFADDR, left_size);
	}
	else
	{
		/* copy get_size bytes of get_start_address to destination_addr */
		wizz_memcpy_uP_to_wizz(msg_ptr, get_start_address, send_size);
	}

	// Increase the S0_TX_WR value
	s0_tx_wr = get_offset + send_size;
	wizzWrite(S0_TX_WR,(s0_tx_wr & 0xFF00) >> 8 );
	wizzWrite(S0_TX_WR + 1,(s0_tx_wr & 0x00FF));

	// Now Send the RECV command
	wizzWrite(S0_CR, CR_SEND);
}


uint16_t wizz_calculate_crc_16b(uint8_t *data, uint16_t data_len_B){
	uint16_t crc_16b = 0;
	uint16_t msb_crc_16b = 0;
	uint16_t i = 0;

	for (i=0; i<data_len_B; i+=2){
		msb_crc_16b = 0;
		msb_crc_16b = *(data + i);
		msb_crc_16b = msb_crc_16b << 8;
		msb_crc_16b += *(data + i + 1);
		crc_16b += msb_crc_16b;
		if (SREG & (1 << SREG_C)){
			crc_16b += 1;
		}
	}
	return(~crc_16b);
}


// ICMP ECHO Request msg
// par: data_len MAX = 80;
// Return: msg len
uint16_t wizz_icmp_echo_msg(uint8_t *msg_ptr, uint8_t msg_type, uint16_t identifier, uint16_t seqence, uint8_t data_len){
	uint8_t i;
	uint8_t counter;
	uint16_t crc_16b;
	uint16_t msg_size;

	if (data_len > 80) {
		data_len = 80;
	}

	// prepare ICMP Request header
	*msg_ptr = msg_type;  // ICMP::Type  0x08 - echo request (ping), 0x00 echo response
	*(msg_ptr + 1) = 0x00;  // ICMP::Code  0
	*(msg_ptr + 2) = 0x00;  // ICMP::CRC	MSB
	*(msg_ptr + 3) = 0x00;  // ICMP::CRC LSB
	*(msg_ptr + 4) = (uint8_t)(identifier >> 8);  // ICMP::Identifier MSB
	*(msg_ptr + 5) = (uint8_t)identifier;  // ICMP::Identifier LSB
	*(msg_ptr + 6) = (uint8_t)(seqence >> 8);  // ICMP::Sequence MSB
	*(msg_ptr + 7) = (uint8_t)seqence;  // ICMP::Sequence LSB*msg_ptr = 0x08;

	msg_size = data_len + 8;

	if (msg_type == 0x08){  // Generate data when request
		// Generate data
		counter = 0;
		for (i=8; i<msg_size; i++){
			if (counter > 10) counter = 0;
			*(msg_ptr + i) = 0x61 + counter;
			counter++;
		}
	}

	// Calculate crc
	crc_16b = wizz_calculate_crc_16b(msg_ptr, msg_size);
	*(msg_ptr + 2) = (uint8_t)((crc_16b &0xFF00) >> 8);
	*(msg_ptr + 3) = (uint8_t)(crc_16b &0x00FF);

	return msg_size;
}


uint8_t wizz_write_socket_0_handler(uint8_t ch){
	uint16_t msg_size;
	uint16_t icmp_request_identifier;
	uint16_t icmp_request_sequence;
	uint8_t icmp_data_size;
	uint8_t i;
	uint8_t msg_sent = FALSE;

	// Wait for command execution
	if (wizzRead(S0_CR) == 0){
		ip_channel[ch].seq++;

		// Clean old msg
		wizz_clear_up_msg();

		// Prepare ICMP request
		icmp_request_identifier = ip_channel[ch].id;
		icmp_request_sequence = ip_channel[ch].seq;
		icmp_data_size = 32;

		for (i=0; i<4; i++){
			dest_ip[i] = ip_channel[ch].ip[i];
		}

		// Set parameters
		msg_size = wizz_icmp_echo_msg((uint8_t*)&up_msg, 0x08, icmp_request_identifier, icmp_request_sequence, icmp_data_size);
		wizz_send_msg(msg_size, (uint8_t*)&dest_ip);
		msg_sent = TRUE;

		itoa(ch, string, 10);
		rs_send("*");
		rs_send((char*)&string);
	}
	return(msg_sent);
}


void wizz_handler(void){
	uint8_t msg_sent;
	uint8_t increment_channel = 0;

	wizz_read_socket_0_handler();

	// Send ping state machine
	// One ping per timeout
	switch (icmp_ping) {
	case ICMP_IDLE:
		counter_16b++;
		if (counter_16b > 5000){
			counter_16b = 0;
			icmp_ping = ICMP_SEND_PING;
		}
		break;
	case ICMP_SEND_PING:
		if (ip_channel[channel].ch_active == TRUE){

			msg_sent = wizz_write_socket_0_handler(channel);

			if (ip_channel[channel].ping_counter > 0){
				ip_channel[channel].ping_counter -= 1;
			} else {
				// POWER OFF / ON
				if (ip_channel[channel].power == ON) {
					ip_channel[channel].power = OFF;
				}
				else {
					ip_channel[channel].power = ON;
					ip_channel[channel].ping_counter = ip_channel[channel].max_dropped_pings;
				}
				rs_sent = FALSE;
			}
			if (msg_sent == TRUE){
				increment_channel = 1;
			} else {
				increment_channel = 0;  // Message not sent for current channel. Try again.
			}
		} else increment_channel = 1;

		// Scan through all channels
		if (channel < 7) {
			channel += increment_channel;
		}
		else channel = 0;

		if (increment_channel == 1){
			icmp_ping = ICMP_IDLE;  //It is possible to go to next state
		}
		break;

	default:
		icmp_ping = ICMP_IDLE;
		break;
	}
}


void wizz_power_handler(void){
	uint8_t i;
	for (i=0; i<8; i++){
		if (ip_channel[i].ch_active == TRUE){
			if (ip_channel[i].power == ON){
				if (rs_sent == FALSE){
					rs_send("\r >> ON");
				}
			} else {
				if (rs_sent == FALSE){
					rs_send("\r >> OFF");
				}
			}
		}
	}
	rs_sent = TRUE;
}
