/**
 * \file
 * \brief Implements an execution handler that executes a given command on a
 *        device and returns the results.
 *
 * This implementation wraps Polling and No polling (simple wait) schemes into
 * a single method and use it across the library. Polling is used by default,
 * however, by defining the ATCA_NO_POLL symbol the code will instead wait an
 * estimated max execution time before requesting the result.
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

#include <stdlib.h>
#include <string.h>
#include "atca_command.h"
#include "atca_device.h"
#include "atca_execution.h"
#include "atca_devtypes.h"
#include "hal/atca_hal.h"

#ifndef ATCA_POLLING_INIT_TIME_MSEC
#define ATCA_POLLING_INIT_TIME_MSEC       1
#endif

#ifndef ATCA_POLLING_FREQUENCY_TIME_MSEC
#define ATCA_POLLING_FREQUENCY_TIME_MSEC  2
#endif

#ifndef ATCA_POLLING_MAX_TIME_MSEC
#define ATCA_POLLING_MAX_TIME_MSEC        2500
#endif

#ifdef ATCA_NO_POLL
// *INDENT-OFF* - Preserve time formatting from the code formatter
/*Execution times for ATSHA204A supported commands...*/
static const device_execution_time_t device_execution_time_204[] = {
    { ATCA_CHECKMAC,     38},
    { ATCA_DERIVE_KEY,   62},
    { ATCA_GENDIG,       43},
    { ATCA_HMAC,         69},
    { ATCA_INFO,         2},
    { ATCA_LOCK,         24},
    { ATCA_MAC,          35},
    { ATCA_NONCE,        60},
    { ATCA_PAUSE,        2},
    { ATCA_RANDOM,       50},
    { ATCA_READ,         5},
    { ATCA_SHA,          22},
    { ATCA_UPDATE_EXTRA, 12},
    { ATCA_WRITE,        42}
};

/*Execution times for ATECC108A supported commands...*/
static const device_execution_time_t device_execution_time_108[] = {
    { ATCA_CHECKMAC,     13},
    { ATCA_COUNTER,      20},
    { ATCA_DERIVE_KEY,   50},
    { ATCA_GENDIG,       11},
    { ATCA_GENKEY,       115},
    { ATCA_HMAC,         23},
    { ATCA_INFO,         2},
    { ATCA_LOCK,         32},
    { ATCA_MAC,          14},
    { ATCA_NONCE,        29},
    { ATCA_PAUSE,        3},
    { ATCA_PRIVWRITE,    48},
    { ATCA_RANDOM,       23},
    { ATCA_READ,         5},
    { ATCA_SHA,          9},
    { ATCA_SIGN,         60},
    { ATCA_UPDATE_EXTRA, 10},
    { ATCA_VERIFY,       72},
    { ATCA_WRITE,        26}
};

/*Execution times for ATECC508A supported commands...*/
static const device_execution_time_t device_execution_time_508[] = {
    { ATCA_CHECKMAC,     13},
    { ATCA_COUNTER,      20},
    { ATCA_DERIVE_KEY,   50},
    { ATCA_ECDH,         58},
    { ATCA_GENDIG,       11},
    { ATCA_GENKEY,       115},
    { ATCA_HMAC,         23},
    { ATCA_INFO,         2},
    { ATCA_LOCK,         32},
    { ATCA_MAC,          14},
    { ATCA_NONCE,        29},
    { ATCA_PAUSE,        3},
    { ATCA_PRIVWRITE,    48},
    { ATCA_RANDOM,       23},
    { ATCA_READ,         5},
    { ATCA_SHA,          9},
    { ATCA_SIGN,         60},
    { ATCA_UPDATE_EXTRA, 10},
    { ATCA_VERIFY,       72},
    { ATCA_WRITE,        26}
};

