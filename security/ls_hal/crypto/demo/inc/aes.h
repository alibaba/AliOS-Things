/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __AES_H__
#define __AES_H__

#include "ls_osa.h"

#define HAL_AES_ENCRYPT     1
#define HAL_AES_DECRYPT     0

#ifdef __cplusplus
extern "C" {
#endif

#define AES_IV_SIZE             16
#define AES_BLOCK_SIZE          16

/**
 * \brief          AES context structure
 *
 * \note           buf is able to hold 32 extra bytes, which can be used:
 *                 - for alignment purposes if VIA padlock is used, and/or
 *                 - to simplify key expansion in the 256-bit case by
 *                 generating an extra round key
 */
typedef struct {
    int nr;                     /*!<  number of rounds  */
    uint32_t *rk;               /*!<  AES round keys    */
    uint32_t buf[68];           /*!<  unaligned data    */
} impl_aes_context;

typedef enum _impl_aes_type_t {
    HAL_AES_ECB         = 0,
    HAL_AES_CBC         = 1,
    HAL_AES_CTR         = 2,
    HAL_AES_CFB         = 3,
} impl_aes_type_t;

typedef struct {
    uint32_t             mode;
    impl_aes_type_t       type;
    uint8_t              iv[AES_IV_SIZE];
    size_t               offset;
    uint8_t              stream_block[AES_BLOCK_SIZE];
    impl_aes_context      ctx;
} impl_aes_ctx_t;

/**
 * \brief          Initialize AES context
 *
 * \param ctx      AES context to be initialized
 */
void impl_aes_init(impl_aes_context *ctx);

/**
 * \brief          Clear AES context
 *
 * \param ctx      AES context to be cleared
 */
void impl_aes_free(impl_aes_context *ctx);

/**
 * \brief          AES key schedule (encryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      encryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or ALI_ALGO_ERR_AES_INVALID_KEY_LENGTH
 */
int impl_aes_setkey_enc(impl_aes_context *ctx, const unsigned char *key,
                      unsigned int keybits);

/**
 * \brief          AES key schedule (decryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      decryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or ALI_ALGO_ERR_AES_INVALID_KEY_LENGTH
 */
int impl_aes_setkey_dec(impl_aes_context *ctx, const unsigned char *key,
                      unsigned int keybits);

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     ALI_ALGO_AES_ENCRYPT or ALI_ALGO_AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int impl_aes_crypt_ecb(impl_aes_context *ctx,
                     int mode,
                     const unsigned char input[16],
                     unsigned char output[16]);

/**
 * \brief          AES-CBC buffer encryption/decryption
 *                 Length should be a multiple of the block
 *                 size (16 bytes)
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     ALI_ALGO_AES_ENCRYPT or ALI_ALGO_AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or ALI_ALGO_ERR_AES_INVALID_INPUT_LENGTH
 */
int impl_aes_crypt_cbc(impl_aes_context *ctx,
                     int mode,
                     size_t length,
                     unsigned char iv[16],
                     const unsigned char *input,
                     unsigned char *output );

/**
 * \brief          AES-CFB128 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * ali_algo_aes_setkey_enc() for both ALI_ALGO_AES_ENCRYPT and ALI_ALGO_AES_DECRYPT.
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     ALI_ALGO_AES_ENCRYPT or ALI_ALGO_AES_DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int impl_aes_crypt_cfb128(impl_aes_context *ctx,
                        int mode,
                        size_t length,
                        size_t *iv_off,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output);

/**
 * \brief          AES-CFB8 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * ali_algo_aes_setkey_enc() for both ALI_ALGO_AES_ENCRYPT and ALI_ALGO_AES_DECRYPT.
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     ALI_ALGO_AES_ENCRYPT or ALI_ALGO_AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int impl_aes_crypt_cfb8(impl_aes_context *ctx,
                      int mode,
                      size_t length,
                      unsigned char iv[16],
                      const unsigned char *input,
                      unsigned char *output );

/**
 * \brief               AES-CTR buffer encryption/decryption
 *
 * Warning: You have to keep the maximum use of your counter in mind!
 *
 * Note: Due to the nature of CTR you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * ali_algo_aes_setkey_enc() for both ALI_ALGO_AES_ENCRYPT and ALI_ALGO_AES_DECRYPT.
 *
 * \param ctx           AES context
 * \param length        The length of the data
 * \param nc_off        The offset in the current stream_block (for resuming
 *                      within current cipher stream). The offset pointer to
 *                      should be 0 at the start of a stream.
 * \param nonce_counter The 128-bit nonce and counter.
 * \param stream_block  The saved stream-block for resuming. Is overwritten
 *                      by the function.
 * \param input         The input data stream
 * \param output        The output data stream
 *
 * \return         0 if successful
 */
int impl_aes_crypt_ctr(impl_aes_context *ctx,
                     size_t length,
                     size_t *nc_off,
                     unsigned char nonce_counter[16],
                     unsigned char stream_block[16],
                     const unsigned char *input,
                     unsigned char *output);

/**
 * \brief           Internal AES block encryption function
 *                  (Only exposed to allow overriding it,
 *                  see ALI_ALGO_AES_ENCRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Plaintext block
 * \param output    Output (ciphertext) block
 */
void impl_aes_encrypt(impl_aes_context *ctx,
                    const unsigned char input[16],
                    unsigned char output[16]);

/**
 * \brief           Internal AES block decryption function
 *                  (Only exposed to allow overriding it,
 *                  see ALI_ALGO_AES_DECRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Ciphertext block
 * \param output    Output (plaintext) block
 */
void impl_aes_decrypt(impl_aes_context *ctx,
                    const unsigned char input[16],
                    unsigned char output[16]);

#ifdef __cplusplus
}
#endif

#endif /* __AES_H__ */
