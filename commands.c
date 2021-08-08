/**
  ******************************************************************************
  * @file    commands.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    26-June-2015
  * @brief   This file will process the command sent by user and trigger the action based on the command
  ******************************************************************************
  */

/*************************************************************************************************
 * #includes
 *************************************************************************************************/
#include "commands.h"

/*************************************************************************************************
 * Global variables and definitions
 *************************************************************************************************/
static uint8_t gSwitchOneName[10];
static uint8_t gSwitchTwoName[10];

char gPrimeUser[20] = "";
char gSecondUser[20] = "";
char gThirdUser[20] = "";
char gLicenseNumber[13] = "";

uint8_t *gCommand;
uint8_t *gArguement;

uint8_t gServiceAcknowledgement = 0;
uint8_t gDeviceLicensed = 0;
 #if(USE_GSM_MODULE != 0)
uint8_t gMissedCallFeature = 1;
 #endif	//USE_GSM_MODULE

static const Struct_Data_Format COMMANDS[] =
{
	//Set of Action Commands
	{SWITCH_ON, "SWITCH ON"},
	{SWITCH_OFF, "SWITCH OFF"},
	{GET_SWITCHSTATE, "GET SWITCHSTATE"},
	{TOGGLE_DEFAULT_SWITCH, "TOGGLE"},

	//Set of Config Commands
	{ACK_ON, "ACK ON"},
	{ACK_OFF, "ACK OFF"},
#if(USE_GSM_MODULE != 0)
	{MISSED_CALL_ON, "MISSED CALL FEATURE ON"},
	{MISSED_CALL_OFF, "MISSED CALL FEATURE OFF"},
#endif	//USE_GSM_MODULE
	{ADD_OPERATOR, "ADD OPERATOR"},
	{REMOVE_OPERATOR, "REMOVE OPERATOR"},
	{SET_PRIMARY_USER, "SET PRIMARY USER"},
	{REMOVE_ALL, "REMOVE ALL"},
	{SET_LICENSE, "SET LICENSE"},
	{GET_LICENSE, "GET LICENSE"},
	{GET_VERSION, "GET VERSION"},
};

const Struct_Data_Format STATUS_CODE[] =
{
	{SUCCESSFUL, "SUCCESS"},
	{DEVICE_ON, "DEVICE IS ON"},
	{STAUTS_ON, "ON"},
	{STATUS_OFF, "OFF"},
	{SUCCESSFULLY_SWITCHED_OFF, "SUCCESSFULLY SWITCHED OFF"},
	{SUCCESSFULLY_SWITCHED_ON, "SUCCESSFULLY SWITCHED ON"},
	{SERVICE_NEEDED, "SERVICE NEEDED"},
	{LICENSE_INFO, (const char*)gLicenseNumber},
	{ALREADY_LICENSED, "ALREADY LICENSED"},
	{VERSION_NUMBER, (const char*)gProductVersion},
	{FAILED, "FAILED"},
#if(USE_DETAILED_RESPONSE != 0)
	{LIST_FULL, "LIST IS FULL"},
	{OPERATOR_EXISTS, "EXISTS"},
	{DOESNOT_EXIST, "NOT IN LIST"},
	{CANNOT_REMOVE, "CANNOT REMOVE PRIMARY USER"},
	{NOT_AUTHERISED, "NOT AUTHERISED"},
	{ALREADY_PRIMARY, "ALREADY PRIMARY"},
	{NOT_LICENSED, "NOT LICENSED"},
	{INVALID_COMMAND, "INVALID COMMAND"},
	{INVALID_USER, "INVALID USER "},
	{TIMEOUT, "TIMED OUT. TRY AGAIN."},
#endif //USE_DETAILED_RESPONSE
};

const Struct_Switch_Config SWITCHES[] =
{
	{ALL_SWITCH, 0, (uint8_t*)"ALL"},
	{DEFAULT_SWITCH, 1, (uint8_t*)gSwitchOneName},
	{SECOND_SWITCH, 2, (uint8_t*)gSwitchTwoName},
};

const uint8_t totalNumberOfCommands = (sizeof(COMMANDS)/sizeof(Struct_Data_Format));
const uint8_t totalNumberOfStatusCodes = (sizeof(STATUS_CODE)/sizeof(Struct_Data_Format));
const uint8_t totalNumberOfSwitches = (sizeof(SWITCHES)/sizeof(Struct_Switch_Config));


/*************************************************************************************************
 * Function Defintions
 *************************************************************************************************/
