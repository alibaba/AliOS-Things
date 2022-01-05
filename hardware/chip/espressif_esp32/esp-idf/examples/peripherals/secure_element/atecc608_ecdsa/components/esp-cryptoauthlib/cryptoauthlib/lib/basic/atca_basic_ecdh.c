/**
 * \file
 * \brief CryptoAuthLib Basic API methods for ECDH command.
 *
 * The ECDH command implements the Elliptic Curve Diffie-Hellman algorithm to
 * combine an internal private key with an external public key to calculate a
 * shared secret.
 *
 * \note List of devices that support this command - ATECC508A, ATECC608A.
 *       There are differences in  the modes that they support. Refer to device
 *       datasheets for full details.
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
#include "host/atca_host.h"


/** \brief Base function for generating premaster secret key using ECDH.
 *  \param[in]  mode        Mode to be used for ECDH computation
 *  \param[in]  key_id      Slot of key for ECDH computation
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH pre-master secret is returned here (32
 *                          bytes) if returned directly. Otherwise NULL.
 *  \param[out] out_nonce   Nonce used to encrypt pre-master secret. NULL if
 *                          output encryption not used.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_base(uint8_t mode, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, uint8_t* out_nonce)
{
    ATCAPacket packet;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    do
    {
        // Build Command
        packet.param1 = mode;
        packet.param2 = key_id;
        memcpy(packet.data, public_key, ATCA_PUB_KEY_SIZE);

        if ((status = atECDH(_gDevice->mCommands, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if (pms != NULL && packet.data[ATCA_COUNT_IDX] >= (3 + ATCA_KEY_SIZE))
        {
            memcpy(pms, &packet.data[ATCA_RSP_DATA_IDX], ATCA_KEY_SIZE);
        }

        if (out_nonce != NULL && packet.data[ATCA_COUNT_IDX] >= (3 + ATCA_KEY_SIZE * 2))
        {
            memcpy(out_nonce, &packet.data[ATCA_RSP_DATA_IDX + ATCA_KEY_SIZE], ATCA_KEY_SIZE);
        }

    }
    while (0);

    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is returned in the clear.
 *
 *  \param[in] key_id     Slot of key for ECDH computation
 *  \param[in] public_key Public key input to ECDH calculation. X and Y
 *                        integers in big-endian format. 64 bytes for P256
 *                        key.
 *  \param[out] pms       Computed ECDH premaster secret is returned here.
 *                        32 bytes.
 *
 *  \return ATCA_SUCCESS on success
 */
ATCA_STATUS atcab_ecdh(uint16_t key_id, const uint8_t* public_key, uint8_t* pms)
{
    ATCA_STATUS status;

    status = atcab_ecdh_base(ECDH_PREFIX_MODE, key_id, public_key, pms, NULL);

    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is read from the next slot.
 *
 * This function only works for even numbered slots with the proper
 * configuration.
 *
 *  \param[in]  key_id       Slot of key for ECDH computation
 *  \param[in]  public_key   Public key input to ECDH calculation. X and Y
 *                           integers in big-endian format. 64 bytes for P256
 *                           key.
 *  \param[out] pms          Computed ECDH premaster secret is returned here
 *                           (32 bytes).
 *  \param[in]  read_key     Read key for the premaster secret slot (key_id|1).
 *  \param[in]  read_key_id  Read key slot for read_key.
 *  \param[in]  num_in       20 byte host nonce to inject into Nonce calculation
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id)
#else
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE])
#endif
{
    ATCA_STATUS status = ATCA_SUCCESS;

    do
    {
        // Check the inputs
        if (public_key == NULL || pms == NULL || read_key == NULL)
        {
            status = ATCA_BAD_PARAM;
            BREAK(status, "Bad input parameters");
        }

        // Send the ECDH command with the public key provided
        if ((status = atcab_ecdh(key_id, public_key, NULL)) != ATCA_SUCCESS)
        {
            BREAK(status, "ECDH Failed");
        }
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
        if ((status = atcab_read_enc(key_id | 0x0001, 0, pms, read_key, read_key_id)) != ATCA_SUCCESS)
#else
        if ((status = atcab_read_enc(key_id | 0x0001, 0, pms, read_key, read_key_id, num_in)) != ATCA_SUCCESS)
#endif
        {
            BREAK(status, "Encrypted read failed");
        }
    }
    while (0);

    return status;
}

/** \brief ECDH command with a private key in a slot and the premaster secret
 *         is returned encrypted using the IO protection key.
 *
 *  \param[in]  key_id       Slot of key for ECDH computation
 *  \param[in]  public_key   Public key input to ECDH calculation. X and Y
 *                           integers in big-endian format. 64 bytes for P256
 *                           key.
 *  \param[out] pms          Computed ECDH premaster secret is returned here
 *                           (32 bytes).
 *  \param[in]  io_key       IO protection key.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_ioenc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    uint8_t mode = ECDH_MODE_SOURCE_EEPROM_SLOT | ECDH_MODE_OUTPUT_ENC | ECDH_MODE_COPY_OUTPUT_BUFFER;
    uint8_t out_nonce[ATCA_KEY_SIZE];
    atca_io_decrypt_in_out_t io_dec_params;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Perform ECDH operation requesting output buffer encryption
    status = atcab_ecdh_base(mode, key_id, public_key, pms, out_nonce);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Decrypt PMS
    memset(&io_dec_params, 0, sizeof(io_dec_params));
    io_dec_params.io_key = io_key;
    io_dec_params.out_nonce = out_nonce;
    io_dec_params.data = pms;
    io_dec_params.data_size = 32;
    status = atcah_io_decrypt(&io_dec_params);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return status;
}

/** \brief ECDH command with a private key in TempKey and the premaster secret
 *         is returned in the clear.
 *
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH premaster secret is returned here
 *                          (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_tempkey(const uint8_t* public_key, uint8_t* pms)
{
    // Perform ECDH operation with TempKey
    uint8_t mode = ECDH_MODE_SOURCE_TEMPKEY | ECDH_MODE_COPY_OUTPUT_BUFFER;

    return atcab_ecdh_base(mode, 0x0000, public_key, pms, NULL);
}

/** \brief ECDH command with a private key in TempKey and the premaster secret
 *         is returned encrypted using the IO protection key.
 *
 *  \param[in]  public_key  Public key input to ECDH calculation. X and Y
 *                          integers in big-endian format. 64 bytes for P256
 *                          key.
 *  \param[out] pms         Computed ECDH premaster secret is returned here
 *                          (32 bytes).
 *  \param[in]  io_key      IO protection key.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_ecdh_tempkey_ioenc(const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key)
{
    uint8_t mode = ECDH_MODE_SOURCE_TEMPKEY | ECDH_MODE_OUTPUT_ENC | ECDH_MODE_COPY_OUTPUT_BUFFER;
    uint8_t out_nonce[ATCA_KEY_SIZE];
    atca_io_decrypt_in_out_t io_dec_params;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Perform ECDH operation requesting output buffer encryption
    status = atcab_ecdh_base(mode, 0x0000, public_key, pms, out_nonce);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Decrypt PMS
    memset(&io_dec_params, 0, sizeof(io_dec_params));
    io_dec_params.io_key = io_key;
    io_dec_params.out_nonce = out_nonce;
    io_dec_params.data = pms;
    io_dec_params.data_size = 32;
    status = atcah_io_decrypt(&io_dec_params);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return status;
}

