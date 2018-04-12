/*
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 * 
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ali_core.h"
#include "ble_ais.h"
#include "ali_transport.h"
#include "ali_auth.h"
#include "ali_gap.h"
#include "ali_ext.h"
#include "ali_ota.h"
#include "ali_error_src.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include <stdbool.h>
#include <bluetooth/conn.h>
#include <aos/aos.h>

#define MODULE_INITIALIZED      (p_ali->is_initialized)

#define FMSK_BLUETOOTH_VER_Pos  0                                           /**< Offset of bluetooth version in FMSK (see specification v1.0.5, ch. 2.3). */
#define FMSK_OTA_Pos            2                                           /**< Offset of OTA firmware upgrade support in FMSK (see specification v1.0.5, ch. 2.3). */
#define FMSK_SECURITY_Pos       3                                           /**< Offset of security support in FMSK (see specification v1.0.5, ch. 2.3). */
#define FMSK_SECRET_TYPE_Pos    4                                           /**< Offset of encryption secret type in FMSK (see specification v1.0.5, ch. 2.3). */

#define MAC_ASCII_LEN           6                                           /**< Length of MAC, which must be exact.  */
#define MAX_SECRET_LEN          ALI_AUTH_SECRET_LEN_MAX                     /**< Maximum length of secret. */

#define TX_BUFF_LEN             (ALI_MAX_SUPPORTED_MTU - 3)                 /**< Transport layer: Tx buffer size (see specification v1.0.4, ch.5.9). */
#define RX_BUFF_LEN             ALI_TRANSPORT_MAX_RX_DATA_LEN               /**< Transport layer: Rx buffer size (see specification v1.0.4, ch.5.9). */

#define AUTH_TIMEOUT            10000                                       /**< Authentication timeout in number of ms (see specification v1.0.4, ch.4.3). */

#define SET_U16_LE(data, val)                                \
{                                                            \
    *(uint8_t *)(data)       = (uint8_t)(val & 0xFF);        \
    *((uint8_t *)(data) + 1) = (uint8_t)((val >> 8) & 0xFF); \
}

#define SET_U32_LE(data, val)                                 \
{                                                             \
    *(uint8_t *)(data)       = (uint8_t)(val & 0xFF);         \
    *((uint8_t *)(data) + 1) = (uint8_t)((val >> 8) & 0xFF);  \
    *((uint8_t *)(data) + 2) = (uint8_t)((val >> 16) & 0xFF); \
    *((uint8_t *)(data) + 3) = (uint8_t)((val >> 24) & 0xFF); \
}

extern struct bt_conn *g_conn;

static uint8_t const m_valid_rx_cmd[10]                         /**< Valid command for Rx. */
    = {
        ALI_CMD_CTRL,                ALI_CMD_QUERY,               ALI_CMD_EXT_DOWN,
        ALI_CMD_AUTH_REQ,            ALI_CMD_AUTH_CFM,
        ALI_CMD_FW_VERSION_REQ,      ALI_CMD_FW_UPGRADE_REQ,
        ALI_CMD_FW_GET_INIT_FW_SIZE, ALI_CMD_FW_XFER_FINISH,
        ALI_CMD_FW_DATA
    };


/**@brief Function to check whether the received command is a valid command. */
static bool is_valid_command(uint8_t cmd)
{
    for (uint8_t i = 0; i < sizeof(m_valid_rx_cmd); i++)
    {
        if (cmd == m_valid_rx_cmd[i])
        {
            return true;
        }
    }

    return false;
}


/**@brief Notify error to higher layer. */
static void notify_error (ali_t * p_ali, uint32_t src, uint32_t err_code)
{
    ali_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_ali->event_handler(p_ali->p_evt_context, &evt);
}


/**@brief Notify event without data to higher layer.
 * @note  Only for ALI_EVT_CONNECTED, ALI_EVT_DISCONNECTED and ALI_EVT_AUTHENTICATED. */
void notify_evt_no_data (ali_t * p_ali, ali_evt_type_t evt_type)
{
    ali_event_t evt;

    /* send event to higher layer. */
    evt.type = evt_type;
    p_ali->event_handler(p_ali->p_evt_context, &evt);
}


