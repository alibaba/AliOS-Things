/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_common.h"
#include "be_jse_export.h"
#include "be_log.h"

#include "be_port_osal.h"
#include "cli_ext.h"
#include "hal/system.h"

#define DEVICE_NAME "deviceName"
#define DEVICE_NAME_LEN 12
#define MODULE_TAG ("cli_sub_call")

static bool stop_flag = false;
extern void tiny_engine_load_addon();

extern const char *load_js_module(const char *moduleName);

void sub_call_restart(void *arg)
{
    char *data = NULL;

    data = (char *)load_js_module((char *)arg);
    if (data) {
        be_debug(MODULE_TAG,
                 "sub_call_restart Running JSEngine InitApplication...");
        bone_engine_exit();
        bone_engine_init();
        bone_engine_load_addon();
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
        be_debug(MODULE_TAG,
                 "sub_call_start Running JSEngine Init Application...");
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
    hal_system_kv_set(DEVICE_NAME, arg, DEVICE_NAME_LEN, 1);
}
