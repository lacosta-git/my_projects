/*
 * LocalLibSendReceive.h
 *
 * Created: 2014-03-03 09:51:28
 *  Author: Tomasz Szkudlarek
 */ 


#ifndef LOCALLIBSENDRECEIVE_H_
#define LOCALLIBSENDRECEIVE_H_

#define RS_TX_COMMAND_LENGHT_MAX	50
volatile uint8_t Rs_Tx_command_index;
volatile uint8_t Tx_Command_Lengh;
volatile char Rs_Tx_command[RS_TX_COMMAND_LENGHT_MAX];
volatile char TempString[20];

#define RS_RX_COMMAND_LENGHT_MAX	20
volatile uint8_t Rs_Rx_command_index;
volatile char Rs_Rx_command[RS_RX_COMMAND_LENGHT_MAX];

volatile uint16_t horizontalPosMinus1;
volatile uint16_t verticalPosMinus1;

void horizontalSettingsWrite(void);
void verticalSettingsWrite(void);
void autoScanSettingsWrite(void);

void verticalPosSet(void);
void horizontalPosSet(void);

void sendHVPosInfraredThemperature(uint16_t, uint16_t, uint16_t, uint16_t);
void writeCommand(void);
void readCommand(void);
void send_positive_response(void);
void execute_command(void);
void startRsTransmit(void);
void startRsTransmitWithNewLine(void);
void syntaxError(void);

#endif /* LOCALLIBSENDRECEIVE_H_ */

