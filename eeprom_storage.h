/**
  ******************************************************************************
  * @file    eeprom_storage.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    26-June-2015
  * @brief   This file defines EEPROM usage details and functions to intialize the necessary variable and update the EEPROM.
  ******************************************************************************
  */
#ifndef _EEPROM_STORAGE_
#define _EEPROM_STORAGE_

/*************************************************************************************************
 * #includes
 *************************************************************************************************/
#include <avr/eeprom.h>
#include "commands.h"
#include "gsm_module.h"
#include "take_action.h"

/*************************************************************************************************
 * #defines
 *************************************************************************************************/
#define	DEVICE_LICENSED			0
#define LICENSE_NUMBER			1
#define	LANGUAGE_SELECTED		2
#define	PRIMARY_OPERATOR		3
#define	SECOND_OPERATOR			4
#define THIRD_OPERATOR			5
#define ACKNOWLEDGEMENT_NEEDED	6
#define	MISSED_CALL_FEATURE		7
#define SWITCH_1_STATE			8
#define SWITCH_2_STATE			9

/*************************************************************************************************
 * Structure Definitions
 *************************************************************************************************/
typedef struct
{
	uint8_t	variable;
	uint8_t varSize;
	uint8_t startAddress;
	uint8_t endAddress;
}Structure_EEPROM_Layout;

/*************************************************************************************************
 * Exported Functions
 *************************************************************************************************/
void initializeDevice();
void updateEEPROM(uint8_t, uint8_t*);
#endif // _EEPROM_STORAGE
