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
#include "aos/kernel.h"
#include "board.h"

typedef enum {
    GPIO_PinState_Reset = 0,
    GPIO_PinState_Set = !GPIO_PinState_Reset,
}GPIO_pinstate;

typedef struct{ 
     uint16_t port;      //Logical Port
     void *   GpioGroup; //GPIOA, GPIOB
     uint16_t Pin;       /*!< Specifies the GPIO pins to be configured.This parameter can be any value of @ref GPIO_pins_define */
     uint16_t Speed;     /*!< Specifies the speed for the selected pins.This parameter can be a value of @ref GPIO_speed_define */
     GPIO_pinstate  defaultLogicalVal;//only used for output
}GPIO_MAPPING;

typedef enum {
	GPIO_LED2,
	GPIO_LED3,
	GPIO_USER_BTN,
    GPIO_TEST_IN,
    GPIO_TEST_OUT,
    TOTAL_GPIO_NUM
} BOARD_GPIO;

#ifdef __cplusplus
}
#endif

#endif /* __HAL_GPIO_STM32L4_H */
