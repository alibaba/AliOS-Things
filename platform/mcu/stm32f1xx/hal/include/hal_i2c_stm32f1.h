/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __HAL_I2C_STM32F1_H
#define __HAL_I2C_STM32F1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "board.h"

#define HAL_I2C_GPIO_NEED_MAP 0xfe

typedef struct {
    PORT_I2C_TYPE  i2c_func_p;
    void*          i2c_physic_p;

    uint16_t needmap;
    uint16_t gpiomaps[2];
}i2c_mapping_t;

extern i2c_mapping_t i2c_mapping[PORT_I2C_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_STM32F1_H */
