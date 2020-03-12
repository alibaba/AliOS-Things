/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_HAL_DIGEST_H
#define OTA_HAL_DIGEST_H
#include "ota/ota_agent.h"

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
#endif /*OTA_HAL_DIGEST_H*/

