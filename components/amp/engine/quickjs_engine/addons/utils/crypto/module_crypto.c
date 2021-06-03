/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_utils.h"
//#include "be_inl.h"

#include "mbedtls/md5.h"
#include "quickjs.h"
#include "crypto_adapter.h"
#include "quickjs_addon_common.h"

#define MOD_STR "CRYPTO"
#define DEBUG_HEX_DUMP(str, buff, len) hexdump(str, buff, len)
// #define DEBUG_HEX_DUMP(str, buff, len)

static JSClassID js_crypto_class_id;

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

static int crypto_char2hex(char c, char *x)
{
    if (c >= '0' && c <= '9') {
        *x = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        *x = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        *x = c - 'A' + 10;
    } else {
        return -1;
    }

    return 0;
}

static int crypto_hex2bin(char *hex, uint8_t *buf)
{
    char dec;
    int hexlen, i;

    hexlen = strlen(hex);
    if (hexlen == 0) {
        return 0;
    }

    /* if hexlen is uneven, return failed */
    if (hexlen % 2) {
        return 0;
    }

    /* regular hex conversion */
    for (i = 0; i < hexlen/2; i++) {
        if (crypto_char2hex(hex[2 * i], &dec) < 0) {
            return 0;
        }
        buf[i] = dec << 4;

        if (crypto_char2hex(hex[2 * i + 1], &dec) < 0) {
            return 0;
        }
        buf[i] += dec;
    }

    return hexlen/2;
}

static int crypto_bin2hex(char *hex, uint8_t *buf, uint8_t buf_len)
{
    int i;

    if (hex == NULL)
        return 0;

    hex[0] = 0;
    for (i = 0; i < buf_len; i++) {
        sprintf(hex+strlen(hex), "%02x", buf[i]);
    }

    return buf_len*2;
}

static JSValue native_crypto_encrypt(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    char *crypt_mode = NULL, *aes_type = NULL, *aes_padding = NULL;
    char *key = NULL;
    char *in = NULL;
    char *out = NULL;
    unsigned char *out_bin, *in_bin;
    unsigned char key_bin[ENC_KEY_SIZE];
    uint16_t in_length, out_length;
    int ret;
    JSValue JSret;
    JSValue JS_crypt_mode, JS_key, JS_in;

    JS_crypt_mode = JS_GetPropertyStr(ctx, argv[0], "crypt_mode");
    crypt_mode = JS_ToCString(ctx, JS_crypt_mode);
    if (strcasecmp(crypt_mode, "aes_cbc") != 0) {
        amp_error(MOD_STR, "not supported crypt_mode: %s", crypt_mode);
        goto out; /* now just support aes encryption */
    }

    /* input.key */
    JS_key = JS_GetPropertyStr(ctx, argv[1], "key");
    key = JS_ToCString(ctx, JS_key);
    if (key == NULL) {
        amp_warn(MOD_STR, "key not specify");
        goto out;
    }
    amp_info(MOD_STR, "key is %s", key);
    crypto_hex2bin(key, key_bin);

    /* input.in */
    JS_in = JS_GetPropertyStr(ctx, argv[1], "in");
    in = JS_ToCString(ctx, JS_in);
    if (in == NULL) {
        amp_warn(MOD_STR, "in not specify");
        goto out;
    }
    amp_info(MOD_STR, "in is %s", in);

    in_length = strlen(in);
    in_length /= 2;
    in_bin = aos_malloc(in_length+3);
    out_bin  = aos_malloc(in_length+16);
    crypto_hex2bin(in, in_bin);
    ret = crypto_adapter_encrypt(Encrypt_mode_AES_CBC, in_bin, in_length, out_bin, &out_length, key_bin);
    if (ret != 0) {
        amp_warn(MOD_STR, "encrypt failed");
        aos_free(in_bin);
        aos_free(out_bin);
        goto out;
    }

    out = aos_malloc(out_length*2+1);
    crypto_bin2hex(out, out_bin, out_length);
    amp_info(MOD_STR, "encrypted leng = %d, %s", out_length, out);

    JSret = JS_NewString(ctx, out);
    aos_free(out);
    return JSret;
out:
    return JS_NewString(ctx, "");
}

static JSValue native_crypto_decrypt(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    char *crypt_mode = NULL, *aes_type = NULL, *aes_padding = NULL;
    char *key = NULL;
    char *in = NULL;
    char *out = NULL;
    unsigned char *out_bin, *in_bin;
    unsigned char key_bin[ENC_KEY_SIZE];
    uint16_t in_length, out_length;
    int ret;
    JSValue JSret;
    JSValue JS_crypt_mode, JS_key, JS_in;

    JS_crypt_mode = JS_GetPropertyStr(ctx, argv[0], "crypt_mode");
    crypt_mode = JS_ToCString(ctx, JS_crypt_mode);
    if (strcasecmp(crypt_mode, "aes_cbc") != 0) {
        amp_error(MOD_STR, "not supprted crypte_mode");
        goto out; /* now just support aes encryption */
    }

    /* input.key */
    JS_key = JS_GetPropertyStr(ctx, argv[1], "key");
    key = JS_ToCString(ctx, JS_key);
    if (key == NULL) {
        amp_warn(MOD_STR, "key not specify");
        goto out;
    }
    amp_info(MOD_STR, "key is %s", key);
    crypto_hex2bin(key, key_bin);

    /* input.in */
    JS_in = JS_GetPropertyStr(ctx, argv[1], "in");
    in = JS_ToCString(ctx, JS_in);
    if (in == NULL) {
        amp_warn(MOD_STR, "in not specify");
        goto out;
    }
    amp_info(MOD_STR, "in is %s", in);

    in_length = strlen(in);
    in_length /= 2;
    in_bin = aos_malloc(in_length+3);
    out_bin  = aos_malloc(in_length+16);
    crypto_hex2bin(in, in_bin);
    ret = crypto_adapter_decrypt(Encrypt_mode_AES_CBC, in_bin, in_length, out_bin, &out_length, key_bin);
    if (ret != 0) {
        amp_warn(MOD_STR, "decrypt failed");
        aos_free(in_bin);
        aos_free(out_bin);
        goto out;
    }

    out = aos_malloc(out_length*2+1);
    crypto_bin2hex(out, out_bin, out_length);
    amp_info(MOD_STR, "decrypted len = %d, %s", out_length, out);

    JSret = JS_NewString(ctx, out);
    aos_free(out);
    return JSret;
out:
    return JS_NewString(ctx, "");
}

static JSClassDef js_crypto_class = {
    "CRYPTO",
};

static const JSCFunctionListEntry js_crypto_funcs[] = {
    JS_CFUNC_DEF("encrypt", 2, native_crypto_encrypt ),
    JS_CFUNC_DEF("decrypt", 2, native_crypto_decrypt ),
};

static int js_crypto_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_crypto_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_crypto_class_id, &js_crypto_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_crypto_funcs,
                               countof(js_crypto_funcs));
    JS_SetClassProto(ctx, js_crypto_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_crypto_funcs,
                                  countof(js_crypto_funcs));
}

JSModuleDef *js_init_module_crypto(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_crypto_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_crypto_funcs, countof(js_crypto_funcs));
    return m;
}


void module_crypto_register(void)
{
    amp_debug(MOD_STR, "module_crypto_register");
    JSContext *ctx = js_get_context();
    aos_printf("module crypto register\n");
    js_init_module_crypto(ctx, "CRYPTO");
}

