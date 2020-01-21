/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_I2C_STM32L4_H
#define __HAL_I2C_STM32L4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define AOS_PORT_I2C1                      1
#define AOS_PORT_I2C2                      2
#define AOS_PORT_I2C3                      3
#define AOS_PORT_I2C4                      4

/******************************** I2C1 configure *****************************/
#define I2C1_INSTANCE                      I2C1
#define I2C1_TIMING                        0x40403E5D
#define I2C1_OWN_ADDRESS1                  0x70
#define I2C1_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C1_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C1_OWNADDRESS2                   0xFF
#define I2C1_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C1_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C1_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/* Definition for I2C1 clock resources */
#define I2C1_RCC_PERIPH_CLOCK_SELECTION    RCC_PERIPHCLK_I2C1
#define I2C1_RCC_CLOCK_SELECTION           RCC_I2C1CLKSOURCE_SYSCLK

#define I2C1_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define I2C1_CLK_DISABLE()                 __HAL_RCC_I2C1_CLK_DISABLE()
#define I2C1_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define I2C1_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define I2C1_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOG_CLK_DISABLE()
#define I2C1_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOG_CLK_DISABLE()
#define I2C1_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define I2C1_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2C1 Pins */
#define I2C1_GPIO_SCL_PIN                  GPIO_PIN_14
#define I2C1_GPIO_SDA_PIN                  GPIO_PIN_13
#define I2C1_GPIO_SCL_PORT                 GPIOG
#define I2C1_GPIO_SDA_PORT                 GPIOG
#define I2C1_GPIO_MODE                     GPIO_MODE_AF_OD
#define I2C1_GPIO_PULL                     GPIO_PULLUP
#define I2C1_GPIO_SPEED                    GPIO_SPEED_FREQ_VERY_HIGH
#define I2C1_GPIO_ALTERNATE                GPIO_AF4_I2C1

/* Definition for I2C1's NVIC */
#define I2C1_EV_IRQn                       I2C1_EV_IRQn
#define I2C1_ER_IRQn                       I2C1_ER_IRQn

/******************************** I2C2 configure *****************************/
#define I2C2_INSTANCE                      I2C2
#define I2C2_TIMING                        0x40403E5D
#define I2C2_OWN_ADDRESS1                  0x70
#define I2C2_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C2_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C2_OWNADDRESS2                   0xFF
#define I2C2_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C2_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C2_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/* Definition for I2C2 clock resources */
#define I2C2_RCC_PERIPH_CLOCK_SELECTION    RCC_PERIPHCLK_I2C2
#define I2C2_RCC_CLOCK_SELECTION           RCC_I2C2CLKSOURCE_SYSCLK

#define I2C2_CLK_ENABLE()                  __HAL_RCC_I2C2_CLK_ENABLE()
#define I2C2_CLK_DISABLE()                 __HAL_RCC_I2C2_CLK_DISABLE()
#define I2C2_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C2_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOH_CLK_ENABLE()
#define I2C2_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C2_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOH_CLK_DISABLE()
#define I2C2_FORCE_RESET()                 __HAL_RCC_I2C2_FORCE_RESET()
#define I2C2_RELEASE_RESET()               __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2C2 Pins */
#define I2C2_GPIO_SCL_PIN                  GPIO_PIN_4
#define I2C2_GPIO_SDA_PIN                  GPIO_PIN_14
#define I2C2_GPIO_SCL_PORT                 GPIOH
#define I2C2_GPIO_SDA_PORT                 GPIOB
#define I2C2_GPIO_MODE                     GPIO_MODE_AF_OD
#define I2C2_GPIO_PULL                     GPIO_PULLUP
#define I2C2_GPIO_SPEED                    GPIO_SPEED_FREQ_VERY_HIGH
#define I2C2_GPIO_ALTERNATE                GPIO_AF4_I2C2

/* Definition for I2C2's NVIC */
#define I2C2_EV_IRQn                       I2C2_EV_IRQn
#define I2C2_ER_IRQn                       I2C2_ER_IRQn


#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_STM32L4_H */
