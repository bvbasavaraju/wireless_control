/**
  ******************************************************************************
  * @file    gsm_module.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   This file defines the GSM Module role in the product
  ******************************************************************************
  */

/*************************************************************************************************
 * #includes
 *************************************************************************************************/  
#include "gsm_module.h"

	#if(USE_GSM_MODULE != 0)
/*************************************************************************************************
 * #defines
 *************************************************************************************************/
#define Ctrl_Z  0x1A

/*************************************************************************************************
 * Gloabl Variables and Definition
 *************************************************************************************************/
static const char* OK_RESPONSE			= "OK";
//const char* ERROR_RESPONSE			= "ERROR";
static const char* RING_RESPONSE		= "RING";
static const char*	NO_CARRIER_RESPONSE	= "NO CARRIER";
static const char* GOT_MESSAGE_RESPONSE	= "+CMTI: ";

static const char* gLicensingUser1 		= "+919686952982";
static const char* gLicensingUser2 		= "+919886433750";

static uint8_t* gResponseDetails;
static uint8_t gResponseLength;
static uint8_t gCommandStartPosition;
static uint8_t gResponseCode;

static eGSM_States gGSMState = GSM_IDLE;
static const uint8_t gMaxRingWait = 0x03;
static const uint8_t gDeleteRetries = 0x03;

static uint8_t gUser[20] = "";
static uint8_t gValidUser = 0;

uint8_t gFlagLicensingUser = 0;
uint8_t gFlagPrimaryUser = 0;

uint8_t gCommandLength;

/*************************************************************************************************
 * Function Definition
 *************************************************************************************************/
/*
 * @name   	GSM_ReceiveWait()
 * @brief	This function will wait for maximum of 10 seconds for ther response form the GSM Module!
 * @param  	None
 * @retval	0x00	- if GSM responds within 10 seconds
 *			0xFF	- if GSM Module doesn't respond in 10 seconds
 * @note	This function wait for 2 seconds first then for 3 seconds and then 5 seconds.
 *			As _delay_ms is just a decreament counter operation, if there is an receive interrupt it should be captured by USART driver!
 */
uint8_t GSM_ReceiveWait()
{
	uint8_t retVal = 0xFF;
	
	//_delay_ms function is anyway decreament operation! So if there is any interrupt it should be capied to receive buffer
	_delay_ms(2000);	// First wait for 2 seocnds
	if(!gReceive_Buffer_Full)
	{
		_delay_ms(3000);	// Then wait for 3 seconds
		if(!gReceive_Buffer_Full)
		{
			_delay_ms(5000);	// At last wait for 5 seconds
		}
	}
	
	if(gReceive_Buffer_Full != 0)
		retVal = 0x00;
		
	return retVal;	
}
 
 /*
 * @name   	GSM_SendRequest()
 * @brief	This function will send the AT command to GSM module
 * @param  	message  - the AT command that has to be sent to GSM module
 *			response - the expected response from the GSM module
 * @retval	0x00	- if GSM responds the expected response for command
 *			0xFF	- if GSM it respond ERROR
 * @note	500ms delay is necessary after sending the message
 */
uint8_t GSM_SendRequest(const char* message, const char* response)
{
	uint8_t retVal = 0xFF;
	USART_FlushReceiveBuffer();

	print("%s\r\n", message);

	if((!GSM_ReceiveWait()) && (strcmp((const char*)gGSM_Response, response) == 0))
		retVal = 0x00;

	USART_FlushReceiveBuffer();

	return retVal;
}

/*
 * @name   	GSM_TestForResponse()
 * @brief	This function is to Check whether GSM module is responding for the AT request!
 * @param  	None
 * @retval	0x00	- if GSM responds to the command
 *			0xFF	- if GSM respond ERROR
 * @note
 */
uint8_t GSM_TestForResponse()
{
	uint8_t retVal;
	
	retVal = GSM_SendRequest("AT", OK_RESPONSE);

	if(retVal)
		retVal = GSM_SendRequest("AT", "ATOK");

	return retVal;
}

/*
 * @name   	GSM_SetEchoOFF()
 * @brief	This function is to set the ECHO OFF
 * @param  	None
 * @retval	0x00 	- if ECHO off successful
 *			0xFF	- if ECHO off unsuccessful
 * @note	This function has to be called by the developer to set the ECHO message from the GSM module
 *			ATE0 has to be sent to set off the echo
 */
