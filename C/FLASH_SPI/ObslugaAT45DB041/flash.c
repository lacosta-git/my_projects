/*******************************************************************************
Obsluga komunikacji z pamiecia FLASH AT45DB041B produkcji ATMEL-a, z 
wykorzystaniem interfejsu SPI. Komunikacja dziala w trybie SPI Mode 0.

UWAGA!!!!
W CHWILI OBECNEJ STEROWNIK OBSLUGUJE 2 UKLADY

Uklad nr 2 w mapie pamieci jest umieszczony za 1 i jest traktowany jako dalszy
ciag tej samej matrycy pamieci. Strony, sa w nim numerowane, jako kolejne.


Do operacji odczytu/zapisu z wykorzystaniem bufora jest uzywany BUFOR_1.
Do operacji AUTO_PAGE_REWRITE jest uzywany BUFOR_2.

Architektura pamieci:
---------------------------------
SEKTOR 0
		BLOK 0
			STRONA 0 ... STRONA 7
---------------------------------
SEKTOR 1
		BLOK 1
			STRONA 8 ... STRONA 15
		...
		...
		BLOK 31
			STRONA 248 ... STRONA 255
---------------------------------
SEKTOR 2
		BLOK 32
			STRONA 256 ... STRONA 263
		...
		...
		BLOK 63
			STRONA 504 ... STRONA 511
---------------------------------
SEKTOR 3
		BLOK 64
			STRONA 512 ... STRONA 519
		...
		...
		BLOK 127
			STRONA 1016 ... STRONA 1023
---------------------------------
SEKTOR 4
		BLOK 128
			STRONA 1024 ... STRONA 1031
		...
		...
		BLOK 191
			STRONA 1528 ... STRONA 1535
---------------------------------
SEKTOR 5
		BLOK 192
			STRONA 1536 ... STRONA 1543
		...
		...
		BLOK 255
			STRONA 2040 ... STRONA 2047
---------------------------------
Kazda strona ma 264 bajty.
*******************************************************************************/
#include <string.h>
#include "glowne_def.h"
#include "spi.h"
#include "mapa_pamieci.h"
#include "system.h"
#include "lcd.h"
#include "stale_napisy.h"



enum{
	FLASH_1,
	FLASH_2
};


//Bity w rejestrze statusu pamieci FLASH
#define	FLASH_BUSY									0x80
#define	FLASH_ROZMIAR								0x38
//Kody polecen
#define	CONTINUOUS_ARRAY_READ					0xe8
#define	MAIN_MEMORY_PAGE_READ					0xd2
#define	BUFFER_READ									0xd4
#define	BUFFER_2_READ								0xd6
#define	STATUS_REGISTER_READ						0xd7
#define	BUFFER_WRITE								0x84
#define	BUFFER_2_WRITE								0x87
#define	BUFFER_TO_MEMORY_WITH_ERASE			0x83
#define	BUFFER_2_TO_MEMORY_WITH_ERASE			0x86
#define	BUFFER_TO_MEMORY_WITHOUT_ERASE		0x88
#define	PAGE_ERASE									0x81
#define	BLOCK_ERASE									0x50
#define	PAGE_PROGRAM_THROUGH_BUFFER			0x82
#define	PAGE_TO_BUFFER_TRANSFER					0x53
#define	PAGE_TO_BUFFER_2_TRANSFER				0x55
#define	PAGE_TO_BUFFER_COMPARE					0x60
#define	PAGE_TO_BUFFER_2_COMPARE				0x61
#define	AUTO_PAGE_REWRITE							0x59

//Do testu pamieci FLASH
#define	WEKTOR_TESTOWY								0xaa

