/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

extern void test_certificate(void);

int application_start(int argc, char *argv[])
{
    LOG("kernel test started.");

    test_certificate();

    return 0;
}

