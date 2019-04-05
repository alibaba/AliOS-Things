/**
 * File : aos.c
 */
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#define AOS_START_STACK 2048

// #define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

ktask_t *g_aos_init;

extern int application_start(int argc, char **argv);

extern void hw_start_hal(void);

void hal_init()
{
    board_init();
}

extern void board_init_later() __attribute__((weak));
static void sys_init(void)
{
    LOG("sys_init.");
    rda_soc_init();

#ifdef BOOTLOADER
    main();
#else
    hal_init();
    rda5981_init_sys_data();
    hw_start_hal();
    vfs_init();
    vfs_device_init();
#ifdef AOS_COMP_CLI
    aos_cli_init();
#endif

#ifdef AOS_COMP_ULOG
    ulog_init("A");
#endif
    aos_kv_init();
    aos_loop_init();
    dumpsys_cli_init();
#ifdef AOS_FOTA
    ota_service_init();
#endif

#ifndef DELETE_HFILOP_CODE
    board_init_later();
#endif

    application_start(0, NULL);
#endif
}


void sys_start(void)
{
    aos_init();
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK*4, (task_entry_t)sys_init, 1);
    aos_start();
}

void entry_main(void)
{
    sys_start();
}


