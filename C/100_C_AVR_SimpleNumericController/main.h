/*
 * main.h
 *
 *  Created on: 15.09.2017
 *      Author: autoMaster
 */

// CPU for which there project is configured
// Use ONLY ONE of defined
#define ATMEGA2560
//#define ATMEGA328P

// Frequency of CPU
#define F_CPU  16000000

#include <avr/io.h>

#define FALSE 0
#define TRUE 1

#ifdef ATMEGA328P
	// Pinout Arduino UNO R3
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB5)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB5) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB5)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB5)

	// Display
	/*
	 *  Function            Arduino         ATmega
	 *  LCD_RS				  D4			  PD4
	 *  LDC_RW				  D5			  PD5
	 *  LDC_E				  D6			  PD6
	 *  LDC_D4				  D8			  PB0
	 *  LDC_D5				  D9			  PB1
	 *  LDC_D6				  D10			  PB2
	 *  LDC_D7				  D11			  PB3
	 */
	#define LCD_RS_DIR		DDRD
	#define LCD_RS_PORT 	PORTD
	#define LCD_RS_PIN		PIND
	#define LCD_RS			(1 << PD4)

	#define LCD_RW_DIR		DDRD
	#define LCD_RW_PORT		PORTD
	#define LCD_RW_PIN		PIND
	#define LCD_RW			(1 << PD5)

	#define LCD_E_DIR		DDRD
	#define LCD_E_PORT		PORTD
	#define LCD_E_PIN		PIND
	#define LCD_E			(1 << PD6)

	#define LCD_DB4_DIR		DDRB
	#define LCD_DB4_PORT	PORTB
	#define LCD_DB4_PIN		PINB
	#define LCD_DB4			(1 << PB0)

	#define LCD_DB5_DIR		DDRB
	#define LCD_DB5_PORT	PORTB
	#define LCD_DB5_PIN		PINB
	#define LCD_DB5			(1 << PB1)

	#define LCD_DB6_DIR		DDRB
	#define LCD_DB6_PORT	PORTB
	#define LCD_DB6_PIN		PINB
	#define LCD_DB6			(1 << PB2)

	#define LCD_DB7_DIR		DDRB
	#define LCD_DB7_PORT	PORTB
	#define LCD_DB7_PIN		PINB
	#define LCD_DB7			(1 << PB3)

	#define LCD_BL 				(1<<PD7)
	#define	BACK_LIGHT_DIR		DDRD
	#define	BACK_LIGHT_TRG()  	PORTD ^= LCD_BL // change state
	#define	BACK_LIGHT_ON() 	PORTD |= LCD_BL
	#define	BACK_LIGHT_OFF()  	PORTD &= ~LCD_BL

	#define KEYBOARD_DATA_DIR	DDRD
	#define KEYBOARD_DATA_PORT	PORTD
	#define KEYBOARD_DATA_PIN	PIND
	#define KEYBOARD_DATA		(1<<PD3)

	#define KEYBOARD_CLK_DIR	DDRD
	#define KEYBOARD_CLK_PORT	PORTD
	#define KEYBOARD_CLK_PIN	PIND
	#define KEYBOARD_CLK		(1<<PD2)

	#define ST_MO_1_DIR_DIR		DDRC
	#define ST_MO_1_DIR_PORT	PORTC
	#define ST_MO_1_DIR_PIN		PINC
	#define ST_MO_1_DIR			(1<<PC0)

	#define ST_MO_1_PUL_DIR		DDRC
	#define ST_MO_1_PUL_PORT	PORTC
	#define ST_MO_1_PUL_PIN		PINC
	#define ST_MO_1_PUL			(1<<PC1)

	#define ST_MO_2_DIR_DIR		DDRC
	#define ST_MO_2_DIR_PORT	PORTC
	#define ST_MO_2_DIR_PIN		PINC
	#define ST_MO_2_DIR			(1<<PC2)

	#define ST_MO_2_PUL_DIR		DDRC
	#define ST_MO_2_PUL_PORT	PORTC
	#define ST_MO_2_PUL_PIN		PINC
	#define ST_MO_2_PUL			(1<<PC3)

	#define ST_MO_3_DIR_DIR		DDRC
	#define ST_MO_3_DIR_PORT	PORTC
	#define ST_MO_3_DIR_PIN		PINC
	#define ST_MO_3_DIR			(1<<PC4)

	#define ST_MO_3_PUL_DIR		DDRC
	#define ST_MO_3_PUL_PORT	PORTC
	#define ST_MO_3_PUL_PIN		PINC
	#define ST_MO_3_PUL			(1<<PC5)

	#define ST_MO_4_DIR_DIR		DDRC
	#define ST_MO_4_DIR_PORT	PORTC
	#define ST_MO_4_DIR_PIN		PINC
	#define ST_MO_4_DIR			(1<<PC6)

	#define ST_MO_4_PUL_DIR		DDRC
	#define ST_MO_4_PUL_PORT	PORTC
	#define ST_MO_4_PUL_PIN		PINC
	#define ST_MO_4_PUL			(1<<PC6)

	#define ST_MO_5_DIR_DIR		DDRC
	#define ST_MO_5_DIR_PORT	PORTC
	#define ST_MO_5_DIR_PIN		PINC
	#define ST_MO_5_DIR			(1<<PC6)

	#define ST_MO_5_PUL_DIR		DDRC
	#define ST_MO_5_PUL_PORT	PORTC
	#define ST_MO_5_PUL_PIN		PINC
	#define ST_MO_5_PUL			(1<<PC6)

	#define ST_MO_6_DIR_DIR		DDRC
	#define ST_MO_6_DIR_PORT	PORTC
	#define ST_MO_6_DIR_PIN		PINC
	#define ST_MO_6_DIR			(1<<PC6)

	#define ST_MO_6_PUL_DIR		DDRC
	#define ST_MO_6_PUL_PORT	PORTC
	#define ST_MO_6_PUL_PIN		PINC
	#define ST_MO_6_PUL			(1<<PC6)

	// Inputs
	#define IN_START_DIR		DDRC
	#define IN_START_PORT		PORTC
	#define IN_START_PIN		PINC
	#define IN_START			(1<<PC6)

	#define IN_STOP_DIR			DDRC
	#define IN_STOP_PORT		PORTC
	#define IN_STOP_PIN			PINC
	#define IN_STOP				(1<<PC6)

	// Interrupts
	#define INT_100us 		TIMER0_OVF_vect
	#define KEYBOARD_CLOCK	INT0_vect

	// Virtual Page for editor MAX_LINES x MAX_COLUMS
	#define MAX_LINES 25
	#define MAX_COLUMNS 40