//Prototypy funkcji
void flash_busy(unsigned char nr_flash);
void flash_init(void);
void strona_do_bufora(unsigned nr_strony);
void bufor_na_strone(unsigned nr_strony);
void odczytaj_z_bufora(unsigned nr_strony, unsigned offset, unsigned char* blok, unsigned ilosc);
void zapisz_do_bufora(unsigned nr_strony, unsigned offset, unsigned char* blok, unsigned ilosc);
void odczytaj_bez_bufora(unsigned nr_strony, unsigned offset, unsigned char* blok, unsigned ilosc);
void auto_page_rewrite(unsigned nr_strony);
void kasuj_strone(unsigned nr_strony);
void kasuj_blok(unsigned nr_bloku);
void test_pamieci_flash(void);

/*******************************************************************************
Oczekiwanie na zakonczenie przez pamiec wewnetrznych operacji (programowanie, 
kasowanie, transfer, etc...). Stan pamieci jest sprawdzany poprzez odczyt 
rejestru statusu.
*******************************************************************************/
void flash_busy(unsigned char nr_flash)
{
unsigned char status;

	if(nr_flash == FLASH_1)
	{
		do{
			spi_cs1_on();
				spi_zapisz_bajt(STATUS_REGISTER_READ);
				status = spi_odczytaj_bajt();
			spi_cs1_off();
		}while(!(status & FLASH_BUSY));
	}
	else
	{
		do{
			spi_cs2_on();
				spi_zapisz_bajt(STATUS_REGISTER_READ);
				status = spi_odczytaj_bajt();
			spi_cs2_off();
		}while(!(status & FLASH_BUSY));
	}
}//flash_busy

/*******************************************************************************
Inicjalizacja pamieci.
*******************************************************************************/
void flash_init(void)
{
unsigned i;

	//RESET wykonany
	for(i=0;i<4000;i++);			//czekaj 20ms na "rozgrzanie sie"
}//flash_init

/*******************************************************************************
Transfer strony do bufora. Chip select wybierany na podstawie numeru strony.
bufor_flash[3] - dowolny bajt.
Parametry:
	- nr_strony			ktora ma byc przetransferowana
*******************************************************************************/
void strona_do_bufora(unsigned nr_strony)
{
unsigned char bufor_flash[4];

	bufor_flash[0] = PAGE_TO_BUFFER_TRANSFER;
	if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
	{
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs1_off();
	}
	else
	{
		nr_strony -= FLASH_ILOSC_STRON_NA_UKLAD;
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs2_off();
	}
}//strona_do_bufora

/*******************************************************************************
Zapisanie zawartosci bufora do pamieci glownej z wbudowanym kasowaniem.
Chip select wybierany na podstawie numeru strony.
bufor_flash[3] - dowolny bajt.
Paramety:
	- nr_strony			na ktora ma byc zapisany bufor.
*******************************************************************************/
void bufor_na_strone(unsigned nr_strony)
{
unsigned char bufor_flash[4];

	bufor_flash[0] = BUFFER_TO_MEMORY_WITH_ERASE;
	if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
	{
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs1_off();
	}
	else
	{
		nr_strony -= FLASH_ILOSC_STRON_NA_UKLAD;
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs2_off();
	}
}//bufor_na_strone

/*******************************************************************************
Odczyt danych z bufora. 
bufor_flash[1] i bufor_flash[4] - dowolne bajty.
parametry:
	- nr_strony			z ktorej zostaly zaladowane odczytywane dane
	- offset				adres liczony od poczatku bufora
	- blok				wskaznik na bufor, w ktorym maja byc umieszczone odczytane 
							dane
	- ilosc				danych do odczytania
*******************************************************************************/
void odczytaj_z_bufora(unsigned nr_strony, unsigned offset, unsigned char* blok, unsigned ilosc)
{
unsigned char bufor_flash[5];

	bufor_flash[0] = BUFFER_READ;
	bufor_flash[2] = (unsigned char) (offset>>8);
	bufor_flash[3] = (unsigned char) offset;
	if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
	{
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
			spi_odczytaj_blok(blok,ilosc);
		spi_cs1_off();
	}
	else
	{
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
			spi_odczytaj_blok(blok,ilosc);
		spi_cs2_off();
	}
}//odczytaj_z_bufora

