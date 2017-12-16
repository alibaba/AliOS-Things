/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include <uart.h>
#include <log.h>

extern void CK_INTC_Init(void);
extern void CK_Exception_Init(void);
extern void ck_uart_putchar(uint8_t ch);

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


