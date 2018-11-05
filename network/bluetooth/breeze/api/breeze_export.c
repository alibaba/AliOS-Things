/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "core.h"
#include "breeze_export.h"
#include "breeze_hal_ble.h"
#include "breeze_hal_os.h"
#include "bzopt.h"

static dev_status_changed_cb m_status_handler;
static set_dev_status_cb m_ctrl_handler;
static get_dev_status_cb m_query_handler;
static apinfo_ready_cb m_apinfo_handler;
static ota_dev_cb m_ota_dev_handler;

struct adv_data_s {
    uint8_t data[MAX_VENDOR_DATA_LEN];
    uint32_t len;
} user_adv = {{0}};

static void notify_status(breeze_event_t event)
{
    if (m_status_handler != NULL) {
        m_status_handler(event);
    }
}

static void event_handler(ali_event_t *p_event)
{
    uint32_t err_code;

    switch (p_event->type) {
        case BZ_EVENT_CONNECTED:
            notify_status(CONNECTED);
            break;

        case BZ_EVENT_DISCONNECTED:
            core_reset();
            notify_status(DISCONNECTED);
            if (m_ota_dev_handler != NULL){
                breeze_otainfo_t m_disc_evt;
                m_disc_evt.type = OTA_EVT;
                m_disc_evt.cmd_evt.m_evt.evt = ALI_OTA_ON_DISCONNECTED;
                m_disc_evt.cmd_evt.m_evt.d = 0;
                m_ota_dev_handler(&m_disc_evt);
            }
            break;

        case BZ_EVENT_AUTHENTICATED:
            notify_status(AUTHENTICATED);
            extern void notify_ota_event(uint8_t ota_evt, uint8_t sub_evt);
            notify_ota_event(ALI_OTA_ON_AUTH_EVT, true);
            break;

        case BZ_EVENT_RX_CTRL:
            if (m_ctrl_handler != NULL) {
                m_ctrl_handler(p_event->rx_data.p_data, p_event->rx_data.length);
            }
            break;

        case BZ_EVENT_RX_QUERY:
            if (m_query_handler != NULL) {
                m_query_handler(p_event->rx_data.p_data, p_event->rx_data.length);
            }
            break;

        case BZ_EVENT_TX_DONE:
            notify_status(TX_DONE);
            break;

        case BZ_EVENT_APINFO:
            if (m_apinfo_handler != NULL) {
                m_apinfo_handler(p_event->rx_data.p_data);
            }
            break;
        case BZ_EVENT_OTAINFO:
	    if (m_ota_dev_handler != NULL){
                m_ota_dev_handler(p_event->rx_data.p_data);
            }
            break;
        default:
            break;
    }
}

int breeze_start(struct device_config *dev_conf)
{
    uint32_t err_code;
    ali_init_t init_ali;

    if ((dev_conf == NULL) || (dev_conf->status_changed_cb == NULL) ||
        (dev_conf->set_cb == NULL) || (dev_conf->get_cb == NULL) ||
        (dev_conf->apinfo_cb == NULL)) {
        return -1;
    }

    m_status_handler = dev_conf->status_changed_cb;
    m_ctrl_handler   = dev_conf->set_cb;
    m_query_handler  = dev_conf->get_cb;
    m_apinfo_handler = dev_conf->apinfo_cb;
    m_ota_dev_handler = dev_conf->ota_cb;

    memset(&init_ali, 0, sizeof(ali_init_t));
    init_ali.event_handler = event_handler;
    init_ali.model_id = dev_conf->product_id;

    init_ali.product_key.p_data = (uint8_t *)dev_conf->product_key;
    init_ali.product_key.length = dev_conf->product_key_len;
    init_ali.device_key.p_data  = (uint8_t *)dev_conf->device_key;
    init_ali.device_key.length  = dev_conf->device_key_len;

    init_ali.secret.p_data         = (uint8_t *)dev_conf->secret;
    init_ali.secret.length         = dev_conf->secret_len;
    init_ali.product_secret.p_data = (uint8_t *)dev_conf->product_secret;
    init_ali.product_secret.length = dev_conf->product_secret_len;
    init_ali.transport_timeout     = BZ_TRANSPORT_TIMEOUT;
    init_ali.max_mtu               = BZ_MAX_SUPPORTED_MTU;
    init_ali.user_adv_data         = user_adv.data;
    init_ali.user_adv_len          = user_adv.len;

    err_code = core_init(&init_ali);
    return ((err_code == BZ_SUCCESS) ? 0 : -1);
}


int breeze_end(void)
{
    int ret = 0;

    if (ble_stack_deinit() != AIS_ERR_SUCCESS) {
        ret = -1;
    }

    return 0;
}

uint32_t breeze_post(uint8_t cmd, uint8_t *buffer, uint32_t length)
{
    if (length == 0 || length > BZ_MAX_PAYLOAD_SIZE) {
        return BZ_EDATASIZE;
    }

    if (cmd == 0) {
        cmd = BZ_CMD_STATUS;
    }
    return transport_tx(TX_INDICATION, cmd, buffer, length);
}

uint32_t breeze_post_fast(uint8_t cmd,uint8_t *buffer, uint32_t length)
{
    if (length == 0 || length > BZ_MAX_PAYLOAD_SIZE) {
        return BZ_EDATASIZE;
    }
    
    if (cmd == 0) {
        cmd = BZ_CMD_STATUS;
    }
    return transport_tx(TX_NOTIFICATION, cmd, buffer, length);
}

void breeze_append_adv_data(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0 || len > MAX_VENDOR_DATA_LEN) {
        BREEZE_LOG_DEBUG("invalid adv data\r\n");
        return;
    }

    memcpy(user_adv.data, data, len);
    user_adv.len = len;
}

void breeze_restart_advertising()
{
    ais_err_t err;
    uint32_t size;

    ais_adv_init_t adv_data = {
        .flag = AIS_AD_GENERAL | AIS_AD_NO_BREDR,
        .name = { .ntype = AIS_ADV_NAME_FULL, .name = "AlibabaIoTService" },
    };

    err = ble_advertising_stop();
    if (err != AIS_ERR_SUCCESS) {
        BREEZE_LOG_ERR("Failed to stop previous adv.\r\n");
        return;
    }

    adv_data.vdata.len = sizeof(adv_data.vdata.data);
    err = get_bz_adv_data(adv_data.vdata.data, &(adv_data.vdata.len));
    if (err) {
        BREEZE_LOG_ERR("%s %d fail.\r\n", __func__, __LINE__);
        return;
    }

    if (user_adv.len > 0) {
        size = sizeof(adv_data.vdata.data) - adv_data.vdata.len;
        if (size < user_adv.len) {
            BREEZE_LOG_ERR("Warning: no space for user adv data (expected %d but"
                   " only %d left). No user adv data set!!!\r\n",
                   user_adv.len, size);
        } else {
            memcpy(adv_data.vdata.data + adv_data.vdata.len,
                   user_adv.data, user_adv.len);
            adv_data.vdata.len += user_adv.len;
        }
    }

     ble_advertising_start(&adv_data);
}

void breeze_disconnect_ble(void)
{
    ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
}
