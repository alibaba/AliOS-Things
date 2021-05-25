/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/rng.h"
#include "rng_test.h"

#define RNG_TEST_TIMES 1000

random_dev_t rng_dev;

uint32_t random_buf[RNG_TEST_TIMES];

void hal_rng_test(void)
{
    int ret = -1;
    int i = 0;
    int j = 0;
    uint32_t data = 0;
    uint32_t repeat_cnt = 0;
    float repeat_rate = 0;

    printf("*********** rng test start ! ***********\n");

    memset(random_buf, 0, sizeof(random_buf));

    rng_dev.port = PORT_RNG_TEST;

    printf("step1: read random number %d times ! \n", (int)RNG_TEST_TIMES);

    for (i = 0; i < RNG_TEST_TIMES; i++)
    {
        ret = hal_random_num_read(rng_dev, &random_buf[i], sizeof(uint32_t));
        if (ret != 0) {
            printf("hal_random_num_read error !\n");
            return;
        }
    }

    for (i = 0; i < RNG_TEST_TIMES; ++i)
    {
        for (j = i; j < (RNG_TEST_TIMES - i - 1); ++j)
        {
            if (random_buf[j] > random_buf[j + 1]) {
                data = random_buf[j + 1];
                random_buf[j + 1] = random_buf[j];
                random_buf[j] = data;
            }
        }
    }

    for (i = 0; i < RNG_TEST_TIMES - 1; i++)
    {
        if (random_buf[j] == random_buf[j + 1]) {
            repeat_cnt++;
        }


        printf("%x\n", random_buf[i]);
    }

    printf("step2: check the number repetition rate !\n");

    repeat_rate = (float)repeat_cnt / (float)RNG_TEST_TIMES;
    printf("repeat_rate is %f \n", repeat_rate);

    if (repeat_rate > 0.001f) {
        printf("hal_rtc_test failed !\n");
        return;
    } 

    printf("rng test result: succeed !\n");

    printf("*********** rng test end ! ***********\n");
}
