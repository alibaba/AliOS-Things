/*
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_RSA_H
#define _TEE_RSA_H

#include "tee_types.h"

// KEY TYPE
#define KEY_512  0
#define KEY_1024 1
#define KEY_2048 2

// SIGN TYPE
#define ID2_MD5    0
#define ID2_SHA1   1
#define ID2_SHA256 2

// PADDING TYPE
#define No_PADDING    0X00
#define PKCS1_PADDING 0X01 // default

// KEY ID, 3~10:reserved
#define KEY_ID_0 0 // ID2
#define KEY_ID_1 1 // Alipay Barcode
#define KEY_ID_2 2 // Alipay Passwordless

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
