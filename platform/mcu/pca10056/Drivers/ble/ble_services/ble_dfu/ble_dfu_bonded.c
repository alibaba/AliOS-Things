/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "compiler_abstraction.h"
#include "nrf_dfu_svci.h"
#include "nordic_common.h"
#include "nrf_dfu_svci.h"
#include "nrf_error.h"
#include "ble_dfu.h"
#include "nrf_log.h"
#include "peer_manager.h"
#include "gatts_cache_manager.h"
#include "peer_id.h"
#include "nrf_sdh_soc.h"

#if defined(NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS) && (NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS == 1)

/**@brief Define function for async interface to set peer data. */
NRF_SVCI_ASYNC_FUNC_DEFINE(NRF_DFU_SVCI_SET_PEER_DATA, nrf_dfu_set_peer_data, nrf_dfu_peer_data_t);

ble_dfu_buttonless_t       * mp_dfu;
static nrf_dfu_peer_data_t   m_peer_data;

void ble_dfu_buttonless_on_sys_evt(uint32_t, void * );
// Register SoC observer for the Buttonless Secure DFU service
NRF_SDH_SOC_OBSERVER(m_dfu_buttonless_soc_obs, BLE_DFU_SOC_OBSERVER_PRIO, ble_dfu_buttonless_on_sys_evt, NULL);

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    uint32_t err_code;
    if (mp_dfu == NULL)
    {
        return;
    }

    // Only handle this when we are waiting to reset into DFU mode
    if (!mp_dfu->is_waiting_for_reset)
    {
        return;
    }

    switch(p_evt->evt_id)
    {
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            if (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_GATT_LOCAL)
            {
                mp_dfu->peers_count--;
                NRF_LOG_DEBUG("Updating Service Changed indication for peers, %d left", mp_dfu->peers_count);
                if (mp_dfu->peers_count == 0)
                {
                    NRF_LOG_DEBUG("Finished updating Service Changed indication for peers");
                    // We have updated Service Changed Indication for all devices.
                    err_code = ble_dfu_buttonless_bootloader_start_finalize();
                    if (err_code != NRF_SUCCESS)
                    {
                        mp_dfu->evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED);
                    }
                }
                break;
            }

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            // Failure to update data. Service Changed cannot be sent but DFU mode is still possible
            err_code = ble_dfu_buttonless_bootloader_start_finalize();
            if (err_code != NRF_SUCCESS)
            {
                mp_dfu->evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED);
            }

            break;

        default:
            break;
    }

}


static uint32_t retrieve_peer_data(void)
{
    ret_code_t              ret_code;
    pm_peer_data_bonding_t  bonding_data;
    pm_peer_id_t            peer_id;

    ret_code = pm_peer_id_get(mp_dfu->conn_handle, &peer_id);
    VERIFY_SUCCESS(ret_code);

    if (peer_id == PM_PEER_ID_INVALID)
    {
        return NRF_ERROR_FORBIDDEN;
    }

    ret_code = pm_peer_data_bonding_load(peer_id, &bonding_data);
    VERIFY_SUCCESS(ret_code);

    memcpy(&m_peer_data.ble_id, &bonding_data.peer_ble_id, sizeof(ble_gap_id_key_t));
    memcpy(&m_peer_data.enc_key, &bonding_data.own_ltk, sizeof(ble_gap_enc_key_t));

    uint16_t len = SYSTEM_SERVICE_ATT_SIZE;
    ret_code = sd_ble_gatts_sys_attr_get(mp_dfu->conn_handle,
                                         m_peer_data.sys_serv_attr,
                                         &len,
                                         BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS);

    NRF_LOG_INFO("---------------system attribute table: %d--------------", len);

    VERIFY_SUCCESS(ret_code);

    return NRF_SUCCESS;
}


/**@brief Function for entering the bootloader.
 *
 * @details This starts forwarding peer data to the Secure DFU bootloader.
 */
static uint32_t enter_bootloader(void)
{
    uint32_t err_code;

    NRF_LOG_INFO("Writing peer data to the bootloader");

    if (mp_dfu->is_waiting_for_svci)
    {
        return ble_dfu_buttonless_resp_send(DFU_OP_ENTER_BOOTLOADER, DFU_RSP_BUSY);
    }

    // If retrieve_peer_data returns NRF_ERROR_FORBIDDEN, then
    // the device was not bonded.
    err_code = retrieve_peer_data();
    VERIFY_SUCCESS(err_code);

    err_code = nrf_dfu_set_peer_data(&m_peer_data);
    if (err_code == NRF_SUCCESS)
    {
        // The request was accepted. Waiting for
        // sys events to progress.
        mp_dfu->is_waiting_for_svci = true;
    }

    return err_code;
}


uint32_t ble_dfu_buttonless_backend_init(ble_dfu_buttonless_t * p_dfu)
{
    uint32_t err_code = NRF_SUCCESS;
    
    VERIFY_PARAM_NOT_NULL(p_dfu);
    
    // Set the memory used by the backend.
    mp_dfu = p_dfu;
    
    // Initialize the Peer manager handler.
    err_code = pm_register(&pm_evt_handler);
    VERIFY_SUCCESS(err_code);
    
    return NRF_SUCCESS;
}


uint32_t nrf_dfu_svci_vector_table_set(void);
uint32_t nrf_dfu_svci_vector_table_unset(void);


