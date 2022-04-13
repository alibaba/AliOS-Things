/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aos/kernel.h"
#include "aos/init.h"
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "uagent/uagent.h"
#include "aos/kernel.h"
#include "hal2vfs/io_wifi.h"
#include "debug_api.h"

#ifdef AOS_COMP_PWRMGMT
    #include <pwrmgmt_api.h>
#endif

#ifdef AOS_COMP_SPIFFS
    #include "fs/spiffs.h"
#endif

#ifdef AOS_COMP_ROMFS
    #include "fs/romfs.h"
#endif

#ifdef AOS_COMP_LITTLEFS
    #include "fs/littlefs.h"
#endif

#ifdef AOS_COMP_FATFS
    #include "fs/fatfs.h"
#endif

#include "network/network.h"
#ifdef WITH_LWIP_TFTP
    #include "lwip/ip_addr.h"
    #include "lwip/apps/tftp.h"
#endif

#ifdef AOS_UND
    #include "und/und.h"
    #include "debug_api.h"
#endif

#ifdef AOS_COMP_SQLITE
    #include <sqlite.h>
#endif

#ifdef AOS_COMP_DEVICEVFS
#include <drivers/u_ld.h>
#endif

extern int nor_init(void);
extern int  vfs_init(void);
extern int  vfs_device_init(void);
#ifdef AOS_LOOP
    #include "aos/yloop.h"
    extern aos_loop_t aos_loop_init(void);
#endif
extern int32_t kv_init(void);
extern void ota_service_init(void);

#ifdef WITH_SAL
    extern int sal_device_init(void);
#endif

#ifdef AOS_BINS

extern void *kmbins_tbl[];
extern char  app_info_addr;
extern k_mm_head  *g_kmm_head;
struct m_app_info_t *app_info = (struct m_app_info_t *) &app_info_addr;

static void app_pre_init(void)
{
    memcpy((void *)(app_info->data_ram_start), (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);

    memset((void *)(app_info->bss_start), 0, app_info->bss_end - app_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(app_info->heap_start),
                         app_info->heap_end - app_info->heap_start);
}
#endif

#ifdef AOS_COMP_CLI
void cli_service_init(kinit_t *kinit)
{
    if (kinit->cli_enable) {
        aos_cli_init();
        /*kernel basic cmds reg*/
    }
    return;
}
#endif

void aos_show_welcome(void)
{
    puts("             Welcome to AliOS Things           ");
#ifdef CONFIG_AOS_INIT_WELCOME
    puts("       �����������[ �����[     �����[ �������������[ ���������������[     ");
    puts("      �����X�T�T�����[�����U     �����U�����X�T�T�T�����[�����X�T�T�T�T�a     ");
    puts("      ���������������U�����U     �����U�����U   �����U���������������[     ");
    puts("      �����X�T�T�����U�����U     �����U�����U   �����U�^�T�T�T�T�����U     ");
    puts("      �����U  �����U���������������[�����U�^�������������X�a���������������U     ");
    puts("      �^�T�a  �^�T�a�^�T�T�T�T�T�T�a�^�T�a �^�T�T�T�T�T�a �^�T�T�T�T�T�T�a     ");
    puts("�����������������[�����[  �����[�����[�������[   �����[ �������������[ ���������������[");
    puts("�^�T�T�����X�T�T�a�����U  �����U�����U���������[  �����U�����X�T�T�T�T�a �����X�T�T�T�T�a");
    puts("   �����U   ���������������U�����U�����X�����[ �����U�����U  �������[���������������[");
    puts("   �����U   �����X�T�T�����U�����U�����U�^�����[�����U�����U   �����U�^�T�T�T�T�����U");
    puts("   �����U   �����U  �����U�����U�����U �^���������U�^�������������X�a���������������U");
    puts("   �^�T�a   �^�T�a  �^�T�a�^�T�a�^�T�a  �^�T�T�T�a �^�T�T�T�T�T�a �^�T�T�T�T�T�T�a");
#endif
}

int aos_components_init(kinit_t *kinit)
{
#ifdef AOS_COMP_VFS
    vfs_init();
#endif

#ifdef AOS_COMP_UAGENT
    uagent_init();
#endif

#ifdef AOS_COMP_CLI
    cli_service_init(kinit);
#endif

#ifdef AOS_COMP_DEVICEVFS
    u_driver_entry("kernel");
#endif

#ifdef AOS_COMP_SPIFFS
    spiffs_register();
#endif

#ifdef AOS_COMP_FTL
    ftl_init();
#endif

#ifdef AOS_COMP_ROMFS
    romfs_register();
#endif

#ifdef AOS_COMP_LITTLEFS
    littlefs_register();
#endif

#ifdef AOS_COMP_FATFS
    fatfs_register();
#endif

#ifdef AOS_COMP_ULOG
    ulog_init();
#endif

#ifdef AOS_COMP_KV
    kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    vfs_device_init();
    aos_loop_init();
#endif

#ifdef AOS_UOTA
    ota_service_init();
#endif

#ifdef AOS_SENSOR
    sensor_init();
#endif

#ifdef AOS_INPUT
    input_core_init();
#endif

#ifdef AOS_FBDEV
    fb_init();
#endif

#ifdef AOS_GPS
    gps_init();
#endif

    aos_show_welcome();

#ifdef AOS_COMP_PWRMGMT
    pwrmgmt_init();
#endif

#ifdef AOS_COMP_DEBUG
    debug_init();
#endif

#ifdef AOS_UND
    und_update_statis(UND_STATIS_DEV_EXCEPTION_IDX, (int)debug_reboot_reason_get());
#endif

#ifdef AOS_BINS
    app_pre_init();

    if (app_info->app_entry) {
        app_info->app_entry((void *)kmbins_tbl, 0, NULL);
    }
#endif

#ifdef AOS_COMP_SQLITE
    sqlite_init();
#endif

    return 0;
}

