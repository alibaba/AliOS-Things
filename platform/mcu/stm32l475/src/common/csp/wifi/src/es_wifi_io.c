/**
  ******************************************************************************
  * @file    es_wifi_io.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   This file implments the IO operations to deal with the es-wifi
  *          module. It mainly Inits and Deinits the SPI interface. Send and
  *          receive data over it.
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
#include "es_wifi_io.h"
#include <string.h>
#include "aos/kernel.h"
#include "k_api.h"
#include "es_wifi_conf.h"

/* Private define ------------------------------------------------------------*/
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi;

static aos_mutex_t spi_tx_mutex;
static aos_mutex_t spi_rx_mutex;
static aos_sem_t spi_tx_sem;
static aos_sem_t spi_rx_sem;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
                       COM Driver Interface (SPI)
*******************************************************************************/
/**
  * @brief  Initialize SPI MSP
  * @param  hspi: SPI handle
  * @retval None
  */
void SPI_WIFI_MspInit(SPI_HandleTypeDef* hspi)
{
  
  GPIO_InitTypeDef GPIO_Init;
  
  __HAL_RCC_SPI3_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  
  /* configure Wake up pin */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET ); 
  GPIO_Init.Pin       = GPIO_PIN_13;
  GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_Init );

  /* configure Data ready pin */
  GPIO_Init.Pin       = GPIO_PIN_1;
  GPIO_Init.Mode      = GPIO_MODE_IT_RISING;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_Init );

  /* configure Reset pin */
  GPIO_Init.Pin       = GPIO_PIN_8;
  GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_LOW;
  GPIO_Init.Alternate = 0;
  HAL_GPIO_Init(GPIOE, &GPIO_Init );
  
  /* configure SPI NSS pin pin */
  HAL_GPIO_WritePin( GPIOE, GPIO_PIN_0, GPIO_PIN_SET ); 
  GPIO_Init.Pin       = GPIO_PIN_0;
  GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init( GPIOE, &GPIO_Init );
  
  /* configure SPI CLK pin */
  GPIO_Init.Pin       = GPIO_PIN_10;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_Init.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init( GPIOC, &GPIO_Init );
  
  /* configure SPI MOSI pin */
  GPIO_Init.Pin       = GPIO_PIN_12;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_Init.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init( GPIOC, &GPIO_Init );
  
  /* configure SPI MISO pin */
  GPIO_Init.Pin       = GPIO_PIN_11;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_PULLUP;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_Init.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init( GPIOC, &GPIO_Init );

  /* Configure the NVIC for SPI */
  /* NVIC for SPI */
  HAL_NVIC_SetPriority(SPI3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(SPI3_IRQn);
}

/**
  * @brief  Initialize the SPI3
  * @param  None
  * @retval None
  */
int8_t SPI_WIFI_Init(void)
{
  uint32_t tickstart = HAL_GetTick();
  uint8_t Prompt[6];
  uint8_t count = 0;
  HAL_StatusTypeDef  Status;
  
  hspi.Instance               = SPI3;
  SPI_WIFI_MspInit(&hspi);
  
  hspi.Init.Mode              = SPI_MODE_MASTER;
  hspi.Init.Direction         = SPI_DIRECTION_2LINES;
  hspi.Init.DataSize          = SPI_DATASIZE_16BIT;
  hspi.Init.CLKPolarity       = SPI_POLARITY_LOW;
  hspi.Init.CLKPhase          = SPI_PHASE_1EDGE;
  hspi.Init.NSS               = SPI_NSS_SOFT;
  hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  /* 80/8= 10MHz (Inventek WIFI module supportes up to 20MHz)*/
  hspi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi.Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi.Init.CRCPolynomial     = 0;
  
  if(HAL_SPI_Init( &hspi ) != HAL_OK)
  {
    return -1;
  }
  
  WIFI_RESET_MODULE();
  
  WIFI_ENABLE_NSS(); 
  
  while (WIFI_IS_CMDDATA_READY())
  {
    Status = HAL_SPI_Receive(&hspi , &Prompt[count], 1, 0xFFFF);  
    count += 2;
    if(((HAL_GetTick() - tickstart ) > 0xFFFF) || (Status != HAL_OK))
    {
      WIFI_DISABLE_NSS(); 
      return -1;
    }    
  }
  
  if((Prompt[0] != 0x15) ||(Prompt[1] != 0x15) ||(Prompt[2] != '\r')||
       (Prompt[3] != '\n') ||(Prompt[4] != '>') ||(Prompt[5] != ' '))
  {
    WIFI_DISABLE_NSS(); 
    return -1;
  }    
   
  WIFI_DISABLE_NSS(); 
  aos_mutex_new(&spi_tx_mutex);
  aos_mutex_new(&spi_rx_mutex);
  aos_sem_new(&spi_tx_sem, 0);
  aos_sem_new(&spi_rx_sem, 0);
  return 0;
}

/**
  * @brief  DeInitialize the SPI
  * @param  None
  * @retval None
  */
int8_t SPI_WIFI_DeInit(void)
{
  HAL_SPI_DeInit( &hspi );
  aos_mutex_free(&spi_tx_mutex);
  aos_mutex_free(&spi_rx_mutex);
  aos_sem_free(&spi_tx_sem);
  aos_sem_free(&spi_rx_sem);
  return 0;
}

