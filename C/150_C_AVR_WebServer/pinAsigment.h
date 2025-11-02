/************************************************
                Connections 
                 
      CPU SIDE
        PD7 (AIN1) -------------> LED Yelow 1

        PC0 (A0) <--------------- OutDoorTemp
		
		PB2 (SPI_CS) -----------> SPI_CS
		PB3 (SPI_MOSI) ---------> SPI_MOSI
		PB4 (SPI_MISO) <--------- SPI_MISO
		PB5 (SPI_SCK) ----------> SPI_SCK
 
*************************************************/

// 1 - Output
// 0 - Input
// Port D direction
#define		DDR_D()			DDRD = (1<<DDD7)

#define		LED_Y1_OFF()	PORTD &=~(1<<DDD7)
#define		LED_Y1_ON()		PORTD |= (1<<DDD7)
#define		LED_Y1_TRG()	PORTD ^= (1<<DDD7)

// Port C direction
/* Port C */
#define PC0_OUT		DDRC |= (1<<DDC0)
#define PC0_IN		DDRC &=~(1<<DDC0)
#define PC0_HiZ		DDRC &=~(1<<DDC0); PORTC |=(1<<PC0)
#define		PC0_OFF()	PORTC &=~(1<<PC0)
#define		PC0_ON()	PORTC |= (1<<PC0)
#define		PC0_TRG()	PORTC ^= (1<<PC0)

#define PC1_OUT		DDRC |= (1<<PC1)
#define PC1_IN		DDRC &=~(1<<PC1)
#define PC1_HiZ		DDRC &=~(1<<PC1); PORTC |=(1<<PC1)

#define PC2_OUT		DDRC |= (1<<PC2)
#define PC2_IN		DDRC &=~(1<<PC2)
#define PC2_HiZ		DDRC &=~(1<<PC2); PORTC |=(1<<PC2)

#define PC3_OUT		DDRC |= (1<<PC3)
#define PC3_IN		DDRC &=~(1<<PC3)
#define PC3_HiZ		DDRC &=~(1<<PC3); PORTC |=(1<<PC3)

#define PC4_OUT		DDRC |= (1<<PC4)
#define PC4_IN		DDRC &=~(1<<PC4)
#define PC4_HiZ		DDRC &=~(1<<PC4); PORTC |=(1<<PC4)

#define PC5_OUT		DDRC |= (1<<PC5)
#define PC5_IN		DDRC &=~(1<<PC5)
#define PC5_HiZ		DDRC &=~(1<<PC5); PORTC |=(1<<PC5)

#define PC6_OUT		DDRC |= (1<<PC6)
#define PC6_IN		DDRC &=~(1<<PC6)
#define PC6_HiZ		DDRC &=~(1<<PC6); PORTC |=(1<<PC6)

#define PC_OUT		PC0_OUT; PC1_OUT; PC2_OUT; PC3_OUT; PC4_OUT; PC5_OUT; PC6_OUT
#define PC_IN		PC0_IN; PC1_IN; PC2_IN; PC3_IN; PC4_IN; PC5_IN; PC6_IN
#define PC_HiZ		PC0_HiZ; PC1_HiZ; PC2_HiZ; PC3_HiZ; PC4_HiZ; PC5_HiZ; PC6_HiZ

// SPI
#define		SPI_PORT		PORTB
#define		SPI_CS			PORTB2
#define		SPI_MOSI		PORTB3
#define		SPI_MISO		PORTB4
#define		SPI_SCK			PORTB5

// Port B direction
// Set MOSI (PORTB3),SCK (PORTB5) and PORTB2 (SS) as output, others as input
#define		DDR_B()			DDRB = (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_CS)
