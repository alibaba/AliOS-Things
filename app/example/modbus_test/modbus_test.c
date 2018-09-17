/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

int application_start(int argc, char *argv[])
{
    LOG("pwr test started.");

    printf("modbus test\n");
    mb_main();

    return 0;
}

