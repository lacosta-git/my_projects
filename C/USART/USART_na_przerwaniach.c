#include <C:/WinAVR/avr/include/avr/io.h>
#include <C:/WinAVR/avr/include/avr/interrupt.h>
 
#ifndef F_CPU
//define cpu clock speed if not defined
#define F_CPU 1000000
#endif
//set desired baud rate
#define BAUDRATE 9600
//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1)

	uint8_t Temp;

void USART_Init()
{
	//Set baud rate
	UBRRL=UBRRVAL;		//low byte
	UBRRH=(UBRRVAL>>8);	//high byte

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);	
	//Enable Transmitter and Receiver and Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<TXCIE)|(1<<RXCIE);

}

ISR(USART_RXC_vect)
{
		Temp = UDR;  // Odczyt danej portu rs


		// Kod programu
		if (0x31 == Temp)  // 1
		{
			PORTB=0x01;
		}
		if (0x32 == Temp)  // 2
		{
			PORTD=0x10;
		}
		if (0x33 == Temp)  // 3
		{
			PORTB=0x00;
			PORTD=0x00;
		}
}


ISR(USART_TXC_vect)
{
//  Do wysylania 
// UDR = 0x01;
}

int main(void)
{
	DDRB = 0xFF;
	DDRD = 0xF0;

	USART_Init();         // Inicjalizacja RS

	//enable global interrupts
	sei();

	while(1)
	{



	}
	return 1;
}
