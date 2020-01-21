
#include "dma.h"
#include "stm32f4xx_hal.h"

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

