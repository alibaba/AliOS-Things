/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Counter command.
 *
 * The Counter command reads or increments the binary count value for one of the
 * two monotonic counters
 *
 * \note List of devices that support this command -  ATECC508A and ATECC608A.
 *       There are differences in the modes that they support. Refer to device
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


/** \brief Compute the Counter functions
 *  \param[in]  mode the mode used for the counter
 *  \param[in]  counter_id The counter to be used
 *  \param[out] counter_value pointer to the counter value returned from device
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS atcab_counter(uint8_t mode, uint16_t counter_id, uint32_t *counter_value)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    do
    {
        if (counter_id > 1)
        {
            return ATCA_BAD_PARAM;
        }

        // build a Counter command
        packet.param1 = mode;
        packet.param2 = counter_id;

        if ((status = atCounter(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        if (counter_value != NULL)
        {
            if (packet.data[ATCA_COUNT_IDX] == 7)
            {
                *counter_value = ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 0] <<  0) |
                                 ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 1] <<  8) |
                                 ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 2] << 16) |
                                 ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 3] << 24);
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

/** \brief Increments one of the device's monotonic counters
 *  \param[in]  counter_id     Counter to be incremented
 *  \param[out] counter_value  New value of the counter is returned here. Can be
 *                             NULL if not needed.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_counter_increment(uint16_t counter_id, uint32_t* counter_value)
{
    return atcab_counter(COUNTER_MODE_INCREMENT, counter_id, counter_value);
}

/** \brief Read one of the device's monotonic counters
 *  \param[in]  counter_id     Counter to be read
 *  \param[out] counter_value  Counter value is returned here.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_counter_read(uint16_t counter_id, uint32_t* counter_value)
{
    return atcab_counter(COUNTER_MODE_READ, counter_id, counter_value);
}
