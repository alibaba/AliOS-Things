/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
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
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(PEER_MANAGER)
#include "ble_err.h"
#include "peer_manager.h"
#include <string.h>
#include "security_manager.h"
#include "security_dispatcher.h"
#include "gatt_cache_manager.h"
#include "gatts_cache_manager.h"
#include "peer_database.h"
#include "peer_data_storage.h"
#include "id_manager.h"
#include "ble_conn_state.h"
#include "peer_manager_internal.h"
#include "nrf_sdh_ble.h"


#define MODULE_INITIALIZED      (m_module_initialized)                  /**< Macro indicating whether the module has been initialized properly. */


static bool                          m_module_initialized;              /**< Whether or not @ref pm_init has been called successfully. */
static bool                          m_peer_rank_initialized;           /**< Whether or not @ref rank_init has been called successfully. */
static bool                          m_deleting_all;                    /**< True from when @ref pm_peers_delete is called until all peers have been deleted. */
static pm_store_token_t              m_peer_rank_token;                 /**< The store token of an ongoing peer rank update via a call to @ref pm_peer_rank_highest. If @ref PM_STORE_TOKEN_INVALID, there is no ongoing update. */
static uint32_t                      m_current_highest_peer_rank;       /**< The current highest peer rank. Used by @ref pm_peer_rank_highest. */
static pm_peer_id_t                  m_highest_ranked_peer;             /**< The peer with the highest peer rank. Used by @ref pm_peer_rank_highest. */
static pm_evt_handler_t              m_evt_handlers[PM_MAX_REGISTRANTS];/**< The subscribers to Peer Manager events, as registered through @ref pm_register. */
static uint8_t                       m_n_registrants;                   /**< The number of event handlers registered through @ref pm_register. */
static ble_conn_state_user_flag_id_t m_bonded_flag_id;                  /**< The flag ID for which connections are with a peer with which we are bonded. */


/**@brief Function for sending a Peer Manager event to all subscribers.
 *
 * @param[in]  p_pm_evt  The event to send.
 */
static void evt_send(pm_evt_t const * p_pm_evt)
{
    for (int i = 0; i < m_n_registrants; i++)
    {
        m_evt_handlers[i](p_pm_evt);
    }
}


/**@brief Function for initializing peer rank static variables.
 */
static void rank_vars_update(void)
{
    ret_code_t err_code = pm_peer_ranks_get(&m_highest_ranked_peer,
                                            &m_current_highest_peer_rank,
                                            NULL,
                                            NULL);

    m_peer_rank_initialized = ((err_code == NRF_SUCCESS) || (err_code == NRF_ERROR_NOT_FOUND));
}


/**@brief Event handler for events from the Peer Database module.
 *        This handler is extern in the Peer Database module.
 *
 * @param[in]  p_pdb_evt  The incoming Peer Database event.
 */
