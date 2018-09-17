/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#include "soc_init.h"

int application_start(int argc, char *argv[])
{
    printf("application_start\n");

    sensor_display_init();

    return 0;
}
