/**
 * \file
 * \brief CryptoAuthLib Basic API methods for SHA command.
 *
 * The SHA command Computes a SHA-256 or HMAC/SHA digest for general purpose
 * use by the host system.
 *
 * \note List of devices that support this command - ATSHA204A, ATECC108A,
 *       ATECC508A, and ATECC608A. There are differences in the modes that they
 *       support. Refer to device datasheets for full details.
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
#include "atca_execution.h"

typedef struct
{
    uint32_t total_msg_size;                     //!< Total number of message bytes processed
    uint32_t block_size;                         //!< Number of bytes in current block
    uint8_t  block[ATCA_SHA256_BLOCK_SIZE * 2];  //!< Unprocessed message storage
} hw_sha256_ctx;

/** \brief Executes SHA command, which computes a SHA-256 or HMAC/SHA-256
 *          digest for general purpose use by the host system.
 *
 * Only the Start(0) and Compute(1) modes are available for ATSHA devices.
 *
 * \param[in]    mode           SHA command mode Start(0), Update/Compute(1),
 *                              End(2), Public(3), HMACstart(4), HMACend(5),
 *                              Read_Context(6), or Write_Context(7). Also
 *                              message digest target location for the
 *                              ATECC608A.
 * \param[in]    length         Number of bytes in the message parameter or
 *                              KeySlot for the HMAC key if Mode is
 *                              HMACstart(4) or Public(3).
 * \param[in]    message        Message bytes to be hashed or Write_Context if
 *                              restoring a context on the ATECC608A. Can be
 *                              NULL if not required by the mode.
 * \param[out]   data_out       Data returned by the command (digest or
 *                              context).
 * \param[inout] data_out_size  As input, the size of the data_out buffer. As
 *                              output, the number of bytes returned in
 *                              data_out.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_base(uint8_t mode, uint16_t length, const uint8_t* message, uint8_t* data_out, uint16_t* data_out_size)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t cmd_mode = (mode & SHA_MODE_MASK);

    if (cmd_mode != SHA_MODE_SHA256_PUBLIC && cmd_mode != SHA_MODE_HMAC_START && length > 0 && message == NULL)
    {
        return ATCA_BAD_PARAM; // message data indicated, but nothing provided
    }
    if (data_out != NULL && data_out_size == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        //Build Command
        packet.param1 = mode;
        packet.param2 = length;

        if (cmd_mode != SHA_MODE_SHA256_PUBLIC && cmd_mode != SHA_MODE_HMAC_START)
        {
            memcpy(packet.data, message, length);
        }

        if ((status = atSHA(ca_cmd, &packet, length)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((data_out != NULL) && (packet.data[ATCA_COUNT_IDX] > 4))
        {
            if (packet.data[ATCA_COUNT_IDX] - ATCA_PACKET_OVERHEAD > *data_out_size)
            {
                status = ATCA_SMALL_BUFFER;
                break;
            }
            *data_out_size = packet.data[ATCA_COUNT_IDX] - ATCA_PACKET_OVERHEAD;
            memcpy(data_out, &packet.data[ATCA_RSP_DATA_IDX], *data_out_size);
        }
    }
    while (0);

    return status;
}

/** \brief Executes SHA command to initialize SHA-256 calculation engine
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_start(void)
{
    return atcab_sha_base(SHA_MODE_SHA256_START, 0, NULL, NULL, NULL);
}

/** \brief Executes SHA command to add 64 bytes of message data to the current
 *          context.
 *
 * \param[in] message  64 bytes of message data to add to add to operation.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_update(const uint8_t *message)
{
    return atcab_sha_base(SHA_MODE_SHA256_UPDATE, 64, message, NULL, NULL);
}

/** \brief Executes SHA command to complete SHA-256 or HMAC/SHA-256 operation.
 *
 *  \param[out] digest   Digest from SHA-256 or HMAC/SHA-256 will be returned
 *                       here (32 bytes).
 *  \param[in]  length   Length of any remaining data to include in hash. Max 64
 *                       bytes.
 *  \param[in]  message  Remaining data to include in hash. NULL if length is 0.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_end(uint8_t *digest, uint16_t length, const uint8_t *message)
{
    uint16_t digest_size = 32;

    return atcab_sha_base(SHA_MODE_SHA256_END, length, message, digest, &digest_size);
}

/** \brief Executes SHA command to read the SHA-256 context back. Only for
 *          ATECC608A with SHA-256 contexts. HMAC not supported.
 *
 *  \param[out]   context       Context data is returned here.
 *  \param[inout] context_size  As input, the size of the context buffer in
 *                              bytes. As output, the size of the returned
 *                              context data.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_read_context(uint8_t* context, uint16_t* context_size)
{
    return atcab_sha_base(SHA_MODE_READ_CONTEXT, 0, NULL, context, context_size);
}

/** \brief Executes SHA command to write (restore) a SHA-256 context into the
 *          the device. Only supported for ATECC608A with SHA-256 contexts.
 *
 *  \param[in] context       Context data to be restored.
 *  \param[in] context_size  Size of the context data in bytes.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_write_context(const uint8_t* context, uint16_t context_size)
{
    return atcab_sha_base(SHA_MODE_WRITE_CONTEXT, context_size, context, NULL, NULL);
}

/** \brief Use the SHA command to compute a SHA-256 digest.
 *
 * \param[in]  length   Size of message parameter in bytes.
 * \param[in]  message  Message data to be hashed.
 * \param[out] digest   Digest is returned here (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha(uint16_t length, const uint8_t *message, uint8_t *digest)
{
    return atcab_hw_sha2_256(message, length, digest);
}

/** \brief Initialize a SHA context for performing a hardware SHA-256 operation
 *          on a device. Note that only one SHA operation can be run at a time.
 *
 * \param[in] ctx  SHA256 context
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256_init(atca_sha256_ctx_t* ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    return atcab_sha_start();
}

/** \brief Add message data to a SHA context for performing a hardware SHA-256
 *          operation on a device.
 *
 * \param[in] ctx        SHA256 context
 * \param[in] data       Message data to be added to hash.
 * \param[in] data_size  Size of data in bytes.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256_update(atca_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint32_t block_count;
    uint32_t rem_size = ATCA_SHA256_BLOCK_SIZE - ctx->block_size;
    uint32_t copy_size = data_size > rem_size ? rem_size : (uint32_t)data_size;
    uint32_t i = 0;

    // Copy data into current block
    memcpy(&ctx->block[ctx->block_size], data, copy_size);

    if (ctx->block_size + data_size < ATCA_SHA256_BLOCK_SIZE)
    {
        // Not enough data to finish off the current block
        ctx->block_size += (uint32_t)data_size;
        return ATCA_SUCCESS;
    }

    // Process the current block
    status = atcab_sha_update(ctx->block);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Process any additional blocks
    data_size -= copy_size; // Adjust to the remaining message bytes
    block_count = (uint32_t)(data_size / ATCA_SHA256_BLOCK_SIZE);
    for (i = 0; i < block_count; i++)
    {
        status = atcab_sha_update(&data[copy_size + i * ATCA_SHA256_BLOCK_SIZE]);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
    }

    // Save any remaining data
    ctx->total_msg_size += (block_count + 1) * ATCA_SHA256_BLOCK_SIZE;
    ctx->block_size = data_size % ATCA_SHA256_BLOCK_SIZE;
    memcpy(ctx->block, &data[copy_size + block_count * ATCA_SHA256_BLOCK_SIZE], ctx->block_size);

    return ATCA_SUCCESS;
}

/** \brief Finish SHA-256 digest for a SHA context for performing a hardware
 *          SHA-256 operation on a device.
 *
 * \param[in]  ctx     SHA256 context
 * \param[out] digest  SHA256 digest is returned here (32 bytes)
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256_finish(atca_sha256_ctx_t* ctx, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint32_t msg_size_bits;
    uint32_t pad_zero_count;
    uint16_t digest_size;

    if (_gDevice->mIface->mIfaceCFG->devtype == ATSHA204A)
    {
        // ATSHA204A only implements the raw 64-byte block operation, but
        // doesn't add in the final footer information. So we do that manually
        // here.

        // Calculate the total message size in bits
        ctx->total_msg_size += ctx->block_size;
        msg_size_bits = (ctx->total_msg_size * 8);

        // Calculate the number of padding zero bytes required between the 1 bit byte and the  ATCA_SHA256_BLOCK_SIZEbit message size in bits.
        pad_zero_count = (ATCA_SHA256_BLOCK_SIZE - ((ctx->block_size + 9) % ATCA_SHA256_BLOCK_SIZE)) % ATCA_SHA256_BLOCK_SIZE;

        // Append a single 1 bit
        ctx->block[ctx->block_size++] = 0x80;

        // Add padding zeros plus upper 4 bytes of total message size in bits (only supporting 32bit message bit counts)
        memset(&ctx->block[ctx->block_size], 0, pad_zero_count + 4);
        ctx->block_size += pad_zero_count + 4;

        // Add the total message size in bits to the end of the current block. Technically this is
        // supposed to be 8 bytes. This shortcut will reduce the max message size to 536,870,911 bytes.
        ctx->block[ctx->block_size++] = (uint8_t)(msg_size_bits >> 24);
        ctx->block[ctx->block_size++] = (uint8_t)(msg_size_bits >> 16);
        ctx->block[ctx->block_size++] = (uint8_t)(msg_size_bits >> 8);
        ctx->block[ctx->block_size++] = (uint8_t)(msg_size_bits >> 0);

        digest_size = 32;
        status = atcab_sha_base(SHA_MODE_SHA256_UPDATE, ATCA_SHA256_BLOCK_SIZE, ctx->block, digest, &digest_size);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
        if (ctx->block_size > ATCA_SHA256_BLOCK_SIZE)
        {
            digest_size = 32;
            status = atcab_sha_base(SHA_MODE_SHA256_UPDATE, ATCA_SHA256_BLOCK_SIZE, &ctx->block[ATCA_SHA256_BLOCK_SIZE], digest, &digest_size);
            if (status != ATCA_SUCCESS)
            {
                return status;
            }
        }
    }
    else
    {
        status = atcab_sha_end(digest, ctx->block_size, ctx->block);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
    }

    return ATCA_SUCCESS;
}

/** \brief Use the SHA command to compute a SHA-256 digest.
 *
 * \param[in]  data       Message data to be hashed.
 * \param[in]  data_size  Size of data in bytes.
 * \param[out] digest     Digest is returned here (32 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hw_sha2_256(const uint8_t * data, size_t data_size, uint8_t* digest)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    atca_sha256_ctx_t ctx;

    status = atcab_hw_sha2_256_init(&ctx);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    status = atcab_hw_sha2_256_update(&ctx, data, data_size);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    status = atcab_hw_sha2_256_finish(&ctx, digest);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

/** \brief Executes SHA command to start an HMAC/SHA-256 operation
 *
 * \param[in] ctx       HMAC/SHA-256 context
 * \param[in] key_slot  Slot key id to use for the HMAC calculation
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac_init(atca_hmac_sha256_ctx_t* ctx, uint16_t key_slot)
{
    memset(ctx, 0, sizeof(*ctx));
    return atcab_sha_base(SHA_MODE_HMAC_START, key_slot, NULL, NULL, NULL);
}

/** \brief Executes SHA command to add an arbitrary amount of message data to
 *          a HMAC/SHA-256 operation.
 *
 * \param[in] ctx        HMAC/SHA-256 context
 * \param[in] data       Message data to add
 * \param[in] data_size  Size of message data in bytes
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac_update(atca_hmac_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint32_t block_count;
    uint32_t rem_size = ATCA_SHA256_BLOCK_SIZE - ctx->block_size;
    uint32_t copy_size = data_size > rem_size ? rem_size : (uint32_t)data_size;
    uint32_t i = 0;

    // Copy data into current block
    memcpy(&ctx->block[ctx->block_size], data, copy_size);

    if (ctx->block_size + data_size < ATCA_SHA256_BLOCK_SIZE)
    {
        // Not enough data to finish off the current block
        ctx->block_size += (uint32_t)data_size;
        return ATCA_SUCCESS;
    }

    // Process the current block
    status = atcab_sha_base(SHA_MODE_HMAC_UPDATE, ATCA_SHA256_BLOCK_SIZE, ctx->block, NULL, NULL);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Process any additional blocks
    data_size -= copy_size; // Adjust to the remaining message bytes
    block_count = (uint32_t)(data_size / ATCA_SHA256_BLOCK_SIZE);
    for (i = 0; i < block_count; i++)
    {
        status = atcab_sha_base(SHA_MODE_HMAC_UPDATE, ATCA_SHA256_BLOCK_SIZE, &data[copy_size + i * ATCA_SHA256_BLOCK_SIZE], NULL, NULL);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
    }

    // Save any remaining data
    ctx->total_msg_size += (block_count + 1) * ATCA_SHA256_BLOCK_SIZE;
    ctx->block_size = data_size % ATCA_SHA256_BLOCK_SIZE;
    memcpy(ctx->block, &data[copy_size + block_count * ATCA_SHA256_BLOCK_SIZE], ctx->block_size);

    return ATCA_SUCCESS;
}

/** \brief Executes SHA command to complete a HMAC/SHA-256 operation.
 *
 * \param[in]  ctx     HMAC/SHA-256 context
 * \param[out] digest  HMAC/SHA-256 result is returned here (32 bytes).
 * \param[in]  target  Where to save the digest internal to the device.
 *                     For ATECC608A, can be SHA_MODE_TARGET_TEMPKEY,
 *                     SHA_MODE_TARGET_MSGDIGBUF, or SHA_MODE_TARGET_OUT_ONLY.
 *                     For all other devices, SHA_MODE_TARGET_TEMPKEY is the
 *                     only option.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac_finish(atca_hmac_sha256_ctx_t *ctx, uint8_t* digest, uint8_t target)
{
    uint8_t mode = SHA_MODE_HMAC_END;
    uint16_t digest_size = 32;

    if (ATECC608A == _gDevice->mIface->mIfaceCFG->devtype)
    {
        mode = SHA_MODE_608_HMAC_END;
    }
    else if (target != SHA_MODE_TARGET_TEMPKEY)
    {
        return ATCA_BAD_PARAM;
    }

    mode |= target;

    return atcab_sha_base(mode, ctx->block_size, ctx->block, digest, &digest_size);
}

/** \brief Use the SHA command to compute an HMAC/SHA-256 operation.
 *
 * \param[in]  data       Message data to be hashed.
 * \param[in]  data_size  Size of data in bytes.
 * \param[in]  key_slot   Slot key id to use for the HMAC calculation
 * \param[out] digest     Digest is returned here (32 bytes).
 * \param[in]  target     Where to save the digest internal to the device.
 *                        For ATECC608A, can be SHA_MODE_TARGET_TEMPKEY,
 *                        SHA_MODE_TARGET_MSGDIGBUF, or
 *                        SHA_MODE_TARGET_OUT_ONLY. For all other devices,
 *                        SHA_MODE_TARGET_TEMPKEY is the only option.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sha_hmac(const uint8_t * data, size_t data_size, uint16_t key_slot, uint8_t* digest, uint8_t target)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    atca_hmac_sha256_ctx_t ctx;

    status = atcab_sha_hmac_init(&ctx, key_slot);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    status = atcab_sha_hmac_update(&ctx, data, data_size);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    status = atcab_sha_hmac_finish(&ctx, digest, target);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}
