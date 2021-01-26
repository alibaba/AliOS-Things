/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>

extern int tetris_main(int argc, char *argv[]);

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("start to play tetris game!\r\n");
    tetris_main(0, NULL);

    while(1) {
        printf("hello world! count %d \r\n", count++);

        aos_msleep(1000);
    };
}

