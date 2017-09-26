/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _KS_H_
#define _KS_H_

#include <stdint.h>

//the max key name len this KM can support
#define MAX_NAME_LEN 16

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
    KM_HMAC,
} km_key_type;

typedef enum {
    KM_NO_PADDING = 0,
    KM_PKCS1,
} km_padding_type;

typedef enum {
    KM_MD5 = 0,
    KM_SHA1,
    KM_SHA256,
    KM_DIGEST_NONE,
} km_digest_type;

typedef struct _rsa_gen_param {
    uint32_t key_size;
    uint64_t exponent;
} rsa_gen_param;

typedef struct _km_sign_param {
    km_padding_type padding_type;
    km_digest_type digest_type;
} km_sign_param;

typedef struct _km_enc_param {
    km_padding_type padding_type;
} km_enc_param;

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
    uint8_t *dp;
    uint8_t *dq;
    uint8_t *iq;
} rsa_key_t;

typedef struct _km_key_data_t {
    km_key_type type;
    union {
        rsa_key_t rsa_data;
    };
} km_key_data_t;

//0 succrss -1 failed
int32_t generate_key(const char *name, const uint32_t name_len,
                     uint32_t key_type, void *arg);
int32_t import_key(const char *name, const uint32_t name_len, km_format_t format,
                   const uint8_t *key_data, const uint32_t key_data_len);
int32_t export_key(const char *name, const uint32_t name_len, km_format_t format,
                   uint8_t **export_data, uint32_t *export_data_size);
int32_t sign(const char *name, const uint32_t name_len, void *sign_params,
             const uint8_t *data, const uint32_t data_len,
             uint8_t **out, uint32_t *out_len);
int32_t verify(const char *name, const uint32_t name_len, void *sign_params,
               const uint8_t *data, const uint32_t data_len,
               const uint8_t *signature, const uint32_t signature_len);
int32_t encrypt(const char *name, const uint32_t name_len, void *enc_params,
                const uint8_t *src, const uint32_t src_len,
             uint8_t **dest, uint32_t *dest_len);
int32_t decrypt(const char *name, const uint32_t name_len, void *enc_params,
                const uint8_t *src, const uint32_t src_len,
               uint8_t **dest, uint32_t *dest_len);
int32_t delete_key(const char *name, const uint32_t name_len);
int32_t delete_all();

#endif /* _KS_H_ */