void pm_pdb_evt_handler(pm_evt_t * p_pdb_evt)
{
    bool send_evt = true;

    p_pdb_evt->conn_handle = im_conn_handle_get(p_pdb_evt->peer_id);

    switch (p_pdb_evt->evt_id)
    {
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            if (p_pdb_evt->params.peer_data_update_succeeded.action == PM_PEER_DATA_OP_UPDATE)
            {
                if (   (m_peer_rank_token != PM_STORE_TOKEN_INVALID)
                    && (m_peer_rank_token == p_pdb_evt->params.peer_data_update_succeeded.token))
                {
                    m_peer_rank_token     = PM_STORE_TOKEN_INVALID;
                    m_highest_ranked_peer = p_pdb_evt->peer_id;

                    p_pdb_evt->params.peer_data_update_succeeded.token = PM_STORE_TOKEN_INVALID;
                }
                else if (    m_peer_rank_initialized
                         && (p_pdb_evt->peer_id == m_highest_ranked_peer)
                         && (p_pdb_evt->params.peer_data_update_succeeded.data_id
                                                == PM_PEER_DATA_ID_PEER_RANK))
                {
                    // Update peer rank variable if highest ranked peer has changed its rank.
                    rank_vars_update();
                }
            }
            else if (p_pdb_evt->params.peer_data_update_succeeded.action == PM_PEER_DATA_OP_DELETE)
            {
                if (    m_peer_rank_initialized
                    && (p_pdb_evt->peer_id == m_highest_ranked_peer)
                    && (p_pdb_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_PEER_RANK))
                {
                    // Update peer rank variable if highest ranked peer has deleted its rank.
                    rank_vars_update();
                }
            }
            break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            if (p_pdb_evt->params.peer_data_update_succeeded.action == PM_PEER_DATA_OP_UPDATE)
            {
                if (   (m_peer_rank_token != PM_STORE_TOKEN_INVALID)
                    && (m_peer_rank_token == p_pdb_evt->params.peer_data_update_failed.token))
                {
                    m_peer_rank_token            = PM_STORE_TOKEN_INVALID;
                    m_current_highest_peer_rank -= 1;

                    p_pdb_evt->params.peer_data_update_succeeded.token = PM_STORE_TOKEN_INVALID;
                }
            }
            break;

        case PM_EVT_PEER_DELETE_SUCCEEDED:
            // Check that no peers marked for deletion are left.
            if (m_deleting_all
                && (pdb_next_peer_id_get(PM_PEER_ID_INVALID) == PM_PEER_ID_INVALID)
                && (pdb_next_deleted_peer_id_get(PM_PEER_ID_INVALID) == PM_PEER_ID_INVALID))
            {
                // pm_peers_delete() has been called and this is the last peer to be deleted.
                m_deleting_all = false;

                pm_evt_t pm_delete_all_evt;
                memset(&pm_delete_all_evt, 0, sizeof(pm_evt_t));
                pm_delete_all_evt.evt_id      = PM_EVT_PEERS_DELETE_SUCCEEDED;
                pm_delete_all_evt.peer_id     = PM_PEER_ID_INVALID;
                pm_delete_all_evt.conn_handle = BLE_CONN_HANDLE_INVALID;

                send_evt = false;

                // Forward the event to all registered Peer Manager event handlers.
                evt_send(p_pdb_evt); // Ensure that PEER_DELETE_SUCCEEDED arrives before PEERS_DELETE_SUCCEEDED.
                evt_send(&pm_delete_all_evt);
            }

            if (m_peer_rank_initialized && (p_pdb_evt->peer_id == m_highest_ranked_peer))
            {
                // Update peer rank variable if highest ranked peer has been deleted.
                rank_vars_update();
            }
            break;

        case PM_EVT_PEER_DELETE_FAILED:
            if (m_deleting_all)
            {
                // pm_peers_delete() was called and has thus failed.

                m_deleting_all = false;

                pm_evt_t pm_delete_all_evt;
                memset(&pm_delete_all_evt, 0, sizeof(pm_evt_t));
                pm_delete_all_evt.evt_id      = PM_EVT_PEERS_DELETE_FAILED;
                pm_delete_all_evt.peer_id     = PM_PEER_ID_INVALID;
                pm_delete_all_evt.conn_handle = BLE_CONN_HANDLE_INVALID;
                pm_delete_all_evt.params.peers_delete_failed_evt.error
                                              = p_pdb_evt->params.peer_delete_failed.error;

                send_evt = false;

                // Forward the event to all registered Peer Manager event handlers.
                evt_send(p_pdb_evt); // Ensure that PEER_DELETE_FAILED arrives before PEERS_DELETE_FAILED.
                evt_send(&pm_delete_all_evt);
            }
            break;

        default:
            // Do nothing.
            break;
    }

    if (send_evt)
    {
        // Forward the event to all registered Peer Manager event handlers.
        evt_send(p_pdb_evt);
    }
}


/**@brief Event handler for events from the Security Manager module.
 *        This handler is extern in the Security Manager module.
 *
 * @param[in]  p_sm_evt  The incoming Security Manager event.
 */
void pm_sm_evt_handler(pm_evt_t * p_sm_evt)
{
    VERIFY_PARAM_NOT_NULL_VOID(p_sm_evt);

    switch (p_sm_evt->evt_id)
    {
        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            bool bonded = p_sm_evt->params.conn_sec_succeeded.procedure
                                == PM_LINK_SECURED_PROCEDURE_BONDING;
            ble_conn_state_user_flag_set(p_sm_evt->conn_handle, m_bonded_flag_id, bonded);
            break;
        }

        default:
            /* No action */
            break;
    }

    // Forward the event to all registered Peer Manager event handlers.
    evt_send(p_sm_evt);
}


