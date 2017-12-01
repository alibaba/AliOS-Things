/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SM_ID2_H_
#define _SM_ID2_H_

#include <stdint.h>

/* KEY TYPE */
#define KEY_512  0
#define KEY_1024 1
#define KEY_2048 2

/* SIGN TYPE */
#define SM_MD5    0
#define SM_SHA1   1
#define SM_SHA256 2

/* PADDING TYPE */
#define SM_No_PADDING    0x00
#define SM_PKCS1_PADDING 0x01 // default
#define SM_PKCS5         0x02
#define SM_PKCS7         0x03

/* BLOCK MODE */
#define SM_ECB           0x00
#define SM_CBC           0x01
#define SM_CTR           0x02

#define SM_AES           0x01
#define SM_DES           0x02
#define SM_3DES          0x03

/* IS ENC */
#define SM_DECRYPT       0x00
#define SM_ENCRYPT       0x01

/* KEY ID, 3~10:reserved */
#define KEY_ID_0  0    // ID2
#define KEY_ID_1  1    // Alipay Barcode
#define KEY_ID_2  2    // Alipay Passwordless
#define KEY_ID_18 18   //3DES ID2 key
#define KEY_ID_33 33   //AES ID2 key

typedef struct _cipher_param_t {
    uint8_t cipher_type;
    uint8_t block_mode;
    uint8_t padding_type;
    uint8_t is_enc;
} cipher_param_t;

#ifdef __cplusplus
extern "C" {
#endif

int tee_get_ID2(uint8_t *id2, uint32_t *len);
int tee_RSA_generate_key(uint8_t ID, uint8_t type);
int tee_RSA_get_pub_key(uint8_t ID, uint8_t *pub, uint32_t *len);
int tee_RSA_sign(uint8_t ID, const uint8_t *in, uint32_t in_len,
                uint8_t *sign, uint32_t *sign_len, uint8_t type);
int tee_RSA_verify(uint8_t ID, const uint8_t *in, uint32_t in_len,
                uint8_t *sign, uint32_t sign_len, uint8_t type);
int tee_RSA_public_encrypt(uint8_t ID, const uint8_t *in, uint32_t in_len,
                uint8_t *out, uint32_t *out_len, uint8_t padding);
int tee_RSA_private_decrypt(uint8_t ID, uint8_t *in, uint32_t in_len,
                uint8_t *out, uint32_t *out_len, uint8_t padding);
int tee_sym_cipher(uint8_t ID, cipher_param_t *cipher_param,
                   const uint8_t *iv, uint32_t iv_len,
                   const uint8_t *in, uint32_t in_len,
                   uint8_t *out, uint32_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* _SM_ID2_H_ */
