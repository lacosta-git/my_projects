
#ifndef F_CPU
//define cpu clock speed if not defined
#define F_CPU 8000000
#endif
//set desired baud rate
#define BAUDRATE 9600
//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "atmega8_pin_config.h"

/*******************************************************************************
Inicjalizacja interfejsu.
*******************************************************************************/
void spi_init(void)
{
	AT45DB_nCS_OUTPUT();
	HI_AT45DB_nCS();

	SPI_MOSI_OUTPUT();
	HI_SPI_MOSI();

	SPI_MISO_INPUT();
	HI_SPI_MISO();

	SPI_SCK_OUTPUT();
	HI_SPI_SCK();

	SPCR = (1<<SPE) | (0<< DORD) | (1<< MSTR) | (1<< CPOL) | (1<< CPHA) |(1<<SPR1) | (1<<SPR0);
	SPSR = (0<<SPI2X);
}//spi_init

/*******************************************************************************
Wyslanie przez interfejs jednego bajtu.
Parametry:
	- dana		bajt do wyslania.
*******************************************************************************/
void spi_zapisz_bajt(unsigned char dana)
{
	SPDR = dana;
	while(!(SPSR & (1<<SPIF)));
}//spi_zapisz_bajt

/*******************************************************************************
Odczytanie przez interfejs jednego bajtu.
Zwraca:
	- odczytany bajt.
*******************************************************************************/
unsigned char spi_odczytaj_bajt(void)
{
	SPDR = 0x0;
	while(!(SPSR & (1<<SPIF)));
	return(SPDR);
}//spi_odczytaj_bajt


volatile uint8_t i;
volatile uint8_t RsByte;
volatile uint8_t firstByteReceivedOnRs;
volatile uint8_t commandToSPI;

int main(void)
{
	//Set baud rate
	UBRRL = (uint8_t)UBRRVAL;		//low byte
	UBRRH = (uint8_t)(UBRRVAL>>8);	//high byte

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);
	//Enable Transmitter and Receiver and NO Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<RXCIE)|(1<<TXEN)|(1<<TXCIE);
	
	LED_OUTPUT();
	firstByteReceivedOnRs = 0;
	spi_init();
	sei(); 
	while(1)
		{
			LED7_TRG();
			_delay_loop_2(60000);
			_delay_loop_2(60000);
		}
}


//*******************************************************
// RX
ISR(USART_RXC_vect)
{

	RsByte = UDR;  										// Odczyt danej portu rs

	// Start command search
	if (RsByte == ';')  //
		{
			LOW_AT45DB_nCS();
			commandToSPI = 0;
			firstByteReceivedOnRs = 0;
			LED6_TRG();
		}
	if (RsByte == '*')  //
		{
			HI_AT45DB_nCS();
		}
	if(RsByte == 'r')
		{
			UDR = spi_odczytaj_bajt();
			LED5_TRG();
		}
	if ((RsByte != 'r') & (RsByte != ';') & (RsByte != '*'))
		{
			if (firstByteReceivedOnRs == 0)
				{
					commandToSPI = (uint8_t)((RsByte & 0x0F)<<4);
					if (RsByte > 0x39)
					{
						commandToSPI = commandToSPI + 0x90;
					}
					firstByteReceivedOnRs = 1;
				}
			else
				{
					commandToSPI = commandToSPI + (uint8_t)(RsByte & 0x0F);
					if (RsByte > 0x39)
					{
						commandToSPI = commandToSPI + 0x09;
					}
					spi_zapisz_bajt(commandToSPI);
					UDR = commandToSPI;
				}
			LED4_TRG();
		}
}
//*******************************************************

//*******************************************************
// TX
ISR(USART_TXC_vect)
{
	
}
//*******************************************************