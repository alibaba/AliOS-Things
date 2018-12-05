#ifndef _OTA_HASH_H_
#define _OTA_HASH_H_
#include "ota_rsa_param.h"
#include "ota_md5.h"
#include "ota_sha256.h"

#define ota_md5_init mbedtls_md5_init
#define ota_md5_starts mbedtls_md5_starts
#define ota_md5_update mbedtls_md5_update
#define ota_md5_finish mbedtls_md5_finish
#define ota_md5_free mbedtls_md5_free

#define ota_sha256_init mbedtls_sha256_init
#define ota_sha256_starts mbedtls_sha256_starts
#define ota_sha256_update mbedtls_sha256_update
#define ota_sha256_finish mbedtls_sha256_finish
#define ota_sha256_free mbedtls_sha256_free

typedef struct {
    unsigned int magic;
    unsigned int status;
    OTA_HASH_E type;
    union {
        unsigned char sym_ctx[1];
        ota_md5_context md5_ctx;
        ota_sha256_context sha256_ctx;
    };
} ota_hash_ctx_t;

enum {
    CRYPTO_STATUS_CLEAN        = 0,
    CRYPTO_STATUS_INITIALIZED  = 1,
    CRYPTO_STATUS_PROCESSING   = 2,
    CRYPTO_STATUS_FINISHED     = 3,
};

#ifndef NULL
#define NULL        (0)
#endif

typedef struct ota_md_info_t ota_hash_info_t;

#define INIT_CTX_MAGIC(m)         (m = 0x12345678)
#define IS_VALID_CTX_MAGIC(m)     (0x12345678 == m)
#define CLEAN_CTX_MAGIC(m)        (m = 0x0)

unsigned char ota_hash_get_size(const struct ota_md_info_t *md_info);
OTA_VERIFY_E ota_hash_init(OTA_HASH_E type, void *context);
OTA_VERIFY_E ota_hash_update(const unsigned char *src, unsigned int size, void *context);
OTA_VERIFY_E ota_hash_final(unsigned char *dgst, void *context);
OTA_VERIFY_E ota_hash_get_ctx_size(OTA_HASH_E type, unsigned int *size);
OTA_VERIFY_E ota_hash_digest(OTA_HASH_E type, const unsigned char *src, unsigned int size, unsigned char *dgst);
#endif
