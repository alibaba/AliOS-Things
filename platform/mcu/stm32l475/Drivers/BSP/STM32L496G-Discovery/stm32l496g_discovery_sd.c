/**
  ******************************************************************************
  * @file    stm32l496g_discovery_sd.c
  * @author  MCD Application Team
  * @brief   This file includes the uSD card driver.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  (#) This driver is used to drive the micro SD external card mounted on STM32L496G-DISCOVERY
     evaluation board.

  (#) This driver does not need a specific component driver for the micro SD device
     to be included with.

  (#) Initialization steps:
       (++) Initialize the micro SD card using the BSP_SD_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            SDMMC1 interface configuration to interface with the external micro SD. It
            also includes the micro SD initialization sequence.
       (++) To check the SD card presence you can use the function BSP_SD_IsDetected() which
            returns the detection status.
       (++) The function BSP_SD_GetCardInfo() is used to get the micro SD card information
            which is stored in the structure "HAL_SD_CardInfoTypedef".

  (#) Micro SD card operations
       (++) The micro SD card can be accessed with read/write block(s) operations once
            it is reay for access. The access cand be performed whether using the polling
            mode by calling the functions BSP_SD_ReadBlocks()/BSP_SD_WriteBlocks(), or by DMA
            transfer using the functions BSP_SD_ReadBlocks_DMA()/BSP_SD_WriteBlocks_DMA()
       (++) The DMA transfer complete is used with interrupt mode. Once the SD transfer
            is complete, the SD interrupt is handeled using the function BSP_SD_IRQHandler(),
            the DMA Tx/Rx transfer complete are handeled using the functions
            BSP_SD_DMA_Tx_IRQHandler()/BSP_SD_DMA_Rx_IRQHandler(). The corresponding user callbacks
            are implemented by the user at application level.
       (++) The SD erase block(s) is performed using the function BSP_SD_Erase() with specifying
            the number of blocks to erase.
       (++) The SD runtime status is returned when calling the function BSP_SD_GetStatus().
   [..]
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
#include "stm32l496g_discovery_io.h"
#include "stm32l496g_discovery_sd.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_SD STM32L496G_DISCOVERY SD
  * @{
  */

/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32L496G_SD_Private_Variables Private Variables
  * @{
  */
SD_HandleTypeDef uSdHandle;
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_SD_Private_Functions Private Functions
  * @{
  */
static void SD_Detect_MspInit(void);
static void SD_Detect_MspDeInit(void);
static HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef *hsd);
static HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef *hsd);

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup STM32L496G_DISCOVERY_SD_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the SD card device.
  * @param  None
  * @retval SD status
  */
uint8_t BSP_SD_Init(void)
{
  uint8_t sd_state = MSD_OK;

  /* uSD device interface configuration */
  uSdHandle.Instance = SDMMC1;
  uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  uSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
  uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
  uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
  uSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;

  /* Msp SD Detect pin initialization */
  SD_Detect_MspInit();

  /* Check if the SD card is plugged in the slot */
  if (BSP_SD_IsDetected() != SD_PRESENT)
  {
    return MSD_ERROR_SD_NOT_PRESENT;
  }

  /* Msp SD initialization */
  BSP_SD_MspInit(&uSdHandle, NULL);
    
  /* HAL SD initialization */
  if (HAL_SD_Init(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  /* Configure SD Bus width */
  if (sd_state == MSD_OK)
  {
    /* Enable wide operation */
    if (HAL_SD_ConfigWideBusOperation(&uSdHandle, SDMMC_BUS_WIDE_4B) != HAL_OK)
    {
      sd_state = MSD_ERROR;
    }
    else
    {
      sd_state = MSD_OK;
    }
  }

  return  sd_state;
}

/**
  * @brief  DeInitializes the SD card device.
  * @param  None
  * @retval SD status
  */
uint8_t BSP_SD_DeInit(void)
{
  uint8_t sd_state = MSD_OK;

  uSdHandle.Instance = SDMMC1;
  /* HAL SD deinitialization */
  if (HAL_SD_DeInit(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  /* Msp SD deinitialization */
  BSP_SD_MspDeInit(&uSdHandle, NULL);
  
  SD_Detect_MspDeInit();

  return  sd_state;
}

/**
  * @brief  Configures Interrupt mode for SD detection pin.
  * @param  None
  * @retval IO_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_SD_ITConfig(void)
{
  /* Check SD card detect pin */
  if (BSP_IO_ReadPin(SD_DETECT_PIN) != SD_DETECT_PIN)
  {
    return BSP_IO_ConfigPin(SD_DETECT_PIN, IO_MODE_IT_RISING_EDGE_PU);
  }
  else
  {
    return BSP_IO_ConfigPin(SD_DETECT_PIN, IO_MODE_IT_FALLING_EDGE_PU);
  }
}



/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param  None
 * @retval Returns if SD is detected or not
 */
uint8_t BSP_SD_IsDetected(void)
{
  __IO uint8_t status = SD_PRESENT;

  /* Check SD card detect pin */
  if (BSP_IO_ReadPin(SD_DETECT_PIN) != GPIO_PIN_RESET)
  {
    status = SD_NOT_PRESENT;
  }

  return status;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @param  Timeout: Timeout for read operation
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  sd_state =  HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout);

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: Timeout for write operation
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  sd_state = HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout);

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  /* Invalidate the dma tx handle*/
  uSdHandle.hdmatx = NULL;

  /* Prepare the dma channel for a read operation */
  sd_state = SD_DMAConfigRx(&uSdHandle);

  if (sd_state == HAL_OK)
  {
    /* Read block(s) in DMA transfer mode */
    sd_state = HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks);
  }

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  /* Invalidate the dma rx handle*/
  uSdHandle.hdmarx = NULL;

  /* Prepare the dma channel for a read operation */
  sd_state = SD_DMAConfigTx(&uSdHandle);

  if (sd_state == HAL_OK)
  {
    /* Write block(s) in DMA transfer mode */
    sd_state = HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks);
  }

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  sd_state = HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr);

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Handles SD card interrupt request.
  * @retval None
  */