/*
 * @name   	licenseCommand()
 * @brief	This function will verifies whether the command is a license command
 * @param  	None
 * @retval	0x00 - If the command is license command
 *			0xFF - If the command is not a license command
 * @note
 */
uint8_t licenseCommand()
{
	uint8_t retVal = 0xFF;
	uint8_t cmdLine = 0;

	while((cmdLine < totalNumberOfCommands) && (compareStrings((const char*)gCommand, COMMANDS[cmdLine].data) != 0))
		cmdLine++;

	if((cmdLine < totalNumberOfCommands) && (COMMANDS[cmdLine].id == SET_LICENSE))
		retVal = 0x00;

	return retVal;
}

/*
 * @name   	licenseCommand()
 * @brief	This function will compares the strings which are passed as arguments
 * @param  	const char* - string 1
 *			const char* - string 2
 * @retval	0 - If passed strings matches
 * @note	strncmp could have been used where ever needed. But line in the code file would have been increased
 */
uint8_t compareStrings(const char* str1, const char* str2)
{
	return (strncmp(str1, str2, strlen(str2)));
}

/*
 * @name   	processCommand()
 * @brief	This function will compares command with the COMMANDS[] and takes the necessary action
 * @param  	none
 * @retval	uint8_t - Status code from STATUS_CODE[]
 * @note
 */
