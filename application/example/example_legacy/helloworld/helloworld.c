/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>

int application_start(int argc, char *argv[])
{
    int count = 0;
    printf("nano entry here!\r\n");

    while(1) {
        printf("hello world! count %d \r\n", count++);

        aos_msleep(1000);
    };
}
