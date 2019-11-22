/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define HARDWARE_REVISION   "V1.0"
#define MODEL               "EMW3060"

#ifdef BOOTLOADER
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 921600
#else
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 921600
#endif
