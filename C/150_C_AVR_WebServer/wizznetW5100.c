/*
 * wizznetW5100.c
 *
 */ 

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "wizznetW5100.h"
#include "SPI.h"
#include "pinAsigment.h"

const char Kuchnia[] PROGMEM = "Kuchnia";
const char SalonJadalnia[] PROGMEM = "SalonJadalnia";
const char SalonTaras[] PROGMEM = "SalonTaras";
const char Pokoj1Julek[] PROGMEM = "Pokoj1Julek";
const char Pokoj2Jas[] PROGMEM = "Pokoj2Jas";
const char LazienkaMala[] PROGMEM = "LazienkaMala";
const char LazienkaDuza[] PROGMEM = "LazienkaDuza";
const char Pokoj3Sypialnia[] PROGMEM = "Pokoj3Sypialnia";

const char* const nazwaPomieszczenia[] = {Kuchnia, SalonJadalnia, SalonTaras, Pokoj1Julek,
										  Pokoj2Jas, LazienkaMala, LazienkaDuza, Pokoj3Sypialnia};

PARAMETRY parametry[9];
PARAMETRY_E EEMEM parametry_E[9];
uint8_t EEMEM eppromInitialized;

volatile uint16_t offaddr, i;
volatile uint8_t socketStatus;
uint8_t buf[MAX_BUF];
volatile int GETindexPointer, wsKorekPtr, podZadPtr, pomNamePtr;
volatile int tmpCWU, tmpCO, tmpOutdor;

