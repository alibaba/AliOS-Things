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

struct _xmodem_uart_map {
	void (*xmodem_uart_init)(u8 uart_idx, u8 pin_mux, u32 baud_rate);
	void (*xmodem_uart_deinit)(void);
	void (*xmodem_uart_port_init)(u8 uart_idx, u8 pin_mux, u32 baud_rate);
	void (*xmodem_uart_port_deinit)(u8 uart_idx);
	char (*xmodem_uart_readable)(void);
	char (*xmodem_uart_writable)(void);
	char (*xmodem_uart_getc)(void);
	void (*xmodem_uart_putc)(char c);
	void (*xmodem_uart_putdata)(u8* buf, u32 cnt);
	char (*xmodem_uart_getc_to)(char *pch, u32 timeout);
	void (*xmodem_uart_clean_rx)(void);
};

extern UART_TypeDef* xmodem_uartx;

_LONG_CALL_ void xmodem_uart_init(u8 uart_idx, u8 pin_mux, u32 baud_rate);
_LONG_CALL_ void xmodem_uart_deinit(void) ;
_LONG_CALL_ void xmodem_uart_port_init(u8 uart_idx, u8 pin_mux, u32 baud_rate);
_LONG_CALL_ void xmodem_uart_port_deinit(u8 uart_idx);
_LONG_CALL_ char xmodem_uart_readable(void) ;
_LONG_CALL_ char xmodem_uart_writable(void) ;
_LONG_CALL_ char xmodem_uart_getc(void) ;
_LONG_CALL_ void xmodem_uart_putc(char c) ;
_LONG_CALL_ void xmodem_uart_putdata(u8* buf, u32 cnt) ;
_LONG_CALL_ char xmodem_uart_getc_to(char *pch, u32 timeout);
_LONG_CALL_ void xmodem_uart_clean_rx(void);

#endif  // end of "#define _XMPORT_UART_H_"

