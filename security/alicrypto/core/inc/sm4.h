/**
* Copyright (C) 2018  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_SM4_H
#define ALI_ALGO_SM4_H

#include "config.h"
#include <stddef.h>
#include <stdint.h>

/* padlock.c rely on these values! */
#define ALI_ALGO_SM4_ENCRYPT     1
#define ALI_ALGO_SM4_DECRYPT     0

#define ALI_ALGO_ERR_SM4_INVALID_KEY_LENGTH                -0x0020  /**< Invalid key length. */

#define ALI_ALGO_ERR_SM4_INVALID_INPUT_LENGTH              -0x0022  /**< Invalid data input length. */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief          SM4 context structure
 *
 * \note           buf is able to hold extra bytes, which can be used:
 *                 - for alignment purposes if VIA padlock is used
 *                 - to simplify key expansion 
 */
typedef struct
{
    int nr;                     /*!<  number of rounds  */
    uint32_t *rk;               /*!<  SM4 round keys    */
    uint32_t buf[68];           /*!<  unaligned data    */
}
ali_algo_sm4_context;

/**
 * \brief          Initialize SM4 context
 *
 * \param ctx      SM4 context to be initialized
 */
void ali_algo_sm4_init( ali_algo_sm4_context *ctx );

/**
 * \brief          Clear SM4 context
 *
 * \param ctx      SM4 context to be cleared
 */
void ali_algo_sm4_free( ali_algo_sm4_context *ctx );

/**
 * \brief          SM4 key schedule 
 *
 * \param ctx      SM4 context to be initialized
 * \param key      encryption or decryption key
 * \param keybits  must be 128
 *
 * \return         0 if successful, or ALI_ALGO_ERR_SM4_INVALID_KEY_LENGTH
 */
int ali_algo_sm4_setkey( ali_algo_sm4_context *ctx, const unsigned char *key,
                    unsigned int keybits );
/**
 * \brief          SM4-ECB block encryption/decryption
 *
 * \param ctx      SM4 context
 * \param mode     ALI_ALGO_SM4_ENCRYPT or ALI_ALGO_SM4_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int ali_algo_sm4_crypt_ecb( ali_algo_sm4_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] );

#if defined(ALI_ALGO_CIPHER_MODE_CBC)
/**
 * \brief          SM4-CBC block encryption/decryption
 *
 * \param ctx      SM4 context
 * \param mode     ALI_ALGO_SM4_ENCRYPT or ALI_ALGO_SM4_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int ali_algo_sm4_crypt_cbc( ali_algo_sm4_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );
#endif /* ALI_ALGO_CIPHER_MODE_CBC */

#ifdef __cplusplus
}
#endif

#endif /* sm4.h */
