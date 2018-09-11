/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_UART_STM32F4_H
#define __HAL_UART_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "hal/hal.h"
#include "board.h"



typedef struct{    
    uint32_t       overSampling;
	uint32_t       max_buf_bytes;  //the size of UartRxBuf used by driver
}uartAttribute;


typedef struct{
    PORT_UART_TYPE uartFuncP; 
    void*          uartPhyP; 
    uartAttribute  attr;
}UART_MAPPING;

//Mapping Table is defined in soc_init.c
extern UART_MAPPING UART_MAPPING_TABLE[PORT_UART_SIZE];

extern void USART3_DMA_RX_IRQHandler(void);
extern void USART3_DMA_TX_IRQHandler(void);
extern void USART6_DMA_RX_IRQHandler(void);
extern void USART6_DMA_TX_IRQHandler(void);
extern void USART_DMA_RX_IRQHandler(const void* uartIns);
extern void USART_DMA_TX_IRQHandler(const void* uartIns);
#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */
