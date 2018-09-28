/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
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
#include "stm32f4xx_hal.h"

static DMA_HandleTypeDef            hFilter0Dma;
static DMA_HandleTypeDef            hFilter1Dma;
static DMA_HandleTypeDef            hFilter2Dma;
static DMA_HandleTypeDef            hFilter3Dma;
static DMA_HandleTypeDef            hFilter4Dma;
static DMA_HandleTypeDef            hdma_i2s_tx;

extern DFSDM_Filter_HandleTypeDef   Dfsdm2Filter1Handle;
extern DFSDM_Filter_HandleTypeDef   Dfsdm2Filter2Handle;
extern DFSDM_Filter_HandleTypeDef   Dfsdm2Filter3Handle;
extern DFSDM_Filter_HandleTypeDef   Dfsdm2Filter4Handle;
extern DFSDM_Filter_HandleTypeDef   Dfsdm1Filter1Handle;
extern I2S_HandleTypeDef            haudio_i2s;

extern void _Error_Handler(char *, int);
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    /**ADC1 GPIO Configuration    
    PC0     ------> ADC1_IN10
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA5     ------> ADC1_IN5
    PC4     ------> ADC1_IN14
    PB1     ------> ADC1_IN9 
    */
    GPIO_InitStruct.Pin = ARD_A0_Pin|ARD_A5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ARD_A1_Pin|ARD_A2_Pin|ARD_A3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ARD_A4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ARD_A4_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{

  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PC0     ------> ADC1_IN10
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA5     ------> ADC1_IN5
    PC4     ------> ADC1_IN14
    PB1     ------> ADC1_IN9 
    */
    HAL_GPIO_DeInit(GPIOC, ARD_A0_Pin|ARD_A5_Pin);

    HAL_GPIO_DeInit(GPIOA, ARD_A1_Pin|ARD_A2_Pin|ARD_A3_Pin);

    HAL_GPIO_DeInit(ARD_A4_GPIO_Port, ARD_A4_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hdac->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspInit 0 */

  /* USER CODE END DAC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DAC_CLK_ENABLE();
  
    /**DAC GPIO Configuration    
    PA4     ------> DAC_OUT1 
    */
    GPIO_InitStruct.Pin = ARD_D8_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ARD_D8_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN DAC_MspInit 1 */

  /* USER CODE END DAC_MspInit 1 */
  }

}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{

  if(hdac->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspDeInit 0 */

  /* USER CODE END DAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DAC_CLK_DISABLE();
  
    /**DAC GPIO Configuration    
    PA4     ------> DAC_OUT1 
    */
    HAL_GPIO_DeInit(ARD_D8_GPIO_Port, ARD_D8_Pin);

  /* USER CODE BEGIN DAC_MspDeInit 1 */

  /* USER CODE END DAC_MspDeInit 1 */
  }

}

void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;
  static uint8_t channelMspInitCount = 0;
    
  if (channelMspInitCount == 0)
  {
    /* Init only once */
    channelMspInitCount++;
  
    /* Enable DFSDM clock */
    __HAL_RCC_DFSDM1_CLK_ENABLE();
    __HAL_RCC_DFSDM2_CLK_ENABLE();

    /* DFSDM pins configuration: DFSDM_CKOUT, DMIC_DATIN pins ------------------*/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_DFSDM2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* MP34DT01TR microphones uses DFSDM2_DATIN1 input pin */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_DFSDM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* MP34DT01TR microphones uses DFSDM2_DATIN7 input pin */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* MP34DT01TR microphones uses DFSDM1_DATIN1 input pin */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);    

    rcc_ex_clk_init_struct.PeriphClockSelection = (RCC_PERIPHCLK_I2S_APB1 | RCC_PERIPHCLK_DFSDM | RCC_PERIPHCLK_DFSDM2);
    rcc_ex_clk_init_struct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;
    rcc_ex_clk_init_struct.DfsdmClockSelection = RCC_DFSDM1CLKSOURCE_APB2|RCC_DFSDM2CLKSOURCE_APB2;
    rcc_ex_clk_init_struct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SM = 8;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SR = 7;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
    
    /* I2S_APB1 selected as DFSDM audio clock source */
    __HAL_RCC_DFSDM1AUDIO_CONFIG(RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1);
    __HAL_RCC_DFSDM2AUDIO_CONFIG(RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB1);
  }
}

