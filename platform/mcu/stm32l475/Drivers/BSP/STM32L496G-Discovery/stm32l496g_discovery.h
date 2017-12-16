/**
  ******************************************************************************
  * @file    stm32l496g_discovery.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32L496G_DISCOVERY's LEDs,
  *          push-buttons hardware resources (MB1261).
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L496G_DISCOVERY_H
#define __STM32L496G_DISCOVERY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Define for STM32L496G_DISCOVERY board
  */



/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY_Common
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_Exported_Types Exported Types
  * @{
  */

/**
 * @brief LED Types Definition
 */
typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED_ORANGE = LED1,
  LED_GREEN  = LED2
}
Led_TypeDef;


/**
 * @brief JOYSTICK Types Definition
 */
typedef enum
{
  JOY_SEL   = 0,
  JOY_LEFT  = 1,
  JOY_RIGHT = 2,
  JOY_DOWN  = 3,
  JOY_UP    = 4,
  JOY_NONE  = 5
} JOYState_TypeDef;

typedef enum
{
  JOY_MODE_GPIO = 0,
  JOY_MODE_EXTI = 1
} JOYMode_TypeDef;

typedef enum 
{
  COM1 = 0
} COM_TypeDef;

/**
 * @brief LCD constroller Types Definition
 */
