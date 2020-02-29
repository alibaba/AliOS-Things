/**
  ******************************************************************************
  * File Name          : pwm.c
  * Description        : This file provides code for the configuration
  *                      of the TIMER instances.
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
#include "stm32f4xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_base)
{
    if(htim_base->Instance==TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    } else if (htim_base->Instance==TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    } else if (htim_base->Instance==TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    } else if (htim_base->Instance==TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    } else if (htim_base->Instance==TIM5) {
        __HAL_RCC_TIM5_CLK_ENABLE();
    } else if (htim_base->Instance==TIM6) {
        __HAL_RCC_TIM6_CLK_ENABLE();
    } else if (htim_base->Instance==TIM7) {
        __HAL_RCC_TIM7_CLK_ENABLE();
    } else if(htim_base->Instance==TIM8) {
        __HAL_RCC_TIM8_CLK_ENABLE();
    } else if (htim_base->Instance == TIM9) {
        __HAL_RCC_TIM9_CLK_ENABLE();
    } else if (htim_base->Instance == TIM10) {
        __HAL_RCC_TIM10_CLK_ENABLE();
    }  else if (htim_base->Instance == TIM11) {
        __HAL_RCC_TIM11_CLK_ENABLE();
    } else if (htim_base->Instance == TIM12) {
        __HAL_RCC_TIM12_CLK_ENABLE();
    } else if (htim_base->Instance == TIM13) {
        __HAL_RCC_TIM13_CLK_ENABLE();
    } else if (htim_base->Instance == TIM14) {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if(htim_base->Instance==TIM1) {
        __HAL_RCC_TIM1_CLK_DISABLE();
    } else if (htim_base->Instance==TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    } else if (htim_base->Instance==TIM3) {
        __HAL_RCC_TIM3_CLK_DISABLE();
    } else if (htim_base->Instance==TIM4) {
        __HAL_RCC_TIM4_CLK_DISABLE();
    } else if (htim_base->Instance==TIM5) {
        __HAL_RCC_TIM5_CLK_DISABLE();
    } else if (htim_base->Instance==TIM6) {
        __HAL_RCC_TIM6_CLK_DISABLE();
    } else if (htim_base->Instance==TIM7) {
        __HAL_RCC_TIM7_CLK_DISABLE();
    } else if (htim_base->Instance==TIM8) {
        __HAL_RCC_TIM8_CLK_DISABLE();
    } else if (htim_base->Instance==TIM9) {
        __HAL_RCC_TIM9_CLK_DISABLE();
    } else if (htim_base->Instance==TIM10) {
        __HAL_RCC_TIM10_CLK_DISABLE();
    } else if (htim_base->Instance==TIM11) {
        __HAL_RCC_TIM11_CLK_DISABLE();
    } else if (htim_base->Instance==TIM12) {
        __HAL_RCC_TIM12_CLK_DISABLE();
    } else if (htim_base->Instance==TIM13) {
        __HAL_RCC_TIM13_CLK_DISABLE();
    } else if (htim_base->Instance==TIM14) {
        __HAL_RCC_TIM14_CLK_DISABLE();
    }
}

#endif /* HAL_TIM_MODULE_ENABLED */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