/**@brief Event handler for events from the GATT Cache Manager module.
 *        This handler is extern in GATT Cache Manager.
 *
 * @param[in]  p_gcm_evt  The incoming GATT Cache Manager event.
 */
void pm_gcm_evt_handler(pm_evt_t * p_gcm_evt)
{
    // Forward the event to all registered Peer Manager event handlers.
    evt_send(p_gcm_evt);
}


/**@brief Event handler for events from the ID Manager module.
 *        This function is registered in the ID Manager.
 *
 * @param[in]  p_im_evt  The incoming ID Manager event.
 */
void pm_im_evt_handler(pm_evt_t * p_im_evt)
{
    switch (p_im_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
            ble_conn_state_user_flag_set(p_im_evt->conn_handle, m_bonded_flag_id, true);
            break;
        default:
            /* No action. */
            break;
    }

    // Forward the event to all registered Peer Manager event handlers.
    evt_send(p_im_evt);
}

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt       Event received from the BLE stack.
 * @param[in]   p_context       Context.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    VERIFY_MODULE_INITIALIZED_VOID();

    im_ble_evt_handler(p_ble_evt);
    sm_ble_evt_handler(p_ble_evt);
    gcm_ble_evt_handler(p_ble_evt);
}

NRF_SDH_BLE_OBSERVER(m_ble_evt_observer, PM_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);


/**@brief Function for resetting the internal state of this module.
 */
static void internal_state_reset()
{
    m_highest_ranked_peer = PM_PEER_ID_INVALID;
    m_peer_rank_token     = PM_STORE_TOKEN_INVALID;
    m_bonded_flag_id      = BLE_CONN_STATE_USER_FLAG_INVALID;
}


ret_code_t pm_init(void)
{
    ret_code_t err_code;

    err_code = pds_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    err_code = pdb_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    err_code = sm_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    err_code = smd_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    err_code = gcm_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    err_code = gscm_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    err_code = im_init();
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

    internal_state_reset();

    m_bonded_flag_id = ble_conn_state_user_flag_acquire();
    if (m_bonded_flag_id == BLE_CONN_STATE_USER_FLAG_INVALID)
    {
        return NRF_ERROR_INTERNAL;
    }

    m_peer_rank_initialized = false;
    m_module_initialized    = true;

    return NRF_SUCCESS;
}


ret_code_t pm_register(pm_evt_handler_t event_handler)
{
    VERIFY_MODULE_INITIALIZED();

    if (m_n_registrants >= PM_MAX_REGISTRANTS)
    {
        return NRF_ERROR_NO_MEM;
    }

    m_evt_handlers[m_n_registrants] = event_handler;
    m_n_registrants += 1;

    return NRF_SUCCESS;
}


ret_code_t pm_sec_params_set(ble_gap_sec_params_t * p_sec_params)
{
    VERIFY_MODULE_INITIALIZED();

    ret_code_t err_code;

    err_code = sm_sec_params_set(p_sec_params);

    // NRF_ERROR_INVALID_PARAM if parameters are invalid,
    // NRF_SUCCESS             otherwise.
    return err_code;
}


ret_code_t pm_conn_secure(uint16_t conn_handle, bool force_repairing)
{
    VERIFY_MODULE_INITIALIZED();

    ret_code_t err_code;

    err_code = sm_link_secure(conn_handle, force_repairing);

    return err_code;
}


void pm_conn_sec_config_reply(uint16_t conn_handle, pm_conn_sec_config_t * p_conn_sec_config)
{
    if (p_conn_sec_config != NULL)
    {
        sm_conn_sec_config_reply(conn_handle, p_conn_sec_config);
    }
}


ret_code_t pm_conn_sec_params_reply(uint16_t               conn_handle,
                                    ble_gap_sec_params_t * p_sec_params,
                                    void           const * p_context)
{
    VERIFY_MODULE_INITIALIZED();

    return sm_sec_params_reply(conn_handle, p_sec_params, p_context);
}


void pm_local_database_has_changed(void)
{
    VERIFY_MODULE_INITIALIZED_VOID();
    gcm_local_database_has_changed();
}


