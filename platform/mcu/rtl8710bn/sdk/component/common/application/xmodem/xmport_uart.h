/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _XMPORT_UART_H_
#define _XMPORT_UART_H_

#include "xmodem.h"

void xmodem_uart_init(u8 uart_idx, u8 pin_mux, u32 baud_rate);
void xmodem_uart_func_hook(XMODEM_COM_PORT *pXComPort);
void xmodem_uart_deinit(void);
char xmodem_uart_readable(void);
char xmodem_uart_writable(void);
char xmodem_uart_getc(void);
void xmodem_uart_putc(char c);

#endif  // end of "#define _XMPORT_UART_H_"

