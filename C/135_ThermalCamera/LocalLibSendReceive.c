/*
 * LocalLibSendReceive.c
 *
 * Created: 2014-03-03 09:59:59
 *  Author: Tomasz Szkudlarek
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "LocalLibSendReceive.h"
#include "LocalLib_PWM.h"
#include "LocalLibServo.h"
#include "LocalLib_PORTS.h"
#include "LocalLibADC.h"

//*******************************************************
//**********			COMMAND 			*************
//*******************************************************
//Execute command
void execute_command(void)
{
	char CommandType;
	
	CommandType = Rs_Rx_command[0];

	if (CommandType == 'w')
	{
		writeCommand();
	}

	else if (CommandType == 'r')
	{
		readCommand();
	}
	else
	{
		syntaxError();
	}
}


//*******************************************************
//**********			PARAMETER 			*************
//*******************************************************
//*******************************************************
//  READ
void readCommand()
{	
	char ReadParameter;
	
	ReadParameter = Rs_Rx_command[2];

	if (ReadParameter == 'd')
		{
//			readDayOfTheYear();
		}

	else if (ReadParameter == 't')
		{
//			readTime();
		}
	else 
	  {
		syntaxError();
	  }  	
}
//*******************************************************
//*******************************************************
//  WRITE
void writeCommand(void)
{	
	char WriteParameter;
	WriteParameter = Rs_Rx_command[2];

	if (WriteParameter == 'H')
		{
			horizontalSettingsWrite();
		}
	else if (WriteParameter == 'V')
		{
			verticalSettingsWrite();
		}
	else if (WriteParameter == 'S')
		{
//			autoScanSettingsWrite();
		}
	else 
	  {
		syntaxError();
	  }
}

//*******************************************************
//  WRITE
//  HORIZONTAL settings
//*******************************************************
void horizontalSettingsWrite()
	{
		char WriteParameter;
		WriteParameter = Rs_Rx_command[3];

		if (WriteParameter == 'p')
		{
			servo_off_watchdog=0;
			SERVO_PWR_ON;
			horizontalPosSet();
		}
		else
		{
			syntaxError();
		}
	}

//*******************************************************
//  WRITE
//  VERTICAL settings
//*******************************************************
void verticalSettingsWrite()
{
	char WriteParameter;
	WriteParameter = Rs_Rx_command[3];

	if (WriteParameter == 'p')
	{
		servo_off_watchdog=0;
		SERVO_PWR_ON;
		verticalPosSet();
	}
	else
	{
		syntaxError();
	}
}

//*******************************************************
// SET Horizontal POS
void horizontalPosSet()
	{
		//Start ADC conversion just before movement to new position
		horizontalPosMinus1=OCR1A - T1_TOP_VALUE_1ms;
		
		//Restart ADC update timer
		adc_update_time = 0;
		
		start_conversion(INFRA_RED_ADC_CHANEL);
		
		//Setting new position
		uint16_t horizontalPos;
		horizontalPos = (Rs_Rx_command[5] & 0x0F)*1000;
		horizontalPos = horizontalPos + (Rs_Rx_command[6] & 0x0F)*100;
		horizontalPos = horizontalPos + (Rs_Rx_command[7] & 0x0F)*10;
		horizontalPos = horizontalPos + (Rs_Rx_command[8] & 0x0F);
		OCR1A = T1_TOP_VALUE_1ms + horizontalPos;
			
	}
//*******************************************************
// SET Vertical POS
void verticalPosSet()
	{
		//Start ADC conversion just before movement to new position
		verticalPosMinus1=OCR1B - T1_TOP_VALUE_1ms;

		//Restart ADC update timer
		adc_update_time = 0;

		start_conversion(INFRA_RED_ADC_CHANEL);
		
		// Setting new position
		uint16_t verticalPos;
		verticalPos = (Rs_Rx_command[5] & 0x0F)*1000;
		verticalPos = verticalPos + (Rs_Rx_command[6] & 0x0F)*100;
		verticalPos = verticalPos + (Rs_Rx_command[7] & 0x0F)*10;
		verticalPos = verticalPos + (Rs_Rx_command[8] & 0x0F);
		OCR1B = T1_TOP_VALUE_1ms + verticalPos;
	}

//*******************************************************
//**********		H/V POS ADC values		*************
//*******************************************************

void sendHVPosInfraredThemperature(uint16_t Hpos,uint16_t Vpos, uint16_t InfraValue, uint16_t SenorThemp)
	{
		sprintf((char *)Rs_Tx_command,"Hp:%04d;\n",Hpos);
		sprintf((char *)TempString,"Vp:%04d;\n",Vpos);
		strcat((char *)Rs_Tx_command,(char *)TempString);
		sprintf((char *)TempString,"Th:%04d;\n",InfraValue);
		strcat((char *)Rs_Tx_command,(char *)TempString);
		sprintf((char *)TempString,"Tm:%04d;\n",SenorThemp);
		strcat((char *)Rs_Tx_command,(char *)TempString);
		startRsTransmit();
	}

//*******************************************************
//*******************************************************
// START Transmit
void startRsTransmit()
{
	// Start RS TX
	Tx_Command_Lengh = strlen((char *)Rs_Tx_command);
	Rs_Tx_command_index = 0;
	if (Rs_Tx_command_index < Tx_Command_Lengh)
	{
		UDR0 = Rs_Tx_command[Rs_Tx_command_index];
		Rs_Tx_command_index++;
	}
}
// START Transmit
void startRsTransmitWithNewLine()
{
	// Start RS TX
	Tx_Command_Lengh = strlen((char *)Rs_Tx_command);
	Rs_Tx_command_index = 0;
	UDR0 = 0x0D;											//Start RS TX with new line
}
//*******************************************************
//*******************************************************
//Positive Response
void send_positive_response()
{
	strcpy((char *)Rs_Tx_command,"OK");
	startRsTransmitWithNewLine();
}

//*******************************************************
//*******************************************************
// ERRORs
void syntaxError()
{
	strcpy((char *)Rs_Tx_command,"Syntax Error");
	startRsTransmitWithNewLine();
	Rs_Rx_command_index = 0;			// Clear RX buffer
}
//*******************************************************
//*******************************************************
// TX
ISR(USART_TX_vect)
{	
	if (Rs_Tx_command_index < Tx_Command_Lengh)
	{
		UDR0 = Rs_Tx_command[Rs_Tx_command_index];
		Rs_Tx_command_index++;
	}
}
//*******************************************************
//*******************************************************
// RX
ISR(USART_RX_vect)
{
	uint8_t RsByte;
	
	//Read data from RS
	RsByte = UDR0;

	// Start command search
	if (RsByte == ';')  //
		{
		if (Rs_Rx_command_index > 0)
			{
				//Execute command
				execute_command();
				Rs_Rx_command_index = 0;
			}
		}
	else
		{
		if (Rs_Rx_command_index <= RS_RX_COMMAND_LENGHT_MAX)
			{
				Rs_Rx_command[Rs_Rx_command_index] = RsByte;
				Rs_Rx_command_index++;
			}

		}
}
//*******************************************************
//*******************************************************