/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "LOG"
#define SCRIPT  "JSLOG"
#define LOG_LEVEL_DEBUG   "debug"
#define LOG_LEVEL_INFO    "info"
#define LOG_LEVEL_WARN    "warning"
#define LOG_LEVEL_ERROR   "error"
#define LOG_LEVEL_FATAL   "fatal"
#define LOG_LEVEL_NONE    "none"

static JSClassID js_gpio_class_id;

static int native_get_log_level(const char *pclevel, aos_log_level_t *paos_log_level)
{
    if (NULL == pclevel || NULL == paos_log_level) {
        amp_error(MOD_STR, "invalid input\n");
        return -1;
    }
    if (strncmp(pclevel, LOG_LEVEL_DEBUG, strlen(LOG_LEVEL_DEBUG)) == 0) {
        *paos_log_level = AOS_LL_DEBUG;
    } else if (strncmp(pclevel, LOG_LEVEL_INFO, strlen(LOG_LEVEL_INFO)) == 0) {
        *paos_log_level = AOS_LL_INFO;
    } else if (strncmp(pclevel, LOG_LEVEL_WARN, strlen(LOG_LEVEL_WARN)) == 0) {
        *paos_log_level = AOS_LL_WARN;
    } else if (strncmp(pclevel, LOG_LEVEL_ERROR, strlen(LOG_LEVEL_ERROR)) == 0) {
        *paos_log_level = AOS_LL_ERROR;
    } else if (strncmp(pclevel, LOG_LEVEL_FATAL, strlen(LOG_LEVEL_FATAL)) == 0) {
        *paos_log_level = AOS_LL_FATAL;
    } else if (strncmp(pclevel, LOG_LEVEL_NONE, strlen(LOG_LEVEL_NONE)) == 0) {
        *paos_log_level = AOS_LL_NONE;
    } else {
        amp_error(MOD_STR, "invalid log level %s", pclevel);
        return -1;
    }

    return 0;
}

static JSValue native_set_stdlog_level(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int ret = 0;
    aos_log_level_t  log_level;

    const char *pclevel = JS_ToCString(ctx, argv[0]);
    if (NULL == pclevel) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return JS_NewInt32(ctx, -1);
    }

    ret = native_get_log_level(pclevel, &log_level);
    if (ret) {
        amp_warn(MOD_STR, "invalid loglevel %s\n", pclevel);
        return JS_NewInt32(ctx, -1);
    }

    ret = aos_set_log_level(log_level);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_set_popcloud_log_level(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int ret = 0;
    aos_log_level_t  log_level;

    const char *pclevel = JS_ToCString(ctx, argv[0]);
    if (NULL == pclevel) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return JS_NewInt32(ctx, -1);
    }

    ret = native_get_log_level(pclevel, &log_level);
    if (ret) {
        amp_warn(MOD_STR, "invalid log level %s\n", pclevel);
        return JS_NewInt32(ctx, -1);
    }

    ret = aos_set_popcloud_log_level(log_level);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_set_popfs_log_level(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int ret = 0;
    aos_log_level_t  log_level;

    const char *pclevel = JS_ToCString(ctx, argv[0]);
    if (NULL == pclevel) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return JS_NewInt32(ctx, -1);
    }

    ret = native_get_log_level(pclevel, &log_level);
    if (ret) {
        amp_warn(MOD_STR, "invalid loglevel %s\n", pclevel);
        return JS_NewInt32(ctx, -1);
    }

    ret = aos_set_popfs_log_level(log_level);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_set_log_file_path(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int ret = 0;

    const char *path = JS_ToCString(ctx, argv[0]);
    if (NULL == path) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return JS_NewInt32(ctx, -1);
    }

    ret = ulog_fs_log_file_path(path);
    if (ret) {
        amp_warn(MOD_STR, "fail to set log file path %s\n", path);
        return JS_NewInt32(ctx, -1);
    }
    return JS_NewInt32(ctx, 0);
}

static JSValue native_set_log_file_size(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int ret = -1;
    unsigned int filesize = 0;

    if (!JS_IsNumber(argv[0])) {
        amp_warn(MOD_STR, "parameter must be [number]\n");
        return JS_NewInt32(ctx, -1);
    }

    JS_ToInt32(ctx, &filesize, argv[0]);

    /*TODO : limit the biggist file size*/
    ret = ulog_fs_log_file_size(filesize);
    if (ret) {
        amp_warn(MOD_STR, "fail to set log file size %d\n", filesize);
        return JS_NewInt32(ctx, -1);
    }

    return JS_NewInt32(ctx, 0);
}

static const char *ulog_get_output_format_msg(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str)
            return NULL;
        JS_FreeCString(ctx, str);
    }
    return str;
}

static JSValue native_debug_log_out(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    const char *msg = ulog_get_output_format_msg(ctx, this_val, argc, argv);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return JS_NewInt32(ctx, -1);
    }

    ulog(AOS_LL_DEBUG, SCRIPT, NULL, 0, msg);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_info_log_out(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    const char *msg = ulog_get_output_format_msg(ctx, this_val, argc, argv);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return JS_NewInt32(ctx, -1);
    }

    ulog(AOS_LL_INFO, SCRIPT, NULL, 0, msg);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_warn_log_out(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    const char *msg = ulog_get_output_format_msg(ctx, this_val, argc, argv);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return JS_NewInt32(ctx, -1);
    }

    ulog(AOS_LL_WARN, SCRIPT, NULL, 0, msg);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_error_log_out(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    const char *msg = ulog_get_output_format_msg(ctx, this_val, argc, argv);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return JS_NewInt32(ctx, -1);
    }

    ulog(AOS_LL_ERROR, SCRIPT, NULL, 0, msg);

    return JS_NewInt32(ctx, 0);
}

static JSValue native_fatal_log_out(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    const char *msg = ulog_get_output_format_msg(ctx, this_val, argc, argv);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return JS_NewInt32(ctx, -1);
    }

    ulog(AOS_LL_FATAL, SCRIPT, NULL, 0, msg);

    return JS_NewInt32(ctx, 0);
}

static JSClassDef js_log_class = {
    "ulog",
};

static const JSCFunctionListEntry js_log_funcs[] = {
    JS_CFUNC_DEF("debug", 0, native_debug_log_out ),
    JS_CFUNC_DEF("info", 0, native_info_log_out ),
    JS_CFUNC_DEF("warn", 0, native_warn_log_out ),
    JS_CFUNC_DEF("error", 0, native_error_log_out),
    JS_CFUNC_DEF("fatal", 0, native_fatal_log_out),
    JS_CFUNC_DEF("stdloglevel", 1, native_set_stdlog_level ),
    JS_CFUNC_DEF("cloudloglevel", 1, native_set_popcloud_log_level ),
    JS_CFUNC_DEF("fsloglevel", 1, native_set_popfs_log_level ),
    JS_CFUNC_DEF("setlogfilepath", 1, native_set_log_file_path ),
    JS_CFUNC_DEF("setlogfilesize", 1, native_set_log_file_size ),
};

static int js_log_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_gpio_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_gpio_class_id, &js_log_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_log_funcs,
                               countof(js_log_funcs));
    JS_SetClassProto(ctx, js_gpio_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_log_funcs,
                                  countof(js_log_funcs));
}

JSModuleDef *js_init_module_log(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_log_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_log_funcs, countof(js_log_funcs));
    return m;
}

void module_log_register(void)
{
    amp_debug(MOD_STR, "module_log_register");
    JSContext *ctx = js_get_context();

    js_init_module_log(ctx, "ulog");
}

