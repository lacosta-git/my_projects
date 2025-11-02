//Wysylanie wartosci z ADC na RS

#include <C:/WinAVR-20090313/avr/include/avr/io.h>
#include <C:/WinAVR-20090313/avr/include/avr/interrupt.h>

// Wartosc TC1
// #define TC1_Wartosc 0xFCDF             // 0.1 msec dla peskalera clk/1  8MHz
// #define TC1_Wartosc 0xF3CA            // 100 msec dla peskalera clk/256  8MHz
#define TC1_Wartosc 0xE17B            // 250 msec dla peskalera clk/256  8MHz
// #define TC1_Wartosc 0xFC2F          // 1 sec dla preskalera clk/1024 1MHz

#ifndef F_CPU
//define cpu clock speed if not defined
#define F_CPU 8000000
#endif
//set desired baud rate
#define BAUDRATE 2400
//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1) 


#define bit_PWM_1 ((PINB&0x02)>>1)    // PB1
#define bit_PWM_2 ((PINB&0x04)>>2)    // PB2


// *************************************
//             DEKLARACJE ZMIENNYCH
// *************************************

uint16_t Czas_w_sec;

// Zmienne PWM
uint16_t licznik;
uint16_t Ilosc_impulsow_okresu_PWM_1, Ilosc_impulsow_okresu_PWM_2, Ilosc_impulsow_1_pwm1, Ilosc_impulsow_1_pwm2;
uint16_t Ilosc_impulsow_okresu_PWM_1_temp, Ilosc_impulsow_okresu_PWM_2_temp, Ilosc_impulsow_1_pwm1_temp, Ilosc_impulsow_1_pwm2_temp;
uint8_t poprzednia_wartosc_bit_PWM1, poprzednia_wartosc_bit_PWM2;

uint8_t y;
uint8_t TempL;
uint8_t TempH;
uint8_t Temp;
//                												  i  Wartosc
const uint8_t wartosc_bitow[] = {0x00, 0x00, 0x00, 0x00, 0x01, // 0		1
								 0x00, 0x00, 0x00, 0x00, 0x02, // 5		2
								 0x00, 0x00, 0x00, 0x00, 0x04, // 10	4
								 0x00, 0x00, 0x00, 0x00, 0x08, // 15	8
								 0x00, 0x00, 0x00, 0x01, 0x06, // 20	16
								 0x00, 0x00, 0x00, 0x03, 0x02, // 25	32
								 0x00, 0x00, 0x00, 0x06, 0x04, // 30	64
								 0x00, 0x00, 0x01, 0x02, 0x08, // 35	128
								 0x00, 0x00, 0x02, 0x05, 0x06, // 40	256
								 0x00, 0x00, 0x05, 0x01, 0x02, // 45	512
								 0x00, 0x01, 0x00, 0x02, 0x04, // 50	1024
								 0x00, 0x02, 0x00, 0x04, 0x08, // 55	2048
								 0x00, 0x04, 0x00, 0x09, 0x06, // 60	4096
								 0x00, 0x08, 0x01, 0x09, 0x02, // 65	8192
								 0x01, 0x06, 0x03, 0x08, 0x04, // 70	16384
								 0x03, 0x02, 0x07, 0x06, 0x08, // 75	32768
								 };

uint8_t tempDec[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};            // index jest od 0 do 4 
// ****************************************
//             FUNKCJE
// ****************************************

// 16 bit hex to dec
void intHEXtoDEC (uint16_t inHEX)
{
uint32_t k;
uint16_t tempHEX;
uint8_t i, index_bitu;
int j;
	
	tempDec[0] = 0x00;
	tempDec[1] = 0x00;
	tempDec[2] = 0x00;
	tempDec[3] = 0x00;
	tempDec[4] = 0x00;

	tempHEX = inHEX;
	index_bitu = 0;


	for (k = 1; k<=0x8000; k = k*2)									// przesowanie po bitach inHEX
	{
		index_bitu++;												// przesowanie indexu bitu
		if (tempHEX & k)
		{
			i = (index_bitu*5) - 5;
			for( j = 4; j>=0; j--)
			{
				tempDec[j] = tempDec[j] + wartosc_bitow[j+i];        // Dodawanie
				
				if (tempDec[j] > 0x09)
				{
					tempDec[j] = tempDec[j] + 6;
				}

				if (tempDec[j] >= 0x10)
				{
					tempDec[j] = tempDec[j] & 0x0F;
					if (j>0)
					{
						tempDec[j-1] = tempDec[j-1] + 1;
					}
				}
			
			}
		}
	}


}

// **********************************
//              PRZERWANIA
// **********************************



