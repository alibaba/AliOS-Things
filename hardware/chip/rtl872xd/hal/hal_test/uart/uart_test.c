/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/uart.h"
#include "uart_test.h"
#include "ymodem.h"

uart_dev_t uart_dev;

void hal_uart_test(void)
{
    int32_t ret = -1;

    printf("*********** uart test start ! ***********\n");

    printf("send file from the PC by ymodem protocol !\n");

    ret = SerialDownload();
    if (ret > 0) {
        printf("uart test result: succeed !\n"); 
    } else {
        printf("uart test result: failed !\n"); 
    }

    printf("*********** uart test end ! ***********\n");
}