/**
  * @brief  Receive wifi Data from SPI
  * @param  pdata : pointer to data
  * @param  len : Data length
  * @param  timeout : send timeout in mS
  * @retval Length of received data (payload)
  */
int16_t SPI_WIFI_ReceiveData(uint8_t *pData, uint16_t len, uint32_t timeout)
{
  uint32_t tickstart = HAL_GetTick();
  int16_t length = 0;
  uint8_t tmp[2];
  HAL_SPI_StateTypeDef state = HAL_SPI_STATE_BUSY_RX;
  aos_mutex_lock(&spi_rx_mutex, RHINO_WAIT_FOREVER);
  
  HAL_SPIEx_FlushRxFifo(&hspi);
  
  WIFI_DISABLE_NSS(); 
  
  while (!WIFI_IS_CMDDATA_READY())
  {
    if((HAL_GetTick() - tickstart ) > timeout)
    {
      aos_mutex_unlock(&spi_rx_mutex);
      return -1;
    }
  }
  
  WIFI_ENABLE_NSS(); 

  while (WIFI_IS_CMDDATA_READY())
  {
    if((length < len) || (!len))
    {
      if (hspi.State != HAL_SPI_STATE_READY) {
        aos_sem_wait(&spi_rx_sem, RHINO_WAIT_FOREVER);
      } else {
        state = HAL_SPI_STATE_READY;
      }
      if (HAL_SPI_Receive_IT(&hspi, tmp, 1) != HAL_OK) {
        aos_mutex_unlock(&spi_rx_mutex);
        return -1;
      }
      if (HAL_SPI_STATE_READY == state) {
        aos_sem_wait(&spi_rx_sem, RHINO_WAIT_FOREVER);
      }
	  /* let some time to hardware to change CMDDATA signal */
      if(tmp[1] == 0x15)
      {
        SPI_WIFI_Delay(1);
      }
      /*This the last data */
      if(!WIFI_IS_CMDDATA_READY())
      {
        if(tmp[1] == 0x15)
        {
          pData[0] = tmp[0];
          length++;
          break;
        }     
      }
      
      pData[0] = tmp[0];
      pData[1] = tmp[1];
      length += 2;
      pData  += 2;

      if (length >= ES_WIFI_DATA_SIZE) {
        printf("SPI_WIFI_ReceiveData: Buffer overflow, receive length = %d!\n", length);
        WIFI_DISABLE_NSS();
        aos_mutex_unlock(&spi_rx_mutex);
        return -1;
      }
      
      if((HAL_GetTick() - tickstart ) > timeout)
      {
        WIFI_DISABLE_NSS(); 
        aos_mutex_unlock(&spi_rx_mutex);
        return -1;
      }
    }
    else
    {
      break;
    }
  }

  WIFI_DISABLE_NSS(); 
  aos_mutex_unlock(&spi_rx_mutex);
  return length;
}
/**
  * @brief  Send wifi Data thru SPI
  * @param  pdata : pointer to data
  * @param  len : Data length
  * @param  timeout : send timeout in mS
  * @retval Length of sent data
  */
int16_t SPI_WIFI_SendData( uint8_t *pdata,  uint16_t len, uint32_t timeout)
{
  uint32_t tickstart = HAL_GetTick();
  uint8_t Padding[2];
  HAL_SPI_StateTypeDef state = HAL_SPI_STATE_BUSY_TX;
  aos_mutex_lock(&spi_tx_mutex, RHINO_WAIT_FOREVER);
  
  while (!WIFI_IS_CMDDATA_READY())
  {
    if((HAL_GetTick() - tickstart ) > timeout)
    {
      WIFI_DISABLE_NSS();
      aos_mutex_unlock(&spi_tx_mutex);
      return -1;
    }
  }
  
  WIFI_ENABLE_NSS(); 
  if (len > 1)
  {
    if (hspi.State != HAL_SPI_STATE_READY) {
      aos_sem_wait(&spi_tx_sem, RHINO_WAIT_FOREVER);
    } else {
      state = HAL_SPI_STATE_READY;
    }
    if( HAL_SPI_Transmit_IT(&hspi, (uint8_t *)pdata , len/2) != HAL_OK)
    {
      WIFI_DISABLE_NSS();
      aos_mutex_unlock(&spi_tx_mutex);
      return -1;
    }
    if (HAL_SPI_STATE_READY == state) {
      aos_sem_wait(&spi_tx_sem, RHINO_WAIT_FOREVER);
    }
  }
  
  if ( len & 1)
  {
    Padding[0] = pdata[len-1];
    Padding[1] = '\n';

    if (hspi.State != HAL_SPI_STATE_READY) {
      aos_sem_wait(&spi_tx_sem, RHINO_WAIT_FOREVER);
    } else {
      state = HAL_SPI_STATE_READY;
    }
    if( HAL_SPI_Transmit_IT(&hspi, Padding, 1) != HAL_OK)
    {
      WIFI_DISABLE_NSS();
      aos_mutex_unlock(&spi_tx_mutex);
      return -1;
    }
    if (HAL_SPI_STATE_READY == state) {
      aos_sem_wait(&spi_tx_sem, RHINO_WAIT_FOREVER);
    }
  }
  
  aos_mutex_unlock(&spi_tx_mutex);

  return len;
}

/**
  * @brief  Delay
  * @param  Delay in ms
  * @retval None
  */
void SPI_WIFI_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    aos_sem_signal(&spi_rx_sem);
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    aos_sem_signal(&spi_tx_sem);
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
