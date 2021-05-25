/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_digest.h"
#include "mbedtls/sha256.h"
#include "mbedtls/md5.h"
#include "mbedtls/bignum.h"
#include "mbedtls/rsa.h"

/* RSA Public Key:User needs sign in alibaba cloud to get and replace them. */
static unsigned char ota_pubn_buf[256] = {0x00};

static unsigned char ota_pube_buf[3] = {0x01, 0x00, 0x01};

/* SHA256 */
void ota_sha256_free(ota_sha256_context *ctx)
{
    mbedtls_sha256_free((mbedtls_sha256_context *)ctx);
}

void ota_sha256_init(ota_sha256_context *ctx)
{
    mbedtls_sha256_init((mbedtls_sha256_context *)ctx);
}

void ota_sha256_starts(ota_sha256_context *ctx, int is224)
{
    mbedtls_sha256_starts((mbedtls_sha256_context *)ctx, is224);
}

void ota_sha256_update(ota_sha256_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_sha256_update((mbedtls_sha256_context *)ctx, input, ilen);
}

void ota_sha256_finish(ota_sha256_context *ctx, unsigned char output[32])
{
    mbedtls_sha256_finish((mbedtls_sha256_context *)ctx, output);
}

/*MD5*/
void ota_md5_free(ota_md5_context *ctx)
{
    mbedtls_md5_free((mbedtls_md5_context *)ctx);
}

void ota_md5_init(ota_md5_context *ctx)
{
    mbedtls_md5_init((mbedtls_md5_context *)ctx);
}

void ota_md5_starts(ota_md5_context *ctx)
{
    mbedtls_md5_starts((mbedtls_md5_context *)ctx);
}

void ota_md5_update(ota_md5_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_md5_update((mbedtls_md5_context *)ctx, input, ilen);
}

void ota_md5_finish(ota_md5_context *ctx, unsigned char output[16])
{
    mbedtls_md5_finish((mbedtls_md5_context *)ctx, output);
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
    ret = mbedtls_rsa_pkcs1_verify(&ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, (unsigned int)0, (const unsigned char *)dig, (const unsigned char *)sig);
    if (0 != ret) {
        goto EXIT;
    }
EXIT:
    if (ret != 0) {
        OTA_LOG_E("rsa verify ret: 0x%x", ret);
    }
    mbedtls_rsa_free(&ctx);
    return ret;
}
