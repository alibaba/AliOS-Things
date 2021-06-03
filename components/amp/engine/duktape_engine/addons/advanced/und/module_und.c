/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "amp_defines.h"
#include "und.h"
#include "be_inl.h"

#define MOD_STR "UND"
/*****************************************************************************
 * Function:    native_fs_issupport
 * Description: js native addon for FILE.issupport()
 *            check if the js FILE object is support
 * Called by:   js api
 * Input:       none
 * Output:      1 :support ,0 :not support
 *****************************************************************************/
static duk_ret_t native_und_start(duk_context *ctx)
{
    int32_t ret = -1;

    ret = und_init();
    if (ret != 0) {
        amp_warn(MOD_STR, "und init failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_read
 * Description: js native addon for FILE.read(filepath)
 *            read the file content
 * Called by:   js api
 * Input:       filepath : string
 * Output:      a String object which the file content is
 *****************************************************************************/
static duk_ret_t native_und_update(duk_context *ctx)
{
    int32_t ret = -1;
    int32_t cap_idx;
    int32_t reason_code;
    
    if (!duk_is_number(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    cap_idx = duk_get_int(ctx, 0);
    reason_code = duk_get_int(ctx, 1);

    ret = und_update_statis(cap_idx, reason_code);
    if (ret != 0) {
        amp_warn(MOD_STR, "und update statis failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*****************************************************************************
 * Function:    native_fs_delete
 * Description: js native addon for FILE.delete(filepath)
 *            delete the file
 * Called by:   js api
 * Input:       filepath : string
 * Output:      0 delete ok ;other delete fail
 *****************************************************************************/
static duk_ret_t native_und_stop(duk_context *ctx)
{
    int32_t ret = -1;

    ret = und_deinit();
    if (ret != 0) {
        amp_warn(MOD_STR, "und deinit failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_und_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("start",  native_und_start, 0);
    AMP_ADD_FUNCTION("update", native_und_update, 2);
    AMP_ADD_FUNCTION("stop",   native_und_stop, 0);

    duk_put_global_string(ctx, "UND");
}
