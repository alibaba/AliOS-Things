/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_VERIFY_H_
#define OTA_VERIFY_H_
#include "ota_hal_os.h"
#include "stdint.h"

#define AOS_SINGLE_TAG (0xabababab)
#define AOS_KERNEL_TAG (0xcdcdcdcd)
#define AOS_APP_TAG    (0xefefefef)

typedef enum {
    OTA_CRYPTO_ERROR = (int)0xffff0000,
    OTA_CRYPTO_NOSUPPORT,
    OTA_CRYPTO_INVALID_KEY,
    OTA_CRYPTO_INVALID_TYPE,
    OTA_CRYPTO_INVALID_CONTEXT,
    OTA_CRYPTO_INVALID_PADDING,
    OTA_CRYPTO_INVALID_AUTHENTICATION,
    OTA_CRYPTO_INVALID_ARG,
    OTA_CRYPTO_INVALID_PACKET,
    OTA_CRYPTO_LENGTH_ERR,
    OTA_CRYPTO_OUTOFMEM,
    OTA_CRYPTO_SHORT_BUFFER,
    OTA_CRYPTO_NULL,
    OTA_CRYPTO_ERR_STATE,
    OTA_CRYPTO_SUCCESS = 0,
} OTA_VERIFY_E;

typedef enum {
    OTA_SIGN_OFF = 0,
    OTA_SIGN_ON
} OTA_SIGN_E;

typedef struct {
    char sign_enable;
    unsigned char sign_value[256];
}ota_sign_t;

typedef struct
{
    OTA_HASH_E hash_method;
    char       hash_value[65];
} ota_hash_t;

typedef struct
{
    OTA_HASH_E hash_method;
    int         ctx_size;
    void       *ctx_hash;
} ota_hash_param_t;

typedef struct {
    unsigned int magic;
    unsigned int status;
    OTA_HASH_E type;
    union {
        ota_md5_context md5_ctx;
        ota_sha256_context sha256_ctx;
    };
} ota_hash_ctx_t;

typedef enum {
    OTA_CRYPTO_STATUS_CLEAN        = 0,
    OTA_CRYPTO_STATUS_INITIALIZED  = 1,
    OTA_CRYPTO_STATUS_PROCESSING   = 2,
    OTA_CRYPTO_STATUS_FINISHED     = 3,
} OTA_CRYPTO_STATUS_E;

#define INIT_CTX_MAGIC(m)         (m = 0x12345678)
#define IS_VALID_CTX_MAGIC(m)     (0x12345678 == m)
#define CLEAN_CTX_MAGIC(m)        (m = 0x0)

int  ota_hash_init(OTA_HASH_E type, void *context);
int  ota_hash_update(const unsigned char *src, unsigned int size, void *context);
int  ota_hash_final(unsigned char *dgst, void *context);
int  ota_hash_get_ctx_size(OTA_HASH_E type, unsigned int *size);
int  ota_hash_digest(OTA_HASH_E type, const unsigned char *src, unsigned int size, unsigned char *dgst);
void ota_save_state(int breakpoint, ota_hash_param_t *hash_ctx);
void ota_free_hash_ctx(void);
int  ota_get_break_point(void);
int  ota_malloc_hash_ctx(OTA_HASH_E type);
int  ota_check_hash(OTA_HASH_E hash_type, char* hash);
int  ota_set_break_point(int offset);
int  ota_get_last_hash_ctx(ota_hash_param_t *hash_ctx);
int  ota_set_cur_hash_ctx(ota_hash_param_t *hash_ctx);
int  ota_get_last_hash(char *value);
int  ota_set_cur_hash(char *value);
int  ota_verify_hash_value(ota_hash_t last_hash, ota_hash_t cur_hash);
int  ota_get_public_key_bitnumb(void);
int  ota_rsa_verify(const ota_rsa_pubkey_t *pub_key, const unsigned char *dig, unsigned int dig_size,
                      const unsigned char *sig, unsigned int sig_size, ota_rsa_padding_t padding, bool *p_result);
int  ota_verify_download_rsa_sign(unsigned char* sign_dat, const char* src_hash_dat, OTA_HASH_E src_hash_method);
ota_hash_param_t *ota_get_hash_ctx(void);
unsigned char    *ota_get_identity_image_md5_strvalue(void);
int  ota_check_image(unsigned int size);
#endif /* OTA_VERIFY_H*/

