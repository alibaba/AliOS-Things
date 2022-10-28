/*
 *  * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include "stm32u5xx_hal.h"
extern UART_HandleTypeDef huart1;


/* use for printk */
int alios_debug_print(const char *buf, int size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        HAL_UART_Transmit(&huart1, (uint8_t *)buf + i, 1, 0xFFFF);
    }
    return size;
}

