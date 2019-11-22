/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#ifndef __SOC_INIT_H
#define __SOC_INIT_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "aos/hal/gpio.h"
#include "aos/hal/i2c.h"
#include "aos/hal/uart.h"

#include "hal/hal_gpio_stm32l4.h"
#include "hal/hal_i2c_stm32l4.h"
#include "hal/hal_uart_stm32l4.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

typedef enum {
	GPIO_ALS_INT,
	GPIO_AUDIO_EN,
	GPIO_LCD_DCX,
	GPIO_LCD_PWR,
	GPIO_LCD_RST,
	GPIO_LED_ALS,
	GPIO_LED_GS,
	GPIO_LED_HTS,
	GPIO_LED_PS,
	GPIO_SW_FUNC_A,
	GPIO_SW_FUNC_B,
	GPIO_SW_WIFI,
	GPIO_WIFI_RST,
	GPIO_WIFI_WU,
	MAX_GPIO_NUM
} BOARD_GPIO;

extern gpio_dev_t brd_gpio_table[];
extern i2c_dev_t brd_i2c1_dev;
extern i2c_dev_t brd_i2c2_dev;
extern uart_dev_t brd_uart1_dev;
/* legency definition for the modules have no hal layer */
#define SW_FUNC_B_Pin GPIO_PIN_13
#define SW_FUNC_B_GPIO_Port GPIOC
#define SW_FUNC_B_EXTI_IRQn EXTI15_10_IRQn
#define LCD_PWR_Pin GPIO_PIN_0
#define LCD_PWR_GPIO_Port GPIOA
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOA
#define LCD_NSS_Pin GPIO_PIN_4
#define LCD_NSS_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_5
#define LCD_SCK_GPIO_Port GPIOA
#define LCD_TX_Pin GPIO_PIN_7
#define LCD_TX_GPIO_Port GPIOA
#define SW_WIFI_Pin GPIO_PIN_0
#define SW_WIFI_GPIO_Port GPIOB
#define SW_WIFI_EXTI_IRQn EXTI0_IRQn
#define LCD_DCX_Pin GPIO_PIN_1
#define LCD_DCX_GPIO_Port GPIOB
#define LED_GS_Pin GPIO_PIN_2
#define LED_GS_GPIO_Port GPIOB
#define AUDIO_EN_Pin GPIO_PIN_8
#define AUDIO_EN_GPIO_Port GPIOA
#define SW_FUNC_A_Pin GPIO_PIN_11
#define SW_FUNC_A_GPIO_Port GPIOA
#define SW_FUNC_A_EXTI_IRQn EXTI15_10_IRQn
#define LED_PS_Pin GPIO_PIN_12
#define LED_PS_GPIO_Port GPIOA
#define LED_HTS_Pin GPIO_PIN_15
#define LED_HTS_GPIO_Port GPIOA
#define WIFI_RST_Pin GPIO_PIN_4
#define WIFI_RST_GPIO_Port GPIOB
#define LED_ALS_Pin GPIO_PIN_5
#define LED_ALS_GPIO_Port GPIOB
#define WIFI_TX_Pin GPIO_PIN_6
#define WIFI_TX_GPIO_Port GPIOB
#define WIFI_RX_Pin GPIO_PIN_7
#define WIFI_RX_GPIO_Port GPIOB
#define ALS_INT_Pin GPIO_PIN_8
#define ALS_INT_GPIO_Port GPIOB
#define ALS_INT_EXTI_IRQn EXTI9_5_IRQn
#define WIFI_WU_Pin GPIO_PIN_9
#define WIFI_WU_GPIO_Port GPIOB
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD4_Pin GPIO_PIN_5
#define LD4_GPIO_Port GPIOA
#define SMPS_EN_Pin GPIO_PIN_12
#define SMPS_EN_GPIO_Port GPIOB
#define SMPS_V1_Pin GPIO_PIN_13
#define SMPS_V1_GPIO_Port GPIOB
#define SMPS_PG_Pin GPIO_PIN_14
#define SMPS_PG_GPIO_Port GPIOB
#define SPMS_SW_Pin GPIO_PIN_15
#define SPMS_SW_GPIO_Port GPIOB
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

#define LCD_DCX_Pin GPIO_PIN_1
#define LCD_DCX_GPIO_Port GPIOB
#define LCD_PWR_Pin GPIO_PIN_0
#define LCD_PWR_GPIO_Port GPIOA
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOA
#define LCD_NSS_Pin GPIO_PIN_4
#define LCD_NSS_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_5
#define LCD_SCK_GPIO_Port GPIOA
#define LCD_TX_Pin GPIO_PIN_7
#define LCD_TX_GPIO_Port GPIOA


#define KIDS_A10_PRT(fmt, args...)  \
  printf("%s: [%s-->%d]=> "fmt,   \
  __FILE__, __FUNCTION__,         \
  __LINE__, ##args)
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

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __SOC_INIT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
