
#include "../main.h"

#define KEYBOARD_4x5

#ifdef ATMEGA8
	#ifdef KEYBOARD_4x5
		// KEYBOARD
		// Outputs
		#define KEY_COLUMNS			4

		#define COL1_DDR() 			DDRB |= (1<<PB6)
		#define	COL1_ON()			PORTB |= (1<<PB6)
		#define	COL1_OFF()			PORTB &= ~(1<<PB6)

		#define COL2_DDR() 			DDRB |= (1<<PB7)
		#define	COL2_ON()			PORTB |= (1<<PB7)
		#define	COL2_OFF()			PORTB &= ~(1<<PB7)

		#define COL3_DDR() 			DDRD |= (1<<PD5)
		#define	COL3_ON()			PORTD |= (1<<PD5)
		#define	COL3_OFF()			PORTD &= ~(1<<PD5)

		#define COL4_DDR() 			DDRD |= (1<<PD6)
		#define	COL4_ON()			PORTD |= (1<<PD6)
		#define	COL4_OFF()			PORTD &= ~(1<<PD6)


		// Inputs
		#define KEY_ROWS 			5

		#define ROW1_DDR()			DDRD &= ~(1<<PD7); PORTD |=(1<<PD7)  // INPUT with pull up
		#define READ_ROW1()			(PIND&(1<<PD7))>>PD7

		#define ROW2_DDR()			DDRB &= ~(1<<PB0); PORTB |=(1<<PB0)  // INPUT with pull up
		#define READ_ROW2()			(PINB&(1<<PB0))>>PB0

		#define ROW3_DDR()			DDRD &= ~(1<<PD2); PORTD |=(1<<PD2)  // INPUT with pull up
		#define READ_ROW3()			(PIND&(1<<PD2))>>PD2

		#define ROW4_DDR()			DDRD &= ~(1<<PD3); PORTD |=(1<<PD3)  // INPUT with pull up
		#define READ_ROW4()			(PIND&(1<<PD3))>>PD3

		#define ROW5_DDR()			DDRD &= ~(1<<PD4); PORTD |=(1<<PD4)  // INPUT with pull up
		#define READ_ROW5()			(PIND&(1<<PD4))>>PD4

		#define INIT_KEYBOARD_OUTPUTS()	COL1_DDR(); COL2_DDR(); COL3_DDR(); COL4_DDR()
		#define INIT_KEYBOARD_INPUTS()	ROW1_DDR(); ROW2_DDR(); ROW3_DDR(); ROW4_DDR(); ROW5_DDR()

		#define KEY_ARROW_LEFT		0x0101  // 0x ROW COL - bit number is line number
		#define KEY_ARROW_RIGHT		0x0104
		#define KEY_ARROW_UP		0x0808
		#define KEY_ARROW_DOWN		0x0408

		#define KEY_ENT				0x0108
		#define KEY_ESC				0x0208
		#define KEY_STAR			0x1008
		#define KEY_HASH			0x1004
		#define KEY_F2				0x1002
		#define KEY_F1				0x1001

		#define KEY_1				0x0801
		#define KEY_2				0x0802
		#define KEY_3				0x0804
		#define KEY_4				0x0401
		#define KEY_5				0x0402
		#define KEY_6				0x0404
		#define KEY_7				0x0201
		#define KEY_8				0x0202
		#define KEY_9				0x0204
		#define KEY_0				0x0102

		#endif  // KEYBOARD_4x5
#endif  // ATMEGA8

#define INIT_KEYBOARD_PORTS()	INIT_KEYBOARD_OUTPUTS(); INIT_KEYBOARD_INPUTS()
#define KEY_OUTPUTS_COUNT		KEY_COLUMNS
#define KEY_INPUTS_COUNT		KEY_ROWS

// FUNCTIONS
void init_keyboard(void);
void keyboard_handler(void);
uint16_t keyboard_read_keys(void);
