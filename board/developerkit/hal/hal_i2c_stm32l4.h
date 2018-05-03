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

#define I2C_IRQ_PRIORITY                   2

/******************************** I2C1 configure *****************************/
#define AOS_PORT_I2C1                      1
#define I2C1_INSTANCE                      I2C1
#define I2C1_TIMING                        0x10909CEC
#define I2C1_OWN_ADDRESS1                  0x00
#define I2C1_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C1_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C1_OWNADDRESS2                   0x00
#define I2C1_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C1_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C1_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/* Definition for I2C1 clock resources */
#define I2C1_RCC_PERIPH_CLOCK_SELECTION    RCC_PERIPHCLK_I2C1
#define I2C1_RCC_CLOCK_SELECTION           RCC_I2C1CLKSOURCE_SYSCLK

#define I2C1_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define I2C1_CLK_DISABLE()                 __HAL_RCC_I2C1_CLK_DISABLE()
#define I2C1_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C1_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C1_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C1_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C1_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define I2C1_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2C1 Pins */
#define I2C1_GPIO_SCL_PIN                  GPIO_PIN_6
#define I2C1_GPIO_SDA_PIN                  GPIO_PIN_9
#define I2C1_GPIO_SCL_PORT                 GPIOB
#define I2C1_GPIO_SDA_PORT                 GPIOB
#define I2C1_GPIO_MODE                     GPIO_MODE_AF_OD
#define I2C1_GPIO_PULL                     GPIO_PULLUP
#define I2C1_GPIO_SPEED                    GPIO_SPEED_FREQ_VERY_HIGH
#define I2C1_GPIO_ALTERNATE                GPIO_AF4_I2C1

/* Definition for I2C1's NVIC */
#define I2C1_EV_IRQn                       I2C1_EV_IRQn
#define I2C1_ER_IRQn                       I2C1_ER_IRQn

/******************************** I2C2 configure *****************************/
#define AOS_PORT_I2C2                      2
#define I2C2_INSTANCE                      I2C2
#define I2C2_TIMING                        0x10909CEC
#define I2C2_OWN_ADDRESS1                  0x00
#define I2C2_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C2_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C2_OWNADDRESS2                   0x00
#define I2C2_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C2_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C2_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/* Definition for I2C2 clock resources */
#define I2C2_RCC_PERIPH_CLOCK_SELECTION    RCC_PERIPHCLK_I2C2
#define I2C2_RCC_CLOCK_SELECTION           RCC_I2C2CLKSOURCE_SYSCLK

#define I2C2_CLK_ENABLE()                  __HAL_RCC_I2C2_CLK_ENABLE()
#define I2C2_CLK_DISABLE()                 __HAL_RCC_I2C2_CLK_DISABLE()
#define I2C2_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C2_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C2_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C2_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C2_FORCE_RESET()                 __HAL_RCC_I2C2_FORCE_RESET()
#define I2C2_RELEASE_RESET()               __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2C2 Pins */
#define I2C2_GPIO_SCL_PIN                  GPIO_PIN_13
#define I2C2_GPIO_SDA_PIN                  GPIO_PIN_14
#define I2C2_GPIO_SCL_PORT                 GPIOB
#define I2C2_GPIO_SDA_PORT                 GPIOB
#define I2C2_GPIO_MODE                     GPIO_MODE_AF_OD
#define I2C2_GPIO_PULL                     GPIO_PULLUP
#define I2C2_GPIO_SPEED                    GPIO_SPEED_FREQ_VERY_HIGH
#define I2C2_GPIO_ALTERNATE                GPIO_AF4_I2C2

/* Definition for I2C2's NVIC */
#define I2C2_EV_IRQn                       I2C2_EV_IRQn
#define I2C2_ER_IRQn                       I2C2_ER_IRQn

/******************************** I2C3 configure *****************************/
#define AOS_PORT_I2C3                      3
#define I2C3_INSTANCE                      I2C3
#define I2C3_TIMING                        0x10909CEC
#define I2C3_OWN_ADDRESS1                  0x00
#define I2C3_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C3_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C3_OWNADDRESS2                   0x00
#define I2C3_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C3_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C3_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/* Definition for I2C3 clock resources */
#define I2C3_RCC_PERIPH_CLOCK_SELECTION    RCC_PERIPHCLK_I2C3
#define I2C3_RCC_CLOCK_SELECTION           RCC_I2C3CLKSOURCE_SYSCLK