/**@brief Notify received data to higher layer. */
static void notify_ctrl_data(ali_t * p_ali, uint8_t * data, uint16_t len)
{
    ali_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_EVT_CTRL_RECEIVED;
    evt.data.rx_data.p_data = data;
    evt.data.rx_data.length = len;
    p_ali->event_handler(p_ali->p_evt_context, &evt);
}


/**@brief Notify received data to higher layer. */
static void notify_query_data(ali_t * p_ali, uint8_t * data, uint16_t len)
{
    ali_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_EVT_QUERY_RECEIVED;
    evt.data.rx_data.p_data = data;
    evt.data.rx_data.length = len;
    p_ali->event_handler(p_ali->p_evt_context, &evt);
}


/**@brief Endian swapping. */
static void endian_swap (uint8_t * buff, uint8_t * data, uint16_t len)
{
    VERIFY_PARAM_NOT_NULL_VOID(buff);
    VERIFY_PARAM_NOT_NULL_VOID(data);

    for (uint8_t i = 0; i < len; i++)
    {
        buff[i] = data[len-i-1];
    }
}


/**@brief Build manufacturer specific advertising data. */
static void create_manuf_spec_adv_data (ali_t * p_ali, uint32_t model_id, uint8_t * mac_bin,
                                        bool enable_auth, bool enable_ota, bool secret_per_device)
{
    uint16_t i;
    uint8_t  fmsk;

    // Company ID (CID)
    SET_U16_LE(p_ali->manuf_spec_adv_data, ALI_COMPANY_ID);
    i = sizeof(uint16_t);

    // Protocol ID (PID)
    p_ali->manuf_spec_adv_data[i++] = ALI_PROTOCOL_ID;

    // Function mask (FMSK)
    fmsk = ALI_BLUETOOTH_VER << FMSK_BLUETOOTH_VER_Pos;
    if (enable_auth)
    {
        fmsk |= 1 << FMSK_SECURITY_Pos;
    }
    if (enable_ota)
    {
        fmsk |= 1 << FMSK_OTA_Pos;
    }
    if (secret_per_device)
    {
        fmsk |= 1 << FMSK_SECRET_TYPE_Pos;
    }
    p_ali->manuf_spec_adv_data[i++] = fmsk;

    // Model ID (MID)
    SET_U32_LE(p_ali->manuf_spec_adv_data + i, model_id);
    i += sizeof(uint32_t);

    for (int j = 5; j >= 0; i++, j--)
    {
        p_ali->manuf_spec_adv_data[i] = mac_bin[j];
    }
    p_ali->manuf_spec_adv_data_len = i;
}


/**@brief Function to send command by notification. */
static uint32_t tx_func_notify(ali_t * p_ali, uint8_t cmd, uint8_t * p_data, uint16_t length)
{
    return ali_transport_send(&p_ali->transport, ALI_TRANSPORT_TX_TYPE_NOTIFY,
                              cmd, p_data, length);
}


/**@brief Function to send command by indication. */
static uint32_t tx_func_indicate(ali_t * p_ali, uint8_t cmd, uint8_t * p_data, uint16_t length)
{
    return ali_transport_send(&p_ali->transport, ALI_TRANSPORT_TX_TYPE_INDICATE,
                              cmd, p_data, length);
}


