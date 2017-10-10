/**
  ******************************************************************************
  * @file    stm32l475e_iot01_tsensor.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-March-2017
  * @brief   This file provides a set of functions needed to manage the temperature sensor
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L475E_IOT01_TSENSOR_H
#define __STM32L475E_IOT01_TSENSOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01.h"
#ifdef USE_LPS22HB_TEMP
#include "../Components/lps22hb/lps22hb.h"
#else /* USE_HTS221_TEMP */
#include "../Components/hts221/hts221.h"
#endif

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @addtogroup STM32L475E_IOT01_TEMPERATURE 
  * @{
  */
   
/** @defgroup STM32L475E_IOT01_TEMPERATURE_Exported_Types TEMPERATURE Exported Types
  * @{
  */
   
/** 
  * @brief  TSENSOR Status  
  */ 
typedef enum
{
  TSENSOR_OK = 0,
  TSENSOR_ERROR
}TSENSOR_Status_TypDef;

/**
  * @}
  */


/** @defgroup STM32L475E_IOT01_TEMPERATURE_Exported_Functions TEMPERATURE Exported Constants
  * @{
  */
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Sensor Configuration Functions */
uint32_t BSP_TSENSOR_Init(void);
float BSP_TSENSOR_ReadTemp(void);
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#endif /* __STM32L475E_IOT01_TSENSOR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
