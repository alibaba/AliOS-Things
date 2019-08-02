/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __IROT_HAL_H__
#define __IROT_HAL_H__

#include "ls_osa.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_ID_ID2                   0  /* Key is used by ID2 */

/* irot hal error code definitions */
#define IROT_SUCCESS                 0  /* The operation was successful */
#define IROT_ERROR_GENERIC          -1  /* The generice error */
#define IROT_ERROR_BAD_PARAMETERS   -2  /* Input parameters are invalid */
#define IROT_ERROR_SHORT_BUFFER     -3  /* The supplied buffer is too short for output */
#define IROT_ERROR_EXCESS_DATA      -4  /* Too much data for the requested operation */
#define IROT_ERROR_OUT_OF_MEMORY    -5  /* Out of memory */
#define IROT_ERROR_COMMUNICATION    -7  /* Communication error */
#define IROT_ERROR_NOT_SUPPORTED    -8  /* The request operation is not supported */
#define IROT_ERROR_NOT_IMPLEMENTED  -9  /* The request operation is not implemented */
#define IROT_ERROR_TIMEOUT          -10 /* Communication timeout */
#define IROT_ERROR_ITEM_NOT_FOUND   -11 /* The item is not exist */

typedef int irot_result_t;

typedef enum {
    CIPHER_TYPE_INVALID         = 0x00,
    CIPHER_TYPE_AES             = 0x01,
    CIPHER_TYPE_3DES            = 0x03,
    CIPHER_TYPE_SM4             = 0x04,
} cipher_t;

typedef enum {
    BLOCK_MODE_ECB              = 0x00,
    BLOCK_MODE_CBC              = 0x01,
    BLOCK_MODE_CTR              = 0x02,
} block_mode_t;

typedef enum {
    SYM_PADDING_NOPADDING       = 0x00,
    SYM_PADDING_PKCS5           = 0x02,
    SYM_PADDING_PKCS7           = 0x03,
} irot_sym_padding_t;

typedef enum {
    ASYM_PADDING_NOPADDING      = 0x00,
    ASYM_PADDING_PKCS1          = 0x01,
} irot_asym_padding_t;

typedef enum {
    MODE_DECRYPT                = 0x00,
    MODE_ENCRYPT                = 0x01,
} crypto_mode_t;

typedef enum {
    ASYM_TYPE_RSA_MD5_PKCS1     = 0x00,
    ASYM_TYPE_RSA_SHA1_PKCS1    = 0x01,
    ASYM_TYPE_RSA_SHA256_PKCS1  = 0x02,
    ASYM_TYPE_RSA_SHA384_PKCS1  = 0x03,
    ASYM_TYPE_RSA_SHA512_PKCS1  = 0x04,
    ASYM_TYPE_SM3_SM2           = 0x05,
    ASYM_TYPE_ECDSA             = 0x06,
} asym_sign_verify_t;

typedef enum {
    HASH_TYPE_SHA1              = 0x00,
    HASH_TYPE_SHA224            = 0x01,
    HASH_TYPE_SHA256            = 0x02,
    HASH_TYPE_SHA384            = 0x03,
    HASH_TYPE_SHA512            = 0x04,
    HASH_TYPE_SM3               = 0x05,
} hash_t;

typedef struct _sym_crypto_param_t {
    block_mode_t block_mode;    ///< block mode
    irot_sym_padding_t padding_type; ///< padding type
    crypto_mode_t mode;                ///< mode(encrypt or decrypt)
} sym_crypto_param_t;

enum {
    KEY_TYPE_3DES            = 0x01,
    KEY_TYPE_AES             = 0x02,
    KEY_TYPE_SM4             = 0x03,

    KEY_TYPE_RSA_PUBLIC      = 0x04,
    KEY_TYPE_RSA_PRIVATE     = 0x05,
    KEY_TYPE_RSA_CRT_PRIVATE = 0x06,
};

enum {
    LENGTH_DES3_2KEY    =   16,
    LENGTH_DES3_3KEY    =   24,
};

enum {
    LENGTH_AES_128      =   16,
    LENGTH_AES_192      =   24,
    LENGTH_AES_256      =   32,
};

enum {
    LENGTH_RSA_1024     =   128,
};

// key object
typedef struct {
    struct {
        uint8_t key_object_type; ///< the key object type
    } head;
    struct {
        uint8_t buf[0x04];      ///< placeholder for key
    } body;
} key_object;

typedef struct {
    struct {
        uint8_t key_object_type;
    } head;
    struct {
        uint8_t *key_value; ///< the key value
        uint32_t key_len;   ///< the key length(bytes)
    } body;
} key_object_sym;

