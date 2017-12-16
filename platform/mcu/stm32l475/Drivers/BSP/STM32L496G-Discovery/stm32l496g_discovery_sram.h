/**
  ******************************************************************************
  * @file    stm32l496g_discovery_sram.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l496g_discovery_sram.c driver.
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

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L496G_DISCOVERY_SRAM_H
#define __STM32L496G_DISCOVERY_SRAM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/** @addtogroup STM32L496G_DISCOVERY_SRAM
  * @{
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_SRAM_Exported_Constants Exported Constants
  * @{
  */

/**
  * @brief  SRAM status structure definition
  */
#define SRAM_OK         0x00
#define SRAM_ERROR      0x01

#define SRAM_DEVICE_ADDR  ((uint32_t)0x64000000)
#define SRAM_DEVICE_SIZE  ((uint32_t)0x80000)  /* SRAM device size in MBytes */

/* #define SRAM_MEMORY_WIDTH    FMC_NORSRAM_MEM_BUS_WIDTH_8  */
#define SRAM_MEMORY_WIDTH    FMC_NORSRAM_MEM_BUS_WIDTH_16

#define SRAM_BURSTACCESS    FMC_BURST_ACCESS_MODE_DISABLE
/* #define SRAM_BURSTACCESS    FMC_BURST_ACCESS_MODE_ENABLE*/

#define SRAM_WRITEBURST    FMC_WRITE_BURST_DISABLE
/* #define SRAM_WRITEBURST   FMC_WRITE_BURST_ENABLE */

/* DMA definitions for SRAM DMA transfer */
#define SRAM_DMAx_CLK_ENABLE              __HAL_RCC_DMA1_CLK_ENABLE
#define SRAM_DMAx_CHANNEL                 DMA1_Channel1
#define SRAM_DMAx_IRQn                    DMA1_Channel1_IRQn
#define SRAM_DMAx_IRQHandler              DMA1_Channel1_IRQHandler

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/

/** @addtogroup STM32L496G_DISCOVERY_SRAM_Exported_Functions
  * @{
  */
uint8_t BSP_SRAM_Init(void);
uint8_t BSP_SRAM_ReadData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t BSP_SRAM_ReadData_DMA(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t BSP_SRAM_WriteData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t BSP_SRAM_WriteData_DMA(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
void BSP_SRAM_DMA_IRQHandler(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L496G_DISCOVERY_SRAM_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
