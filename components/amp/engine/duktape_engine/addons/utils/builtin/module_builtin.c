/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "be_inl.h"
#include "aos_fs.h"
#include "board_mgr.h"

#define MOD_STR "BUILTIN"
#define GLOBAL_APPCONFIG_STRING_1 "Object.defineProperty(this, 'appConfig', { value: "
#define GLOBAL_APPCONFIG_STRING_2 ", writable: false });"

extern int g_repl_config;
static duk_ret_t native_console_log(duk_context *ctx)
{
    duk_idx_t n = duk_get_top(ctx);
    duk_idx_t i;

    duk_get_global_string(ctx, "console");
    duk_get_prop_string(ctx, -1, "format");

    for (i = 0; i < n; i++)
    {
        if (duk_check_type_mask(ctx, i, DUK_TYPE_MASK_OBJECT))
        {
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
    const char *msg = duk_safe_to_string(ctx, -1);

    if (bone_console_get_log_flag()) {
        amp_console("%s", msg);
        if (g_repl_config) {
            #ifdef AMP_REPL_STDIO_EXTERNAL
            repl_printf("%s", msg);
            repl_printf("\r\n");
            #endif
        }
    }

    return 0;
}

/* export appConfig */
static void native_add_global_appconfig(duk_context *ctx)
{
    char json_dir[32] = {0};
    char *json = NULL;
    void *json_data = NULL;
    uint32_t len = 0;
    uint32_t global_config_len = 0;
    int  json_fd = -1;

    snprintf(json_dir, 32, AMP_APP_MAIN_JSON);

    // amp_debug(MOD_STR, "path:%s", json_dir);
    if ((json_fd = aos_open(json_dir, O_RDONLY)) < 0) {
        amp_error(MOD_STR, "open:%s fail", json_dir);
        return;
    }
    // amp_debug(MOD_STR, "jse_lseek:%d", *(int *)json_fd);
    len = aos_lseek(json_fd, 0, SEEK_END);
    printf("%s %d\r\n", __func__, len);

    global_config_len = len + strlen(GLOBAL_APPCONFIG_STRING_1) + strlen(GLOBAL_APPCONFIG_STRING_2) + 1;
    // amp_debug(MOD_STR, "jse_lseek, len: %d, globallen:%d", len, global_config_len);
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

    if (duk_peval_string(ctx, json_data) != DUK_EXEC_SUCCESS) {
        amp_console("export appConfig failed: %s\n", duk_safe_to_stacktrace(ctx, -1));
    }

    duk_pop(ctx);
    amp_debug(MOD_STR, "export appConfig");

    aos_close(json_fd);
    aos_free(json_data);
}

void module_builtin_register(void)
{
    amp_debug(MOD_STR, "module_builtin_register");
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    /* Custom function to format objects; user can replace.
     * For now, try JX-formatting and if that fails, fall back
     * to ToString(v).
     */
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

    duk_push_c_function(ctx, native_console_log, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "log");

    duk_put_global_string(ctx, "console");

    /* print module */
    duk_push_c_function(ctx, native_console_log, DUK_VARARGS);
    duk_put_global_string(ctx, "print");

    /* export appConfig */
    native_add_global_appconfig(ctx);
}
