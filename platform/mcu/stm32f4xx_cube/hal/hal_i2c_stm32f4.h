/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_I2C_STM32F4_H
#define __HAL_I2C_STM32F4_H

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

#define HAL_I2C_GPIO_NEED_MAP 0xfe

typedef struct{
    PORT_I2C_TYPE  i2cFuncP;
    void*          i2cPhyP;
    uint16_t needmap;
    uint16_t gpiomaps[2];
}I2C_MAPPING;

extern I2C_MAPPING i2c_mapping_table[];


#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_STM32L4_H */