void initializeParametry(){
	
	if (eeprom_read_byte(&eppromInitialized) != EEPROM_INITIALIZED)
	{
		// Initial values
		// 32.6 = 326    wszystko x10 (dec)
		eeprom_update_word(&parametry_E[KUCHNIA].podlogaZadana,241);
		eeprom_update_word(&parametry_E[KUCHNIA].podlogaWsKorekcji,2);  // => 0.2
		
		eeprom_update_word(&parametry_E[SALON_JADALNIA].podlogaZadana,222);
		eeprom_update_word(&parametry_E[SALON_JADALNIA].podlogaWsKorekcji,0);  // => 0.2
		
		eeprom_update_word(&parametry_E[SALON_TARAS].podlogaZadana,230);
		eeprom_update_word(&parametry_E[SALON_TARAS].podlogaWsKorekcji,1);  // => 0.2
		
		eeprom_update_word(&parametry_E[POKOJ1_JULEK].podlogaZadana,199);
		eeprom_update_word(&parametry_E[POKOJ1_JULEK].podlogaWsKorekcji,0);  // => 0.2
		
		eeprom_update_word(&parametry_E[POKOJ2_JAS].podlogaZadana,200);
		eeprom_update_word(&parametry_E[POKOJ2_JAS].podlogaWsKorekcji,1);  // => 0.2
		
		eeprom_update_word(&parametry_E[LAZIENKA_MALA].podlogaZadana,244);
		eeprom_update_word(&parametry_E[LAZIENKA_MALA].podlogaWsKorekcji,0);  // => 0.2
		
		eeprom_update_word(&parametry_E[LAZIENKA_DUZA].podlogaZadana,244);
		eeprom_update_word(&parametry_E[LAZIENKA_DUZA].podlogaWsKorekcji,2);  // => 0.2
		
		eeprom_update_word(&parametry_E[POKOJ3_SYPIALNIA].podlogaZadana,190);
		eeprom_update_word(&parametry_E[POKOJ3_SYPIALNIA].podlogaWsKorekcji,0);  // => 0.2
		
		eeprom_update_byte(&eppromInitialized, EEPROM_INITIALIZED);
	}
	// Initial values
	// 32.6 = 326    wszystko x10 (dec)
	tmpCWU = 367;
	tmpCO = 345;
	tmpOutdor = -54;

	parametry[KUCHNIA].podlogaZadana = eeprom_read_word(&parametry_E[KUCHNIA].podlogaZadana);
	parametry[KUCHNIA].podlogaAktualna = 194;
	parametry[KUCHNIA].podlogaWsKorekcji = eeprom_read_word(&parametry_E[KUCHNIA].podlogaWsKorekcji);
	parametry[KUCHNIA].podlogaKorekta = 2;
	parametry[KUCHNIA].powietrzeAktualna = 210;
	parametry[KUCHNIA].otwarcieZaworu = 450;

	parametry[SALON_JADALNIA].podlogaZadana = eeprom_read_word(&parametry_E[SALON_JADALNIA].podlogaZadana);
	parametry[SALON_JADALNIA].podlogaAktualna = 245;
	parametry[SALON_JADALNIA].podlogaWsKorekcji = eeprom_read_word(&parametry_E[SALON_JADALNIA].podlogaWsKorekcji);
	parametry[SALON_JADALNIA].podlogaKorekta = 0;
	parametry[SALON_JADALNIA].powietrzeAktualna = 196;
	parametry[SALON_JADALNIA].otwarcieZaworu = 526;

	parametry[SALON_TARAS].podlogaZadana = eeprom_read_word(&parametry_E[SALON_TARAS].podlogaZadana);
	parametry[SALON_TARAS].podlogaAktualna = 250;
	parametry[SALON_TARAS].podlogaWsKorekcji = eeprom_read_word(&parametry_E[SALON_TARAS].podlogaWsKorekcji);
	parametry[SALON_TARAS].podlogaKorekta = 0;
	parametry[SALON_TARAS].powietrzeAktualna = 199;
	parametry[SALON_TARAS].otwarcieZaworu = 581;

	parametry[POKOJ1_JULEK].podlogaZadana = eeprom_read_word(&parametry_E[POKOJ1_JULEK].podlogaZadana);
	parametry[POKOJ1_JULEK].podlogaAktualna = 230;
	parametry[POKOJ1_JULEK].podlogaWsKorekcji = eeprom_read_word(&parametry_E[POKOJ1_JULEK].podlogaWsKorekcji);
	parametry[POKOJ1_JULEK].podlogaKorekta = 0;
	parametry[POKOJ1_JULEK].powietrzeAktualna = 183;
	parametry[POKOJ1_JULEK].otwarcieZaworu = 444;

	parametry[POKOJ2_JAS].podlogaZadana = eeprom_read_word(&parametry_E[POKOJ2_JAS].podlogaZadana);
	parametry[POKOJ2_JAS].podlogaAktualna = 230;
	parametry[POKOJ2_JAS].podlogaWsKorekcji = eeprom_read_word(&parametry_E[POKOJ2_JAS].podlogaWsKorekcji);
	parametry[POKOJ2_JAS].podlogaKorekta = 0;
	parametry[POKOJ2_JAS].powietrzeAktualna = 186;
	parametry[POKOJ2_JAS].otwarcieZaworu = 449;

	parametry[LAZIENKA_MALA].podlogaZadana = eeprom_read_word(&parametry_E[LAZIENKA_MALA].podlogaZadana);
	parametry[LAZIENKA_MALA].podlogaAktualna = 260;
	parametry[LAZIENKA_MALA].podlogaWsKorekcji = eeprom_read_word(&parametry_E[LAZIENKA_MALA].podlogaWsKorekcji);
	parametry[LAZIENKA_MALA].podlogaKorekta = 0;
	parametry[LAZIENKA_MALA].powietrzeAktualna = 213;
	parametry[LAZIENKA_MALA].otwarcieZaworu = 601;

	parametry[LAZIENKA_DUZA].podlogaZadana = eeprom_read_word(&parametry_E[LAZIENKA_DUZA].podlogaZadana);
	parametry[LAZIENKA_DUZA].podlogaAktualna = 272;
	parametry[LAZIENKA_DUZA].podlogaWsKorekcji = eeprom_read_word(&parametry_E[LAZIENKA_DUZA].podlogaWsKorekcji);
	parametry[LAZIENKA_DUZA].podlogaKorekta = 0;
	parametry[LAZIENKA_DUZA].powietrzeAktualna = 216;
	parametry[LAZIENKA_DUZA].otwarcieZaworu = 623;
	
	parametry[POKOJ3_SYPIALNIA].podlogaZadana = eeprom_read_word(&parametry_E[POKOJ3_SYPIALNIA].podlogaZadana);
	parametry[POKOJ3_SYPIALNIA].podlogaAktualna = 232;
	parametry[POKOJ3_SYPIALNIA].podlogaWsKorekcji = eeprom_read_word(&parametry_E[POKOJ3_SYPIALNIA].podlogaWsKorekcji);
	parametry[POKOJ3_SYPIALNIA].podlogaKorekta = 0;
	parametry[POKOJ3_SYPIALNIA].powietrzeAktualna = 182;
	parametry[POKOJ3_SYPIALNIA].otwarcieZaworu = 323;
}

