/**
  ******************************************************************************
  * @file    stm32l475e_iot01.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-March-2017
  * @brief   STM32L475E IOT01 board support package
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L475E_IOT01_H
#define __STM32L475E_IOT01_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */
      
/** @addtogroup STM32L475E_IOT01_LOW_LEVEL
  * @{
  */ 

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Exported_Types LOW LEVEL Exported Types
  * @{
  */
typedef enum 
{
LED2 = 0,
LED_GREEN = LED2,
}Led_TypeDef;


typedef enum 
{  
  BUTTON_USER = 0
}Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

typedef enum 
{
  COM1 = 0,
  COM2 = 0,
}COM_TypeDef;
/**
  * @}
  */ 

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */ 

/** 
  * @brief  Define for STM32L475E_IOT01 board
  */ 
#if !defined (USE_STM32L475E_IOT01)
 #define USE_STM32L475E_IOT01
#endif

#define LEDn                             ((uint8_t)1)

#define LED2_PIN                         GPIO_PIN_14
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()



#define LEDx_GPIO_CLK_ENABLE(__INDEX__)  do{if((__INDEX__) == 0) LED2_GPIO_CLK_ENABLE();}while(0)

#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do{if((__INDEX__) == 0) LED2_GPIO_CLK_DISABLE();}while(0)

/* Only one User/Wakeup button */
#define BUTTONn                             ((uint8_t)1)

/**
  * @brief Wakeup push-button
  */
#define USER_BUTTON_PIN                   GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT             GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn             EXTI15_10_IRQn



#define COMn                              ((uint8_t)1)

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define DISCOVERY_COM1                          USART1
#define DISCOVERY_COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define DISCOVERY_COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define DISCOVERY_COM1_TX_PIN                   GPIO_PIN_6
#define DISCOVERY_COM1_TX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()   
#define DISCOVERY_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()  
#define DISCOVERY_COM1_TX_AF                    GPIO_AF7_USART1

#define DISCOVERY_COM1_RX_PIN                   GPIO_PIN_7
#define DISCOVERY_COM1_RX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()   
#define DISCOVERY_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()  
#define DISCOVERY_COM1_RX_AF                    GPIO_AF7_USART1

#define DISCOVERY_COM1_IRQn                     USART1_IRQn
                                              

#define DISCOVERY_COMx_CLK_ENABLE(__INDEX__)            do { if((__INDEX__) == COM1) {DISCOVERY_COM1_CLK_ENABLE();}} while(0)
#define DISCOVERY_COMx_CLK_DISABLE(__INDEX__)           do { if((__INDEX__) == COM1) {DISCOVERY_COM1_CLK_DISABLE();}} while(0)

#define DISCOVERY_COMx_TX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) {DISCOVERY_COM1_TX_GPIO_CLK_ENABLE();}} while(0)
#define DISCOVERY_COMx_TX_GPIO_CLK_DISABLE(__INDEX__)   do { if((__INDEX__) == COM1) {DISCOVERY_COM1_TX_GPIO_CLK_DISABLE();}} while(0)

#define DISCOVERY_COMx_RX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) {DISCOVERY_COM1_RX_GPIO_CLK_ENABLE();}} while(0)
#define DISCOVERY_COMx_RX_GPIO_CLK_DISABLE(__INDEX__)   do { if((__INDEX__) == COM1) {DISCOVERY_COM1_RX_GPIO_CLK_DISABLE();}} while(0)


/* User can use this section to tailor I2Cx instance used and associated resources */
/* Definition for I2Cx resources */
#define DISCOVERY_I2Cx                             I2C2
#define DISCOVERY_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
#define DISCOVERY_I2Cx_CLK_DISABLE()               __HAL_RCC_I2C2_CLK_DISABLE()   
#define DISCOVERY_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()
   
#define DISCOVERY_I2Cx_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
#define DISCOVERY_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_I2Cx_SCL_PIN                     GPIO_PIN_10
#define DISCOVERY_I2Cx_SDA_PIN                     GPIO_PIN_11                                               
#define DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT           GPIOB
#define DISCOVERY_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C2

/* I2C interrupt requests */
#define DISCOVERY_I2Cx_EV_IRQn                     I2C2_EV_IRQn
#define DISCOVERY_I2Cx_ER_IRQn                     I2C2_ER_IRQn

/* I2C clock speed configuration (in Hz)
  WARNING:
   Make sure that this define is not already declared in other files.
   It can be used in parallel by other modules. */
#ifndef DISCOVERY_I2C_SPEED
 #define DISCOVERY_I2C_SPEED                             100000
#endif /* DISCOVERY_I2C_SPEED */

#ifndef DISCOVERY_I2Cx_TIMING  
#define DISCOVERY_I2Cx_TIMING                     ((uint32_t)0x00702681)
#endif /* DISCOVERY_I2Cx_TIMING */ 


/* I2C Sensors address */
/* LPS22HB (Pressure) I2C Address */
#define LPS22HB_I2C_ADDRESS  (uint8_t)0xBA
/* HTS221 (Humidity) I2C Address */
#define HTS221_I2C_ADDRESS   (uint8_t)0xBE

#ifdef USE_LPS22HB_TEMP
/* LPS22HB Sensor hardware I2C address */ 
#define TSENSOR_I2C_ADDRESS     LPS22HB_I2C_ADDRESS
#else /* USE_HTS221_TEMP */
/* HTS221 Sensor hardware I2C address */ 
#define TSENSOR_I2C_ADDRESS     HTS221_I2C_ADDRESS
#endif
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
                                               
/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Exported_Functions LOW LEVEL Exported Functions
  * @{
  */
uint32_t         BSP_GetVersion(void);  
void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
void             BSP_LED_Toggle(Led_TypeDef Led);
void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);
void             BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *husart);
void             BSP_COM_DeInit(COM_TypeDef COM, UART_HandleTypeDef *huart);
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
#ifdef __cplusplus
}
#endif

#endif /* __STM32L475E_IOT01_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
