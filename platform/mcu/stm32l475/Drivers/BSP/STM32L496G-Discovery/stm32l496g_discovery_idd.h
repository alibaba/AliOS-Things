/**
 ******************************************************************************
 * @file    stm32l496g_discovery_idd.h
 * @author  MCD Application Team
 * @brief   Header file for stm32l496g_discovery_idd.c module.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L496G_DISCOVERY_IDD_H
#define __STM32L496G_DISCOVERY_IDD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery.h"
/* Include Idd measurement component driver */
#include "../Components/mfxstm32l152/mfxstm32l152.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY_IDD
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_IDD_Exported_Types  Exported Types
  * @{
  */

/** @defgroup IDD_Config  IDD Config
  * @{
  */
typedef enum
{
  IDD_OK = 0,
  IDD_TIMEOUT = 1,
  IDD_ZERO_VALUE = 2,
  IDD_ERROR = 0xFF
}
IDD_StatusTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_IDD_Exported_Defines  Exported Defines
  * @{
  */
/**
  * @brief  Shunt values on discovery in milli ohms
  */
#define DISCOVERY_IDD_SHUNT0_VALUE                  ((uint16_t) 1000)     /*!< value in milliohm */
#define DISCOVERY_IDD_SHUNT1_VALUE                  ((uint16_t) 24)       /*!< value in ohm */
#define DISCOVERY_IDD_SHUNT2_VALUE                  ((uint16_t) 620)      /*!< value in ohm */
#define DISCOVERY_IDD_SHUNT4_VALUE                  ((uint16_t) 10000)    /*!< value in ohm */

/**
  * @brief  Shunt stabilization delay on discovery in milli ohms
  */
#define DISCOVERY_IDD_SHUNT0_STABDELAY              ((uint8_t) 149)       /*!< value in millisec */
#define DISCOVERY_IDD_SHUNT1_STABDELAY              ((uint8_t) 149)       /*!< value in millisec */
#define DISCOVERY_IDD_SHUNT2_STABDELAY              ((uint8_t) 149)       /*!< value in millisec */
#define DISCOVERY_IDD_SHUNT4_STABDELAY              ((uint8_t) 255)       /*!< value in millisec */

/**
  * @brief  IDD Ampli Gain on discovery
  */
#define DISCOVERY_IDD_AMPLI_GAIN                    ((uint16_t) 4967)     /*!< value is gain * 100 */

/**
  * @brief  IDD Vdd Min on discovery
  */
#define DISCOVERY_IDD_VDD_MIN                       ((uint16_t) 2000)     /*!< value in millivolt */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_IDD_Exported_Functions  Exported Functions
  * @{
  */
uint8_t   BSP_IDD_Init(void);
void      BSP_IDD_DeInit(void);
void      BSP_IDD_Reset(void);
void      BSP_IDD_LowPower(void);
void      BSP_IDD_WakeUp(void);
void      BSP_IDD_StartMeasure(void);
void      BSP_IDD_Config(IDD_ConfigTypeDef IddConfig);
void      BSP_IDD_GetValue(uint32_t *IddValue);
void      BSP_IDD_EnableIT(void);
void      BSP_IDD_ClearIT(void);
uint8_t   BSP_IDD_GetITStatus(void);
void      BSP_IDD_DisableIT(void);
uint8_t   BSP_IDD_ErrorGetCode(void);
void      BSP_IDD_ErrorEnableIT(void);
void      BSP_IDD_ErrorClearIT(void);
uint8_t   BSP_IDD_ErrorGetITStatus(void);
void      BSP_IDD_ErrorDisableIT(void);

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

#ifdef __cplusplus
}
#endif

#endif /* __STM32L496G_DISCOVERY_IDD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