uint32_t ble_dfu_buttonless_async_svci_init(void)
{
    uint32_t ret_val;

    // Set the vector table base address to the bootloader.
    ret_val = nrf_dfu_svci_vector_table_set();
    VERIFY_SUCCESS(ret_val);

    // Initialize the asynchronous SuperVisor interface to set peer data in Secure DFU bootloader.
    ret_val = nrf_dfu_set_peer_data_init();
    VERIFY_SUCCESS(ret_val);

    // Set the vector table base address back to main application.
    ret_val = nrf_dfu_svci_vector_table_unset();
    VERIFY_SUCCESS(ret_val);

    return ret_val;
}


void ble_dfu_buttonless_on_sys_evt(uint32_t sys_evt, void * p_context)
{
    uint32_t err_code;

    if (!nrf_dfu_set_peer_data_is_initialized())
    {
        return;
    }

    err_code = nrf_dfu_set_peer_data_on_sys_evt(sys_evt);
    if (err_code == NRF_ERROR_BUSY)
    {
        // The async operations are ongoing.
        // No action is taken, and nothing is reported
    }
    else if (err_code == NRF_SUCCESS)
    {
        // Peer data was successfully forwarded to the Secure DFU bootloader.
        // Set the flag indicating that we are waiting for indication response
        // to activate the reset.
        mp_dfu->is_waiting_for_reset = true;
        mp_dfu->is_waiting_for_svci = false;

        // Report back the positive response
        err_code = ble_dfu_buttonless_resp_send(DFU_OP_ENTER_BOOTLOADER, DFU_RSP_SUCCESS);
        if (err_code != NRF_SUCCESS)
        {
            mp_dfu->evt_handler(BLE_DFU_EVT_RESPONSE_SEND_ERROR);
            mp_dfu->is_waiting_for_reset = false;
        }
    }
    else
    {
        // Failed to set peer data. Report this.
        mp_dfu->is_waiting_for_reset = false;
        mp_dfu->is_waiting_for_svci = false;
        err_code = ble_dfu_buttonless_resp_send(DFU_OP_ENTER_BOOTLOADER, DFU_RSP_OPERATION_FAILED);

        // Report the failure to send the response to the client
        if (err_code != NRF_SUCCESS)
        {
            mp_dfu->evt_handler(BLE_DFU_EVT_RESPONSE_SEND_ERROR);
        }

        // Report the failure to enter DFU mode
        mp_dfu->evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED);
    }
}


uint32_t ble_dfu_buttonless_char_add(ble_dfu_buttonless_t * p_dfu)
{
    ble_gatts_char_md_t char_md         = {{0}};
    ble_gatts_attr_md_t cccd_md         = {{0}};
    ble_gatts_attr_t    attr_char_value = {0};
    ble_gatts_attr_md_t attr_md         = {{0}};
    ble_uuid_t          char_uuid;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    char_md.char_props.indicate     = 1;
    char_md.char_props.write        = 1;
    char_md.p_char_user_desc        = NULL;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = &cccd_md;
    char_md.p_sccd_md               = NULL;

    char_uuid.type = p_dfu->uuid_type;
    char_uuid.uuid = BLE_DFU_BUTTONLESS_BONDED_CHAR_UUID;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 1;
    attr_md.vlen    = 1;

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 0;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_GATT_ATT_MTU_DEFAULT;
    attr_char_value.p_value   = 0;

    return sd_ble_gatts_characteristic_add(p_dfu->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_dfu->control_point_char);
}


void ble_dfu_buttonless_on_ctrl_pt_write(ble_gatts_evt_write_t const * p_evt_write)
{
    uint32_t err_code;
    ble_dfu_buttonless_rsp_code_t rsp_code = DFU_RSP_OPERATION_FAILED;

    // Start executing the control point write action
    switch (p_evt_write->data[0])
    {
        case DFU_OP_ENTER_BOOTLOADER:
            err_code = enter_bootloader();
            if (err_code == NRF_SUCCESS)
            {
                rsp_code = DFU_RSP_SUCCESS;
            }
            else if (err_code == NRF_ERROR_FORBIDDEN)
            {
                rsp_code = DFU_RSP_NOT_BONDED;
            }
            break;

        default:
            rsp_code = DFU_RSP_OP_CODE_NOT_SUPPORTED;
            break;
    }


    // Report back in case of error
    if (rsp_code != DFU_RSP_SUCCESS)
    {
        err_code = ble_dfu_buttonless_resp_send((ble_dfu_buttonless_op_code_t)p_evt_write->data[0], rsp_code);
        if (err_code != NRF_SUCCESS)
        {
            mp_dfu->evt_handler(BLE_DFU_EVT_RESPONSE_SEND_ERROR);
        }

        // Report the error to the main application
        mp_dfu->evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED);
    }
}


uint32_t ble_dfu_buttonless_bootloader_start_prepare(void)
{
    uint32_t err_code;

    NRF_LOG_DEBUG("In ble_dfu_buttonless_bootloader_start_prepare");

    // Indicate to main app that DFU mode is starting.
    // This event can be used to let the device take down any connection to
    // bonded devices.
    mp_dfu->evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE);

    // Disconnect from the device to enable Service Changed on next connection.
    // (Stored in Peer Manager persistent storage for next bootup).
    err_code = sd_ble_gap_disconnect(mp_dfu->conn_handle,
                                     BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed to disconnect GAP connection");
        return err_code;
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected peer");
    }

    // Store the number of peers for which Peer Manager is expected to successfully write events.
    mp_dfu->peers_count = peer_id_n_ids();

    // Set local database changed to get Service Changed indication for all bonded peers
    // on next bootup (either because of a successful or aborted DFU).
    gscm_local_database_has_changed();

    return err_code;
}

#endif //defined(NRF_DFU_BOTTONLESS_SUPPORT_BOND) && (NRF_DFU_BOTTONLESS_SUPPORT_BOND == 1)

