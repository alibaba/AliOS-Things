/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_pwm.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "PWM"

static JSClassID js_pwm_class_id;

typedef struct sim_info {
    int freq;
    int duty;
} pwm_module_t;

static JSValue native_pwm_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    item_handle_t pwm_handle;
    pwm_handle.handle     = NULL;
    pwm_dev_t *pwm_device = NULL;
    const char *id;

    if((argc < 1) || (!JS_IsString(argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    id = JS_ToCString(ctx, argv[0]);

    printf("open %s\n\r", id);
    ret = board_attach_item(MODULE_PWM, id, &pwm_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item %d fail!\n", ret);
        goto out;
    }

    amp_debug(MOD_STR, "pwm handle:%u\n", pwm_handle.handle);
    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    amp_debug(MOD_STR, "%s:%d:%d:%f\n", id, pwm_device->port, pwm_device->config.freq,
          pwm_device->config.duty_cycle);

    ret = aos_hal_pwm_init(pwm_device);

out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }

    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_PWM, &pwm_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_pwm_class_id);
        JS_SetOpaque(obj, (void *)pwm_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, 0);
}

static JSValue native_pwm_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    pwm_handle.handle = JS_GetOpaque2(ctx, this_val, js_pwm_class_id);
    if (!pwm_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_pwm_stop(pwm_device);
    ret |= aos_hal_pwm_finalize(pwm_device);
    ret = board_disattach_item(MODULE_PWM, &pwm_handle);
    printf("pwm close %d\n\r", ret);
    amp_debug(MOD_STR, "aos_hal_pwm_finalize ret: %d\n", ret);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_pwm_getConfig(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    item_handle_t pwm_handle;
    pwm_module_t pwm_config;
    pwm_dev_t *pwm_device = NULL;

    pwm_handle.handle = JS_GetOpaque2(ctx, this_val, js_pwm_class_id);
    if (!pwm_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    pwm_config.duty = (int)(pwm_device->config.duty_cycle * 100);
    pwm_config.freq = (int)(pwm_device->config.freq);

    JSValue t = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, t, "freq", JS_NewInt32(ctx, pwm_config.freq));
    JS_SetPropertyStr(ctx, t, "duty", JS_NewInt32(ctx, pwm_config.duty));

    return t;

out:
    return JS_NewInt32(ctx, 0);
}

static JSValue native_pwm_setConfig(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    int32_t freq;
    int duty    = 0;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;
    JSValue val;

    if(argc < 1)
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    pwm_handle.handle = JS_GetOpaque2(ctx, this_val, js_pwm_class_id);
    if (!pwm_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    /* 获取频率参数*/
    val = JS_GetPropertyStr(ctx, argv[0], "freq");
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", "freq");
        goto out;
    }
    ret = JS_ToInt32(ctx, &freq, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", "freq");
        goto out;
    }

    /* 获取占空比参数*/
    val = JS_GetPropertyStr(ctx, argv[0], "duty");
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", "duty");
        goto out;
    }
    ret = JS_ToInt32(ctx, &duty, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", "duty");
        goto out;
    }

    pwm_device->config.duty_cycle = (float)duty / 100.0;
    pwm_device->config.freq = freq;

    ret = aos_hal_pwm_stop(pwm_device);
    if (ret != 0) {
        amp_warn(MOD_STR, "amp hal pwm stop failed\n");
        goto out;
    }

    pwm_config_t para;
    para.duty_cycle = pwm_device->config.duty_cycle;
    para.freq = pwm_device->config.freq;

    ret = aos_hal_pwm_para_chg(pwm_device, para);
    if (ret != 0) {
        amp_warn(MOD_STR, "amp hal pwm init failed\n");
        goto out;
    }
    ret = aos_hal_pwm_start(pwm_device);

out:
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_pwm_class = {
    "PWM",
};

static const JSCFunctionListEntry js_pwm_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_pwm_open),
    JS_CFUNC_DEF("close", 0, native_pwm_close),
    JS_CFUNC_DEF("getConfig", 0, native_pwm_getConfig),
    JS_CFUNC_DEF("setConfig", 1, native_pwm_setConfig),
};

static int js_pwm_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_pwm_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_pwm_class_id, &js_pwm_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_pwm_funcs,
                               countof(js_pwm_funcs));
    JS_SetClassProto(ctx, js_pwm_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_pwm_funcs,
                                  countof(js_pwm_funcs));
}

JSModuleDef *js_init_module_pwm(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_pwm_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_pwm_funcs, countof(js_pwm_funcs));
    return m;
}

void module_pwm_register(void)
{
    amp_debug(MOD_STR, "module_pwm_register");
    JSContext *ctx = js_get_context();
    aos_printf("module pwm register\n");
    js_init_module_pwm(ctx, "PWM");
}
