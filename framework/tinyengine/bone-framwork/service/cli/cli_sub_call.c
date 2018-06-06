/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <be_osal.h>
#include "utils/be_common.h"
#include "be_jse_api.h"

#define DEVICE_NAME "deviceName"
#define DEVICE_NAME_LEN 12
#define MODULE_TAG ("cli_sub_call")


static bool stop_flag = false;


extern void tiny_engine_load_addon(bool reload);
extern const char *load_js_module(const char *moduleName); 


void sub_call_restart(void *arg)
{
    char *data = NULL;
    
    data = (char *)load_js_module((char *)arg);
    if (data) {
        be_debug(MODULE_TAG,"sub_call_restart Running TinyEngine Init Application...");
        bone_engine_exit();
        bone_engine_init();
        tiny_engine_load_addon(true);
        bone_engine_start(data);
        stop_flag = false;
        free(data);
    }
}

void sub_call_start(void *arg)
{
    char *data;

    if (true == stop_flag) {
        sub_call_restart(arg);
    } else {
        data = (char *)load_js_module((char *)arg);
        be_debug(MODULE_TAG,"sub_call_start Running TinyEngine Init Application...");
        if (data) {
            bone_engine_start(data);
            free(data);
        }
    }
}

void sub_call_stop(void *arg)
{
    stop_flag = true;
    bone_engine_exit();
}


void sub_call_rename(void *arg)
{
    be_osal_kv_set(DEVICE_NAME, arg, DEVICE_NAME_LEN, 1);
}

void sub_call_wifi(void *arg)
{
    be_osal_wifi_config_t *config = (be_osal_wifi_config_t *)arg;
    be_osal_wifi_suspend();
    be_osal_wifi_connect(config);
    be_osal_wifi_save_config(config);
    free(config);
}