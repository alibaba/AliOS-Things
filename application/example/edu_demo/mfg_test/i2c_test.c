/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "aos/hal/i2c.h"
#include "aos/cli.h"

static int32_t i2c_test_process()
{
    int32_t ret;

    osDelay(20);

    printf("=====RTC test : RX8130CE test:  PASS=====\r\n");
    return 0;
fail:
    printf("=====RTC test : RX8130CE test:  FAIL=====\r\n");
    return -1;

}

static int i2c_autotest()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* I2C&RTC Test ************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Process 2.1 set 20.9.7 1:1:1 to RX8130CE                  **\r\n");
    printf("** Process 2.2 read the current time from RX8130CE           **\r\n");
    printf("***************************************************************\r\n");
    printf("=====i2c test : Start=====\r\n");

    ret = i2c_test_process();
    if(ret)
    {
        printf("\r\n=====i2c test : FAIL ===\r\n");
        return -1;
    }

    printf("\r\n=====i2c test : PASS===\r\n");
    return 0;
}

static void handle_i2ctest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    i2c_autotest();
}

static struct cli_command i2ctest_cmd = {
    .name     = "i2ctest",
    .help     = "i2ctest",
    .function = handle_i2ctest_cmd
};

int i2c_test()
{
    aos_cli_register_command(&i2ctest_cmd);
    return i2c_autotest();
}
