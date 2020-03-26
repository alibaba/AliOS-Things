/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __HAL_SPI_AAMCU_H
#define __HAL_SPI_AAMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "board.h"

#define HAL_SPI_GPIO_NEED_MAP 0xfe

typedef enum {
    SPI_PIN_CS,
    SPI_PIN_CLK,
    SPI_PIN_MOSI,
    SPI_PIN_MISO
} SPI_PIN_NAME_TYPE;

typedef struct {
    uint8_t  pin_name;
    uint16_t pin;
} gpio_spi_pin_config_t;

typedef struct{
    PORT_SPI_TYPE          spiFuncP;
    void*                  spiPhyP;
    spiAttribute           attr;
    uint16_t               needmap;
    gpio_spi_pin_config_t *pin_conf;
    uint16_t               pin_cnt;
}SPI_MAPPING;

extern SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SPI_AAMCU_H */