uint8_t GSM_SetEchoOFF()
{
	//return GSM_SendRequest("ATE0", OK_RESPONSE);
	return GSM_SendRequest("ATE0", "ATE0OK");	//As Echo is not set to OFF yet, ATE0 will also captured in gGSM_Response
}

/*
 * @name   	GSM_DeleteAllMessages()
 * @brief	This function is to delete all the messages in the storage media
 * @param  	None
 * @retval	0x00 	- if all messages deleted successfully
 *			0xFF	- if deleting messages is unsuccessful
 * @note	This function takes total of minimum of 15 seconds to complete the action
 */
uint8_t GSM_DeleteAllMessages()
{
	uint8_t retVal = 0xFF;
	USART_FlushReceiveBuffer();

	print("AT+CMGDA=\"DEL ALL\"\r\n");

	if((!GSM_ReceiveWait()) && (strcmp((const char*)gGSM_Response, OK_RESPONSE) == 0))
		retVal = 0x00;
	//During Debug it is needed
	/*else
	{
		print("<del: %s>", gGSM_Response);
	}*/

    USART_FlushReceiveBuffer();

	return retVal;
}

/*
 * @name   	GSM_SetupForSMS()
 * @brief	This function is to set to text mode and storage medium to SIM storage.
 * @param  	None
 * @retval	0x00 	- if all messages deleted successfully and set to text mode
 *			0xFF	- otherwise
 * @note	This function will also delete the messages. So it will take more than 15 seconds to execute
 */
uint8_t GSM_SetupForSMS()
{
	uint8_t retVal = 0xFF;
	//Set the SMS to text mode
	if(GSM_SendRequest("AT+CMGF=1", OK_RESPONSE) == 0x00)
	{
		USART_FlushReceiveBuffer();

		//Set the storage media as SIM CARD
		_delay_ms(500);
		print("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");
		
		GSM_ReceiveWait();	// Wait for some time to recieve some data!
		USART_FlushReceiveBuffer();		//Clear Buffer

		//Delete all message
		_delay_ms(500);
		retVal = GSM_DeleteAllMessages();
	}
	return retVal;
}

/*
 * @name   	GSM_SetPrimayUser()
 * @brief	This function will store one of the licensing user as primary user
 * @param  	number  - phone number of the licensing user
 * @retval	None
 * @note
 */
void GSM_SetPrimayUser(const char* number)
{
	strcpy(gPrimeUser, number);
	updateEEPROM(PRIMARY_OPERATOR, (uint8_t*)gPrimeUser);
}

/*
 * @name   	GSM_CheckValidUser()
 * @brief	This function will whether the message or call received from the valid user
 * @param  	doubelQuoteOccurance - is the occurance of double quote in the response when either call or message is received
 *			doubelQuoteOccurance == 1 for the call
 *			doubelQuoteOccurance == 3 for message
 * @retval	0x00 	- if user is autherised user
 *			0xFF	- otherwise
 * @note	This function will look for the occurance of double quote in '"' in the reponse. and get the phone number from there!
 *			'+' is not present with number then, a '+' will be added in the valid user buffer
 */