static void disconnect_ble(void *arg)
{
    (void *)arg;
    if (!g_conn) return;
    bt_conn_disconnect(g_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}


/**@brief Authentication module: event handler function. */
static void auth_event_handler (ali_t * p_ali, ali_auth_event_t * p_event)
{
    uint32_t err_code;

    switch (p_event->type)
    {
        case ALI_AUTH_EVT_DONE:
#ifdef CONFIG_AIS_OTA
            ali_ota_on_auth(&p_ali->ota, p_event->data.auth_done.result);
#endif
            ali_ext_on_auth(&p_ali->ext, p_event->data.auth_done.result);
            /* Disconnect if authentication failed. */
            if (!p_event->data.auth_done.result
                 && p_ali->conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                disconnect_ble(NULL);
            }
            else
            {
                notify_evt_no_data (p_ali, ALI_EVT_AUTHENTICATED);
            }
            break;

        case ALI_AUTH_EVT_KEY_UPDATE:
            err_code = ali_transport_set_key(&p_ali->transport, p_event->data.new_key.p_sec_key);
            if (err_code != NRF_SUCCESS)
            {
                notify_error(p_ali, ALI_ERROR_SRC_CORE_CANNOT_SET_KEY, err_code);
            }
            break;

        case ALI_AUTH_EVT_ERROR:
            if (p_event->data.error.err_code == NRF_ERROR_TIMEOUT)
            {
                disconnect_ble(NULL);
            }
            notify_error(p_ali, p_event->data.error.source, p_event->data.error.err_code);
            break;

        default:
            break;
    }
}


/**@brief GAP module: event handler function. */
static void gap_event_handler (ali_t * p_ali, ali_gap_event_t * p_event)
{
    switch (p_event->type)
    {
        case ALI_GAP_EVT_TX_DONE:
            notify_evt_no_data (p_ali, ALI_EVT_TX_DONE);
            break;

        case ALI_GAP_EVT_ON_CTRL:
            notify_ctrl_data(p_ali, p_event->data.rx_data.p_data, p_event->data.rx_data.length);
            break;

        case ALI_GAP_EVT_ON_QUERY:
            notify_query_data(p_ali, p_event->data.rx_data.p_data, p_event->data.rx_data.length);
            break;

        case ALI_GAP_EVT_ERROR:
            notify_error(p_ali, p_event->data.error.source, p_event->data.error.err_code);
            break;

        default:
            break;
    }
}


/**@brief OTA firmware upgrade module: event handler function. */
static void ota_event_handler (ali_t * p_ali, ali_ota_event_t * p_event)
{
    switch (p_event->type)
    {
        case ALI_OTA_EVT_NEW_FW:
            notify_evt_no_data (p_ali, ALI_EVT_NEW_FIRMWARE);
            break;

        case ALI_OTA_EVT_ERROR:
            notify_error(p_ali, p_event->data.error.source, p_event->data.error.err_code);
            break;

        default:
            break;
    }
}


/**@brief Transport layer: event handler function. */
static void transport_event_handler (ali_t * p_ali, ali_transport_event_t * p_event)
{
    bool send_err = false;
    uint32_t err_code;

    switch (p_event->type)
    {
        case ALI_TRANSPORT_EVT_TX_DONE:
            ali_gap_on_tx_done(&p_ali->gap, p_event->data.rxtx.cmd);
#ifdef CONFIG_AIS_OTA
            ali_ota_on_tx_done(&p_ali->ota, p_event->data.rxtx.cmd);
#endif
            break;

        case ALI_TRANSPORT_EVT_RX_DONE:
            if (!is_valid_command(p_event->data.rxtx.cmd))
            {
                send_err = true;
                break;
            }

            ali_gap_on_command(&p_ali->gap, p_event->data.rxtx.cmd, p_event->data.rxtx.p_data,
                               p_event->data.rxtx.length);
            ali_auth_on_command(&p_ali->auth, p_event->data.rxtx.cmd, p_event->data.rxtx.p_data,
                                p_event->data.rxtx.length);
#ifdef CONFIG_AIS_OTA
            ali_ota_on_command(&p_ali->ota, p_event->data.rxtx.cmd, p_event->data.rxtx.p_data,
                               p_event->data.rxtx.length, p_event->data.rxtx.num_frames);
#endif
            ali_ext_on_command(&p_ali->ext, p_event->data.rxtx.cmd, p_event->data.rxtx.p_data,
                               p_event->data.rxtx.length);
            break;

        case ALI_TRANSPORT_EVT_TX_TIMEOUT:
            notify_error(p_ali, ALI_ERROR_SRC_TRANSPORT_TX_TIMER, NRF_ERROR_TIMEOUT);
            break;

        case ALI_TRANSPORT_EVT_RX_TIMEOUT:
            notify_error(p_ali, ALI_ERROR_SRC_TRANSPORT_RX_TIMER, NRF_ERROR_TIMEOUT);
            break;

        case ALI_TRANSPORT_EVT_ERROR:
            notify_error(p_ali, p_event->data.error.source, p_event->data.error.err_code);

            /* Trigger resend if error is NOT caused by the module itself. */
            if (p_event->data.error.err_code != NRF_ERROR_INTERNAL)
            {
                send_err = true;

                if (p_event->data.error.source == ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC)
                {
#ifdef CONFIG_AIS_OTA
                    ali_ota_on_discontinuous_frame(&p_ali->ota);
#endif
                }
            }
            break;

        default:
            break;
    }

    if (send_err)
    {
        // Send error to central
        err_code = ali_transport_send(&p_ali->transport, ALI_TRANSPORT_TX_TYPE_NOTIFY,
                                      ALI_CMD_ERROR, NULL, 0);
        if (err_code != NRF_SUCCESS)
        {
            notify_error(p_ali, ALI_ERROR_SRC_CORE_SEND_ERR, err_code);
            return;
        }
    }
}


/**@brief Extend module: event handler function. */
static void ext_event_handler (ali_t * p_ali, ali_ext_event_t * p_event)
{
    switch (p_event->type)
    {
        case ALI_EXT_EVT_ERROR:
            notify_error(p_ali, p_event->data.error.source, p_event->data.error.err_code);
            break;

        default:
            break;
    }
}


static void ble_ais_event_handler(ali_t * p_ali, ble_ais_event_t * p_event)
{
    switch (p_event->type)
    {
        case BLE_AIS_EVT_RX_DATA:
            ali_transport_on_rx_data(&p_ali->transport, p_event->data.rx_data.p_data, p_event->data.rx_data.length);
            break;

        case BLE_AIS_EVT_TX_DONE:
            ali_transport_on_tx_complete(&p_ali->transport, p_event->data.tx_done.pkt_sent);
            ali_auth_on_tx_complete(&p_ali->auth);	 
            break;

        case BLE_AIS_EVT_SVC_ENABLED:
            ali_auth_on_enable_service(&p_ali->auth);
            if (!p_ali->is_auth_enabled)
            {
#ifdef CONFIG_AIS_OTA
                ali_ota_on_auth(&p_ali->ota, true);
#endif
            }
            break;

        default:
            break;
    }
}

ali_t *g_ali;

/*@brief Function for initializing ble_ais module. */
static uint32_t ais_init (ali_t * p_ali, ali_init_t const * p_init)
{
    ble_ais_init_t init_ais;

    g_ali = p_ali;

    memset(&init_ais, 0, sizeof(ble_ais_init_t)); 
    init_ais.event_handler = (ble_ais_event_handler_t)ble_ais_event_handler;
    init_ais.p_context     = p_ali;
    init_ais.mtu           = p_init->max_mtu;
    return ble_ais_init(&p_ali->ais, &init_ais);
}


/*@brief Function for initializing ali_transport module, the transport layer. */
static uint32_t transport_init (ali_t * p_ali, ali_init_t const * p_init)
{
    ali_transport_init_t init_transport;

    memset(&init_transport, 0, sizeof(ali_transport_init_t)); 
    init_transport.tx_buffer         = p_ali->tx_buff;
    init_transport.tx_buffer_len     = TX_BUFF_LEN;
    init_transport.rx_buffer         = (uint8_t *)p_ali->rx_buff;
    init_transport.rx_buffer_len     = RX_BUFF_LEN;
    init_transport.timeout           = p_init->transport_timeout;
    init_transport.event_handler     = (ali_transport_event_handler_t)transport_event_handler;
    init_transport.p_evt_context     = p_ali;
    init_transport.tx_func_notify    = (ali_transport_tx_func_t)ble_ais_send_notification;
    init_transport.tx_func_indicate  = (ali_transport_tx_func_t)ble_ais_send_indication;
    init_transport.p_tx_func_context = &p_ali->ais;

    return ali_transport_init(&p_ali->transport, &init_transport);
}


/*@brief Function for initializing ali_auth, the authentication module. */
static uint32_t auth_init (ali_t * p_ali, ali_init_t const * p_init, uint8_t * mac)
{
    ali_auth_init_t init_auth;

    memset(&init_auth, 0, sizeof(ali_auth_init_t)); 
    init_auth.feature_enable    = p_init->enable_auth;
    init_auth.timeout           = AUTH_TIMEOUT;
    init_auth.event_handler     = (ali_auth_event_handler_t)auth_event_handler;
    init_auth.p_evt_context     = p_ali;
    init_auth.tx_func           = (ali_auth_tx_func_t)tx_func_indicate;
    init_auth.p_tx_func_context = p_ali;
    init_auth.p_mac             = mac;
    init_auth.p_secret          = p_init->secret.p_data;
    init_auth.secret_len        = p_init->secret.length;
    init_auth.p_dkey            = p_init->device_key.p_data;
    init_auth.dkey_len          = p_init->device_key.length;
    init_auth.p_pkey            = p_init->product_key.p_data;
    init_auth.pkey_len          = p_init->product_key.length;
    init_auth.model_id          = p_init->model_id;

    return ali_auth_init(&p_ali->auth, &init_auth);
}


/*@brief Function for initializing ali_gap, the Generic Access module. */
static uint32_t gap_init (ali_t * p_ali, ali_init_t const * p_init)
{
    ali_gap_init_t init_gap;

    memset(&init_gap, 0, sizeof(ali_gap_init_t)); 
    init_gap.event_handler     = (ali_gap_event_handler_t)gap_event_handler;
    init_gap.p_evt_context     = p_ali;
    init_gap.tx_func_notify    = (ali_gap_tx_func_t)tx_func_notify;
    init_gap.tx_func_indicate  = (ali_gap_tx_func_t)tx_func_indicate;
    init_gap.p_tx_func_context = p_ali;

    return ali_gap_init(&p_ali->gap, &init_gap);
}


#ifdef CONFIG_AIS_OTA
/*@brief Function for initializing ali_ota, the OTA firmware upgrade module. */
static uint32_t ota_init (ali_t * p_ali, ali_init_t const * p_init)
{
    ali_ota_init_t init_ota;

    memset(&init_ota, 0, sizeof(ali_ota_init_t)); 
    init_ota.feature_enable    = p_init->enable_ota;
    init_ota.event_handler     = (ali_ota_event_handler_t)ota_event_handler;
    init_ota.p_evt_context     = p_ali;
    init_ota.tx_func           = (ali_ota_tx_func_t)tx_func_indicate;
    init_ota.p_tx_func_context = p_ali;
    init_ota.p_fw_version      = p_init->sw_ver.p_data;
    init_ota.fw_version_len    = p_init->sw_ver.length;

    return ali_ota_init(&p_ali->ota, &init_ota);
}
#endif


/*@brief Function for initializing ali_ext, the extend module. */
static uint32_t ext_init (ali_t * p_ali, ali_init_t const * p_init)
{
    ali_ext_init_t init_ext;

    memset(&init_ext, 0, sizeof(ali_ext_init_t)); 
    init_ext.event_handler     = (ali_ext_event_handler_t)ext_event_handler;
    init_ext.p_evt_context     = p_ali;
    init_ext.tx_func           = (ali_ext_tx_func_t)tx_func_indicate;
    init_ext.p_tx_func_context = p_ali;

    init_ext.p_fw_version      = p_init->sw_ver.p_data;
    init_ext.fw_version_len    = p_init->sw_ver.length;
    init_ext.model_id          = p_init->model_id;

    ali_auth_get_device_name(&p_ali->auth, &init_ext.p_device_name, &init_ext.device_name_len);
    ali_auth_get_product_key(&p_ali->auth, &init_ext.p_product_key, &init_ext.product_key_len);
    ali_auth_get_secret(&p_ali->auth, &init_ext.p_secret, &init_ext.secret_len);

    //ali_auth_get_v2_signature(&p_ali->auth, &init_ext.p_v2_signature, &init_ext.v2_signature_len);

    return ali_ext_init(&p_ali->ext, &init_ext);
}


/**@brief Function for verifying initialization parameters. */
static ret_code_t verify_init_params(ali_init_t const * p_init)
{
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->event_handler);

    if (p_init->context_size < sizeof(ali_t))
    {
        return NRF_ERROR_DATA_SIZE;
    }

    if (p_init->enable_auth)
    {
        VERIFY_PARAM_NOT_NULL(p_init->mac.p_data);
        VERIFY_PARAM_NOT_NULL(p_init->secret.p_data);

        if (p_init->mac.length != MAC_ASCII_LEN)
        {
            return NRF_ERROR_INVALID_LENGTH;
        }

        if (p_init->product_key.length == 0 && p_init->device_key.length == 0)
        {
            return NRF_ERROR_INVALID_LENGTH; // no product or device key available. 
        }

        if (p_init->product_key.length != 0)
        {
            VERIFY_PARAM_NOT_NULL(p_init->product_key.p_data);
        }

        if (p_init->device_key.length != 0)
        {
            VERIFY_PARAM_NOT_NULL(p_init->device_key.p_data);
            if (p_init->device_key.length > ALI_AUTH_DKEY_LEN_MAX)
            {
                return NRF_ERROR_INVALID_LENGTH;
            }
        }

        if ((p_init->product_key.length != ALI_AUTH_PKEY_V1_LEN || p_init->device_key.length != 0 || p_init->secret.length != ALI_AUTH_SECRET_V1P_LEN)
            && (p_init->product_key.length != 0 || p_init->device_key.length != ALI_AUTH_DKEY_V1_LEN || p_init->secret.length != ALI_AUTH_SECRET_V1D_LEN)
            && (p_init->product_key.length != ALI_AUTH_PKEY_V2_LEN || p_init->device_key.length == 0
                || (p_init->secret.length != ALI_AUTH_SECRET_V2P_LEN && p_init->secret.length != ALI_AUTH_SECRET_V2D_LEN)))
        {
            return NRF_ERROR_INVALID_LENGTH;
        }
    }

    if (p_init->enable_ota)
    {
        VERIFY_PARAM_NOT_NULL(p_init->sw_ver.p_data);
        if (p_init->sw_ver.length == 0 || p_init->sw_ver.length > ALI_OTA_MAX_FW_VERSION_LEN)
        {
            return NRF_ERROR_INVALID_LENGTH;
        }
    }

    return NRF_SUCCESS;
}


