/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_digest.h"

#if defined OTA_CONFIG_ITLS
#include "ali_crypto.h"
#else
#include "mbedtls/sha256.h"
#include "mbedtls/md5.h"
#include "mbedtls/bignum.h"
#include "mbedtls/rsa.h"
#endif

/* RSA Public Key:User needs sign in alibaba cloud to get and replace them. */
static const unsigned char ota_pube_buf[3] = {0x01, 0x00, 0x01};
static const unsigned char ota_pubn_buf[256] = {
    0xd7, 0xff, 0xc1, 0x1c, 0x68, 0xe1, 0x87, 0xd2,
    0x2e, 0x5e, 0xd3, 0x8e, 0x5f, 0xb6, 0xad, 0xa9,
    0x7d, 0xd2, 0xc4, 0xcf, 0xa8, 0x62, 0x70, 0x79,
    0x6d, 0x6e, 0x5e, 0x67, 0x8f, 0x94, 0x82, 0x68,
    0x32, 0x91, 0xf5, 0xb7, 0x7e, 0x00, 0x92, 0x97,
    0x27, 0x4b, 0xd6, 0xb3, 0x88, 0x1b, 0x03, 0xb3,
    0x5c, 0xd6, 0x59, 0xb1, 0xda, 0xfc, 0x6d, 0x41,
    0x9b, 0x2b, 0xe4, 0xb3, 0xe8, 0xc2, 0xb0, 0x6b,
    0x2e, 0x3d, 0x57, 0x28, 0xc8, 0x8b, 0x2e, 0x2f,
    0x98, 0x59, 0x82, 0xc7, 0xe5, 0x8a, 0x68, 0x2f,
    0x30, 0xc2, 0x8c, 0xd0, 0x6b, 0x20, 0x96, 0xa8,
    0x73, 0x37, 0xa1, 0xe4, 0xf6, 0xa9, 0xaa, 0xe1,
    0x1c, 0xf3, 0xac, 0x3b, 0xe4, 0x8a, 0xfe, 0x69,
    0xb7, 0x0a, 0xa2, 0xee, 0x2b, 0x80, 0xdc, 0x40,
    0x96, 0x1b, 0xc5, 0x24, 0x9e, 0x9c, 0xf4, 0x11,
    0xda, 0xaf, 0x7e, 0xd2, 0x27, 0x75, 0xc9, 0x7f,
    0xc6, 0x1e, 0xd4, 0x5d, 0xdc, 0x1c, 0x11, 0x81,
    0x54, 0xc8, 0x31, 0x91, 0x35, 0x4f, 0x99, 0x37,
    0xee, 0x30, 0x7f, 0x7a, 0x88, 0x43, 0xe6, 0xc7,
    0x9b, 0x22, 0xfc, 0xd7, 0xe9, 0x2b, 0x0d, 0x31,
    0xf5, 0xbf, 0xa6, 0x3e, 0x1a, 0x94, 0x50, 0xf0,
    0xce, 0x1f, 0xb6, 0xab, 0x47, 0x92, 0xc0, 0x13,
    0x83, 0xda, 0xfc, 0x16, 0xa9, 0x37, 0x4b, 0x02,
    0x29, 0xf9, 0x2d, 0x80, 0xa8, 0xee, 0xc5, 0x46,
    0xa6, 0xcd, 0xf7, 0x19, 0x75, 0x35, 0x92, 0x20,
    0x6f, 0x5d, 0xf1, 0x2a, 0x5b, 0xb5, 0x70, 0xa1,
    0x74, 0xda, 0x59, 0xc0, 0x18, 0x37, 0xab, 0xa6,
    0x74, 0x77, 0xe9, 0x76, 0xa9, 0x05, 0x73, 0x37,
    0x7a, 0x71, 0xad, 0x89, 0x34, 0x8c, 0xc4, 0x02,
    0x25, 0x48, 0x33, 0x7d, 0xda, 0x50, 0x29, 0x61,
    0x36, 0xf8, 0x5a, 0x51, 0xae, 0xc4, 0xc0, 0xed,
    0xac, 0x6a, 0xbf, 0x12, 0x9a, 0xc4, 0xe7, 0x59};

/* SHA256 */
void ota_sha256_free(ota_sha256_context *ctx)
{
#if !defined OTA_CONFIG_ITLS
    mbedtls_sha256_free((mbedtls_sha256_context *)ctx);
#else
    if(ctx == NULL) {
        return;
    }
    if(ctx->ali_ctx) {
        ota_free(ctx->ali_ctx);
    }
    memset(ctx, 0, sizeof(ota_sha256_context));
#endif
}

void ota_sha256_init(ota_sha256_context *ctx)
{
#if !defined OTA_CONFIG_ITLS
    mbedtls_sha256_init((mbedtls_sha256_context *)ctx);
#else
    memset(ctx, 0, sizeof(ota_sha256_context));
#endif
}

void ota_sha256_starts(ota_sha256_context *ctx, int is224)
{
#if !defined OTA_CONFIG_ITLS
    mbedtls_sha256_starts((mbedtls_sha256_context *)ctx, is224);
#else
    size_t ctx_size;
    ali_crypto_result result;
    if (NULL == ctx->ali_ctx) {
        result = ali_hash_get_ctx_size(SHA256, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            OTA_LOG_E("get ctx size fail - 0x%x\n", result);
        }

        ctx->size = ctx_size;
        ctx->ali_ctx = ota_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            OTA_LOG_I("mbedtls_calloc(%d) fail\n", (int)ctx_size);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }
    result = ali_hash_init(SHA256, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        OTA_LOG_I("sha256 init fail - 0x%x\n", result);
    }
#endif
}

