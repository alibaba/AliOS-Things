/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     drv/rsa.h
 * @brief    Header File for RSA Driver
 * @version  V1.0
 * @date     02. June 2020
 * @model    rsa
 ******************************************************************************/
#ifndef _DRV_RSA_H_
#define _DRV_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <drv/common.h>

/*----- RSA Control Codes: Mode Parameters: Key Bits -----*/
typedef enum {
    RSA_KEY_BITS_192             = 0,  ///< 192 Key bits
    RSA_KEY_BITS_256,                  ///< 256 Key bits
    RSA_KEY_BITS_512,                  ///< 512 Key bits
    RSA_KEY_BITS_1024,                 ///< 1024 Key bits
    RSA_KEY_BITS_2048,                 ///< 2048 Key bits
    RSA_KEY_BITS_3072,                 ///< 3072 Key bits
    RSA_KEY_BITS_4096                  ///< 4096 Key bits
} csi_rsa_key_bits_t;

typedef enum {
    RSA_PADDING_MODE_NO           = 0, ///< RSA NO Padding Mode
    RSA_PADDING_MODE_PKCS1,            ///< RSA PKCS1 Padding Mode
    RSA_PADDING_MODE_PKCS1_OAEP,       ///< RSA PKCS1 OAEP Padding Mode
    RSA_PADDING_MODE_SSLV23,           ///< RSA SSLV23 Padding Mode
    RSA_PADDING_MODE_X931,             ///< RSA X931 Padding Mode
    RSA_PADDING_MODE_PSS               ///< RSA PSS Padding Mode
} csi_rsa_padding_type_t;

typedef enum {
    RSA_HASH_TYPE_MD5            = 0,
    RSA_HASH_TYPE_SHA1,
    RSA_HASH_TYPE_SHA224,
    RSA_HASH_TYPE_SHA256,
    RSA_HASH_TYPE_SHA384,
    RSA_HASH_TYPE_SHA512
} csi_rsa_hash_type_t;

typedef struct {
    void *n;                                ///< Pointer to the public modulus
    void *e;                                ///< Pointer to the public exponent
    void *d;                                ///< Pointer to the private exponent
    csi_rsa_key_bits_t  key_bits;           ///< RSA KEY BITS
    csi_rsa_padding_type_t padding_type;    ///< RSA PADDING TYPE
} csi_rsa_context_t;

/**
\brief RSA State
*/
typedef struct {
    uint8_t busy             : 1;        ///< Calculate busy flag
    uint8_t error            : 1;        ///< Calculate error flag
} csi_rsa_state_t;

typedef struct {
    csi_dev_t           dev;
    void                *cb;
    void                *arg;
    csi_rsa_state_t     state;
    void                *prim;
} csi_rsa_t;

typedef struct {
  uint32_t pout[64];
  uint8_t *pouts;
  uint32_t *pout_size;
  uint32_t u32keywords;
  uint8_t *pdst;
  uint32_t u32padding;
  uint32_t u32dst_words;
  uint32_t u32type;
  uint32_t rsa_state;
}rsa_middle_t;

/****** RSA Event *****/
typedef enum {
    RSA_EVENT_COMPLETE    = 0,   ///< rsa event completed
    RSA_EVENT_VERIFY_SUCCESS,
    RSA_EVENT_VERIFY_FAILED,
    RSA_EVENT_ERROR,             ///< error event
} csi_rsa_event_t;

typedef void (*csi_rsa_callback_t)(csi_rsa_t *rsa, csi_rsa_event_t event, void *arg);   ///< Pointer to \ref csi_rsa_callback_t : RSA Event call back.

/**
  \brief       Initialize RSA Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[in]   rsa  RSA handle to operate.
  \param[in]   idx  Device id
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_init(csi_rsa_t *rsa, uint32_t idx);

/**
  \brief       De-initialize RSA Interface. stops operation and releases the software resources used by the interface
  \param[in]   rsa  RSA handle to operate.
  \return      none
*/
void csi_rsa_uninit(csi_rsa_t *rsa);

/**
  \brief       Attach the callback handler to RSA
  \param[in]   rsa  Operate handle.
  \param[in]   cb    Callback function
  \param[in]   arg   User can define it by himself as callback's param
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_attach_callback(csi_rsa_t *rsa, csi_rsa_callback_t cb, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   rsa  Operate handle.
*/
void csi_rsa_detach_callback(csi_rsa_t *rsa);

/**
  \brief       Generate rsa key pair.
  \param[in]   rsa       RSA handle to operate.
  \param[out]  context   Pointer to the rsa context
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_gen_key(csi_rsa_t *rsa, csi_rsa_context_t *context);

/**
  \brief       Encrypt
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[out]  out       Pointer to the result buffer
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_encrypt(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out);

/**
  \brief       decrypt
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  The result size
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_decrypt(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out, uint32_t *out_size);

/**
  \brief       RSA sign
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type The source data hash type
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_sign(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, csi_rsa_hash_type_t hash_type);

/**
  \brief       RSA verify
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  The signature size
  \param[in]   hash_type The source data hash type
  \return      Verify result
*/
bool csi_rsa_verify(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, uint32_t sig_size, csi_rsa_hash_type_t hash_type);

/**
  \brief       encrypt(async mode)
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[out]  out       Pointer to the result buffer
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_encrypt_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out);

/**
  \brief       decrypt(async mode)
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  The result size
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_decrypt_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out, uint32_t *out_size);

/**
  \brief       RSA sign(async mode)
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type The source data hash type
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_sign_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, csi_rsa_hash_type_t hash_type);

/**
  \brief       RSA verify(async mode)
  \param[in]   rsa       RSA handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  The source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  The signature size
  \param[in]   hash_type The source data hash type
  \return      Verify result
*/
csi_error_t  csi_rsa_verify_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, uint32_t sig_size, csi_rsa_hash_type_t hash_type);

/**
  \brief       Get RSA state.
  \param[in]   rsa      RSA handle to operate.
  \param[out]  state    RSA state \ref csi_rsa_state_t.
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_get_state(csi_rsa_t *rsa, csi_rsa_state_t *state);

/**
  \brief       Get big prime data
  \param[in]   rsa          RSA handle to operate.
  \param[in]   p            Pointer to the prime
  \param[in]   bit_length   Pointer to the prime bit length
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_get_prime(csi_rsa_t *rsa, void *p, uint32_t bit_length);

/**
  \brief       Enable rsa power manage
  \param[in]   rsa  RSA handle to operate.
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_rsa_enable_pm(csi_rsa_t *rsa);

/**
  \brief       Disable rsa power manage
  \param[in]   rsa  RSA handle to operate.
*/
void csi_rsa_disable_pm(csi_rsa_t *rsa);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_RSA_H_ */
