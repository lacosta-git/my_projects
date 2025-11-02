//Wysylanie wartosci z FLASH na SPI

#include <C:/WinAVR-20090313/avr/include/avr/io.h>
#include <C:/WinAVR-20090313/avr/include/avr/wdt.h>
#include <C:/WinAVR-20090313/avr/include/avr/interrupt.h>
#include <D:/Projekty/AVRStudio/EcoDriver/ATmega8_PIN_Config.h>

// Wartosc TC2
#define TC2_Wartosc 0x9B       // 10 msec dla preskalera clk/8
#define Divade_value      1    // Speed of dimming 1 = max speed   255 = min speed
#define LED_Low_Limit     0    // min 0
#define LED_High_Limit  100    // max 250

#ifndef F_CPU
//define cpu clock speed if not defined
#define F_CPU 8000000
#endif
//set desired baud rate
#define BAUDRATE 4800
//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1) 

// 
		 uint8_t Divider;
		 uint8_t Direction;

volatile uint16_t Counter_T2_interrupts;
volatile uint8_t Engine_Speed;
volatile uint8_t Dimming_pulse;
volatile uint8_t timer;                  // licznik 10ms od 0 do 100
volatile uint8_t ON_Time_LED_Y3;         // czas wlonczenia LED w procentach od 0 do 100
volatile uint8_t ON_Time_LED_Y2;
volatile uint8_t ON_Time_LED_Y1;
volatile uint8_t ON_Time_LED_G3;
volatile uint8_t ON_Time_LED_G2;
volatile uint8_t ON_Time_LED_G1;
volatile uint8_t ON_Time_LED_R3;
volatile uint8_t ON_Time_LED_R2;
volatile uint8_t ON_Time_LED_R1;

volatile uint8_t LED_Y3_ON_Limit;                // 
volatile uint8_t LED_Y3_OFF_Limit;                //
volatile uint8_t LED_Y2_ON_Limit;                //
volatile uint8_t LED_Y2_OFF_Limit;                //
volatile uint8_t LED_Y1_ON_Limit;                //
volatile uint8_t LED_Y1_OFF_Limit;                //
volatile uint8_t LED_G3_ON_Limit;                // 
volatile uint8_t LED_G3_OFF_Limit;                //
volatile uint8_t LED_G2_ON_Limit;                //
volatile uint8_t LED_G2_OFF_Limit;                //
volatile uint8_t LED_G1_ON_Limit;                //
volatile uint8_t LED_G1_OFF_Limit;                //
volatile uint8_t LED_R3_ON_Limit;                // 
volatile uint8_t LED_R3_OFF_Limit;                //
volatile uint8_t LED_R2_ON_Limit;                //
volatile uint8_t LED_R2_OFF_Limit;                //
volatile uint8_t LED_R1_ON_Limit;                //
volatile uint8_t LED_R1_OFF_Limit;                //



// Funkcje
void reboot() {
  wdt_disable();  
  wdt_enable(WDTO_15MS);
  while (1) {}
}

// Write EEPROM
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEWE))
;
/* Set up address and data registers */
EEAR = uiAddress;
EEDR = ucData;
/* Write logical one to EEMWE */
EECR |= (1<<EEMWE);
/* Start eeprom write by setting EEWE */
EECR |= (1<<EEWE);
}


//Read EEPROM
unsigned char EEPROM_read(unsigned int uiAddress)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEWE))
;
/* Set up address register */
EEAR = uiAddress;
/* Start eeprom read by writing EERE */
EECR |= (1<<EERE);
/* Return data from data register */
return EEDR;
}



// Przerwania
// TC2 Przerwanie
SIGNAL (SIG_OVERFLOW2)
{

	// Przeladowanie timera T2
	TCNT2 = TC2_Wartosc;        		// przeladuj timer 2
	timer++;
	Counter_T2_interrupts++;
	if (timer == (LED_High_Limit+2))
		{
			timer = LED_Low_Limit;

			// Dimming pulse generator
			if (Divider >= Divade_value)
				{
					Dimming_pulse = 1;		// Must be cleared while executed
					Divider = 0;
				}
			else
				{
					Divider++;
				}
		}
}

// RX
ISR(USART_RXC_vect)
{
char komenda;

	komenda = UDR;

	if (komenda == 'a')                  // 
	{		
		Engine_Speed = 0;
	}

	if (komenda == 'z')                  // 
	{
		Engine_Speed = 50;
	}

	if (komenda == 'r')                  // reboot
	{
		reboot();
	}

}

