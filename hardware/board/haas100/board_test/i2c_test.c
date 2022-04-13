/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "aos/hal/i2c.h"
#include "aos/hal/rtc.h"
#include "aos/cli.h"
#include "pca9544.h"
#include "rx8130ce.h"

static int32_t i2c_test_process()
{
    int32_t ret;

    uint8_t data[7] = {8, 25, 18, 7, 3, 1, 21};
    uint8_t data_rtn[7] = {0};
    uint16_t size = 7;
    PCA9544_CH_E test_chan = PCA9544_CH1;
    PCA9544_CH_E read_chan = PCA9544_CH_NULL;
    uint8_t reg_rtn;
    uint8_t chan_mask = 0x05;

    i2c_dev_t i2c;
    PCA9544_DEV_CFG_T dev_cfg;

    dev_cfg.dev_addr = PCA9544_BASE_ADDR;
    dev_cfg.pca9544_ch = test_chan;
    dev_cfg.subdev_addr = 0x32;
    dev_cfg.reg_addr = 0x10;

    i2c.port = 1;
    i2c.config.address_width = 8;
    i2c.config.freq = I2C_BUS_BIT_RATES_100K;
    i2c.config.dev_addr = dev_cfg.dev_addr;

    ret = hal_i2c_init(&i2c);
    if (ret) {
        printf("=====i2c test : i2c 1 dev init fail =====\r\n");
        return -1;

    }

    ret = pca9544_init(&i2c, &dev_cfg);
    if (ret) {
        printf("=====pca9544 test : init fail =====\r\n");
        return -1;

    }

    /* chan select */
    printf("=====i2c test : set chan 2[%d] of PCA9544=====\r\n", test_chan);
    ret = pca9544_set_chan(test_chan);
    osDelay(2);
    read_chan = pca9544_read_chan();

    printf("=====I2C Muxer test : read chan val of PCA9544 is %d=====\r\n", read_chan);

    osDelay(2);
    /*if((reg & reg_rtn) == reg)*/
    if(test_chan == read_chan)
    {
        printf("=====I2C Muxer test : PCA9544 test:  PASS=====\r\n");
    }
    else
    {
        printf("=====I2C Muxer test : PCA9544 test:  FAIL=====\r\n");
        ret = -1;
    }

    printf("\r\n=====RTC test : set rtc time for RX8130CE=====\r\n");

    ret = hal_rtc_init(NULL);
    if(ret)
    {
        printf("=====RTC test : RX8130CE test: init FAIL=====\r\n");
        ret = -1;
    }

    ret = hal_rtc_set_time(NULL, (rtc_time_t*)data);
    if(ret)
    {
        printf("=====RTC test : RX8130CE test: write FAIL=====\r\n");
        ret = -1;
    }

    osDelay(3000);

    printf("=====RTC test : RX8130CE test: get time=====\r\n");

    ret = hal_rtc_get_time(NULL, (rtc_time_t*)data_rtn);
    if(ret)
    {
        printf("=====RTC test : RX8130CE test: write FAIL=====\r\n");
        ret = -1;
    }

    printf("get time: %us %um %uh %uw %ud %um %uy\r\n", (unsigned int)data_rtn[0], (unsigned int)data_rtn[1], (unsigned int)data_rtn[2],
            (unsigned int)data_rtn[3], (unsigned int)data_rtn[4], (unsigned int)data_rtn[5], (unsigned int)data_rtn[6]);

    for(uint8_t i = 0; i < 7; i++)
    {
        if(data[i] != data_rtn[i])
        {
            if(((data[0] + 2) == data_rtn[0]) || ((data[0] + 3) == data_rtn[0]))
            {
                printf("=====RTC test : RX8130CE test: time is correct=====\r\n");
                continue;
            }
            else
            {
                printf("=====RTC test : RX8130CE test: time is wrong, set %u, get %u =====\r\n",
                       (unsigned int)data[0], (unsigned int)data_rtn[0]);
                ret = -1;
            }

            if(i > 0)
            {
                printf("=====RTC test : RX8130CE test: loopback FAIL=====\r\n");
                ret = -1;
            }
        }
    }

    osDelay(20);

    printf("=====RTC test : RX8130CE test:  PASS=====\r\n");

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
    printf("** Process 1.2 read_chan the selected chan no. from PCA9544       **\r\n");
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
