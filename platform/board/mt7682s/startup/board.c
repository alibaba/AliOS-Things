/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <k_api.h>

#include "aos/hal/uart.h"

/* hal includes */
#include "hal.h"
#include "hal_platform.h"

extern uart_dev_t uart_0;
extern void platform_init(void);
extern void hw_wifi_init(void);

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    //hal_uart_send(&uart_0,(char *)&ch,1,0);
    return ch;
}

void board_init(void)
{
    platform_init();

    hw_wifi_init();
}
