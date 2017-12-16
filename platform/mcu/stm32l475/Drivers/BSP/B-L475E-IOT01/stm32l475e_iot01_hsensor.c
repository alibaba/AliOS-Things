/**
  ******************************************************************************
  * @file    stm32l475e_iot01_hsensor.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-March-2017
  * @brief   This file provides a set of functions needed to manage the humidity sensor
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_hsensor.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_HUMIDITY HUMIDITY
  * @{
  */

/** @defgroup STM32L475E_IOT01_HUMIDITY_Private_Variables HUMIDITY Private Variables 
  * @{
  */ 
static HSENSOR_DrvTypeDef *Hsensor_drv;  
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_HUMIDITY_Private_Functions HUMIDITY Private Functions
  * @{
  */ 

/**
  * @brief  Initializes peripherals used by the I2C Humidity Sensor driver.
  * @retval HSENSOR status
  */
uint32_t BSP_HSENSOR_Init(void)
{
  uint32_t ret;
  
  if(HTS221_H_Drv.ReadID(HTS221_I2C_ADDRESS) != HTS221_WHO_AM_I_VAL)
  {
    ret = HSENSOR_ERROR;
  }
  else
  {
    Hsensor_drv = &HTS221_H_Drv;
    /* HSENSOR Init */   
    Hsensor_drv->Init(HTS221_I2C_ADDRESS);
    ret = HSENSOR_OK;
  }
  
  return ret;
}

/**
  * @brief  Read ID of HTS221.
  * @retval HTS221 ID value.
  */
uint8_t BSP_HSENSOR_ReadID(void)
{ 
  return Hsensor_drv->ReadID(HTS221_I2C_ADDRESS);
}

/**
  * @brief  Read Humidity register of HTS221.
  * @retval HTS221 measured humidity value.
  */
float BSP_HSENSOR_ReadHumidity(void)
{ 
  return Hsensor_drv->ReadHumidity(HTS221_I2C_ADDRESS);
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
