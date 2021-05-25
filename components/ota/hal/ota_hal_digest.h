/**@defgroup ota_hal_digest_api
 * @{
 * This is an include file of OTA verify interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef OTA_HAL_DIGEST_H
#define OTA_HAL_DIGEST_H
#include "ota_agent.h"

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************
***   OTA verify module: hash:md5/sha256 sign:RSA            ***
****************************************************************/
/**
 *  Struct:  MD5 Context.
 */
typedef struct {
    unsigned int  total[2];
    unsigned int  state[4];
    unsigned char buffer[64];
} ota_md5_context;

/**
 *  Struct:  SHA256 Context.
 */
typedef struct {
    unsigned int  total[2];
    unsigned int  state[8];
    unsigned char buffer[64];
    int is224;
} ota_sha256_context;

/**
 *  Struct:  ota sign context.
 */
typedef struct {
    char sign_enable;              /* enable sign */
    unsigned char sign_value[256]; /* sign value */
} ota_sign_t;

/**
 *  Struct:  ota hash context.
 */
typedef struct {
    unsigned char hash_method;         /* hash method: md5, sha256 */
    union {
        ota_md5_context md5_ctx;       /* md5 hash context */
        ota_sha256_context sha256_ctx; /* sh256 hash context */
    };
} ota_hash_ctx_t;

/**
 * ota_hash_init  ota hash init.
 *
 * @param[in] ota_hash_ctx_t *ctx   OTA hash context
 * @param[in] unsigned char type    OTA hash type
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_hash_init(ota_hash_ctx_t *ctx, unsigned char type);

/**
 * ota_hash_update  ota hash update.
 *
 * @param[in] ota_hash_ctx_t *ctx      OTA hash context
 * @param[in] const unsigned char *buf OTA hash buf
 * @param[in] unsigned int len         OTA hash len
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_hash_update(ota_hash_ctx_t *ctx, const unsigned char *buf, unsigned int len);

/**
 * ota_hash_final  OTA final hash.
 *
 * @param[in] ota_hash_ctx_t *ctx      OTA hash context
 * @param[in]  unsigned char *buf      OTA hash digest
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_hash_final(ota_hash_ctx_t *ctx, unsigned char *dgst);

/**
 * ota_verify_rsa  OTA verify RSA sign.
 *
 * @param[in]  unsigned char *sign  OTA firmware sign
 * @param[in]     const char *hash  OTA firmware hash
 * @param[in]  unsigned char hash_type  OTA hash type
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_verify_rsa(unsigned char *sign, const char *hash, unsigned char hash_type);

/*Verify API*/
/*SHA256*/
void ota_sha256_free(ota_sha256_context *ctx);
void ota_sha256_init(ota_sha256_context *ctx);
void ota_sha256_starts(ota_sha256_context *ctx, int is224);
void ota_sha256_update(ota_sha256_context *ctx, const unsigned char *input, unsigned int ilen);
void ota_sha256_finish(ota_sha256_context *ctx, unsigned char output[32]);
/*MD5*/
void ota_md5_free(ota_md5_context *ctx);
void ota_md5_init(ota_md5_context *ctx);
void ota_md5_starts(ota_md5_context *ctx);
void ota_md5_update(ota_md5_context *ctx, const unsigned char *input, unsigned int ilen);
void ota_md5_finish(ota_md5_context *ctx, unsigned char output[16]);
/*RSA*/
const unsigned char *ota_rsa_pubkey_n(void);
const unsigned char *ota_rsa_pubkey_e(void);
unsigned int ota_rsa_pubkey_n_size(void);
unsigned int ota_rsa_pubkey_e_size(void);
int ota_rsa_pubkey_verify(const unsigned char *pubkey_n,
                          const unsigned char *pubkey_e,
                          unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size,
                          const unsigned char *dig,
                          unsigned int dig_size,
                          const unsigned char *sig,
                          unsigned int sig_size);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /*OTA_HAL_DIGEST_H*/