/**
  * @brief  Initializes the DFSDM filter MSP.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  
  if(hdfsdm_filter == &Dfsdm2Filter1Handle)
  {
    /* AUDIO_DFSDM2 FILTER0 */
    /* Configure the hdma_dfsdm_left handle parameters */
    hFilter0Dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hFilter0Dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hFilter0Dma.Init.MemInc              = DMA_MINC_ENABLE;
    hFilter0Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hFilter0Dma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hFilter0Dma.Init.Mode                = DMA_CIRCULAR;
    hFilter0Dma.Init.Priority            = DMA_PRIORITY_HIGH;
    hFilter0Dma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hFilter0Dma.Init.MemBurst            = DMA_MBURST_SINGLE;
    hFilter0Dma.Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hFilter0Dma.Init.Channel             = DMA_CHANNEL_8;
    hFilter0Dma.Instance                 = DMA2_Stream0;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&Dfsdm2Filter1Handle, hdmaReg, hFilter0Dma);

    if (HAL_OK != HAL_DMA_Init(&hFilter0Dma))
    {
      Error_Handler();
    }
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  }
  else if(hdfsdm_filter == &Dfsdm2Filter2Handle)
  {
    /* AUDIO_DFSDM2_FILTER1 */
    /* Configure the hdma_dfsdm_right handle parameters */
    hFilter1Dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hFilter1Dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hFilter1Dma.Init.MemInc              = DMA_MINC_ENABLE;
    hFilter1Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hFilter1Dma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hFilter1Dma.Init.Mode                = DMA_CIRCULAR;
    hFilter1Dma.Init.Priority            = DMA_PRIORITY_HIGH;
    hFilter1Dma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hFilter1Dma.Init.MemBurst            = DMA_MBURST_SINGLE;
    hFilter1Dma.Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hFilter1Dma.Init.Channel             = DMA_CHANNEL_8;
    hFilter1Dma.Instance                 = DMA2_Stream1;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&Dfsdm2Filter2Handle, hdmaReg, hFilter1Dma);

    if (HAL_OK != HAL_DMA_Init(&hFilter1Dma))
    {
      Error_Handler();
    }
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  }
  else if(hdfsdm_filter == &Dfsdm2Filter3Handle)
  {
    /* AUDIO_DFSDM2_FILTER2 */
    /* Configure the hdma_dfsdm_right handle parameters */
    hFilter2Dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hFilter2Dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hFilter2Dma.Init.MemInc              = DMA_MINC_ENABLE;
    hFilter2Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hFilter2Dma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hFilter2Dma.Init.Mode                = DMA_CIRCULAR;
    hFilter2Dma.Init.Priority            = DMA_PRIORITY_HIGH;
    hFilter2Dma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hFilter2Dma.Init.MemBurst            = DMA_MBURST_SINGLE;
    hFilter2Dma.Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hFilter2Dma.Init.Channel             = DMA_CHANNEL_8;
    hFilter2Dma.Instance                 = DMA2_Stream2;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&Dfsdm2Filter3Handle, hdmaReg, hFilter2Dma);

    if (HAL_OK != HAL_DMA_Init(&hFilter2Dma))
    {
      Error_Handler();
    }
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  }
  else if(hdfsdm_filter == &Dfsdm2Filter4Handle)
  {
    /* AUDIO_DFSDM2_FILTER3 */
    /* Configure the hdma_dfsdm_right handle parameters */
    hFilter3Dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hFilter3Dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hFilter3Dma.Init.MemInc              = DMA_MINC_ENABLE;
    hFilter3Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hFilter3Dma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hFilter3Dma.Init.Mode                = DMA_CIRCULAR;
    hFilter3Dma.Init.Priority            = DMA_PRIORITY_HIGH;
    hFilter3Dma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hFilter3Dma.Init.MemBurst            = DMA_MBURST_SINGLE;
    hFilter3Dma.Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hFilter3Dma.Init.Channel             = DMA_CHANNEL_8;
    hFilter3Dma.Instance                 = DMA2_Stream3;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&Dfsdm2Filter4Handle, hdmaReg, hFilter3Dma);

    if (HAL_OK != HAL_DMA_Init(&hFilter3Dma))
    {
      Error_Handler();
    }
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  }
  else if(hdfsdm_filter == &Dfsdm1Filter1Handle)
  {
    /* AUDIO_DFSDM1_FILTER0 */
    /* Configure the hdma_dfsdm_right handle parameters */
    hFilter4Dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hFilter4Dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hFilter4Dma.Init.MemInc              = DMA_MINC_ENABLE;
    hFilter4Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hFilter4Dma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hFilter4Dma.Init.Mode                = DMA_CIRCULAR;
    hFilter4Dma.Init.Priority            = DMA_PRIORITY_HIGH;
    hFilter4Dma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hFilter4Dma.Init.MemBurst            = DMA_MBURST_SINGLE;
    hFilter4Dma.Init.PeriphBurst         = DMA_PBURST_SINGLE;
    hFilter4Dma.Init.Channel             = DMA_CHANNEL_3;
    hFilter4Dma.Instance                 = DMA2_Stream6;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&Dfsdm1Filter1Handle, hdmaReg, hFilter4Dma);

    if (HAL_OK != HAL_DMA_Init(&hFilter4Dma))
    {
      Error_Handler();
    }
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
  }
}

