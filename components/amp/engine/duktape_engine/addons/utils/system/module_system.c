/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"
#include "amp_utils.h"

#define MOD_STR "SYSTEM"

static duk_ret_t native_module_versions(duk_context *ctx)
{
    duk_push_object(ctx);

    duk_push_string(ctx, HAL_Get_System_Version());
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

static duk_ret_t native_system_platform(duk_context *ctx)
{
    duk_push_string(ctx, HAL_Get_system_platform_type());
    return 1;
}

static duk_ret_t native_system_uptime(duk_context *ctx)
{
    duk_push_number(ctx, HAL_UptimeMs());
    return 1;
}

static duk_ret_t native_system_memory(duk_context *ctx)
{
    int32_t ret = -1;

    amp_heap_info_t heap_info;

    ret = HAL_Heap_Memory_Info(&heap_info);
    if (ret != 0) {
        amp_warn(MOD_STR, "get heap memory failed");
        goto out;
    }

    duk_push_object(ctx);

    duk_push_number(ctx, heap_info.heap_total);
    duk_put_prop_string(ctx, -2, "heapTotal");

    duk_push_number(ctx, heap_info.heap_used);
    duk_put_prop_string(ctx, -2, "heapUsed");

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

    /* sdk version */
    duk_push_c_function(ctx, native_module_versions, 0);
    duk_put_prop_string(ctx, -2, "versions");

    /* version */
    duk_push_c_function(ctx, native_system_version, 0);
    duk_put_prop_string(ctx, -2, "version");

    /* platform */
    duk_push_c_function(ctx, native_system_platform, 0);
    duk_put_prop_string(ctx, -2, "platform");

    /* uptime() */
    duk_push_c_function(ctx, native_system_uptime, 0);
    duk_put_prop_string(ctx, -2, "uptime");

    /* memory() */
    duk_push_c_function(ctx, native_system_memory, 0);
    duk_put_prop_string(ctx, -2, "memory");

    /* memory() */
    duk_push_c_function(ctx, native_system_gc, 0);
    duk_put_prop_string(ctx, -2, "gc");

    duk_put_global_string(ctx, "system");
}
