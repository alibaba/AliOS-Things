/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _XMPORT_LOGUART_H_
#define _XMPORT_LOGUART_H_

#include "xmodem.h"

//void xmodem_loguart_init(void);
void xmodem_loguart_init(u32 BaudRate);
void xmodem_loguart_func_hook(XMODEM_COM_PORT *pXComPort);
void xmodem_loguart_deinit(void);
char xmodem_loguart_readable(void);
char xmodem_loguart_writable(void);
char xmodem_loguart_getc(void);
void xmodem_loguart_putc(char c);

#endif  // end of "#define _XMPORT_LOGUART_H_"