// TC1 Przerwanie
SIGNAL (SIG_OVERFLOW1)
{
uint8_t licznik_zboczy_narastajacych;

	TCNT1 = TC1_Wartosc;        		// przeladuj timer 1
	licznik_zboczy_narastajacych = 0;

	do{
		Ilosc_impulsow_okresu_PWM_1_temp++;       // Okres + 1

		if((bit_PWM_1 == 1) & (poprzednia_wartosc_bit_PWM1 == 0))
		{
			Ilosc_impulsow_okresu_PWM_1 = (Ilosc_impulsow_okresu_PWM_1+Ilosc_impulsow_okresu_PWM_1_temp)/2;
			Ilosc_impulsow_1_pwm1 = (Ilosc_impulsow_1_pwm1+Ilosc_impulsow_1_pwm1_temp)/2;

			poprzednia_wartosc_bit_PWM1 = 1;
			Ilosc_impulsow_okresu_PWM_1_temp = 0;
			Ilosc_impulsow_1_pwm1_temp = 0;
			licznik_zboczy_narastajacych++;
		}

		if((bit_PWM_1 == 1) & (poprzednia_wartosc_bit_PWM1 == 1))
		{
			Ilosc_impulsow_1_pwm1_temp++;
		}


		if((bit_PWM_1 == 0) & (poprzednia_wartosc_bit_PWM1 == 1))
		{
			poprzednia_wartosc_bit_PWM1 = 0;
		}

	}while((licznik_zboczy_narastajacych <4) && (Ilosc_impulsow_okresu_PWM_1_temp < 30000));

// Wyslanie danych na rs co 0.5 sec
	licznik++;
	if (licznik >= 1)
	{
		Czas_w_sec++;
		y=0;
		UDR = 0x0D;						// Powrot karetki CR. Rozpoczecie transmisji danych na RS
		licznik = 0;
		PORTB = (!PORTB & 0x01);
	}
}




// Obsluga przerwania or RS Tx
ISR(USART_TXC_vect)
{
switch(y)
	{

	case 0:  

			UDR = 0x0A;								// New Line in ASCII
			y++;
	break;
	case 1:  
			TempL = Czas_w_sec;						// mlodszy byte Czas_w_sec
			TempH = (Czas_w_sec>>8);	

			Temp = ((TempH>>4) + 0x30);				// wartosc + ASCII			

			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			} 
			
			UDR = Temp;								// Wyslanie X--- z Czas_w_sec
			y++;
	break;
	case 2:  

			Temp = ((TempH & 0x0F) + 0x30);			// wartosc + ASCII
			
			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			}

			UDR = Temp;								// Wyslanie -X-- z Czas_w_sec
			y++;
	break;	
	case 3:  

			Temp = ((TempL>>4) + 0x30);				// wartosc + ASCII
			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			}
			UDR = Temp;								// Wyslanie --X- z Czas_w_sec
			y++;
	break;
	case 4:  

			Temp = ((TempL & 0x0F) + 0x30);			// wartosc + ASCII
			if (Temp > 0x39)
			{
				Temp = Temp + 7;
			}
			UDR = Temp;								// Wyslanie ---X z Czas_w_sec
			y++;
	break;
	case 5:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 6:  
			
			intHEXtoDEC(Ilosc_impulsow_1_pwm1);
			
			UDR = tempDec[0] + 0x30;
			y++;
	break;
	case 7: 
			UDR = tempDec[1] + 0x30;
			y++;
	break;

	case 8:  
			UDR = tempDec[2] + 0x30;
			y++;
	break;

	case 9:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 10:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;

	case 11:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 12:  
			
			intHEXtoDEC(Ilosc_impulsow_okresu_PWM_1);
			
			UDR = tempDec[0] + 0x30;
			y++;
	break;
	case 13: 
			UDR = tempDec[1] + 0x30;
			y++;
	break;

	case 14:  
			UDR = tempDec[2] + 0x30;
			y++;
	break;

	case 15:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 16:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 17:  

			UDR = 0x09;								// wypelnienie w procentach
			y++;
	break;
	case 18:  
			Temp = Ilosc_impulsow_1_pwm1/(Ilosc_impulsow_okresu_PWM_1/100);
			intHEXtoDEC(Temp);
			
			UDR = tempDec[0] + 0x30;
			y++;
	break;
	case 19: 
			UDR = tempDec[1] + 0x30;
			y++;
	break;

	case 20:  
			UDR = tempDec[2] + 0x30;
			y++;
	break;

	case 21:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 22:  
			UDR = tempDec[4] + 0x30;
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

  TCCR1B = _BV(CS12);       // clk/256
                 

// Ustawienia RS
	//Set baud rate
	UBRRL=UBRRVAL;		//low byte
	UBRRH=(UBRRVAL>>8);	//high byte

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);	
	//Enable Transmitter and Receiver and NO Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<TXCIE);


// Ustawienia portu B       
	DDRB = 0x01;					// LSB tylko jako wyjscie

	licznik = 0;
  sei();       				         // wlacz obsluge przerwan


	while(1){	

	}
	return(1);
}