/*******************************************************************************
Zapis danych do bufora. Numer ukladu MUSI byc wczesniej ustawiony!
bufor_flash[1] - dowolny bajt.
parametry:
	- nr_strony			na ktorej maja wyladowac zapisywane dane
	- offset				adres liczony od poczatku bufora
	- blok				wskaznik na bufor z danymi do zapisania
	- ilosc				danych do zapisania
*******************************************************************************/
void zapisz_do_bufora(unsigned nr_strony, unsigned offset, unsigned char* blok, unsigned ilosc)
{
unsigned char bufor_flash[4];

	bufor_flash[0] = BUFFER_WRITE;
	bufor_flash[2] = (unsigned char) (offset>>8);
	bufor_flash[3] = (unsigned char) offset;
	if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
	{
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
			spi_zapisz_blok(blok,ilosc);
		spi_cs1_off();
	}
	else
	{
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
			spi_zapisz_blok(blok,ilosc);
		spi_cs2_off();
	}
}//zapisz_do_bufora

/*******************************************************************************
Odczyt danych z pamieci, bez uzycia bufora. Dane moga znajdowac sie na wiecej 
niz 1 stronie. Chip select wybierany na podstawie numeru strony.
bufor_flash[4]...bufor_flash[7] - dowolne bajty.
Parametry:
	- nr_strony			adres poczatku bloku danych (strona)
	- offset				adres poczatku bloku danych (wzgledem poczatku strony)
	- blok				wskaznik na bufor, w ktorym maja byc umieszczone odczytane 
							dane
	- ilosc				danych do odczytania
*******************************************************************************/
void odczytaj_bez_bufora(unsigned nr_strony, unsigned offset, unsigned char* blok, unsigned ilosc)
{
unsigned char bufor_flash[8];
unsigned char ilosc_stron_do_odczytania;
unsigned char ilosc_bajtow_do_odczytania_1;
unsigned char ilosc_bajtow_do_odczytania_2;

	if(ilosc<=FLASH_ILOSC_BAJTOW_NA_STRONE-offset)
	{
		//odczyt tylko z 1 strony
		bufor_flash[0] = CONTINUOUS_ARRAY_READ;
		if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
		{
			bufor_flash[1] = (unsigned char) (nr_strony>>7);
			bufor_flash[2] = ((unsigned char) (nr_strony<<1) ) | ((unsigned char) (offset>>8));
			bufor_flash[3] = (unsigned char) offset;
			flash_busy(FLASH_1);
			spi_cs1_on();
				spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
				spi_odczytaj_blok(blok,ilosc);
			spi_cs1_off();
		}
		else
		{
			nr_strony -= FLASH_ILOSC_STRON_NA_UKLAD;
			bufor_flash[1] = (unsigned char) (nr_strony>>7);
			bufor_flash[2] = ((unsigned char) (nr_strony<<1)) | ((unsigned char) (offset>>8));
			bufor_flash[3] = (unsigned char) offset;
			flash_busy(FLASH_2);
			spi_cs2_on();
				spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
				spi_odczytaj_blok(blok,ilosc);
			spi_cs2_off();
		}
	}
	else
	{
		//odczyt z wiecej niz 1 strony
		ilosc_stron_do_odczytania = (ilosc - (FLASH_ILOSC_BAJTOW_NA_STRONE - offset)) / FLASH_ILOSC_BAJTOW_NA_STRONE;
		ilosc_stron_do_odczytania++;
		if((ilosc - (FLASH_ILOSC_BAJTOW_NA_STRONE - offset)) % FLASH_ILOSC_BAJTOW_NA_STRONE)
		{
			ilosc_stron_do_odczytania++;
		}
		if((nr_strony < FLASH_ILOSC_STRON_NA_UKLAD) && (nr_strony+ilosc_stron_do_odczytania>FLASH_ILOSC_STRON_NA_UKLAD))
		{
			//odczyt z dwoch ukladow
			ilosc_bajtow_do_odczytania_1 = (FLASH_ILOSC_STRON_NA_UKLAD - nr_strony - 1) * FLASH_ILOSC_BAJTOW_NA_STRONE;
			ilosc_bajtow_do_odczytania_1 += FLASH_ILOSC_BAJTOW_NA_STRONE - offset;
			ilosc_bajtow_do_odczytania_2 = ilosc - ilosc_bajtow_do_odczytania_1;
			//odczytaj z 1
			bufor_flash[0] = CONTINUOUS_ARRAY_READ;
			bufor_flash[1] = (unsigned char) (nr_strony>>7);
			bufor_flash[2] = ((unsigned char) (nr_strony<<1)) | ((unsigned char) (offset>>8));
			bufor_flash[3] = (unsigned char) offset;
			flash_busy(FLASH_1);
			spi_cs1_on();
				spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
				spi_odczytaj_blok(blok,ilosc_bajtow_do_odczytania_1);
			spi_cs1_off();
			//odczytaj z 2
			bufor_flash[0] = CONTINUOUS_ARRAY_READ;
			bufor_flash[1] = 0;
			bufor_flash[2] = 0;
			bufor_flash[3] = 0;
			flash_busy(FLASH_2);
			spi_cs2_on();
				spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
				spi_odczytaj_blok(blok+ilosc_bajtow_do_odczytania_1,ilosc_bajtow_do_odczytania_2);
			spi_cs2_off();
		}
		else
		{
			//odczyt z jednego ukladu
			bufor_flash[0] = CONTINUOUS_ARRAY_READ;
			if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
			{
				bufor_flash[1] = (unsigned char) (nr_strony>>7);
				bufor_flash[2] = ((unsigned char) (nr_strony<<1) ) | ((unsigned char) (offset>>8));
				bufor_flash[3] = (unsigned char) offset;
				flash_busy(FLASH_1);
				spi_cs1_on();
					spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
					spi_odczytaj_blok(blok,ilosc);
				spi_cs1_off();
			}
			else
			{
				nr_strony -= FLASH_ILOSC_STRON_NA_UKLAD;
				bufor_flash[1] = (unsigned char) (nr_strony>>7);
				bufor_flash[2] = ((unsigned char) (nr_strony<<1)) | ((unsigned char) (offset>>8));
				bufor_flash[3] = (unsigned char) offset;
				flash_busy(FLASH_2);
				spi_cs2_on();
					spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
					spi_odczytaj_blok(blok,ilosc);
				spi_cs2_off();
			}
		}
	}
}//odczytaj_bez_bufora

