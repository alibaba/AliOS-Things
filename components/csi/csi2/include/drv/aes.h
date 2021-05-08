/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file       drv/aes.h
 * @brief      Header File for AES Driver
 * @version    V1.0
 * @date       9. Oct 2020
 * @model      aes
 ******************************************************************************/

#ifndef _DRV_AES_H_
#define _DRV_AES_H_

#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----- Encrypt & Decrypt: Config key length -----*/
typedef enum {
    AES_KEY_LEN_BITS_128        = 0,       ///< 128 Data bits
    AES_KEY_LEN_BITS_192,                  ///< 192 Data bits
    AES_KEY_LEN_BITS_256                   ///< 256 Data bits
} csi_aes_key_bits_t;

/**
\brief AES Ctrl Block
*/
typedef struct {
    csi_dev_t               dev;
    void                    *priv;
} csi_aes_t;

/**
  \brief       Initialize AES interface. Initializes the resources needed for the AES interface
  \param[in]   aes    Handle to operate
  \param[in]   idx    Device id
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_init(csi_aes_t *aes, uint32_t idx);

/**
  \brief       De-initialize AES interface. Stops operation and releases the software resources used by the interface
  \param[in]   aes    Dandle to operate
  \return      None
*/
void csi_aes_uninit(csi_aes_t *aes);

/**
  \brief       Set encrypt key
  \param[in]   aes        Handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref csi_aes_key_bits_t
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_set_encrypt_key(csi_aes_t *aes, void *key, csi_aes_key_bits_t key_len);

/**
  \brief       Set decrypt key
  \param[in]   aes        Handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref csi_aes_key_bits_t
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_set_decrypt_key(csi_aes_t *aes, void *key, csi_aes_key_bits_t key_len);

/**
  \brief       AES ecb encrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_ecb_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size);

/**
  \brief       AES ecb decrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_ecb_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size);

/**
  \brief       AES cbc encrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_cbc_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv);

/**
  \brief       AES cbc decrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_cbc_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv);

/**
  \brief       AES cfb1 encrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_cfb1_encrypt(csi_aes_t *aes, void *in, void *out,  uint32_t size, void *iv);

/**
  \brief       AES cfb1 decrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_cfb1_decrypt(csi_aes_t *aes, void *in, void *out,  uint32_t size, void *iv);

/**
  \brief       AES cfb8 encrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_cfb8_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv);

/**
  \brief       AES cfb8 decrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \return      Error code \ref Csi_error_t
*/
csi_error_t csi_aes_cfb8_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv);

/**
  \brief       AES cfb128 decrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \param[out]  num     The number of the 128-bit block we have used
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb128_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);

/**
  \brief       AES cfb128 encrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \param[out]  num     The number of the 128-bit block we have used
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb128_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);

/**
  \brief       AES ofb encrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \param[out]  num     The number of the 128-bit block we have used
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_ofb_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);

/**
  \brief       AES ofb decrypt
  \param[in]   aes     Handle to operate
  \param[in]   in      Pointer to the source data
  \param[out]  out     Pointer to the result data
  \param[in]   size    The source data size
  \param[in]   iv      Init vector
  \param[out]  num     The number of the 128-bit block we have used
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_ofb_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);

/**
  \brief       AES ctr encrypt
  \param[in]   aes              Handle to operate
  \param[in]   in               Pointer to the source data
  \param[out]  out              Pointer to the result data
  \param[in]   size             The source data size
  \param[in]   nonce_counter    Pointer to the 128-bit nonce and counter
  \param[in]   stream_block     Pointer to the saved stream-block for resuming
  \param[in]   iv               Init vector
  \param[out]  num              The number of the 128-bit block we have used
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_ctr_encrypt(csi_aes_t *aes,
                                void *in,
                                void *out,
                                uint32_t size,
                                uint8_t nonce_counter[16],
                                uint8_t stream_block[16],
                                void *iv,
                                uint32_t *num);

/**
  \brief       AES ctr decrypt
  \param[in]   aes              Handle to operate
  \param[in]   in               Pointer to the source data
  \param[out]  out              Pointer to the result data
  \param[in]   size             The source data size
  \param[in]   nonce_counter    Pointer to the 128-bit nonce and counter
  \param[in]   stream_block     Pointer to the saved stream-block for resuming
  \param[in]   iv               Init vecotr
  \param[out]  num              The number of the 128-bit block we have used
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_ctr_decrypt(csi_aes_t *aes,
                                void *in,
                                void *out,
                                uint32_t size,
                                uint8_t nonce_counter[16],
                                uint8_t stream_block[16],
                                void *iv,
                                uint32_t *num);

/**
  \brief       Enable AES power manage
  \param[in]   aes    Handle to operate
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_aes_enable_pm(csi_aes_t *aes);

/**
  \brief       Disable AES power manage
  \param[in]   aes    Handle to operate
  \return      None
*/
void csi_aes_disable_pm(csi_aes_t *aes);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_AES_H_ */