ret_code_t pm_id_addr_set(ble_gap_addr_t const * p_addr)
{
    VERIFY_MODULE_INITIALIZED();
    return im_id_addr_set(p_addr);
}


ret_code_t pm_id_addr_get(ble_gap_addr_t * p_addr)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_addr);
    return im_id_addr_get(p_addr);
}


ret_code_t pm_privacy_set(pm_privacy_params_t const * p_privacy_params)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_privacy_params);
    return im_privacy_set(p_privacy_params);
}


ret_code_t pm_privacy_get(pm_privacy_params_t * p_privacy_params)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_privacy_params);
    VERIFY_PARAM_NOT_NULL(p_privacy_params->p_device_irk);
    return im_privacy_get(p_privacy_params);
}


bool pm_address_resolve(ble_gap_addr_t const * p_addr, ble_gap_irk_t const * p_irk)
{
    VERIFY_MODULE_INITIALIZED();

    if ((p_addr == NULL) || (p_irk == NULL))
    {
        return false;
    }
    else
    {
        return im_address_resolve(p_addr, p_irk);
    }
}


ret_code_t pm_whitelist_set(pm_peer_id_t const * p_peers,
                            uint32_t             peer_cnt)
{
    VERIFY_MODULE_INITIALIZED();
    return im_whitelist_set(p_peers, peer_cnt);
}


ret_code_t pm_whitelist_get(ble_gap_addr_t * p_addrs,
                            uint32_t       * p_addr_cnt,
                            ble_gap_irk_t  * p_irks,
                            uint32_t       * p_irk_cnt)
{
    VERIFY_MODULE_INITIALIZED();

    if (((p_addrs == NULL) && (p_irks     == NULL)) ||
        ((p_addrs != NULL) && (p_addr_cnt == NULL)) ||
        ((p_irks  != NULL) && (p_irk_cnt  == NULL)))
    {
        // The buffers can't be both NULL, and if a buffer is provided its size must be specified.
        return NRF_ERROR_NULL;
    }

    return im_whitelist_get(p_addrs, p_addr_cnt, p_irks, p_irk_cnt);
}


ret_code_t pm_device_identities_list_set(pm_peer_id_t const * p_peers,
                                         uint32_t             peer_cnt)
{
    VERIFY_MODULE_INITIALIZED();
    return im_device_identities_list_set(p_peers, peer_cnt);
}


ret_code_t pm_conn_sec_status_get(uint16_t conn_handle, pm_conn_sec_status_t * p_conn_sec_status)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_conn_sec_status);

    ble_conn_state_status_t status = ble_conn_state_status(conn_handle);

    if (status == BLE_CONN_STATUS_INVALID)
    {
        return BLE_ERROR_INVALID_CONN_HANDLE;
    }

    p_conn_sec_status->connected      = (status == BLE_CONN_STATUS_CONNECTED);
    p_conn_sec_status->bonded         = ble_conn_state_user_flag_get(conn_handle, m_bonded_flag_id);
    p_conn_sec_status->encrypted      = ble_conn_state_encrypted(conn_handle);
    p_conn_sec_status->mitm_protected = ble_conn_state_mitm_protected(conn_handle);
    return NRF_SUCCESS;
}


ret_code_t pm_lesc_public_key_set(ble_gap_lesc_p256_pk_t * p_public_key)
{
    VERIFY_MODULE_INITIALIZED();
    return sm_lesc_public_key_set(p_public_key);
}


ret_code_t pm_conn_handle_get(pm_peer_id_t peer_id, uint16_t * p_conn_handle)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_conn_handle);
    *p_conn_handle = im_conn_handle_get(peer_id);
    return NRF_SUCCESS;
}


ret_code_t pm_peer_id_get(uint16_t conn_handle, pm_peer_id_t * p_peer_id)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_peer_id);
    *p_peer_id = im_peer_id_get_by_conn_handle(conn_handle);
    return NRF_SUCCESS;
}


uint32_t pm_peer_count(void)
{
    if (!MODULE_INITIALIZED)
    {
        return 0;
    }
    return pdb_n_peers();
}


pm_peer_id_t pm_next_peer_id_get(pm_peer_id_t prev_peer_id)
{
    if (!MODULE_INITIALIZED)
    {
        return PM_PEER_ID_INVALID;
    }
    return pdb_next_peer_id_get(prev_peer_id);
}