int main (void)
{

// Preskaler
// CS22 CS21 CS20
//   0    0    0   -   No clock source. (Timer/Counter stopped)  
//   0    0    1   -   clk/1 (No prescaling)
//   0    1    0   -   clk/8 
//   0    1    1   -   clk/64 
//   1    0    0   -   clk/256 
//   1    0    1   -   clk/1024 
//  TCCR2 = _BV(CS22)|_BV(CS21)|_BV(CS20);


// Ustawienia TC2
  	TIMSK = _BV(TOIE2);     	         // wlacz obsluge przerwan T/C2
	TCNT2 = TC2_Wartosc;        		 // przeladuj timer 2
	TCCR2 = _BV(CS21);       // clk/8 tryb czasomierza T/C2


// Ustawienia RS
	//Set baud rate
	UBRRL=UBRRVAL;		//low byte
	UBRRH=(UBRRVAL>>8);	//high byte

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);	
	//Enable Receiver and Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<RXCIE);



 	LED_OUTPUT_Yelow();
	LED_OUTPUT_Green();
	LED_OUTPUT_Red();

	LED_Y3_OFF();
	LED_Y2_OFF();
	LED_Y1_OFF();
	LED_G3_OFF();
	LED_G2_OFF();
	LED_G1_OFF();
	LED_R3_OFF();
	LED_R2_OFF();
	LED_R1_OFF();

