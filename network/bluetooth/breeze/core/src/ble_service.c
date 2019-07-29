/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>

#include "breeze_hal_ble.h"
#include "ble_service.h"
#include "common.h"
#include "core.h"
#include "bzopt.h"

ble_ais_t g_ais;
init_done_t g_init_done;

static void service_enabled(void)
{
    if (g_ais.is_indication_enabled && g_ais.is_notification_enabled) {
        BREEZE_LOG_INFO("Let's notify that service is enabled.\r\n");
#ifdef EN_LONG_MTU
        trans_update_mtu();
#endif
#if BZ_ENABLE_AUTH
        auth_service_enabled();
#endif
    }
}

static void connected()
{
    g_ais.conn_handle = BLE_CONN_HANDLE_MAGIC;
    g_ais.is_indication_enabled = false;
    g_ais.is_notification_enabled = false;

#if BZ_ENABLE_AUTH
    auth_connected();
#endif
    event_notify(BZ_EVENT_CONNECTED, NULL, 0);
}

static void disconnected()
{
    g_ais.conn_handle = BLE_CONN_HANDLE_INVALID;
    g_ais.is_indication_enabled = false;
    g_ais.is_notification_enabled = false;
    event_notify(BZ_EVENT_DISCONNECTED, NULL, 0);
}

static void ic_ccc_handler(ais_ccc_value_t val)
{
    g_ais.is_indication_enabled = (val == AIS_CCC_VALUE_INDICATE ? true : false);
    service_enabled();
}

static void nc_ccc_handler(ais_ccc_value_t val)
{
    g_ais.is_notification_enabled = (val == AIS_CCC_VALUE_NOTIFY ? true : false);
    service_enabled();
}

static size_t wc_write_handler(const void *buf, uint16_t len)
{
    transport_rx((uint8_t *)buf, len);
    return len;
}

static size_t wwnrc_write_handler(const void *buf, uint16_t len)
{
    transport_rx((uint8_t *)buf, len);
    return len;
}

ais_bt_init_t ais_attr_info = {
    /* service */
    AIS_UUID_DECLARE_16(BLE_UUID_AIS_SERVICE),
    /* rc */
    { .uuid          = AIS_UUID_DECLARE_16(BLE_UUID_AIS_RC),
      .prop          = AIS_GATT_CHRC_READ,
      .perm          = AIS_GATT_PERM_READ | AIS_GATT_PERM_READ_AUTHEN,
      .on_read       = NULL,
      .on_write      = NULL,
      .on_ccc_change = NULL },
    /* wc */
    { .uuid          = AIS_UUID_DECLARE_16(BLE_UUID_AIS_WC),
      .prop          = AIS_GATT_CHRC_READ | AIS_GATT_CHRC_WRITE,
      .perm          = AIS_GATT_PERM_READ | AIS_GATT_PERM_WRITE,
      .on_read       = NULL,
      .on_write      = wc_write_handler,
      .on_ccc_change = NULL },
    /* ic */
    { .uuid          = AIS_UUID_DECLARE_16(BLE_UUID_AIS_IC),
      .prop          = AIS_GATT_CHRC_READ | AIS_GATT_CHRC_INDICATE,
      .perm          = AIS_GATT_PERM_READ,
      .on_read       = NULL,
      .on_write      = NULL,
      .on_ccc_change = ic_ccc_handler },
    /* wwnrc */
    { .uuid          = AIS_UUID_DECLARE_16(BLE_UUID_AIS_WWNRC),
      .prop          = AIS_GATT_CHRC_READ | AIS_GATT_CHRC_WRITE_WITHOUT_RESP,
      .perm          = AIS_GATT_PERM_READ | AIS_GATT_PERM_WRITE,
      .on_read       = NULL,
      .on_write      = wwnrc_write_handler,
      .on_ccc_change = NULL },
    /* nc */
    { .uuid          = AIS_UUID_DECLARE_16(BLE_UUID_AIS_NC),
      .prop          = AIS_GATT_CHRC_READ | AIS_GATT_CHRC_NOTIFY,
      .perm          = AIS_GATT_PERM_READ,
      .on_read       = NULL,
      .on_write      = NULL,
      .on_ccc_change = nc_ccc_handler },
    connected,
    disconnected
};

static void stack_init_done(uint8_t res)
{
    g_init_done(res);
}

uint32_t ble_ais_init(const ble_ais_init_t *p_ais_init)
{
    memset(&g_ais, 0, sizeof(ble_ais_t));
    g_ais.conn_handle = BLE_CONN_HANDLE_INVALID;
    g_ais.is_indication_enabled = false;
    g_ais.is_notification_enabled = false;
    g_ais.max_pkt_size = p_ais_init->mtu - 3;
    g_init_done = p_ais_init->init_done;

    return ble_stack_init(&ais_attr_info, stack_init_done);
}

uint32_t ble_ais_send_notification(uint8_t *p_data, uint16_t length)
{
    int err;

    if (g_ais.conn_handle == BLE_CONN_HANDLE_INVALID ||
        g_ais.is_notification_enabled == false) {
        return BZ_EINVALIDSTATE;
    }

    if (length > g_ais.max_pkt_size) {
        return BZ_EDATASIZE;
    }

    err = ble_send_notification(p_data, length);
    if (err) {
        return BZ_EGATTNOTIFY;
    } else {
        return BZ_SUCCESS;
    }
}

static void send_indication_done(uint8_t res)
{
    if (res == BZ_SUCCESS) {
        transport_txdone(1);
    }
}

uint32_t ble_ais_send_indication(uint8_t *p_data, uint16_t length)
{
    int err;

    if (g_ais.conn_handle == BLE_CONN_HANDLE_INVALID ||
        g_ais.is_indication_enabled == false) {
        return BZ_EINVALIDSTATE;
    }

    if (length > g_ais.max_pkt_size) {
        return BZ_EDATASIZE;
    }
    err = ble_send_indication(p_data, length, send_indication_done);

    if (err) {
        return BZ_EGATTINDICATE;
    } else {
        return BZ_SUCCESS;
    }
}
