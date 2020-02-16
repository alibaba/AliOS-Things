/**
  ******************************************************************************
  * File Name          : DAC.c
  * Description        : This file provides code for the configuration
  *                      of the DAC instances.
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
#include <string.h>
#include "aos/hal/dac.h"
#include "stm32f4xx_hal.h"
#include "aos/hal/gpio.h"

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  if(hdac->Instance==DAC)
  {
      __HAL_RCC_DAC_CLK_ENABLE();

#ifdef HAL_DAC_USE_CHAN1
    __HAL_RCC_GPIOA_CLK_ENABLE();
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
    GPIO_InitStruct.Pin = HAL_DAC_CHAN1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#ifdef HAL_DAC_USE_CHAN2
    __HAL_RCC_GPIOA_CLK_ENABLE();
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
    GPIO_InitStruct.Pin = HAL_DAC_CHAN2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
  }
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{

  if(hdac->Instance == DAC)
  {
    __HAL_RCC_DAC_CLK_DISABLE();
    __HAL_RCC_DAC_FORCE_RESET();
    __HAL_RCC_DAC_RELEASE_RESET();

#ifdef HAL_DAC_USE_CHAN1
    HAL_GPIO_DeInit(GPIOA, HAL_DAC_CHAN1_Pin);
#endif

#ifdef HAL_DAC_USE_CHAN2
      HAL_GPIO_DeInit(GPIOA, HAL_DAC_CHAN2_Pin);
#endif
  }
}

