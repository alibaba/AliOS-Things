/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "ali_core.h"
#include "breeze_export.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <breeze_hal_ble.h>
#include <breeze_hal_os.h>

extern struct bt_conn *g_conn;

#define BREEZE_LOG_MODULE_NAME "ALI"

#define TRANSPORT_TIMEOUT                                             \
    10000 /**< Transport layer inter-packet timeout, in number of ms, \
             applicable to both Tx and Rx. */
#define APP_TIMER_PRESCALER 0 /**< Value of the RTC1 PRESCALER register. */

#ifndef BREEZE_LOG_DEBUG
#define BREEZE_LOG_DEBUG printf
#endif

static dev_status_changed_cb m_status_handler; /**< Handler for status. */
static set_dev_status_cb
  m_ctrl_handler; /**< Handler for the payload of cmd=0x00. */
static get_dev_status_cb
  m_query_handler; /**< Handler for the payload of cmd=0x02. */
static apinfo_ready_cb
            m_apinfo_handler; /**< Handler for the apinfo ready event. */
static ota_dev_cb m_ota_dev_handler;/*handler for ota event/cmd, e.g.0x20, 0x22, 0x24, 0x27, 0x2F, 0x28*/
static bool m_new_firmware; /**< Flag indicating whether there is a new firmware
                               pending for commit. */
extern uint16_t m_conn_handle; /**< Handle of the current connection. */

uint32_t m_ali_context[ALI_CONTEXT_SIZE]; /**< Global context of ali_core. */

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

uint32_t *fetch_ali_context()
{
    return m_ali_context;
}

/**@brief Event handler function. */
static void ali_event_handler(void *p_context, ali_event_t *p_event)
{
    uint32_t err_code;

    switch (p_event->type) {
        case ALI_EVT_CONNECTED:
            BREEZE_LOG_DEBUG("ALI_EVT_CONNECTED\r\n");
            notify_status(CONNECTED);
            break;

        case ALI_EVT_DISCONNECTED:
            BREEZE_LOG_DEBUG("ALI_EVT_DISCONNECTED\r\n");
            ali_reset(m_ali_context);
            notify_status(DISCONNECTED);
            if (m_new_firmware) {
#ifdef CONFIG_AIS_OTA
                BREEZE_LOG_DEBUG("Firmware download completed, system will reboot now!");
                os_reboot();
#endif
            }
            break;

        case ALI_EVT_AUTHENTICATED:
            BREEZE_LOG_DEBUG("ALI_EVT_AUTHENTICATED\r\n");
            notify_status(AUTHENTICATED);
            break;

        case ALI_EVT_CTRL_RECEIVED:
            BREEZE_LOG_DEBUG("ALI_EVT_CTRL_RECEIVED\r\n");
            if (m_ctrl_handler != NULL) {
                m_ctrl_handler(p_event->data.rx_data.p_data,
                               p_event->data.rx_data.length);
            }
            break;

        case ALI_EVT_QUERY_RECEIVED:
            BREEZE_LOG_DEBUG("ALI_EVT_QUERY_RECEIVED\r\n");
            if (m_query_handler != NULL) {
                m_query_handler(p_event->data.rx_data.p_data,
                                p_event->data.rx_data.length);
            }
            break;

        case ALI_EVT_TX_DONE:
            BREEZE_LOG_DEBUG("ALI_EVT_TX_DONE\r\n");
            notify_status(TX_DONE);
            break;

        case ALI_EVT_APINFO:
            BREEZE_LOG_DEBUG("ALI_EVT_APINFO\r\n");
            if (m_apinfo_handler != NULL) {
                m_apinfo_handler(p_event->data.rx_data.p_data);
            }
            break;
        case ALI_EVT_OTA_CMD:
            BREEZE_LOG_DEBUG("ALI_OTA_CMD_EVT\r\n");
	    if (m_ota_dev_handler != NULL){
                m_ota_dev_handler(p_event->data.rx_data.p_data);
            }
            break;
        case ALI_EVT_ERROR:
            BREEZE_LOG_DEBUG("ALI_EVT_ERROR: source=0x%08x, err_code=%08x\r\n",
                          p_event->data.error.source,
                          p_event->data.error.err_code);
            break;

        default:
            break;
    }
}