ret_code_t ali_init(void * p_ali_ext, ali_init_t const * p_init)
{
    ali_t  * p_ali = (ali_t *)p_ali_ext;
    uint8_t  mac_be[ALI_AUTH_MAC_LEN];
    uint32_t err_code;
    bool     secret_per_device;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_ali);
    err_code = verify_init_params(p_init);
    VERIFY_SUCCESS(err_code);

    /* Check if 4-byte aligned. */
    if (((uint32_t)p_ali & 0x3) != 0)
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    endian_swap(mac_be, p_init->mac.p_data, ALI_AUTH_MAC_LEN);

    /* Initialize context */
    memset(p_ali, 0, sizeof(ali_t));
    p_ali->is_auth_enabled   = p_init->enable_auth;
    p_ali->event_handler     = p_init->event_handler;
    p_ali->p_evt_context     = p_init->p_evt_context;
    p_ali->is_authenticated  = false;
    p_ali->conn_handle       = BLE_CONN_HANDLE_INVALID;

    /* check if V1 or V2 network. */
    if (p_init->product_key.length == ALI_AUTH_PKEY_V2_LEN && p_init->device_key.length != 0
        && (p_init->secret.length == ALI_AUTH_SECRET_V2P_LEN || p_init->secret.length == ALI_AUTH_SECRET_V2D_LEN))
    {
        /* V2 Network. */
        secret_per_device = (p_init->secret.length == ALI_AUTH_SECRET_V2D_LEN);
    }
    else
    {
        /* V1 Network. */
        secret_per_device = (p_init->device_key.length != 0);
    }

    // Dervive manufacturer-specific advertising data.
    create_manuf_spec_adv_data (p_ali, p_init->model_id, mac_be, p_init->enable_auth, p_init->enable_ota, secret_per_device);

    /* Initialize Alibaba Information Service (AIS). */
    err_code = ais_init(p_ali, p_init);
    VERIFY_SUCCESS(err_code);

    /* Initialize transport layer. */
    err_code = transport_init(p_ali, p_init);
    VERIFY_SUCCESS(err_code);

    /* Initialize Authentication module. */
    err_code = auth_init(p_ali, p_init, mac_be);
    VERIFY_SUCCESS(err_code);

    /* Initialize GAP module. */
    err_code = gap_init(p_ali, p_init);
    VERIFY_SUCCESS(err_code);

