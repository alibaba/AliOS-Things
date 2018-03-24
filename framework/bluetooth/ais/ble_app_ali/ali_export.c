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
#include "api_export.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include <stdbool.h>
#include "ali_common.h"
#include <bluetooth/conn.h>

extern struct bt_conn *g_conn;

#define NRF_LOG_MODULE_NAME "ALI"

#ifndef NRF_LOG_DEBUG
#define NRF_LOG_DEBUG printf
#endif

#define TRANSPORT_TIMEOUT                10000          /**< Transport layer inter-packet timeout, in number of ms, applicable to both Tx and Rx. */
#define APP_TIMER_PRESCALER              0              /**< Value of the RTC1 PRESCALER register. */


static dev_status_changed_cb m_status_handler;          /**< Handler for status. */
static set_dev_status_cb     m_ctrl_handler;            /**< Handler for the payload of cmd=0x00. */
static get_dev_status_cb     m_query_handler;           /**< Handler for the payload of cmd=0x02. */
static bool                  m_new_firmware;            /**< Flag indicating whether there is a new firmware pending for commit. */
extern uint16_t              m_conn_handle;             /**< Handle of the current connection. */

extern uint32_t m_ali_context[ALI_CONTEXT_SIZE];        /**< Global context of ali_core. */


static void notify_status (alink_event_t event)
{
    if (m_status_handler != NULL)
    {
        m_status_handler(event);
    }
}

static void disconnect_ble(void *arg)
{
    (void *)arg;
    if (!g_conn) return;
    bt_conn_disconnect(g_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

/**@brief Event handler function. */
static void ali_event_handler (void * p_context, ali_event_t * p_event)
{
    uint32_t err_code;

    switch (p_event->type)
    {
        case ALI_EVT_CONNECTED:
            NRF_LOG_DEBUG("ALI_EVT_CONNECTED\r\n");
            notify_status(CONNECTED);
            break;

        case ALI_EVT_DISCONNECTED:
            NRF_LOG_DEBUG("ALI_EVT_DISCONNECTED\r\n");
            ali_reset(m_ali_context);
            notify_status(DISCONNECTED);
            if (m_new_firmware)
            {
                NRF_LOG_DEBUG("Firmware download completed, system will reboot now!");
                aos_reboot();
            }
            break;

        case ALI_EVT_AUTHENTICATED:
            NRF_LOG_DEBUG("ALI_EVT_AUTHENTICATED\r\n");
            notify_status(AUTHENTICATED);
            break;

        case ALI_EVT_CTRL_RECEIVED:
            NRF_LOG_DEBUG("ALI_EVT_CTRL_RECEIVED\r\n");
            if (m_ctrl_handler != NULL)
            {
                m_ctrl_handler(p_event->data.rx_data.p_data, p_event->data.rx_data.length);
            }
            break;

        case ALI_EVT_QUERY_RECEIVED:
            NRF_LOG_DEBUG("ALI_EVT_QUERY_RECEIVED\r\n");
            if (m_query_handler != NULL)
            {
                m_query_handler(p_event->data.rx_data.p_data, p_event->data.rx_data.length);
            }
            break;

        case ALI_EVT_TX_DONE:
            NRF_LOG_DEBUG("ALI_EVT_TX_DONE\r\n");
            notify_status(TX_DONE);
            break;

        case ALI_EVT_NEW_FIRMWARE:
            NRF_LOG_DEBUG("ALI_EVT_NEW_FIRMWARE\r\n");
            m_new_firmware = true;
            /* still have data feedback to app, so do disconnection after a while */
            aos_post_delayed_action(5000, disconnect_ble, NULL);
            break;

        case ALI_EVT_ERROR:
            NRF_LOG_DEBUG("ALI_EVT_ERROR: source=0x%08x, err_code=%08x\r\n",
                          p_event->data.error.source, p_event->data.error.err_code);
            break;

        default:
            break;
    }
}


int alink_start (struct device_config * dev_conf)
{
    uint32_t   err_code;
    ali_init_t init_ali;

    if ((dev_conf == NULL)
        || (dev_conf->status_changed_cb == NULL)
        || (dev_conf->set_cb == NULL)
        || (dev_conf->get_cb == NULL))
    {
        return -1;
    }

    m_status_handler = dev_conf->status_changed_cb;
    m_ctrl_handler   = dev_conf->set_cb;
    m_query_handler  = dev_conf->get_cb;
    m_new_firmware   = false;

    memset(&init_ali, 0, sizeof(ali_init_t));
    init_ali.context_size      = sizeof(m_ali_context);
    init_ali.event_handler     = ali_event_handler;
    init_ali.p_evt_context     = NULL;
    init_ali.model_id          = dev_conf->shortmodel;
    init_ali.p_mac             = dev_conf->bd_addr;
    init_ali.mac_len           = BD_ADDR_LEN;
    init_ali.p_secret          = (uint8_t *)dev_conf->secret;
    init_ali.secret_len        = STR_SEC_LEN - 1;           // Excluding zero termination.
    init_ali.p_sw_ver          = (uint8_t *)dev_conf->version;
    init_ali.sw_ver_len        = strlen(dev_conf->version);
    init_ali.timer_prescaler   = APP_TIMER_PRESCALER;
    init_ali.transport_timeout = TRANSPORT_TIMEOUT;
    init_ali.enable_auth       = dev_conf->enable_auth;
    init_ali.enable_ota        = dev_conf->enable_ota;
    init_ali.max_mtu           = ALI_MAX_SUPPORTED_MTU;

    err_code = ali_init(m_ali_context, &init_ali);
    return ((err_code == NRF_SUCCESS)? 0: -1);
}


int alink_end (void)
{
    return 0;
}


void alink_post (uint8_t * buffer, uint32_t length)
{
    uint32_t err_code = ali_send_indicate(m_ali_context, buffer, length);
    VERIFY_SUCCESS_VOID(err_code);
}


void alink_post_fast (uint8_t * buffer, uint32_t length)
{
    uint32_t err_code = ali_send_notify(m_ali_context, buffer, length);
    VERIFY_SUCCESS_VOID(err_code);
}