#define I2C3_CLK_ENABLE()                  __HAL_RCC_I2C3_CLK_ENABLE()
#define I2C3_CLK_DISABLE()                 __HAL_RCC_I2C3_CLK_DISABLE()
#define I2C3_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2C3_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2C3_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define I2C3_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define I2C3_FORCE_RESET()                 __HAL_RCC_I2C3_FORCE_RESET()
#define I2C3_RELEASE_RESET()               __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2C3 Pins */
#define I2C3_GPIO_SCL_PIN                  GPIO_PIN_0
#define I2C3_GPIO_SDA_PIN                  GPIO_PIN_1
#define I2C3_GPIO_SCL_PORT                 GPIOC
#define I2C3_GPIO_SDA_PORT                 GPIOC
#define I2C3_GPIO_MODE                     GPIO_MODE_AF_OD
#define I2C3_GPIO_PULL                     GPIO_PULLUP
#define I2C3_GPIO_SPEED                    GPIO_SPEED_FREQ_VERY_HIGH
#define I2C3_GPIO_ALTERNATE                GPIO_AF4_I2C3

/* Definition for I2C3's NVIC */
#define I2C3_EV_IRQn                       I2C3_EV_IRQn
#define I2C3_ER_IRQn                       I2C3_ER_IRQn

/******************************** I2C4 configure *****************************/
#define AOS_PORT_I2C4                      4
#define I2C4_INSTANCE                      I2C4
#define I2C4_TIMING                        0x10909CEC
#define I2C4_OWN_ADDRESS1                  0x00
#define I2C4_ADDRESSING_MODE               I2C_ADDRESSINGMODE_7BIT
#define I2C4_DUAL_ADDRESS_MODE             I2C_DUALADDRESS_DISABLE
#define I2C4_OWNADDRESS2                   0x00
#define I2C4_OWN_ADDRESS2_MASK             I2C_OA2_NOMASK
#define I2C4_GENERAL_CALL_MODE             I2C_GENERALCALL_DISABLE
#define I2C4_NO_STRETCH_MODE               I2C_NOSTRETCH_DISABLE

/* Definition for I2C4 clock resources */
#define I2C4_RCC_PERIPH_CLOCK_SELECTION    RCC_PERIPHCLK_I2C4
#define I2C4_RCC_CLOCK_SELECTION           RCC_I2C4CLKSOURCE_SYSCLK

#define I2C4_CLK_ENABLE()                  __HAL_RCC_I2C4_CLK_ENABLE()
#define I2C4_CLK_DISABLE()                 __HAL_RCC_I2C4_CLK_DISABLE()
#define I2C4_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define I2C4_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define I2C4_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()
#define I2C4_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()
#define I2C4_FORCE_RESET()                 __HAL_RCC_I2C4_FORCE_RESET()
#define I2C4_RELEASE_RESET()               __HAL_RCC_I2C4_RELEASE_RESET()

/* Definition for I2C4 Pins */
#define I2C4_GPIO_SCL_PIN                  GPIO_PIN_12
#define I2C4_GPIO_SDA_PIN                  GPIO_PIN_13
#define I2C4_GPIO_SCL_PORT                 GPIOD
#define I2C4_GPIO_SDA_PORT                 GPIOD
#define I2C4_GPIO_MODE                     GPIO_MODE_AF_OD
#define I2C4_GPIO_PULL                     GPIO_PULLUP
#define I2C4_GPIO_SPEED                    GPIO_SPEED_FREQ_VERY_HIGH
#define I2C4_GPIO_ALTERNATE                GPIO_AF4_I2C4

/* Definition for I2C4's NVIC */
#define I2C4_EV_IRQn                       I2C4_EV_IRQn
#define I2C4_ER_IRQn                       I2C4_ER_IRQn


#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_STM32L4_H */
