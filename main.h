/**
  ******************************************************************************
  * @file    main.h
  * @author  Basavaraju B V
  * @version V1.0.0
  * @date    03-Oct-2014
  * @brief   This file contains the following things:
                + #defines to including features
  ******************************************************************************
  *
  ******************************************************************************
  */

/*******************************************************************************
    Include Header files
*******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "wireless_control_config.h"
#include "printf_code.h"
#include "scanf_code.h"
#include "atmega328p_usart.h"
#include "eeprom_storage.h"
#include "take_action.h"
 #if(USE_GSM_MODULE != 0)
#include "gsm_module.h"
 #endif // USE_GSM_MODULE
