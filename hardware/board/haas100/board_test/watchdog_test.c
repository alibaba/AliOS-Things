/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"
#include "led.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"

static void timer1_func(void *timer, void *arg)
{
    static int times = 1;
    printf("\r\n%.1f s\r\n", times * 0.2);
    times ++;
}

static void handle_watchdog_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_timer_t timer1;
    int status;

    if(argc < 2) {
        printf("Usage: watchdog stopfeed\n");
        printf("Example: watchdog stopfeed\n");
        return;
    }

    if(0 == strcmp(argv[1], "stopfeed")) {
        printf("begin stop feed dog, board will reboot in 1.6 s ...\n");
        status = aos_timer_new(&timer1, timer1_func, NULL, 200, 1);
        if (status != 0) {
            printf("create timer error\n");
            return;
        }
        watchdog_stopfeed();
    } else {
        printf("Usage: watchdog stopfeed\n");
        printf("Example: watchdog stopfeed\n");
        return;
    }
}

static struct cli_command watchdog_test_cmd = {
    .name     = "watchdog",
    .help     = "watchdog [stopfeed]",
    .function = handle_watchdog_test_cmd
};

void watchdog_test()
{
    int ret = 0;
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** watchdog Test *************************\r\n");
    printf("** How to test: input [watchdog stopfeed] command *************\r\n");
    printf("***************************************************************\r\n");
    printf("=====watchdog test : Start=====\r\n");

    aos_cli_register_command(&watchdog_test_cmd);
    watchdog_stopfeed();
}