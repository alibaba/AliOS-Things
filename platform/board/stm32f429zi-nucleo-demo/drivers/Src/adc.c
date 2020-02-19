/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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
#include "aos/hal/adc.h"
#include "stm32f4xx_hal.h"
#include "aos/hal/gpio.h"
#include "hal_adc_stm32f4.h"

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    stm32_adc_dev_t *adc_dev;
    ADC_MAPPINT     *entry;
    GPIO_TypeDef    *GPIOx;
    uint32_t         pin;

    adc_dev = entryof_stm32_adc_dev(hadc);
    entry = hal_adc_get_mapping_entry(adc_dev->port);

    /* Only adc channle 0 ~ channel 16 are gpio mapped */
    if (entry->hal_adc_channel <= HAL_ADC_CHANNEL_16) {
        if (hal_gpio_group(entry->hal_gpio_pin, &GPIOx)) {
           return;
        }

        pin = hal_gpio_pin(entry->hal_gpio_pin);
        hal_gpio_enable_clk(GPIOx);

        memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

        GPIO_InitStruct.Pin = pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    }

    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_ENABLE();
    } else if (hadc->Instance == ADC2) {
        __HAL_RCC_ADC2_CLK_ENABLE();
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    stm32_adc_dev_t *adc_dev;
    ADC_MAPPINT     *entry;
    GPIO_TypeDef    *GPIOx;
    uint16_t         pin;

    adc_dev = entryof_stm32_adc_dev(hadc);
    entry = hal_adc_get_mapping_entry(adc_dev->port);
    pin = hal_gpio_pin(entry->hal_gpio_pin);
    if (hal_gpio_group(entry->hal_gpio_pin, &GPIOx)) {
       return;
    }


    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_DISABLE();
    } else if (hadc->Instance == ADC2) {
        __HAL_RCC_ADC2_CLK_DISABLE();
    }

    HAL_GPIO_DeInit(GPIOx, pin);
}