void W5100_Init(void)
{
	// Ethernet Setup
	unsigned char mac_addr[] = {0x00,0x16,0x36,0xDE,0x58,0xF6};
	unsigned char ip_addr[] = {192,168,0,201};
	unsigned char sub_mask[] = {255,255,255,0};
	unsigned char gtw_addr[] = {192,168,0,1};
	// Setting the Wiznet W5100 Mode Register: 0x0000
	wizzWrite(MR,0x80);            // MR = 0b10000000;
	// Setting the Wiznet W5100 Gateway Address (GAR): 0x0001 to 0x0004
	wizzWrite(GAR + 0,gtw_addr[0]);
	wizzWrite(GAR + 1,gtw_addr[1]);
	wizzWrite(GAR + 2,gtw_addr[2]);
	wizzWrite(GAR + 3,gtw_addr[3]);
	// Setting the Wiznet W5100 Source Address Register (SAR): 0x0009 to 0x000E
	wizzWrite(SAR + 0,mac_addr[0]);
	wizzWrite(SAR + 1,mac_addr[1]);
	wizzWrite(SAR + 2,mac_addr[2]);
	wizzWrite(SAR + 3,mac_addr[3]);
	wizzWrite(SAR + 4,mac_addr[4]);
	wizzWrite(SAR + 5,mac_addr[5]);
	// Setting the Wiznet W5100 Sub Mask Address (SUBR): 0x0005 to 0x0008
	wizzWrite(SUBR + 0,sub_mask[0]);
	wizzWrite(SUBR + 1,sub_mask[1]);
	wizzWrite(SUBR + 2,sub_mask[2]);
	wizzWrite(SUBR + 3,sub_mask[3]);
	// Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
	wizzWrite(SIPR + 0,ip_addr[0]);
	wizzWrite(SIPR + 1,ip_addr[1]);
	wizzWrite(SIPR + 2,ip_addr[2]);
	wizzWrite(SIPR + 3,ip_addr[3]);

	// Setting the Wiznet W5100 RX and TX Memory Size (2KB),
	wizzWrite(RMSR,NET_MEMALLOC);
	wizzWrite(TMSR,NET_MEMALLOC);
}



void wizzWrite(uint16_t addr,uint8_t data)
{
	// Activate the CS pin
	SPI_PORT &= ~(1<<SPI_CS);
	for (i=0; i<=50;i++);
	// Start Wiznet W5100 Write OpCode transmission
	SPIwriteByte(WIZNET_WRITE_OPCODE);
	// Start Wiznet W5100 Address
	SPIwriteWord(addr);
	// Start Data transmission
	SPIwriteByte(data);
	// CS pin is not active
	SPI_PORT |= (1<<SPI_CS);
}
unsigned char wizzRead(uint16_t addr)
{
	uint8_t data;
	// Activate the CS pin
	SPI_PORT &= ~(1<<SPI_CS);
	for (i=0; i<=50;i++);
	// Start Wiznet W5100 Read OpCode transmission
	SPIwriteByte(WIZNET_READ_OPCODE);
	// Start Wiznet W5100 Address
	SPIwriteWord(addr);
	// Read data
	data = SPIreadByte();
	// CS pin is not active
	SPI_PORT |= (1<<SPI_CS);
	return(data);
}

void disconnectSocket(uint8_t sock)
{
	if (sock != 0) return;

	// Send DISCONNNECT Command
	wizzWrite(S0_CR,CR_DISCON);
	// Waiting until the S0_CR is clear
	while(wizzRead(S0_CR));
}


