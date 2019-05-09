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

#define OTA_HASH_NONE                          (0)
#define OTA_SHA256                             (1)
#define OTA_MD5                                (2)

#define OTA_SIGN_BITNUMB                       (2048)

#define OTA_SIGN_OFF                           (0)
#define OTA_SIGN_ON                            (1)

#define OTA_CRYPTO_RET_SUCCESS                 (0)
#define OTA_CRYPTO_ERR                         (-0xA0)
#define OTA_CRYPTO_PARAM_NULL                  (OTA_CRYPTO_ERR - 0x01)
#define OTA_CRYPTO_HASH_CONTEXT_NULL           (OTA_CRYPTO_ERR - 0x02)
#define OTA_CRYPTO_INVALID_ARG                 (OTA_CRYPTO_ERR - 0x03)
#define OTA_CRYPTO_INVALID_HASH_TYPE           (OTA_CRYPTO_ERR - 0x04)
#define OTA_CRYPTO_LENGTH_ERRO                 (OTA_CRYPTO_ERR - 0x05)
#define OTA_CRYPTO_MALLOC_FAIL                 (OTA_CRYPTO_ERR - 0x06)
#define OTA_CRYPTO_VERIFY_FAIL                 (OTA_CRYPTO_ERR - 0x07)
#define OTA_CRYPTO_HASH_CTX_SET_ERR            (OTA_CRYPTO_ERR - 0x08)
#define OTA_CRYPTO_HASH_CTX_GET_ERR            (OTA_CRYPTO_ERR - 0x09)
#define OTA_CRYPTO_HASH_CHECK_MD5_FAIL         (OTA_CRYPTO_ERR - 0x0A)
#define OTA_CRYPTO_HASH_CHECK_SHA256_FAIL      (OTA_CRYPTO_ERR - 0x0B)
#define OTA_CRYPTO_HASH_INIT_ERR               (OTA_CRYPTO_ERR - 0x0C)
#define OTA_CRYPTO_HASH_UPDATE_ERR             (OTA_CRYPTO_ERR - 0x0D)
#define OTA_CRYPTO_HASH_FINAL_ERR              (OTA_CRYPTO_ERR - 0x0E)
#define OTA_CRYPTO_READ_FLASH_FAIL             (OTA_CRYPTO_ERR - 0x10)
#define OTA_CRYPTO_CALCUL_HASH_FAIL            (OTA_CRYPTO_ERR - 0x11)
#define OTA_CRYPTO_MAKE_DIG_HASH_FAIL          (OTA_CRYPTO_ERR - 0x12)

typedef struct {
    char sign_enable;
    unsigned char sign_value[256];
}ota_sign_t;

typedef struct
{
    unsigned char   hash_method;
    char            hash_value[65];
} ota_hash_t;

typedef struct {
    unsigned char hash_method;
    union {
        ota_md5_context md5_ctx;
        ota_sha256_context sha256_ctx;
    };
} ota_hash_ctx_t;

int  ota_get_break_point(void);
void ota_free_global_hash_ctx(void);
int  ota_set_break_point(int offset);
ota_hash_ctx_t *ota_get_hash_ctx(void);
int ota_check_image(unsigned int size);
int  ota_set_cur_hash_value(char *value);
int  ota_get_last_hash_value(char *value);
int  ota_hash_init(ota_hash_ctx_t *context);
void ota_free_hash_ctx(ota_hash_ctx_t *tmp_ctx);
int  ota_set_cur_hash_ctx(ota_hash_ctx_t *hash_ctx);
int  ota_get_last_hash_ctx(ota_hash_ctx_t *hash_ctx);
int  ota_make_global_hash_ctx(unsigned char hash_method);
int  ota_check_hash(unsigned char hash_type, char* hash);
void ota_save_state(int breakpoint, ota_hash_ctx_t *hash_ctx);
ota_hash_ctx_t *ota_malloc_hash_ctx(unsigned char hash_method);
int  ota_hash_final(unsigned char *dgst, ota_hash_ctx_t *context);
int ota_get_image_data(int *off_set, char *buf, unsigned int len);
int  ota_verify_hash_value(unsigned char last_hash, unsigned char cur_hash);
int  ota_hash_update(const unsigned char *src, unsigned int size, ota_hash_ctx_t *context);
int ota_verify_download_rsa_sign(unsigned char *sign_dat, const char *src_hash_dat, unsigned char src_hash_method);
int ota_calculate_image_hash_value(int offset, int image_len, unsigned char hash_method, char *hash_value, int len);
int ota_rsa_pubkey_verify(const unsigned char *pubkey_n, const unsigned char *pubkey_e,unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size, const unsigned char *dig, unsigned int dig_size, const unsigned char *sig, unsigned int sig_size);
#endif /* OTA_VERIFY_H*/

