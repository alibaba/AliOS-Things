/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_GPIO_STM32F4_H
#define __HAL_GPIO_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "aos/hal/gpio.h"
#include "aos/kernel.h"

#include "board.h"

typedef struct{
    uint16_t    port;  //Logical Port
    void*       GpioGroup; //GPIOA, GPIOB
    uint16_t Pin;       /*!< Specifies the GPIO pins to be configured.
                            This parameter can be any value of @ref GPIO_pins_define */
    uint16_t Speed;     /*!< Specifies the speed for the selected pins.
                            This parameter can be a value of @ref GPIO_speed_define */
    gpio_pinstate_t  defaultLogicalVal;//only used for output

}gpio_mapping_t;

typedef  struct gpio_irq_slop {
    int32_t pin_num;
    int32_t irq_num;
    gpio_irq_handler_t handler;
    void * priv;
} gpio_irq_slop_t;

extern  const gpio_mapping_t gpio_mapping_table[];

#define PINS_IN_GROUP  16
#define GROUP_GPIOA    0
#define GROUP_GPIOB    1
#define GROUP_GPIOC    2
#define GROUP_GPIOD    3
#define GROUP_GPIOE    4

#define HAL_GPIO_0                 ((uint8_t)0)        /* represent GPIOA pin 0 */
#define HAL_GPIO_1                 ((uint8_t)1)        /* represent GPIOA pin 1 */
#define HAL_GPIO_2                 ((uint8_t)2)        /* represent GPIOA pin 2 */
#define HAL_GPIO_3                 ((uint8_t)3)        /* represent GPIOA pin 3 */
#define HAL_GPIO_4                 ((uint8_t)4)        /* represent GPIOA pin 4 */
#define HAL_GPIO_5                 ((uint8_t)5)        /* represent GPIOA pin 5 */
#define HAL_GPIO_6                 ((uint8_t)6)        /* represent GPIOA pin 6 */
#define HAL_GPIO_7                 ((uint8_t)7)        /* represent GPIOA pin 7 */
#define HAL_GPIO_8                 ((uint8_t)8)        /* represent GPIOA pin 8 */
#define HAL_GPIO_9                 ((uint8_t)9)        /* represent GPIOA pin 9 */
#define HAL_GPIO_10                ((uint8_t)10)       /* represent GPIOA pin 10 */
#define HAL_GPIO_11                ((uint8_t)11)       /* represent GPIOA pin 11 */
#define HAL_GPIO_12                ((uint8_t)12)       /* represent GPIOA pin 12 */
#define HAL_GPIO_13                ((uint8_t)13)       /* represent GPIOA pin 13 */
#define HAL_GPIO_14                ((uint8_t)14)       /* represent GPIOA pin 14 */
#define HAL_GPIO_15                ((uint8_t)15)       /* represent GPIOA pin 15 */

#define HAL_GPIO_16                ((uint8_t)16)       /* represent GPIOB pin 0 */
#define HAL_GPIO_17                ((uint8_t)17)       /* represent GPIOB pin 1 */
#define HAL_GPIO_18                ((uint8_t)18)       /* represent GPIOB pin 2 */
#define HAL_GPIO_19                ((uint8_t)19)       /* represent GPIOB pin 3 */
#define HAL_GPIO_20                ((uint8_t)20)       /* represent GPIOB pin 4 */
#define HAL_GPIO_21                ((uint8_t)21)       /* represent GPIOB pin 5 */
#define HAL_GPIO_22                ((uint8_t)22)       /* represent GPIOB pin 6 */
#define HAL_GPIO_23                ((uint8_t)23)       /* represent GPIOB pin 7 */
#define HAL_GPIO_24                ((uint8_t)24)       /* represent GPIOB pin 8 */
#define HAL_GPIO_25                ((uint8_t)25)       /* represent GPIOB pin 9 */
#define HAL_GPIO_26                ((uint8_t)26)       /* represent GPIOB pin 10 */
#define HAL_GPIO_27                ((uint8_t)27)       /* represent GPIOB pin 11 */
#define HAL_GPIO_28                ((uint8_t)28)       /* represent GPIOB pin 12 */
#define HAL_GPIO_29                ((uint8_t)29)       /* represent GPIOB pin 13 */
#define HAL_GPIO_30                ((uint8_t)30)       /* represent GPIOB pin 14 */
#define HAL_GPIO_31                ((uint8_t)31)       /* represent GPIOB pin 15 */

