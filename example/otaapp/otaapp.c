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

#ifdef CONFIG_AOS_CLI
#include <aos/cli.h>
#endif

#include "ota_service.h"

static ota_service_t ctx = {0};

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

static void ota_work(void *ctx)
{
#ifdef OTA_WITH_LINKKIT
    IOT_OpenLog("mqtt");
    IOT_SetLogLevel(IOT_LOG_DEBUG);
#endif
    /*Main device*/
    ota_service_init(ctx);
    while (1) {
        IOT_MQTT_Yield(NULL,200);
        aos_msleep(1000);
    }
}

#ifdef CONFIG_AOS_CLI
static void handle_ota_cmd(char *buf, int blen, int argc, char **argv)
{
    if (argc <= 3) {
        usage();
        return;
    }
    memset(&ctx, 0, sizeof(ota_service_t));
    strncpy(ctx.pk, argv[1], sizeof(ctx.pk)-1);
    strncpy(ctx.dn, argv[2], sizeof(ctx.dn)-1);
    strncpy(ctx.ds, argv[3], sizeof(ctx.ds)-1);
    strncpy(ctx.ps, argv[4], sizeof(ctx.ps)-1);
    HAL_SetProductKey(ctx.pk);
    HAL_SetDeviceName(ctx.dn);
    HAL_SetDeviceSecret(ctx.ds);
    HAL_SetProductSecret(ctx.ps);
    printf("Hello OTA.\n");
    aos_task_new("ota_example", ota_work, &ctx, 1024 * 6);
}

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
    uint32_t ota_type;
    uint32_t update_type;
    uint32_t splict_size;
    uint8_t  diff_version;
} ota_reboot_info_t;

static void handle_diff_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[0] : "";
    if (strcmp(rtype, "ota_diff") == 0) {
        if (argc != 3) {
            return;
        }
        uint32_t ota_size = atoi(argv[1]);
        uint32_t splict_size = atoi(argv[2]);
        ota_reboot_info_t ota_info;
        ota_info.ota_len = ota_size;
        ota_info.ota_type = 1;
        ota_info.diff_version = 1;
        ota_info.splict_size = splict_size;
        LOG("%s %d %d %p\n", rtype, ota_size, splict_size,&ota_info);
        //hal_ota_switch_to_new_fw(&ota_info);
    }
}

static struct cli_command diffcmd = {
    .name = "ota_diff",
    .help = "ota diff",
    .function = handle_diff_cmd
};

static struct cli_command otacmd = { .name     = "OTA_APP",
                                     .help     = "OTA_APP pk dn ps",
                                     .function = handle_ota_cmd };

#endif
int application_start(int argc, char *argv[])
{
#ifdef WITH_SAL
    sal_init();
#endif
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    netmgr_start(true);
#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&otacmd);
    aos_cli_register_command(&diffcmd);
#endif
    aos_loop_run();
    return 0;
}
