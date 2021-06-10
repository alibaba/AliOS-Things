/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "amp_utils.h"
#include "crc16.h"

#define MOD_STR "CHECKSUM"

static duk_ret_t native_checksum_md5(duk_context *ctx)
{
    char *src = NULL, *data = NULL;
    int src_len = 0;
    char *dest;
    amp_md5_context s_ctx;
    uint8_t result[16] = {0};
    char md5[33] = {0};
    int i = 0;

    // amp_debug(MOD_STR, "native_crypto_md5, type:%d %d", duk_get_type(ctx, 0), duk_get_length(ctx, 0));
    if (!duk_is_object(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter");
        goto done;
    }

    src_len = duk_get_length(ctx, 0);
    src     = (char *)aos_malloc(src_len);
    if (!src) {
        amp_error(MOD_STR, "allocate memory failed");
        goto done;
    }

    for (i = 0; i < src_len; i++) {
        duk_get_prop_index(ctx, 0, i);
        src[i] = (char)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    // DEBUG_HEX_DUMP("content data:", src, src_len);
    amp_md5(src, src_len, result);
    // DEBUG_HEX_DUMP("result data:", result, 16);
    
    if (src) {
        aos_free(src);
    }

    for(i = 0; i < 16; ++i){
        num2hex(result[i], &md5[2 * i]);
    }
    // amp_debug(MOD_STR, "md5: %s", md5);
    duk_push_string(ctx, md5);
    return 1;

done:
    duk_push_string(ctx, "");
    return 1;
}

static duk_ret_t native_checksum_crc16(duk_context *ctx)
{
    char *src = NULL;
    int src_len = 0;
    uint16_t result;
    int i = 0;

    // amp_debug(MOD_STR, "native_crypto_md5, type:%d %d", duk_get_type(ctx, 0), duk_get_length(ctx, 0));
    // if (!duk_is_number(ctx, 0)) {
    //     amp_warn(MOD_STR, "invalid parameter");
    //     goto done;
    // }

    src_len = duk_get_length(ctx, 0);
    src     = (char *)aos_malloc(src_len);
    if (!src) {
        amp_error(MOD_STR, "allocate memory failed");
        goto done;
    }

    for (i = 0; i < src_len; i++) {
        duk_get_prop_index(ctx, 0, i);
        src[i] = (char)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    result = crc16(src, src_len);
    
    if (src) {
        aos_free(src);
    }

    duk_push_int(ctx, result);
    return 1;

done:
    duk_push_string(ctx, "");
    return 1;
}

static duk_ret_t native_checksum_crc32(duk_context *ctx)
{
    char *src = NULL;
    int src_len = 0;
    uint32_t result;
    int i = 0;

    // amp_debug(MOD_STR, "native_crypto_md5, type:%d %d", duk_get_type(ctx, 0), duk_get_length(ctx, 0));
    // if (!duk_is_number(ctx, 0)) {
    //     amp_warn(MOD_STR, "invalid parameter");
    //     goto done;
    // }

    src_len = duk_get_length(ctx, 0);
    src     = (char *)aos_malloc(src_len);
    if (!src) {
        amp_error(MOD_STR, "allocate memory failed");
        goto done;
    }

    for (i = 0; i < src_len; i++) {
        duk_get_prop_index(ctx, 0, i);
        src[i] = (char)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    result = crc32(src, src_len);
    
    if (src) {
        aos_free(src);
    }

    duk_push_int(ctx, result);
    return 1;

done:
    duk_push_string(ctx, "");
    return 1;
}

void module_checksum_register(void)
{
    amp_debug(MOD_STR, "module_checksum_register");
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("md5",   native_checksum_md5, 1);
    AMP_ADD_FUNCTION("crc16", native_checksum_crc16, 1);
    AMP_ADD_FUNCTION("crc32", native_checksum_crc32, 1);

    duk_put_prop_string(ctx, -2, "CHECKSUM");
}
