

#include "../main.h"
#define ATMEGA328P

#define NEW_DATA_RECEIVED  1

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		ifc_LED_DDR()			DDRB |= (1<<PB5)
	#define		ifc_LED_TRG()			PORTB ^= (1<<PB5) // change state
	#define		ifc_LED_ON()			PORTB |= (1<<PB5)
	#define		ifc_LED_OFF()			PORTB &= ~(1<<PB5)

#endif

uint8_t ifc_init(void);
uint8_t ifc_handler(void);
uint8_t ifc_read_received_data(char*);
uint8_t ifc_print(char*);
uint8_t ifc_print_new_line(void);
