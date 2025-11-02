/*

A sample program to test the Extrenal EEPROM interfacing
routines. The program fills the whole EEPROM with 7 and 
then reads the whole EEPROM memory to see if all of them
contains 7.

This helps in quick testing of you hardware /software before
using these routines in your own programs.

The target for this program is ATmega8, ATmega16, or ATmega32
running at 16MHz. If you use slower crystal the program will
simply run slow but without any problems.

This program also makes use of eXtreme Electronics 16x2 LCD
Interfacing routines. See the related page for more info

Author : Avinash Gupta
Date : 16 March 2009
Mail : me@avinashgupta.com
Web : www.eXtremeElectronics.co.in

NOTE: IF YOU USE THIS LIBRARY IN YOUR PROGRAMS AND FINDS 
IT USEFUL PLEASE MENTION US IN CREDIT AND RECOMEND OTHERS.


*/

#include <avr/io.h>
#include "util/delay.h"

#include "24c64.h"
#include "lcd.h"


/**********************************

A simple delay routine to wait 
between messages given to user
so that he/she gets time to read them

***********************************/
void Wait()
{
	uint8_t i;

	for(i=0;i<100;i++)
		_delay_loop_2(0);
}

void main()
{
	//Varriables
	uint8_t failed;
	unsigned int address;

	//Initialize LCD
	LCDInit(LS_BLINK);

	//Init EEPROM
	EEOpen();

	_delay_loop_2(0);
	
	LCDClear();
	LCDWriteString("External EEPROM");
	LCDWriteStringXY(0,1,"Test");

	Wait();

	LCDClear();
	LCDWriteString("Writting...");

	//Fill whole eeprom 8KB (8192 bytes)
	//with number 7
	failed=0;
	for(address=0;address<8192;address++)
	{
		if(EEWriteByte(address,7)==0)
		{
			//Write Failed
			LCDClear();
			LCDWriteString("Write Failed !");
			LCDWriteStringXY(0,1,"Addess = ");
			LCDWriteInt(address,4);
			failed=1;
			Wait();
			break;
		}
	}

	LCDClear();

	if(!failed)
		LCDWriteString("Written 8192bytes");

	Wait();

	LCDClear();
	LCDWriteString("Verifying ...");

	//Check if every location in EEPROM has 
	//number 7 stored
	failed=0;
	for(address=0;address<8192;address++)
	{
		if(EEReadByte(address)!=7)
		{
			//Failed !
			LCDClear();
			LCDWriteString("Verify Failed");
			LCDWriteStringXY(0,1,"Addess = ");
			LCDWriteInt(address,4);
			failed=1;
			Wait();
			break;
		}
	}

	if(!failed)
	{
		//We have Done it !!!
		LCDClear();
		LCDWriteString("Write Success !");
	}
	
	while(1);

}