void HAL_FMPI2C_MspInit(FMPI2C_HandleTypeDef* hfmpi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hfmpi2c->Instance==FMPI2C1)
  {
  /* USER CODE BEGIN FMPI2C1_MspInit 0 */

  /* USER CODE END FMPI2C1_MspInit 0 */
  
    /**FMPI2C1 GPIO Configuration    
    PC6     ------> FMPI2C1_SCL
    PC7     ------> FMPI2C1_SDA 
    */
    GPIO_InitStruct.Pin = I2CFMP1_SCL_Pin|I2CFMP_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_FMPI2C1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_FMPI2C1_CLK_ENABLE();
  /* USER CODE BEGIN FMPI2C1_MspInit 1 */

  /* USER CODE END FMPI2C1_MspInit 1 */
  }

}

void HAL_FMPI2C_MspDeInit(FMPI2C_HandleTypeDef* hfmpi2c)
{

  if(hfmpi2c->Instance==FMPI2C1)
  {
  /* USER CODE BEGIN FMPI2C1_MspDeInit 0 */

  /* USER CODE END FMPI2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FMPI2C1_CLK_DISABLE();
  
    /**FMPI2C1 GPIO Configuration    
    PC6     ------> FMPI2C1_SCL
    PC7     ------> FMPI2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOC, I2CFMP1_SCL_Pin|I2CFMP_SDA_Pin);

  /* USER CODE BEGIN FMPI2C1_MspDeInit 1 */

  /* USER CODE END FMPI2C1_MspDeInit 1 */
  }

}

