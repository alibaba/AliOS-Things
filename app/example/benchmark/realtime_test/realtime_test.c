/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_realtime_api.h"

int application_start(int argc, char *argv[])
{
    LOG("benchmark started.");
    test_realtime_main(0);
    return 0;
}