#endif

#ifdef ATMEGA2560
	// Pinout Arduino MEGA 2560
	// Diagnostic LED
	#define		LED_STRING_1_DDR()			DDRB |= (1<<PB7)
	#define		LED_STRING_1_TRG()			PORTB ^= (1<<PB7) // change state
	#define		LED_STRING_1_ON()			PORTB |= (1<<PB7)
	#define		LED_STRING_1_OFF()			PORTB &= ~(1<<PB7)

	// Display
	#define LCD_RS_DIR		DDRL
	#define LCD_RS_PORT 	PORTL
	#define LCD_RS_PIN		PINL
	#define LCD_RS			(1 << PL0)

	#define LCD_RW_DIR		DDRL
	#define LCD_RW_PORT		PORTL
	#define LCD_RW_PIN		PINL
	#define LCD_RW			(1 << PL1)

	#define LCD_E_DIR		DDRL
	#define LCD_E_PORT		PORTL
	#define LCD_E_PIN		PINL
	#define LCD_E			(1 << PL2)

	#define LCD_DB4_DIR		DDRL
	#define LCD_DB4_PORT	PORTL
	#define LCD_DB4_PIN		PINL
	#define LCD_DB4			(1 << PL3)

	#define LCD_DB5_DIR		DDRL
	#define LCD_DB5_PORT	PORTL
	#define LCD_DB5_PIN		PINL
	#define LCD_DB5			(1 << PL4)

	#define LCD_DB6_DIR		DDRL
	#define LCD_DB6_PORT	PORTL
	#define LCD_DB6_PIN		PINL
	#define LCD_DB6			(1 << PL5)

	#define LCD_DB7_DIR		DDRL
	#define LCD_DB7_PORT	PORTL
	#define LCD_DB7_PIN		PINL
	#define LCD_DB7			(1 << PL6)

	#define LCD_BL 				(1<<PL7)
	#define	BACK_LIGHT_DIR		DDRL
	#define	BACK_LIGHT_TRG()  	PORTL ^= LCD_BL // change state
	#define	BACK_LIGHT_ON() 	PORTL |= LCD_BL
	#define	BACK_LIGHT_OFF()  	PORTL &= ~LCD_BL

	#define KEYBOARD_DATA_DIR	DDRD
	#define KEYBOARD_DATA_PORT	PORTD
	#define KEYBOARD_DATA_PIN	PIND
	#define KEYBOARD_DATA		(1<<PD3)

	#define KEYBOARD_CLK_DIR	DDRD
	#define KEYBOARD_CLK_PORT	PORTD
	#define KEYBOARD_CLK_PIN	PIND
	#define KEYBOARD_CLK		(1<<PD2)

	#define ST_MO_1_DIR_DIR		DDRB
	#define ST_MO_1_DIR_PORT	PORTB
	#define ST_MO_1_DIR_PIN		PINB
	#define ST_MO_1_DIR			(1<<PB5)

	#define ST_MO_1_PUL_DIR		DDRB
	#define ST_MO_1_PUL_PORT	PORTB
	#define ST_MO_1_PUL_PIN		PINB
	#define ST_MO_1_PUL			(1<<PB4)

	#define ST_MO_2_DIR_DIR		DDRH
	#define ST_MO_2_DIR_PORT	PORTH
	#define ST_MO_2_DIR_PIN		PINH
	#define ST_MO_2_DIR			(1<<PH6)

	#define ST_MO_2_PUL_DIR		DDRH
	#define ST_MO_2_PUL_PORT	PORTH
	#define ST_MO_2_PUL_PIN		PINH
	#define ST_MO_2_PUL			(1<<PH5)

	#define ST_MO_3_DIR_DIR		DDRH
	#define ST_MO_3_DIR_PORT	PORTH
	#define ST_MO_3_DIR_PIN		PINH
	#define ST_MO_3_DIR			(1<<PH4)

	#define ST_MO_3_PUL_DIR		DDRH
	#define ST_MO_3_PUL_PORT	PORTH
	#define ST_MO_3_PUL_PIN		PINH
	#define ST_MO_3_PUL			(1<<PH3)

	#define ST_MO_4_DIR_DIR		DDRE
	#define ST_MO_4_DIR_PORT	PORTE
	#define ST_MO_4_DIR_PIN		PINE
	#define ST_MO_4_DIR			(1<<PE3)

	#define ST_MO_4_PUL_DIR		DDRG
	#define ST_MO_4_PUL_PORT	PORTG
	#define ST_MO_4_PUL_PIN		PING
	#define ST_MO_4_PUL			(1<<PG5)

	#define ST_MO_5_DIR_DIR		DDRE
	#define ST_MO_5_DIR_PORT	PORTE
	#define ST_MO_5_DIR_PIN		PINE
	#define ST_MO_5_DIR			(1<<PE5)

	#define ST_MO_5_PUL_DIR		DDRE
	#define ST_MO_5_PUL_PORT	PORTE
	#define ST_MO_5_PUL_PIN		PINE
	#define ST_MO_5_PUL			(1<<PE4)

	#define ST_MO_6_DIR_DIR		DDRE
	#define ST_MO_6_DIR_PORT	PORTE
	#define ST_MO_6_DIR_PIN		PINE
	#define ST_MO_6_DIR			(1<<PE5)

	#define ST_MO_6_PUL_DIR		DDRE
	#define ST_MO_6_PUL_PORT	PORTE
	#define ST_MO_6_PUL_PIN		PINE
	#define ST_MO_6_PUL			(1<<PE4)

	// Inputs
	#define IN_START_DIR		DDRF
	#define IN_START_PORT		PORTF
	#define IN_START_PIN		PINF
	#define IN_START			(1<<PF2)

	#define IN_STOP_DIR			DDRF
	#define IN_STOP_PORT		PORTF
	#define IN_STOP_PIN			PINF
	#define IN_STOP				(1<<PF3)

	// Interrupts
	#define INT_100us 		TIMER0_OVF_vect
	#define KEYBOARD_CLOCK	INT2_vect

	// Virtual Page for editor MAX_LINES x MAX_COLUMS
	#define MAX_LINES 25
	#define MAX_COLUMNS 40
#endif


#define DISPLAY_ROWS 3
#define STATUS_BAR_ROW 4
#define DISPLAY_COLUMNS 20
volatile char vDisplay[DISPLAY_ROWS][DISPLAY_COLUMNS];
volatile char vStatusBar[1][DISPLAY_COLUMNS];
volatile char vPaper[MAX_LINES][MAX_COLUMNS];
volatile uint32_t free_time;
