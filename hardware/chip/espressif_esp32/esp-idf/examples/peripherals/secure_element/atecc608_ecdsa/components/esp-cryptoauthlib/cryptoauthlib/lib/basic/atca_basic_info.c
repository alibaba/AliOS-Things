/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Info command.
 *
 * Info command returns a variety of static and dynamic information about the
 * device and its state. Also is used to control the GPIO pin and the persistent
 * latch.
 *
 * \note The ATSHA204A refers to this command as DevRev instead of Info,
 *       however, the OpCode and operation is the same.
 *
 * \note List of devices that support this command - ATSHA204A, ATECC108A,
 *       ATECC508A & ATECC608A. There are differences in the modes that they
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

/** \brief Issues an Info command, which return internal device information and
 *          can control GPIO and the persistent latch.
 *
 * \param[in]  mode      Selects which mode to be used for info command.
 * \param[in]  param2    Selects the particular fields for the mode.
 * \param[out] out_data  Response from info command (4 bytes). Can be set to
 *                       NULL if not required.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info_base(uint8_t mode, uint16_t param2, uint8_t* out_data)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // build an info command
    packet.param1 = mode;
    packet.param2 = param2;

    do
    {

        if ((status = atInfo(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if (out_data != NULL && packet.data[ATCA_COUNT_IDX] >= 7)
        {
            memcpy(out_data, &packet.data[ATCA_RSP_DATA_IDX], 4);
        }
    }
    while (0);

    return status;
}

/** \brief Use the Info command to get the device revision (DevRev).
 *  \param[out] revision  Device revision is returned here (4 bytes).
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info(uint8_t* revision)
{
    if (revision == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    return atcab_info_base(INFO_MODE_REVISION, 0, revision);
}

/** \brief Use the Info command to get the persistent latch current state for
 *          an ATECC608A device.
 *
 *  \param[out] state  The state is returned here. Set (true) or Cler (false).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS atcab_info_get_latch(bool* state)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t out_data[4];

    if (state == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    status = atcab_info_base(INFO_MODE_VOL_KEY_PERMIT, 0, out_data);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    *state = (out_data[0] == 1);

    return status;
}

/** \brief Use the Info command to set the persistent latch state for an
 *          ATECC608A device.
 *
 *  \param[out] state  Persistent latch state. Set (true) or clear (false).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_info_set_latch(bool state)
{
    uint16_t param2 = INFO_PARAM2_SET_LATCH_STATE;

    param2 |= state ? INFO_PARAM2_LATCH_SET : INFO_PARAM2_LATCH_CLEAR;
    return atcab_info_base(INFO_MODE_VOL_KEY_PERMIT, param2, NULL);
}
