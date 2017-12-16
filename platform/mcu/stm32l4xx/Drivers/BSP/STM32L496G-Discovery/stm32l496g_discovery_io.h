/**
  ******************************************************************************
  * @file    stm32l496g_discovery_io.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l496g_eval_io.c driver.
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
#ifndef __STM32L496G_DISCO_IO_H
#define __STM32L496G_DISCO_IO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery.h"
#include "../Components/mfxstm32l152/mfxstm32l152.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCO
  * @{
  */

/** @addtogroup STM32L496G_DISCO_IO
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup STM32L496G_DISCO_IO_Exported_Types Exported Types
  * @{
  */
typedef enum
{
  IO_OK       = 0x00,
  IO_ERROR    = 0x01,
  IO_TIMEOUT  = 0x02,
  IO_ALREADY_INITIALIZED  = 0x03
}
IO_StatusTypeDef;

/**
  * @}
  */

/** @defgroup STM32L496G_DISCO_IO_Exported_Constants Exported Constants
  * @{
  */
/* Virtual pin offset IOExpander1 */
#define IO1_PIN_OFFSET               0


/* Pins definition IOExpander */
#define IO1_PIN_0                     (uint32_t)(0x00000001 << IO1_PIN_OFFSET)
#define IO1_PIN_1                     (uint32_t)(0x00000002 << IO1_PIN_OFFSET)
#define IO1_PIN_2                     (uint32_t)(0x00000004 << IO1_PIN_OFFSET)
#define IO1_PIN_3                     (uint32_t)(0x00000008 << IO1_PIN_OFFSET)
#define IO1_PIN_4                     (uint32_t)(0x00000010 << IO1_PIN_OFFSET)
#define IO1_PIN_5                     (uint32_t)(0x00000020 << IO1_PIN_OFFSET)
#define IO1_PIN_6                     (uint32_t)(0x00000040 << IO1_PIN_OFFSET)
#define IO1_PIN_7                     (uint32_t)(0x00000080 << IO1_PIN_OFFSET)
#define IO1_PIN_8                     (uint32_t)(0x00000100 << IO1_PIN_OFFSET)
#define AGPIO_PIN_0                   (uint32_t)(0x00010000 << IO1_PIN_OFFSET)
#define AGPIO_PIN_1                   (uint32_t)(0x00020000 << IO1_PIN_OFFSET)

#define IO1_PIN_ALL                   (uint32_t)(0x000301FF << IO1_PIN_OFFSET)




/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup STM32L496G_DISCO_IO_Exported_Functions Exported Functions
  * @{
  */

uint8_t  BSP_IO_Init(void);
void     BSP_IO_ITClear(uint32_t IO_Pin);
uint32_t BSP_IO_ITGetStatus(uint32_t IO_Pin);
uint8_t  BSP_IO_ConfigPin(uint32_t IO_Pin, IO_ModeTypedef IO_Mode);
void     BSP_IO_WritePin(uint32_t IO_Pin, uint8_t PinState);
uint32_t BSP_IO_ReadPin(uint32_t IO_Pin);
void     BSP_IO_TogglePin(uint32_t IO_Pin);

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

#endif /* __STM32L496G_DISCO_IO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
