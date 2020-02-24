/**
  ******************************************************************************
  * @file    hal_uart_stm32f1.h
  * @author  MCU China FAE team
  * @version 1.0
  * @date    05/01/2019
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  ******************************************************************************
  */

#ifndef __HAL_UART_STM32F1_H
#define __HAL_UART_STM32F1_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "board.h"

typedef enum {
    UART_TX,
    UART_RX,
    UART_CTS,
    UART_RTS,
    UART_CK,
} PIN_NAME_TYPE;

typedef struct {
    uint8_t pin_name;
    uint8_t pin;
} gpio_uart_pin_config_t;

typedef struct{
    uint32_t       overSampling;
    uint32_t       max_buf_bytes;  //the size of UartRxBuf used by driver
}uartAttribute;

typedef struct{
    PORT_UART_TYPE uartFuncP;
    void*          uartPhyP;
    uartAttribute  attr;
#if (HAL_VERSION >= 30100)
    gpio_uart_pin_config_t *pin_conf;
    uint8_t                 pin_cnt;
#endif
}UART_MAPPING;

//Mapping Table is defined in soc_init.c
extern UART_MAPPING UART_MAPPING_TABLE[PORT_UART_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */
