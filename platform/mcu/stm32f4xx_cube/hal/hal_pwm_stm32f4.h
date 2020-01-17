/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_PWM_STM32F4_H
#define __HAL_PWM_STM32F4_H

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

#define PORT_PWM0 0
#define PORT_PWM1 1

typedef struct{
    PORT_PWM_TYPE   logical_func;
    void*           physical_port;
    uint32_t        channel;
    uint32_t        gpio_group;
    uint32_t        gpio_pin;
}PWM_MAPPING;

extern PWM_MAPPING PWM_MAPPING_TABLE[PORT_PWM_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PWM_STM32F4_H */