uint8_t GSM_CheckValidUser(uint8_t doubelQuoteOccurance)
{
    uint8_t  retVal = 0xFF;
	uint8_t i = 0, j = 0;
	uint8_t count = 0;
	
	gValidUser = 0;

	_delay_ms(100);		//Delay is necessary!
	gResponseDetails = gGSM_Response;

	gResponseLength = strlen((const char*)gGSM_Response);
	count = 0;
	//find the double quote occurance
	for(i=1; i<gResponseLength; i++)
	{
		if(gResponseDetails[i] == '"')
			count++;

		if(count == doubelQuoteOccurance)
			break;
	}

	//if '+' is not preceeded with the phone number then add +!
	if(gResponseDetails[i+1] != '+')
	{
		gResponseDetails[i] = '+';
	}
	else
		i++;

	//Now copy the number with '+' sign
	for( j = 0; gResponseDetails[i] != '"'; i++, j++)
		gUser[j] = gResponseDetails[i];

	gUser[j] = '\0';
	//print("++V: %s++", gUser);

	_delay_ms(200);
	//Compare the number with the valid user list!
	//if(strcmp((const char*)gUser, gPrimeUser) == 0)
	gFlagLicensingUser = 0;
	gFlagPrimaryUser = 0;
	if((strlen((const char*)gPrimeUser) > 0) && (compareStrings((const char*)gPrimeUser, (const char*)gUser) == 0))
	{
		gFlagPrimaryUser = 1;
		gValidUser = 1;
		retVal = 0x00;
	}
	else if((strlen((const char*)gSecondUser) > 0) && (compareStrings((const char*)gSecondUser, (const char*)gUser) == 0))
	{
		gValidUser = 1;
		retVal = 0x00;
	}
	else if((strlen((const char*)gThirdUser) > 0) && (compareStrings((const char*)gThirdUser, (const char*)gUser) == 0))
	{
		gValidUser = 1;
		retVal = 0x00;
	}

	//Check if the user is a gLicensingUser1
	if((compareStrings((const char*)gUser, (const char*)gLicensingUser1) == 0) || (compareStrings((const char*)gUser, (const char*)gLicensingUser2) == 0))
	{
		gFlagLicensingUser = 1;
		gValidUser = 1;
		retVal = 0x00;
	}

    return retVal;
}

/*
 * @name   	GSM_PlayAudio()
 * @brief	This function will play audio option for the user
 * @param  	None
 * @retval	<Not Defined yet>
 * @note	This function will play the audio option for the user
 */
void GSM_PlayAudio()
{
	//Yet to implement!
}

/*
 * @name   	GSM_HandleCall()
 * @brief	This function check for the number of rings from the autherised user
 * @param  	None
 * @retval	None
 * @note	This function will increment the ring count given by the user
 *			If gMaxRingWait count is reached it will accept the call and play the audio option.
 *			If gMaxRingWait count is not reached then gGSMState will be changed to GSM_MISSED_CALL
 */
void GSM_HandleCall()
{
	uint8_t ringCount = 0;

	ringCount++;

	while(ringCount < gMaxRingWait)
	{
		while(!gReceive_Buffer_Full)
			;

		if(strcmp((const char*)gGSM_Response, (const char*)RING_RESPONSE) == 0)
			ringCount++;

		else if(strcmp((const char*)gGSM_Response, (const char*)NO_CARRIER_RESPONSE) == 0)
			break;

		USART_FlushReceiveBuffer();
	}

	if(ringCount >= gMaxRingWait)
	{
		GSM_PlayAudio();
		GSM_SendRequest("ATH0", OK_RESPONSE);
		gGSMState = GSM_IDLE;
	}
	else	//Missed call! Toggle the default switch
	{
		gGSMState = GSM_MISSED_CALL;
	}
}

/*
 * @name   	GSM_ExtractArguement()
 * @brief	This function extract the arguement 
 * @param  	uint8_t - lenght of the command which was sent!
 * @retval	None
 * @note	This function will extract the arguement from the message response captured in gGSM_Response
 *			Ex of command sent is: SWITCH ON ALL,
 *			SWITCH ON - is a command
 *			ALL		  - is an arguement
 */
void GSM_ExtractArguement(uint8_t length)
{
	uint8_t i;
	uint8_t size;
	uint8_t arguementStartPosition;
	
	size = (gCommandLength - (length + 1));
	arguementStartPosition = gCommandStartPosition + length + 1;
	
	//gResponseDetails += (length + 1);
	gArguement = (uint8_t *)malloc((sizeof(uint8_t) * size) + 1);
	//strncpy((char *)gArguement, (const char*)gResponseDetails, (gCommandLength - (length + 1)));
	for(i = 0; i< size; i++)
	{
		gArguement[i] = gGSM_Response[arguementStartPosition + i];
	}
	gArguement[i] = '\0';
}

/*
 * @name   	GSM_ProcessMessage()
 * @brief	This function will process the message and do the action accordingly!
 * @param  	None
 * @retval	0x00 - if message received from autherised user and appropriate action has been taken place
 *			0xFF - otherwise
 * @note	This function will check whether user is autherised user
 *			Then from the end look for first double quote '"'
 *			Extract command
 *			This function will delete the message. Hence it will take at least 15 seconds to respond!
 *			User has to provide minimum of 15 seconds after a message has been sent to give next command.
 *			if any new command sent or called in proper response is not guarenteed!
 */
