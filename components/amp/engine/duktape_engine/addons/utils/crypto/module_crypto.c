/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_utils.h"
#include "be_inl.h"

#include "mbedtls/md5.h"

#define MOD_STR "CRYPTO"
#define DEBUG_HEX_DUMP(str, buff, len) hexdump(str, buff, len)
// #define DEBUG_HEX_DUMP(str, buff, len)

#if 0
static duk_ret_t native_crypto_encrypt(duk_context *ctx)
{
    char *crypte_mode = NULL, *aes_type = NULL, *aes_padding = NULL;

    int i = 0;
    char *key = NULL;
    char *iv = NULL;
    unsigned char *input = NULL;
    unsigned char *dest = NULL;
    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_string(ctx, 2) || !duk_is_object(ctx, 3)){
        amp_error(MOD_STR, "invalid parameter");;
        goto done;
    }
    crypte_mode = (const char *)duk_get_string(ctx, 0);
    aes_type = (const char *)duk_get_string(ctx, 1);
    aes_padding = (const char *)duk_get_string(ctx, 2);

    if (strcasecmp(crypte_mode, "aes") != 0) {
        amp_error(MOD_STR, "not supprted crypte_mode");
        goto done; /* now just support aes encryption */
    }

    if (strcasecmp(aes_type, "cbc") != 0) {
        amp_warn(MOD_STR, "aes type only cbc supported");
        goto done;
    }

    /* input.key */
    memset(&options, 0, sizeof(udp_options_t));
    duk_get_prop_string(ctx, 1, "key");
    if (!duk_is_string(ctx, -1)) {
        amp_warn(MOD_STR, "key not specify");
        duk_pop(ctx);
        goto out;
    }
    key = (const char *)duk_get_string(ctx, -1);
    duk_pop(ctx);

    /* input.iv */
    duk_get_prop_string(ctx, 1, "iv");
    if (!duk_is_string(ctx, -1)) {
        amp_warn(MOD_STR, "iv not specify");
        duk_pop(ctx);
        goto out;
    }
    strncpy(options.ip, duk_get_string(ctx, -1), sizeof(options.ip) - 1);
    duk_pop(ctx);

    crypte_mode = (const char *)duk_get_string(ctx, 3);
    amp_debug(MOD_STR, "native_crypto_encrypt");

    if (!arg0 || !symbol_is_string(arg0) || !arg1 || !symbol_is_string(arg1) ||
        !arg2 || !symbol_is_string(arg2) || !arg3 || !symbol_is_object(arg3)) {
        goto done;
    }
    return 1;

done:
    duk_push_string(ctx, "");
    return 1;
}

#endif

static duk_ret_t native_crypto_decrypt(duk_context *ctx)
{
    amp_debug(MOD_STR, "native_crypto_decrypt");
    return 1;
}

static duk_ret_t native_crypto_md5(duk_context *ctx)
{
    char *src = NULL, *data = NULL;
    int src_len = 0;
    char *dest;
    amp_md5_context s_ctx;
    uint8_t result[16] = {0};
    char md5[33] = {0};
    int i              = 0;

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


void module_crypto_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    AMP_ADD_FUNCTION("md5",     native_crypto_md5, 1);
    // AMP_ADD_FUNCTION("encrypt", native_crypto_encrypt, 1);
    // AMP_ADD_FUNCTION("decrypt", native_crypto_decrypt, 1);

    duk_put_prop_string(ctx, -2, "crypto");
}