#ifdef CONFIG_AIS_OTA
    /* Initialize OTA module. */
    err_code = ota_init(p_ali, p_init);
    VERIFY_SUCCESS(err_code);
#endif

    /* Initialize extend module. */
    err_code = ext_init(p_ali, p_init);
    VERIFY_SUCCESS(err_code);

    p_ali->is_initialized = true;
    return NRF_SUCCESS;
}


void ali_reset(void * p_ali_ext)
{
    ali_t * p_ali = (ali_t *)p_ali_ext;

    /* Check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ali);

    /* Check if 4-byte aligned. */
    if (((uint32_t)p_ali & 0x3) != 0)
    {
        return;
    }

    /* Check if module is initialized. */
    VERIFY_MODULE_INITIALIZED_VOID();

    /* Reset variables. */
    p_ali->is_authenticated = false;

#ifdef CONFIG_AIS_OTA
    /* Reset modules */
    ali_ota_reset(&p_ali->ota);
#endif
    ali_ext_reset(&p_ali->ext);
    ali_gap_reset(&p_ali->gap);
    ali_auth_reset(&p_ali->auth);
    ali_transport_reset(&p_ali->transport);
    ble_ais_set_auth(&p_ali->ais, false);
}


ret_code_t ali_send_notify(void * p_ali_ext, uint8_t * p_data, uint16_t length)
{
    ali_t * p_ali = (ali_t *)p_ali_ext;

    /* Check parameters */
    VERIFY_PARAM_NOT_NULL(p_ali);
    VERIFY_PARAM_NOT_NULL(p_data);

    /* Check if 4-byte aligned. */
    if (((uint32_t)p_ali & 0x3) != 0)
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    /* Check if module is initialized. */
    VERIFY_MODULE_INITIALIZED();

    if (length == 0 || length > ALI_TRANSPORT_MAX_TX_DATA_LEN)
    {
        return NRF_ERROR_DATA_SIZE;
    }

    return ali_gap_send_notify(&p_ali->gap, p_data, length);
}


