/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_QSPI_STM32L4_H
#define __HAL_QSPI_STM32L4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"

/* qspi port defines */
#define PORT_QSPI1 1
#define PORT_QSPI2 2
#define PORT_QSPI3 3
#define PORT_QSPI4 4

/* cmd defines in function hal_qspi_command */
#define CMD_QUAD_PAGE_PROG 1

/* cmd defines in function hal_qspi_autoPolling */
#define AP_GET_MEM_STATUS          1
#define AP_GET_WRITE_ENABLE_STATUS 2	

#ifdef __cplusplus
}
#endif

#endif /* __HAL_QSPI_STM32L4_H */
