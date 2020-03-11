/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_UART_XMC4800_H
#define __HAL_UART_XMC4800_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"

#include "board.h"



typedef struct{    
    uint16_t       irqNumber;
    uint16_t       pPri;
    uint16_t       sPri;
    uint32_t       overSampling;
}uartAttribute;


typedef struct{
    PORT_UART_TYPE uartFuncP;
    void*          uartPhyP; 
    uartAttribute  attr;
}UART_MAPPING;

//Mapping Table is defined in soc_init.c
extern UART_MAPPING UART_MAPPING_TABLE[PORT_UART_SIZE];



#define MAX_BUF_UART_BYTES  16384
#define MAX_BUF_UART_DMA_BYTES  4000

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_XMC4800_H */