uint8_t GSM_ProcessMessage()
{
	uint8_t retVal = FAILED;
	uint8_t i = 0;

	USART_FlushReceiveBuffer();

	//Set for Text format
	if(GSM_SendRequest("AT+CMGF=1", OK_RESPONSE) == 0x00)
	{
		USART_FlushReceiveBuffer();
		//Read the message
		print("AT+CMGR=1\r\n");	//Read Message in location 1

		if(!GSM_ReceiveWait())
		{
			if(!GSM_CheckValidUser(3))	// 3rd occurance of double quote
			{
				gResponseDetails = gGSM_Response;
				gResponseLength = strlen((const char*) gGSM_Response);
				//check for first " from the end!
				for(i = gResponseLength-1; i>=0; i--)
				{
					if(gResponseDetails[i] == '"')
					{
						gCommandStartPosition = i;
						break;
					}
				}

				//Extract the gCommand
				gCommandStartPosition += 1;
				gCommandLength = gResponseLength - 2 - gCommandStartPosition;

				//gResponseDetails = gGSM_Response;
				//gResponseDetails += gCommandStartPosition;
				gCommand = (uint8_t *)(malloc((sizeof(uint8_t) * gCommandLength) + 1));
				//strncpy((char *)gCommand, (const char*)gResponseDetails, gCommandLength);
				for(i = 0; i<gCommandLength ; i++)
					gCommand[i] = gGSM_Response[gCommandStartPosition + i];
				gCommand[i] = '\0';
				
				//Needed for Debug!
				//print("<msg: %s>", gGSM_Response);
				//print("<Resp: %s>", gResponseDetails);
				//print("<cmd: %s>", gCommand);

				if((gDeviceLicensed) || ((!gDeviceLicensed) && (!licenseCommand())))
				{
					retVal = processCommand();
				}
#if(USE_DETAILED_RESPONSE != 0)
				else
					retVal = NOT_LICENSED;
#endif	//USE_DETAILED_RESPONSE
				if(strlen((const char*)gCommand));
					free(gCommand);
			}
#if(USE_DETAILED_RESPONSE != 0)
			else
				retVal = INVALID_USER;
#endif	//USE_DETAILED_RESPONSE

			/*USART_FlushReceiveBuffer();
			
			//If in some cases deleting messages fails, retry for maximum allowed number of times
			for(i = 0; i < gDeleteRetries; i++)
			{
				if(!GSM_DeleteAllMessages())
					break;
			}
			if(i >= 3)
				retVal = SERVICE_NEEDED;

			USART_FlushReceiveBuffer();*/
		}
#if(USE_DETAILED_RESPONSE != 0)
		else
			retVal = TIMEOUT;
#endif	//USE_DETAILED_RESPONSE
	}
	
	//If message is recieved, success or failure in processing command, messages should be deleted!
	USART_FlushReceiveBuffer();		
	//If in some cases deleting messages fails, retry for maximum allowed number of times
	for(i = 0; i < gDeleteRetries; i++)
	{
		if(!GSM_DeleteAllMessages())
			break;
	}
	if(i >= 3)
		retVal = SERVICE_NEEDED;

	USART_FlushReceiveBuffer();

	return retVal;
}

/*
 * @name   	GSM_AcknowledgeService()
 * @brief	This function will reply back the acknowledgement for the service
 * @param  	None
 * @retval	None
 * @note	This function will look for the id number in the OPERATIONS[]. and responds the ack message to Primery user.
 */
