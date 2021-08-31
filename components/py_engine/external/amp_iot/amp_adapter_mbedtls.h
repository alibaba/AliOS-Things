/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/rsa.h"
#include "mbedtls/md5.h"
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/bignum.h"
#include "mbedtls/base64.h"

#define amp_encrypt_printf  aos_printf
#define amp_encrypt_malloc  aos_malloc
#define amp_encrypt_free    aos_free
#define amp_encrypt_memset  memset

// define amp encrypt param
#define AMP_HASH_NONE                     (0)
#define AMP_HASH_SHA256                   (1)
#define AMP_HASH_MD5                      (2)
#define AMP_HASH_SHA256_HASH_SIZE         (32)
#define AMP_HASH_MD5_HASH_SIZE            (16)
#define AMP_ENCRYPT_SIGN_BITNUMB          (2048)

#define AMP_MPI_RW_BUFFER_SIZE   MBEDTLS_MPI_RW_BUFFER_SIZE
// 

// define encrypt error code
typedef enum {
        AMP_ENCRYPT_SUCCESS               = 0,
        AMP_ENCRYPT_INIT_FAIL             = -1,  /*AMP_ENCRYPT init failed.*/
        AMP_ENCRYPT_VERIFY_MD5_FAIL       = -2, /*AMP_ENCRYPT verfiy MD5 failed.*/
        AMP_ENCRYPT_VERIFY_SHA2_FAIL      = -3, /*AMP_ENCRYPT verfiy SH256 failed.*/
        AMP_ENCRYPT_VERIFY_RSA_FAIL       = -4, /*AMP_ENCRYPT verfiy RSA failed.*/
        AMP_ENCRYPT_GET_PUBKEY_FAIL       = -5, /*AMP_ENCRYPT get public key failed.*/
        AMP_ENCRYPT_PUBKEY_ENCRYPT_FAIL   = -6, /*AMP_ENCRYPT get public key failed.*/
        AMP_ENCRYPT_INVALID_PARAMETER     = -20, /*AMP_ENCRYPT INVALID PARAMETER.*/
} AMP_ENCRYPT_ERRNO_E;


#define assert_exit(cond, ret) \
    do { if (!(cond)) { \
        amp_encrypt_printf("  !. assert: failed [line: %d, error: -0x%04X]\n", __LINE__, -ret); \
        goto cleanup; \
    } } while (0)

// amp sha256 context
typedef struct {
    unsigned int  total[2];
    unsigned int  state[8];
    unsigned char buffer[64];
    int is224;
} amp_sha256_context;
// typedef mbedtls_sha256_context haas_sha256_context;

// amp md5 context
typedef struct {
    unsigned int  total[2];
    unsigned int  state[4];
    unsigned char buffer[64];
} amp_md5_context;

// hash context define
typedef struct {
    unsigned char hash_method;         /* hash method: md5, sha256 */
    union {
        amp_md5_context md5_ctx;       /* md5 hash context */
        amp_sha256_context sha256_ctx; /* sh256 hash context */
    };
} amp_hash_ctx_t;


/*hash cal API*/
/*SHA256*/
void amp_sha256_free(amp_sha256_context *ctx);
void amp_sha256_init(amp_sha256_context *ctx);
void amp_sha256_starts(amp_sha256_context *ctx, int is224);
void amp_sha256_update(amp_sha256_context *ctx, const unsigned char *input, unsigned int ilen);
void amp_sha256_finish(amp_sha256_context *ctx, unsigned char output[32]);

/*MD5*/
void amp_md5_free(amp_md5_context *ctx);
void amp_md5_init(amp_md5_context *ctx);
void amp_md5_starts(amp_md5_context *ctx);
void amp_md5_update(amp_md5_context *ctx, const unsigned char *input, unsigned int ilen);
void amp_md5_finish(amp_md5_context *ctx, unsigned char output[16]);

/* base64 decode & encode */
int amp_base64_decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);
int amp_base64_encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);

int amp_get_pubkey(unsigned char * public_key,  char * pub_N, size_t buflen_N,  char * pub_E, size_t buflen_E);

int amp_rsa_pubkey_verify(const unsigned char *pubkey_n,
                          const unsigned char *pubkey_e,
                          unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size,
                          const unsigned char *dig,
                          unsigned int dig_size,
                          const unsigned char *sig,
                          unsigned int sig_size);

int amp_rsa_pubkey_encrypt(unsigned char *dig_src, unsigned char *sig_des, size_t sig_len, size_t *sig_olen, unsigned char *pub_key);