void close(uint8_t sock)
{
	if (sock != 0) return;

	// Send Close Command
	wizzWrite(S0_CR,CR_CLOSE);
	// Waiting until the S0_CR is clear
	while(wizzRead(S0_CR));
}

void openSocket(uint8_t sock,uint8_t eth_protocol,uint16_t tcp_port)
{
	if (sock != 0) return;

	// Assigned Socket 0 Mode Register
	wizzWrite(S0_MR,eth_protocol);

	// Now open the Socket 0
	wizzWrite(S0_PORT,((tcp_port & 0xFF00) >> 8 ));
	wizzWrite(S0_PORT + 1,(tcp_port & 0x00FF));
	wizzWrite(S0_CR,CR_OPEN);                   // Open Socket
	
	// Wait for Opening Process
	while(wizzRead(S0_CR));
}
void goListening(uint8_t sock)
{
	if (sock != 0) return;
	
	// Send the LISTEN Command
	wizzWrite(S0_CR,CR_LISTEN);
	
	// Wait for Listening Process
	while(wizzRead(S0_CR));
}

void copyData(char *buf, uint16_t buflen, uint16_t offset)
{
	uint16_t realaddr;
	
while(buflen) {
		buflen--;
		// Calculate the real W5100 physical Tx Buffer Address
		realaddr = TXBUFADDR + offset;
		// Copy the application data to the W5100 Tx Buffer
		wizzWrite(realaddr,*buf);
		offset++;
		buf++;
	}
}

void copyToSendingBuffer(char *buf, uint16_t offaddr, uint16_t buflen)
{
	uint16_t offset, upperTXFreeMemorySize, remainingDataSize;
	
	offset = (offaddr & TX_BUF_MASK);
				
	// Calculate if all tx data will fit to tx buffer or there is a need for roll the start address
	if ((offset + buflen) > TX_BUF_MASK)
	{
		// There is a need for dividing TX buffer. Part of data will start from beginning of address
		upperTXFreeMemorySize = (TX_BUF_MASK + 1) - offset;
		// Sending data to TX buffer
		copyData(buf, upperTXFreeMemorySize, offset);
			
		// Update data source address
		buf += upperTXFreeMemorySize;
			
		// calculate remaining data to be sent to TX buffer
		remainingDataSize = buflen - upperTXFreeMemorySize;
		// Sending data to TX buffer
		copyData(buf, remainingDataSize, 0);
	}
	else
	{
		// all data will fit in TX buffer
		copyData(buf, buflen, offset);
	}

}

void sendWebPage(void)
{
	// Now Send the SEND command
	wizzWrite(S0_CR,CR_SEND);

	// Wait for Sending Process
	while(wizzRead(S0_CR));
}

int recv(uint8_t sock,uint8_t *buf,uint16_t buflen)
{
	uint16_t ptr,offaddr,realaddr;

	if (buflen <= 0 || sock != 0) return -1;

	// If the request size > MAX_BUF,just truncate it
	if (buflen > MAX_BUF)
		buflen=MAX_BUF - 2;
	// Read the Rx Read Pointer
	ptr = wizzRead(S0_RX_RD);
	offaddr = (((ptr & 0x00FF) << 8 ) + wizzRead(S0_RX_RD + 1));

	while(buflen) {
		buflen--;
		realaddr=RXBUFADDR + (offaddr & RX_BUF_MASK);
		*buf = wizzRead(realaddr);
		offaddr++;
		buf++;
	}
	*buf='\0';        // String terminated character

	// Increase the S0_RX_RD value, so it point to the next receive
	wizzWrite(S0_RX_RD,(offaddr & 0xFF00) >> 8 );
	wizzWrite(S0_RX_RD + 1,(offaddr & 0x00FF));

	// Now Send the RECV command
	wizzWrite(S0_CR,CR_RECV);
	
	// Wait for command execution
	while(wizzRead(S0_CR));
	
	return 1;
}

uint16_t recv_size(void)
{
	return ((wizzRead(S0_RX_RSR) & 0x00FF) << 8 ) + wizzRead(S0_RX_RSR + 1);
}