/*Execution times for ATECC608A-M0 supported commands...*/
static const device_execution_time_t device_execution_time_608_m0[] = {
    { ATCA_AES,          27},
    { ATCA_CHECKMAC,     40},
    { ATCA_COUNTER,      25},
    { ATCA_DERIVE_KEY,   50},
    { ATCA_ECDH,         75},
    { ATCA_GENDIG,       25},
    { ATCA_GENKEY,       115},
    { ATCA_INFO,         5},
    { ATCA_KDF,          165},
    { ATCA_LOCK,         35},
    { ATCA_MAC,          55},
    { ATCA_NONCE,        20},
    { ATCA_PRIVWRITE,    50},
    { ATCA_RANDOM,       23},
    { ATCA_READ,         5},
    { ATCA_SECUREBOOT,   80},
    { ATCA_SELFTEST,     250},
    { ATCA_SHA,          36},
    { ATCA_SIGN,         115},
    { ATCA_UPDATE_EXTRA, 10},
    { ATCA_VERIFY,       105},
    { ATCA_WRITE,        45}
};

/*Execution times for ATECC608A-M1 supported commands...*/
static const device_execution_time_t device_execution_time_608_m1[] = {
    { ATCA_AES,          27},
    { ATCA_CHECKMAC,     40},
    { ATCA_COUNTER,      25},
    { ATCA_DERIVE_KEY,   50},
    { ATCA_ECDH,         172},
    { ATCA_GENDIG,       35},
    { ATCA_GENKEY,       215},
    { ATCA_INFO,         5},
    { ATCA_KDF,          165},
    { ATCA_LOCK,         35},
    { ATCA_MAC,          55},
    { ATCA_NONCE,        20},
    { ATCA_PRIVWRITE,    50},
    { ATCA_RANDOM,       23},
    { ATCA_READ,         5},
    { ATCA_SECUREBOOT,   160},
    { ATCA_SELFTEST,     625},
    { ATCA_SHA,          42},
    { ATCA_SIGN,         220},
    { ATCA_UPDATE_EXTRA, 10},
    { ATCA_VERIFY,       295},
    { ATCA_WRITE,        45}
};

/*Execution times for ATECC608A-M2 supported commands...*/
static const device_execution_time_t device_execution_time_608_m2[] = {
    { ATCA_AES,          27},
    { ATCA_CHECKMAC,     40},
    { ATCA_COUNTER,      25},
    { ATCA_DERIVE_KEY,   50},
    { ATCA_ECDH,         531},
    { ATCA_GENDIG,       35},
    { ATCA_GENKEY,       653},
    { ATCA_INFO,         5},
    { ATCA_KDF,          165},
    { ATCA_LOCK,         35},
    { ATCA_MAC,          55},
    { ATCA_NONCE,        20},
    { ATCA_PRIVWRITE,    50},
    { ATCA_RANDOM,       23},
    { ATCA_READ,         5},
    { ATCA_SECUREBOOT,   480},
    { ATCA_SELFTEST,     2324},
    { ATCA_SHA,          75},
    { ATCA_SIGN,         665},
    { ATCA_UPDATE_EXTRA, 10},
    { ATCA_VERIFY,       1085},
    { ATCA_WRITE,        45}
};
// *INDENT-ON*
#endif

#ifdef ATCA_NO_POLL
/** \brief return the typical execution time for the given command
 *  \param[in] opcode  Opcode value of the command
 *  \param[in] ca_cmd  Command object for which the execution times are associated
 *  \return ATCA_SUCCESS
 */
