/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_lcd.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "LCD"

#define GPIO_IRQ_RISING_EDGE "rising"
#define GPIO_IRQ_FALLING_EDGE "falling"
#define GPIO_IRQ_BOTH_EDGE "both"

static uint16_t lcd_init_flag = 0;
static JSClassID js_lcd_class_id;

static JSValue native_lcd_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;

    ret = aos_hal_lcd_init();
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_lcd_init fail!");
    }

    return JS_NewInt32(ctx, ret);
}

static JSValue native_lcd_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int32_t ret = -1;

    ret = aos_hal_lcd_uninit();
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_lcd_uninit fail!");
    }

    return JS_NewInt32(ctx, ret);
}


static JSValue native_lcd_show(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret          = -1;
    uint8_t *buf;
    uint32_t buf_len     = 0;
    int32_t x, y, w, h;
    bool rotate;

    if (argc < 6) {
        amp_warn(MOD_STR, "parameter must be x, y, w, h, color");
        goto out;
    }

    JS_ToInt32(ctx, &x, argv[0]);
    JS_ToInt32(ctx, &y, argv[1]);
    JS_ToInt32(ctx, &w, argv[2]);
    JS_ToInt32(ctx, &h, argv[3]);

    buf = JS_GetArrayBuffer(ctx, &buf_len, argv[4]);
    if (!buf) {
        amp_warn(MOD_STR, "parameter buffer is invalid, size: %d", buf_len);
        goto out;
    }

    rotate = JS_ToBool(ctx, argv[5]);

    ret = aos_hal_lcd_show(x, y, w, h, buf, rotate);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_lcd_fill fail!");
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_lcd_fill(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret          = -1;
    int32_t x, y, w, h;
    uint32_t color;

    if (argc < 5) {
        amp_warn(MOD_STR, "parameter must be x, y, w, h, color");
        goto out;
    }

    JS_ToInt32(ctx, &x, argv[0]);
    JS_ToInt32(ctx, &y, argv[1]);
    JS_ToInt32(ctx, &w, argv[2]);
    JS_ToInt32(ctx, &h, argv[3]);
    JS_ToUint32(ctx, &color, argv[4]);

    ret = aos_hal_lcd_fill(x, y, w, h, color);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_lcd_fill fail!");
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_lcd_class = {
    "LCD",
};

static const JSCFunctionListEntry js_lcd_funcs[] = {
    JS_CFUNC_DEF("open", 0, native_lcd_open),
    JS_CFUNC_DEF("show", 0, native_lcd_show),
    JS_CFUNC_DEF("fill", 0, native_lcd_fill),
    JS_CFUNC_DEF("close", 0, native_lcd_close),
};

static int js_lcd_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_lcd_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_lcd_class_id, &js_lcd_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_lcd_funcs,
                               countof(js_lcd_funcs));
    JS_SetClassProto(ctx, js_lcd_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_lcd_funcs,
                                  countof(js_lcd_funcs));
}

JSModuleDef *js_init_module_lcd(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_lcd_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_lcd_funcs, countof(js_lcd_funcs));
    return m;
}

void module_lcd_register(void)
{
    amp_debug(MOD_STR, "module_lcd_register");
    JSContext *ctx = js_get_context();
    aos_printf("module lcd register\n");
    js_init_module_lcd(ctx, "lcd");
}
