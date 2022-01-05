/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Nonce command.
 *
 * The Nonce command generates a nonce for use by a subsequent commands of the
 * device by combining an internally generated random number with an input value
 * from the system.
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

/** \brief Executes Nonce command, which loads a random or fixed nonce/data
 *          into the device for use by subsequent commands.
 *
 * \param[in]  mode         Controls the mechanism of the internal RNG or fixed
 *                          write.
 * \param[in]  zero         Param2, normally 0, but can be used to indicate a
 *                          nonce calculation mode (bit 15).
 * \param[in]  num_in       Input value to either be included in the nonce
 *                          calculation in random modes (20 bytes) or to be
 *                          written directly (32 bytes or 64 bytes(ATECC608A))
 *                          in pass-through mode.
 * \param[out] rand_out     If using a random mode, the internally generated
 *                          32-byte random number that was used in the nonce
 *                          calculation is returned here. Can be NULL if not
 *                          needed.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce_base(uint8_t mode, uint16_t zero, const uint8_t *num_in, uint8_t* rand_out)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t nonce_mode = mode & NONCE_MODE_MASK;

    do
    {
        // build a nonce command
        packet.param1 = mode;
        packet.param2 = zero;

        // Copy the right amount of NumIn data
        if ((nonce_mode == NONCE_MODE_SEED_UPDATE || nonce_mode == NONCE_MODE_NO_SEED_UPDATE))
        {
            memcpy(packet.data, num_in, NONCE_NUMIN_SIZE);
        }
        else if (nonce_mode == NONCE_MODE_PASSTHROUGH)
        {
            if ((mode & NONCE_MODE_INPUT_LEN_MASK) == NONCE_MODE_INPUT_LEN_64)
            {
                memcpy(packet.data, num_in, 64);
            }
            else
            {
                memcpy(packet.data, num_in, 32);
            }
        }
        else
        {
            return ATCA_BAD_PARAM;
        }

        if ((status = atNonce(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((rand_out != NULL) && (packet.data[ATCA_COUNT_IDX] >= 35))
        {
            memcpy(&rand_out[0], &packet.data[ATCA_RSP_DATA_IDX], 32);
        }

    }
    while (0);

    return status;
}


/** \brief Execute a Nonce command in pass-through mode to initialize TempKey
 *         to a specified value.
 *
 *  \param[in] num_in  Data to be loaded into TempKey (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce(const uint8_t *num_in)
{
    return atcab_nonce_base(NONCE_MODE_PASSTHROUGH, 0, num_in, NULL);
}


/** \brief Execute a Nonce command in pass-through mode to load one of the
 *          device's internal buffers with a fixed value.
 *
 * For the ATECC608A, available targets are TempKey (32 or 64 bytes), Message
 * Digest Buffer (32 or 64 bytes), or the Alternate Key Buffer (32 bytes). For
 * all other devices, only TempKey (32 bytes) is available.
 *
 *  \param[in] target       Target device buffer to load. Can be
 *                          NONCE_MODE_TARGET_TEMPKEY,
 *                          NONCE_MODE_TARGET_MSGDIGBUF, or
 *                          NONCE_MODE_TARGET_ALTKEYBUF.
 *  \param[in] num_in       Data to load into the buffer.
 *  \param[in] num_in_size  Size of num_in in bytes. Can be 32 or 64 bytes
 *                          depending on device and target.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce_load(uint8_t target, const uint8_t *num_in, uint16_t num_in_size)
{
    uint8_t mode = NONCE_MODE_PASSTHROUGH | (NONCE_MODE_TARGET_MASK & target);

    if (num_in_size == 32)
    {
        mode |= NONCE_MODE_INPUT_LEN_32;
    }
    else if (num_in_size == 64)
    {
        mode |= NONCE_MODE_INPUT_LEN_64;
    }
    else
    {
        return ATCA_BAD_PARAM;
    }

    return atcab_nonce_base(mode, 0, num_in, NULL);
}

/** \brief Execute a Nonce command to generate a random nonce combining a host
 *          nonce (num_in) and a device random number.
 *
 *  \param[in]  num_in    Host nonce to be combined with the device random
 *                        number (20 bytes).
 *  \param[out] rand_out  Internally generated 32-byte random number that was
 *                        used in the nonce/challenge calculation is returned
 *                        here. Can be NULL if not needed.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_nonce_rand(const uint8_t *num_in, uint8_t* rand_out)
{
    return atcab_nonce_base(NONCE_MODE_SEED_UPDATE, 0, num_in, rand_out);
}

/** \brief Execute a Nonce command in pass-through mode to initialize TempKey
 *          to a specified value.
 *
 *  \param[in] num_in  Data to be loaded into TempKey (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_challenge(const uint8_t *num_in)
{
    return atcab_nonce_base(NONCE_MODE_PASSTHROUGH, 0, num_in, NULL);
}

/** \brief Execute a Nonce command to generate a random challenge combining
 *         a host nonce (num_in) and a device random number.
 *
 *  \param[in]  num_in    Host nonce to be combined with the device random
 *                        number (20 bytes).
 *  \param[out] rand_out  Internally generated 32-byte random number that was
 *                        used in the nonce/challenge calculation is returned
 *                        here. Can be NULL if not needed.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_challenge_seed_update(const uint8_t *num_in, uint8_t* rand_out)
{
    return atcab_nonce_base(NONCE_MODE_SEED_UPDATE, 0, num_in, rand_out);
}