int strindex(char *s,char *t)
{
	uint16_t i,n;

	n=strlen(t);
	for(i=0;*(s+i); i++) {
		if (strncmp(s+i,t,n) == 0)
		return i;
	}
	return -1;
}
void sendDataToTXBuffer(char *buf)
{
	uint16_t buflen;
	
	buflen = strlen(buf);
	copyToSendingBuffer(buf, offaddr, buflen);
	offaddr += buflen;	// update offset address. It will be starting point for next data
	buf[0]='\0';  // Clear buffer
}

void intDiv10ToString(char *buf, int intVal)
{
	div_t divResult;
	char asciiBuf[6];

	divResult = div(intVal, 10);
	itoa(divResult.quot, asciiBuf, 10);
	strcat(buf, asciiBuf);
	strcat(buf, ".");
	divResult.rem = abs(divResult.rem);
	itoa(divResult.rem, asciiBuf, 10);
	strcat(buf, asciiBuf);
}
void generateWebPage(char *buf, uint8_t room){
	
	uint16_t ptr;
	uint8_t j, loops;
	
	// Read the Tx Write Pointer
	ptr = wizzRead(S0_TX_WR);
	offaddr = (((ptr & 0x00FF) << 8 ) + wizzRead(S0_TX_WR + 1));
	
	buf[0]='\0';  // Clear buffer
	
	// Create the HTTP Response	Header
	strcpy_P(buf,PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"));
	strcat_P(buf,PSTR("<html><title>Dom Zdziechow</title>\r\n"));
	strcat_P(buf,PSTR("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\r\n"));
	sendDataToTXBuffer(buf);
	
	strcat_P(buf,PSTR("<body bgcolor=\"#EEEEEE\" style=\"width:400px; margin: 0px auto;\">\r\n"));
	strcat_P(buf,PSTR("<p allign=\"left\"><font face=\"Arial\">\r\n"));
	strcat_P(buf,PSTR("<h1>Dom Zdziechow</h1>\r\n"));
	sendDataToTXBuffer(buf);
	
	strcat_P(buf,PSTR("<h2>Temperatury</h2></font></p><div allign=\"left\">\r\n"));
	strcat_P(buf,PSTR("<p><font face=\"Courier New\" color=\"Blue\">\r\n"));
	sendDataToTXBuffer(buf);
	
	strcat_P(buf,PSTR("Ciepla woda uzytkowa: <b>"));
	intDiv10ToString(buf, tmpCWU);
	strcat_P(buf,PSTR("</b><sup>o</sup>C<br />\r\nCentralne ogrzewanie: <b>"));
	intDiv10ToString(buf, tmpCO);
	strcat_P(buf,PSTR("</b><sup>o</sup>C<br />\r\n"));
	sendDataToTXBuffer(buf);
	
	strcat_P(buf,PSTR("Na zewnatrz: <b>"));
	intDiv10ToString(buf, tmpOutdor);
	strcat_P(buf,PSTR("</b><sup>o</sup>C<br /><br />\r\n<a href=\"../0.html\">KUCHNIA</a> "));
	strcat_P(buf,PSTR("<a href=\"../1.html\">SALON</a> "));
	strcat_P(buf,PSTR("<a href=\"../2.html\">SYPIALNIA</a> "));
	strcat_P(buf,PSTR("<a href=\"../3.html\">LAZIENKA-DUZA</a><br />"));
	sendDataToTXBuffer(buf);
	
	strcat_P(buf,PSTR("<a href=\"../4.html\">POKOJ-JULKA</a> "));
	strcat_P(buf,PSTR("<a href=\"../5.html\">POKOJ-JANKA</a> "));
	strcat_P(buf,PSTR("<a href=\"../6.html\">LAZIENKA-MALA</a><br />"));
	strcat_P(buf,PSTR("</font></p></div>\r\n"));
	sendDataToTXBuffer(buf);
	
	if (room == SALON_JADALNIA) loops = 2; else loops = 1;	
		for (j=1; j<=loops;j++)
		  {
			if (j==2) room = SALON_TARAS;
			// Next part of HTTP response
			strcat_P(buf,PSTR("<form method=\"GET\"><div allign=\"left\" ><p>\r\n"));
			strcat_P(buf,PSTR("<font face=\"Courier New\" color=\"Blue\">\r\n"));
			strcat_P(buf,PSTR("<fieldset><legend><h3>"));
			strcat_P(buf,nazwaPomieszczenia[room]);
			strcat_P(buf,PSTR("</h3></legend><br />\r\n"));
			sendDataToTXBuffer(buf);
			
			strcat_P(buf,PSTR("Podloga:..Zadana: <b><input type=\"text\" name=\"PodZad\" value=\""));
			intDiv10ToString(buf, parametry[room].podlogaZadana);
			strcat_P(buf,PSTR("\"\r\n maxlength=\"4\" size=\"4\" style=\"font-weight: bold\" /></b><sup>o</sup>C<br />\r\n"));
			strcat_P(buf,PSTR("Podloga:Aktualna: <b>"));
			intDiv10ToString(buf, parametry[room].podlogaAktualna);
			strcat_P(buf,PSTR("</b><sup>o</sup>C<br />\r\nPodloga:ws.korek: <input type=\"text\" name=\"WsKorek\" value=\""));
			intDiv10ToString(buf, parametry[room].podlogaWsKorekcji);
			sendDataToTXBuffer(buf);
			
			strcat_P(buf,PSTR("\"\r\n maxlength=\"4\" size=\"4\" style=\"font-weight: bold\" /><br />\r\nPodloga:.Korekta: "));
			intDiv10ToString(buf, parametry[room].podlogaKorekta);
			strcat_P(buf,PSTR("<sup>o</sup>C<br />\r\nPowietrze:Aktual: <b>"));
			intDiv10ToString(buf, parametry[room].powietrzeAktualna);
			sendDataToTXBuffer(buf);
			
			strcat_P(buf,PSTR("</b><sup>o</sup>C<br />\r\nOtwarcie zaworu : "));
			intDiv10ToString(buf, parametry[room].otwarcieZaworu);
			strcat_P(buf,PSTR(" procent<br />\r\n<input type=\"submit\" value=\"Zapisz\" name=\""));
			strcat_P(buf,nazwaPomieszczenia[room]);
			strcat_P(buf,PSTR("\" />\r\n</fieldset></font></p></div></form>\r\n"));
			sendDataToTXBuffer(buf);	  
		  }
			
	strcat_P(buf,PSTR("</body></html>\r\n"));
	sendDataToTXBuffer(buf);

	// Increase the S0_TX_WR value, so it point to the next transmit
	// Need to be after last TX transfer
	wizzWrite(S0_TX_WR,(offaddr & 0xFF00) >> 8 );
	wizzWrite(S0_TX_WR + 1,(offaddr & 0x00FF));

	// Now Send the HTTP Response
	sendWebPage();
				
}

int convertFirstDigitToInt(uint16_t index)
{
	char stringBuf[10];
	
	while ((buf[index] !='=') && (buf[index] !='\0'))
	{ index++;}

	i=0;
	while ((buf[index] !='&') && (buf[index] !='\0'))
	{
		if (isdigit(buf[index]))
		{
			stringBuf[i] = buf[index];
			i++;
		}
		index++;
	}
	stringBuf[i+1]='\0';
	return atoi(stringBuf);
}


void parseRequest(void){
	int podZad, wsKorek;
	uint8_t room, index;
	char stringBuf[20];
	
	stringBuf[0]='\0';
	podZad = 0;
	wsKorek = 0;
	
	// Text to parse   1.html?PodZad=25.0&WsKorek=0.4&SalonTaras=Zpisz
	pomNamePtr = strindex((char *)buf,"=Zapisz");
	if (pomNamePtr > 0)
	{
		i=0;
		// Find beginning of the room name
		while ((buf[pomNamePtr] !='&') && (buf[pomNamePtr] !='\0'))
		  {
			i++;
			pomNamePtr--;
		  }
		  
		// Copy room name
		i--;
		pomNamePtr++;
		for (index=0;index<i; index++)
		  {
			stringBuf[index] = buf[pomNamePtr+index];
		  }
		stringBuf[index]='\0';

		room = 0xFF;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[KUCHNIA])==0) room = KUCHNIA;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[SALON_JADALNIA])==0) room = SALON_JADALNIA;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[SALON_TARAS])==0) room = SALON_TARAS;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[POKOJ1_JULEK])==0) room = POKOJ1_JULEK;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[POKOJ2_JAS])==0) room = POKOJ2_JAS;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[LAZIENKA_MALA])==0) room = LAZIENKA_MALA;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[LAZIENKA_DUZA])==0) room = LAZIENKA_DUZA;
		if (strcmp_P((char *)stringBuf,nazwaPomieszczenia[POKOJ3_SYPIALNIA])==0) room = POKOJ3_SYPIALNIA;
		
		podZadPtr = strindex((char *)buf,"PodZad");
		if (podZadPtr >= 0)
		{ // There is a request to parse
			podZad = convertFirstDigitToInt(podZadPtr);
		}
		
		wsKorekPtr = strindex((char *)buf,"WsKorek");
		if (wsKorekPtr >= 0)
		{ // There is a request to parse
			wsKorek = convertFirstDigitToInt(wsKorekPtr);
		}
		
		if (room != 0xFF)
		{
			eeprom_update_word(&parametry_E[room].podlogaZadana,podZad);
			eeprom_update_word(&parametry_E[room].podlogaWsKorekcji,wsKorek);
			parametry[room].podlogaZadana = eeprom_read_word(&parametry_E[room].podlogaZadana);
			parametry[room].podlogaWsKorekcji = eeprom_read_word(&parametry_E[room].podlogaWsKorekcji);
		}
	}
}


