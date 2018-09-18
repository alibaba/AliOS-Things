/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "ali_crypto_test.h"

static void crypto_test(void *arg)
{
    ali_crypto_test_entry();
}

int application_start(int argc, char *argv[])
{
    LOG("application started.");
    //ali_crypto_test_entry();
    // if alicrypto test fails on some platform,
    // please try to use the following code to increase stack
    aos_task_new("crypto_test", crypto_test, NULL, 4096);

    return 0;
}
