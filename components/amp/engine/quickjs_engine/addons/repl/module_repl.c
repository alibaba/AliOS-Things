/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0
*/

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "repl.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "MODULE_REPL"

static JSClassID js_repl_class_id;

extern JSContext *js_get_context(void);

static uint16_t repl_init_flag = 0;
static uint16_t repl_js_cb_flag = 0;
static JSValue repl_js_cb_ref = JS_UNDEFINED;
static aos_sem_t g_repl_close_sem = NULL;

static JSValue native_repl_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    aos_repl_init(NULL);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_repl_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = 0;
    repl_init_flag = 0;

    aos_sem_wait(&g_repl_close_sem, AOS_WAIT_FOREVER);

    ret = aos_repl_close();

    return JS_NewInt32(ctx, ret);
}

static JSValue native_repl_puts(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    int i   = 0;
    char *msg = NULL;
    size_t msg_len = 0;
    JSValue val;
    const char *buf;

    if(argc < 1)
    {
        amp_warn(MOD_STR, "parameter must be array");
        goto out;
    }

    if(JS_IsString(argv[0])) {
        buf = JS_ToCString(ctx, argv[0]);
        msg_len = strlen(buf);
        i = 1;
    } else {
        buf = JS_GetArrayBuffer(ctx, &msg_len, argv[0]);
        if(!buf) {
            amp_warn(MOD_STR, "parameter buffer is invalid, size: %d", msg_len);
            goto out;
        }
    }

    msg = (char *)aos_malloc(msg_len + 1);
    if (!msg) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    memcpy(msg, buf, msg_len);

    msg[msg_len] = 0;

    ret  = aos_repl_write(msg);
    if (-1 == ret) {
        amp_error(MOD_STR, "native_repl_write fail!");
    }
    aos_free(msg);
out:
    if(i == 1) {
        JS_FreeCString(ctx, buf);
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_repl_read(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    uint32_t max_len = 16;
    uint32_t recvsize = 0;

    uint64_t pos, len;
    size_t size;
    ssize_t ret;
    uint8_t *buf;

    if (JS_ToIndex(ctx, &pos, argv[1]))
        return JS_EXCEPTION;
    if (JS_ToIndex(ctx, &len, argv[2]))
        return JS_EXCEPTION;

    buf = JS_GetArrayBuffer(ctx, &size, argv[0]);
    if (!buf)
        return JS_EXCEPTION;
    if ((uint32_t)pos + (uint32_t)len > size)
        return JS_ThrowRangeError(ctx, "read/write array buffer overflow");

    ret = aos_repl_read(buf + (uint32_t)pos, (uint32_t)len, &recvsize);

    return JS_NewInt32(ctx, ret);
}

static void repl_read_notify(void *arg)
{
    JSContext *ctx = js_get_context();
    if (repl_init_flag) {
        JSValue val = JS_Call(ctx, repl_js_cb_ref, JS_UNDEFINED, 0, NULL);
        JS_FreeValue(ctx, val);
    }
}

static void repl_task_entry()
{
    aos_printf("repl task begin\n");
    while (repl_init_flag) {
        if(repl_js_cb_flag == 1) {
            amp_task_schedule_call(repl_read_notify, NULL);
        }
        aos_msleep(100);
    }
    aos_printf("repl task exited\n");
    aos_sem_signal(&g_repl_close_sem);
    aos_task_exit(0);
}

void repl_read_task_start(void)
{
    aos_task_t repl_task;

    if (!repl_init_flag) {
        repl_init_flag = 1;
        aos_task_new_ext(&repl_task, "amp repl task", repl_task_entry, NULL, 1024 * 4, AOS_DEFAULT_APP_PRI);
    }
}

static JSValue native_repl_setReadHandler(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    JSValue read_cb = argv[0];
    if (!JS_IsFunction(ctx, read_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    repl_js_cb_ref = JS_DupValue(ctx, read_cb);
    repl_js_cb_flag = 1;
    return JS_NewInt32(ctx, 0);
}

static void module_repl_source_clean(void)
{
    JSContext *ctx = js_get_context();
    if (repl_js_cb_flag) {
        JS_FreeValue(ctx, repl_js_cb_ref);
    }
}

static JSClassDef js_repl_class = {
    "REPL",
};

static const JSCFunctionListEntry js_repl_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_repl_open),
    JS_CFUNC_DEF("read", 0, native_repl_read ),
    JS_CFUNC_DEF("puts", 1, native_repl_puts ),
    JS_CFUNC_DEF("exit", 0, native_repl_close ),
    JS_CFUNC_DEF("setReadHandler", 1, native_repl_setReadHandler )
};

static int js_repl_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_repl_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_repl_class_id, &js_repl_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_repl_funcs,
                               countof(js_repl_funcs));
    JS_SetClassProto(ctx, js_repl_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_repl_funcs,
                                  countof(js_repl_funcs));
}

JSModuleDef *js_init_module_repl(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_repl_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_repl_funcs, countof(js_repl_funcs));
    return m;
}

void module_repl_register(void)
{
    amp_debug(MOD_STR, "module_repl_register\n");
    JSContext *ctx = js_get_context();

    if (!g_repl_close_sem) {
        if (aos_sem_new(&g_repl_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create repl sem fail");
            return;
        }
    }

    amp_module_free_register(module_repl_source_clean);

    js_init_module_repl(ctx, "REPL");
}
