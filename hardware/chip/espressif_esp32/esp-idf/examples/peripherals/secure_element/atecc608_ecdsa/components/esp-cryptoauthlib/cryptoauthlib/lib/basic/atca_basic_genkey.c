/**
 * \file
 * \brief CryptoAuthLib Basic API methods for GenKey command.
 *
 * The GenKey command is used for creating ECC private keys, generating ECC
 * public keys, and for digest calculations involving public keys.
 *
 * \note List of devices that support this command - ATECC108A, ATECC508A,
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

/** \brief Issues GenKey command, which can generate a private key, compute a
 *          public key, nd/or compute a digest of a public key.
 *
 * \param[in]  mode        Mode determines what operations the GenKey
 *                         command performs.
 * \param[in]  key_id      Slot to perform the GenKey command on.
 * \param[in]  other_data  OtherData for PubKey digest calculation. Can be set
 *                         to NULL otherwise.
 * \param[out] public_key  If the mode indicates a public key will be
 *                         calculated, it will be returned here. Format will
 *                         be the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve. Set to NULL if public key
 *                         isn't required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_genkey_base(uint8_t mode, uint16_t key_id, const uint8_t* other_data, uint8_t* public_key)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    do
    {
        // Build GenKey command
        packet.param1 = mode;
        packet.param2 = key_id;
        if (other_data)
        {
            memcpy(packet.data, other_data, GENKEY_OTHER_DATA_SIZE);
        }

        if ((status = atGenKey(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if (public_key != NULL)
        {
            if (packet.data[ATCA_COUNT_IDX] == (ATCA_PUB_KEY_SIZE + ATCA_PACKET_OVERHEAD))
            {
                memcpy(public_key, &packet.data[ATCA_RSP_DATA_IDX], ATCA_PUB_KEY_SIZE);
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

/** \brief Issues GenKey command, which generates a new random private key in
 *          slot and returns the public key.
 *
 * \param[in]  key_id      Slot number where an ECC private key is configured.
 *                         Can also be ATCA_TEMPKEY_KEYID to generate a private
 *                         key in TempKey.
 * \param[out] public_key  Public key will be returned here. Format will be
 *                         the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve. Set to NULL if public key
 *                         isn't required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_genkey(uint16_t key_id, uint8_t *public_key)
{
    return atcab_genkey_base(GENKEY_MODE_PRIVATE, key_id, NULL, public_key);
}

/** \brief Uses GenKey command to calculate the public key from an existing
 *          private key in a slot.
 *
 *  \param[in]  key_id      Slot number of the private key.
 *  \param[out] public_key  Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve. Set to NULL if public key
 *                          isn't required.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_get_pubkey(uint16_t key_id, uint8_t *public_key)
{
    return atcab_genkey_base(GENKEY_MODE_PUBLIC, key_id, NULL, public_key);
}