typedef struct
{
  __IO uint16_t REG;
  __IO uint16_t RAM;
} LCD_CONTROLLER_TypeDef;

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_FMC FMC LCD Constants
  * @{
  */
/* We use BANK1 as we use FMC_NE1 signal */
#define FMC_BANK1_BASE  ((uint32_t)(0x60000000 | 0x00000000))
#define FMC_LCD_BASE    ((uint32_t)(0x60000000 | 0x00080000))  /*using A18*/
#define FMC_BANK1_ADDR  ((LCD_CONTROLLER_TypeDef *) FMC_BANK1_BASE)
#define LCD_ADDR        ((LCD_CONTROLLER_TypeDef *) FMC_LCD_BASE)

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_LED LED Constants
  * @{
  */
/* To do: invert LED1 and LED2 */
#define LEDn                              2

/* LED1 is accessed thru the MFX */
#define LED1_PIN                          IO1_PIN_4

#define LED2_PIN                          GPIO_PIN_13
#define LED2_GPIO_PORT                    GPIOB
#define LED2_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()


/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_BUTTON  BUTTON Constants
  * @{
  */
#define JOYn                              5

/**
* @brief Joystick Right push-button
*/
#define RIGHT_JOY_PIN                     GPIO_PIN_11  /* PF.11 */
#define RIGHT_JOY_GPIO_PORT               GPIOF
#define RIGHT_JOY_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define RIGHT_JOY_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOF_CLK_DISABLE()
#define RIGHT_JOY_EXTI_IRQn               EXTI15_10_IRQn

/**
* @brief Joystick Left push-button
*/
#define LEFT_JOY_PIN                      GPIO_PIN_9  /* PI.09 */
#define LEFT_JOY_GPIO_PORT                GPIOI
#define LEFT_JOY_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOI_CLK_ENABLE()
#define LEFT_JOY_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOI_CLK_DISABLE()
#define LEFT_JOY_EXTI_IRQn                EXTI9_5_IRQn

/**
* @brief Joystick Up push-button
*/
#define UP_JOY_PIN                        GPIO_PIN_8  /* PI.08 */
#define UP_JOY_GPIO_PORT                  GPIOI
#define UP_JOY_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOI_CLK_ENABLE()
#define UP_JOY_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOI_CLK_DISABLE()
#define UP_JOY_EXTI_IRQn                  EXTI9_5_IRQn

/**
 * @brief Joystick Down push-button
 */
#define DOWN_JOY_PIN                      GPIO_PIN_10   /* PI.10 */
#define DOWN_JOY_GPIO_PORT                GPIOI
#define DOWN_JOY_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOI_CLK_ENABLE()
#define DOWN_JOY_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOI_CLK_DISABLE()
#define DOWN_JOY_EXTI_IRQn                EXTI15_10_IRQn

/**
 * @brief Joystick Sel push-button
 */
#define SEL_JOY_PIN                       GPIO_PIN_13   /* PC.13 */
#define SEL_JOY_GPIO_PORT                 GPIOC
#define SEL_JOY_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define SEL_JOY_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define SEL_JOY_EXTI_IRQn                 EXTI15_10_IRQn

#define JOYx_GPIO_CLK_ENABLE(__JOY__)     do { if((__JOY__) == JOY_SEL)   { SEL_JOY_GPIO_CLK_ENABLE();   } else \
                                               if((__JOY__) == JOY_DOWN)  { DOWN_JOY_GPIO_CLK_ENABLE();  } else \
                                               if((__JOY__) == JOY_LEFT)  { LEFT_JOY_GPIO_CLK_ENABLE();  } else \
                                               if((__JOY__) == JOY_RIGHT) { RIGHT_JOY_GPIO_CLK_ENABLE(); } else \
                                               if((__JOY__) == JOY_UP)    { UP_JOY_GPIO_CLK_ENABLE(); }  } while(0)

#define JOYx_GPIO_CLK_DISABLE(__JOY__)    do { if((__JOY__) == JOY_SEL)   { SEL_JOY_GPIO_CLK_DISABLE();   } else \
                                               if((__JOY__) == JOY_DOWN)  { DOWN_JOY_GPIO_CLK_DISABLE();  } else \
                                               if((__JOY__) == JOY_LEFT)  { LEFT_JOY_GPIO_CLK_DISABLE();  } else \
                                               if((__JOY__) == JOY_RIGHT) { RIGHT_JOY_GPIO_CLK_DISABLE(); } else \
                                               if((__JOY__) == JOY_UP)    { UP_JOY_GPIO_CLK_DISABLE(); }  } while(0)

#define JOY_ALL_PINS                      (RIGHT_JOY_PIN | LEFT_JOY_PIN | UP_JOY_PIN | DOWN_JOY_PIN | SEL_JOY_PIN)

/**
  * @}
  */

/** @defgroup STM32L4R9I_DISCOVERY_COM COM Constants
  * @{
  */
#define COMn                                    1

/**
 * @brief Definition for COM port1, connected to USART2 (ST-Link USB Virtual Com Port)
 */ 
#define DISCOVERY_COM1                          USART2
#define DISCOVERY_COM1_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define DISCOVERY_COM1_CLK_DISABLE()            __HAL_RCC_USART2_CLK_DISABLE()

#define DISCOVERY_COM1_TX_PIN                   GPIO_PIN_2
#define DISCOVERY_COM1_TX_GPIO_PORT             GPIOA
#define DISCOVERY_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()   
#define DISCOVERY_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()  
#define DISCOVERY_COM1_TX_AF                    GPIO_AF7_USART2

#define DISCOVERY_COM1_RX_PIN                   GPIO_PIN_6
#define DISCOVERY_COM1_RX_GPIO_PORT             GPIOD
#define DISCOVERY_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()   
#define DISCOVERY_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()  
#define DISCOVERY_COM1_RX_AF                    GPIO_AF7_USART2

#define DISCOVERY_COM1_IRQn                     USART2_IRQn
                                              

#define DISCOVERY_COMx_CLK_ENABLE(__INDEX__)            do { if((__INDEX__) == COM1) {DISCOVERY_COM1_CLK_ENABLE();}} while(0)
#define DISCOVERY_COMx_CLK_DISABLE(__INDEX__)           do { if((__INDEX__) == COM1) {DISCOVERY_COM1_CLK_DISABLE();}} while(0)

#define DISCOVERY_COMx_TX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) {DISCOVERY_COM1_TX_GPIO_CLK_ENABLE();}} while(0)
#define DISCOVERY_COMx_TX_GPIO_CLK_DISABLE(__INDEX__)   do { if((__INDEX__) == COM1) {DISCOVERY_COM1_TX_GPIO_CLK_DISABLE();}} while(0)

#define DISCOVERY_COMx_RX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) {DISCOVERY_COM1_RX_GPIO_CLK_ENABLE();}} while(0)
#define DISCOVERY_COMx_RX_GPIO_CLK_DISABLE(__INDEX__)   do { if((__INDEX__) == COM1) {DISCOVERY_COM1_RX_GPIO_CLK_DISABLE();}} while(0)

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_BUS  BUS Constants
  * @{
  */

#if defined(HAL_I2C_MODULE_ENABLED)
/*##################### I2C1 ###################################*/
/* User can use this section to tailor I2C1 instance used and associated
   resources */
/* Definition for I2C1 Pins */
#define DISCOVERY_I2C1_SCL_GPIO_PORT            GPIOG
#define DISCOVERY_I2C1_SDA_GPIO_PORT            GPIOG
#define DISCOVERY_I2C1_SCL_PIN                  GPIO_PIN_14
#define DISCOVERY_I2C1_SDA_PIN                  GPIO_PIN_13

#define DISCOVERY_I2C1_SCL_SDA_AF               GPIO_AF4_I2C1

/* Definition for I2C1 clock resources */
#define DISCOVERY_I2C1                          I2C1
#define DISCOVERY_I2C1_CLK_ENABLE()             __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCOVERY_I2C1_CLK_DISABLE()            __HAL_RCC_I2C1_CLK_DISABLE()
#define DISCOVERY_I2C1_SDA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOG_CLK_ENABLE()
#define DISCOVERY_I2C1_SCL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOG_CLK_ENABLE()
#define DISCOVERY_I2C1_SDA_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOG_CLK_DISABLE()
#define DISCOVERY_I2C1_SCL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOG_CLK_DISABLE()
#define DISCOVERY_I2C1_FORCE_RESET()            __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_I2C1_RELEASE_RESET()          __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2C1's NVIC */
#define DISCOVERY_I2C1_EV_IRQn                  I2C1_EV_IRQn
#define DISCOVERY_I2C1_EV_IRQHandler            I2C1_EV_IRQHandler
#define DISCOVERY_I2C1_ER_IRQn                  I2C1_ER_IRQn
#define DISCOVERY_I2C1_ER_IRQHandler            I2C1_ER_IRQHandler

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 80 MHz */
/* Set 0x90112626 value to reach 100 KHz speed (Rise time = 640ns, Fall time = 20ns) */
#ifndef DISCOVERY_I2C1_TIMING
#define DISCOVERY_I2C1_TIMING                  0x90D00e28/* 0x90112626*/
#endif /* DISCOVERY_I2C1_TIMING */

/* I2C clock speed configuration (in Hz)
   WARNING:
   Make sure that this define is not already declared in other files (ie.
   stm324xg_discovery.h file). It can be used in parallel by other modules. */
#ifndef BSP_I2C_SPEED
#define BSP_I2C_SPEED                              100000
#endif /* BSP_I2C_SPEED */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define DISCOVERY_I2C1_TIMEOUT_MAX              3000


/*##################### I2C2 ###################################*/
/* User can use this section to tailor I2C2 instance used and associated
   resources */
/* Definition for I2C2 Pins */
#define DISCOVERY_I2C2_SCL_PIN                  GPIO_PIN_4
#define DISCOVERY_I2C2_SCL_GPIO_PORT            GPIOH
#define DISCOVERY_I2C2_SDA_PIN                  GPIO_PIN_14
#define DISCOVERY_I2C2_SDA_GPIO_PORT            GPIOB
#define DISCOVERY_I2C2_SCL_SDA_AF               GPIO_AF4_I2C2
/* Definition for I2C2 clock resources */
#define DISCOVERY_I2C2                          I2C2
#define DISCOVERY_I2C2_CLK_ENABLE()             __HAL_RCC_I2C2_CLK_ENABLE()
#define DISCOVERY_I2C2_CLK_DISABLE()            __HAL_RCC_I2C2_CLK_DISABLE()
#define DISCOVERY_I2C2_SDA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_I2C2_SCL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOH_CLK_ENABLE()
#define DISCOVERY_I2C2_SDA_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOB_CLK_DISABLE()
#define DISCOVERY_I2C2_SCL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOH_CLK_DISABLE()
#define DISCOVERY_I2C2_FORCE_RESET()            __HAL_RCC_I2C2_FORCE_RESET()
#define DISCOVERY_I2C2_RELEASE_RESET()          __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2C2's NVIC */
#define DISCOVERY_I2C2_EV_IRQn                  I2C2_EV_IRQn
#define DISCOVERY_I2C2_ER_IRQn                  I2C2_ER_IRQn

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 80 MHz */
/* Set 0x90112626 value to reach 100 KHz speed (Rise time = 25ns, Fall time = 10ns) */
#ifndef DISCOVERY_I2C2_TIMING
#define DISCOVERY_I2C2_TIMING         0x40403E5D
#endif /* DISCOVERY_I2C2_TIMING */

#define IDD_I2C_ADDRESS                  ((uint16_t) 0x84)
#define IO1_I2C_ADDRESS                  ((uint16_t) 0x84)
#define AUDIO_I2C_ADDRESS                ((uint16_t) 0x94)
#define TS_I2C_ADDRESS                   ((uint16_t) 0x70)
#define CAMERA_I2C_ADDRESS               ((uint16_t) 0x60)

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define DISCOVERY_I2C2_TIMEOUT_MAX              3000


#ifndef DISCOVERY_I2C_TIMING
#define DISCOVERY_I2C_TIMING                 /*0x90D00e28*/ 0x90112626
#endif /* DISCOVERY_I2C2_TIMING */

/* Written here after to enable compilation only, to be thoroughly reviewed */

/* Definition for AUDIO I2Cx resources */
#define DISCOVERY_AUDIO_I2Cx                             I2C1
#define DISCOVERY_AUDIO_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCOVERY_AUDIO_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISCOVERY_AUDIO_I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_AUDIO_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_AUDIO_I2Cx_SCL_PIN                     GPIO_PIN_6
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT           GPIOB
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C1
#define DISCOVERY_AUDIO_I2Cx_SDA_PIN                     GPIO_PIN_7

/* I2C interrupt requests */
#define DISCOVERY_AUDIO_I2Cx_EV_IRQn                     I2C1_EV_IRQn
#define DISCOVERY_AUDIO_I2Cx_ER_IRQn                     I2C1_ER_IRQn


/* Definition for TS (Capacitive Touch Panel) I2Cx resources */
#define DISCOVERY_TS_I2Cx                             I2C2
#define DISCOVERY_TS_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
#define DISCOVERY_TS_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_TS_I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_TS_I2Cx_SDA_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOB_CLK_DISABLE()
#define DISCOVERY_TS_I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE()
#define DISCOVERY_TS_I2Cx_SCL_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOH_CLK_DISABLE()

#define DISCOVERY_TS_I2Cx_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
#define DISCOVERY_TS_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_TS_I2Cx_SCL_PIN                     GPIO_PIN_4
#define DISCOVERY_TS_I2Cx_SCL_GPIO_PORT               GPIOH
#define DISCOVERY_TS_I2Cx_SDA_PIN                     GPIO_PIN_14
#define DISCOVERY_TS_I2Cx_SDA_GPIO_PORT               GPIOB
#define DISCOVERY_TS_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C2

/* I2C interrupt requests */
#define DISCOVERY_TS_I2Cx_EV_IRQn                     I2C2_EV_IRQn
#define DISCOVERY_TS_I2Cx_ER_IRQn                     I2C2_ER_IRQn

/* I2C clock setting */
#define DISCOVERY_TS_RCC_PERIPHCLK_I2C               RCC_PERIPHCLK_I2C2
#define DISCOVERY_TS_RCC_CLKSOURCE_I2C               RCC_I2C2CLKSOURCE_SYSCLK



/* Definition for Camera I2Cx resources */
#define DISCOVERY_CAMERA_I2Cx                             I2C2
#define DISCOVERY_CAMERA_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
#define DISCOVERY_CAMERA_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_CAMERA_I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_CAMERA_I2Cx_SDA_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOB_CLK_DISABLE()
#define DISCOVERY_CAMERA_I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE()
#define DISCOVERY_CAMERA_I2Cx_SCL_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOH_CLK_DISABLE()

#define DISCOVERY_CAMERA_I2Cx_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
#define DISCOVERY_CAMERA_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_CAMERA_I2Cx_SCL_PIN                     GPIO_PIN_4
#define DISCOVERY_CAMERA_I2Cx_SCL_GPIO_PORT               GPIOH
#define DISCOVERY_CAMERA_I2Cx_SDA_PIN                     GPIO_PIN_14
#define DISCOVERY_CAMERA_I2Cx_SDA_GPIO_PORT               GPIOB
#define DISCOVERY_CAMERA_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C2

/* I2C interrupt requests */
#define DISCOVERY_CAMERA_I2Cx_EV_IRQn                     I2C2_EV_IRQn
#define DISCOVERY_CAMERA_I2Cx_ER_IRQn                     I2C2_ER_IRQn

/* I2C clock setting */
#define DISCOVERY_CAMERA_RCC_PERIPHCLK_I2C               RCC_PERIPHCLK_I2C2
#define DISCOVERY_CAMERA_RCC_CLKSOURCE_I2C               RCC_I2C2CLKSOURCE_SYSCLK





/* Definition for external, camera and Arduino connector I2Cx resources */  /* to be reviewed */
#define DISCOVERY_EXT_I2Cx                               I2C2
#define DISCOVERY_EXT_I2Cx_CLK_ENABLE()                  __HAL_RCC_I2C2_CLK_ENABLE()
#define DISCOVERY_EXT_DMAx_CLK_ENABLE()                  __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISCOVERY_EXT_I2Cx_FORCE_RESET()                 __HAL_RCC_I2C2_FORCE_RESET()
#define DISCOVERY_EXT_I2Cx_RELEASE_RESET()               __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_EXT_I2Cx_SCL_PIN                       GPIO_PIN_10
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT             GPIOB
#define DISCOVERY_EXT_I2Cx_SCL_AF                        GPIO_AF4_I2C2
#define DISCOVERY_EXT_I2Cx_SDA_AF                        GPIO_AF4_I2C2
#define DISCOVERY_EXT_I2Cx_SDA_PIN                       GPIO_PIN_9

/* I2C interrupt requests */
#define DISCOVERY_EXT_I2Cx_EV_IRQn                       I2C2_EV_IRQn
#define DISCOVERY_EXT_I2Cx_ER_IRQn                       I2C2_ER_IRQn

#ifndef DISCOVERY_I2C_SPEED
#define DISCOVERY_I2C_SPEED                             100000
#endif /* DISCOVERY_I2C_SPEED */


#endif /* HAL_I2C_MODULE_ENABLED */



/*##################### Audio Codec ##########################*/
/**
  * @brief  Audio codec chip reset definition
  */
/* Audio codec power on/off macro definition */
#define CODEC_AUDIO_POWER_OFF()      HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_RESET)
#define CODEC_AUDIO_POWER_ON()       HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_SET)

/* Audio Reset Pin definition */
#define AUDIO_RESET_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define AUDIO_RESET_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()
#define AUDIO_RESET_PIN                         GPIO_PIN_6
#define AUDIO_RESET_GPIO                        GPIOC




/*##################### MFX ##########################*/
/**
  * @brief  MFX interface pins
  */

#define MFX_INT_GPIO_PORT                 GPIOC                       /* GPIOC */
#define MFX_INT_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define MFX_INT_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define MFX_INT_PIN                       GPIO_PIN_5                  /* PC.05 */
#define MFX_INT_EXTI_IRQn                 EXTI9_5_IRQn

#define MFX_WAKEUP_GPIO_PORT              GPIOH                       /* GPIOH */
#define MFX_WAKEUP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
#define MFX_WAKEUP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOH_CLK_DISABLE()
#define MFX_WAKEUP_PIN                    GPIO_PIN_6                  /* PH.06 */



/* Legacy  */
#define IDD_INT_GPIO_PORT                 MFX_INT_GPIO_PORT
#define IDD_INT_GPIO_CLK_ENABLE()         MFX_INT_GPIO_CLK_ENABLE()
#define IDD_INT_GPIO_CLK_DISABLE()        MFX_INT_GPIO_CLK_DISABLE()
#define IDD_INT_PIN                       MFX_INT_PIN
#define IDD_INT_EXTI_IRQn                 MFX_INT_EXTI_IRQn
#define IDD_WAKEUP_GPIO_PORT              MFX_WAKEUP_GPIO_PORT
#define IDD_WAKEUP_GPIO_CLK_ENABLE()      MFX_WAKEUP_GPIO_CLK_ENABLE()
#define IDD_WAKEUP_GPIO_CLK_DISABLE()     MFX_WAKEUP_GPIO_CLK_DISABLE()
#define IDD_WAKEUP_PIN                    MFX_WAKEUP_PIN


/**
  * @brief  Idd current measurement interface pins on MFX
  */
#define IDD_AMP_CONTROL_PIN               AGPIO_PIN_1

/**
  * @brief TS INT pin
  */
#define TS_INT_PIN                        GPIO_PIN_14
#define TS_INT_GPIO_PORT                  GPIOG
#define TS_INT_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOG_CLK_ENABLE()
#define TS_INT_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOG_CLK_DISABLE()
#define TS_INT_EXTI_IRQn                  EXTI15_10_IRQn

/**
  * @brief TS RST pin
  */
/* TS RST is accessed thru the MFX */
#define TS_RST_PIN                        IO1_PIN_1

/**
  * @}
  */

/**
  * @}
  */


/** @defgroup STM32L496G_DISCOVERY_Exported_Functions Exported Functions
  * @{
  */
uint32_t                BSP_GetVersion(void);
void                    BSP_LED_Init(Led_TypeDef Led);
void                    BSP_LED_DeInit(Led_TypeDef Led);
void                    BSP_LED_On(Led_TypeDef Led);
void                    BSP_LED_Off(Led_TypeDef Led);
void                    BSP_LED_Toggle(Led_TypeDef Led);
uint8_t                 BSP_JOY_Init(JOYMode_TypeDef Joy_Mode);
void                    BSP_JOY_DeInit(void);
JOYState_TypeDef        BSP_JOY_GetState(void);
#if defined(HAL_UART_MODULE_ENABLED)
void                    BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *husart);
void                    BSP_COM_DeInit(COM_TypeDef COM, UART_HandleTypeDef *huart);
#endif /* HAL_UART_MODULE_ENABLED */

/* These __weak functions can be surcharged by application code for specific application needs */
void                    BSP_ErrorHandler(void);


void FMC_BANK1_MspInit(void);
void FMC_BANK1_MspDeInit(void);
#if defined(HAL_I2C_MODULE_ENABLED)
void I2C2_Init(void);
void I2C2_DeInit(void);
#endif /* HAL_I2C_MODULE_ENABLED */

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

#endif /* __STM32L496G_DISCOVERY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
