/**
  ******************************************************************************
  * @file    eeprom_storage.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    26-June-2015
  * @brief   This file defines EEPROM usage details and functions to intialize the necessary variable and update the EEPROM.
  ******************************************************************************
  */

/*************************************************************************************************
 * #includes
 *************************************************************************************************/
#include "eeprom_storage.h"

/*************************************************************************************************
 * Golabal Varibales and defintion
 *************************************************************************************************/
const Structure_EEPROM_Layout EEPROM_Layout_Details[] =
{
	{DEVICE_LICENSED, 1, 0, 0},
	{LICENSE_NUMBER, 13, 1, 13},
	{LANGUAGE_SELECTED, 1, 14, 14},
	{PRIMARY_OPERATOR, 20, 15, 34},
	{SECOND_OPERATOR, 20, 35, 54},
	{THIRD_OPERATOR, 20, 55, 74},
	{ACKNOWLEDGEMENT_NEEDED, 1, 75, 75},
	{MISSED_CALL_FEATURE, 1, 76, 76},
	{SWITCH_1_STATE, 1, 77, 77},
	{SWITCH_2_STATE, 1, 78, 78},
};

static const uint8_t totalVariables = sizeof(EEPROM_Layout_Details)/sizeof(Structure_EEPROM_Layout);

/*************************************************************************************************
 * Function Definitions
 *************************************************************************************************/
/*
 * @name   	initializeDevice()
 * @brief	This function will intializes the all the necessary variables
 * @param  	None
 * @retval	None
 * @note	After Initializing the variables from EEPROM updateSwitches() will be called to retain the status of the switches.
 */
void initializeDevice()
{
	uint8_t i;
	uint8_t k;
	uint8_t data;
	uint8_t exitLoop = 0;
	int j;

	eeprom_busy_wait();	//Wait for EEPROM is ready to use

	for(i = 0; i<totalVariables; i++)
	{
		exitLoop = 0;
		for(j=EEPROM_Layout_Details[i].startAddress, k=0; j<=EEPROM_Layout_Details[i].endAddress; k++,j++)
		{
			data = eeprom_read_byte((uint8_t*)j);
			switch(EEPROM_Layout_Details[i].variable)
			{
				case DEVICE_LICENSED:
						if((data == 0) || (data == 1))
							gDeviceLicensed = data;
					break;

				case LICENSE_NUMBER:
						if(data == '\0')
							exitLoop = 1;
						gLicenseNumber[k] = data;
					break;

				case LANGUAGE_SELECTED:
						//languageSelected = data;
					break;

				case PRIMARY_OPERATOR:
						if(data == '\0')
							exitLoop = 1;
						gPrimeUser[k] = data;
					break;

				case SECOND_OPERATOR:
						if(data == '\0')
							exitLoop = 1;
						gSecondUser[k] = data;
					break;

				case THIRD_OPERATOR:
						if(data == '\0')
							exitLoop = 1;
						gThirdUser[k] = data;
					break;

				case ACKNOWLEDGEMENT_NEEDED:
						gServiceAcknowledgement = data;
					break;

				case MISSED_CALL_FEATURE:
						gMissedCallFeature = data;
					break;

				case SWITCH_1_STATE:
						gDefaultSwitchState = data;
					break;

				case SWITCH_2_STATE:
						gSecondSwitchState = data;
					break;

				default:
					break;
			}
			if(exitLoop)
				break;
		}
	}
	updateSwitches();
}

/*
 * @name   	updateEEPROM()
 * @brief	This function will update the EEPROM variables
 * @param  	uint8_t - variable whose data has to be updated in the EEPROM
 *			uint8_t* - address of the variable has to be updated to the EEPROM
 * @retval	None
 * @note	
 */
void updateEEPROM(uint8_t var, uint8_t *data)
{
	uint8_t i;
	uint8_t j;
	int k;

	for(i=0; i<totalVariables; i++)
	{
		if(EEPROM_Layout_Details[i].variable == var)
			break;
	}

	if(i<totalVariables)
	{
		if(EEPROM_Layout_Details[i].varSize > 1)
		{
			for(k = EEPROM_Layout_Details[i].startAddress, j = 0; k<=EEPROM_Layout_Details[i].endAddress; j++, k++)
			{
				if(data[j] != '\0')
					eeprom_write_byte((uint8_t*)k, (uint8_t)data[j]);
				else
				{
					eeprom_write_byte((uint8_t*)k, (uint8_t)data[j]);
					break;
				}
			}
		}
		else
		{
			k = EEPROM_Layout_Details[i].startAddress;
			eeprom_write_byte((uint8_t*)k, *data);
		}
	}
}
