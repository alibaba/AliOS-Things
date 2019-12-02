/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <ulog/ulog.h>

#include "aos/kernel.h"
#include <k_api.h>

#define MODBUSM_APP "modbus_app"

void mb_main();

int application_start(int argc, char *argv[])
{
    LOGI(MODBUSM_APP, "modbus test");
    aos_set_log_level(AOS_LL_DEBUG);

    mb_main();

    return 0;
}
