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
static const unsigned char ota_pubn_buf[256] = {0xC9, 0x2E, 0x73, 0x39, 0xA3, 0x2A, 0xF8, 0xDC, 0x94, 0x86, 0x06, 0x9A, 0x23, 0xD9, 0x59, 0x3F, 0x8E, 0xDC, 0x96, 0x00, 0x0E, 0x50, 0x5B, 0x1F, 0xC2, 0x7E, 0xD2, 0x25, 0x7B, 0x98, 0x0B, 0x1C, 0x10, 0x2A, 0xBA, 0x1D, 0x10, 0x31, 0xB5, 0xB8, 0xD8, 0x9B, 0x6C, 0x20, 0x5D, 0xAD, 0x3B, 0x33, 0x64, 0xC7, 0xF1, 0x54, 0x41, 0xE6, 0x25, 0xBC, 0x9C, 0x3C, 0x79, 0xC3, 0xC3, 0x3C, 0xED, 0x88, 0x57, 0x36, 0x7E, 0x55, 0x14, 0x15, 0x1B, 0xE2, 0xDC, 0x2E, 0xFD, 0xB1, 0x8E, 0xD6, 0x45, 0x48, 0x6D, 0x2A, 0x53, 0x49, 0x15, 0x35, 0xBE, 0xC7, 0xC0, 0x5A, 0x44, 0x2A, 0x27, 0x7C, 0x43, 0x28, 0xF4, 0xFA, 0x45, 0xF7, 0xD0, 0x37, 0xDF, 0x29, 0xCF, 0x70, 0xAA, 0x83, 0x32, 0x4F, 0xD8, 0x4B, 0x25, 0xD3, 0x22, 0x15, 0x88, 0x36, 0x36, 0x04, 0xA9, 0x19, 0x05, 0x77, 0xAE, 0xB8, 0x9C, 0x9F, 0xDD, 0x5A, 0x08, 0x4F, 0xC5, 0x9B, 0xD9, 0x50, 0xC3, 0x8B, 0x0E, 0x56, 0x0F, 0x70, 0x2B, 0xF4, 0x91, 0x94, 0x74, 0x89, 0xC9, 0x62, 0x34, 0x70, 0x2A, 0xFC, 0x76, 0x24, 0x4D, 0x83, 0x9C, 0xFE, 0x47, 0xED, 0xF0, 0xEB, 0x94, 0x72, 0x63, 0x72, 0x49, 0xB3, 0xA2, 0x73, 0x31, 0xDE, 0x23, 0xEE, 0x1F, 0x2A, 0x0C, 0x1E, 0x3E, 0xEC, 0xF4, 0x2D, 0x76, 0x5B, 0x1E, 0x0C, 0x0A, 0xB5, 0x67, 0x36, 0xE4, 0xF3, 0x51, 0xFB, 0xD0, 0x6A, 0xC4, 0xFD, 0x97, 0xAD, 0x62, 0x2C, 0xC9, 0xC6, 0x78, 0xAE, 0x93, 0xBC, 0xD8, 0x98, 0x6B, 0x82, 0x8A, 0x5A, 0xCA, 0x79, 0xAD, 0xEF, 0xEF, 0x24, 0x74, 0xC1, 0x45, 0xC7, 0x8C, 0x6D, 0xC8, 0xF2, 0x1D, 0x98, 0x11, 0x52, 0x13, 0x9E, 0x2C, 0xC1, 0xDE, 0x79, 0x2B, 0x74, 0xB5, 0xB2, 0x75, 0xFA, 0xF1, 0x19, 0x67, 0xDD, 0x5B, 0xF0, 0x6A, 0xEB, 0x4E, 0x99};

static const unsigned char ota_pube_buf[3] = {0x01, 0x00, 0x01};

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
