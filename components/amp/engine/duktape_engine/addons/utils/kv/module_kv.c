/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_kv.h"
#include "amp_defines.h"
#include "wrappers_defs.h"
#include "be_inl.h"

#define MOD_STR "KV"
#define KV_BUFFER_MAX_LEN   256
/*****************************************************************************
 * Function:    native_fs_issupport
 * Description: js native addon for FILE.issupport()
 *            check if the js FILE object is support
 * Called by:   js api
 * Input:       none
 * Output:      1 :support ,0 :not support
 *****************************************************************************/
static duk_ret_t native_kv_setStorage(duk_context *ctx)
{
    int32_t ret = -1;
    int32_t value_len = 0;
    const char *key;
    const char *value;
    
    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    key = duk_get_string(ctx, 0);
    value = duk_get_string(ctx, 1);
    value_len = strlen(value);

    ret = HAL_Kv_Set(key, value, value_len, 1);
    if (ret != 0) {
        amp_warn(MOD_STR, "kv set storage failed");
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
static duk_ret_t native_kv_getStorage(duk_context *ctx)
{
    int32_t ret = -1;
    int32_t value_len = KV_BUFFER_MAX_LEN;
    const char *key;
    char *value;
    
    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    key = duk_get_string(ctx, 0);

    value = (char *)amp_malloc(KV_BUFFER_MAX_LEN);
    if (value == NULL) {
        amp_warn(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    ret = HAL_Kv_Get(key, value, &value_len);
    if (ret != 0) {
        amp_warn(MOD_STR, "kv get storage failed");
        amp_free(value);
        goto out;
    }

    duk_push_lstring(ctx, value, value_len);
    amp_free(value);
    return 1;

out:
    duk_push_undefined(ctx);
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
static duk_ret_t native_kv_removeStorage(duk_context *ctx)
{
    int32_t ret = -1;
    int32_t value_len = 0;
    const char *key;
    char *value;
    
    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    key = duk_get_string(ctx, 0);

    ret = HAL_Kv_Del(key);
    if (ret != 0) {
        amp_warn(MOD_STR, "kv delete item failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_kv_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_kv_setStorage, 2);
    duk_put_prop_string(ctx, -2, "setStorageSync");

    duk_push_c_function(ctx, native_kv_getStorage, 1);
    duk_put_prop_string(ctx, -2, "getStorageSync");

    duk_push_c_function(ctx, native_kv_removeStorage, 1);
    duk_put_prop_string(ctx, -2, "removeStorageSync");

    duk_put_prop_string(ctx, -2, "KV");
}