void BSP_SD_IRQHandler(void)
{
  HAL_SD_IRQHandler(&uSdHandle);
}

/**
  * @brief  Handles SD DMA Tx transfer interrupt request.
  * @retval None
  */
void BSP_SD_DMA_Tx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uSdHandle.hdmatx);
}

/**
  * @brief  Handles SD DMA Rx transfer interrupt request.
  * @retval None
  */
void BSP_SD_DMA_Rx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

/**
  * @brief  Gets the current SD card data status.
  * @param  None
  * @retval Data transfer state.
  */
uint8_t BSP_SD_GetCardState(void)
{
  HAL_SD_CardStateTypedef card_state;
  card_state = HAL_SD_GetCardState(&uSdHandle);

  if (card_state == HAL_SD_CARD_TRANSFER)
  {
    return (SD_TRANSFER_OK);
  }
  else if ((card_state == HAL_SD_CARD_SENDING) ||
           (card_state == HAL_SD_CARD_RECEIVING) ||
           (card_state == HAL_SD_CARD_PROGRAMMING))
  {
    return (SD_TRANSFER_BUSY);
  }
  else
  {
    return (SD_TRANSFER_ERROR);
  }
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None
  */
void BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo)
{
  /* Get SD card Information */
  HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}

/**
  * @brief  Initializes the SD MSP.
  * @note   The SDMMC clock configuration done within this function assumes that
  *         the PLLSAI1 input clock runs at 8 MHz.
  * @param hsd: SD handle
  * @param Params: Additional parameters
  * @retval None
  */
__weak void BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params)
{
  GPIO_InitTypeDef gpioinitstruct = {0};
  RCC_PeriphCLKInitTypeDef  RCC_PeriphClkInit;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(Params);

  HAL_RCCEx_GetPeriphCLKConfig(&RCC_PeriphClkInit);

  /* Configure the SDMMC1 clock source. The clock is derived from the PLLSAI1 */
  /* Hypothesis is that PLLSAI1 VCO input is 8Mhz */
  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1;
  RCC_PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  RCC_PeriphClkInit.PLLSAI1.PLLSAI1Q = 4;
  RCC_PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
  RCC_PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK)
  {
    while (1) {}
  }

  /* Enable SDMMC1 clock */
  __HAL_RCC_SDMMC1_CLK_ENABLE();

  /* Enable DMA2 clocks */
  SD_DMAx_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Common GPIO configuration */
  gpioinitstruct.Mode      = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull      = GPIO_PULLUP;
  gpioinitstruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpioinitstruct.Alternate = GPIO_AF12_SDMMC1;

  /* GPIOC configuration */
  gpioinitstruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;

  HAL_GPIO_Init(GPIOC, &gpioinitstruct);

  /* GPIOD configuration */
  gpioinitstruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* NVIC configuration for SDMMC1 interrupts */
  HAL_NVIC_SetPriority(SDMMCx_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(SDMMCx_IRQn);

  /* DMA initialization should be done here but , as there is only one channel for RX and TX it is configured and done directly when required*/
}