/*******************************************************************************
Odswiezenie strony w pamieci. Dane sa kopiowane do bufora, a nastepnie bufor 
jest ponownie zapisywany na tej samej stronie. Chip select wybierany na podstawie 
numeru strony.
bufor_flash[3] - dowolny bajt.
Parametry:
	- nr_strony			ktora ma byc odswiezona.
*******************************************************************************/
void auto_page_rewrite(unsigned nr_strony)
{
unsigned char bufor_flash[4];

	bufor_flash[0] = AUTO_PAGE_REWRITE;
	if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
	{
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs1_off();
	}
	else
	{
		nr_strony -= FLASH_ILOSC_STRON_NA_UKLAD;
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs2_off();
	}
}//auto_page_rewrite

/*******************************************************************************
Skasowanie jednej strony. Chip select wybierany na podstawie numeru strony.
bufor_flash[3] - dowolny bajt.
Parametry:
	- nr_strony			ktora ma byc skasowana.
*******************************************************************************/
void kasuj_strone(unsigned nr_strony)
{
unsigned char bufor_flash[4];

	bufor_flash[0] = PAGE_ERASE;
	if(nr_strony < FLASH_ILOSC_STRON_NA_UKLAD)
	{
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs1_off();
	}
	else
	{
		nr_strony -= FLASH_ILOSC_STRON_NA_UKLAD;
		bufor_flash[1] = (unsigned char) (nr_strony>>7);
		bufor_flash[2] = (unsigned char) (nr_strony<<1);
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs2_off();
	}	
}//kasuj_strone