ATCA_STATUS atGetExecTime(uint8_t opcode, ATCACommand ca_cmd)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    const device_execution_time_t *execution_times;
    uint8_t i, no_of_commands;


    switch (ca_cmd->dt)
    {
    case ATSHA204A:
        execution_times = device_execution_time_204;
        no_of_commands = sizeof(device_execution_time_204) / sizeof(device_execution_time_t);
        break;

    case ATECC108A:
        execution_times = device_execution_time_108;
        no_of_commands = sizeof(device_execution_time_108) / sizeof(device_execution_time_t);
        break;

    case ATECC508A:
        execution_times = device_execution_time_508;
        no_of_commands = sizeof(device_execution_time_508) / sizeof(device_execution_time_t);
        break;

    case ATECC608A:
        if (ca_cmd->clock_divider == ATCA_CHIPMODE_CLOCK_DIV_M1)
        {
            execution_times = device_execution_time_608_m1;
            no_of_commands = sizeof(device_execution_time_608_m1) / sizeof(device_execution_time_t);
        }
        else if (ca_cmd->clock_divider == ATCA_CHIPMODE_CLOCK_DIV_M2)
        {
            execution_times = device_execution_time_608_m2;
            no_of_commands = sizeof(device_execution_time_608_m2) / sizeof(device_execution_time_t);
        }
        else
        {
            // Assume default M0 clock divider
            execution_times = device_execution_time_608_m0;
            no_of_commands = sizeof(device_execution_time_608_m0) / sizeof(device_execution_time_t);
        }
        break;

    default:
        no_of_commands = 0;
        execution_times = NULL;
        break;
    }

    ca_cmd->execution_time_msec = ATCA_UNSUPPORTED_CMD;

    for (i = 0; i < no_of_commands; i++)
    {
        if (execution_times[i].opcode == opcode)
        {
            ca_cmd->execution_time_msec = execution_times[i].execution_time_msec;
            break;
        }
    }

    if (ca_cmd->execution_time_msec == ATCA_UNSUPPORTED_CMD)
    {
        status = ATCA_BAD_OPCODE;
    }

    return status;
}
#endif

/** \brief Wakes up device, sends the packet, waits for command completion,
 *         receives response, and puts the device into the idle state.
 *
 * \param[inout] packet  As input, the packet to be sent. As output, the
 *                       data buffer in the packet structure will contain the
 *                       response.
 * \param[in]    device  CryptoAuthentication device to send the command to.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_execute_command(ATCAPacket* packet, ATCADevice device)
{
    ATCA_STATUS status;
    uint32_t execution_or_wait_time;
    uint32_t max_delay_count;
    uint16_t rxsize;

    do
    {
#ifdef ATCA_NO_POLL
        if ((status = atGetExecTime(packet->opcode, device->mCommands)) != ATCA_SUCCESS)
        {
            return status;
        }
        execution_or_wait_time = device->mCommands->execution_time_msec;
        max_delay_count = 0;
#else
        execution_or_wait_time = ATCA_POLLING_INIT_TIME_MSEC;
        max_delay_count = ATCA_POLLING_MAX_TIME_MSEC / ATCA_POLLING_FREQUENCY_TIME_MSEC;
#endif

        if ((status = atwake(device->mIface)) != ATCA_SUCCESS)
        {
            break;
        }

        // send the command
        if ((status = atsend(device->mIface, (uint8_t*)packet, packet->txsize)) != ATCA_SUCCESS)
        {
            break;
        }

        // Delay for execution time or initial wait before polling
        atca_delay_ms(execution_or_wait_time);

        do
        {
            memset(packet->data, 0, sizeof(packet->data));
            // receive the response
            rxsize = sizeof(packet->data);
            if ((status = atreceive(device->mIface, packet->data, &rxsize)) == ATCA_SUCCESS)
            {
                break;
            }

#ifndef ATCA_NO_POLL
            // delay for polling frequency time
            atca_delay_ms(ATCA_POLLING_FREQUENCY_TIME_MSEC);
#endif
        }
        while (max_delay_count-- > 0);
        if (status != ATCA_SUCCESS)
        {
            break;
        }

        // Check response size
        if (rxsize < 4)
        {
            if (rxsize > 0)
            {
                status = ATCA_RX_FAIL;
            }
            else
            {
                status = ATCA_RX_NO_RESPONSE;
            }
            break;
        }

        if ((status = atCheckCrc(packet->data)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = isATCAError(packet->data)) != ATCA_SUCCESS)
        {
            break;
        }
    }
    while (0);

    atidle(device->mIface);
    return status;
}

/** @} */
