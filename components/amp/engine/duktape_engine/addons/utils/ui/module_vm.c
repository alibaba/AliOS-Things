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

#define MOD_STR "MODEL"

duk_ret_t native_model_setdata(duk_context *ctx);

void module_vm_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    duk_push_c_function(ctx, native_model_setdata, 1);
    duk_put_prop_string(ctx, -2, "setData");

    duk_put_prop_string(ctx, -2, "VM");
}
