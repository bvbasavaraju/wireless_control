/**
  ******************************************************************************
  * @file    take_action.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    25-June-2015
  * @brief   This file will trigger the switches to ON or OFF
  ******************************************************************************
  */

/*************************************************************************************************
 * #includes
 *************************************************************************************************/ 
#include "take_action.h"
#include "atmega328p_gpio.h"

/*************************************************************************************************
 * Global Variables and definition
 *************************************************************************************************/ 
uint8_t gDefaultSwitchState = 0x00;	// PortD pin2 == switch 1 == default switch
uint8_t gSecondSwitchState = 0x00;	// PortD pin3 == switch 2

/*************************************************************************************************
 * Function Definitions
 *************************************************************************************************/
/*
 * @name   	updateSwitches()
 * @brief	This function will trigger switches based on EEPROM Stored data
 * @param  	None
 * @retval	None
 * @note	When Device gets initialized updateSwitches function will be called, to trigger the switch to ON or OFF
 */
void updateSwitches()
{
	if(gDefaultSwitchState)
        GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_SET);
    if(gSecondSwitchState)
		GPIO_Write(GPIOD, PIN_THREE, GPIO_PIN_SET);
}

/*
 * @name   	toggleDefaultSwitch()
 * @brief	This function will toggle the Default switch
 * @param  	None
 * @retval	uin8_t - SWITCHED OFF or ON information
 * @note	When Missed call is given by a autherised user the default switch will toggle its switch state
 */
uint8_t toggleDefaultSwitch()
{
    uint8_t retVal = SUCCESSFUL;
	if(gDefaultSwitchState)
    {
        GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_RESET);
		gDefaultSwitchState = 0x00;
		retVal = SUCCESSFULLY_SWITCHED_OFF;
    }
    else
    {
        GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_SET);
		gDefaultSwitchState = 0x01;
		retVal = SUCCESSFULLY_SWITCHED_ON;
    }
	updateEEPROM(SWITCH_1_STATE, &gDefaultSwitchState);
	return retVal;
}

/*
 * @name   	turnON()
 * @brief	This function will turn ON the switch which is passed on as the arguement
 * @param  	uint8_t - Switch, which has to be turned ON
 * @retval	None
 * @note	Once the switch state is changed the data will be stored into EEPROM
 */
void turnON(uint8_t whichOne)
{
    switch(whichOne)
    {
		case ALL_SWITCH:
            GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_SET);
			gDefaultSwitchState = 0x01;
            GPIO_Write(GPIOD, PIN_THREE, GPIO_PIN_SET);
			gSecondSwitchState = 0x01;
			updateEEPROM(SWITCH_1_STATE, &gDefaultSwitchState);
			updateEEPROM(SWITCH_2_STATE, &gSecondSwitchState);
            break;

        case DEFAULT_SWITCH:
            GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_SET);
			gDefaultSwitchState = 0x01;
			updateEEPROM(SWITCH_1_STATE, &gDefaultSwitchState);
            break;
        case SECOND_SWITCH:
            GPIO_Write(GPIOD, PIN_THREE, GPIO_PIN_SET);
			gSecondSwitchState = 0x01;
			updateEEPROM(SWITCH_2_STATE, &gSecondSwitchState);
            break;

        default:
            break;
    }
}

/*
 * @name   	turnOFF()
 * @brief	This function will turn OFF the switch which is passed on as the arguement
 * @param  	uint8_t - Switch, which has to be turned OFF
 * @retval	None
 * @note	Once the switch state is changed the data will be stored into EEPROM
 */
void turnOFF(uint8_t whichOne)
{
    switch(whichOne)
    {
		case ALL_SWITCH:
            GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_RESET);
			gDefaultSwitchState = 0x00;
            GPIO_Write(GPIOD, PIN_THREE, GPIO_PIN_RESET);
			gSecondSwitchState = 0x00;
			updateEEPROM(SWITCH_1_STATE, &gDefaultSwitchState);
			updateEEPROM(SWITCH_2_STATE, &gSecondSwitchState);
            break;

        case DEFAULT_SWITCH:
            GPIO_Write(GPIOD, PIN_TWO, GPIO_PIN_RESET);
			gDefaultSwitchState = 0x00;
			updateEEPROM(SWITCH_1_STATE, &gDefaultSwitchState);
            break;
        case SECOND_SWITCH:
            GPIO_Write(GPIOD, PIN_THREE, GPIO_PIN_RESET);
			gSecondSwitchState = 0x00;
			updateEEPROM(SWITCH_2_STATE, &gSecondSwitchState);
            break;

        default:
            break;
    }
}

/*
 * @name   	getStatus()
 * @brief	This function will returns the status of the switch
 * @param  	uint8_t - Switch, whose status is requested
 * @retval	uint8_t - Status of the switch
 * @note	
 */
uint8_t getStatus(uint8_t whichOne)
{
	uint8_t retVal = 0;

	switch(whichOne)
	{
		case DEFAULT_SWITCH:
			retVal = gDefaultSwitchState;
            break;

        case SECOND_SWITCH:
			retVal = gSecondSwitchState;
            break;

        default:
            break;
	}

	return retVal;
}
