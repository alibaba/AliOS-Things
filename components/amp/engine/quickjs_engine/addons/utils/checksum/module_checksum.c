/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "board_mgr.h"
#include "amp_utils.h"
#include "crc16.h"
#include "crc32.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "CHECKSUM"

static JSClassID js_checksum_class_id;

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

static int checksum_char2hex(char c, char *x)
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

static int checksum_hex2bin(char *hex, uint8_t *buf)
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
        if (checksum_char2hex(hex[2 * i], &dec) < 0) {
            return 0;
        }
        buf[i] = dec << 4;

        if (checksum_char2hex(hex[2 * i + 1], &dec) < 0) {
            return 0;
        }
        buf[i] += dec;
    }

    return hexlen/2;
}

static int checksum_bin2hex(char *hex, uint8_t *buf, uint8_t buf_len)
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

static JSValue native_checksum_md5(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    char *src = NULL, *src_bin = NULL;
    int src_len = 0;
    char *dest;
    amp_md5_context s_ctx;
    uint8_t result[16] = {0};
    char md5[33] = {0};
    int i = 0;
    JSValue JS_src;

    JS_src = JS_GetPropertyStr(ctx, argv[0], "src");
    src = JS_ToCString(ctx, JS_src);
    src_len = strlen(src);
    src_len /= 2;
    src_bin  = (char *)aos_malloc(src_len);
    if (src_bin == NULL) {
        amp_error(MOD_STR, "allocate memory failed");
        goto done;
    }
    checksum_hex2bin(src, src_bin);

    amp_md5(src_bin, src_len, result);

    if (src_bin) {
        aos_free(src_bin);
    }
    checksum_bin2hex(md5, result, 16);
    amp_debug(MOD_STR, "md5: %s", md5);
    return JS_NewString(ctx, md5);
done:
    return JS_NewString(ctx, "");
}

static JSValue native_checksum_crc16(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    char *src = NULL, *src_bin = NULL;
    int src_len = 0;
    char *dest;
    int32_t result;
    char md5[33] = {0};
    int i = 0;
    JSValue JS_src;

    JS_src = JS_GetPropertyStr(ctx, argv[0], "src");
    src = JS_ToCString(ctx, JS_src);
    amp_debug(MOD_STR, "src: %s", src);
    src_len = strlen(src);
    src_len /= 2;
    src_bin  = (char *)aos_malloc(src_len);
    if (src_bin == NULL) {
        amp_error(MOD_STR, "allocate memory failed");
        goto done;
    }
    checksum_hex2bin(src, src_bin);

    result = utils_crc16(src_bin, src_len);
    if (src_bin) {
        aos_free(src_bin);
    }

    amp_debug(MOD_STR, "crc16: %x", result);
    return JS_NewInt32(ctx, result);
done:
    return JS_NewInt32(ctx, -1);
}

static JSValue native_checksum_crc32(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    char *src = NULL, *src_bin = NULL;
    int src_len = 0;
    char *dest;
    int32_t result;
    char md5[33] = {0};
    int i = 0;
    JSValue JS_src;

    JS_src = JS_GetPropertyStr(ctx, argv[0], "src");
    src = JS_ToCString(ctx, JS_src);
    amp_debug(MOD_STR, "src: %s", src);
    src_len = strlen(src);
    src_len /= 2;
    src_bin  = (char *)aos_malloc(src_len);
    if (src_bin == NULL) {
        amp_error(MOD_STR, "allocate memory failed");
        goto done;
    }
    checksum_hex2bin(src, src_bin);

    result = utils_crc32(src_bin, src_len);
    if (src_bin) {
        aos_free(src_bin);
    }

    amp_debug(MOD_STR, "crc32: %x", result);
    return JS_NewInt32(ctx, result);
done:
    return JS_NewInt32(ctx, -1);
}

static JSClassDef js_checksum_class = {
    "CHECKSUM",
};

static const JSCFunctionListEntry js_checksum_funcs[] = {
    JS_CFUNC_DEF("md5", 1, native_checksum_md5 ),
    JS_CFUNC_DEF("crc16", 1, native_checksum_crc16 ),
    JS_CFUNC_DEF("crc32", 1, native_checksum_crc32 ),
};

static int js_checksum_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_checksum_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_checksum_class_id, &js_checksum_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_checksum_funcs,
                               countof(js_checksum_funcs));
    JS_SetClassProto(ctx, js_checksum_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_checksum_funcs,
                                  countof(js_checksum_funcs));
}

JSModuleDef *js_init_module_checksum(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_checksum_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_checksum_funcs, countof(js_checksum_funcs));
    return m;
}

void module_checksum_register(void)
{
    amp_debug(MOD_STR, "module_checksum_register");

    JSContext *ctx = js_get_context();
    aos_printf("module checksum register\n");
    js_init_module_checksum(ctx, "CHECKSUM");
}
