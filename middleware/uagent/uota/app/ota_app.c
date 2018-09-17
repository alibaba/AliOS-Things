/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "ota_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void usage(void)
{
    printf("usage:%s pk dn ds ps \n\n"
           "Make sure network is ready to get IP address.\n"
           "  $1, Product Key.\n"
           "  $2, Device Name.\n"
           "  $3, Device Secret.\n"
           "  $4, Product Secret.\n"
           "\n",
           UOTA_VERSION);
}

int main(int argc, char *argv[])
{
    if (argc <= 3) {
        usage();
        return 1;
    }
    ota_service_manager ctx = { 0 };
    strncpy(ctx.pk, argv[1], sizeof(ctx.pk)-1);
    strncpy(ctx.dn, argv[2], sizeof(ctx.dn)-1);
    strncpy(ctx.ds, argv[3], sizeof(ctx.ds)-1);
    strncpy(ctx.ps, argv[4], sizeof(ctx.ps)-1);
    HAL_SetProductSecret(ctx.ps);
    ctx.trans_protcol = 0;
    ctx.dl_protcol = 3;
    printf("Hello OTA.\n");
    ota_service_init(&ctx);
    while (1) {
        IOT_MQTT_Yield((void *)mqtt_get_instance(), 200);
        sleep(1);
    };
    return 0;
}
