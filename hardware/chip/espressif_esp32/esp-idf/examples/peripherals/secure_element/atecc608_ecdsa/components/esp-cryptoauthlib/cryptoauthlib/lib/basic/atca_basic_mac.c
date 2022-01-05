/**
 * \file
 * \brief CryptoAuthLib Basic API methods for MAC command.
 *
 * The MAC command computes a SHA-256 digest of a key stored in the device, a
 * challenge, and other information on the device. The output of this command
 * is the digest of this message.
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

/** \brief Executes MAC command, which computes a SHA-256 digest of a key
 *          stored in the device, a challenge, and other information on the
 *          device.
 *
 *	\param[in]  mode       Controls which fields within the device are used in
 *                         the message
 *	\param[in]  key_id     Key in the CryptoAuth device to use for the MAC
 *	\param[in]  challenge  Challenge message (32 bytes). May be NULL if mode
 *                         indicates a challenge isn't required.
 *	\param[out] digest     MAC response is returned here (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_mac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    do
    {
        if (digest == NULL)
        {
            return ATCA_BAD_PARAM;
        }

        // build mac command
        packet.param1 = mode;
        packet.param2 = key_id;
        if (!(mode & MAC_MODE_BLOCK2_TEMPKEY))
        {
            if (challenge == NULL)
            {
                return ATCA_BAD_PARAM;
            }
            memcpy(&packet.data[0], challenge, 32);  // a 32-byte challenge
        }

        if ((status = atMAC(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        memcpy(digest, &packet.data[ATCA_RSP_DATA_IDX], MAC_SIZE);

    }
    while (0);

    return status;
}
