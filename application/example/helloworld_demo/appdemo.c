/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("nano entry here!\r\n");

    //fd = board_lcd_create("name");
    //board_lcd_write(fd,buffer,len);

    while(1) {
        printf("hello world! count %d \r\n", count++);

        aos_msleep(1000);
    };
}

