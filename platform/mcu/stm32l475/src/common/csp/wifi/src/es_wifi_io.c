/**
  ******************************************************************************
  * @file    es_wifi_io.c
  * @author  MCD Application Team
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
#include "es_wifi.h"
#include "es_wifi_io.h"
#include <string.h>
#include "es_wifi_conf.h"
#include <core_cm4.h>

/* Private define ------------------------------------------------------------*/
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi;
static  int volatile spi_rx_event=0;
static  int volatile spi_tx_event=0;
static  int volatile cmddata_rdy_rising_event=0;



/* Private function prototypes -----------------------------------------------*/
static  int wait_cmddata_rdy_high(int timeout);
static  int wait_cmddata_rdy_rising_event(int timeout);
static  int wait_spi_tx_event(int timeout);
static  int wait_spi_rx_event(int timeout);
static  void SPI_WIFI_DelayUs(uint32_t);
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
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13, GPIO_PIN_RESET ); 
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
  HAL_GPIO_WritePin( GPIOE , GPIO_PIN_0, GPIO_PIN_SET ); 
  GPIO_Init.Pin       =  GPIO_PIN_0;
  GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init( GPIOE, &GPIO_Init );
  
  /* configure SPI CLK pin */
  GPIO_Init.Pin       =  GPIO_PIN_10;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_Init.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_Init );
  
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
  HAL_GPIO_Init( GPIOC,&GPIO_Init );
}

/**
  * @brief  Initialize the SPI3
  * @param  None
  * @retval None
  */
int8_t SPI_WIFI_Init(uint16_t mode)
{
  uint32_t tickstart = HAL_GetTick();
  int8_t  rc=0;
  
  if (mode == ES_WIFI_INIT)
  {
    hspi.Instance               = SPI3;
    SPI_WIFI_MspInit(&hspi);
  
    hspi.Init.Mode              = SPI_MODE_MASTER;
    hspi.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi.Init.DataSize          = SPI_DATASIZE_16BIT;
    hspi.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi.Init.NSS               = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; /* 80/8= 10MHz (Inventek WIFI module supportes up to 20MHz)*/
    hspi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial     = 0;
  
    if(HAL_SPI_Init( &hspi ) != HAL_OK)
    {
      return -1;
    }

	 // Enable Interrupt for Data Ready pin , GPIO_PIN1
     HAL_NVIC_SetPriority((IRQn_Type)EXTI1_IRQn, 0x0F, 0x00);
     HAL_NVIC_EnableIRQ((IRQn_Type)EXTI1_IRQn);
	 
	 // Enable Interrupt for SPI tx and rx
     HAL_NVIC_SetPriority((IRQn_Type)SPI3_IRQn, 1, 0);
     HAL_NVIC_EnableIRQ((IRQn_Type)SPI3_IRQn);
    
    // create Mutex and Semaphore
	RTOS_CREATE_SEM_MUTEX();
  }
  
  rc= SPI_WIFI_ResetModule();

  return rc;
}


int8_t SPI_WIFI_ResetModule(void)
{
  uint32_t tickstart = HAL_GetTick();
  uint8_t Prompt[6];
  uint8_t count = 0;
  HAL_StatusTypeDef  Status;
 
  WIFI_RESET_MODULE();
  WIFI_ENABLE_NSS(); 
  SPI_WIFI_DelayUs(15);
 
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
  
  WIFI_DISABLE_NSS(); 
  if((Prompt[0] != 0x15) ||(Prompt[1] != 0x15) ||(Prompt[2] != '\r')||
       (Prompt[3] != '\n') ||(Prompt[4] != '>') ||(Prompt[5] != ' '))
  {
    return -1;
  }    
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
  RTOS_FREE_SEM_MUTEX();
  return 0;
}



/**
  * @brief  Receive wifi Data from SPI
  * @param  pdata : pointer to data
  * @param  len : Data length
  * @param  timeout : send timeout in mS
  * @retval Length of received data (payload)
  */

int wait_cmddata_rdy_high(int timeout)
{
  int tickstart = HAL_GetTick();
  while (WIFI_IS_CMDDATA_READY()==0)
  {
    if((HAL_GetTick() - tickstart ) > timeout)
    {
      return -1;
    }
  }
  return 0;
}



int wait_cmddata_rdy_rising_event(int timeout)
{
#ifdef SEM_WAIT
   return SEM_WAIT(cmddata_rdy_rising_sem, timeout); 
#else
  int tickstart = HAL_GetTick();
  while (cmddata_rdy_rising_event==1)
  {
    if((HAL_GetTick() - tickstart ) > timeout)
    {
      return -1;
    }
  }
  return 0; 
#endif
}

