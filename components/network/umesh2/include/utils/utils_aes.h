/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef UTILS_AES_H
#define UTILS_AES_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(UTILS_AES_BUILDIN)
#include "utils_aes_config.h"
#endif

#include <stddef.h>
#include <stdint.h>

typedef void *p_Aes128_t;

typedef enum {
    AES_DECRYPTION = 0,
    AES_ENCRYPTION = 1,
} AES_DIR_t;

/* padlock.c and aesni.c rely on these values! */
#define UTILS_AES_ENCRYPT     1
#define UTILS_AES_DECRYPT     0

#define UTILS_ERR_AES_INVALID_KEY_LENGTH                -0x0020  /**< Invalid key length. */
#define UTILS_ERR_AES_INVALID_INPUT_LENGTH              -0x0022  /**< Invalid data input length. */

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) ) && \
    !defined(inline) && !defined(__cplusplus)
#define inline __inline
#endif

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
}
utils_aes_context;

/**
 * \brief          Initialize AES context
 *
 * \param ctx      AES context to be initialized
 */

void utils_aes_init(utils_aes_context *ctx);

/**
 * \brief          Clear AES context
 *
 * \param ctx      AES context to be cleared
 */
void utils_aes_free(utils_aes_context *ctx);

/**
 * \brief          AES key schedule (encryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      encryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or UTILS_ERR_AES_INVALID_KEY_LENGTH
 */
int utils_aes_setkey_enc(utils_aes_context *ctx, const unsigned char *key,
                         unsigned int keybits);

/**
 * \brief          AES key schedule (decryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      decryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or UTILS_ERR_AES_INVALID_KEY_LENGTH
 */
int utils_aes_setkey_dec(utils_aes_context *ctx, const unsigned char *key,
                         unsigned int keybits);

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     UTILS_AES_ENCRYPT or UTILS_AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int utils_aes_crypt_ecb(utils_aes_context *ctx,
                        int mode,
                        const unsigned char input[16],
                        unsigned char output[16]);

#if defined(UTILS_CIPHER_MODE_CBC)
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
 * \param mode     UTILS_AES_ENCRYPT or UTILS_AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or UTILS_ERR_AES_INVALID_INPUT_LENGTH
 */
int utils_aes_crypt_cbc(utils_aes_context *ctx,
                        int mode,
                        size_t length,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output);
#endif /* UTILS_CIPHER_MODE_CBC */

#if defined(UTILS_CIPHER_MODE_CFB)
/**
 * \brief          AES-CFB128 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * utils_aes_setkey_enc() for both UTILS_AES_ENCRYPT and UTILS_AES_DECRYPT.
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
 * \param mode     UTILS_AES_ENCRYPT or UTILS_AES_DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int utils_aes_crypt_cfb128(utils_aes_context *ctx,
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
 * utils_aes_setkey_enc() for both UTILS_AES_ENCRYPT and UTILS_AES_DECRYPT.
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
 * \param mode     UTILS_AES_ENCRYPT or UTILS_AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int utils_aes_crypt_cfb8(utils_aes_context *ctx,
                         int mode,
                         size_t length,
                         unsigned char iv[16],
                         const unsigned char *input,
                         unsigned char *output);
#endif /*UTILS_CIPHER_MODE_CFB */

#if defined(UTILS_CIPHER_MODE_CTR)
/**
 * \brief               AES-CTR buffer encryption/decryption
 *
 * Warning: You have to keep the maximum use of your counter in mind!
 *
 * Note: Due to the nature of CTR you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * utils_aes_setkey_enc() for both UTILS_AES_ENCRYPT and UTILS_AES_DECRYPT.
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
int utils_aes_crypt_ctr(utils_aes_context *ctx,
                        size_t length,
                        size_t *nc_off,
                        unsigned char nonce_counter[16],
                        unsigned char stream_block[16],
                        const unsigned char *input,
                        unsigned char *output);
#endif /* UTILS_CIPHER_MODE_CTR */

/**
 * \brief           Internal AES block encryption function
 *                  (Only exposed to allow overriding it,
 *                  see UTILS_AES_ENCRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Plaintext block
 * \param output    Output (ciphertext) block
 *
 * \return          0 if successful
 */
int utils_internal_aes_encrypt(utils_aes_context *ctx,
                               const unsigned char input[16],
                               unsigned char output[16]);

/**
 * \brief           Internal AES block decryption function
 *                  (Only exposed to allow overriding it,
 *                  see UTILS_AES_DECRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Ciphertext block
 * \param output    Output (plaintext) block
 *
 * \return          0 if successful
 */
int utils_internal_aes_decrypt(utils_aes_context *ctx,
                               const unsigned char input[16],
                               unsigned char output[16]);

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int utils_aes_self_test(int verbose);

typedef utils_aes_context mbedtls_aes_context;

#if !defined(UTILS_AES_BUILDIN)
void mbedtls_aes_free(mbedtls_aes_context *ctx);
void mbedtls_aes_init(mbedtls_aes_context *ctx);
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits);
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits);
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output);
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output);

#define MBEDTLS_AES_DECRYPT UTILS_AES_DECRYPT
#define MBEDTLS_AES_ENCRYPT UTILS_AES_ENCRYPT
#endif

#define AES_BLOCK_SIZE (16)



p_Aes128_t utils_aes128_init(
            const uint8_t *key,
            const uint8_t *iv,
            AES_DIR_t dir);

int utils_aes128_destroy(p_Aes128_t aes);

int utils_aes128_cbc_decrypt(
            p_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);

int utils_aes128_cfb_decrypt(
            p_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst);

int utils_aes128_cfb_encrypt(
            p_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst);

int utils_aes128_cbc_encrypt(
            p_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);

#ifdef __cplusplus
}
#endif
#endif /* UTILS_AES_H */

