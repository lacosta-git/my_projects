//Wysylanie wartosci z FLASH na SPI

#include <C:/WinAVR-20090313/avr/include/avr/io.h>
#include <C:/WinAVR-20090313/avr/include/avr/interrupt.h>
#include <D:/Projekty/AVRStudio/FLASH_SPI/ATmega8_PIN_Config.h>

// Wartosc TC1
#define TC1_Wartosc 0xF150
// #define TC1_Wartosc 0xFC2F          // 1 sec dla preskalera clk/1024

#ifndef F_CPU
//define cpu clock speed if not defined
#define F_CPU 8000000
#endif
//set desired baud rate
#define BAUDRATE 4800
//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1) 



//Ustawienia trybow pracy interfejsu SPI (w rejestrze SPCR)
#define	SPI_MODE_0		0
#define	SPI_MODE_1		CPHA
#define	SPI_MODE_2		CPOL
#define	SPI_MODE_3		CPOL+CPHA


//Prototypy funkcji
void spi_init(void);
void spi_cs_on(void);
void spi_cs_off(void);

void spi_zapisz_bajt(unsigned char dana);
unsigned char spi_odczytaj_bajt(void);
//void spi_zapisz_blok(unsigned char* blok, unsigned ilosc);
//void spi_odczytaj_blok(unsigned char* blok, unsigned ilosc);



uint16_t Czas_w_sec;
uint8_t y, tik;
uint8_t TempL;
uint8_t TempH;
uint8_t Temp;
unsigned char Temp_char;


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
	while(!(SPSR & SPIF));
}//spi_zapisz_bajt

/*******************************************************************************
Odczytanie przez interfejs jednego bajtu.
Zwraca:
	- odczytany bajt.
*******************************************************************************/
unsigned char spi_odczytaj_bajt(void)
{
	SPDR = 0x0;
	while(!(SPSR & SPIF));
	return(SPDR);
}//spi_odczytaj_bajt

/*******************************************************************************
Wyslanie przez interfejs bloku danych.
Parametry:
	- blok			wskaznik na blok danych,
	- ilosc			ilosc danych do wyslania.
******************************************************************************
void spi_zapisz_blok(unsigned char* blok, unsigned ilosc)
{
unsigned i;

	for(i=0;i<ilosc;i++)
	{
		SPDR = *(blok+i);
		while(!(SPSR & SPIF));
	}
}//spi_zapisz_blok

*******************************************************************************
Odczytanie przez interfejs bloku danych.
Parametry:
	- blok			wskaznik na bufor na dane,
	- ilosc			ilosc danych do odebrania.
******************************************************************************
void spi_odczytaj_blok(unsigned char* blok, unsigned ilosc)
{
unsigned i;

	for(i=0;i<ilosc;i++)
	{
		SPDR = 0xFF;
		while(!(SPSR & SPIF));
		*(blok+i) = SPDR;
	}
}//spi_odczytaj_blok
*/

// TC1 Przerwanie
SIGNAL (SIG_OVERFLOW1)
{
	TCNT1 = TC1_Wartosc;        		// przeladuj timer 1
	
	y=0;
	UDR = 0x0D;						// Powrot karetki CR. Rozpoczecie transmisji danych na RS	

}



// Obsluga przerwania or RS Tx
ISR(USART_TXC_vect)
{


switch(y)
	{

	case 0:  
			LOW_AT45DB_nCS();
			UDR = 0x0A;								// New Line in ASCII
			y++;
	break;
	case 1:  

//			spi_zapisz_bajt(0x9F);					// komenda
			SPDR = 0x9F;
			LED7_TRG();
			while(!(SPSR & (1<<SPIF)));
			LED6_ON();

			SPDR = 0x0;
			while(!(SPSR & (1<<SPIF)));
			Temp_char = SPDR;		        		// odczytanie bajtu

			Temp = ((Temp_char>>4) + 0x30);				// wartosc + ASCII			

			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			} 
			
			UDR = Temp;								// Wyslanie X-
			y++;
	break;
	case 2:  

			Temp = ((Temp_char & 0x0F) + 0x30);			// wartosc + ASCII
			
			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			}

			UDR = Temp;								// Wyslanie -X
			y++;

	break;	
	case 3:  

//			Temp_char = spi_odczytaj_bajt();				// odczytanie bajtu

			SPDR = 0x0;
			while(!(SPSR & (1<<SPIF)));
			Temp_char = SPDR;

			Temp = ((Temp_char>>4) + 0x30);				// wartosc + ASCII			

			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			} 
			
			UDR = Temp;								// Wyslanie X-
			y++;
	break;
	case 4:  

			Temp = ((Temp_char & 0x0F) + 0x30);			// wartosc + ASCII
			
			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			}

			UDR = Temp;								// Wyslanie -X
			y++;

	break;	
	default: ;
	}





}


int main (void)
{

// Ustawienia TC1
  TIMSK = _BV(TOIE1);     	         // wlacz obsluge przerwan T/C1
  TCNT1 = TC1_Wartosc;      	     // wartosc poczatkowa T/C1
  TCCR1A = 0x00;       		    	 // wlacz tryb czasomierza T/C1

// Preskaler
// CS12 CS11 CS10
//   0    0    0   -   No clock source. (Timer/Counter stopped)  
//   0    0    1   -   clk/1 (No prescaling)
//   0    1    0   -   clk/8 
//   0    1    1   -   clk/64 
//   1    0    0   -   clk/256 
//   1    0    1   -   clk/1024 
//  TCCR1B = _BV(CS12)|_BV(CS11)|_BV(CS10);

  TCCR1B = (_BV(CS10)|_BV(CS12));       // clk/1024
                 

// Ustawienia RS
	//Set baud rate
	UBRRL=UBRRVAL;		//low byte
	UBRRH=(UBRRVAL>>8);	//high byte

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);	
	//Enable Transmitter and Receiver and NO Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<TXCIE);

// Ustawienia ADC

	// AVCC with external capacitor at AREF pin
	// ADC Left Adjust Result 
	// ADC input select
//	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX1)|_BV(MUX0);

// ADC ON
// Preskaler na 125k dla F_OSC = 1MHz + start konwersji
//	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);
  LED_OUTPUT();
  spi_init();

  sei();       				         // wlacz obsluge przerwan


	while(1){	

	}
	return(1);
}
