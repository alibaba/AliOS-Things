/**
 * \file
 * \brief CryptoAuthLib Basic API methods for AES command.
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
#include "atca_execution.h"
#ifdef _WIN32
#include <stdlib.h>
#endif


/** \brief Compute the AES-128 encrypt, decrypt, or GFM calculation.
 *  \param[in]  mode     The mode for the AES command.
 *  \param[in]  key_id   Key location. Can either be a slot number or
 *                       ATCA_TEMPKEY_KEYID for TempKey.
 *  \param[in]  aes_in   Input data to the AES command (16 bytes).
 *  \param[out] aes_out  Output data from the AES command is returned here (16
 *                       bytes).
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes(uint8_t mode, uint16_t key_id, const uint8_t* aes_in, uint8_t* aes_out)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    do
    {
        if (aes_in == NULL)
        {
            status = ATCA_BAD_PARAM;
            break;
        }

        // build a AES command
        packet.param1 = mode;
        packet.param2 = key_id;
        if (AES_MODE_GFM == (mode & AES_MODE_GFM))
        {
            memcpy(packet.data, aes_in, ATCA_AES_GFM_SIZE);
        }
        else
        {
            memcpy(packet.data, aes_in, AES_DATA_SIZE);
        }

        if ((status = atAES(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if (aes_out && packet.data[ATCA_COUNT_IDX] >= (3 + AES_DATA_SIZE))
        {
            // The AES command return a 16 byte data.
            memcpy(aes_out, &packet.data[ATCA_RSP_DATA_IDX], AES_DATA_SIZE);
        }

    }
    while (0);

    return status;
}

/** \brief Perform an AES-128 encrypt operation with a key in the device.
 *
 * \param[in]  key_id      Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block   Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in]  plaintext   Input plaintext to be encrypted (16 bytes).
 * \param[out] ciphertext  Output ciphertext is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS  atcab_aes_encrypt(uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext)
{
    uint8_t mode;

    mode = AES_MODE_ENCRYPT | (AES_MODE_KEY_BLOCK_MASK & (key_block << AES_MODE_KEY_BLOCK_POS));
    return atcab_aes(mode, key_id, plaintext, ciphertext);
}

/** \brief Perform an AES-128 decrypt operation with a key in the device.
 *
 * \param[in]   key_id     Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]   key_block  Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in]  ciphertext  Input ciphertext to be decrypted (16 bytes).
 * \param[out] plaintext   Output plaintext is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS  atcab_aes_decrypt(uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext)
{
    uint8_t mode;

    mode = AES_MODE_DECRYPT | (AES_MODE_KEY_BLOCK_MASK & (key_block << AES_MODE_KEY_BLOCK_POS));
    return atcab_aes(mode, key_id, ciphertext, plaintext);
}

/** \brief Perform a Galois Field Multiply (GFM) operation.
 *
 * \param[in]   h       First input value (16 bytes).
 * \param[in]   input   Second input value (16 bytes).
 * \param[out]  output  GFM result is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_gfm(const uint8_t* h, const uint8_t* input, uint8_t* output)
{
    uint8_t aes_in[AES_DATA_SIZE * 2];

    memcpy(aes_in, h, AES_DATA_SIZE);
    memcpy(aes_in + AES_DATA_SIZE, input, AES_DATA_SIZE);
    // KeyID is ignored for GFM mode
    return atcab_aes(AES_MODE_GFM, 0x0000, aes_in, output);
}