typedef struct {
    struct {
        uint8_t key_object_type;
    } head;
    struct {
        uint8_t *e;         ///< public exponent
        uint32_t e_len;     ///< public exponent length(bytes)
        uint8_t *n;         ///< public modulus
        uint32_t n_len;     ///< public modulus length(bytes)
    } body;
} key_object_rsa_public;

typedef struct {
    struct {
        uint8_t key_object_type;
    } head;
    struct {
        uint8_t *d;         ///< private exponent
        uint32_t d_len;     ///< private exponent length(bytes)
        uint8_t *n;         ///< private modulus
        uint32_t n_len;     ///< private modulus length(bytes)
    } body;
} key_object_rsa_private;

typedef struct {
    struct {
        uint8_t key_object_type;
    } head;
    struct {
        uint8_t *p;         ///< 1st prime factor
        uint8_t *q;         ///< 2st prime factor
        uint8_t *dp;        ///< d % (p - 1)
        uint8_t *dq;        ///< d % (q - 1)
        uint8_t *qinv;      ///< (1/q) % p
        uint32_t len;       ///< the length for the 5 parameters must with the same length(bytes)
    } body;
} key_object_rsa_crt_private;

irot_result_t irot_hal_init(void);

/**
 * @brief get the ID2 value, the length is 12 bytes with hex format.
 *
 * @param[out]   id2:  output buffer, which size must >= 12 bytes.
 * @param[inout] len:  in - the ID2 buffer size; out - the actual length.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_get_id2(uint8_t *id2, uint32_t *len);

/**
 * @brief encrypt or decrypt the data with symmetric algorithms.
 *
 * @param[in]    key_obj: if the key object is not null, then use this parameter as the key;
 *                        otherwise, use the internal key identified by the key id parameter.
 * @param[in]    key_id:  identify the internal key.
 * @param[in]    in:      the input data.
 * @param[in]    in_len:  the input data length.
 * @param[out]   out:     the output buffer.
 * @param[inout] out_len: in - the buffer size; out - the actual length.
 * @param[in]    crypto_param: see sym_crypto_param_t definition.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_sym_crypto(key_object *key_obj, uint8_t key_id,
                                  const uint8_t *iv, uint32_t iv_len,
                                  const uint8_t *in, uint32_t in_len,
                                  uint8_t *out, uint32_t *out_len,
                                  sym_crypto_param_t *crypto_param);

/**
 * @brief compute the signature result with the asymmetric algorithms.
 *
 * @param[in]     key_obj: if the key object is not null, then use this parameter as the key;
 *                         otherwise, use the internal key identified by the key id parameter.
 * @param[in]     key_id:  identify the internal key.
 * @param[in]     in:      the input data.
 * @param[in]     in_len:  the input data length.
 * @param[out]    out:     the output buffer.
 * @param[inout]  out_len: in - the buffer size; out - the actual length.
 * @param[in]     type:    see asym_sign_verify_t definition.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_asym_priv_sign(key_object *key_obj, uint8_t key_id,
                                      const uint8_t *in, uint32_t in_len,
                                      uint8_t *out, uint32_t *out_len,
                                      asym_sign_verify_t type);

/**
 * @brief decrypt the data with the asymmetric algorithms.
 *
 * @param[in]     key_obj: if the key object is not null, then use this parameter as the key;
 *                         otherwise, use the internal key identified by the key id parameter.
 * @param[in]     key_id:  identify the internal key.
 * @param[in]     in:      the input data.
 * @param[in]     in_len:  the input data length.
 * @param[out]    out:     the output buffer.
 * @param[inout]  out_len: in - the buffer size; out - the actual length.
 * @param[in]     padding: see asym_padding_t definition.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_asym_priv_decrypt(key_object *key_obj, uint8_t key_id,
                                         const uint8_t *in, uint32_t in_len,
                                         uint8_t *out, uint32_t *out_len,
                                         irot_asym_padding_t padding);
/**
 * @brief compute the hash with the hash type.
 *
 * @param[in]    in:      the input data.
 * @param[in]    in_len:  the input data length.
 * @param[out]   out:     the output data buffer.
 * @param[inout] out_len: in - the buffer size; out - the actual length.
 * @param[in]    type:    the hash type.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_hash_sum(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len, hash_t type);

/**
 * @brief generate random number with the given length.
 *
 * @param[in] buf: the output buffer.
 * @param[in] len: the output length to be generated with random bytes.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_get_random(uint8_t *buf, uint32_t len);

/*
 * @brief irot hal cleanup.
 *
 * @return @see error code definitions.
 */
irot_result_t irot_hal_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif  /* __IROT_HAL_H__ */

