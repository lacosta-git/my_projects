//Wysylanie wartosci z ADC na RS

#include <C:/WinAVR-20090313/avr/include/avr/io.h>
#include <C:/WinAVR-20090313/avr/include/avr/interrupt.h>

#ifndef F_CPU
//define cpu clock speed if not defined
#define F_CPU 8000000
#endif
//set desired baud rate
#define BAUDRATE 19200
//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1)


#define PORT_LED        PORTB 
#define DDR_LED         DDRB 
 
#define LED_OUTPUT()    DDR_LED |=0x01
#define LED0_OFF()      PORT_LED&=~(1<<0) 
#define LED0_ON()       PORT_LED|= (1<<0)  
#define LED0_TRG()      PORT_LED^= (1<<0) 

// *************************************
//             DEKLARACJE ZMIENNYCH
// *************************************

volatile uint16_t Numer_pomiaru;
volatile uint32_t czas_pomiaru;
uint32_t Temp_czas_pomiaru;
uint8_t y;
uint8_t TempL;
uint8_t TempH;
uint8_t Temp;
volatile uint8_t ADC_port_1;
volatile uint8_t ADC_port_2;
volatile uint8_t ADC_port_3;
volatile uint8_t ADC_port_4;
volatile uint8_t ADC_port_5;
volatile uint8_t ADC_port_6;
volatile uint16_t TC1_Wartosc = 0xE17B;		// 1 sec dla 8 MHz i Fclk/1024
volatile uint32_t u32liczba_impulsow_1sec;
volatile uint16_t u16liczba_impulsow_1sec;

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

SIGNAL (SIG_OVERFLOW0)
{
	u32liczba_impulsow_1sec = u32liczba_impulsow_1sec + 0x100;
}

// TC1 Przerwanie
SIGNAL (SIG_OVERFLOW1)
{

	// Przeladowanie timera T1
	TCNT1 = TC1_Wartosc;        		// przeladuj timer 1
	
	//Obliczenie czasu pomiaru w msec	
	Temp_czas_pomiaru = 0xFFFF - TC1_Wartosc;
	Temp_czas_pomiaru = Temp_czas_pomiaru * 1000;
	Temp_czas_pomiaru = Temp_czas_pomiaru / 7812;
	czas_pomiaru = czas_pomiaru + Temp_czas_pomiaru;

	// Liczba impulsow na 1 sec
	u32liczba_impulsow_1sec = u32liczba_impulsow_1sec + TCNT0;
	u32liczba_impulsow_1sec = u32liczba_impulsow_1sec * 1000;
	u16liczba_impulsow_1sec = u32liczba_impulsow_1sec / Temp_czas_pomiaru;
	TCNT0 = 0; 							//Zerowanie mlodszego bajta licznika impulsow

	// Zwiekszanie numeru probki
	Numer_pomiaru++; 					// Zwiekszanie numeru pomiaru o 1

	// Rozpoczencie transmisji na RS
	y=0;
	UDR = 0x0D;						// Powrot karetki CR. Rozpoczecie transmisji danych na RS	

	// Miganie dioda
	LED0_TRG();

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
			intHEXtoDEC(Numer_pomiaru);
			
			UDR = tempDec[0] + 0x30;				// Wyslanie X--- z Numer_pomiaru
			y++;
	break;
	case 2:  
			UDR = tempDec[1] + 0x30;
			y++;
	break;
	case 3:  
			UDR = tempDec[2] + 0x30;
			y++;
	break;
	case 4:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 5:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 6:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 7:  
			Temp = (czas_pomiaru>>28);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 8:  
			Temp = (czas_pomiaru>>24);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;							
			y++;
	break;
	case 9:  
			Temp = (czas_pomiaru>>20);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 10:  
			Temp = (czas_pomiaru>>16);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 11:  
			Temp = (czas_pomiaru>>12);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 12:  
			Temp = (czas_pomiaru>>8);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 13:  
			Temp = (czas_pomiaru>>4);
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 14:  
			Temp = czas_pomiaru;
			Temp = (Temp & 0x0F) + 0x30;
			if (Temp > 0x39) Temp = Temp + 7;
			UDR = Temp;								
			y++;
	break;
	case 15:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 16:  
			intHEXtoDEC(u16liczba_impulsow_1sec);
			u32liczba_impulsow_1sec = 0;			// Wyzerowanie licznika impulsow
			
			UDR = tempDec[0] + 0x30;				// Wyslanie X--- z Numer_pomiaru
			y++;
	break;
	case 17:  
			UDR = tempDec[1] + 0x30;
			y++;
	break;
	case 18:  
			UDR = tempDec[2] + 0x30;
			y++;
	break;
	case 19:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 20:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 21:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 22: 
						 // czekanie na zakonczenie konwersji
			intHEXtoDEC(ADC_port_1);
			
			UDR = tempDec[2] + 0x30;				// Wyslanie X--- z dana
			y++;
	break;
	case 23:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 24:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 25:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 26:  

	// ADC1 input selected
			intHEXtoDEC(ADC_port_2);
			
			UDR = tempDec[2] + 0x30;				// Wyslanie X--- z dana
			y++;
	break;
	case 27:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 28:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 29:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 30:
		// ADC2 input selected
			intHEXtoDEC(ADC_port_3);
			
			UDR = tempDec[2] + 0x30;				// Wyslanie X--- z dana
			y++;
	break;
	case 31:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 32:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 33:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 34:
		// ADC3 input selected
			intHEXtoDEC(ADC_port_4);
			
			UDR = tempDec[2] + 0x30;				// Wyslanie X--- z dana
			y++;
	break;
	case 35:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 36:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 37:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 38:
		// ADC4 input selected 
			intHEXtoDEC(ADC_port_5);
			
			UDR = tempDec[2] + 0x30;				// Wyslanie X--- z dana
			y++;
	break;
	case 39:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 40:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	case 41:  

			UDR = 0x09;								// Wyslanie TAB in ASCII
			y++;
	break;
	case 42:
		// ADC5 input selected
			intHEXtoDEC(ADC_port_6);
			
			UDR = tempDec[2] + 0x30;				// Wyslanie X--- z dana
			y++;
	break;
	case 43:  
			UDR = tempDec[3] + 0x30;
			y++;
	break;
	case 44:  
			UDR = tempDec[4] + 0x30;
			y++;
	break;
	default: ;
	}

}

