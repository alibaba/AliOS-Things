/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "jse_port.h"
#include "bone_engine_inl.h"
#ifdef JSE_IDE_DEBUG
#include "websocket.h"
#endif

static duk_ret_t native_console_log(duk_context *ctx)
{
    duk_push_string(ctx, "");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);
    const char *msg = duk_safe_to_string(ctx, -1);

    if (bone_console_get_log_flag()) {
        jse_debug(BonePrefix "%s\n", msg);
        fflush(stdout);
    }

#ifdef JSE_IDE_DEBUG
    char *buf = (char *)jse_malloc(sizeof(BonePrefix) + strlen(msg));
    sprintf(buf, BonePrefix "%s", msg);
    bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_INFO, (char *)buf);
    jse_free(buf);
#endif
    return 0;
}

void module_builtin_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_console_log, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "log");

    duk_put_global_string(ctx, "console");

    /* print module */
    duk_push_c_function(ctx, native_console_log, DUK_VARARGS);
    duk_put_global_string(ctx, "print");
}
