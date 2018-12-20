/***************************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
***************************************************************/

#ifndef __LIBPRINTF_H__
#define __LIBPRINTF_H__

#include "lib_uart.h"
#include <stdio.h>

int fputc(int ch, FILE *f);
static char *itoa(int value, char *string, int radix);
ErrorStatus UART_printf(UART_TypeDef* UARTx, uint8_t *Data,...);

#endif

/*************************END OF FILE**********************/