// EEPROM values write - DEVELOPMNET
/*	EEPROM_write (0x0000, 1 );       // Zapis LED Y3 OFF	
	EEPROM_write (0x0001, 10);       // Zapis LED Y3 ON
	EEPROM_write (0x0002, 15);       // Zapis LED Y2 OFF	
	EEPROM_write (0x0003, 20);       // Zapis LED Y2 ON
	EEPROM_write (0x0004, 25);       // Zapis LED Y1 OFF	
	EEPROM_write (0x0005, 30);       // Zapis LED Y1 ON

	EEPROM_write (0x0006, 35);       // Zapis LED G3 OFF	
	EEPROM_write (0x0007, 40);       // Zapis LED G3 ON
	EEPROM_write (0x0008, 45);       // Zapis LED G2 OFF	
	EEPROM_write (0x0009, 50);       // Zapis LED G2 ON
	EEPROM_write (0x000A, 55);       // Zapis LED G1 OFF	
	EEPROM_write (0x000B, 60);       // Zapis LED G1 ON

	EEPROM_write (0x000C, 65);       // Zapis LED R3 OFF	
	EEPROM_write (0x000D, 70);       // Zapis LED R3 ON
	EEPROM_write (0x000E, 75);       // Zapis LED R2 OFF	
	EEPROM_write (0x000F, 80);       // Zapis LED R2 ON
	EEPROM_write (0x0010, 85);       // Zapis LED R1 OFF	
	EEPROM_write (0x0011, 90);       // Zapis LED R1 ON
*/
// EEPROM values readout

	LED_Y3_ON_Limit  = EEPROM_read (0x0000);
	LED_Y3_OFF_Limit = EEPROM_read (0x0001);
	LED_Y2_ON_Limit  = EEPROM_read (0x0002);
	LED_Y2_OFF_Limit = EEPROM_read (0x0003);
	LED_Y1_ON_Limit  = EEPROM_read (0x0004);
	LED_Y1_OFF_Limit = EEPROM_read (0x0005);

	LED_G3_ON_Limit  = EEPROM_read (0x0006);
	LED_G3_OFF_Limit = EEPROM_read (0x0007);
	LED_G2_ON_Limit  = EEPROM_read (0x0008);
	LED_G2_OFF_Limit = EEPROM_read (0x0009);
	LED_G1_ON_Limit  = EEPROM_read (0x000A);
	LED_G1_OFF_Limit = EEPROM_read (0x000B);

	LED_R3_ON_Limit  = EEPROM_read (0x000C);
	LED_R3_OFF_Limit = EEPROM_read (0x000D);
	LED_R2_ON_Limit  = EEPROM_read (0x000E);
	LED_R2_OFF_Limit = EEPROM_read (0x000F);
	LED_R1_ON_Limit  = EEPROM_read (0x0010);
	LED_R1_OFF_Limit = EEPROM_read (0x0011);

	Direction = 1;

	sei(); 
	while(1){

			//ON OFF conditions check	
			if (Dimming_pulse)           //Dimming step on timer = low value
				{
					Dimming_pulse = 0;

					if ((Engine_Speed > LED_Y3_ON_Limit) && (Engine_Speed < LED_Y1_ON_Limit))
						{
							if (ON_Time_LED_Y3 < LED_High_Limit)
								{
									ON_Time_LED_Y3++;             // Dimming UP
								}
						}
					else
						{
							if (ON_Time_LED_Y3 > LED_Low_Limit)
								{
									ON_Time_LED_Y3--;             // Dimming Down
								}
						}
					if ((Engine_Speed > LED_Y2_ON_Limit) && (Engine_Speed < LED_G3_ON_Limit))
						{
							if (ON_Time_LED_Y2 < LED_High_Limit)
								{
									ON_Time_LED_Y2++;
								}
						}
					else
						{
							if (ON_Time_LED_Y2 > LED_Low_Limit)
								{
									ON_Time_LED_Y2--;
								}
						}
					if ((Engine_Speed > LED_Y1_ON_Limit) && (Engine_Speed < LED_G2_ON_Limit))
						{
							if (ON_Time_LED_Y1 < LED_High_Limit)
								{
									ON_Time_LED_Y1++;
								}
						}
					else
						{
							if (ON_Time_LED_Y1 > LED_Low_Limit)
								{
									ON_Time_LED_Y1--;
								}
						}


					if ((Engine_Speed > LED_G3_ON_Limit) && (Engine_Speed < LED_G1_ON_Limit))
						{
							if (ON_Time_LED_G3 < LED_High_Limit)
								{
									ON_Time_LED_G3++;             // Dimming UP
								}
						}
					else
						{
							if (ON_Time_LED_G3 > LED_Low_Limit)
								{
									ON_Time_LED_G3--;             // Dimming Down
								}
						}
					if ((Engine_Speed > LED_G2_ON_Limit) && (Engine_Speed < LED_R3_ON_Limit))
						{
							if (ON_Time_LED_G2 < LED_High_Limit)
								{
									ON_Time_LED_G2++;
								}
						}
					else
						{
							if (ON_Time_LED_G2 > LED_Low_Limit)
								{
									ON_Time_LED_G2--;
								}
						}
					if ((Engine_Speed > LED_G1_ON_Limit) && (Engine_Speed < LED_R2_ON_Limit))
						{
							if (ON_Time_LED_G1 < LED_High_Limit)
								{
									ON_Time_LED_G1++;
								}
						}
					else
						{
							if (ON_Time_LED_G1 > LED_Low_Limit)
								{
									ON_Time_LED_G1--;
								}
						}

					if ((Engine_Speed > LED_R3_ON_Limit) && (Engine_Speed < LED_R1_ON_Limit))
						{
							if (ON_Time_LED_R3 < LED_High_Limit)
								{
									ON_Time_LED_R3++;             // Dimming UP
								}
						}
					else
						{
							if (ON_Time_LED_R3 > LED_Low_Limit)
								{
									ON_Time_LED_R3--;             // Dimming Down
								}
						}
					if (Engine_Speed > LED_R2_ON_Limit) 
						{
							if (ON_Time_LED_R2 < LED_High_Limit)
								{
									ON_Time_LED_R2++;
								}
						}
					else
						{
							if (ON_Time_LED_R2 > LED_Low_Limit)
								{
									ON_Time_LED_R2--;
								}
						}
					if (Engine_Speed > LED_R1_ON_Limit)
						{
							if (ON_Time_LED_R1 < LED_High_Limit)
								{
									ON_Time_LED_R1++;
								}
						}
					else
						{
							if (ON_Time_LED_R1 > LED_Low_Limit)
								{
									ON_Time_LED_R1--;
								}
						}
						
				}

			// LEDs
			//Y3
			if (timer >= ON_Time_LED_Y3)
				{
					LED_Y3_OFF();
				}
			else
				{
					LED_Y3_ON();
				}
			//Y2
			if (timer >= ON_Time_LED_Y2)
				{
					LED_Y2_OFF();
				}
			else
				{
					LED_Y2_ON();
				}
			//Y1
			if (timer >= ON_Time_LED_Y1)
				{
					LED_Y1_OFF();
				}
			else
				{
					LED_Y1_ON();
				}
			//G3
			if (timer >= ON_Time_LED_G3)
				{
					LED_G3_OFF();
				}
			else
				{
					LED_G3_ON();
				}
			//G2
			if (timer >= ON_Time_LED_G2)
				{
					LED_G2_OFF();
				}
			else
				{
					LED_G2_ON();
				}
			//G1
			if (timer >= ON_Time_LED_G1)
				{
					LED_G1_OFF();
				}
			else
				{
					LED_G1_ON();
				}
			//R3
			if (timer >= ON_Time_LED_R3)
				{
					LED_R3_OFF();
				}
			else
				{
					LED_R3_ON();
				}
			//R2
			if (timer >= ON_Time_LED_R2)
				{
					LED_R2_OFF();
				}
			else
				{
					LED_R2_ON();
				}
			//R1
			if (timer >= ON_Time_LED_R1)
				{
					LED_R1_OFF();
				}
			else
				{
					LED_R1_ON();
				}

// KONEC PROGRAMU
	// Linijka 
		if (Counter_T2_interrupts > 1000)
			{
				if (Direction == 1)
					{
						Engine_Speed++;
						Counter_T2_interrupts = 0;
						if (Engine_Speed > 100)
							{
								Direction = 0;
							}
					}
				else
					{
						Engine_Speed--;
						Counter_T2_interrupts = 0;
						if (Engine_Speed < 1)
							{
								Direction = 1;
							}
					}
			}

}
return(1);
}

