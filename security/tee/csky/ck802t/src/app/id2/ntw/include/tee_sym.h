/*
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_SYM_H_
#define _TEE_SYM_H_

#include "tee_types.h"

/* BLOCK MODE */
#define ID2_ECB           0x00
#define ID2_CBC           0x01
#define ID2_CTR           0x02

#define No_PADDING        0X00
#define ID2_PKCS5         0x02
#define ID2_PKCS7         0x03

#define ID2_AES           0x01
#define ID2_DES           0x02
#define ID2_3DES          0x03

/* IS ENC */
#define ID2_DECRYPT       0x00
#define ID2_ENCRYPT       0x01

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

int tee_sym_cipher(uint8_t ID, cipher_param_t *cipher_param,
                   const uint8_t *iv, uint32_t iv_len,
                   const uint8_t *in, uint32_t in_len,
                   uint8_t *out, uint32_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* _TEE_SYM_H_ */