ret_code_t pm_peer_data_load(pm_peer_id_t       peer_id,
                             pm_peer_data_id_t  data_id,
                             void             * p_data,
                             uint16_t         * p_length)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_data);
    VERIFY_PARAM_NOT_NULL(p_length);
    if (ALIGN_NUM(4, *p_length) != *p_length)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    pm_peer_data_t peer_data;
    memset(&peer_data, 0, sizeof(peer_data));
    peer_data.length_words = BYTES_TO_WORDS(*p_length);
    peer_data.data_id      = data_id;
    peer_data.p_all_data   = p_data;

    ret_code_t err_code = pdb_peer_data_load(peer_id, data_id, &peer_data);

    *p_length = peer_data.length_words * BYTES_PER_WORD;

    return err_code;
}


ret_code_t pm_peer_data_bonding_load(pm_peer_id_t             peer_id,
                                     pm_peer_data_bonding_t * p_data)
{
    uint16_t length = sizeof(pm_peer_data_bonding_t);
    return pm_peer_data_load(peer_id,
                             PM_PEER_DATA_ID_BONDING,
                             p_data,
                             &length);
}


ret_code_t pm_peer_data_remote_db_load(pm_peer_id_t        peer_id,
                                       ble_gatt_db_srv_t * p_data,
                                       uint16_t          * p_length)
{
    return pm_peer_data_load(peer_id,
                             PM_PEER_DATA_ID_GATT_REMOTE,
                             p_data,
                             p_length);
}


ret_code_t pm_peer_data_app_data_load(pm_peer_id_t peer_id,
                                      void       * p_data,
                                      uint16_t   * p_length)
{
    return pm_peer_data_load(peer_id,
                             PM_PEER_DATA_ID_APPLICATION,
                             p_data,
                             p_length);
}


ret_code_t pm_peer_data_store(pm_peer_id_t       peer_id,
                              pm_peer_data_id_t  data_id,
                              void       const * p_data,
                              uint16_t           length,
                              pm_store_token_t * p_token)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_data);
    if (ALIGN_NUM(4, length) != length)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    pm_peer_data_flash_t peer_data;
    memset(&peer_data, 0, sizeof(peer_data));
    peer_data.length_words = BYTES_TO_WORDS(length);
    peer_data.data_id      = data_id;
    peer_data.p_all_data   = p_data;

    return pdb_raw_store(peer_id, &peer_data, p_token);
}


ret_code_t pm_peer_data_bonding_store(pm_peer_id_t                   peer_id,
                                      pm_peer_data_bonding_t const * p_data,
                                      pm_store_token_t             * p_token)
{
    return pm_peer_data_store(peer_id,
                              PM_PEER_DATA_ID_BONDING,
                              p_data,
                              ALIGN_NUM(4, sizeof(pm_peer_data_bonding_t)),
                              p_token);
}


ret_code_t pm_peer_data_remote_db_store(pm_peer_id_t              peer_id,
                                        ble_gatt_db_srv_t const * p_data,
                                        uint16_t                  length,
                                        pm_store_token_t        * p_token)
{
    return pm_peer_data_store(peer_id,
                              PM_PEER_DATA_ID_GATT_REMOTE,
                              p_data,
                              length,
                              p_token);
}


ret_code_t pm_peer_data_app_data_store(pm_peer_id_t       peer_id,
                                       void       const * p_data,
                                       uint16_t           length,
                                       pm_store_token_t * p_token)
{
    return pm_peer_data_store(peer_id,
                              PM_PEER_DATA_ID_APPLICATION,
                              p_data,
                              length,
                              p_token);
}


ret_code_t pm_peer_data_delete(pm_peer_id_t peer_id, pm_peer_data_id_t data_id)
{
    VERIFY_MODULE_INITIALIZED();

    if (data_id == PM_PEER_DATA_ID_BONDING)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    return pdb_clear(peer_id, data_id);
}


