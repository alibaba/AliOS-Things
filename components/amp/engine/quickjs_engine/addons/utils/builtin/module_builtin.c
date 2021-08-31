/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_platform.h"
#include "aos/kernel.h"
#include "aos/vfs.h"
#include "aos_fs.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "aos_fs.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "BUILTIN"
#define GLOBAL_APPCONFIG_STRING_1 "Object.defineProperty(this, 'appConfig', { value: "
#define GLOBAL_APPCONFIG_STRING_2 ", writable: true, configurable: true, enumerable: true});"

/* export appConfig */
static void native_add_global_appconfig(JSContext *ctx)
{
    char json_dir[32] = {0};
    char *json = NULL;
    void *json_data = NULL;
    uint32_t len = 0;
    uint32_t global_config_len = 0;
    int  json_fd = -1;

    snprintf(json_dir, 32, AMP_APP_MAIN_JSON);

    if ((json_fd = aos_open(json_dir, O_RDONLY)) < 0) {
        amp_error(MOD_STR, "open:%s fail", json_dir);
        return;
    }

    len = aos_lseek(json_fd, 0, SEEK_END);

    global_config_len = len + strlen(GLOBAL_APPCONFIG_STRING_1) + strlen(GLOBAL_APPCONFIG_STRING_2) + 1;

    if ((json_data = aos_calloc(1, global_config_len)) == NULL)
    {
        aos_close(json_fd);
        amp_error(MOD_STR, "memory overflow");
        return;
    }

    aos_lseek(json_fd, 0, SEEK_SET);
    strncpy(json_data, GLOBAL_APPCONFIG_STRING_1, global_config_len);
    aos_read(json_fd, json_data + strlen(GLOBAL_APPCONFIG_STRING_1), len);
    strcpy(json_data + strlen(GLOBAL_APPCONFIG_STRING_1) + len, GLOBAL_APPCONFIG_STRING_2);

    JSValue val = JS_Eval(ctx, json_data, strlen(json_data), "appconfig", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(val)) {
        aos_printf("global.createInstance eval jsbundle exec error\n");
        qjs_std_dump_error(ctx);
    }
    JS_FreeValue(ctx, val);

    amp_debug(MOD_STR, "export appConfig");

    aos_close(json_fd);
    aos_free(json_data);
}

void module_builtin_register(void)
{
    amp_debug(MOD_STR, "module_builtin_register");
    JSContext *ctx = js_get_context();

     /* export appConfig */
    native_add_global_appconfig(ctx);
}