#define HAL_GPIO_32                ((uint8_t)32)       /* represent GPIOC pin 0 */
#define HAL_GPIO_33                ((uint8_t)33)       /* represent GPIOC pin 1 */
#define HAL_GPIO_34                ((uint8_t)34)       /* represent GPIOC pin 2 */
#define HAL_GPIO_35                ((uint8_t)35)       /* represent GPIOC pin 3 */
#define HAL_GPIO_36                ((uint8_t)36)       /* represent GPIOC pin 4 */
#define HAL_GPIO_37                ((uint8_t)37)       /* represent GPIOC pin 5 */
#define HAL_GPIO_38                ((uint8_t)38)       /* represent GPIOC pin 6 */
#define HAL_GPIO_39                ((uint8_t)39)       /* represent GPIOC pin 7 */
#define HAL_GPIO_40                ((uint8_t)40)       /* represent GPIOC pin 8 */
#define HAL_GPIO_41                ((uint8_t)41)       /* represent GPIOC pin 9 */
#define HAL_GPIO_42                ((uint8_t)42)       /* represent GPIOC pin 10 */
#define HAL_GPIO_43                ((uint8_t)43)       /* represent GPIOC pin 11 */
#define HAL_GPIO_44                ((uint8_t)44)       /* represent GPIOC pin 12 */
#define HAL_GPIO_45                ((uint8_t)45)       /* represent GPIOC pin 13 */
#define HAL_GPIO_46                ((uint8_t)46)       /* represent GPIOC pin 14 */
#define HAL_GPIO_47                ((uint8_t)47)       /* represent GPIOC pin 15 */

#define HAL_GPIO_48                ((uint8_t)48)       /* represent GPIOD pin 0 */
#define HAL_GPIO_49                ((uint8_t)49)       /* represent GPIOD pin 1 */
#define HAL_GPIO_50                ((uint8_t)50)       /* represent GPIOD pin 2 */
#define HAL_GPIO_51                ((uint8_t)51)       /* represent GPIOD pin 3 */
#define HAL_GPIO_52                ((uint8_t)52)       /* represent GPIOD pin 4 */
#define HAL_GPIO_53                ((uint8_t)53)       /* represent GPIOD pin 5 */
#define HAL_GPIO_54                ((uint8_t)54)       /* represent GPIOD pin 6 */
#define HAL_GPIO_55                ((uint8_t)55)       /* represent GPIOD pin 7 */
#define HAL_GPIO_56                ((uint8_t)56)       /* represent GPIOD pin 8 */
#define HAL_GPIO_57                ((uint8_t)57)       /* represent GPIOD pin 9 */
#define HAL_GPIO_58                ((uint8_t)58)       /* represent GPIOD pin 10 */
#define HAL_GPIO_59                ((uint8_t)59)       /* represent GPIOD pin 11 */
#define HAL_GPIO_60                ((uint8_t)60)       /* represent GPIOD pin 12 */
#define HAL_GPIO_61                ((uint8_t)61)       /* represent GPIOD pin 13 */
#define HAL_GPIO_62                ((uint8_t)62)       /* represent GPIOD pin 14 */
#define HAL_GPIO_63                ((uint8_t)63)       /* represent GPIOD pin 15 */

#define ON_BOARD_LED01 HAL_GPIO_16
#define ON_BOARD_LED02 HAL_GPIO_23
#define ON_BOARD_LED03 HAL_GPIO_30
#define ON_BOARD_TIM4_CH4   HAL_GPIO_63

#define GPIOA_SPEED    GPIO_SPEED_FREQ_VERY_HIGH

#ifdef __cplusplus
}
#endif

#endif /* __HAL_GPIO_STM32L4_H */
