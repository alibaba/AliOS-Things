/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jse_port.h"
#include "be_common.h"
#include "be_jse_export.h"

#include "cli_ext.h"

#define DEVICE_NAME "deviceName"
#define DEVICE_NAME_LEN 12
#define MODULE_TAG ("cli_sub_call")

static bool stop_flag = false;
extern void tiny_engine_load_addon();

extern const char *load_js_module(const char *moduleName);

void sub_call_restart(void *arg)
{
    char *data = NULL;

    jsengine_exit();
    jsengine_init();
    jsengine_eval_file((char *)arg);
    stop_flag = false;
}

void sub_call_start(void *arg)
{
    char *data;

    if (true == stop_flag) {
        sub_call_restart(arg);
    } else {
        jsengine_eval_file((char *)arg);
    }
}

void sub_call_stop(void *arg)
{
    stop_flag = true;
    jsengine_exit();
}

void sub_call_rename(void *arg)
{
    jse_system_kv_set(DEVICE_NAME, arg, DEVICE_NAME_LEN, 1);
}
