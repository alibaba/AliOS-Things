/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "aos/log.h"
#include "aos/yloop.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/cli.h>
#include <hal/ota.h>

#include "ota_service.h"

#define PRODUCT_KEY "a16UKrlKekO"
#define DEVICE_NAME "gateway_test01"
#define DEVICE_SECRET "AT2XFOPOIbJaKfXsKeaEhabJ8TLhMQYp"

static void usage(void)
{
    printf("usage:%s [OPTIONS] \n\n"
           "  $1, Product Key.\n"
           "  $2, Device Name.\n"
           "  $3, Device Secret.\n"
           "  $4, Transport Protcol:default:0--> MQTT 1-->COAP 2-->HTTP.\n"
           "  $5, Download Protcol:default:3--> HTTPS 1-->COAP 2-->HTTP.\n"
           "\n",
           UOTA_VERSION);
}

static void ota_work(void *ctx)
{
    ota_service_init(ctx);
}

static void handle_ota_cmd(char *buf, int blen, int argc, char **argv)
{
    if (argc <= 4) {
        usage();
        return;
    }
    ota_service_manager ctx = { 0 };
    strcpy(ctx.pk, argv[1]);
    strcpy(ctx.dn, argv[2]);
    strcpy(ctx.ds, argv[3]);
    if (argc > 4)
        ctx.trans_protcol = atoi(argv[4]);
    else
        ctx.trans_protcol = 0;
    if (argc > 5)
        ctx.dl_protcol = atoi(argv[5]);
    else
        ctx.dl_protcol = 3;

    printf("Hello OTA.\n");
    aos_task_new("ota_example", ota_work, &ctx, 1024 * 6);
}

static struct cli_command otacmd = { .name     = "OTA_APP",
                                     .help     = "OTA_APP pk dn ps tr dl",
                                     .function = handle_ota_cmd };

int application_start(int argc, char *argv[])
{
    // HAL_SetProductKey(PRODUCT_KEY);
    // HAL_SetDeviceName(DEVICE_NAME);
    // HAL_SetDeviceSecret(DEVICE_SECRET);
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    netmgr_start(true);
    aos_cli_register_command(&otacmd);
    aos_loop_run();
    return 0;
}
