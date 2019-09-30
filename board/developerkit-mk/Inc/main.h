/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define SIM_DET_Pin GPIO_PIN_2
#define SIM_DET_GPIO_Port GPIOE
#define GS_LED_Pin GPIO_PIN_3
#define GS_LED_GPIO_Port GPIOE
#define PCIE_RST_Pin GPIO_PIN_13
#define PCIE_RST_GPIO_Port GPIOC
#define SECURE_IO_Pin GPIO_PIN_0
#define SECURE_IO_GPIO_Port GPIOA
#define SECURE_RST_Pin GPIO_PIN_1
#define SECURE_RST_GPIO_Port GPIOA
#define LCD_DCX_Pin GPIO_PIN_6
#define LCD_DCX_GPIO_Port GPIOA
#define WIFI_RST_Pin GPIO_PIN_0
#define WIFI_RST_GPIO_Port GPIOB
#define WIFI_WU_Pin GPIO_PIN_1
#define WIFI_WU_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_2
#define LCD_RST_GPIO_Port GPIOB
#define LCD_PWR_Pin GPIO_PIN_7
#define LCD_PWR_GPIO_Port GPIOE
#define ZIGBEE_INT_Pin GPIO_PIN_8
#define ZIGBEE_INT_GPIO_Port GPIOE
#define ZIGBEE_INT_EXTI_IRQn EXTI9_5_IRQn
#define IR_IN_Pin GPIO_PIN_9
#define IR_IN_GPIO_Port GPIOE
#define KEY_3_Pin GPIO_PIN_10
#define KEY_3_GPIO_Port GPIOE
#define KEY_3_EXTI_IRQn EXTI15_10_IRQn
#define KEY_1_Pin GPIO_PIN_11
#define KEY_1_GPIO_Port GPIOE
#define KEY_1_EXTI_IRQn EXTI15_10_IRQn
#define CAM_PD_Pin GPIO_PIN_13
#define CAM_PD_GPIO_Port GPIOE
#define KEY_2_Pin GPIO_PIN_14
#define KEY_2_GPIO_Port GPIOE
#define KEY_2_EXTI_IRQn EXTI15_10_IRQn
#define SECURE_CLK_Pin GPIO_PIN_15
#define SECURE_CLK_GPIO_Port GPIOE
#define HTS_LED_Pin GPIO_PIN_11
#define HTS_LED_GPIO_Port GPIOD
#define PS_LED_Pin GPIO_PIN_14
#define PS_LED_GPIO_Port GPIOD
#define COMPASS_LED_Pin GPIO_PIN_15
#define COMPASS_LED_GPIO_Port GPIOD
#define CAM_MCLK_Pin GPIO_PIN_8
#define CAM_MCLK_GPIO_Port GPIOA
#define ALS_INT_Pin GPIO_PIN_15
#define ALS_INT_GPIO_Port GPIOA
#define ALS_INT_EXTI_IRQn EXTI15_10_IRQn
#define AUDIO_WU_Pin GPIO_PIN_4
#define AUDIO_WU_GPIO_Port GPIOD
#define AUDIO_CTL_Pin GPIO_PIN_5
#define AUDIO_CTL_GPIO_Port GPIOD
#define AUDIO_RST_Pin GPIO_PIN_6
#define AUDIO_RST_GPIO_Port GPIOD
#define ZIGBEE_RST_Pin GPIO_PIN_7
#define ZIGBEE_RST_GPIO_Port GPIOD
#define USB_PCIE_SW_Pin GPIO_PIN_5
#define USB_PCIE_SW_GPIO_Port GPIOB
#define ALS_LED_Pin GPIO_PIN_6
#define ALS_LED_GPIO_Port GPIOB
#define CAM_RST_Pin GPIO_PIN_8
#define CAM_RST_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
