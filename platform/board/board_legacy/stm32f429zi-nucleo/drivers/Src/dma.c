/**
  ******************************************************************************
  * File Name          : dma.c
  * Description        : This file provides code for the configuration
  *                      of all the requested memory to memory DMA transfers.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "dma.h"
#include "stm32f4xx_hal.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

void DMA2_Stream1_IRQHandler(void)
{
    krhino_intrpt_enter();
    uint32_t temp_reg;
    temp_reg = DMA2_Stream1->CR & DMA_SxCR_CHSEL;
    switch(temp_reg)
    {
    case DMA_CHANNEL_0:
        break;
    case DMA_CHANNEL_1:
        break;
    case DMA_CHANNEL_2:
        break;
    case DMA_CHANNEL_3:
        break;
    case DMA_CHANNEL_4: 
        break;
    case DMA_CHANNEL_5://USART6_RX
        USART_DMA_RX_IRQHandler(USART6);
        break;
    case DMA_CHANNEL_6:
        break;
    case DMA_CHANNEL_7:
        break;
    default:
        break;
    }

    krhino_intrpt_exit();
}

//USART6_DMA_TX_IRQHandler
void DMA2_Stream6_IRQHandler(void)
{
    krhino_intrpt_enter();
    uint32_t temp_reg;
    temp_reg = DMA2_Stream6->CR & DMA_SxCR_CHSEL;
    switch(temp_reg)
    {
        case DMA_CHANNEL_0:
            break;
        case DMA_CHANNEL_1:
            break;
        case DMA_CHANNEL_2:
            break;
        case DMA_CHANNEL_3:
            break;
        case DMA_CHANNEL_4:
            break;
        case DMA_CHANNEL_5://USART6_TX
            USART_DMA_TX_IRQHandler(USART6);
            break;
        case DMA_CHANNEL_6:
            break;
        case DMA_CHANNEL_7:
            break;
        default:
        	break;
    }

    krhino_intrpt_exit();
    }

/* USER CODE END 1 */

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
