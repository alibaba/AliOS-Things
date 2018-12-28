/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>

#include <k_api.h>
#include "debug.h"
#include <aos/hal/uart.h>

static char debug_tx_buf[128];
extern uart_dev_t uart_1;

#if (RHINO_CONFIG_PANIC > 0)
//use DBG_Send in exception print
void print_str() __attribute__ ((alias ("dbg_send")));
#endif

void dbg_send(const char *format, ...)
{
    va_list args;
    uint8_t len;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    va_start(args, format);
    len = vsnprintf(debug_tx_buf, sizeof(debug_tx_buf), format, args);
    va_end(args);
    RHINO_CPU_INTRPT_ENABLE();
    hal_uart_send(&uart_1, (void *)debug_tx_buf, len, 500);
}

void _printf_float()
{
}