int breeze_start(struct device_config *dev_conf)
{
    uint32_t   err_code;
    ali_init_t init_ali;

    if ((dev_conf == NULL) || (dev_conf->status_changed_cb == NULL) ||
        (dev_conf->set_cb == NULL) || (dev_conf->get_cb == NULL) ||
        (dev_conf->apinfo_cb == NULL)) {
        return -1;
    }

    m_status_handler = dev_conf->status_changed_cb;
    m_ctrl_handler   = dev_conf->set_cb;
    m_query_handler  = dev_conf->get_cb;
    m_new_firmware   = false;
    m_apinfo_handler = dev_conf->apinfo_cb;
    m_ota_dev_handler = dev_conf->ota_cb;

    memset(&init_ali, 0, sizeof(ali_init_t));
    init_ali.context_size  = sizeof(m_ali_context);
    init_ali.event_handler = ali_event_handler;
    init_ali.p_evt_context = NULL;
    init_ali.model_id      = dev_conf->product_id;
    init_ali.mac.p_data    = NULL;
    init_ali.mac.length    = BD_ADDR_LEN;

    init_ali.product_key.p_data = (uint8_t *)dev_conf->product_key;
    init_ali.product_key.length = dev_conf->product_key_len;
    init_ali.device_key.p_data  = (uint8_t *)dev_conf->device_key;
    init_ali.device_key.length  = dev_conf->device_key_len;

    init_ali.secret.p_data         = (uint8_t *)dev_conf->secret;
    init_ali.secret.length         = dev_conf->secret_len;
    init_ali.product_secret.p_data = (uint8_t *)dev_conf->product_secret;
    init_ali.product_secret.length = dev_conf->product_secret_len;
    init_ali.sw_ver.p_data         = (uint8_t *)dev_conf->version;
    init_ali.sw_ver.length         = strlen(dev_conf->version);
    init_ali.timer_prescaler       = APP_TIMER_PRESCALER;
    init_ali.transport_timeout     = TRANSPORT_TIMEOUT;
    init_ali.enable_auth           = dev_conf->enable_auth;
    init_ali.enable_ota            = dev_conf->enable_ota;
    init_ali.max_mtu               = ALI_MAX_SUPPORTED_MTU;
    init_ali.user_adv_data         = user_adv.data;
    init_ali.user_adv_len          = user_adv.len;

    err_code = ali_init(m_ali_context, &init_ali);
    return ((err_code == BREEZE_SUCCESS) ? 0 : -1);
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
    return ali_send_indicate(m_ali_context, cmd, buffer, length);
}


uint32_t breeze_post_fast(uint8_t cmd,uint8_t *buffer, uint32_t length)
{
    return ali_send_notify(m_ali_context, cmd, buffer, length);
}


void breeze_event_dispatcher()
{
    os_start_event_dispatcher();
    while (1);
}

void breeze_append_adv_data(uint8_t *data, uint32_t len)
{
    if (!data) return;

    if (len > MAX_VENDOR_DATA_LEN) {
        printf("Warning: adv data too long, not set!!!\r\n");
        return;
    }

    if (len > 0) memcpy(user_adv.data, data, len);
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
        printf("Failed to stop previous adv.\r\n");
        return;
    }

    adv_data.vdata.len = sizeof(adv_data.vdata.data);
    err = ali_get_manuf_spec_adv_data(NULL, adv_data.vdata.data,
                                      &(adv_data.vdata.len));
    if (err) {
        printf("%s %d fail.\r\n", __func__, __LINE__);
        return;
    }

    if (user_adv.len > 0) {
        size = sizeof(adv_data.vdata.data) - adv_data.vdata.len;
        if (size < user_adv.len) {
            printf("Warning: no space for user adv data (expected %d but"
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
