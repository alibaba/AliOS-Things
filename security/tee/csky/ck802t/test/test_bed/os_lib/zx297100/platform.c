/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <uart.h>
#include <log.h>

extern void CK_INTC_Init(void);
extern void CK_Exception_Init(void);
extern void ck_uart_putchar(uint8_t ch);
extern void watchdog_start(void);

int putch(char c)
{
    ck_uart_putchar(c);
    return 0;
}

/*
 * initialize the device registered
 */
void CK_Platform_Init(void)
{
    CK_INTC_Init();
    CK_Exception_Init();
}

void hal_system_reset(void)
{
    watchdog_start();
    while (1)
        ;
}

// vincent add
extern void wsc_entry(void);

__attribute__((__section__(".wsc.data")))
const void (*ebr_except_table[1])(void) = { (void *)wsc_entry };
//