/**
  * @brief  De-Initializes the SD MSP.
  * @param hsd: SD handle
  * @param Params: Additional parameters
  * @retval None
  */
__weak void BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params)
{
  GPIO_InitTypeDef gpioinitstruct = {0};

  /* Prevent unused argument(s) compilation warning */
  UNUSED(Params);

  /* Enable SDMMC1 clock */
  __HAL_RCC_SDMMC1_CLK_DISABLE();

  /* Enable DMA2 clocks */
  SD_DMAx_CLK_DISABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Common GPIO configuration */
  gpioinitstruct.Mode      = GPIO_MODE_ANALOG;
  gpioinitstruct.Pull      = GPIO_NOPULL;
  gpioinitstruct.Speed     = GPIO_SPEED_FREQ_LOW;
  gpioinitstruct.Alternate = 0;

  /* GPIOC configuration */
  gpioinitstruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;

  HAL_GPIO_Init(GPIOC, &gpioinitstruct);

  /* GPIOD configuration */
  gpioinitstruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* NVIC configuration for SDMMC1 interrupts */
  HAL_NVIC_DisableIRQ(SDMMCx_IRQn);

}

/**
  * @brief BSP SD Abort callback
  * @retval None
  */
__weak void BSP_SD_AbortCallback(void)
{

}

/**
  * @brief BSP Tx Transfer completed callback
  * @retval None
  */
__weak void BSP_SD_WriteCpltCallback(void)
{

}

/**
  * @brief BSP Rx Transfer completed callback
  * @retval None
  */
__weak void BSP_SD_ReadCpltCallback(void)
{

}

/**
  * @brief SD Abort callback
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_AbortCallback();
}

/**
  * @brief Tx Transfer completed callback
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_WriteCpltCallback();
}

/**
  * @brief Rx Transfer completed callback
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_ReadCpltCallback();
}

/**
  * @}
  */


/** @addtogroup STM32L496G_DISCOVERY_SD_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the SD Detect pin MSP.
  * @param  hsd: SD handle
  * @param  Params
  * @retval None
  */
static void SD_Detect_MspInit(void)
{
  if (BSP_IO_Init() == IO_ERROR)
  {
    BSP_ErrorHandler();
  }
  BSP_IO_ConfigPin(SD_DETECT_PIN, IO_MODE_INPUT_PU);
}

/**
  * @brief  Initializes the SD Detect pin MSP.
  * @param  hsd: SD handle
  * @param  Params
  * @retval None
  */
static void SD_Detect_MspDeInit(void)
{
  /* Disable all interrupts */
  /*HAL_NVIC_DisableIRQ(MFX_INT_EXTI_IRQn);*/

  if (BSP_IO_Init() == IO_ERROR)
  {
    BSP_ErrorHandler();
  }
  BSP_IO_ConfigPin(SD_DETECT_PIN, IO_MODE_ANALOG);
}

/**
  * @brief Configure the DMA to receive data from the SD card
  * @retval
  *  HAL_ERROR or HAL_OK
  */
static HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef *hsd)
{
  static DMA_HandleTypeDef hdma_rx;
  HAL_StatusTypeDef status = HAL_ERROR;

  /* Configure DMA Rx parameters */
  hdma_rx.Init.Request             = DMA_REQUEST_7;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_rx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

  hdma_rx.Instance = SD_DMAx_STREAM;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hsd, hdmarx, hdma_rx);

  /* Stop any ongoing transfer and reset the state*/
  HAL_DMA_Abort(&hdma_rx);

  /* Deinitialize the Channel for new transfer */
  HAL_DMA_DeInit(&hdma_rx);

  /* Configure the DMA Channel */
  status = HAL_DMA_Init(&hdma_rx);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_IRQn);

  return (status);
}

/**
  * @brief Configure the DMA to transmit data to the SD card
  * @retval
  *  HAL_ERROR or HAL_OK
  */
static HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef *hsd)
{
  static DMA_HandleTypeDef hdma_tx;
  HAL_StatusTypeDef status;

  /* Configure DMA Tx parameters */
  hdma_tx.Init.Request             = DMA_REQUEST_7;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

  hdma_tx.Instance = SD_DMAx_STREAM;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hsd, hdmatx, hdma_tx);

  /* Stop any ongoing transfer and reset the state*/
  HAL_DMA_Abort(&hdma_tx);

  /* Deinitialize the Channel for new transfer */
  HAL_DMA_DeInit(&hdma_tx);

  /* Configure the DMA Channel */
  status = HAL_DMA_Init(&hdma_tx);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_IRQn);

  return (status);
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
