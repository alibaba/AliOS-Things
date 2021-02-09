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

#define TEST_VOLT_MV 1800

static int32_t adc_test_process()
{
    int32_t ret = 0;
    uint32_t output = 0;
    uint32_t test_sum = 0;
    uint32_t test_avrg = 0;
    uint32_t test_min = 3300;
    uint32_t test_max = 0;

    adc_dev_t adc = {1, 1000, 0x12345678};

    ret = hal_adc_init(&adc);
    if (ret)
    {
        printf("\r\n=====adc test : adc init failed===\r\n");
        return -1;
    }

    for (int32_t i = 0; i < 34; i++)
    {
        hal_adc_value_get(&adc, &output, 1000);
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

    hal_adc_finalize(&adc);

    test_avrg = (test_sum - test_min - test_max) >> 5;

    printf("\r\n=====adc test : the samping volage is:%dmv===\r\n", test_avrg);

    if (((test_avrg - 200) > TEST_VOLT_MV) || ((test_avrg + 200) < TEST_VOLT_MV))
    {
        printf("\r\n=====adc test : the samping volage is out of scope===\r\n");
        printf("\r\n=====adc test : FAIL===\r\n");
        return -1;
    }

    printf("=====adc test : Done=====\r\n");

    return 0;
}

static int adc_autotest()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*************************** ADC Test **************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Note: adc Test pin is 15, 25, 30 in 21*2 male pin header  **\r\n");
    printf("** Note: the testing voltage : 1.6v                          **\r\n");
    printf("** How to test: pls connect the dc power supply to Pin 25    **\r\n");
    printf("***************************************************************\r\n");
    printf("=====adc test : Start=====\r\n");

    ret = adc_test_process();
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
    adc_autotest();
}

static struct cli_command adctest_cmd = {
    .name = "adctest",
    .help = "adctest",
    .function = handle_adctest_cmd};

int adc_test()
{
    aos_cli_register_command(&adctest_cmd);
    return adc_autotest();
}
