/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/dac.h"
#include "dac_test.h"

dac_dev_t dac_dev;

void hal_dac_test(void)
{
    int ret = -1;

    printf("*********** dac test start ! ***********\n");

    dac_dev.port = PORT_DAC_TEST;
    ret = hal_dac_init(&dac_dev);
    if (ret != 0) {
        printf("hal_dac_init error !\n");
        return;
    }

    ret = hal_dac_finalize(&dac_dev);
    if (ret != 0) {
        printf("hal_dac_finalize error !\n");
        return;
    }

    printf("dac test result: succeed !\n");

    printf("*********** dac test end ! ***********\n");
}