ret_code_t pm_peer_new(pm_peer_id_t           * p_new_peer_id,
                       pm_peer_data_bonding_t * p_bonding_data,
                       pm_store_token_t       * p_token)
{
    ret_code_t           err_code;
    pm_peer_id_t         peer_id;
    pm_peer_data_flash_t peer_data;

    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_bonding_data);
    VERIFY_PARAM_NOT_NULL(p_new_peer_id);

    memset(&peer_data, 0, sizeof(pm_peer_data_flash_t));

    // Search through existing bonds to look for a duplicate.
    pds_peer_data_iterate_prepare();

    // @note emdi: should maybe use a critical section, since data is not copied while iterating.
    while (pds_peer_data_iterate(PM_PEER_DATA_ID_BONDING, &peer_id, &peer_data))
    {
        if (im_is_duplicate_bonding_data(p_bonding_data, peer_data.p_bonding_data))
        {
            *p_new_peer_id = peer_id;
            return NRF_SUCCESS;
        }
    }

    // If no duplicate data is found, prepare to write a new bond to flash.

    *p_new_peer_id = pdb_peer_allocate();

    if (*p_new_peer_id == PM_PEER_ID_INVALID)
    {
        return NRF_ERROR_NO_MEM;
    }

    memset(&peer_data, 0, sizeof(pm_peer_data_flash_t));

    peer_data.data_id        = PM_PEER_DATA_ID_BONDING;
    peer_data.p_bonding_data = p_bonding_data;
    peer_data.length_words   = BYTES_TO_WORDS(sizeof(pm_peer_data_bonding_t));

    err_code = pdb_raw_store(*p_new_peer_id, &peer_data, p_token);

    if (err_code != NRF_SUCCESS)
    {
        if (im_peer_free(*p_new_peer_id) != NRF_SUCCESS)
        {
            return NRF_ERROR_INTERNAL;
        }

        // NRF_ERROR_STORAGE_FULL, if no space in flash.
        // NRF_ERROR_BUSY,         if flash filesystem was busy.
        // NRF_ERROR_INTENRAL,     on internal error.
        return err_code;
    }

    return NRF_SUCCESS;
}


ret_code_t pm_peer_delete(pm_peer_id_t peer_id)
{
    VERIFY_MODULE_INITIALIZED();

    return im_peer_free(peer_id);
}


ret_code_t pm_peers_delete(void)
{
    VERIFY_MODULE_INITIALIZED();

    m_deleting_all = true;

    pm_peer_id_t current_peer_id = pdb_next_peer_id_get(PM_PEER_ID_INVALID);

    if (current_peer_id == PM_PEER_ID_INVALID)
    {
        // No peers bonded.
        m_deleting_all = false;

        pm_evt_t pm_delete_all_evt;
        memset(&pm_delete_all_evt, 0, sizeof(pm_evt_t));
        pm_delete_all_evt.evt_id      = PM_EVT_PEERS_DELETE_SUCCEEDED;
        pm_delete_all_evt.peer_id     = PM_PEER_ID_INVALID;
        pm_delete_all_evt.conn_handle = BLE_CONN_HANDLE_INVALID;

        evt_send(&pm_delete_all_evt);
    }

    while (current_peer_id != PM_PEER_ID_INVALID)
    {
        ret_code_t err_code = pm_peer_delete(current_peer_id);
        if (err_code != NRF_SUCCESS)
        {
            return NRF_ERROR_INTERNAL;
        }

        current_peer_id = pdb_next_peer_id_get(current_peer_id);
    }

    return NRF_SUCCESS;
}


