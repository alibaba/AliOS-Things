/*!
 * @file quickjs_addon_common.h
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AMP_QUICKJS_ADDON_COMMON_H_
#define _AMP_QUICKJS_ADDON_COMMON_H_

#include "stdint.h"
#include "quickjs.h"

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

extern JSContext *js_get_context(void);

#ifdef JSE_ADVANCED_ADDON_AIOT_DEVICE
extern JSClassID js_aiot_device_class_id;
#endif

#define QUICKJS_GLOBAL_FUNC(name, func)           \
    do {                                          \
        JSValue global_func;                      \
        global_func = JS_GetGlobalObject(ctx);    \
        JS_SetPropertyStr(ctx, global_func, name, \
            JS_NewCFunction(ctx, func, name, 1)); \
        JS_FreeValue(ctx, global_func); \
    } while (0);

static inline char *QUICKJS_GET_PROPERTY_STR(JSContext *ctx, JSValueConst argv, char *name)
{
    JSValue js_value = JS_GetPropertyStr(ctx, argv, name);
    char *str = NULL;
    if(!JS_IsString(js_value)){
        amp_error("QUCIJS", "request %s is invalid", name);
        JS_FreeValue(ctx, js_value);
        return NULL;
    }
    str =  (char *)JS_ToCString(ctx, js_value);
    JS_FreeValue(ctx, js_value);
    return str;
}

static inline int QUICKJS_GET_PROPERTY_INT32(JSContext *ctx, JSValueConst argv, char *name)
{
    JSValue js_value = JS_GetPropertyStr(ctx, argv, name);
    int32_t val = 0;
    if(!JS_IsNumber(js_value)){
        amp_error("QUCIJS", "request %s is invalid", name);
        JS_FreeValue(ctx, js_value);
        return 0;
    }
    JS_ToInt32(ctx, &val, js_value);
    JS_FreeValue(ctx, js_value);
    return val;
}

static inline bool QUICKJS_GET_PROPERTY_BOOL(JSContext *ctx, JSValueConst argv, char *name)
{
    JSValue js_value = JS_GetPropertyStr(ctx, argv, name);
    bool val = false;
    if(!JS_IsBool(js_value)){
        amp_error("QUCIJS", "request %s is invalid", name);
        JS_FreeValue(ctx, js_value);
        return 0;
    }
    val = JS_ToBool(ctx, js_value);
    JS_FreeValue(ctx, js_value);
    return val;
}

#endif