void GSM_AcknowledgeService()
{
	uint8_t i;

	if((gValidUser != 0) || ((strlen((const char*)gPrimeUser) > 0) && (gResponseCode == DEVICE_ON)))
	{
		if(GSM_SendRequest("AT+CMGF=1", OK_RESPONSE) == 0x00)
		{
			USART_FlushReceiveBuffer();
			_delay_ms(500);
			
			if(gResponseCode == DEVICE_ON)
				print("at+cmgs=\"%s\"\r\n", gPrimeUser);
			else
				print("at+cmgs=\"%s\"\r\n", gUser);
			
			if((!GSM_ReceiveWait()) && (gGSM_Response[0] == 0x3E))	// 0x3E == '>' indicating to compose message to be sent from GSM module
			{
				USART_FlushReceiveBuffer();

				i = 0;
				while((i<totalNumberOfStatusCodes) && (STATUS_CODE[i].id != gResponseCode))
					i++;
				if(i<totalNumberOfStatusCodes)
					print("%s", STATUS_CODE[i].data);

				//Special Character Ctrl+Z to be sent to close the message
				USART_PutChar(Ctrl_Z); //26 in Decimal

				GSM_ReceiveWait();

				_delay_ms(1000);//Delay is necessary
			}
			
			//Special Character Ctrl+Z to be sent to close the message
			/*USART_PutChar(Ctrl_Z); //26 in Decimal

			GSM_ReceiveWait();

			_delay_ms(1000);//Delay is necessary*/
		}
	}
}

/*
 * @name   	GSM_WaitAndProcessRequest()
 * @brief	This function will handle the state machine of the GSM module!
 * @param  	None
 * @retval	None
 * @note	This function is an infinite loop
 *			Initial state is GSM_IDLE. Here system wait for either a message or call
 *			GSM_VOICE_CALL - Handles the voice call. If user is autherised user. else cut the call and set the state to GSM_IDLE
 *			GSM_MISSED_CALL - If autherised user gives missed call toggle the default switch
 *			GSM_READ_MESSAGE - if autherised user sends the message then do appropriate action
 *			GSM_WRITE_MESSAGE - send the response back to valid user!
 */
void GSM_WaitAndProcessRequest()
{
	//Inform the prime user that Initialization complete
	gResponseCode = DEVICE_ON;
	gGSMState = GSM_WRITE_MESSAGE;

	while(1)
	{
		switch(gGSMState)
		{
			case GSM_IDLE:

					USART_FlushReceiveBuffer();
					
					//Wait for either message or call to arrive!
					while(!gReceive_Buffer_Full)
						;
					
					if(compareStrings((const char*)gGSM_Response, RING_RESPONSE) == 0)
					{
						USART_FlushReceiveBuffer();
						gGSMState = GSM_VOICE_CALL;
					}
					else if(compareStrings((const char*)gGSM_Response, GOT_MESSAGE_RESPONSE) == 0)
					{
						gGSMState = GSM_READ_MESSAGE;
					}
					
				break;

			case GSM_VOICE_CALL:
					if(gReceive_Buffer_Full)
					{
					    if(gDeviceLicensed && (!GSM_CheckValidUser(1)))		// 1st occurance of double quote
						{
							GSM_HandleCall();
						}
						else
						{
							GSM_SendRequest("ATH0", OK_RESPONSE);
#if(USE_DETAILED_RESPONSE != 0)
							if(!gDeviceLicensed)
							{
								gResponseCode = NOT_LICENSED;
								gGSMState = GSM_WRITE_MESSAGE;
							}
							else
#endif	//USE_DETAILED_RESPONSE
								gGSMState = GSM_IDLE;
						}
					}
				break;

			case GSM_MISSED_CALL:
				//Toggle Default Switch
				if(gMissedCallFeature)
				{
					gCommand = (uint8_t*)"TOGGLE";
					gResponseCode = processCommand();
					USART_FlushReceiveBuffer();
					//Change State to Writing message
					gGSMState = GSM_WRITE_MESSAGE;
				}
				else
					gGSMState = GSM_IDLE;
				break;

			case GSM_READ_MESSAGE:
					gResponseCode = GSM_ProcessMessage();
					USART_FlushReceiveBuffer(); //For safer side
					gGSMState = GSM_WRITE_MESSAGE;
				break;

			case GSM_WRITE_MESSAGE:
					USART_FlushReceiveBuffer();
					//#if 0
					if((gResponseCode == DEVICE_ON) || gServiceAcknowledgement)
						GSM_AcknowledgeService();
					//#endif
					gGSMState = GSM_IDLE;
				break;

			default:
				//System Should never enter to this state
				break;
		}
	}
}

	#endif	//USE_GSM_MODULE
