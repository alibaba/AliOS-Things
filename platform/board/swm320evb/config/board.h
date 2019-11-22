/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __BOARD__H_
#define __BOARD__H_

#define HARDWARE_REVISION   "V1.0"
#define MODEL               "SWM320"

#ifdef BOOTLOADER
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 115200
#endif

typedef enum{
    PORT_UART_STD,
    PORT_UART_AT,
    PORT_UART_ARDUINO,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255,
}PORT_UART_TYPE;

void flash_partition_init(void);

#endif /*__BOARD__H_*/