uint8_t processCommand()
{
	uint8_t retVal;
	uint8_t cmdLine;
	uint8_t switchLine;
	uint8_t error;
	uint8_t loopCount;

	cmdLine = 0;
	error = 0;
	while((cmdLine < totalNumberOfCommands) && (compareStrings((const char*)gCommand, COMMANDS[cmdLine].data) != 0))
		cmdLine++;

	if(cmdLine < totalNumberOfCommands)
	{
		retVal = SUCCESSFUL;
		switch(COMMANDS[cmdLine].id)
		{
			case SWITCH_ON:
			case SWITCH_OFF:
			case GET_SWITCHSTATE:
					if(gCommandLength > (strlen((const char*)COMMANDS[cmdLine].data)))	//Check if gCommand has extra bytes!
					{
#if(USE_GSM_MODULE > 0)
						GSM_ExtractArguement(strlen((const char*)COMMANDS[cmdLine].data));
#endif	//USE_GSM_MODULE
						switchLine = 0;
						while(switchLine<totalNumberOfSwitches)
						{
							if(((strlen((const char*)SWITCHES[switchLine].name) > 0) && (strlen((const char*) gArguement) > 0) \
									/*(strlen((const char*)SWITCHES[switchLine].name) == strlen((const char*) gArguement))*/ \
								&& (compareStrings((const char*)gArguement, (const char*)SWITCHES[switchLine].name) == 0))	\
								|| (SWITCHES[switchLine].num == (gArguement[0] - '0')))
							{
								break;
							}
							switchLine++;
						}
						if(switchLine<totalNumberOfSwitches)
						{
							if(COMMANDS[cmdLine].id == SWITCH_ON)
								turnON(SWITCHES[switchLine].whichSwitch);
							else if(COMMANDS[cmdLine].id == SWITCH_OFF)
								turnOFF(SWITCHES[switchLine].whichSwitch);
							else
							{
								if(SWITCHES[switchLine].whichSwitch > ALL_SWITCH)
								{
									if(getStatus(SWITCHES[switchLine].whichSwitch) > 0)
										retVal = STAUTS_ON;
									else
										retVal = STATUS_OFF;
								}
								else
									error = 1;
							}
						}
						else	//If the name or number of switches is greater than supported, then turn on/off ALL
							error = 1;
					}
					else	//If switch number or name is not mentioned then it will be invalid gCommand
						error = 1;
				break;

			case TOGGLE_DEFAULT_SWITCH:
					retVal = toggleDefaultSwitch();
				break;

			case ACK_OFF:
					gServiceAcknowledgement = 0;
					updateEEPROM(ACKNOWLEDGEMENT_NEEDED, &gServiceAcknowledgement);
				break;

			case ACK_ON:
					gServiceAcknowledgement = 1;
					updateEEPROM(ACKNOWLEDGEMENT_NEEDED, &gServiceAcknowledgement);
				break;
#if(USE_GSM_MODULE != 0)
			case MISSED_CALL_OFF:
					gMissedCallFeature = 0;
					updateEEPROM(MISSED_CALL_FEATURE, &gMissedCallFeature);
				break;

			case MISSED_CALL_ON:
					gMissedCallFeature = 1;
					updateEEPROM(MISSED_CALL_FEATURE, &gMissedCallFeature);
				break;
#endif //USE_GSM_MODULE
			case ADD_OPERATOR:
					if(gCommandLength > (strlen((const char*)COMMANDS[cmdLine].data)))	//Check if gCommand has extra bytes!
					{
						if(gFlagLicensingUser || gFlagPrimaryUser)
						{
#if(USE_GSM_MODULE > 0)
							GSM_ExtractArguement(strlen((const char*)COMMANDS[cmdLine].data));
							if(gArguement[0] != '+')
							{
								retVal = FAILED;
								break;
							}
#endif	//USE_GSM_MODULE
							if(((strlen((const char*)gPrimeUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gPrimeUser) == 0)) \
								|| ((strlen((const char*)gSecondUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gSecondUser) == 0)) \
								|| ((strlen((const char*)gThirdUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gThirdUser) == 0))
							  )
							{
#if(USE_DETAILED_RESPONSE != 0)
								retVal = OPERATOR_EXISTS;
#else	//USE_DETAILED_RESPONSE
								retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
							}
							else if((strlen((const char*)gSecondUser) > 0) && (strlen((const char*)gThirdUser) > 0))
							{
#if(USE_DETAILED_RESPONSE != 0)
								retVal = LIST_FULL;
#else	//USE_DETAILED_RESPONSE
								retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
							}
							else
							{
								if(strlen((const char*)gSecondUser) == 0)
								{
									//strncpy((char *)gSecondUser, (const char*)gArguement, strlen((const char*)gArguement));
									for(loopCount = 0; loopCount < strlen((const char*)gArguement); loopCount++)
										gSecondUser[loopCount] = gArguement[loopCount];
									gSecondUser[loopCount] = '\0';
									updateEEPROM(SECOND_OPERATOR, (uint8_t*)gSecondUser);
								}
								else	// copy to Third user!
								{
									//strncpy((char *)gThirdUser, (const char*)gArguement, strlen((const char*)gArguement));
									for(loopCount = 0; loopCount < strlen((const char*)gArguement); loopCount++)
										gThirdUser[loopCount] = gArguement[loopCount];
									gThirdUser[loopCount] = '\0';
									updateEEPROM(THIRD_OPERATOR, (uint8_t*)gThirdUser);
								}
							}
						}
						else
						{
#if(USE_DETAILED_RESPONSE != 0)
							retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
							retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
						}
					}
					else
						error = 1;
				break;

			case REMOVE_OPERATOR:
					if(gCommandLength > (strlen((const char*)COMMANDS[cmdLine].data)))	//Check if gCommand has extra bytes!
					{
						if(gFlagLicensingUser || gFlagPrimaryUser)
						{
#if(USE_GSM_MODULE > 0)
							GSM_ExtractArguement(strlen((const char*)COMMANDS[cmdLine].data));
#endif	//USE_GSM_MODULE
							if((strlen((const char*)gPrimeUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gPrimeUser) == 0))
							{
#if(USE_DETAILED_RESPONSE != 0)
								retVal = CANNOT_REMOVE;		//Cannot remove Primary Operator
#else	//USE_DETAILED_RESPONSE
								retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
							}
							else if((strlen((const char*)gSecondUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gSecondUser) == 0))
							{
								strcpy((char*)gSecondUser, "");
								updateEEPROM(SECOND_OPERATOR, (uint8_t*)gSecondUser);
							}
							else if((strlen((const char*)gThirdUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gThirdUser) == 0))
							{
								strcpy((char*)gThirdUser, "");
								updateEEPROM(THIRD_OPERATOR, (uint8_t*)gThirdUser);
							}
							else
							{
#if(USE_DETAILED_RESPONSE != 0)
								retVal = DOESNOT_EXIST;
#else	//USE_DETAILED_RESPONSE
								retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
							}
						}
						else
						{
#if(USE_DETAILED_RESPONSE != 0)
							retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
							retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
						}
					}
					else
						error = 1;
				break;

			case SET_PRIMARY_USER:
					if(gCommandLength > (strlen((const char*)COMMANDS[cmdLine].data)))	//Check if gCommand has extra bytes!
					{
						if(gFlagLicensingUser)
						{
#if(USE_GSM_MODULE > 0)
							GSM_ExtractArguement(strlen((const char*)COMMANDS[cmdLine].data));
							if(gArguement[0] != '+')
							{
								retVal = FAILED;
								break;
							}
#endif	//USE_GSM_MODULE
							if((strlen((const char*)gPrimeUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gPrimeUser) == 0))
							{
#if(USE_DETAILED_RESPONSE != 0)
								retVal = ALREADY_PRIMARY;
#endif	//USE_DETAILED_RESPONSE
							}
							else
							{
								//strncpy((char*)gPrimeUser, (const char*)gArguement, strlen((const char*)gArguement));
								for(loopCount = 0; loopCount < strlen((const char*)gArguement); loopCount++)
									gPrimeUser[loopCount] = gArguement[loopCount];
								gPrimeUser[loopCount] = '\0';

								updateEEPROM(PRIMARY_OPERATOR, (uint8_t*)gPrimeUser);
								//If operator is exist as second or thrid operator then remove that operator!
								if((strlen((const char*)gSecondUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gSecondUser) == 0))
								{
									strcpy((char*)gSecondUser, "");
									updateEEPROM(SECOND_OPERATOR, (uint8_t*)gSecondUser);
								}
								if((strlen((const char*)gThirdUser) > 0) && (compareStrings((const char*)gArguement, (const char*)gThirdUser) == 0))
								{
									strcpy((char*)gThirdUser, "");
									updateEEPROM(THIRD_OPERATOR, (uint8_t*)gThirdUser);
								}
							}
						}
						else
						{
#if(USE_DETAILED_RESPONSE != 0)
							retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
							retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
						}
					}
					else
						error  = 1;
				break;

			case REMOVE_ALL:
					if(gFlagLicensingUser)	// Licensing Users can remove all operators!
					{
						strcpy((char*)gPrimeUser, "");
						updateEEPROM(PRIMARY_OPERATOR, (uint8_t*)gPrimeUser);
					}
					else if(gFlagPrimaryUser)	//Primary User can only remove Second and Third Operator!
					{
						strcpy((char*)gSecondUser, "");
						updateEEPROM(SECOND_OPERATOR, (uint8_t*)gSecondUser);
						strcpy((char*)gThirdUser, "");
						updateEEPROM(THIRD_OPERATOR, (uint8_t*)gThirdUser);
					}
					else	// Others don't have any permission to remove any operator!
					{
#if(USE_DETAILED_RESPONSE != 0)
						retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
						retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
					}

				break;

			case SET_LICENSE:
					if(gCommandLength > (strlen((const char*)COMMANDS[cmdLine].data)))	//Check if gCommand has extra bytes!
					{
						if(gFlagLicensingUser)
						{
							if(!gDeviceLicensed)
							{
								gDeviceLicensed = 1;
#if(USE_GSM_MODULE > 0)
								GSM_ExtractArguement(strlen((const char*)COMMANDS[cmdLine].data));

#endif	//USE_GSM_MODULE
								//print("<L: %s>", gArguement);
								updateEEPROM(DEVICE_LICENSED, &gDeviceLicensed);
								updateEEPROM(LICENSE_NUMBER, gArguement);
								strncpy((char *)gLicenseNumber, (const char*)gArguement, strlen((const char*)gArguement));
							}
							else
							{
								retVal = ALREADY_LICENSED;
							}
						}
						else
						{
#if(USE_DETAILED_RESPONSE != 0)
							retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
							retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
						}
					}
					else
						error = 1;
				break;

			case GET_LICENSE:
					if(gFlagLicensingUser)
					{
						if(strlen((const char*)gLicenseNumber) > 0)
							retVal = LICENSE_INFO;
						else
							retVal = FAILED;
					}
					else
					{
#if(USE_DETAILED_RESPONSE != 0)
						retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
						retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
					}
				break;

			case GET_VERSION:
					if(gFlagLicensingUser)
						retVal = VERSION_NUMBER;
					else
					{
#if(USE_DETAILED_RESPONSE != 0)
						retVal = NOT_AUTHERISED;
#else	//USE_DETAILED_RESPONSE
						retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
					}
				break;

			default:
				break;
		}

		if(strlen((const char*)gArguement) > 0)
			free(gArguement);
	}
	else
	{
		error = 1;
	}

	if(error != 0)
	{
#if(USE_DETAILED_RESPONSE != 0)
		retVal = INVALID_COMMAND;
#else	//USE_DETAILED_RESPONSE
		retVal = FAILED;
#endif	//USE_DETAILED_RESPONSE
	}

	return retVal;
}
