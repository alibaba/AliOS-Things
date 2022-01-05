/**
 * \file
 * \brief CryptoAuthLib Basic API methods for AES CBC mode.
 *
 * The AES command supports 128-bit AES encryption or decryption of small
 * messages or data packets in ECB mode. Also can perform GFM (Galois Field
 * Multiply) calculation in support of AES-GCM.
 *
 * \note List of devices that support this command - ATECC608A. Refer to device
 *       datasheet for full details.
 *
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */
#include "atca_basic.h"
#ifdef _WIN32
#include <stdlib.h>
#endif

/** \brief Initialize context for AES CBC operation.
 *
 * \param[in] ctx        AES CBC context to be initialized
 * \param[in] key_id     Key location. Can either be a slot number or
 *                       ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block  Index of the 16-byte block to use within the key
 *                       location for the actual key.
 * \param[in] iv         Initialization vector (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbc_init(atca_aes_cbc_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv)
{
    if (ctx == NULL || iv == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    memset(ctx, 0, sizeof(*ctx));
    ctx->key_id = key_id;
    ctx->key_block = key_block;
    memcpy(ctx->ciphertext, iv, sizeof(ctx->ciphertext));

    return ATCA_SUCCESS;
}

/** \brief Encrypt a block of data using CBC mode and a key within the
 *         ATECC608A. atcab_aes_cbc_init() should be called before the
 *         first use of this function.
 *
 * \param[in]  ctx         AES CBC context.
 * \param[in]  plaintext   Plaintext to be encrypted (16 bytes).
 * \param[out] ciphertext  Encrypted data is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbc_encrypt_block(atca_aes_cbc_ctx_t* ctx, const uint8_t* plaintext, uint8_t* ciphertext)
{
    uint8_t input[AES_DATA_SIZE];
    int i;
    ATCA_STATUS status = ATCA_SUCCESS;

    if (ctx == NULL || plaintext == NULL || ciphertext == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // XOR plaintext with previous block's ciphertext to get input value to block encrypt
    for (i = 0; i < AES_DATA_SIZE; i++)
    {
        input[i] = plaintext[i] ^ ctx->ciphertext[i];
    }

    // Block encrypt of input data
    status = atcab_aes_encrypt(ctx->key_id, ctx->key_block, input, ciphertext);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Save copy of ciphertext for next block operation
    memcpy(ctx->ciphertext, ciphertext, AES_DATA_SIZE);

    return status;
}

/** \brief Decrypt a block of data using CBC mode and a key within the
 *         ATECC608A. atcab_aes_cbc_init() should be called before the
 *         first use of this function.
 *
 * \param[in]  ctx         AES CBC context.
 * \param[in]  ciphertext  Ciphertext to be decrypted (16 bytes).
 * \param[out] plaintext   Decrypted data is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbc_decrypt_block(atca_aes_cbc_ctx_t* ctx, const uint8_t* ciphertext, uint8_t* plaintext)
{
    uint8_t output[AES_DATA_SIZE];
    int i;
    ATCA_STATUS status = ATCA_SUCCESS;

    if (ctx == NULL || ciphertext == NULL || plaintext == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Block decrypt of ciphertext
    status = atcab_aes_decrypt(ctx->key_id, ctx->key_block, ciphertext, output);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // XOR output with previous block's ciphertext to get plaintext
    for (i = 0; i < AES_DATA_SIZE; i++)
    {
        plaintext[i] = output[i] ^ ctx->ciphertext[i];
    }

    // Save copy of ciphertext for next block operation
    memcpy(ctx->ciphertext, ciphertext, AES_DATA_SIZE);

    return status;
}
