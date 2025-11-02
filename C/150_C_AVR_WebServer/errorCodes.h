/*
 * errorCodes.h
 *
 */ 


#define		RESP_OK				0		// No errors
#define		RESP_GENERAL_ERROR	1		// General error





/*
 * wizznetW5100.c
 *
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
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

PARAMETRY parametry[10];

void initializeParametry(){
	
	// Initial values
	// 32.6 = 326    wszystko x10 (dec)
	parametry[KUCHNIA].podlogaZadana = 240;
	parametry[KUCHNIA].podlogaAktualna = 240;  // => 24.0
	parametry[KUCHNIA].podlogaWsKorekcji = 2;  // => 0.2
	parametry[KUCHNIA].podlogaKorekta = 0;
	parametry[KUCHNIA].powietrzeAktualna = 192;
	parametry[KUCHNIA].otwarcieZaworu = 543;	// => 54.3 %

	parametry[SALON_JADALNIA].podlogaZadana = 245;
	parametry[SALON_JADALNIA].podlogaAktualna = 245;
	parametry[SALON_JADALNIA].podlogaWsKorekcji = 3;
	parametry[SALON_JADALNIA].podlogaKorekta = 0;
	parametry[SALON_JADALNIA].powietrzeAktualna = 196;
	parametry[SALON_JADALNIA].otwarcieZaworu = 526;

	parametry[SALON_TARAS].podlogaZadana = 250;
	parametry[SALON_TARAS].podlogaAktualna = 250;
	parametry[SALON_TARAS].podlogaWsKorekcji = 4;
	parametry[SALON_TARAS].podlogaKorekta = 0;
	parametry[SALON_TARAS].powietrzeAktualna = 199;
	parametry[SALON_TARAS].otwarcieZaworu = 581;

	parametry[POKOJ1_JULEK].podlogaZadana = 230;
	parametry[POKOJ1_JULEK].podlogaAktualna = 230;
	parametry[POKOJ1_JULEK].podlogaWsKorekcji = 2;
	parametry[POKOJ1_JULEK].podlogaKorekta = 0;
	parametry[POKOJ1_JULEK].powietrzeAktualna = 183;
	parametry[POKOJ1_JULEK].otwarcieZaworu = 444;

	parametry[POKOJ2_JAS].podlogaZadana = 231;
	parametry[POKOJ2_JAS].podlogaAktualna = 230;
	parametry[POKOJ2_JAS].podlogaWsKorekcji = 2;
	parametry[POKOJ2_JAS].podlogaKorekta = 0;
	parametry[POKOJ2_JAS].powietrzeAktualna = 186;
	parametry[POKOJ2_JAS].otwarcieZaworu = 449;

	parametry[LAZIENKA_MALA].podlogaZadana = 260;
	parametry[LAZIENKA_MALA].podlogaAktualna = 260;
	parametry[LAZIENKA_MALA].podlogaWsKorekcji = 2;
	parametry[LAZIENKA_MALA].podlogaKorekta = 0;
	parametry[LAZIENKA_MALA].powietrzeAktualna = 213;
	parametry[LAZIENKA_MALA].otwarcieZaworu = 601;

	parametry[LAZIENKA_DUZA].podlogaZadana = 272;
	parametry[LAZIENKA_DUZA].podlogaAktualna = 272;
	parametry[LAZIENKA_DUZA].podlogaWsKorekcji = 2;
	parametry[LAZIENKA_DUZA].podlogaKorekta = 0;
	parametry[LAZIENKA_DUZA].powietrzeAktualna = 216;
	parametry[LAZIENKA_DUZA].otwarcieZaworu = 623;
	
	parametry[POKOJ3_SYPIALNIA].podlogaZadana = 232;
	parametry[POKOJ3_SYPIALNIA].podlogaAktualna = 232;
	parametry[POKOJ3_SYPIALNIA].podlogaWsKorekcji = 1;
	parametry[POKOJ3_SYPIALNIA].podlogaKorekta = 0;
	parametry[POKOJ3_SYPIALNIA].powietrzeAktualna = 182;
	parametry[POKOJ3_SYPIALNIA].otwarcieZaworu = 323;

}
void W5100_Init(void)
{
	// Ethernet Setup
	unsigned char mac_addr[] = {0x00,0x16,0x36,0xDE,0x58,0xF6};
	unsigned char ip_addr[] = {192,168,2,10};
	unsigned char sub_mask[] = {255,255,255,0};
	unsigned char gtw_addr[] = {192,168,2,1};
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

void close(uint8_t sock)
{
	if (sock != 0) return;

	// Send Close Command
	wizzWrite(S0_CR,CR_CLOSE);
	// Waiting until the S0_CR is clear
	while(wizzRead(S0_CR));
}
void disconnect(uint8_t sock)
{
	if (sock != 0) return;
	// Send Disconnect Command
	wizzWrite(S0_CR,CR_DISCON);
	// Wait for Disconecting Process
	while(wizzRead(S0_CR));
}
uint8_t socket(uint8_t sock,uint8_t eth_protocol,uint16_t tcp_port)
{
	uint8_t retval=0;
	if (sock != 0) return retval;

	// Make sure we close the socket first
	if (wizzRead(S0_SR) == SOCK_CLOSED) {
		close(sock);
	}
	// Assigned Socket 0 Mode Register
	wizzWrite(S0_MR,eth_protocol);

	// Now open the Socket 0
	wizzWrite(S0_PORT,((tcp_port & 0xFF00) >> 8 ));
	wizzWrite(S0_PORT + 1,(tcp_port & 0x00FF));
	wizzWrite(S0_CR,CR_OPEN);                   // Open Socket
	// Wait for Opening Process
	while(wizzRead(S0_CR));
	// Check for Init Status
	if (wizzRead(S0_SR) == SOCK_INIT)
	retval=1;
	else
	close(sock);

	return retval;
}
uint8_t listen(uint8_t sock)
{
	uint8_t retval = 0;
	if (sock != 0) return retval;
	if (wizzRead(S0_SR) == SOCK_INIT) {
		// Send the LISTEN Command
		wizzWrite(S0_CR,CR_LISTEN);

		// Wait for Listening Process
		while(wizzRead(S0_CR));
		// Check for Listen Status
		if (wizzRead(S0_SR) == SOCK_LISTEN)
		retval=1;
		else
		close(sock);
	}
	return retval;
}

/*
uint16_t send(uint8_t sock,const uint8_t *buf,uint16_t buflen)
{
	uint16_t ptr,offaddr,realaddr,txsize,timeout;

	if (buflen <= 0 || sock != 0) return 0;
	// Make sure the TX Free Size Register is available
	txsize=wizzRead(SO_TX_FSR);
	txsize=(((txsize & 0x00FF) << 8 ) + wizzRead(SO_TX_FSR + 1));
	timeout=0;
	while (txsize < buflen) {
		_delay_ms(1);
		txsize=wizzRead(SO_TX_FSR);
		txsize=(((txsize & 0x00FF) << 8 ) + wizzRead(SO_TX_FSR + 1));
		// Timeout for approx 1000 ms
		if (timeout++ > 1000) {
			// Disconnect the connection
			disconnect(sock);
			LED_Y1_TRG();
			return 0;
		}
	}

	// Read the Tx Write Pointer
	ptr = wizzRead(S0_TX_WR);
	offaddr = (((ptr & 0x00FF) << 8 ) + wizzRead(S0_TX_WR + 1));

	while(buflen) {
		buflen--;
		// Calculate the real W5100 physical Tx Buffer Address
		realaddr = TXBUFADDR + (offaddr & TX_BUF_MASK);
		// Copy the application data to the W5100 Tx Buffer
		wizzWrite(realaddr,*buf);
		offaddr++;
		buf++;
	}

	// Increase the S0_TX_WR value, so it point to the next transmit
	wizzWrite(S0_TX_WR,(offaddr & 0xFF00) >> 8 );
	wizzWrite(S0_TX_WR + 1,(offaddr & 0x00FF));

	// Now Send the SEND command
	wizzWrite(S0_CR,CR_SEND);

	// Wait for Sending Process
	while(wizzRead(S0_CR));

	return 1;
}
*/
uint16_t recv(uint8_t sock,uint8_t *buf,uint16_t buflen)
{
	uint16_t ptr,offaddr,realaddr;

	if (buflen <= 0 || sock != 0) return 1;

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
	_delay_us(5);    // Wait for Receive Process

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

void socketState(uint8_t sock) 
{
	// At this moment function is written only for socket 0	
	if (sock != 0) return; 
	
	// Main state switch
	uint8_t socketStatus;
	uint16_t rsize;
	int getidx,postidx;
	uint8_t buf[MAX_BUF];
	char tempString[8];
	
	// Read Socked Status Register
	socketStatus=wizzRead(S0_SR);
	
	switch(socketStatus) {
	
	case SOCK_CLOSED:
		// Initialize socket if closed
		if (socket(sock,MR_TCP,TCP_PORT) > 0) {
			// Set socket 0 in LISTEN state
			listen(sock);
			_delay_ms(20);
		}
		break;
	
	case SOCK_ESTABLISHED:
		// Get the client request size
		rsize = recv_size();
		if (rsize > 0) {
			// Now read the client Request
			if (recv(sock,buf,rsize) <= 0) break;
			
			// Check the Request Header
			getidx=strindex((char *)buf,"GET /");
			postidx=strindex((char *)buf,"POST /");
			if (getidx >= 0 || postidx >= 0) {
				// Now check the Radio Button for POST request
					/*
					if (postidx >= 0) {
						if (strindex((char *)buf,"radio=0") > 0)
							// ledmode=0;
						if (strindex((char *)buf,"radio=1") > 0)
							// ledmode=1;
					} */
				// Create the HTTP Response	Header
				strcpy_P((char *)buf,PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"));
				strcat_P((char *)buf,PSTR("<html><title>Dom Zdziechow</title>\r\n"));
				strcat_P((char *)buf,PSTR("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\r\n"));
				strcat_P((char *)buf,PSTR("<body bgcolor=\"#EEEEEE\" style=\"width:500px; margin: 0px auto;\">\r\n"));
				strcat_P((char *)buf,PSTR("<p allign=\"left\"><font face=\"Arial\">\r\n"));
				strcat_P((char *)buf,PSTR("<h1>Dom Zdziechow</h1>\r\n"));
				strcat_P((char *)buf,PSTR("<h2>Temperatury</h2></font></p><div allign=\"left\">\r\n"));
				strcat_P((char *)buf,PSTR("<p><font face=\"Courier New\" color=\"Blue\">\r\n"));
				strcat_P((char *)buf,PSTR("Ciepla woda uzytkowa: <b>34.6</b><sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("Centralne ogrzewanie: <b>33.1</b><sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("Na zewnatrz: <b>14.9</b><sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("</font></p></div>\r\n"));
				
				strcat_P((char *)buf,PSTR("<form method=\"GET\"><div allign=\"left\" ><p>\r\n"));
				strcat_P((char *)buf,PSTR("<font face=\"Courier New\" color=\"Blue\">\r\n"));
				strcat_P((char *)buf,PSTR("<fieldset><legend><h3>"));
				strcat_P((char *)buf,nazwaPomieszczenia[KUCHNIA]);
				strcat_P((char *)buf,PSTR("</h3></legend><br />\r\n"));
				strcat_P((char *)buf,PSTR("Podloga:..Zadana: <b><input type=\"text\" name=\"KuchniaPodZad\" value=\"\r\n"));
				strcat((char *)buf, "555");
				strcat_P((char *)buf,PSTR("\"\r\n maxlength=\"4\" size=\"4\" style=\"font-weight: bold\" /></b><sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("Podloga:Aktualna: <b>28.6</b><sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("Podloga:ws.korek: <input type=\"text\" name=\"KuchniaWsKorek\" value=\"0.20\"\r\n"));
				strcat_P((char *)buf,PSTR(" maxlength=\"4\" size=\"4\" style=\"font-weight: bold\" /><br />\r\n"));
				strcat_P((char *)buf,PSTR("Podloga:.Korekta: 00.3<sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("Powietrze:Aktual: <b>21.4</b><sup>o</sup>C<br />\r\n"));
				strcat_P((char *)buf,PSTR("Otwarcie zaworu : 67.9 procent<br />\r\n"));
				strcat_P((char *)buf,PSTR("<input type=\"submit\" value=\"Zpisz\" />\r\n"));
				strcat_P((char *)buf,PSTR("</fieldset></font></p></div></form>\r\n"));
				
				strcat_P((char *)buf,PSTR("</body></html>\r\n"));
				// Now Send the HTTP Response
				if (send(sock,buf,strlen((char *)buf)) <= 0) break;
				

				// Create the HTTP Temperature Response
				//sprintf((char *)temp,"%d",tempvalue);        // Convert temperature value to string
				//strcpy_P((char *)buf,PSTR("<strong>Temp: <input type=\"text\" size=2 value=\""));
				//strcat((char *)buf,"21");
				//strcat_P((char *)buf,PSTR("\"> <sup>O</sup>C\r\n"));
				/*
				if (ledmode == 1) {
					strcpy(radiostat0,"");
					strcpy_P(radiostat1,PSTR("checked"));
					} else {
					strcpy_P(radiostat0,PSTR("checked"));
					strcpy(radiostat1,"");
				}
				// Create the HTTP Radio Button 0 Response
				strcat_P((char *)buf,PSTR("<p><input type=\"radio\" name=\"radio\" value=\"0\" "));
				strcat((char *)buf,radiostat0);
				strcat_P((char *)buf,PSTR(">Blinking LED\r\n"));
				strcat_P((char *)buf,PSTR("<br><input type=\"radio\" name=\"radio\" value=\"1\" "));
				strcat((char *)buf,radiostat1);
				strcat_P((char *)buf,PSTR(">Scanning LED\r\n"));
				strcat_P((char *)buf,PSTR("</strong><p>\r\n"));
				strcat_P((char *)buf,PSTR("<input type=\"submit\">\r\n"));
				strcat_P((char *)buf,PSTR("</form></span></body></html>\r\n"));
				// Now Send the HTTP Remaining Response
				if (send(sock,buf,strlen((char *)buf)) <= 0) break;
				*/
			}
			// Disconnect the socket
			disconnect(sock);
		_delay_us(10);    // Wait for request
		break;
		}
	case SOCK_FIN_WAIT:
	case SOCK_CLOSING:
	case SOCK_TIME_WAIT:
	case SOCK_CLOSE_WAIT:
	case SOCK_LAST_ACK:
	// Force to close the socket
	close(sock);
	break;
	}
}
