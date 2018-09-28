/**
  ******************************************************************************
  * @file   DFSDM/DFSDM_PulseSkipper/Src/pulse_skipper.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to pulse skipper.
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
#include "stm32f4xx_hal.h"
#include "pulse_skipper.h"


/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup DFSDM_PulseSkipper
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
#define NB_BITSTREAM_CLOCK_PERIOD               4
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Timer3 handler declaration for DFSDM2 */
TIM_HandleTypeDef            TimDfsdm2Handle;
/* Timer4 handler declaration for DFSDM1 */
TIM_HandleTypeDef            TimDfsdm1Handle;
/* Private function prototypes -----------------------------------------------*/
static void Skipper_MspInit(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Enable Multi Channel  clock Delay for DFSDM1 and DFSDM2 .
  * @param  None
  * @retval None
  */
void Pulse_Skipper_Init(void)
{ 
  /* Configure TIM3 and TIM4 */
  Skipper_MspInit();

  /* Enable delay clock gating */
  HAL_DFSDM_EnableDelayClock(HAL_MCHDLY_CLOCK_DFSDM1);
  HAL_DFSDM_EnableDelayClock(HAL_MCHDLY_CLOCK_DFSDM2);
}


/**
  * @brief  Inject DFSDM2 audio clock as clock source for the bitstream clock
  * @param  None
  * @retval None
  */
void Pulse_Skipper_Bitstream_Start(void)
{
  HAL_DFSDM_BitstreamClock_Start();
}

/**
  * @brief  Stop DFSDM2 audio clock as clock source for the bitstream clock.
  * @param  None
  * @retval None
  */
void Pulse_Skipper_Bitstream_Stop(void)
{
  HAL_DFSDM_BitstreamClock_Stop();
}

/**
  * @brief  Generate Pulse skipper on selected channel
  * @param  skipperstruct: Structure for channel
  * @retval None
  */
void Pulse_Skipper_Generate_Pulse(PulseSkipper_InitTypeDef* skipperstruct)
{
  
  /*****************DFSDM1 Pulse Skipper Channel Generation ********************/
  if (skipperstruct->DFSDM1PulseSkipperCh == DFSDM1_PULSE_SKIPPER_CH02)
  {
    /* Disable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm1Handle.Instance, DFSDM1_TIM4_OC2, TIM_CCx_DISABLE);
        
    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm1Handle.Instance, DFSDM1_TIM4_OC2, TIM_CCx_ENABLE);
    
    /* Use TIM_Base in order to get interrupt once pulse ended */
    /* Enable the TIM Update interrupt */
    __HAL_TIM_ENABLE_IT(&TimDfsdm1Handle, TIM_IT_UPDATE);  
        
    /* Enable the Peripheral */
    __HAL_TIM_ENABLE(&TimDfsdm1Handle);   
  }
  else if (skipperstruct->DFSDM1PulseSkipperCh == DFSDM1_PULSE_SKIPPER_CH13)
  {
    /* Disable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm1Handle.Instance, DFSDM1_TIM4_OC1, TIM_CCx_DISABLE);
        
    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm1Handle.Instance, DFSDM1_TIM4_OC1, TIM_CCx_ENABLE);
    
    /* Use TIM_Base in order to get interrupt once pulse ended */
    /* Enable the TIM Update interrupt */
    __HAL_TIM_ENABLE_IT(&TimDfsdm1Handle, TIM_IT_UPDATE);  
        
    /* Enable the Peripheral */
    __HAL_TIM_ENABLE(&TimDfsdm1Handle);   
  }
  
  /*****************DFSDM2 Pulse Skipper Channel Generation ********************/
  if (skipperstruct->DFSDM2PulseSkipperCh == DFSDM2_PULSE_SKIPPER_CH04)
  {
    /* Disable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC4, TIM_CCx_DISABLE);
        
    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC4, TIM_CCx_ENABLE);
    
    /* Use TIM_Base in order to get interrupt once pulse ended */
    /* Enable the TIM Update interrupt */
    __HAL_TIM_ENABLE_IT(&TimDfsdm2Handle, TIM_IT_UPDATE);  
        
    /* Enable the Peripheral */
    __HAL_TIM_ENABLE(&TimDfsdm2Handle);   
  }
  else if (skipperstruct->DFSDM2PulseSkipperCh == DFSDM2_PULSE_SKIPPER_CH15)
  {
    /* Disable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC3, TIM_CCx_DISABLE);
        
    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC3, TIM_CCx_ENABLE);
    
    /* Use TIM_Base in order to get interrupt once pulse ended */
    /* Enable the TIM Update interrupt */
    __HAL_TIM_ENABLE_IT(&TimDfsdm2Handle, TIM_IT_UPDATE);  
        
    /* Enable the Peripheral */
    __HAL_TIM_ENABLE(&TimDfsdm2Handle);   
  }
  else if (skipperstruct->DFSDM2PulseSkipperCh == DFSDM2_PULSE_SKIPPER_CH26)
  {
    /* Disable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC2, TIM_CCx_DISABLE);
        
    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC2, TIM_CCx_ENABLE);
    
    /* Use TIM_Base in order to get interrupt once pulse ended */
    /* Enable the TIM Update interrupt */
    __HAL_TIM_ENABLE_IT(&TimDfsdm2Handle, TIM_IT_UPDATE);  
        
    /* Enable the Peripheral */
    __HAL_TIM_ENABLE(&TimDfsdm2Handle);   
  }
  else if (skipperstruct->DFSDM2PulseSkipperCh == DFSDM2_PULSE_SKIPPER_CH37)
  {
    /* Disable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC1, TIM_CCx_DISABLE);
        
    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TimDfsdm2Handle.Instance, DFSDM2_TIM3_OC1, TIM_CCx_ENABLE);
    
    /* Use TIM_Base in order to get interrupt once pulse ended */
    /* Enable the TIM Update interrupt */
    __HAL_TIM_ENABLE_IT(&TimDfsdm2Handle, TIM_IT_UPDATE);  
        
    /* Enable the Peripheral */
    __HAL_TIM_ENABLE(&TimDfsdm2Handle);   
  }  
}

/**
  * @brief  Turns selected LED Off. 
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED5
  *            @arg  LED8
  * @retval None
  */
static void Skipper_MspInit(void)
{
   /* Output compare structure */
   TIM_OC_InitTypeDef           sOCConfig;
   /* Slave configuration structure */
   TIM_SlaveConfigTypeDef       sSlaveConfig;
   
  /* Timers Configuration */
  TimDfsdm2Handle.Instance = TIM3;
  TimDfsdm1Handle.Instance = TIM4;

  /*====================== TIM3 configuration     =======================*/
  /* Initialize TIM3 peripheral in PWM mode */
  TimDfsdm2Handle.Init.Period            = NB_BITSTREAM_CLOCK_PERIOD ;
  TimDfsdm2Handle.Init.Prescaler         = 0;
  TimDfsdm2Handle.Init.ClockDivision     = 0;
  TimDfsdm2Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimDfsdm2Handle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&TimDfsdm2Handle) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Configure the OPM Mode */ 
  HAL_TIM_OnePulse_Init(&TimDfsdm2Handle, TIM_OPMODE_SINGLE);
  
  /* Configure the PWM_channel_1 */
  sOCConfig.OCMode     = TIM_OCMODE_PWM1;
  sOCConfig.OCPolarity = TIM_OCPOLARITY_LOW;
  sOCConfig.Pulse = 1;
  sOCConfig.OCFastMode = TIM_OCFAST_DISABLE ;
  if (HAL_TIM_PWM_ConfigChannel(&TimDfsdm2Handle, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }
  if (HAL_TIM_PWM_ConfigChannel(&TimDfsdm2Handle, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }
  if (HAL_TIM_PWM_ConfigChannel(&TimDfsdm2Handle, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
  {
   /* Initialization Error */
    while(1);;
  }
  if (HAL_TIM_PWM_ConfigChannel(&TimDfsdm2Handle, &sOCConfig, TIM_CHANNEL_4) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }

  /* Configure TIM3 in Gated slave mode &
  use the Internal Trigger 0 (ITR1) as trigger source */
  sSlaveConfig.SlaveMode        = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger     = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity  = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter    = 0;
  if (HAL_TIM_SlaveConfigSynchronization(&TimDfsdm2Handle, &sSlaveConfig) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }

  /*======================  TIM4 configuration   =======================*/
  /* Initialize TIM4 peripheral in PWM mode */
  TimDfsdm1Handle.Init.Period            = NB_BITSTREAM_CLOCK_PERIOD;
  TimDfsdm1Handle.Init.Prescaler         = 0;
  TimDfsdm1Handle.Init.ClockDivision     = 0;
  TimDfsdm1Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimDfsdm1Handle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&TimDfsdm1Handle) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }

  /* Configure the OPM Mode */
  HAL_TIM_OnePulse_Init(&TimDfsdm1Handle, TIM_OPMODE_SINGLE);
  
  /* Configure the PWM_channel_1 */
  sOCConfig.OCMode     = TIM_OCMODE_PWM1;
  sOCConfig.OCPolarity = TIM_OCPOLARITY_LOW;
  sOCConfig.Pulse = 1;
    sOCConfig.OCFastMode = TIM_OCFAST_DISABLE ;
  if (HAL_TIM_PWM_ConfigChannel(&TimDfsdm1Handle, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }
  if (HAL_TIM_PWM_ConfigChannel(&TimDfsdm1Handle, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }

  /* Configure TIM4 in Gated slave mode &
  use the Internal Trigger 0 (ITR1) as trigger source */
  sSlaveConfig.SlaveMode        = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger     = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity  = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter    = 0;
  if (HAL_TIM_SlaveConfigSynchronization(&TimDfsdm1Handle, &sSlaveConfig) != HAL_OK)
  {
   /* Initialization Error */
    while(1);
  }
}

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  
  /* Enable  TIM4, TIM3 clocks */
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();
  
  /* Enable  GPIOB, GPIOD clocks */  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  /* Configure PB.04 (TIM3_Channel1): pin D12 from CN6 */
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Configure PB.05 (TIM3_Channel2): pin D11 from CN6 */
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Configure PB.00 (TIM3_Channel3):  pin D6 from CN8 */
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Configure PB.01 (TIM3_Channel4):  pin A4 from CN9 */
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure PB.06 (TIM4_Channel1) :  pin D4 from CN8 */ 
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure PD.13 (TIM4_Channel2)  */
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);   
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);   
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
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
