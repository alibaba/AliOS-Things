/**
  ******************************************************************************
  * @file    stm32l496g_discovery_sram.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to drive the
  *          IS66WV51216EBLL SRAM memory mounted on STM32L496G-Discovery board.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) This driver is used to drive the IS66WV51216EBLL-70BLI SRAM external memory mounted
       on STM32L496G-Discovery evaluation board.

   (#) This driver does not need a specific component driver for the SRAM device
       to be included with.

   (#) Initialization steps:
       (++) Initialize the SRAM external memory using the BSP_SRAM_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            FMC controller configuration to interface with the external SRAM memory.

   (#) SRAM read/write operations
       (++) SRAM external memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with AHB access using the functions
            BSP_SRAM_ReadData()/BSP_SRAM_WriteData(), or by DMA transfer using the functions
            BSP_SRAM_ReadData_DMA()/BSP_SRAM_WriteData_DMA().
       (++) The AHB access is performed with 16-bit width transaction, the DMA transfer
            configuration is fixed at single (no burst) halfword transfer
            (see the SRAM_MspInit() static function).
       (++) User can implement his own functions for read/write access with his desired
            configurations.
       (++) If interrupt mode is used for DMA transfer, the function BSP_SRAM_DMA_IRQHandler()
            is called in IRQ handler file, to serve the generated interrupt once the DMA
            transfer is complete.
  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery_sram.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_SRAM STM32L496G-DISCOVERY SRAM
  * @{
  */


/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_SRAM_Private_Variables Private Variables
  * @{
  */
static SRAM_HandleTypeDef sramHandle;
static FMC_NORSRAM_TimingTypeDef Timing;

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_SRAM_Private_Constants Private Constants
  * @{
  */
/* Timings for SRAM IS66WV51216EBLL-70BLI */
#define SRAM_ADDR_SETUP_TIME  5 /* 60ns with a clock at 80 MHz (period of 12.5 ns) */
#define SRAM_DATA_SETUP_TIME  3 /* 30ns with a clock at 80 MHz (period of 12.5 ns) */
#define SRAM_TURN_AROUND_TIME 1 /* 5ns with a clock at 80 MHz (perido of 12.5 ns) */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_SRAM_Private_Functions Private Functions
  * @{
  */
static void SRAM_MspInit(void);

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_SRAM_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Initializes the SRAM device.
  * @retval SRAM status
  */
uint8_t BSP_SRAM_Init(void)
{
  sramHandle.Instance  = FMC_NORSRAM_DEVICE;
  sramHandle.Extended  = FMC_NORSRAM_EXTENDED_DEVICE;

  /* SRAM device configuration */
  Timing.AddressSetupTime      = SRAM_ADDR_SETUP_TIME;
  Timing.DataSetupTime         = SRAM_DATA_SETUP_TIME;
  Timing.BusTurnAroundDuration = SRAM_TURN_AROUND_TIME;
  Timing.AccessMode            = FMC_ACCESS_MODE_A;

  sramHandle.Init.NSBank             = FMC_NORSRAM_BANK2;
  sramHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  sramHandle.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;
  sramHandle.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  sramHandle.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;
  sramHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  sramHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  sramHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  sramHandle.Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;
  sramHandle.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  sramHandle.Init.WriteFifo          = FMC_WRITE_FIFO_DISABLE;
  sramHandle.Init.PageSize           = FMC_PAGE_SIZE_NONE;

  /* SRAM controller initialization */
  SRAM_MspInit();
  if (HAL_SRAM_Init(&sramHandle, &Timing, &Timing) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Reads an amount of data from the SRAM device in polling mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of read data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_ReadData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if (HAL_SRAM_Read_16b(&sramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Reads an amount of data from the SRAM device in DMA mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of read data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_ReadData_DMA(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if (HAL_SRAM_Read_DMA(&sramHandle, (uint32_t *)uwStartAddress, (uint32_t *)pData, uwDataSize) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data from the SRAM device in polling mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of written data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_WriteData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if (HAL_SRAM_Write_16b(&sramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data from the SRAM device in DMA mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of written data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_WriteData_DMA(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if (HAL_SRAM_Write_DMA(&sramHandle, (uint32_t *)uwStartAddress, (uint32_t *)pData, uwDataSize) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Handles SRAM DMA transfer interrupt request.
  * @retval None
  */
void BSP_SRAM_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(sramHandle.hdma);
}

/** @addtogroup STM32L476G_EVAL_SRAM_Private_Functions
  * @{
  */

/**
  * @brief  Initializes SRAM MSP.
  * @retval None
  */
static void SRAM_MspInit(void)
{
  static DMA_HandleTypeDef dmaHandle;
  GPIO_InitTypeDef gpioinitstruct;
  SRAM_HandleTypeDef *hsram = &sramHandle;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable chosen DMAx clock */
  SRAM_DMAx_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /* Common GPIO configuration */
  gpioinitstruct.Mode      = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull      = GPIO_NOPULL;
  gpioinitstruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpioinitstruct.Alternate = GPIO_AF12_FMC;

  /*## Data Bus #######*/
  /* GPIOD configuration */
  gpioinitstruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                       GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* GPIOE configuration */
  gpioinitstruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                       GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpioinitstruct);

  /*## Address Bus #######*/
  /* GPIOD configuration */
  gpioinitstruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* GPIOF configuration */
  gpioinitstruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                       GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 |
                       GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &gpioinitstruct);

  /* GPIOG configuration */
  gpioinitstruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                       GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOG, &gpioinitstruct);

  /*## NOE and NWE configuration #######*/
  gpioinitstruct.Pull = GPIO_PULLUP;
  gpioinitstruct.Pin  = GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);
  HAL_Delay(1);

  /*## NBL0, NBL1 configuration #######*/
  gpioinitstruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  HAL_GPIO_Init(GPIOE, &gpioinitstruct);

  /*## NE configuration #######*/
  gpioinitstruct.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOG, &gpioinitstruct);

  /* Configure common DMA parameters */
  dmaHandle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  dmaHandle.Init.PeriphInc           = DMA_PINC_ENABLE;
  dmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  dmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  dmaHandle.Init.Mode                = DMA_NORMAL;
  dmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  dmaHandle.Instance = SRAM_DMAx_CHANNEL;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hsram, hdma, dmaHandle);

  /* Deinitialize the Stream for new transfer */
  HAL_DMA_DeInit(&dmaHandle);

  /* Configure the DMA Stream */
  HAL_DMA_Init(&dmaHandle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SRAM_DMAx_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(SRAM_DMAx_IRQn);
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
