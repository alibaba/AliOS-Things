/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "aos/hal/adc.h"
#include "aos/cli.h"
struct adc_check{
    adc_dev_t st_adc_info;
    uint32_t test_threshold;
    uint32_t test_jitter;
};

static struct adc_check edu_adc_dev[3] =
{
    {{0, 1000, 0x12345678}, 200, 250}, // mic adc
    {{1, 1000, 0x12345678}, 1500, 300}, // vol
    {{2, 1000, 0x12345678}, 1100, 300}, // extra IO
};

#define TEST_VOLT_MV 1500

static int32_t adc_test_process(uint32_t port)
{
    int32_t ret = 0;
    uint32_t output = 0;
    uint32_t test_sum = 0;
    uint32_t test_avrg = 0;
    uint32_t test_min = 3300;
    uint32_t test_max = 0;

    ret = hal_adc_init(&edu_adc_dev[port].st_adc_info);
    if (ret)
    {
        printf("\r\n=====adc test : adc init failed===\r\n");
        return -1;
    }

    for (int32_t i = 0; i < 34; i++)
    {
        hal_adc_value_get(&edu_adc_dev[port].st_adc_info, &output, 1000);
        test_sum += output;

        /* the min sampling voltage */
        if (test_min >= output)
        {
            test_min = output;
        }
        /* the max sampling voltage */
        if (test_max <= output)
        {
            test_max = output;
        }

        osDelay(20);
    }

    hal_adc_finalize(&edu_adc_dev[port].st_adc_info);

    test_avrg = (test_sum - test_min - test_max) >> 5;

    printf("\r\n=====adc test : the samping volage is:%dmv===\r\n", test_avrg);
    if(port != 0){
        if (((test_avrg - edu_adc_dev[port].test_jitter) > edu_adc_dev[port].test_threshold) || ((test_avrg + edu_adc_dev[port].test_jitter) < edu_adc_dev[port].test_threshold))
        {
            printf("\r\n=====adc test : the samping volage is out of scope===\r\n");
            printf("\r\n=====adc test : FAIL===\r\n");
            return -1;
        }
    }else{
        if(test_avrg < 1)
        {
            printf("\r\n=====adc test : the samping volage is out of scope===\r\n");
            printf("\r\n=====adc test : FAIL===\r\n");
            return -1;
        }
        //goto
    }
    printf("=====adc test : Done=====\r\n");

    return 0;
}

static int adc_autotest(uint32_t port)
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*************************** ADC Test **************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Note: adc Test include 3 section  **\r\n");
    printf("***************************************************************\r\n");
    printf("=====adc test : Start=====\r\n");

    ret = adc_test_process(port);
    if (ret)
    {
        printf("\r\n=====adc test : FAIL ===\r\n");
        return -1;
    }

    printf("\r\n=====adc test : PASS===\r\n");

    return 0;
}

static void handle_adctest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if(argv != 2)
        return;

    adc_autotest(atoi(argv[1]));
}

static struct cli_command adctest_cmd = {
    .name = "adctest index",
    .help = "adctest index",
    .function = handle_adctest_cmd};

int adc_test(uint32_t port)
{
    aos_cli_register_command(&adctest_cmd);
    return adc_autotest(port);
}
