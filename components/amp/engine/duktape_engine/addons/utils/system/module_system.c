/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"
#include "amp_utils.h"

#define MOD_STR "SYSTEM"

extern const char *amp_jsapp_version_get(void);

static duk_ret_t native_module_versions(duk_context *ctx)
{
    duk_push_object(ctx);

    duk_push_string(ctx, aos_get_system_version());
    duk_put_prop_string(ctx, -2, "module");

    return 1;
}

static duk_ret_t native_system_version(duk_context *ctx)
{
    char version[AMP_VERSION_LENGTH] = {0};
    amp_version_get(version);

    duk_push_string(ctx, version);
    return 1;
}

static duk_ret_t native_system_app_version(duk_context *ctx)
{
    const char *version = amp_jsapp_version_get();
    if (strlen(version) == 0)
        version = "null";
    duk_push_string(ctx, version);
    return 1;
}

static duk_ret_t native_system_platform(duk_context *ctx)
{
    duk_push_string(ctx, aos_get_platform_type());
    return 1;
}

static duk_ret_t native_system_uptime(duk_context *ctx)
{
    duk_push_number(ctx, aos_now_ms());
    return 1;
}

static duk_ret_t native_system_memory(duk_context *ctx)
{
    int32_t ret = -1;

    amp_heap_info_t heap_info;

    ret = amp_heap_memory_info(&heap_info);
    if (ret != 0) {
        amp_warn(MOD_STR, "get heap memory failed");
        goto out;
    }

    duk_push_object(ctx);

    duk_push_number(ctx, heap_info.heap_total);
    duk_put_prop_string(ctx, -2, "heapTotal");

    duk_push_number(ctx, heap_info.heap_used);
    duk_put_prop_string(ctx, -2, "heapUsed");

    duk_push_number(ctx, heap_info.heap_free);
    duk_put_prop_string(ctx, -2, "heapFree");

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_system_gc(duk_context *ctx)
{
    int32_t ret = -1;

    duk_gc(ctx, 0);
    duk_gc(ctx, 0);

    duk_push_true(ctx);
    return 1;
}

void module_system_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    AMP_ADD_FUNCTION("versions", native_module_versions, 0);
    AMP_ADD_FUNCTION("version",  native_system_version, 0);
    AMP_ADD_FUNCTION("appversion",  native_system_app_version, 0);
    AMP_ADD_FUNCTION("platform", native_system_platform, 0);
    AMP_ADD_FUNCTION("uptime",   native_system_uptime, 0);
    AMP_ADD_FUNCTION("memory",   native_system_memory, 0);
    AMP_ADD_FUNCTION("gc",       native_system_gc, 0);

    duk_put_global_string(ctx, "system");
}
