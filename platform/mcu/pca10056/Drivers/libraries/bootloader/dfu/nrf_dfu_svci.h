/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/**@file
 *
 * @defgroup nrf_dfu_svci Supervisor call interface
 * @{
 * @ingroup  sdk_nrf_bootloader
 * @brief The Supervisor call interface is a thread-safe method to call into the current application or into an external application using a Supervisor instruction.
 *
 */


#ifndef NRF_DFU_SVCI_H__
#define NRF_DFU_SVCI_H__

#include <stdbool.h>
#include "nrf_svci.h"
#include "nrf_svci_async_function.h"
#include "ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYSTEM_SERVICE_ATT_SIZE 8       /**< Size of the system service attribute length including CRC-16 at the end. */

typedef enum
{
    NRF_DFU_SVCI_EXECUTE_COMMAND_INIT,
    NRF_DFU_SVCI_EXECUTE_DATA_INIT,
    NRF_DFU_SVCI_SET_PEER_DATA,
    NRF_DFU_SVCI_SET_ADV_NAME,
} nrf_dfu_svci_numbers;


typedef struct
{
    uint32_t            crc;                                    /**< CRC of the rest of the parameters in this struct. */
    ble_gap_id_key_t    ble_id;                                 /**< BLE GAP identity key of the device that initiated the DFU process. */
    ble_gap_enc_key_t   enc_key;                                /**< Encryption key structure containing encrypted diversifier and LTK for reestablishing the bond. */
    uint8_t             sys_serv_attr[SYSTEM_SERVICE_ATT_SIZE]; /**< System service attributes for restoring of Service Changed Indication setting in DFU mode. */
} nrf_dfu_peer_data_t;


typedef enum
{
    DFU_PEER_DATA_STATE_INVALID             = 0,
    DFU_PEER_DATA_STATE_INITIALIZED         = 1,
    DFU_PEER_DATA_STATE_WRITE_REQUESTED     = 2,
    DFU_PEER_DATA_STATE_WRITE_FINISHED      = 3,
    DFU_PEER_DATA_STATE_WRITE_FAILED        = 4,
} nrf_dfu_peer_data_state_t;


typedef struct
{
    uint32_t    crc;                            /**< CRC of the rest of the parameters in this struct. Calculated by the bootloader. */
    uint8_t     name[20];                       /**< New advertisement name to set. */
    uint32_t    len;                            /**< Length of the advertisement name. */
} nrf_dfu_adv_name_t;


typedef enum
{
    DFU_ADV_NAME_STATE_INVALID              = 0,
    DFU_ADV_NAME_STATE_INITIALIZED          = 1,
    DFU_ADV_NAME_STATE_WRITE_REQUESTED      = 2,
    DFU_ADV_NAME_STATE_WRITE_FINISHED       = 3,
    DFU_ADV_NAME_STATE_WRITE_FAILED         = 4,
} nrf_dfu_set_adv_name_state_t;


/**@brief Sets up the async SVCI interface for exchanging peer data like bonding and the system attribute table.
 *
 * @details The peer data will be stored in flash by the bootloader. This requires memory management and
 *          handling forwarding of system events and state from the main application to the bootloader.
 *
 * @note    This is only available in the buttonless DFU that supports bond sharing.
 */
NRF_SVCI_ASYNC_FUNC_DECLARE(NRF_DFU_SVCI_SET_PEER_DATA, nrf_dfu_set_peer_data, nrf_dfu_peer_data_t, nrf_dfu_peer_data_state_t);


/**@brief Sets up the async SVCI interface for exchanging advertisement name to use when entering DFU mode.
 *
 * @details The advertisement name will be stored in flash by the bootloader. This requires memory management
 *          and handling forwarding of system events and state from the main application to the bootloader.
 *
 * @note    This is only available in the buttonless DFU that does not support bond sharing.
 */
NRF_SVCI_ASYNC_FUNC_DECLARE(NRF_DFU_SVCI_SET_ADV_NAME, nrf_dfu_set_adv_name, nrf_dfu_adv_name_t, nrf_dfu_set_adv_name_state_t);

#ifdef __cplusplus
}
#endif

#endif // NRF_DFU_SVCI_H__

/** @} */
