/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/

#ifndef __LIBPRINTF_H__
#define __LIBPRINTF_H__

#include "lib_uart.h"
#include <stdio.h>
#include "type.h"


#ifdef __clang__

ErrorStatus UART_printf(uint8_t *Data,...);

#elif defined __CC_ARM

#define UART_printf  printf

#endif

#endif

/*************************END OF FILE**********************/
