/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include <k_api.h>

void mb_main();

int application_start(int argc, char *argv[])
{
    printf("modbus test\n");

    mb_main();

    return 0;
}
