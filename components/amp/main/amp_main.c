/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "amp_defines.h"
#include "amp_config.h"
#include "aos_system.h"
#include "aos_network.h"
#ifdef AMP_KV_ENABLE
#include "aos/kv.h"
#endif
#include "aos_fs.h"
#include "amp_boot_recovery.h"
#include "amp_boot.h"

#ifdef JSE_ADVANCED_ADDON_UI
#include "render_public.h"
#include "../../../../littlevgl/lv_conf.h"
#include "view_model.h"
extern volatile int app_run_flag;

volatile int g_ui_run_flag = 1;
#endif

#define MOD_STR "AMP_MAIN"

extern void jsengine_main(void);

#ifdef AMP_NETWORK_ENABLE
void network_func(void *argv)
{
    aos_network_status_registercb(NULL, NULL);

    while(!aos_get_network_status()) {
        aos_msleep(1000);
    }
    app_management_center_init();
    aos_task_exit(0);

    return;
}
#endif

void jsengine_func(void *argv)
{
    jsengine_main();

    aos_task_exit(0);

    return;
}

int amp_main(void)
{
    int ret = -1;

    /* add memory init */
    amp_memory_init();

    /* printf amp system info */
    aos_printf("=================amp info=================\r\n");
    aos_printf("amp version: amp-v%s-%s\r\n", AMP_VERSION_NUMBER, AMP_GIT_COMMIT);
    aos_printf("amp build time: %s, %s\r\n", __DATE__, __TIME__);
    aos_printf("amp device name: %s\r\n", aos_get_device_name());
    aos_printf("==========================================\r\n");

    /* system init */
    aos_system_init();

    /* file system init */
    aos_fs_init();

    /* ulog module init */
    ulog_init();

    /* set ulog level, make all the level of log is not lower than this value could be logged */
    aos_set_log_level(AOS_LL_ERROR);

    /* amp recovery service init */
    amp_boot_main();

    aos_task_t jsengine_task;

#ifdef AMP_NETWORK_ENABLE
    aos_task_t network_task;
#endif

#ifdef AMP_KV_ENABLE
    ret = kv_init();
    if (ret != 0) {
        amp_warn(MOD_STR, "kv init failed!");
    }
#endif

    /* amp main start */
    ret = aos_task_new_ext(&jsengine_task, "amp_jsengine", jsengine_func, NULL, 1024 * 8, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        amp_debug(MOD_STR, "jsengine task creat failed!");
        return ret;
    }

#ifdef AMP_NETWORK_ENABLE
    /* network start */
    ret = aos_task_new_ext(&network_task, "amp_network", network_func, NULL, 1024 * 4, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        amp_debug(MOD_STR, "network task creat failed!");
        return ret;
    }
#endif

#ifdef JSE_ADVANCED_ADDON_UI
    while(!app_run_flag){
        aos_msleep(50);
    }

    if (g_ui_run_flag) {
        amp_view_model_init();
        render_init();
	}
#endif
    while(1) {
        aos_msleep(5000);
    }

    return 0;
}