int wait_spi_rx_event(int timeout)
{
#ifdef SEM_WAIT
   return SEM_WAIT(spi_rx_sem, timeout); 
#else
  int tickstart = HAL_GetTick();
  while (spi_rx_event==1)
  {
    if((HAL_GetTick() - tickstart ) > timeout)
    {
      return -1;
    }
  }
  return 0; 
#endif
}

int wait_spi_tx_event(int timeout)
{
#ifdef SEM_WAIT
   return SEM_WAIT(spi_tx_sem, timeout); 
#else
  int tickstart = HAL_GetTick();
  while (spi_tx_event==1)
  {
    if((HAL_GetTick() - tickstart ) > timeout)
    {
      return -1;
    }
  }
  return 0; 
#endif
}



int16_t SPI_WIFI_ReceiveData(uint8_t *pData, uint16_t len, uint32_t timeout)
{
  int16_t length = 0;
  uint8_t tmp[2];
  
  WIFI_DISABLE_NSS(); 
  UNLOCK_SPI();
  SPI_WIFI_DelayUs(3);


  if (wait_cmddata_rdy_rising_event(timeout)<0)
  {
      return ES_WIFI_ERROR_WAITING_DRDY_FALLING;
  }

  LOCK_SPI();  
  WIFI_ENABLE_NSS(); 
  SPI_WIFI_DelayUs(15);
  while (WIFI_IS_CMDDATA_READY())
  {
    if((length < len) || (!len))
    {
      spi_rx_event=1;
      if (HAL_SPI_Receive_IT(&hspi, tmp, 1) != HAL_OK) {
        WIFI_DISABLE_NSS();
        UNLOCK_SPI();
        return ES_WIFI_ERROR_SPI_FAILED;
      }
  
      wait_spi_rx_event(timeout);

      pData[0] = tmp[0];
      pData[1] = tmp[1];
      length += 2;
      pData  += 2;
      
      if (length >= ES_WIFI_DATA_SIZE) {
        WIFI_DISABLE_NSS();
        SPI_WIFI_ResetModule();    
        UNLOCK_SPI();
        return ES_WIFI_ERROR_STUFFING_FOREVER;
      }     
    }
    else
    {
      break;
    }
  }
  WIFI_DISABLE_NSS(); 
  UNLOCK_SPI();
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
  uint8_t Padding[2];
  
  if (wait_cmddata_rdy_high(timeout)<0)
  {
    return ES_WIFI_ERROR_SPI_FAILED;
  }
    
  // arm to detect rising event
  cmddata_rdy_rising_event=1;
  LOCK_SPI();
  WIFI_ENABLE_NSS();
  SPI_WIFI_DelayUs(15);
  if (len > 1)
  {
    spi_tx_event=1;
    if( HAL_SPI_Transmit_IT(&hspi, (uint8_t *)pdata , len/2) != HAL_OK)
    {
      WIFI_DISABLE_NSS();
      UNLOCK_SPI();
      return ES_WIFI_ERROR_SPI_FAILED;
    }
    wait_spi_tx_event(timeout);
  }
  
  if ( len & 1)
  {
    Padding[0] = pdata[len-1];
    Padding[1] = '\n';

    spi_tx_event=1;
    if( HAL_SPI_Transmit_IT(&hspi, Padding, 1) != HAL_OK)
    {
      WIFI_DISABLE_NSS();
      UNLOCK_SPI();
      return ES_WIFI_ERROR_SPI_FAILED;
    }  
    wait_spi_tx_event(timeout);
    
  }
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
   * @brief  Delay
  * @param  Delay in us
  * @retval None
  */
void SPI_WIFI_DelayUs(uint32_t n)
{
  uint32_t freq = (SystemCoreClock/1000000L);
  n=n*freq;
  DWT->CTRL |= 1 ; // enable  counter
  DWT->CYCCNT = 0; // reset  counter
  while(DWT->CYCCNT < n);
  DWT->CTRL &= ~1 ; // disable  counter
  return;
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (spi_rx_event)
  {
    SEM_SIGNAL(spi_rx_sem);
    spi_rx_event=0;
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (spi_tx_event)
  {
    SEM_SIGNAL(spi_tx_sem);
    spi_tx_event=0;
  }
}


/**
  * @brief  Interrupt handler for  Data RDY signal
  * @param  None
  * @retval None
  */
void    SPI_WIFI_ISR(void)
{
   if (cmddata_rdy_rising_event==1)  
   {
     SEM_SIGNAL(cmddata_rdy_rising_sem);
     cmddata_rdy_rising_event=0;
   }
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