void socketTCPStateMachine(uint8_t sock) 
{
	// At this moment function is written only for socket 0	
	if (sock != 0) return; 
	
	// Main state switch
	uint16_t rsize;
	unsigned char room;
	uint8_t* bufRoomNRptr;
	
	// Read Socked Status Register
	socketStatus=wizzRead(S0_SR);
	
	switch(socketStatus) {
	
	case SOCK_CLOSED:
		// Initialize socket
		openSocket(sock,MR_TCP,TCP_PORT);
		for (i=0; i<=TIME_TO_WAIT;i++);
		break;
		
	case SOCK_INIT:
		// Set socket in LISTEN state SERVER mode
		goListening(sock);
		for (i=0; i<=TIME_TO_WAIT;i++);
		break;
	
	case SOCK_LISTEN:
		// Listening to any client requests
		break;
		
	case SOCK_ESTABLISHED:
		for (i=0; i<=TIME_TO_WAIT;i++);
		// Get the client request size
		rsize = recv_size();
		if (rsize > 0) {
			// Now read the client Request
			if (recv(sock,buf,rsize) <= 0) break;
			
			room = '\0';
			// Analyze request
			GETindexPointer = strindex((char *)buf,"GET /");
			if (GETindexPointer >= 0)
			  {
				  parseRequest();
				  bufRoomNRptr = &buf[0];
				  room = *(bufRoomNRptr + 5);
				  if (room == '0' || room == ' ') generateWebPage((char *)buf, KUCHNIA);
				  if (room == '1') generateWebPage((char *)buf, SALON_JADALNIA);
				  if (room == '2') generateWebPage((char *)buf, POKOJ3_SYPIALNIA);
				  if (room == '3') generateWebPage((char *)buf, LAZIENKA_DUZA);
				  if (room == '4') generateWebPage((char *)buf, POKOJ1_JULEK);
				  if (room == '5') generateWebPage((char *)buf, POKOJ2_JAS);
				  if (room == '6') generateWebPage((char *)buf, LAZIENKA_MALA);
			  }
			for (i=0; i<=TIME_TO_WAIT;i++);
			disconnectSocket(sock);
			close(sock);
			}
		break;
		
	case SOCK_FIN_WAIT:
	case SOCK_CLOSING:
	case SOCK_TIME_WAIT:
	case SOCK_CLOSE_WAIT:
		{
			disconnectSocket(sock);
			close(sock);
		}
		break;
	
	}
}
