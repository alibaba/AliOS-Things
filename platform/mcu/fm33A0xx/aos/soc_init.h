

/**
	******************************************************************************
	* File Name 				 : main.hpp
	* Description 			 : This file contains the common defines of the application
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
	* 	1. Redistributions of source code must retain the above copyright notice,
	* 		 this list of conditions and the following disclaimer.
	* 	2. Redistributions in binary form must reproduce the above copyright notice,
	* 		 this list of conditions and the following disclaimer in the documentation
	* 		 and/or other materials provided with the distribution.
	* 	3. Neither the name of STMicroelectronics nor the names of its contributors
	* 		 may be used to endorse or promote products derived from this software
	* 		 without specific prior written permission.
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

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
#include "fm33A0xx_include_all.h"
#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "i2c.h"
#include "interpt.h"
#include "nand.h"
#include "nor.h"
#include "pwm.h"
#include "qspi.h"
#include "rng.h"
#include "rtc.h"
#include "sd.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "wdg.h"

#include "k_api.h"
#include "kernel.h"
/*
 *  GPIO port name
 */
typedef enum 
{
  GPIO_A,
  GPIO_B,
  GPIO_C,
  GPIO_D,
  GPIO_E,
  GPIO_F,
  GPIO_G
}hal_gpio_port_name_t;

/*
 *  GPIO pin name
 */
typedef enum 
{
  GPIO_PIN_0,
  GPIO_PIN_1,
  GPIO_PIN_2,
  GPIO_PIN_3,
  GPIO_PIN_4,
  GPIO_PIN_5,
  GPIO_PIN_6,
  GPIO_PIN_7,
  GPIO_PIN_8,
  GPIO_PIN_9,
  GPIO_PIN_10,
  GPIO_PIN_11,
  GPIO_PIN_12,
  GPIO_PIN_13,
  GPIO_PIN_14,
  GPIO_PIN_15
}hal_gpio_pin_name_t;

/*
 *  GPIO irq name
 */
typedef enum 
{
  EXTI_0,
  EXTI_1,
  EXTI_2
}hal_gpio_irq_name_t;
/*
 * UART Name
 */
typedef enum 
{
  UART_0,
  UART_1,
  UART_2,
  UART_3,
  UART_4,
  UART_5
}hal_uart_name_t;

/*
 * UART Port/Pin
 */
#define UART0RX_Pin						  GPIO_Pin_3
#define UART0RX_Port 					  GPIOF
#define UART0TX_Pin						  GPIO_Pin_4
#define UART0TX_Port 					  GPIOF
  
#define UART1RX_Pin						  GPIO_Pin_0
#define UART1RX_Port 					  GPIOB
#define UART1TX_Pin						  GPIO_Pin_1
#define UART1TX_Port 					  GPIOB
  
  //#define UART1RX_Pin             GPIO_Pin_3
  //#define UART1RX_Port            GPIOE
  //#define UART1TX_Pin             GPIO_Pin_4
  //#define UART1TX_Port            GPIOE
  
#define UART2RX_Pin						  GPIO_Pin_2
#define UART2RX_Port 					  GPIOB
#define UART2TX_Pin						  GPIO_Pin_3
#define UART2TX_Port 					  GPIOB
  
#define UART3RX_Pin						  GPIO_Pin_10
#define UART3RX_Port 					  GPIOC
#define UART3TX_Pin						  GPIO_Pin_11
#define UART3TX_Port 					  GPIOC
  
#define UART4RX_Pin						  GPIO_Pin_0
#define UART4RX_Port 					  GPIOD
#define UART4TX_Pin						  GPIO_Pin_1
#define UART4TX_Port 					  GPIOD
  
  //#define UART4RX_Pin             GPIO_Pin_9
  //#define UART4RX_Port            GPIOD
  //#define UART4TX_Pin             GPIO_Pin_10
  //#define UART4TX_Port            GPIOD
  
#define UART5RX_Pin						  GPIO_Pin_4
#define UART5RX_Port 					  GPIOC
#define UART5TX_Pin						  GPIO_Pin_5
#define UART5TX_Port 					  GPIOC

#define LED1_Pin								GPIO_Pin_10
#define LED1_Port 							GPIOB
#define LED2_Pin								GPIO_Pin_9
#define LED2_Port 							GPIOB

#define LED3_Pin								GPIO_Pin_15
#define LED3_Port 							GPIOC
#define LED4_Pin								GPIO_Pin_14
#define LED4_Port 							GPIOC

#define DBGCOMRX_Pin						GPIO_Pin_0
#define DBGCOMRX_Port 					GPIOB
#define DBGCOMTX_Pin						GPIO_Pin_1
#define DBGCOMTX_Port 					GPIOB 
#define DBGUART 								UART_1 

//定义常量, 常数
//系统时钟默认使用RCHF
#define RCHFCLKCFG							8	//8, 16, 24, 32MHz

//define_all.h中RCHFCLKCFG控制系统时钟
#if ( 													RCHFCLKCFG == 8 )//8.0MHz
#define clkmode 								1
#define SYSCLKdef 							RCC_RCHFCON_FSEL_8MHZ//RCHF中心频率8MHz
#define SYSTEM_CLK							8000000
#elif ( 												RCHFCLKCFG == 16 )//16.0MHz
#define clkmode 								2
#define SYSCLKdef 							RCC_RCHFCON_FSEL_16MHZ//RCHF中心频率16MHz
#define SYSTEM_CLK							16000000

#elif ( 												RCHFCLKCFG == 24 )//24.0MHz
#define clkmode 								3
#define SYSCLKdef 							RCC_RCHFCON_FSEL_24MHZ//RCHF中心频率24MHz
#define SYSTEM_CLK							24000000

#elif ( 												RCHFCLKCFG == 32 )//32.0MHz
#define clkmode 								4
#define SYSCLKdef 							RCC_RCHFCON_FSEL_32MHZ//RCHF中心频率32MHz
#define SYSTEM_CLK							32000000

#endif

/* ########################## Assert Selection ############################## */
/**
	* @brief Uncomment the line below to expanse the "assert_param" macro in the 
	* 			 HAL drivers code
	*/
/* #define USE_FULL_ASSERT		1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#ifdef __cplusplus
extern "C"
{
#endif
  typedef int (*ConditionHook)(void);
  extern void TicksDelayMs(uint32_t ms , ConditionHook Hook);
  extern void TicksDelayUs(uint32_t us);
  extern void AnalogIO( GPIOx_Type* GPIOx, uint32_t PinNum );
  #define IN_NORMAL 							0
  #define IN_PULLUP 							1
  extern void InputIO( GPIOx_Type* GPIOx, uint32_t PinNum, uint8_t Type );
  #define OUT_PUSHPULL						0
  #define OUT_OPENDRAIN 					1
  extern void OutputIO( GPIOx_Type* GPIOx, uint32_t PinNum, uint8_t Type );
  #define ALTFUN_NORMAL 					0
  #define ALTFUN_OPENDRAIN				1
  #define ALTFUN_PULLUP 					2
  #define ALTFUN_OPENDRAIN_PULLUP 3
  extern void AltFunIO( GPIOx_Type* GPIOx, uint32_t PinNum, uint8_t Type );
  extern void CloseIO( GPIOx_Type* GPIOx, uint32_t PinNum );
	void _Error_Handler(char *, int);

#define 	Error_Handler() 				_Error_Handler(__FILE__, __LINE__)

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
