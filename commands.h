/**
  ******************************************************************************
  * @file    commands.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    26-June-2015
  * @brief   This is a header file for commands.c
  ******************************************************************************
  */

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/*************************************************************************************************
 * #includes
 *************************************************************************************************/
#include "take_action.h"
#include "wireless_control_config.h"
#include "atmega328p_usart.h"
#include "printf_code.h"
#include "take_action.h"
#include "gsm_module.h"
#include "eeprom_storage.h"

/*************************************************************************************************
 * #defines
 *************************************************************************************************/
//Operations Commands #defines
#define SWITCH_ON				0x01
#define SWITCH_OFF				0x02
#define GET_SWITCHSTATE			0x03
#define TOGGLE_DEFAULT_SWITCH	0x04
//Cofign commands #defines
#define	ACK_ON					0x20
#define ACK_OFF					0x21
 #if(USE_GSM_MODULE != 0)
#define	MISSED_CALL_OFF			0x22
#define	MISSED_CALL_ON			0x23
 #endif	//USE_GSM_MODULE
//Operators related commands
#define ADD_OPERATOR			0x40
#define	REMOVE_OPERATOR			0x41
#define SET_PRIMARY_USER		0x42
#define REMOVE_ALL				0x43
//Lincese Command
#define	SET_LICENSE				0xF0
#define GET_LICENSE				0xF1
#define GET_VERSION				0xFF

//Status Codes
#define SUCCESSFUL			        0x00
#define DEVICE_ON			        0x01
#define STAUTS_ON			        0x02	//Switch Status ON
#define STATUS_OFF			        0x03	//Switch Status OFF
#define SUCCESSFULLY_SWITCHED_OFF   0x04
#define SUCCESSFULLY_SWITCHED_ON    0x05
#define SERVICE_NEEDED		        0x80
#define LICENSE_INFO		        0xA0
#define ALREADY_LICENSED	        0xA1
#define VERSION_NUMBER		        0xB0
#define FAILED				        0xFF
 #if(USE_DETAILED_RESPONSE != 0)
#define LIST_FULL					0x40
#define OPERATOR_EXISTS				0x41
#define DOESNOT_EXIST				0x42	//not in list
#define CANNOT_REMOVE				0x43	//cannot remove primay user
#define NOT_AUTHERISED				0x44	//Not a primery user or licesing people
#define	ALREADY_PRIMARY				0x45
#define NOT_LICENSED				0xF0
#define	INVALID_COMMAND				0xF1
#define INVALID_USER				0xF2
#define TIMEOUT						0xF3
 #endif	//USE_DETAILED_RESPONSE


/*************************************************************************************************
 * Strcuture Definitions
 *************************************************************************************************/
typedef struct
{
	uint8_t id;			//Command Id or status code
	const char* data;	//Command name or status message
}Struct_Data_Format;

typedef struct
{
	uint8_t whichSwitch;
	uint8_t num;
	uint8_t* name;
}Struct_Switch_Config;

/*************************************************************************************************
 * Exported/Imported Variables
 *************************************************************************************************/
extern const Struct_Data_Format STATUS_CODE[];
extern const uint8_t totalNumberOfStatusCodes;
extern const uint8_t gProductVersion[];
extern uint8_t gServiceAcknowledgement; 	// 1 -> Every request will be acknowledged
extern uint8_t gDeviceLicensed;

extern char gPrimeUser[20];
extern char gSecondUser[20];
extern char gThirdUser[20];
extern char gLicenseNumber[13];

extern uint8_t *gCommand;
extern uint8_t *gArguement;
 #if(USE_GSM_MODULE != 0)
extern uint8_t gMissedCallFeature;
 #endif	//USE_GSM_MODULE

/*************************************************************************************************
 * Exported Functions
 *************************************************************************************************/
uint8_t processCommand();
uint8_t licenseCommand();
uint8_t compareStrings(const char*, const char*);

#endif	//_COMMANDS_H_