ret_code_t ali_send_indicate(void * p_ali_ext, uint8_t * p_data, uint16_t length)
{
    ali_t * p_ali = (ali_t *)p_ali_ext;

    /* Check parameters */
    VERIFY_PARAM_NOT_NULL(p_ali);
    VERIFY_PARAM_NOT_NULL(p_data);

    /* Check if 4-byte aligned. */
    if (((uint32_t)p_ali & 0x3) != 0)
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    /* Check if module is initialized. */
    VERIFY_MODULE_INITIALIZED();

    if (length == 0 || length > ALI_TRANSPORT_MAX_TX_DATA_LEN)
    {
        return NRF_ERROR_DATA_SIZE;
    }

    return ali_gap_send_indicate(&p_ali->gap, p_data, length);
}


ret_code_t ali_get_manuf_spec_adv_data(void * p_ali_ext, uint8_t * p_data, uint16_t * length)
{
    ali_t * p_ali = (ali_t *)p_ali_ext;

    /* Check parameters */
    VERIFY_PARAM_NOT_NULL(p_ali);
    VERIFY_PARAM_NOT_NULL(p_data);
    VERIFY_PARAM_NOT_NULL(length);

    /* Check if 4-byte aligned. */
    if (((uint32_t)p_ali & 0x3) != 0)
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    /* Check if module is initialized. */
    VERIFY_MODULE_INITIALIZED();

    if (*length < p_ali->manuf_spec_adv_data_len)
    {
        return NRF_ERROR_NO_MEM;
    }

    memcpy(p_data, p_ali->manuf_spec_adv_data, p_ali->manuf_spec_adv_data_len);
    *length = p_ali->manuf_spec_adv_data_len;
    return NRF_SUCCESS;
}

ret_code_t ali_ctrl(void * p_ali_ext, ali_ctrl_t ctrl_word, void * p_data)
{
    ali_t * p_ali = (ali_t *)p_ali_ext;
    uint32_t err_code;

    /* Check parameters */
    VERIFY_PARAM_NOT_NULL(p_ali);

    /* Check if 4-byte aligned. */
    if (((uint32_t)p_ali & 0x3) != 0)
    {
        return NRF_ERROR_INVALID_ADDR;
    }

    /* Check if module is initialized. */
    VERIFY_MODULE_INITIALIZED();

    switch (ctrl_word)
    {
        case ALI_CTRL_SET_MTU:
        {
            uint16_t * p_mtu = (uint16_t *)p_data;
            VERIFY_PARAM_NOT_NULL(p_data);

            err_code = ble_ais_set_mtu(&p_ali->ais, *p_mtu);
            VERIFY_SUCCESS(err_code);

            return ali_transport_set_mtu(&p_ali->transport, *p_mtu);
        }
        default:
            return NRF_ERROR_NOT_SUPPORTED;
    }
}


