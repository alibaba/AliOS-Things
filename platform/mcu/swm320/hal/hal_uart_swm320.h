/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_UART_SWM320_H
#define __HAL_UART_SWM320_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "SWM320_uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "board.h"
#include "SWM320.h"

typedef struct{
    uint32_t       overSampling;
    uint32_t       OneBitSampling;
    uint32_t       AdvFeatureInit;
    uint32_t       max_buf_bytes;  //the size of UartRxBuf used by driver
}uartAttribute;


typedef struct{
    PORT_UART_TYPE uartFuncP;
    void*          uartPhyP;
    uartAttribute  attr;
}UART_MAPPING;

//Mapping Table is defined in soc_init.c
extern UART_MAPPING UART_MAPPING_TABLE[PORT_UART_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_SWM320_H */
