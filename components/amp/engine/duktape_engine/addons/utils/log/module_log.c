/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "be_inl.h"

#define MOD_STR "LOG"
#define SCRIPT  "JSLOG"
#define LOG_LEVEL_DEBUG   "debug"
#define LOG_LEVEL_INFO    "info"
#define LOG_LEVEL_WARN    "warn"
#define LOG_LEVEL_ERROR   "error"
#define LOG_LEVEL_FATAL   "fatal"
#define LOG_LEVEL_NONE    "none"

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

static duk_ret_t native_set_stdlog_level(duk_context *ctx)
{
    int ret = 0;
    aos_log_level_t  log_level;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    const char *pclevel = duk_get_string(ctx, 0);
    if (NULL == pclevel) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    ret = native_get_log_level(pclevel, &log_level);
    if (ret) {
        amp_warn(MOD_STR, "invalid loglevel %s\n", pclevel);
        return -1;
    }

    ret = aos_set_log_level(log_level);

    return ret;
}

static duk_ret_t native_set_popcloud_log_level(duk_context *ctx)
{
    int ret = 0;
    aos_log_level_t  log_level;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    const char *pclevel = duk_get_string(ctx, 0);
    if (NULL == pclevel) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    ret = native_get_log_level(pclevel, &log_level);
    if (ret) {
        amp_warn(MOD_STR, "invalid log level %s\n", pclevel);
        return -1;
    }

    ret = aos_set_popcloud_log_level(log_level);

    return ret;
}

static duk_ret_t native_set_popfs_log_level(duk_context *ctx)
{
    int ret = 0;
    aos_log_level_t  log_level;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    const char *pclevel = duk_get_string(ctx, 0);
    if (NULL == pclevel) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    ret = native_get_log_level(pclevel, &log_level);
    if (ret) {
        amp_warn(MOD_STR, "invalid loglevel %s\n", pclevel);
        return -1;
    }

    ret = aos_set_popfs_log_level(log_level);

    return ret;
}

static duk_ret_t native_set_log_file_path(duk_context *ctx)
{
    int ret = 0;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }
    const char *path = duk_get_string(ctx, 0);
    if (NULL == path) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return -1;
    }

    ret = ulog_fs_log_file_path(path);
    if (ret) {
        amp_warn(MOD_STR, "fail to set log file path %s\n", path);
        return -1;
    }
    return 0;
}

static duk_ret_t native_set_log_file_size(duk_context *ctx)
{
    int ret = -1;
    unsigned int filesize = 0;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be [number]\n");
        return -1;
    }

    filesize = duk_get_number(ctx, 0);

    /*TODO : limit the biggist file size*/
    ret = ulog_fs_log_file_size(filesize);
    if (ret) {
        amp_warn(MOD_STR, "fail to set log file size %d\n", filesize);
        return -1;
    }

    return 0;
}

static const char *ulog_get_output_format_msg(duk_context *ctx)
{
    duk_idx_t n = duk_get_top(ctx);
    duk_idx_t i;

    duk_get_global_string(ctx, "ulog");
    duk_get_prop_string(ctx, -1, "format");

    for (i = 0; i < n; i++) {
        if (duk_check_type_mask(ctx, i, DUK_TYPE_MASK_OBJECT)) {
            /* Slow path formatting. */
            duk_dup(ctx, -1); /* console.format */
            duk_dup(ctx, i);
            duk_call(ctx, 1);
            duk_replace(ctx, i); /* arg[i] = console.format(arg[i]); */
        }
    }

    duk_pop_2(ctx);

    duk_push_string(ctx, "");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);

    return duk_safe_to_string(ctx, -1);
}

static duk_ret_t native_debug_log_out(duk_context *ctx)
{
    const char *msg = ulog_get_output_format_msg(ctx);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return -1;
    }

    ulog(AOS_LL_DEBUG, SCRIPT, NULL, 0, msg);

    return 0;
}

static duk_ret_t native_info_log_out(duk_context *ctx)
{
    const char *msg = ulog_get_output_format_msg(ctx);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return -1;
    }

    ulog(AOS_LL_INFO, SCRIPT, NULL, 0, msg);

    return 0;
}

static duk_ret_t native_warn_log_out(duk_context *ctx)
{
    const char *msg = ulog_get_output_format_msg(ctx);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return -1;
    }

    ulog(AOS_LL_WARN, SCRIPT, NULL, 0, msg);

    return 0;
}

static duk_ret_t native_error_log_out(duk_context *ctx)
{
    const char *msg = ulog_get_output_format_msg(ctx);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return -1;
    }

    ulog(AOS_LL_ERROR, SCRIPT, NULL, 0, msg);

    return 0;
}

static duk_ret_t native_fatal_log_out(duk_context *ctx)
{
    const char *msg = ulog_get_output_format_msg(ctx);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return -1;
    }

    ulog(AOS_LL_FATAL, SCRIPT, NULL, 0, msg);

    return 0;
}

void module_log_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    /*copy from console.log don't why*/
    duk_eval_string(ctx,
                    "(function (E) {"
                    "return function format(v){"
                    "try{"
                    "return E('jx',v);"
                    "}catch(e){"
                    "return String(v);" /* String() allows symbols, ToString() internal algorithm doesn't. */
                    "}"
                    "};"
                    "})(Duktape.enc)");
    duk_put_prop_string(ctx, -2, "format");

    AMP_ADD_FUNCTION("debug",    native_debug_log_out, DUK_VARARGS);
    AMP_ADD_FUNCTION("info",     native_info_log_out, DUK_VARARGS);
    AMP_ADD_FUNCTION("warn",    native_warn_log_out, DUK_VARARGS);
    AMP_ADD_FUNCTION("error",     native_error_log_out, DUK_VARARGS);
    AMP_ADD_FUNCTION("fatal",     native_fatal_log_out, DUK_VARARGS);
    AMP_ADD_FUNCTION("stdloglevel", native_set_stdlog_level, 1);
    AMP_ADD_FUNCTION("cloudloglevel", native_set_popcloud_log_level, 1);
    AMP_ADD_FUNCTION("fsloglevel", native_set_popfs_log_level, 1);
    AMP_ADD_FUNCTION("setlogfilepath",      native_set_log_file_path, 1);
    AMP_ADD_FUNCTION("setlogfilesize",     native_set_log_file_size, 1);

    duk_put_global_string(ctx, "ulog");

}
