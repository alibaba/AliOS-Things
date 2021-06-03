/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"

#include "smartcard.h"


#define MOD_STR        "Smartcard"

static duk_ret_t native_smartcard_init(duk_context *ctx)
{
    int ret;

    ret = smartcard_init();
    if (ret) {
        amp_error(MOD_STR, "smartcard init fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_smartcard_deinit(duk_context *ctx)
{
    smartcard_deinit();
    duk_push_int(ctx, 0);
    return 1;
}

static duk_ret_t native_smartcard_select(duk_context *ctx)
{
    int ret = -1;
    int type;
    smartcard_change_operator_t _operator;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    type = duk_get_int(ctx, 0);
    switch (type) {
    case 1:
        _operator = SMARTCARD_CHANGE_TO_CM;
        break;
    case 2:
        _operator = SMARTCARD_CHANGE_TO_CU;
        break;
    case 3:
        _operator = SMARTCARD_CHANGE_TO_CT;
        break;
    case 4:
        _operator = SMARTCARD_CHANGE_TO_NEXT;
        break;
    default:
        amp_error(MOD_STR, "unknown operator %d", type);
        return -1;
    }

    ret = smartcard_change_operator(type);
    if (ret) {
        amp_error(MOD_STR, "change operator %d fail %d", type, ret);
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void module_smartcard_clean(void)
{
    smartcard_deinit();
}

void module_smartcard_register(void)
{
    duk_context *ctx = be_get_context();

    amp_module_free_register(module_smartcard_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("init",    native_smartcard_init, 0);
    AMP_ADD_FUNCTION("deinit",  native_smartcard_deinit, 0);
    AMP_ADD_FUNCTION("select",  native_smartcard_select, 1);

    duk_put_prop_string(ctx, -2, "smartcard");
}

