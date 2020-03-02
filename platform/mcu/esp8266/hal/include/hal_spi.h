/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __HAL_SPI_H
#define __HAL_SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "spi_interface.h"
#include "board.h"

typedef struct{
    PORT_SPI_TYPE  spiFuncP;
    SpiNum         spiPhyP;
    SpiSubMode     subMode;
    SpiBitOrder    bitOrder;
}SPI_MAPPING;

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SPI_H */
