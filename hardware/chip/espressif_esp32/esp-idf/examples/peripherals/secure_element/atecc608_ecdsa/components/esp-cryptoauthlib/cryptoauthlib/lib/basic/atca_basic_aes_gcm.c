/**
 * \file
 * \brief CryptoAuthLib Basic API methods for AES GCM mode.
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
#include "atca_basic_aes_gcm.h"
#include "atca_compiler.h"

/** \ingroup atcab_
 * @{
 */

const char* atca_basic_aes_gcm_version = "1.0";

/** \brief Performs running GHASH calculations using the current hash value,
 *         hash subkey, and data received. In case of partial blocks, the last
 *         block is padded with zeros to get the output.
 *
 * \param[in]     h          Subkey to use in GHASH calculations.
 * \param[in]     data       Input data to hash.
 * \param[in]     data_size  Data size in bytes.
 * \param[in,out] y          As input, current hash value. As output, the new
 *                           hash output.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS atcab_aes_ghash(const uint8_t* h, const uint8_t* data, size_t data_size, uint8_t* y)
{
    ATCA_STATUS status;
    uint8_t pad_bytes[AES_DATA_SIZE];
    size_t xor_index;

    if (h == NULL || data == NULL || y == NULL)
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }
    if (data_size == 0)
    {
        return ATCA_SUCCESS;
    }

    while (data_size / AES_DATA_SIZE)
    {
        for (xor_index = 0; xor_index < AES_DATA_SIZE; xor_index++)
        {
            y[xor_index] ^= *data++;
        }

        if (ATCA_SUCCESS != (status = atcab_aes_gfm(h, y, y)))
        {
            RETURN(status, "GHASH GFM (full block) failed");
        }

        data_size -= AES_DATA_SIZE;
    }

    if (data_size)
    {
        memcpy(pad_bytes, data, data_size);
        memset(&pad_bytes[data_size], 0, sizeof(pad_bytes) - data_size);

        for (xor_index = 0; xor_index < AES_DATA_SIZE; xor_index++)
        {
            y[xor_index] ^= pad_bytes[xor_index];
        }

        if (ATCA_SUCCESS != (status = atcab_aes_gfm(h, y, y)))
        {
            RETURN(status, "GHASH GFM (partial block) failed");
        }
    }

    return ATCA_SUCCESS;
}

/** \brief Increments AES GCM counter value.
 *
 * \param[in,out] cb            AES GCM counter block to be incremented
 *                              (16 bytes).
 * \param[in]     counter_size  Counter size in bytes. Should be 4 for GCM.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS atcab_aes_gcm_increment(uint8_t* cb, size_t counter_size)
{
    size_t i;

    if (cb == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Not converting to uint32_t and incrementing as there may be alignment
    // issues with the cb buffer

    // Increment the big-endian counter value
    for (i = 0; i < counter_size; i++)
    {
        // Counter is right-aligned in buffer
        if (++(cb[AES_DATA_SIZE - i - 1]) != 0)
        {
            break;
        }
    }
    if (i >= counter_size)
    {
        // Counter overflowed
        memset(&cb[AES_DATA_SIZE - counter_size], 0, counter_size);
    }

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES GCM operation with an existing IV, which
 *         is common when starting a decrypt operation.
 *
 * \param[in] ctx           AES GCM context to be initialized.
 * \param[in] key_id        Key location. Can either be a slot number or
 *                          ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block     Index of the 16-byte block to use within the key
 *                          location for the actual key.
 * \param[in] iv            Initialization vector.
 * \param[in] iv_size       Size of IV in bytes. Standard is 12 bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_init(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv, size_t iv_size)
{
    ATCA_STATUS status;
    uint8_t ghash_data[AES_DATA_SIZE];
    uint32_t length;

    if (ctx == NULL || iv == NULL || iv_size == 0)
    {
        RETURN(ATCA_BAD_PARAM, "GCM init failed; Either null pointer or 0 iv_size");
    }

    memset(ctx, 0, sizeof(*ctx));

    //Calculate H = CIPHK(0^128)
    if (ATCA_SUCCESS != (status = atcab_aes_encrypt(key_id, key_block, ctx->h, ctx->h)))
    {
        RETURN(status, "GCM - H failed");
    }

    //Calculate J0
    if (iv_size == ATCA_AES_GCM_IV_STD_LENGTH)
    {
        //J0 = IV || 0^31 ||1.
        memcpy(ctx->j0, iv, iv_size);
        ctx->j0[AES_DATA_SIZE - 1] = 0x01;
    }
    else
    {
        //J0=GHASH(H, IV||0^(s+64)||[len(IV)]64)
        if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, iv, iv_size, ctx->j0)))
        {
            RETURN(status, "GCM - J0 (IV) failed");
        }

        memset(ghash_data, 0, AES_DATA_SIZE);
        length = ATCA_UINT32_HOST_TO_BE((uint32_t)(iv_size * 8));
        memcpy(&ghash_data[12], &length, sizeof(length));
        if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, ghash_data, sizeof(ghash_data), ctx->j0)))
        {
            RETURN(status, "GCM - J0 (IV Size) failed");
        }
    }

    ctx->key_id = key_id;
    ctx->key_block = key_block;
    memcpy(ctx->cb, ctx->j0, AES_DATA_SIZE);

    if (ATCA_SUCCESS != (status = atcab_aes_gcm_increment(ctx->cb, 4)))
    {
        RETURN(status, "GCM CTR increment failed");
    }

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES GCM operation with a IV composed of a
 *         random and optional fixed(free) field, which is common when
 *         starting an encrypt operation.
 *
 * \param[in]  ctx              AES CTR context to be initialized.
 * \param[in]  key_id           Key location. Can either be a slot number or
 *                              ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block        Index of the 16-byte block to use within the
 *                              key location for the actual key.
 * \param[in]  rand_size        Size of the random field in bytes. Minimum and
 *                              recommended size is 12 bytes. Max is 32 bytes.
 * \param[in]  free_field       Fixed data to include in the IV after the
 *                              random field. Can be NULL if not used.
 * \param[in]  free_field_size  Size of the free field in bytes.
 * \param[out] iv               Initialization vector is returned here. Its
 *                              size will be rand_size and free_field_size
 *                              combined.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_init_rand(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, size_t rand_size,
                                    const uint8_t* free_field, size_t free_field_size, uint8_t* iv)
{
    ATCA_STATUS status;
    uint8_t random[RANDOM_NUM_SIZE];

    if (ctx == NULL || iv == NULL || (free_field_size > 0 && free_field == NULL))
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }
    // 800-38D 8.2.2 specifies a minimum rand_field size of 12 bytes (96 bits)
    if (rand_size < 12 || rand_size > RANDOM_NUM_SIZE)
    {
        RETURN(ATCA_BAD_PARAM, "Bad rand_size");
    }

    if (ATCA_SUCCESS != (status = atcab_random(random)))
    {
        RETURN(status, "GCM init rand - Random Generation failed");
    }
    memcpy(iv, random, rand_size);
    memcpy(&iv[rand_size], free_field, free_field_size);

    if (ATCA_SUCCESS != (status = atcab_aes_gcm_init(ctx, key_id, key_block, iv, rand_size + free_field_size)))
    {
        RETURN(status, "GCM init failed");
    }

    return ATCA_SUCCESS;
}

/** \brief Process Additional Authenticated Data (AAD) using GCM mode and a
 *         key within the ATECC608A device.
 *
 * This can be called multiple times. atcab_aes_gcm_init() or
 * atcab_aes_gcm_init_rand() should be called before the first use of this
 * function. When there is AAD to include, this should be called before
 * atcab_aes_gcm_encrypt_update() or atcab_aes_gcm_decrypt_update().
 *
 * \param[in] ctx       AES GCM context
 * \param[in] aad       Additional authenticated data to be added
 * \param[in] aad_size  Size of aad in bytes
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_aad_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* aad, uint32_t aad_size)
{
    ATCA_STATUS status;
    uint32_t block_count;
    uint32_t rem_size;
    uint32_t copy_size;

    if (ctx == NULL || (aad_size > 0 && aad == NULL))
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }

    if (aad_size == 0)
    {
        return ATCA_SUCCESS;
    }

    rem_size = AES_DATA_SIZE - (uint32_t)ctx->partial_aad_size;
    copy_size = aad_size > rem_size ? rem_size : aad_size;

    // Copy data into current block
    memcpy(&ctx->partial_aad[ctx->partial_aad_size], aad, copy_size);

    if (ctx->partial_aad_size + aad_size < AES_DATA_SIZE)
    {
        // Not enough data to finish off the current block
        ctx->partial_aad_size += aad_size;
        return ATCA_SUCCESS;
    }

    // Process the current block
    if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, ctx->partial_aad, AES_DATA_SIZE, ctx->y)))
    {
        RETURN(status, "GCM - S (AAD) failed");
    }

    // Process any additional blocks
    aad_size -= copy_size; // Adjust to the remaining aad bytes
    block_count = aad_size / AES_DATA_SIZE;
    if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, &aad[copy_size], block_count * AES_DATA_SIZE, ctx->y)))
    {
        RETURN(status, "GCM - S (AAD) failed");
    }

    // Save any remaining data
    ctx->aad_size += (block_count + 1) * AES_DATA_SIZE;
    ctx->partial_aad_size = aad_size % AES_DATA_SIZE;
    memcpy(ctx->partial_aad, &aad[copy_size + block_count * AES_DATA_SIZE], ctx->partial_aad_size);

    return ATCA_SUCCESS;
}

/** \brief Process data using GCM mode and a key within the ATECC608A device.
 *         atcab_aes_gcm_init() or atcab_aes_gcm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx         AES GCM context structure.
 * \param[in]  input       Data to be processed.
 * \param[in]  input_size  Size of input in bytes.
 * \param[out] output      Output data is returned here.
 * \param[in]  is_encrypt  Encrypt operation if true, otherwise decrypt.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS atcab_aes_gcm_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* input, uint32_t input_size,
                                        uint8_t* output, bool is_encrypt)
{
    ATCA_STATUS status;
    uint32_t data_idx;
    uint32_t i;

    if (ctx == NULL || (input_size > 0 && (input == NULL || output == NULL)))
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }

    if (ctx->partial_aad_size > 0)
    {
        // We have a partial block of AAD that needs to be added
        if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, ctx->partial_aad, ctx->partial_aad_size, ctx->y)))
        {
            RETURN(status, "GCM - S (AAD partial) failed");
        }
        ctx->aad_size += ctx->partial_aad_size;
        ctx->partial_aad_size = 0;
    }

    if (input_size == 0)
    {
        // Nothing to do
        return ATCA_SUCCESS;
    }

    data_idx = 0;
    while (data_idx < input_size)
    {
        if (ctx->data_size % AES_DATA_SIZE == 0)
        {
            // Need to calculate next encrypted counter block
            if (ATCA_SUCCESS != (status = atcab_aes_encrypt(ctx->key_id, ctx->key_block, ctx->cb, ctx->enc_cb)))
            {
                RETURN(status, "AES GCM CB encrypt failed");
            }

            // Increment counter
            if (ATCA_SUCCESS != (status = atcab_aes_gcm_increment(ctx->cb, 4)))
            {
                RETURN(status, "AES GCM counter increment failed");
            }
        }

        // Process data with current encrypted counter block
        for (i = ctx->data_size % AES_DATA_SIZE; i < AES_DATA_SIZE && data_idx < input_size; i++, data_idx++)
        {
            output[data_idx] = input[data_idx] ^ ctx->enc_cb[i];
            // Save the current ciphertext block depending on whether this is an encrypt or decrypt operation
            ctx->ciphertext_block[i] = is_encrypt ? output[data_idx] : input[data_idx];
            ctx->data_size += 1;
        }

        if (ctx->data_size % AES_DATA_SIZE == 0)
        {
            // Calculate running hash with completed block
            if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, ctx->ciphertext_block, AES_DATA_SIZE, ctx->y)))
            {
                RETURN(status, "GCM - S (data) failed");
            }
        }
    }

    return ATCA_SUCCESS;
}

/** \brief Encrypt data using GCM mode and a key within the ATECC608A device.
 *         atcab_aes_gcm_init() or atcab_aes_gcm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx             AES GCM context structure.
 * \param[in]  plaintext       Plaintext to be encrypted (16 bytes).
 * \param[in]  plaintext_size  Size of plaintext in bytes.
 * \param[out] ciphertext      Encrypted data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_encrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext)
{
    return atcab_aes_gcm_update(ctx, plaintext, plaintext_size, ciphertext, true);
}

/** \brief Complete GCM operation to generate the authentication tag.
 *
 * It calculates output block (S) by including AAD size and data size, then
 * calculates the tag. This should be called last in a encrypt/decrypt
 * operation.
 *
 * \param[in]  ctx          AES GCM context structure.
 * \param[out] tag          Authentication tag is returned here.
 * \param[in]  tag_size     Required size for the tag. Must be 12 to 16 bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS atcab_aes_gcm_calc_auth_tag(atca_aes_gcm_ctx_t* ctx, uint8_t* tag, size_t tag_size)
{
    ATCA_STATUS status;
    size_t xor_index;
    uint8_t temp_data[AES_DATA_SIZE];
    uint64_t length;

    if (ctx == NULL || tag == NULL)
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }
    // 800-38D 5.2.1.2 specifies these tags sizes
    if (tag_size < 12 || tag_size > 16)
    {
        RETURN(ATCA_BAD_PARAM, "Invalid tag size");
    }

    memset(temp_data, 0, AES_DATA_SIZE);
    length = ATCA_UINT64_HOST_TO_BE(((uint64_t)ctx->aad_size) * 8);
    memcpy(&temp_data[0], &length, sizeof(length));
    length = ATCA_UINT64_HOST_TO_BE(((uint64_t)ctx->data_size) * 8);
    memcpy(&temp_data[8], &length, sizeof(length));

    //S = GHASH(H, [len(A)]64 || [len(C)]64))
    if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, temp_data, AES_DATA_SIZE, ctx->y)))
    {
        RETURN(status, "GCM - S (lengths) failed");
    }

    //T = GCTR(J0, S)
    if (ATCA_SUCCESS != (status = atcab_aes_encrypt(ctx->key_id, ctx->key_block, ctx->j0, temp_data)))
    {
        RETURN(status, "Tag GCTR Encryption failed");
    }

    for (xor_index = 0; xor_index < tag_size; xor_index++)
    {
        tag[xor_index] = temp_data[xor_index] ^ ctx->y[xor_index];
    }

    return ATCA_SUCCESS;
}

/** \brief Complete a GCM encrypt operation returning the authentication tag.
 *
 * \param[in]  ctx       AES GCM context structure.
 * \param[out] tag       Authentication tag is returned here.
 * \param[in]  tag_size  Tag size in bytes (12 to 16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_encrypt_finish(atca_aes_gcm_ctx_t* ctx, uint8_t* tag, size_t tag_size)
{
    ATCA_STATUS status;

    if (ctx == NULL)
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }

    if (ctx->partial_aad_size > 0)
    {
        // Incomplete AAD with no encrypted data, need to complete AAD hash
        if (ATCA_SUCCESS != (status = atcab_aes_gcm_update(ctx, NULL, 0, NULL, true)))
        {
            RETURN(status, "GCM - S (AAD) failed");
        }
    }

    // Update hash with any partial block of ciphertext
    //S = GHASH(H, C || 0^u)
    if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, ctx->ciphertext_block, ctx->data_size % AES_DATA_SIZE, ctx->y)))
    {
        RETURN(status, "GCM - S (C - encrypt update) failed");
    }

    if (ATCA_SUCCESS != (status = atcab_aes_gcm_calc_auth_tag(ctx, tag, tag_size)))
    {
        RETURN(status, "GCM encrypt tag calculation failed");
    }

    return ATCA_SUCCESS;
}

/** \brief Decrypt data using GCM mode and a key within the ATECC608A device.
 *         atcab_aes_gcm_init() or atcab_aes_gcm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx              AES GCM context structure.
 * \param[in]  ciphertext       Ciphertext to be decrypted.
 * \param[in]  ciphertext_size  Size of ciphertext in bytes.
 * \param[out] plaintext        Decrypted data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_decrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext)
{
    return atcab_aes_gcm_update(ctx, ciphertext, ciphertext_size, plaintext, false);
}

/** \brief Complete a GCM decrypt operation verifying the authentication tag.
 *
 * \param[in]  ctx          AES GCM context structure.
 * \param[in]  tag          Expected authentication tag.
 * \param[in]  tag_size     Size of tag in bytes (12 to 16 bytes).
 * \param[out] is_verified  Returns whether or not the tag verified.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gcm_decrypt_finish(atca_aes_gcm_ctx_t* ctx, const uint8_t* tag, size_t tag_size, bool* is_verified)
{
    ATCA_STATUS status;
    uint8_t calc_tag[AES_DATA_SIZE];

    if (ctx == NULL || is_verified == NULL)
    {
        RETURN(ATCA_BAD_PARAM, "Null pointer");
    }
    *is_verified = false;

    if (ctx->partial_aad_size > 0)
    {
        // Incomplete AAD with no decrypted data, need to complete AAD hash
        if (ATCA_SUCCESS != (status = atcab_aes_gcm_update(ctx, NULL, 0, NULL, false)))
        {
            RETURN(status, "GCM - S (AAD) failed");
        }
    }

    // Update hash with any partial block of ciphertext
    //S = GHASH(H, C || 0^u)
    if (ATCA_SUCCESS != (status = atcab_aes_ghash(ctx->h, ctx->ciphertext_block, ctx->data_size % AES_DATA_SIZE, ctx->y)))
    {
        RETURN(status, "GCM - S (C - encrypt update) failed");
    }

    if (ATCA_SUCCESS != (status = atcab_aes_gcm_calc_auth_tag(ctx, calc_tag, tag_size)))
    {
        RETURN(status, "Tag calculation failed");
    }

    *is_verified = (memcmp(calc_tag, tag, tag_size) == 0);

    return ATCA_SUCCESS;
}

/** @} */