/*******************************************************************************
Skasowanie bloku stron. 1 blok to 8 stron. Chip select wybierany na podstawie 
numeru bloku.
bufor_flash[3] - dowolny bajt. 
Parametry:
	- nr_bloku			ktory ma byc skasowany.
*******************************************************************************/
void kasuj_blok(unsigned nr_bloku)
{
unsigned char bufor_flash[4];

	bufor_flash[0] = BLOCK_ERASE;
	if(nr_bloku < FLASH_ILOSC_BLOKOW_NA_UKLAD)
	{
		bufor_flash[1] = (unsigned char) (nr_bloku>>4);
		bufor_flash[2] = (unsigned char) (nr_bloku<<4);
		flash_busy(FLASH_1);
		spi_cs1_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs1_off();
	}
	else
	{
		nr_bloku -= FLASH_ILOSC_BLOKOW_NA_UKLAD;
		bufor_flash[1] = (unsigned char) (nr_bloku>>4);
		bufor_flash[2] = (unsigned char) (nr_bloku<<4);
		flash_busy(FLASH_2);
		spi_cs2_on();
			spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
		spi_cs2_off();
	}
}//kasuj_blok

/*******************************************************************************
Test pamieci FLASH. Podczas wykonywania testu oraz po jego zakonczeniu 
miernik nie reaguje na klawiature, ani na transmisje. Mozna go tylko wylaczyc.
test da sie uruchomic tylko gdy wyswietlany ekran powitalny.
Wyniki testu sa wyswietlane na ekranie miernika.

UWAGA!!! Funkcja serwisowa!
*******************************************************************************/
void test_pamieci_flash(void)
{
unsigned strona_testowana;
unsigned i;
unsigned char bufor_flash[8];
unsigned char blad;
float procent;

	__disable_interrupt();
	__watchdog_reset();
	//Zapisz wektor testowy do bufora 2 ukladow FLASH_1 i FLASH_2
	bufor_flash[0] = BUFFER_2_WRITE;
	bufor_flash[2] = 0;
	bufor_flash[3] = 0;
	flash_busy(FLASH_1);
	spi_cs1_on();
		spi_zapisz_blok(bufor_flash,4);
		for(i=0;i<FLASH_ILOSC_BAJTOW_NA_STRONE;i++)
		{
			spi_zapisz_bajt(0xaa);
		}
	spi_cs1_off();
	flash_busy(FLASH_2);
	spi_cs2_on();
		spi_zapisz_blok(bufor_flash,4);
		for(i=0;i<FLASH_ILOSC_BAJTOW_NA_STRONE;i++)
		{
			spi_zapisz_bajt(WEKTOR_TESTOWY);
		}
	spi_cs2_off();

	//Wykonuj test
	blad = NIE;
	clrscr();
	puts_P(_test_pamieci_flash);
	for(strona_testowana=0;strona_testowana<FLASH_MAX_ILOSC_STRON;strona_testowana++)
	{
		__watchdog_reset();
		//Przepisz strone pamieci do bufora1
		strona_do_bufora(strona_testowana);
		//Zapisz bufor2 na strone pamieci
		bufor_flash[0] = BUFFER_2_TO_MEMORY_WITH_ERASE;
		if(strona_testowana < FLASH_ILOSC_STRON_NA_UKLAD)
		{
			bufor_flash[1] = (unsigned char) (strona_testowana>>7);
			bufor_flash[2] = (unsigned char) (strona_testowana<<1);
			flash_busy(FLASH_1);
			spi_cs1_on();
				spi_zapisz_blok(bufor_flash,4);
			spi_cs1_off();
		}
		else
		{
			bufor_flash[1] = (unsigned char) ((strona_testowana-FLASH_ILOSC_STRON_NA_UKLAD)>>7);
			bufor_flash[2] = (unsigned char) ((strona_testowana-FLASH_ILOSC_STRON_NA_UKLAD)<<1);
			flash_busy(FLASH_2);
			spi_cs2_on();
				spi_zapisz_blok(bufor_flash,4);
			spi_cs2_off();
		}
		//Odczytaj bez bufora i sprawdz czy OK
		//Jezeli blad, to ustaw blad
		bufor_flash[0] = CONTINUOUS_ARRAY_READ;
		if(strona_testowana < FLASH_ILOSC_STRON_NA_UKLAD)
		{
			bufor_flash[1] = (unsigned char) (strona_testowana>>7);
			bufor_flash[2] = (unsigned char) (strona_testowana<<1);
			bufor_flash[3] = (unsigned char) 0;
			flash_busy(FLASH_1);
			spi_cs1_on();
				spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
				for(i=0;i<FLASH_ILOSC_BAJTOW_NA_STRONE;i++)
				{
					if(spi_odczytaj_bajt() != WEKTOR_TESTOWY)
					{
						blad = TAK;
						break;
					}
				}
			spi_cs1_off();
		}
		else
		{
			bufor_flash[1] = (unsigned char) ((strona_testowana-FLASH_ILOSC_STRON_NA_UKLAD)>>7);
			bufor_flash[2] = (unsigned char) ((strona_testowana-FLASH_ILOSC_STRON_NA_UKLAD)<<1);
			bufor_flash[3] = (unsigned char) 0;
			flash_busy(FLASH_2);
			spi_cs2_on();
				spi_zapisz_blok(bufor_flash,sizeof(bufor_flash));
				for(i=0;i<FLASH_ILOSC_BAJTOW_NA_STRONE;i++)
				{
					if(spi_odczytaj_bajt() != WEKTOR_TESTOWY)
					{
						blad = TAK;
						break;
					}
				}
			spi_cs2_off();
		}
		//Zapisz bufor1 na strone pamieci
		bufor_na_strone(strona_testowana);
		//Wyswietl postep
		gotoxy(0,4);
		puts_P(_uklad);
		gotoxy(50,4);
		if(strona_testowana<FLASH_ILOSC_STRON_NA_UKLAD)
		{
			puts_P(_u5);
		}
		else
		{
			puts_P(_u17);
		}
		gotoxy(0,8);
		puts_P(_strona);
		sprintf_P(linia,_format_d,strona_testowana);
		gotoxy(50,8);
		puts(linia);
		procent = ((float)(strona_testowana+1))/((float)FLASH_MAX_ILOSC_STRON) * 1000.0;
		sprintf_P(linia,_format_procent,((unsigned) procent)/10,((unsigned) procent)%10);
		if(procent < 1000.0)
		{
			if(procent < 100.0)
			{
				gotoxy(102,8);
			}
			else
			{
				gotoxy(96,8);
			}
		}
		else
		{
			gotoxy(90,8);
		}
		puts(linia);
		//Sprawdz czy nie bylo bledu
		if(blad)
		{
			//Byl blad, zakoncz test
			break;
		}
	}

	//Wyswietl wyniki
	gotoxy(0,12);
	puts_P(_wynik);
	gotoxy(50,12);
	ustaw_czcionke(ZNAKI_7_BOLD);
	if(blad)
	{
		puts_P(_blad);
	}
	else
	{
		puts_P(_ok);
	}
	//Koniec testu. Czekaj.....
	while(1)
	{
		__watchdog_reset();
	}
}//test_pamieci_flash


