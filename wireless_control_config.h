/**
  ******************************************************************************
  * @file    wireless_control_config.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    28-June-2015
  * @brief   This file defines which modules to be present in the product
  ******************************************************************************
  */
  
#ifndef _WIRELESS_CONTROL_CONFIG_H_
#define _WIRELESS_CONTROL_CONFIG_H_

/***************************************************************
Set to 1, if GPIO Driver is used
*/
#ifndef USE_GPIO_DRIVER
#define USE_GPIO_DRIVER  1
#endif // USE_GPIO_DRIVER

/***************************************************************
Set to 1, if USART Driver is used
*/
#ifndef USE_USART_DRIVER
#define USE_USART_DRIVER 1
#endif // USE_USART_DRIVER

/***************************************************************
Set to 1, if GSM module is used
*/

#ifndef USE_GSM_MODULE
#define USE_GSM_MODULE 	1
#endif	//USE_GSM_MODULE

/**************************************************************
USE_DETAILED_RESPONSE:
If it is set to 0 Only SUCCESS or FAILED will be acknowledged
If it is set to 1, response will be acknowledged in detial. Check, commands.h and commands.c file for details
*/
// set to 1 GSM Module is used
#ifndef USE_DETAILED_RESPONSE
#define USE_DETAILED_RESPONSE 	1
#endif	//USE_DETAILED_RESPONSE

#endif	//_WIRELESS_CONTROL_CONFIG_H_
