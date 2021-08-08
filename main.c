/**
  ******************************************************************************
  * @file    main.c
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    22-June-2015
  * @brief   This file will initialize the U(S)ART and GSM Module.
  ******************************************************************************
  */

/*************************************************************************************************
 * #includes
 *************************************************************************************************/

#include "main.h"

/*************************************************************************************************
 * Global variables and definitions
 *************************************************************************************************/
 /*
  * Hardware version: <Processing_Unit>_<Communication_Hardware>_<RELAY>_<Audio_Module>_<Auto_Switch>
  *					  ex: Product_Unit => nth hardware change of Aurduino, Communication_Hardware => nth hardware change of GSM module
  *
  * Software version: <Major_Release>_<Minor_Release>_<Bug_Fix>
  */
const uint8_t gProductVersion[] = "HWV_1_1_1_0_0; SWV_V_3_5_3";

/*************************************************************************************************
 * Function Definition
 *************************************************************************************************/
int main(void)
{
#if (USE_USART_DRIVER > 0)
	//Driver part
	USART_StructureType USART_Config;

    //USART_Config.USART_BaudRate = 19200;
    USART_Config.USART_BaudRate = 9600;
    USART_Config.USART_Communication = BOTH;
    USART_Config.USART_DataBits = EIGHT;
    USART_Config.USART_Modes = ASYNCHRONOUS;
    USART_Config.USART_Parity = NOPARITY;
    USART_Config.USART_StopBits = ONESTOPBIT;

    USARTInit(USART_Config);

    USART_EnableInterrupt(RECEIVE);
 #if(USE_GSM_MODULE != 0)

	while(GSM_TestForResponse())	//Wait for Network to get acquired!
		;

	GSM_SetEchoOFF();
	GSM_SetupForSMS();

	initializeDevice();

	//Enable for testing Licensing!
	//gDeviceLicensed = 0;
	//updateEEPROM(DEVICE_LICENSED, &gDeviceLicensed);

    GSM_WaitAndProcessRequest();
 #endif // USE_GSM_MODULE

#endif //USE_USART_DRIVER
    return 0;
}
