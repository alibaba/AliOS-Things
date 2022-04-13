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

    char data[7] = {0x01, 1, 1, 1, 7, 9, 20};
    char data_rtn[7] = {0};
    uint16_t size = 7;
    uint8_t reg = 0x05;
    uint8_t reg_rtn;
    uint8_t chan_mask = 0x05;

    i2c_dev_t i2c;
    i2c.port = 1;
    i2c.config.address_width = 8;
    i2c.config.freq = I2C_BUS_BIT_RATES_100K;
    i2c.config.dev_addr = 0x70;
    ret = hal_i2c_init(&i2c);
    if (ret) {
        printf("=====i2c test : i2c 1 dev init fail =====\r\n");
        return -1;

    }

    /* chan select */
    printf("=====i2c test : set chan 2[%d] of PCA9544=====\r\n", reg);
    hal_i2c_master_send(&i2c, i2c.config.dev_addr, &reg, 1, 1000);
    osDelay(2);
    hal_i2c_master_recv(&i2c, i2c.config.dev_addr, &reg_rtn, 1, 1000);
    reg_rtn &= chan_mask;
    printf("=====I2C Muxer test : read chan val of PCA9544 is %d=====\r\n", reg_rtn);
    osDelay(2);
    if((reg & reg_rtn) == reg)
    {
        printf("=====I2C Muxer test : PCA9544 test:  PASS=====\r\n");
    }
    else
    {
        printf("=====I2C Muxer test : PCA9544 test:  FAIL=====\r\n");
        ret = -1;
    }

    printf("\r\n=====RTC test : set rtc time for RX8310CE=====\r\n", reg);
    hal_i2c_mem_write(&i2c, 0x32, 0x10, 1,&data[0], 1, 1000);
    hal_i2c_mem_write(&i2c, 0x32, 0x11, 1,&data[1], 1, 1000);
    hal_i2c_mem_write(&i2c, 0x32, 0x12, 1,&data[2], 1, 1000);
    hal_i2c_mem_write(&i2c, 0x32, 0x13, 1,&data[3], 1, 1000);
    hal_i2c_mem_write(&i2c, 0x32, 0x14, 1,&data[4], 1, 1000);
    hal_i2c_mem_write(&i2c, 0x32, 0x15, 1,&data[5], 1, 1000);
    hal_i2c_mem_write(&i2c, 0x32, 0x16, 1,&data[6], 1, 1000);
    osDelay(20);

    hal_i2c_mem_read(&i2c, 0x32, 0x10, 1,&data_rtn[0], 1, 1000);
    hal_i2c_mem_read(&i2c, 0x32, 0x11, 1,&data_rtn[1], 1, 1000);
    hal_i2c_mem_read(&i2c, 0x32, 0x12, 1,&data_rtn[2], 1, 1000);
    hal_i2c_mem_read(&i2c, 0x32, 0x13, 1,&data_rtn[3], 1, 1000);
    hal_i2c_mem_read(&i2c, 0x32, 0x14, 1,&data_rtn[4], 1, 1000);
    hal_i2c_mem_read(&i2c, 0x32, 0x15, 1,&data_rtn[5], 1, 1000);
    hal_i2c_mem_read(&i2c, 0x32, 0x16, 1,&data_rtn[6], 1, 1000);

    for(int i = 0; i < 7; i++)
    {
        if(data[i] != data_rtn[i])
        {
            printf("=====RTC test : RX8310CE test:  FAIL=====\r\n");
            ret = -1;
        }
    }

    printf("=====RTC test : RX8310CE test:  PASS=====\r\n");

    hal_i2c_finalize(&i2c);

    return 0;

}

static int i2c_autotest()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*************************** I2C&RTC Test **************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Note: this test don't need to connect any external tool   **\r\n");
    printf("** How to test: this test is a autotest                      **\r\n");
    printf("** Process 1.1 set chan1(0x05) for PCA9544 by i2c            **\r\n");
    printf("** Process 1.2 read the selected chan no. from PCA9544       **\r\n");
    printf("** Process 2.1 set 20.9.7 1:1:1 to RX8310CE                  **\r\n");
    printf("** Process 2.2 read the current time from RX8310CE           **\r\n");
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