void ota_sha256_update(ota_sha256_context *ctx, const unsigned char *input, unsigned int ilen)
{
#if !defined OTA_CONFIG_ITLS
    mbedtls_sha256_update((mbedtls_sha256_context *)ctx, input, ilen);
#else
    ali_crypto_result result;
    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        OTA_LOG_I("sha256 update fail - 0x%x\n", result);
    }
#endif
}

void ota_sha256_finish(ota_sha256_context *ctx, unsigned char output[32])
{
#if !defined OTA_CONFIG_ITLS
    mbedtls_sha256_finish((mbedtls_sha256_context *)ctx, output);
#else
    ali_crypto_result result;
    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        OTA_LOG_I("sha256 final fail - 0x%x\n", result);
    }
#endif
}

/*MD5*/
void ota_md5_free(ota_md5_context *ctx)
{
#if defined OTA_CONFIG_BLE
#elif !defined OTA_CONFIG_ITLS
    mbedtls_md5_free((mbedtls_md5_context *)ctx);
#else
    if(ctx == NULL) {
        return;
    }
    if(ctx->ali_ctx) {
        ota_free(ctx->ali_ctx);
    }
    memset(ctx, 0, sizeof(ota_md5_context));
#endif
}

void ota_md5_init(ota_md5_context *ctx)
{
#if defined OTA_CONFIG_BLE
#elif !defined OTA_CONFIG_ITLS
    mbedtls_md5_init((mbedtls_md5_context *)ctx);
#else
    memset(ctx, 0, sizeof(ota_md5_context));
#endif
}

void ota_md5_starts(ota_md5_context *ctx)
{
#if defined OTA_CONFIG_BLE
#elif !defined OTA_CONFIG_ITLS
    mbedtls_md5_starts((mbedtls_md5_context *)ctx);
#else
    size_t ctx_size;
    ali_crypto_result result;
    if (NULL == ctx->ali_ctx) {
        result = ali_hash_get_ctx_size(MD5, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            OTA_LOG_I("get ctx size fail - 0x%x\n", result);
        }

        ctx->size = ctx_size;
        ctx->ali_ctx = ota_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            OTA_LOG_I("mbedtls_calloc(%d) fail\n", (int)ctx_size);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }
    result = ali_hash_init(MD5, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        OTA_LOG_I("md5 init fail - 0x%x\n", result);
    }
#endif
}

void ota_md5_update(ota_md5_context *ctx, const unsigned char *input, unsigned int ilen)
{
#if defined OTA_CONFIG_BLE
#elif !defined OTA_CONFIG_ITLS
    mbedtls_md5_update((mbedtls_md5_context *)ctx, input, ilen);
#else
    ali_crypto_result result;
    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        OTA_LOG_I("md5 update fail - 0x%x\n", result);
    }
#endif
}

void ota_md5_finish(ota_md5_context *ctx, unsigned char output[16])
{
#if defined OTA_CONFIG_BLE
#elif !defined OTA_CONFIG_ITLS
    mbedtls_md5_finish((mbedtls_md5_context *)ctx, output);
#else
    ali_crypto_result result;
    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        OTA_LOG_I("md5 final fail - 0x%x\n", result);
    }
#endif
}

/*RSA*/
const unsigned char *ota_rsa_pubkey_n(void)
{
    return ota_pubn_buf;
}

const unsigned char *ota_rsa_pubkey_e(void)
{
    return ota_pube_buf;
}

unsigned int ota_rsa_pubkey_n_size(void)
{
    return sizeof(ota_pubn_buf);
}

unsigned int ota_rsa_pubkey_e_size(void)
{
    return sizeof(ota_pube_buf);
}

int ota_rsa_pubkey_verify(const unsigned char *pubkey_n,
                          const unsigned char *pubkey_e,
                          unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size,
                          const unsigned char *dig,
                          unsigned int dig_size,
                          const unsigned char *sig,
                          unsigned int sig_size)
{
    int                   ret = 0;
#if !defined (OTA_CONFIG_ITLS)
    mbedtls_rsa_context   ctx;

    if (pubkey_n == NULL || pubkey_n == NULL || dig == NULL || sig == NULL) {
        ret = OTA_VERIFY_RSA_FAIL;
        goto EXIT;
    }
    if (pubkey_n_size == 0 || pubkey_e_size == 0 || sig_size == 0 || dig_size != OTA_SHA256_HASH_SIZE) {
        ret = OTA_VERIFY_RSA_FAIL;
        goto EXIT;
    }
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);
    ret = mbedtls_mpi_read_binary(&ctx.N, pubkey_n, pubkey_n_size);
    if (0 != ret) {
       goto EXIT;
    }
    ret = mbedtls_mpi_read_binary(&ctx.E, pubkey_e, pubkey_e_size);
    if (0 != ret) {
        goto EXIT;
    }
    ctx.len = pubkey_n_size;
    ret = mbedtls_rsa_check_pubkey(&ctx);
    if (0 != ret) {
        goto EXIT;
    }
    ret = mbedtls_rsa_pkcs1_verify(&ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256,(unsigned int)0, (const unsigned char *)dig, (const unsigned char *)sig);
    if (0 != ret) {
        goto EXIT;
    }
EXIT:
    if(ret != 0) {
        OTA_LOG_E("rsa verify ret: 0x%x", ret);
    }
    mbedtls_rsa_free(&ctx);
#endif
    return ret;
}
