/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : board.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#define HARDWARE_REVISION   "C3"
#define MODEL               "STM32F103RB-NUCLEO"

#ifdef BOOTLOADER
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART_BUADRATE 115200
#endif

typedef enum{
    PORT_UART_STD = 0,
    PORT_UART_AT = 1,
    PORT_UART_BLE = 2,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255,
}PORT_UART_TYPE;

typedef enum {
    PORT_TIMER1,
    PORT_TIMER2,
    PORT_TIMER3,
    PORT_TIMER4,
    PORT_TIMER_SIZE,
    PORT_TIMER_INVALID = 255
};

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void board_gpio_init(void);
void board_dma_init(void);
void SystemClock_Config(void);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

//LED
//#define LED1    128 //GPIO21  //GPIOB-PIN5   FOR ST-LINK**********************************
#define LED2    5 //GPIO21  //GPIOA-PIN5
#define LED3    ((uint8_t)0xff)
#define LED4    ((uint8_t)0xff)

//KEY
#define KEY1    45  //GPIOC-PIN13
//#define KEY2    67  //GPIOE-PIN3
//#define KEY3    66  //GPIOE-PIN2
#define KEY4    ((uint8_t)0xff)

#define KEY1_EXTI_IRQn EXTI15_10_IRQn

/* Private defines -----------------------------------------------------------*/
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define USART3_TX_Pin GPIO_PIN_10
#define USART3_TX_GPIO_Port GPIOC
#define USART3_RX_Pin GPIO_PIN_11
#define USART3_RX_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
