/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "amp_defines.h"
#include "amp_config.h"
#include "amp_system.h"
#include "amp_network.h"
#include "app_ota_service.h"

#define MOD_STR "AMP_MAIN"

extern void jsengine_main(void);

void *network_func(void *argv)
{
    HAL_network_status_registercb(NULL, NULL);

    while(!HAL_Get_network_status()) {
        HAL_SleepMs(1000);
    }
    app_ota_service_init();
    HAL_ThreadDelete(NULL);

    return NULL;
}

void *jsengine_func(void *argv)
{
    jsengine_main();

    HAL_ThreadDelete(NULL);

    return NULL;
}

int amp_main(void)
{   
    char version[AMP_VERSION_LENGTH] = {0};
    amp_version_get(version);
    /* init ulog module */
    HAL_Printf("=================amp info=================\n");
    HAL_Printf("amp version: %s\n", version);
    HAL_Printf("amp build time: %s,%s\n", __DATE__, __TIME__);
    HAL_Printf("==========================================\n");

    /* system init */
    HAL_Sysinit();

    /* init ulog module */
    ulog_init();

    /* set ulog level, make all the level of log is not lower than this value could be logged */
    aos_set_log_level(AOS_LL_DEBUG);
    
    int ret = -1;

    void *jsengine_task;
    int jsengine_stack_used;
    amp_os_thread_param_t jsengine_task_params = {0};
   
#ifdef AMP_NETWORK_ENABLE
    void *network_task;
    int network_stack_used;
    amp_os_thread_param_t network_task_params = {0};
#endif

#ifdef AMP_KV_ENABLE
    ret = HAL_Kv_Init();
    if (ret != 0) {
        amp_warn(MOD_STR, "kv init failed!");
    }
#endif

    jsengine_task_params.name = "amp_jsengine";
    jsengine_task_params.priority = HAL_GetDefaultTaskPriority();
    jsengine_task_params.stack_size = 1024 * 8;

#ifdef AMP_NETWORK_ENABLE
    network_task_params.name = "amp_network";
    network_task_params.priority = HAL_GetDefaultTaskPriority();
    network_task_params.stack_size = 1024 * 2;
#endif

    /* amp main start */
    ret = HAL_ThreadCreate(&jsengine_task, jsengine_func, NULL, &jsengine_task_params, &jsengine_stack_used);
    if (ret != 0) {
        amp_debug(MOD_STR, "jsengine task creat failed!");
        return ret;
    }

#ifdef AMP_NETWORK_ENABLE
    /* network start */
    ret = HAL_ThreadCreate(&network_task, network_func, NULL, &network_task_params, &network_stack_used);
    if (ret != 0) {
        amp_debug(MOD_STR, "network task creat failed!");
        return ret;
    }
#endif

    while(1) {
        HAL_SleepMs(5000);
    }

    return 0;
}

