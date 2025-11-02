/************************************************
 copyright by gumistor.pl 2010                            
Title:             Konfiguracja pinow dla ATmega8 EcoDriver
Current version:   v1.0 
processor          ATMEGA8 
Clock:             8MHz Crystal - internal
Author:            gumistor.pl
Contact:           gumistor.pl 
Date:              2010-03-23
                 
                Connections 
                 
      ATMEGA8 SIDE          LEDs  
        PC5 ---------------> LED Yelow 3      
        PC4 ---------------> LED Yelow 2
		PC3 ---------------> LED Yelow 1

		PC2 ---------------> LED Green 3
		PC1 ---------------> LED Green 2
		PC0 ---------------> LED Green 1

		PB3 ---------------> LED Red 3
		PB2 ---------------> LED Red 2
		PB1 ---------------> LED Red 1

		T0 <--------------  Engine speed - pulses
 
*************************************************/
#include <C:/WinAVR-20090313/avr/include/avr/io.h>
 
#define DDR_IN          0 
#define DDR_OUT         1 
 
#define PORT_T0         PORTD 
#define DDR_T0          DDRD  
    
#define PIN_T0             4//  PIN PD4
#define PIN_T0_INPUT()         DDR_T0&= ~(1<<PIN_T0)

#define PORT_T1         PORTD 
#define DDR_T1          DDRD  
    
#define PIN_T1             5//  PIN PD5
#define PIN_T1_INPUT()         DDR_T1&= ~(1<<PIN_T1)

#define PORT_LED_Yelow        PORTC 
#define DDR_LED_Yelow         DDRC 
#define LED_OUTPUT_Yelow()    DDR_LED_Yelow |=0b00111000 
#define LED_Y3_ON()        PORT_LED_Yelow&=~(1<<5)
#define LED_Y3_OFF()       PORT_LED_Yelow|= (1<<5)
#define LED_Y3_TRG()       PORT_LED_Yelow^= (1<<5)

#define LED_Y2_ON()        PORT_LED_Yelow&=~(1<<4)
#define LED_Y2_OFF()       PORT_LED_Yelow|= (1<<4)
#define LED_Y2_TRG()       PORT_LED_Yelow^= (1<<4)

#define LED_Y1_ON()        PORT_LED_Yelow&=~(1<<3)
#define LED_Y1_OFF()       PORT_LED_Yelow|= (1<<3)
#define LED_Y1_TRG()       PORT_LED_Yelow^= (1<<3)


#define PORT_LED_Green        PORTC 
#define DDR_LED_Green         DDRC 
#define LED_OUTPUT_Green()    DDR_LED_Green |=0b00000111
#define LED_G3_ON()        PORT_LED_Green&=~(1<<2)
#define LED_G3_OFF()       PORT_LED_Green|= (1<<2)
#define LED_G3_TRG()       PORT_LED_Green^= (1<<2)

#define LED_G2_ON()        PORT_LED_Green&=~(1<<1)
#define LED_G2_OFF()       PORT_LED_Green|= (1<<1)
#define LED_G2_TRG()       PORT_LED_Green^= (1<<1)

#define LED_G1_ON()        PORT_LED_Green&=~(1<<0)
#define LED_G1_OFF()       PORT_LED_Green|= (1<<0)
#define LED_G1_TRG()       PORT_LED_Green^= (1<<0)

#define PORT_LED_Red          PORTB 
#define DDR_LED_Red           DDRB 
#define LED_OUTPUT_Red()      DDR_LED_Red |=0b00001110
#define LED_R3_ON()        PORT_LED_Red&=~(1<<3)
#define LED_R3_OFF()       PORT_LED_Red|= (1<<3)
#define LED_R3_TRG()       PORT_LED_Red^= (1<<3)
 
#define LED_R2_ON()        PORT_LED_Red&=~(1<<2)
#define LED_R2_OFF()       PORT_LED_Red|= (1<<2)
#define LED_R2_TRG()       PORT_LED_Red^= (1<<2)

#define LED_R1_ON()        PORT_LED_Red&=~(1<<1)
#define LED_R1_OFF()       PORT_LED_Red|= (1<<1)
#define LED_R1_TRG()       PORT_LED_Red^= (1<<1)
