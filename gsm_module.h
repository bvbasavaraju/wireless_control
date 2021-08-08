/**
  ******************************************************************************
  * @file    gsm_module.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   This file is the header file for gsm_module.c
  ******************************************************************************
  */

#ifndef _GSM_MODULE_
#define _GSM_MODULE_

/*************************************************************************************************
 * #includes
 *************************************************************************************************/ 
#include<stdio.h>
#include<string.h>
#include <util/delay.h>
#include "atmega328p_usart.h"
#include "printf_code.h"
#include "wireless_control_config.h"
#include "commands.h"
#include "eeprom_storage.h"

    #if(USE_GSM_MODULE != 0)
/*************************************************************************************************
 * ENUM Definition
 *************************************************************************************************/ 
typedef enum
{
	GSM_IDLE            = 0x00,
	GSM_VOICE_CALL      = 0x01,
	GSM_MISSED_CALL     = 0x02,
	GSM_READ_MESSAGE    = 0x03,
	GSM_WRITE_MESSAGE   = 0x04,
}eGSM_States;

/*************************************************************************************************
 * Exported variables
 *************************************************************************************************/ 
extern uint8_t gFlagLicensingUser;
extern uint8_t gFlagPrimaryUser;
extern uint8_t gCommandLength;

/*************************************************************************************************
 * Exported Function
 *************************************************************************************************/ 
void GSM_SetPrimayUser(const char*);
void GSM_WaitAndProcessRequest();
void GSM_ExtractArguement(uint8_t length);
uint8_t GSM_TestForResponse();
uint8_t GSM_SetEchoOFF();
uint8_t GSM_SetupForSMS();

	#endif	//USE_GSM_MODULE

#endif // _GSM_MODULE_
