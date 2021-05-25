/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 *
 */

#ifndef _TRACE_UART_H_
#define _TRACE_UART_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef bool (*UART_TX_CB)(void);

bool trace_uart_init(void);

bool trace_uart_deinit(void);

bool trace_uart_tx(uint8_t *p_buf, uint16_t len, UART_TX_CB tx_cb);

#ifdef __cplusplus
}
#endif

#endif

