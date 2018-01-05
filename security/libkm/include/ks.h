/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _KS_H_
#define _KS_H_

#include <stdint.h>

//the max key name len this KM can support
#define MAX_NAME_LEN 16

#define KM_SUCCESS               0x00000000
#define KM_ERR_GENERIC           0xffff0000
#define KM_ERR_AUTHENTICATION    0xffff0001
#define KM_ERR_BAD_PARAMS        0xffff0002
#define KM_ERR_NOT_SUPPORT       0xffff0003
#define KM_ERR_BAD_FORMAT        0xffff0004
#define KM_ERR_SHORT_BUFFER      0xffff0005
#define KM_ERR_OUT_OF_MEMORY     0xffff0006
#define KM_ERR_ACCESS_CONFLICT   0xffff0007
#define KM_ERR_ITEM_NOT_FOUND    0xffff0008
#define KM_ERR_CORRUPT_KEY       0xffff0009

typedef void * km_op_handle_t;

typedef enum {
    KM_KEY_FORMAT_X509 = 0, /* for public key export*/
    KM_KEY_FORMAT_PKCS8 = 1, /* for asymmetric key pair import*/
    KM_KEY_FORMAT_RAW = 2, /* for symmetric key pair import */
} km_format_t;

/* only support rsa now */
typedef enum {
    KM_RSA,
    KM_ECC,
    KM_AES,
    KM_DES,
    KM_DES3,
    KM_HMAC,
} km_key_type;

typedef enum {
    KM_ECB,
    KM_CBC,
    KM_CTR,
    KM_XTS, //not support yet
    KM_GCM,
} km_block_mode_type;

typedef enum {
    KM_NO_PADDING = 0,
    KM_PKCS1,
    KM_PKCS5,
    KM_PKCS7,
} km_padding_type;

typedef enum {
    KM_MD5 = 0,
    KM_SHA1,
    KM_SHA256,
    KM_DIGEST_NONE,
} km_digest_type;

typedef enum {
    KM_PURPOSE_ENCRYPT,
    KM_PURPOSE_DECRYPT,
    KM_PURPOSE_SIGN,
    KM_PURPOSE_VERIFY,
} km_purpose_type;

typedef struct _rsa_gen_param {
    uint32_t key_size;
    uint64_t exponent;
} rsa_gen_param;

typedef struct _sym_gen_param {
    uint32_t key_size;
} sym_gen_param;

typedef struct _km_sign_param {
    km_padding_type padding_type;
    km_digest_type digest_type;
} km_sign_param;

typedef struct _km_enc_param {
    km_padding_type padding_type;
} km_enc_param;

typedef struct _km_cipher_param {
    km_key_type cipher_type;
    km_purpose_type purpose_type;
    km_block_mode_type block_mode;
    km_padding_type padding_type;
} km_cipher_param;

typedef struct _km_hmac_param {
    km_digest_type hash_type;
} km_hmac_param;

//for import raw format
typedef struct _rsa_key_t {
    uint32_t n_len;
    uint32_t e_len;
    uint32_t d_len;
    uint32_t p_len;
    uint32_t q_len;
    uint32_t dp_len;
    uint32_t dq_len;
    uint32_t iq_len;
    uint8_t *n;
    uint8_t *e;
    uint8_t *d;
    uint8_t *p;
    uint8_t *q;
    uint8_t *dp;
    uint8_t *dq;
    uint8_t *iq;
} rsa_key_t;

typedef struct _sym_key_t {
    uint32_t key_bit;
    uint8_t *key;
} sym_key_t;

typedef struct _km_key_data_t {
    km_key_type type;
    union {
        rsa_key_t rsa_data;
        sym_key_t aes_key;
    };
} km_key_data_t;

uint32_t generate_key(const char *name, const uint32_t name_len,
                     uint32_t key_type, void *arg);
uint32_t import_key(const char *name, const uint32_t name_len, km_format_t format,
                   const uint8_t *key_data, const uint32_t key_data_len);
uint32_t export_key(const char *name, const uint32_t name_len, km_format_t format,
                   uint8_t **export_data, uint32_t *export_data_size);
uint32_t sign(const char *name, const uint32_t name_len, void *sign_params,
             const uint8_t *data, const size_t data_len,
             uint8_t **out, size_t *out_len);
uint32_t verify(const char *name, const uint32_t name_len, void *sign_params,
               const uint8_t *data, const size_t data_len,
               const uint8_t *signature, const size_t signature_len);
uint32_t encrypt(const char *name, const uint32_t name_len, void *enc_params,
                const uint8_t *src, const size_t src_len,
             uint8_t **dest, size_t *dest_len);
uint32_t decrypt(const char *name, const uint32_t name_len, void *enc_params,
                const uint8_t *src, const size_t src_len,
               uint8_t **dest, size_t *dest_len);
uint32_t begin(const char *name, const uint32_t name_len, void *cipher_params,
                    const uint8_t *iv, const uint32_t iv_len,
                    km_op_handle_t *operation_handle);
uint32_t update(km_op_handle_t operation_handle,
                      const uint8_t *src, const size_t src_len,
                      uint8_t *dest, size_t *dest_len);
uint32_t finish(km_op_handle_t operation_handle,
                     const uint8_t *src, const size_t src_len,
                     uint8_t *dest, size_t *dest_len);
uint32_t delete_key(const char *name, const uint32_t name_len);
uint32_t delete_all();

uint32_t ks_init();

#endif /* _KS_H_ */