// RX
ISR(USART_RXC_vect)
{
char komenda;
uint16_t RS_temp;

	komenda = UDR;
	if (komenda == '1')					// Powrot do wartosci 1 Hz dla 8 MHz /1024
	{
		TC1_Wartosc = 0xE17B;
	}

	if (komenda == 'u')                  // Przyspieszenie x2
	{
		RS_temp = 0xFFFF - TC1_Wartosc;
		RS_temp = RS_temp / 2;
		TC1_Wartosc = 0xFFFF - RS_temp;

	}

	if (komenda == 'd')                  // Spowolnienie /2
	{
		RS_temp = 0xFFFF - TC1_Wartosc;
		RS_temp = RS_temp * 2;
		TC1_Wartosc = 0xFFFF - RS_temp;

	}

}	

int main (void)
{

// Ustawienia TC0
  TIMSK = _BV(TOIE0);     	             	   // wlacz obsluge przerwan T/C0
  TCCR0 = _BV(CS02) | _BV(CS01) | _BV(CS00);   //External clock source on T0 pin. Clock on rising edge


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
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<TXCIE)|(1<<RXCIE);

// Ustawienia portu B       
	DDRB = 0x01;					// 1 LSB portu B jako wyjscia

  sei();       				         // wlacz obsluge przerwan

	while(1){

		// ADC0 input selected
	ADCSRA = (1<<ADIF);
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
	ADCSRA = (0<<ADIF);        						     // Wyzerowanie flagi skonczonego przerwania	
	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);  // ADC Start conversion 125kHz dla F_OSC = 8MHz
	while(!(ADCSRA & (1<<ADIF)));
	ADC_port_1 = ADCH;

		// ADC1 input selected
	ADCSRA = (1<<ADIF);
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(1<<MUX0);
	ADCSRA = (0<<ADIF);        						     // Wyzerowanie flagi skonczonego przerwania	
	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);  // ADC Start conversion 125kHz dla F_OSC = 8MHz
	while(!(ADCSRA & (1<<ADIF)));
	ADC_port_2 = ADCH;

		// ADC2 input selected
	ADCSRA = (1<<ADIF);
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(0<<MUX0);
	ADCSRA = (0<<ADIF);        						     // Wyzerowanie flagi skonczonego przerwania	
	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);  // ADC Start conversion 125kHz dla F_OSC = 8MHz
	while(!(ADCSRA & (1<<ADIF)));
	ADC_port_3 = ADCH;

		// ADC3 input selected
	ADCSRA = (1<<ADIF);
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(1<<MUX0);
	ADCSRA = (0<<ADIF);        						     // Wyzerowanie flagi skonczonego przerwania	
	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);  // ADC Start conversion 125kHz dla F_OSC = 8MHz
	while(!(ADCSRA & (1<<ADIF)));
	ADC_port_4 = ADCH;

		// ADC4 input selected
	ADCSRA = (1<<ADIF);
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(0<<MUX0);
	ADCSRA = (0<<ADIF);        						     // Wyzerowanie flagi skonczonego przerwania	
	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);  // ADC Start conversion 125kHz dla F_OSC = 8MHz
	while(!(ADCSRA & (1<<ADIF)));
	ADC_port_5 = ADCH;

		// ADC5 input selected
	ADCSRA = (1<<ADIF);
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0);
	ADCSRA = (0<<ADIF);        						     // Wyzerowanie flagi skonczonego przerwania	
	ADCSRA = _BV(ADSC)|_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);  // ADC Start conversion 125kHz dla F_OSC = 8MHz
	while(!(ADCSRA & (1<<ADIF)));
	ADC_port_6 = ADCH;

	}
	return(1);
}
