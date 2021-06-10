/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define LOG_NDEBUG 0
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_rtc.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "RTC"

#define RTC_YEAR "year"
#define RTC_MONTH "month"
#define RTC_DAY "day"
#define RTC_HOUR "hour"
#define RTC_MINUTE "minute"
#define RTC_SECOND "second"

#define RTC_TIME_FORMAT                                                 \
    "{\"year\":\"%d\",\"month\":\"%d\",\"day\":\"%d\",\"hour\":\"%d\"," \
    "\"minute\":\"%d\",\"second\":\"%d\"}"

static rtc_dev_t rtc_dev;
static JSClassID js_rtc_class_id;

static JSValue native_rtc_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = aos_hal_rtc_init(&rtc_dev);
    amp_debug(MOD_STR, "port: %d, format: %d\n", rtc_dev.port, rtc_dev.config.format);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_rtc_init fail!\n");
    }

    return JS_NewInt32(ctx, ret);
}

static JSValue native_rtc_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = aos_hal_rtc_finalize(&rtc_dev);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_rtc_finalize fail!\n");
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_rtc_get_time(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    rtc_time_t rtcTime;

    ret = aos_hal_rtc_get_time(&rtc_dev, &rtcTime);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_rtc_get_time fail!\n");
         JS_SetContextOpaque(ctx, NULL);
    }
    else
    {
        JSValue t = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, t, "year", JS_NewInt32(ctx, ((uint32_t)rtcTime.year)));
        JS_SetPropertyStr(ctx, t, "month", JS_NewInt32(ctx, rtcTime.month));
        JS_SetPropertyStr(ctx, t, "day", JS_NewInt32(ctx, rtcTime.date));
        JS_SetPropertyStr(ctx, t, "hour", JS_NewInt32(ctx, rtcTime.hr));
        JS_SetPropertyStr(ctx, t, "minute", JS_NewInt32(ctx, rtcTime.min));
        JS_SetPropertyStr(ctx, t, "second", JS_NewInt32(ctx, rtcTime.sec));

        return t;
    }

    return JS_NewInt32(ctx, 0);
}

static JSValue native_rtc_set_time(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    rtc_time_t *setTime;
    int data;
    JSValue val;

    if(argc < 1)
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }
    setTime = (rtc_time_t *)aos_malloc(sizeof(rtc_time_t));
    if (!setTime) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    ret = aos_hal_rtc_get_time(&rtc_dev, setTime);
    if (ret < 0) {
        amp_error(MOD_STR, "aos_hal_rtc_get_time fail!\n");
        goto out;
    }

    /* 获取年份参数*/
    val = JS_GetPropertyStr(ctx, argv[0], RTC_YEAR);
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", RTC_YEAR);
        goto out;
    }
    ret = JS_ToInt32(ctx, &data, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", RTC_YEAR);
        goto out;
    }

    setTime->year = data;

    JS_FreeValue(ctx, val);

    /* 获取月份参数*/
    val = JS_GetPropertyStr(ctx, argv[0], RTC_MONTH);
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", RTC_MONTH);
        goto out;
    }

    ret = JS_ToInt32(ctx, &data, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", RTC_MONTH);
        goto out;
    }

    setTime->month = data;

    JS_FreeValue(ctx, val);

    /* 获取日期参数*/
    val = JS_GetPropertyStr(ctx, argv[0], RTC_DAY);
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", RTC_DAY);
        goto out;
    }

    ret = JS_ToInt32(ctx, &data, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", RTC_DAY);
        goto out;
    }

    setTime->date = data;

    JS_FreeValue(ctx, val);

    /* 获取小时参数*/
    val = JS_GetPropertyStr(ctx, argv[0], RTC_HOUR);
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", RTC_HOUR);
        goto out;
    }

    ret = JS_ToInt32(ctx, &data, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", RTC_HOUR);
        goto out;
    }

    setTime->hr = data;

    JS_FreeValue(ctx, val);

    /* 获取分钟参数*/
    val = JS_GetPropertyStr(ctx, argv[0], RTC_MINUTE);
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", RTC_MINUTE);
        goto out;
    }

    ret = JS_ToInt32(ctx, &data, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", RTC_MINUTE);
        goto out;
    }

    setTime->min = data;
    JS_FreeValue(ctx, val);

    /* 获取秒参数*/
    val = JS_GetPropertyStr(ctx, argv[0], RTC_SECOND);
    if (JS_IsException(val) || JS_IsUndefined(val))
    {
        amp_warn(MOD_STR, "parameter %s failed!\n", RTC_SECOND);
        goto out;
    }

    ret = JS_ToInt32(ctx, &data, val);
    if(ret != 0)
    {
        amp_warn(MOD_STR, "parameter %s to int32 failed!\n", RTC_SECOND);
        goto out;
    }

    setTime->sec = data;
    JS_FreeValue(ctx, val);

    amp_info(MOD_STR,
        "year: %d, month: %d, day: %d, hour: %d, minute: %d, second: %d\n",
        setTime->year, setTime->month, setTime->date, setTime->hr, setTime->min,
        setTime->sec);

    ret = aos_hal_rtc_set_time(&rtc_dev, setTime);
out:
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_rtc_class = {
    "RTC",
};

static const JSCFunctionListEntry js_rtc_funcs[] = {
    JS_CFUNC_DEF("open", 0, native_rtc_open ),
    JS_CFUNC_DEF("close", 0, native_rtc_close ),
    JS_CFUNC_DEF("getTime", 0, native_rtc_get_time ),
    JS_CFUNC_DEF("setTime", 1, native_rtc_set_time),
};

static int js_rtc_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_rtc_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_rtc_class_id, &js_rtc_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_rtc_funcs,
                               countof(js_rtc_funcs));
    JS_SetClassProto(ctx, js_rtc_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_rtc_funcs,
                                  countof(js_rtc_funcs));
}

JSModuleDef *js_init_module_rtc(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_rtc_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_rtc_funcs, countof(js_rtc_funcs));
    return m;
}


void module_rtc_register(void)
{
    amp_debug(MOD_STR, "module_rtc_register");
    JSContext *ctx = js_get_context();
    aos_printf("module rtc register\n");
    js_init_module_rtc(ctx, "RTC");
}
