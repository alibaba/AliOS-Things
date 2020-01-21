/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define HARDWARE_REVISION   "V1.0"
#define MODEL               "BK7231"

#ifdef BOOTLOADER
#define STDIO_UART 1
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART 1
#define STDIO_UART_BUADRATE 115200
#endif

#define ATCMD_UART 0
#define ATCMD_UART_BUADRATE 115200
