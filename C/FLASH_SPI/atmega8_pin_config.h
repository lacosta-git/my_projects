/************************************************
 copyright by gumistor.pl 2010                            
Title:             Konfiguracja pinow dla ATmega8 + SPI FLASH AT45DBxxxx
Current version:   v1.0 
processor          ATMEGA8 
Clock:             8MHz Crystal 
Author:            gumistor.pl
Contact:           gumistor.pl 
Date:              2010-03-23
                 
                Connections 
                 
      ATMEGA8 SIDE      AT45DBxxx SIDE   
        SCK--------------->SCK       
       MISO<---------------SO 
       MOSI--------------->SI 
        nSS--------------->nCS          
        
       PD4~PD7: LED0~LED3 
 
*************************************************/
#include <C:/WinAVR-20090313/avr/include/avr/io.h>
 
#define DDR_IN          0 
#define DDR_OUT         1 
 
#define PORT_nCS        PORTB 
#define PIN_nCS         PINB  
#define DDR_nCS         DDRB  
 
#define PORT_MOSI       PORTB 
#define PIN_MOSI        PINB    
#define DDR_MOSI        DDRB 
 
#define PORT_SCK        PORTB 
#define PIN_SCK         PINB   
#define DDR_SCK         DDRB 
 
#define PORT_MISO       PORTB 
#define PIN_MISO        PINB 
#define DDR_MISO        DDRB  
 
#define PORT_LED        PORTD 
#define DDR_LED         DDRD 
  
#define PB7             7// 
#define PB6             6//  | 
#define SPI_SCK         5//  | 
#define SPI_MISO        4//  |SPI_PORT 
#define SPI_MOSI        3//  | 
#define AT45DB_nCS      2//  | 
#define PB1             1//  | 
#define PB0             0//  
 
#define AT45DB_nCS_OUTPUT()    DDR_nCS |= (1<<AT45DB_nCS) 
#define HI_AT45DB_nCS()    	   PORT_nCS|= (1<<AT45DB_nCS) 
#define LOW_AT45DB_nCS()       PORT_nCS&=~(1<<AT45DB_nCS) 
 
#define SPI_MOSI_OUTPUT() 	   DDR_MOSI |= (1<<SPI_MOSI) 
#define HI_SPI_MOSI()          PORT_MOSI|= (1<<SPI_MOSI) 
#define LOW_SPI_MOSI()         PORT_MOSI&=~(1<<SPI_MOSI)
 
#define SPI_MISO_INPUT()       DDR_MISO&= ~(1<<SPI_MISO)
#define HI_SPI_MISO()          PIN_MISO&=(1<<SPI_MISO) 
 
#define SPI_SCK_OUTPUT()       DDR_SCK |= (1<<SPI_SCK)
#define HI_SPI_SCK()           PORT_SCK|= (1<<SPI_SCK)
#define LOW_SPI_SCK()          PORT_SCK&=~(1<<SPI_SCK)
 
#define LED_OUTPUT()    DDR_LED |=0xF0 
#define LED7_OFF()      PORT_LED&=~(1<<7) 
#define LED7_ON()       PORT_LED|= (1<<7)  
#define LED7_TRG()      PORT_LED^= (1<<7) 
 
#define LED6_OFF()      PORT_LED&=~(1<<6) 
#define LED6_ON()       PORT_LED|= (1<<6) 
#define LED6_TRG()      PORT_LED^= (1<<6) 
 
#define LED5_OFF()      PORT_LED&=~(1<<5) 
#define LED5_ON()       PORT_LED|= (1<<5)  
#define LED5_TRG()      PORT_LED^= (1<<5) 
 
#define LED4_OFF()      PORT_LED&=~(1<<4) 
#define LED4_ON()       PORT_LED|= (1<<4) 
#define LED4_TRG()      PORT_LED^= (1<<4) 