ret_code_t pm_peer_ranks_get(pm_peer_id_t * p_highest_ranked_peer,
                             uint32_t     * p_highest_rank,
                             pm_peer_id_t * p_lowest_ranked_peer,
                             uint32_t     * p_lowest_rank)
{
    VERIFY_MODULE_INITIALIZED();

    pm_peer_id_t         peer_id      = pdb_next_peer_id_get(PM_PEER_ID_INVALID);
    uint32_t             peer_rank    = 0;
    //lint -save -e65 -e64
    pm_peer_data_t       peer_data    = {.length_words = BYTES_TO_WORDS(sizeof(peer_rank)),
                                         .p_peer_rank  = &peer_rank};
    //lint -restore
    ret_code_t           err_code     = pdb_peer_data_load(peer_id, PM_PEER_DATA_ID_PEER_RANK, &peer_data);
    uint32_t             highest_rank = 0;
    uint32_t             lowest_rank  = 0xFFFFFFFF;
    pm_peer_id_t         highest_ranked_peer = PM_PEER_ID_INVALID;
    pm_peer_id_t         lowest_ranked_peer  = PM_PEER_ID_INVALID;

    if (err_code == NRF_ERROR_INVALID_PARAM)
    {
        // No peer IDs exist.
        return NRF_ERROR_NOT_FOUND;
    }

    while ((err_code == NRF_SUCCESS) || (err_code == NRF_ERROR_NOT_FOUND))
    {
        if (err_code == NRF_ERROR_NOT_FOUND)
        {
            peer_rank = 0;
        }
        if (peer_rank >= highest_rank)
        {
            highest_rank      = peer_rank;
            highest_ranked_peer = peer_id;
        }
        if (peer_rank < lowest_rank)
        {
            lowest_rank      = peer_rank;
            lowest_ranked_peer = peer_id;
        }
        peer_id = pdb_next_peer_id_get(peer_id);
        err_code = pdb_peer_data_load(peer_id, PM_PEER_DATA_ID_PEER_RANK, &peer_data);
    }
    if (peer_id == PM_PEER_ID_INVALID)
    {
        err_code = NRF_SUCCESS;
        if (p_highest_ranked_peer != NULL)
        {
            *p_highest_ranked_peer = highest_ranked_peer;
        }
        if (p_highest_rank != NULL)
        {
            *p_highest_rank = highest_rank;
        }
        if (p_lowest_ranked_peer != NULL)
        {
            *p_lowest_ranked_peer = lowest_ranked_peer;
        }
        if (p_lowest_rank != NULL)
        {
            *p_lowest_rank = lowest_rank;
        }
    }
    else
    {
        err_code = NRF_ERROR_INTERNAL;
    }
    return err_code;
}


/**@brief Function for initializing peer rank functionality.
 */
static void rank_init(void)
{
    rank_vars_update();
}


ret_code_t pm_peer_rank_highest(pm_peer_id_t peer_id)
{
    VERIFY_MODULE_INITIALIZED();

    ret_code_t err_code;
    //lint -save -e65 -e64
    pm_peer_data_flash_t peer_data   = {.length_words = BYTES_TO_WORDS(sizeof(m_current_highest_peer_rank)),
                                        .data_id      = PM_PEER_DATA_ID_PEER_RANK,
                                        .p_peer_rank  = &m_current_highest_peer_rank};
    //lint -restore


    if (!m_peer_rank_initialized)
    {
        rank_init();
    }

    if (!m_peer_rank_initialized || (m_peer_rank_token != PM_STORE_TOKEN_INVALID))
    {
        err_code = NRF_ERROR_BUSY;
    }
    else
    {
        if ((peer_id == m_highest_ranked_peer) && (m_current_highest_peer_rank > 0))
        {
            pm_evt_t pm_evt;

            // The reported peer is already regarded as highest (provided it has an index at all)
            err_code = NRF_SUCCESS;

            memset(&pm_evt, 0, sizeof(pm_evt));
            pm_evt.evt_id      = PM_EVT_PEER_DATA_UPDATE_SUCCEEDED;
            pm_evt.conn_handle = im_conn_handle_get(peer_id);
            pm_evt.peer_id     = peer_id;
            pm_evt.params.peer_data_update_succeeded.data_id       = PM_PEER_DATA_ID_PEER_RANK;
            pm_evt.params.peer_data_update_succeeded.action        = PM_PEER_DATA_OP_UPDATE;
            pm_evt.params.peer_data_update_succeeded.token         = PM_STORE_TOKEN_INVALID;
            pm_evt.params.peer_data_update_succeeded.flash_changed = false;

            evt_send(&pm_evt);
        }
        else
        {
            m_current_highest_peer_rank += 1;
            err_code = pdb_raw_store(peer_id, &peer_data, &m_peer_rank_token);
            if (err_code != NRF_SUCCESS)
            {
                m_peer_rank_token    = PM_STORE_TOKEN_INVALID;
                m_current_highest_peer_rank -= 1;
                {
                if ((err_code != NRF_ERROR_BUSY) && (err_code != NRF_ERROR_STORAGE_FULL))
                    err_code = NRF_ERROR_INTERNAL;
                }
            }
        }
    }
    return err_code;
}
#endif // NRF_MODULE_ENABLED(PEER_MANAGER)
