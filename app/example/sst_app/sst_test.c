/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos/aos.h"
#include "sst.h"
#include "sst_test.h"

static int init(void)
{
    uint32_t ret = 0;

    ret = sst_init();
    if (0 != ret) {
        printf("%s %d: error[%x]\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }
    return 0;
}

int application_start(int argc, char *argv[])
{
    int ret = 0;

    LOG("sst test started.");

    ret = init();
    if (ret) {
        LOG("init failed\n");
        return -1;
    }

    ret = sec_sst_basic_test();
    if (ret) {
        LOG("sst basic test failed\n");
        return -1;
    }

    LOG("sst basic test success\n");

    ret = sst_test_performance(1, 1000);
    if (ret) {
        LOG("performance test failed 0x%x\n", ret);
        return -1;
    }

    return 0;
}

