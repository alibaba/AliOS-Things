/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#ifndef _ALI_CRYPTO_TYPES_H_
#define _ALI_CRYPTO_TYPES_H_

#include <stdint.h>
#include <stddef.h> /* for size_t */
#include <stdbool.h>

#if 0
typedef unsigned char       bool;
#endif

#ifndef false
#define false   (0)
#endif

#ifndef true
#define true    (1)
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

#define UNUSED(expr) do { (void)(expr); } while (0)

typedef enum _ali_crypto_result {
    ALI_CRYPTO_ERROR = (int)0xffff0000, /* Generic Error */
    ALI_CRYPTO_NOSUPPORT,               /* Scheme not support */
    ALI_CRYPTO_INVALID_KEY,             /* Invalid Key in asymmetric scheme: RSA/DSA/ECCP/DH etc */
    ALI_CRYPTO_INVALID_TYPE,            /* Invalid aes_type/des_type/authenc_type/hash_type/cbcmac_type/cmac_type */
    ALI_CRYPTO_INVALID_CONTEXT,         /* Invalid context in multi-thread cipher/authenc/mac/hash etc */
    ALI_CRYPTO_INVALID_PADDING,         /* Invalid sym_padding/rsassa_padding/rsaes_padding */
    ALI_CRYPTO_INVALID_AUTHENTICATION,  /* Invalid authentication in AuthEnc(AES-CCM/AES-GCM)/asymmetric verify(RSA/DSA/ECCP DSA) */
    ALI_CRYPTO_INVALID_ARG,             /* Invalid arguments */
    ALI_CRYPTO_INVALID_PACKET,          /* Invalid packet in asymmetric enc/dec(RSA) */
    ALI_CRYPTO_LENGTH_ERR,              /* Invalid Length in arguments */
    ALI_CRYPTO_OUTOFMEM,                /* Memory alloc NULL */
    ALI_CRYPTO_SHORT_BUFFER,            /* Output buffer is too short to store result */
    ALI_CRYPTO_NULL,                    /* NULL pointer in arguments */
    ALI_CRYPTO_ERR_STATE,               /* Bad state in mulit-thread cipher/authenc/mac/hash etc */
    ALI_CRYPTO_SUCCESS = 0,             /* Success */
} ali_crypto_result;

#define AES_BLOCK_SIZE       16   /* don't change this value,since AES only support 16 byte block size */
#define AES_IV_SIZE          16
#define SM4_BLOCK_SIZE       16
#define SM4_IV_SIZE          16
#define DES_BLOCK_SIZE       8
#define DES_IV_SIZE          8

/* hash constants */
#if (CONFIG_HAL_SHA512 || CONFIG_SAL_SHA512)
#define MAX_HASH_SIZE         64  /* longest known is SHA512 */
#else
#define MAX_HASH_SIZE         32  /* longest known is SHA256 or less */
#endif

/* RSA constants */
#define ALI_CRYPTO_RSA_PUBLIC      0
#define ALI_CRYPTO_RSA_PRIVATE     1

#define ALI_CRYPTO_RSA_PKCS_V15    0
#define ALI_CRYPTO_RSA_PKCS_V21    1

#define ALI_CRYPTO_RSA_SIGN        1
#define ALI_CRYPTO_RSA_CRYPT       2

typedef enum _sym_padding_t {
    SYM_NOPAD       = 0,
    SYM_PKCS5_PAD   = 1,
    SYM_ZERO_PAD    = 2,
} sym_padding_t;

typedef enum _aes_type_t {
    AES_ECB         = 0,
    AES_CBC         = 1,
    AES_CTR         = 2,
    AES_CTS         = 3,
    AES_XTS         = 4,
    AES_CFB8        = 6,
    AES_CFB128      = 7,
} aes_type_t;

typedef enum _sm4_type_t {
    SM4_ECB     = 0,
    SM4_CBC     = 1,
} sm4_type_t;

typedef enum _des_type_t {
    DES_ECB     = 0,
    DES_CBC     = 1,
    DES3_ECB    = 2,
    DES3_CBC    = 3,
} des_type_t;

typedef enum _hash_type_t {
    HASH_NONE   = 0,
    SHA1        = 1,
    SHA224      = 2,
    SHA256      = 3,
    SHA384      = 4,
    SHA512      = 5,
    MD5         = 6,
    SM3         = 7,
} hash_type_t;

enum {
    MD5_HASH_SIZE       = 16,
    SHA1_HASH_SIZE      = 20,
    SHA224_HASH_SIZE    = 28,
    SHA256_HASH_SIZE    = 32,
    SM3_HASH_SIZE       = 32,
    SHA384_HASH_SIZE    = 48,
    SHA512_HASH_SIZE    = 64,
};

typedef enum _rsa_key_attr_t {
    RSA_MODULUS          = 0x130,
    RSA_PUBLIC_EXPONENT  = 0x230,
    RSA_PRIVATE_EXPONENT = 0x330,
    RSA_PRIME1           = 0x430,
    RSA_PRIME2           = 0x530,
    RSA_EXPONENT1        = 0x630,
    RSA_EXPONENT2        = 0x730,
    RSA_COEFFICIENT      = 0x830,
} rsa_key_attr_t;

typedef enum _rsa_pad_type_t {
    RSA_NOPAD               = 0,

    /* encrypt */
    RSAES_PKCS1_V1_5        = 10,
    RSAES_PKCS1_OAEP_MGF1   = 11,

    /* sign */
    RSASSA_PKCS1_V1_5       = 20,
    RSASSA_PKCS1_PSS_MGF1   = 21,
} rsa_pad_type_t;

enum {
    CRYPTO_STATUS_CLEAN        = 0,
    CRYPTO_STATUS_INITIALIZED  = 1,
    CRYPTO_STATUS_PROCESSING   = 2,
    CRYPTO_STATUS_FINISHED     = 3,
};


#endif /* _ALI_CRYPTO_TYPES_H_ */
