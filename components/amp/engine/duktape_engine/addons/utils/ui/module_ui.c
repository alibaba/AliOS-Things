/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "aos_network.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "amp_task.h"
#include "be_inl.h"

#define MOD_STR "UI"

char g_app_path[128];
extern void amp_page_changed_set(void);

duk_ret_t native_ui_redirect(duk_context *ctx)
{
    char *path;
    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto done;
    }

    path = duk_get_string(ctx, 0);
    if (path != NULL) {
        memset(&g_app_path[0], 0, sizeof(g_app_path));
        snprintf(&g_app_path[0],128,"%s",path);
        amp_debug(MOD_STR, "native_ui_redirect path is %s\n", path);
        amp_page_changed_set();
    }

    amp_debug(MOD_STR, "native_ui_redirect success");

done:
    return 1;
}

void module_ui_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    duk_push_c_function(ctx, native_ui_redirect, 1);
    duk_put_prop_string(ctx, -2, "redirectTo");

    duk_put_prop_string(ctx, -2, "UI");
}

