/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_TIMER_STM32F4_H
#define __HAL_TIMER_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "aos/kernel.h"
#include "board.h"

#define PORT_TIMER0 0
#define PORT_TIMER1 1
#define PORT_TIMER2 2
#define PORT_TIMER3 3
#define PORT_TIMER4 4

typedef struct{
    PORT_TIMER_TYPE  logical_func;
    void*            physical_port;
}TIMER_MAPPING;

extern TIMER_MAPPING TIMER_MAPPING_TABLE[PORT_TIMER_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_TIMER_STM32L4_H */