//by: HAL_I2S_Init()
void HAL_I2S_MspInit(I2S_HandleTypeDef* hi2s)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2s->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();
  
    /**I2S2 GPIO Configuration    
    PC2     ------> I2S2_ext_SD
    PC3     ------> I2S2_SD
    PA3     ------> I2S2_MCK
    PD3     ------> I2S2_CK
    PB9     ------> I2S2_WS 
    */
    GPIO_InitStruct.Pin = CODEC_ext_SD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI2;
    HAL_GPIO_Init(CODEC_ext_SD_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CODEC_SD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(CODEC_SD_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CODEC_MCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(CODEC_MCK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CODEC_CK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(CODEC_CK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CODEC_WS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(CODEC_WS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_MspInit 1 */
  /* Configure DMA used for I2S */
	  __HAL_RCC_DMA1_CLK_ENABLE();
	  hdma_i2s_tx.Init.Channel             = DMA_CHANNEL_0;
	  hdma_i2s_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	  hdma_i2s_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	  hdma_i2s_tx.Init.MemInc              = DMA_MINC_ENABLE;
	  hdma_i2s_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	  hdma_i2s_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
	  hdma_i2s_tx.Init.Mode                = DMA_CIRCULAR;
	  hdma_i2s_tx.Init.Priority            = DMA_PRIORITY_HIGH;
	  hdma_i2s_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	  hdma_i2s_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	  hdma_i2s_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
	  hdma_i2s_tx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

	  hdma_i2s_tx.Instance = DMA1_Stream4;

	  /* Associate the DMA handle */
	  __HAL_LINKDMA(&haudio_i2s, hdmatx, hdma_i2s_tx);

	  /* Deinitialize the Stream for new transfer */
	  HAL_DMA_DeInit(&hdma_i2s_tx);

	  if (HAL_OK != HAL_DMA_Init(&hdma_i2s_tx))
	  {
	    Error_Handler();
	  }
	  /* Enable and set I2Sx Interrupt to a lower priority */
	  HAL_NVIC_SetPriority(SPI2_IRQn, 0x01, 0);
	  HAL_NVIC_EnableIRQ(SPI2_IRQn);
	  /* I2S DMA IRQ Channel configuration */
	  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0x01, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	  /* USER CODE END SPI2_MspInit 1 */
	  printf("%s: init I2S2 success\n", __func__);
  }

}

void HAL_I2S_MspDeInit(I2S_HandleTypeDef* hi2s)
{

  if(hi2s->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();
  
    /**I2S2 GPIO Configuration    
    PC2     ------> I2S2_ext_SD
    PC3     ------> I2S2_SD
    PA3     ------> I2S2_MCK
    PD3     ------> I2S2_CK
    PB9     ------> I2S2_WS 
    */
    HAL_GPIO_DeInit(GPIOC, CODEC_ext_SD_Pin|CODEC_SD_Pin);

    HAL_GPIO_DeInit(CODEC_MCK_GPIO_Port, CODEC_MCK_Pin);

    HAL_GPIO_DeInit(CODEC_CK_GPIO_Port, CODEC_CK_Pin);

    HAL_GPIO_DeInit(CODEC_WS_GPIO_Port, CODEC_WS_Pin);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* hqspi)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hqspi->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();
  
    /**QUADSPI GPIO Configuration    
    PE2     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    PD13     ------> QUADSPI_BK1_IO3
    PG6     ------> QUADSPI_BK1_NCS 
    */
    GPIO_InitStruct.Pin = QSPI_BK1_IO2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
    HAL_GPIO_Init(QSPI_BK1_IO2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_IO0_Pin|QSPI_BK1_IO1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
    HAL_GPIO_Init(QSPI_CLK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_IO3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
    HAL_GPIO_Init(QSPI_BK1_IO3_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_NCS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(QSPI_BK1_NCS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }

}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* hqspi)
{

  if(hqspi->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();
  
    /**QUADSPI GPIO Configuration    
    PE2     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    PD13     ------> QUADSPI_BK1_IO3
    PG6     ------> QUADSPI_BK1_NCS 
    */
    HAL_GPIO_DeInit(QSPI_BK1_IO2_GPIO_Port, QSPI_BK1_IO2_Pin);

    HAL_GPIO_DeInit(GPIOF, QSPI_BK1_IO0_Pin|QSPI_BK1_IO1_Pin);

    HAL_GPIO_DeInit(QSPI_CLK_GPIO_Port, QSPI_CLK_Pin);

    HAL_GPIO_DeInit(QSPI_BK1_IO3_GPIO_Port, QSPI_BK1_IO3_Pin);

    HAL_GPIO_DeInit(QSPI_BK1_NCS_GPIO_Port, QSPI_BK1_NCS_Pin);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }

}

void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hsd->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();
  
    /**SDIO GPIO Configuration    
    PA6     ------> SDIO_CMD
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK 
    */
    GPIO_InitStruct.Pin = SD_CMD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(SD_CMD_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SD_D0_Pin|SD_D1_Pin|SD_D2_Pin|SD_D3_Pin 
                          |SD_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }

}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{

  if(hsd->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();
  
    /**SDIO GPIO Configuration    
    PA6     ------> SDIO_CMD
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK 
    */
    HAL_GPIO_DeInit(SD_CMD_GPIO_Port, SD_CMD_Pin);

    HAL_GPIO_DeInit(GPIOC, SD_D0_Pin|SD_D1_Pin|SD_D2_Pin|SD_D3_Pin 
                          |SD_CLK_Pin);

  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }

}


void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    /* Enable GPIO TX/RX clock */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* Enable I2Cx clock */
    __HAL_RCC_I2C2_CLK_ENABLE();
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==UART10)
  {
  /* USER CODE BEGIN UART10_MspInit 0 */

  /* USER CODE END UART10_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_UART10_CLK_ENABLE();
  
    /**UART10 GPIO Configuration    
    PG11     ------> UART10_RX
    PG12     ------> UART10_TX 
    */
    GPIO_InitStruct.Pin = WIFI_SPI_CSN_Pin|WIFI_DRDY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_UART10;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN UART10_MspInit 1 */

  /* USER CODE END UART10_MspInit 1 */
  }
  else if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();
  
    /**USART6 GPIO Configuration    
    PG9     ------> USART6_RX
    PG14     ------> USART6_TX 
    */
    GPIO_InitStruct.Pin = VCP_RX_Pin|VCP_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
  else if(huart->Instance == UART7)
  {
    __HAL_RCC_UART7_CLK_ENABLE();
    GPIO_InitStruct.Pin = ARD_D0_Pin|ARD_D1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	printf("%s: uart7 msp init\n", __func__);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==UART10)
  {
  /* USER CODE BEGIN UART10_MspDeInit 0 */

  /* USER CODE END UART10_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART10_CLK_DISABLE();
  
    /**UART10 GPIO Configuration    
    PG11     ------> UART10_RX
    PG12     ------> UART10_TX 
    */
    HAL_GPIO_DeInit(GPIOG, WIFI_SPI_CSN_Pin|WIFI_DRDY_Pin);

  /* USER CODE BEGIN UART10_MspDeInit 1 */

  /* USER CODE END UART10_MspDeInit 1 */
  }
  else if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();
  
    /**USART6 GPIO Configuration    
    PG9     ------> USART6_RX
    PG14     ------> USART6_TX 
    */
    HAL_GPIO_DeInit(GPIOG, VCP_RX_Pin|VCP_TX_Pin);

  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }

}

static uint32_t FSMC_Initialized = 0;

static void HAL_FSMC_MspInit(void){
  /* USER CODE BEGIN FSMC_MspInit 0 */

  /* USER CODE END FSMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct;
  if (FSMC_Initialized) {
    return;
  }
  FSMC_Initialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FSMC_CLK_ENABLE();
  
  /** FSMC GPIO Configuration  
  PF0   ------> FSMC_A0
  PF1   ------> FSMC_A1
  PF2   ------> FSMC_A2
  PF3   ------> FSMC_A3
  PF4   ------> FSMC_A4
  PF5   ------> FSMC_A5
  PF12   ------> FSMC_A6
  PF13   ------> FSMC_A7
  PF14   ------> FSMC_A8
  PF15   ------> FSMC_A9
  PG0   ------> FSMC_A10
  PG1   ------> FSMC_A11
  PE7   ------> FSMC_D4
  PE8   ------> FSMC_D5
  PE9   ------> FSMC_D6
  PE10   ------> FSMC_D7
  PE11   ------> FSMC_D8
  PE12   ------> FSMC_D9
  PE13   ------> FSMC_D10
  PE14   ------> FSMC_D11
  PE15   ------> FSMC_D12
  PD8   ------> FSMC_D13
  PD9   ------> FSMC_D14
  PD10   ------> FSMC_D15
  PD11   ------> FSMC_A16
  PD12   ------> FSMC_A17
  PD14   ------> FSMC_D0
  PD15   ------> FSMC_D1
  PG2   ------> FSMC_A12
  PG3   ------> FSMC_A13
  PG4   ------> FSMC_A14
  PG5   ------> FSMC_A15
  PD0   ------> FSMC_D2
  PD1   ------> FSMC_D3
  PD4   ------> FSMC_NOE
  PD5   ------> FSMC_NWE
  PD7   ------> FSMC_NE1
  PG10   ------> FSMC_NE3
  PE0   ------> FSMC_NBL0
  PE1   ------> FSMC_NBL1
  */
  GPIO_InitStruct.Pin = PSRAM_A0_Pin|PSRAM_A1_Pin|PSRAM_A2_Pin|PSRAM_A3_Pin 
                          |PSRAM_A4_Pin|PSRAM_A5_Pin|PSRAM_A6_Pin|PSRAM_A7_Pin 
                          |PSRAM_A8_Pin|PSRAM_A9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PSRAM_A10_Pin|PSRAM_A11_Pin|PSRAM_A12_Pin|PSRAM_A13_Pin 
                          |PSRAM_A14_Pin|PSRAM_A15_Pin|LCD_NE3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_PSRAM_D4_Pin|LCD_PSRAM_D5_Pin|LCD_PSRAM_D6_Pin|LCD_PSRAM_D7_Pin 
                          |LCD_PSRAM_D8_Pin|LCD_PSRAM_D9_Pin|LCD_PSRAM_D10_Pin|LCD_PSRAM_D11_Pin 
                          |LCD_PSRAM_D12_Pin|PSRAM_NBL0_Pin|PSRAM_NBL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_PSRAM_D13_Pin|LCD_PSRAM_D14_Pin|LCd_PSRAM_D15_Pin|PSRAM_A16_Pin 
                          |PSRAM_A17_Pin|LCD_PSRAM_D0_Pin|LCD_PSRAM_D1_Pin|LCD_PSRAM_D2_Pin 
                          |LCD_PSRAM_D3_Pin|LCD_PSRAM_NOE_Pin|LCD_PSRAM_NWE_Pin|PSRAM_NE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN FSMC_MspInit 1 */

  /* USER CODE END FSMC_MspInit 1 */
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* hsram){
  /* USER CODE BEGIN SRAM_MspInit 0 */

  /* USER CODE END SRAM_MspInit 0 */
  HAL_FSMC_MspInit();
  /* USER CODE BEGIN SRAM_MspInit 1 */

  /* USER CODE END SRAM_MspInit 1 */
}

static uint32_t FSMC_DeInitialized = 0;

static void HAL_FSMC_MspDeInit(void){
  /* USER CODE BEGIN FSMC_MspDeInit 0 */

  /* USER CODE END FSMC_MspDeInit 0 */
  if (FSMC_DeInitialized) {
    return;
  }
  FSMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FSMC_CLK_DISABLE();
  
  /** FSMC GPIO Configuration  
  PF0   ------> FSMC_A0
  PF1   ------> FSMC_A1
  PF2   ------> FSMC_A2
  PF3   ------> FSMC_A3
  PF4   ------> FSMC_A4
  PF5   ------> FSMC_A5
  PF12   ------> FSMC_A6
  PF13   ------> FSMC_A7
  PF14   ------> FSMC_A8
  PF15   ------> FSMC_A9
  PG0   ------> FSMC_A10
  PG1   ------> FSMC_A11
  PE7   ------> FSMC_D4
  PE8   ------> FSMC_D5
  PE9   ------> FSMC_D6
  PE10   ------> FSMC_D7
  PE11   ------> FSMC_D8
  PE12   ------> FSMC_D9
  PE13   ------> FSMC_D10
  PE14   ------> FSMC_D11
  PE15   ------> FSMC_D12
  PD8   ------> FSMC_D13
  PD9   ------> FSMC_D14
  PD10   ------> FSMC_D15
  PD11   ------> FSMC_A16
  PD12   ------> FSMC_A17
  PD14   ------> FSMC_D0
  PD15   ------> FSMC_D1
  PG2   ------> FSMC_A12
  PG3   ------> FSMC_A13
  PG4   ------> FSMC_A14
  PG5   ------> FSMC_A15
  PD0   ------> FSMC_D2
  PD1   ------> FSMC_D3
  PD4   ------> FSMC_NOE
  PD5   ------> FSMC_NWE
  PD7   ------> FSMC_NE1
  PG10   ------> FSMC_NE3
  PE0   ------> FSMC_NBL0
  PE1   ------> FSMC_NBL1
  */
  HAL_GPIO_DeInit(GPIOF, PSRAM_A0_Pin|PSRAM_A1_Pin|PSRAM_A2_Pin|PSRAM_A3_Pin 
                          |PSRAM_A4_Pin|PSRAM_A5_Pin|PSRAM_A6_Pin|PSRAM_A7_Pin 
                          |PSRAM_A8_Pin|PSRAM_A9_Pin);

  HAL_GPIO_DeInit(GPIOG, PSRAM_A10_Pin|PSRAM_A11_Pin|PSRAM_A12_Pin|PSRAM_A13_Pin 
                          |PSRAM_A14_Pin|PSRAM_A15_Pin|LCD_NE3_Pin);

  HAL_GPIO_DeInit(GPIOE, LCD_PSRAM_D4_Pin|LCD_PSRAM_D5_Pin|LCD_PSRAM_D6_Pin|LCD_PSRAM_D7_Pin 
                          |LCD_PSRAM_D8_Pin|LCD_PSRAM_D9_Pin|LCD_PSRAM_D10_Pin|LCD_PSRAM_D11_Pin 
                          |LCD_PSRAM_D12_Pin|PSRAM_NBL0_Pin|PSRAM_NBL1_Pin);

  HAL_GPIO_DeInit(GPIOD, LCD_PSRAM_D13_Pin|LCD_PSRAM_D14_Pin|LCd_PSRAM_D15_Pin|PSRAM_A16_Pin 
                          |PSRAM_A17_Pin|LCD_PSRAM_D0_Pin|LCD_PSRAM_D1_Pin|LCD_PSRAM_D2_Pin 
                          |LCD_PSRAM_D3_Pin|LCD_PSRAM_NOE_Pin|LCD_PSRAM_NWE_Pin|PSRAM_NE1_Pin);

  /* USER CODE BEGIN FSMC_MspDeInit 1 */

  /* USER CODE END FSMC_MspDeInit 1 */
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram){
  /* USER CODE BEGIN SRAM_MspDeInit 0 */

  /* USER CODE END SRAM_MspDeInit 0 */
  HAL_FSMC_MspDeInit();
  /* USER CODE BEGIN SRAM_MspDeInit 1 */

  /* USER CODE END SRAM_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


