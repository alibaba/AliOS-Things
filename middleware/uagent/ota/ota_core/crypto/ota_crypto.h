/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_CRYPTO_H
#define OTA_CRYPTO_H
#include "stdint.h"
#include "ota_hal_common.h"

#define AOS_SINGLE_TAG                    (0xabababab)
#define AOS_KERNEL_TAG                    (0xcdcdcdcd)
#define AOS_APP_TAG                       (0xefefefef)

#define OTA_HASH_NONE                     (0)
#define OTA_SHA256                        (1)
#define OTA_MD5                           (2)

#define OTA_SIGN_BUF_SIZE                 (256)
#define OTA_HASH_VALUE_SIZE               (65)

#define OTA_SIGN_BITNUMB                  (2048)

#define OTA_SIGN_OFF                      (0)
#define OTA_SIGN_ON                       (1)

#define OTA_CRYPTO_RET_SUCCESS            (0)
#define OTA_CRYPTO_ERR                    (-0x01)
#define OTA_CRYPTO_PARAM_NULL             (OTA_CRYPTO_ERR - 0x01)
#define OTA_CRYPTO_HASH_CONTEXT_NULL      (OTA_CRYPTO_ERR - 0x02)
#define OTA_CRYPTO_INVALID_ARG            (OTA_CRYPTO_ERR - 0x03)
#define OTA_CRYPTO_INVALID_HASH_TYPE      (OTA_CRYPTO_ERR - 0x04)
#define OTA_CRYPTO_LENGTH_ERRO            (OTA_CRYPTO_ERR - 0x05)
#define OTA_CRYPTO_MALLOC_FAIL            (OTA_CRYPTO_ERR - 0x06)
#define OTA_CRYPTO_VERIFY_FAIL            (OTA_CRYPTO_ERR - 0x07)
#define OTA_CRYPTO_HASH_CHECK_MD5_FAIL    (OTA_CRYPTO_ERR - 0x08)
#define OTA_CRYPTO_HASH_CHECK_SHA256_FAIL (OTA_CRYPTO_ERR - 0x09)
#define OTA_CRYPTO_HASH_INIT_ERR          (OTA_CRYPTO_ERR - 0x0A)
#define OTA_CRYPTO_HASH_UPDATE_ERR        (OTA_CRYPTO_ERR - 0x0B)
#define OTA_CRYPTO_HASH_FINAL_ERR         (OTA_CRYPTO_ERR - 0x0C)
#define OTA_CRYPTO_READ_FLASH_FAIL        (OTA_CRYPTO_ERR - 0x0D)
#define OTA_CRYPTO_CALCUL_HASH_FAIL       (OTA_CRYPTO_ERR - 0x0E)
#define OTA_CRYPTO_MAKE_DIG_HASH_FAIL     (OTA_CRYPTO_ERR - 0x0F)

typedef struct {
    char sign_enable;
    unsigned char sign_value[OTA_SIGN_BUF_SIZE];
}ota_sign_t;

typedef struct
{
    unsigned char   hash_method;
    char            hash_value[OTA_HASH_VALUE_SIZE];
} ota_hash_t;

typedef struct {
    unsigned char hash_method;
    union {
        ota_md5_context md5_ctx;
        ota_sha256_context sha256_ctx;
    };
} ota_hash_ctx_t;

int ota_check_image(unsigned int size);
int ota_hash_init(ota_hash_ctx_t *context);
ota_hash_ctx_t *ota_malloc_hash_ctx(unsigned char hash_method);
int ota_hash_final(unsigned char *dgst, ota_hash_ctx_t *context);
int ota_get_image_data(unsigned int *off_set, char *buf, unsigned int len);
int ota_verify_hash_value(unsigned char last_hash, unsigned char cur_hash);
int ota_check_hash(unsigned char hash_type, char *dest_hash, char *src_hash);
int ota_hash_update(const unsigned char *src, unsigned int size, ota_hash_ctx_t *context);
int ota_hex2str(char *dest_buf, const unsigned char *src_buf, unsigned int dest_len, unsigned int src_len);
int ota_verify_download_rsa_sign(unsigned char *sign_dat, const char *src_hash_dat, unsigned char src_hash_method);
int ota_calculate_image_hash_value(unsigned int offset, int image_len, unsigned char hash_method, char *hash_value, int len);
int ota_rsa_pubkey_verify(const unsigned char *pubkey_n, const unsigned char *pubkey_e,unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size, const unsigned char *dig, unsigned int dig_size, const unsigned char *sig, unsigned int sig_size);
#endif /*OTA_CRYPTO_H*/

