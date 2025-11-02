/*******************************************************************************
Sterownik interfejsu SPI.
Automatyczne jest sterowanie liniami: MOSI, MISO i SCK. SS jest wystawiane 
przez oprogramowanie.
Interfejs dziala jako MASTER, w trybie SPI 0, z predkoscia Fosc/2. 
Przy kwarcu 3,6864MHz jest to 1,8432MHz (w trybie SPI2X).
Wszystkie dane sa przesylane z MSB jako pierwszym.
*******************************************************************************/
#include <C:/WinAVR-20090313/avr/include/avr/io.h>


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



/*******************************************************************************
Inicjalizacja interfejsu.
*******************************************************************************/
void spi_init(void)
{
	SPCR = SPE + MSTR + SPI_MODE_3;
	SPSR = SPR1 + SPR0;                 //fosc/128 
}//spi_init

/*******************************************************************************
Sterowanie chip select-em SS. ON.
*******************************************************************************/
void spi_cs_on(void)
{
	PORTB &= ~PB2;
}//spi_cs_on

/*******************************************************************************
Sterowanie chip select-em SS. OFF.
*******************************************************************************/
void spi_cs_off(void)
{
	PORTB |= PB2;
}//spi_cs_off

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
	SPDR = 0xFF;
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

