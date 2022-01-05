/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Sign command.
 *
 * The Sign command generates a signature using the private key in slot with
 * ECDSA algorithm.
 *
 * \note List of devices that support this command - ATECC108A, ATECC508A, and
 *       ATECC608A. There are differences in the modes that they support. Refer
 *       to device datasheets for full details.
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

/** \brief Executes the Sign command, which generates a signature using the
 *          ECDSA algorithm.
 *
 * \param[in]  mode       Mode determines what the source of the message to be
 *                        signed.
 * \param[in]  key_id     Private key slot used to sign the message.
 * \param[out] signature  Signature is returned here. Format is R and S
 *                        integers in big-endian format. 64 bytes for P256
 *                        curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign_base(uint8_t mode, uint16_t key_id, uint8_t *signature)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    if (signature == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        // Build sign command
        packet.param1 = mode;
        packet.param2 = key_id;
        if ((status = atSign(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if (signature != NULL)
        {
            if (packet.data[ATCA_COUNT_IDX] == (ATCA_SIG_SIZE + ATCA_PACKET_OVERHEAD))
            {
                memcpy(signature, &packet.data[ATCA_RSP_DATA_IDX], ATCA_SIG_SIZE);
            }
            else
            {
                status = ATCA_RX_FAIL;
            }

        }
    }
    while (0);

    return status;
}

/** \brief Executes Sign command, to sign a 32-byte external message using the
 *                   private key in the specified slot. The message to be signed
 *                   will be loaded into the Message Digest Buffer to the
 *                   ATECC608A device or TempKey for other devices.
 *
 *  \param[in]  key_id     Slot of the private key to be used to sign the
 *                         message.
 *  \param[in]  msg        32-byte message to be signed. Typically the SHA256
 *                         hash of the full message.
 *  \param[out] signature  Signature will be returned here. Format is R and S
 *                         integers in big-endian format. 64 bytes for P256
 *                         curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign(uint16_t key_id, const uint8_t *msg, uint8_t *signature)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t nonce_target = NONCE_MODE_TARGET_TEMPKEY;
    uint8_t sign_source = SIGN_MODE_SOURCE_TEMPKEY;

    do
    {
        // Make sure RNG has updated its seed
        if ((status = atcab_random(NULL)) != ATCA_SUCCESS)
        {
            break;
        }

        // Load message into device
        if (_gDevice->mCommands->dt == ATECC608A)
        {
            // Use the Message Digest Buffer for the ATECC608A
            nonce_target = NONCE_MODE_TARGET_MSGDIGBUF;
            sign_source = SIGN_MODE_SOURCE_MSGDIGBUF;
        }
        if ((status = atcab_nonce_load(nonce_target, msg, 32)) != ATCA_SUCCESS)
        {
            break;
        }

        // Sign the message
        if ((status = atcab_sign_base(SIGN_MODE_EXTERNAL | sign_source, key_id, signature)) != ATCA_SUCCESS)
        {
            break;
        }
    }
    while (0);

    return status;
}

/** \brief Executes Sign command to sign an internally generated message.
 *
 *  \param[in]  key_id         Slot of the private key to be used to sign the
 *                             message.
 *  \param[in]  is_invalidate  Set to true if the signature will be used with
 *                             the Verify(Invalidate) command. false for all
 *                             other cases.
 *  \param[in]  is_full_sn     Set to true if the message should incorporate
 *                             the device's full serial number.
 *  \param[out] signature      Signature is returned here. Format is R and S
 *                             integers in big-endian format. 64 bytes for
 *                             P256 curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_sign_internal(uint16_t key_id, bool is_invalidate, bool is_full_sn, uint8_t *signature)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t mode = SIGN_MODE_INTERNAL;

    do
    {
        // Sign the message
        if (is_invalidate)
        {
            mode |= SIGN_MODE_INVALIDATE;
        }

        if (is_full_sn)
        {
            mode |= SIGN_MODE_INCLUDE_SN;
        }

        if ((status = atcab_sign_base(mode, key_id, signature)) != ATCA_SUCCESS)
        {
            break;
        }

    }
    while (0);

    return status;
}
