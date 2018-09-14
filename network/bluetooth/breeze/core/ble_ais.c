/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "breeze_hal_ble.h"
#include "ble_ais.h"
#include "ali_common.h"
#include "ali_core.h"

#define MAX_MTU  247

ble_ais_t *g_ais;

/**@brief Notify Rx data to higher layer. */
static void notify_data(ble_ais_t *p_ais, uint8_t *p_data, uint16_t length)
{
    ble_ais_event_t evt;

    evt.type                = BLE_AIS_EVT_RX_DATA;
    evt.data.rx_data.p_data = p_data;
    evt.data.rx_data.length = length;
    p_ais->event_handler(p_ais->p_context, &evt);
}


/**@brief Notify number of packets sent to higher layer (i.e. Tx-done). */
static void notify_pkt_sent(ble_ais_t *p_ais, uint8_t pkt_sent)
{
    ble_ais_event_t evt;

    evt.type                  = BLE_AIS_EVT_TX_DONE;
    evt.data.tx_done.pkt_sent = pkt_sent;
    p_ais->event_handler(p_ais->p_context, &evt);
}


/**@brief Notify higher layer that service has been enabled.
 *
 * @note  Both the notification of "Notify Characteristics" characteristic
 *        and the indication of "Indicate Characteristics" characteristic
 *        has been enabled
 */
static void notify_svc_enabled(ble_ais_t *p_ais)
{
    ble_ais_event_t evt;

    if (p_ais->is_indication_enabled && p_ais->is_notification_enabled) {
        printf("Let's notify that service is enabled.\r\n");
        evt.type = BLE_AIS_EVT_SVC_ENABLED;
        p_ais->event_handler(p_ais->p_context, &evt);
    }
}

static void connected()
{
    g_ais->conn_handle             = BLE_CONN_HANDLE_MAGIC;
    g_ali->conn_handle             = BLE_CONN_HANDLE_MAGIC;
    g_ais->is_authenticated        = false;
    g_ais->is_indication_enabled   = false;
    g_ais->is_notification_enabled = false;

    ali_auth_on_connected(&g_ali->auth);
    ble_ais_set_auth(g_ais, true);
    notify_evt_no_data(g_ali, ALI_EVT_CONNECTED);
}

static void disconnected()
{
    g_ais->conn_handle             = BLE_CONN_HANDLE_INVALID;
    g_ais->is_authenticated        = false;
    g_ais->is_indication_enabled   = false;
    g_ais->is_notification_enabled = false;
    notify_evt_no_data(g_ali, ALI_EVT_DISCONNECTED);
}

static void ic_ccc_handler(ais_ccc_value_t val)
{
    g_ais->is_indication_enabled =
      (val == AIS_CCC_VALUE_INDICATE ? true : false);
    notify_svc_enabled(g_ais);
}

static void nc_ccc_handler(ais_ccc_value_t val)
{
    g_ais->is_notification_enabled =
      (val == AIS_CCC_VALUE_NOTIFY ? true : false);
    notify_svc_enabled(g_ais);
}

static size_t wc_write_handler(const void *buf, uint16_t len)
{
    notify_data(g_ais, (uint8_t *)buf, len);
    return len;
}

static size_t wwnrc_write_handler(const void *buf, uint16_t len)
{
    notify_data(g_ais, (uint8_t *)buf, len);
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

static void ble_event_handler(os_event_t *event, void *priv_data)
{
    if (event->type != OS_EV_BLE) return;
    switch (event->code) {
        case OS_EV_CODE_BLE_TX_COMPLETED:
            notify_pkt_sent(g_ais, event->value);
            break;
        default:
            break;
    }
}

uint32_t ble_ais_init(ble_ais_t *p_ais, const ble_ais_init_t *p_ais_init)
{
    VERIFY_PARAM_NOT_NULL(p_ais);
    VERIFY_PARAM_NOT_NULL(p_ais_init);
    VERIFY_PARAM_NOT_NULL(p_ais_init->event_handler);
    if (p_ais_init->mtu < GATT_MTU_SIZE_DEFAULT || p_ais_init->mtu > MAX_MTU) {
        return BREEZE_ERROR_INVALID_PARAM;
    }

    os_register_event_filter(OS_EV_BLE, ble_event_handler, NULL);
    g_ais = p_ais;

    // Initialize the service structure.
    memset(p_ais, 0, sizeof(ble_ais_t));
    p_ais->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_ais->event_handler           = p_ais_init->event_handler;
    p_ais->p_context               = p_ais_init->p_context;
    p_ais->is_indication_enabled   = false;
    p_ais->is_notification_enabled = false;
    p_ais->is_authenticated        = false;
    p_ais->max_pkt_size            = p_ais_init->mtu - 3;

    return ble_stack_init(&ais_attr_info);
}

uint32_t ble_ais_set_auth(ble_ais_t *p_ais, bool is_authenticated)
{
    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return BREEZE_ERROR_INVALID_STATE;
    }

    p_ais->is_authenticated = is_authenticated;
    return BREEZE_SUCCESS;
}

uint32_t ble_ais_set_mtu(ble_ais_t *p_ais, uint16_t mtu)
{
    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return BREEZE_ERROR_INVALID_STATE;
    }

    if (mtu > MAX_MTU) {
        return BREEZE_ERROR_INVALID_PARAM;
    }

    p_ais->max_pkt_size = mtu - 3;
    return BREEZE_SUCCESS;
}

uint32_t ble_ais_send_notification(ble_ais_t *p_ais, uint8_t *p_data,
                                   uint16_t length)
{
    int err;

    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID ||
        !p_ais->is_notification_enabled) {
        return BREEZE_ERROR_INVALID_STATE;
    }

    if (length > p_ais->max_pkt_size) {
        return BREEZE_ERROR_DATA_SIZE;
    }

    err = ble_send_notification(p_data, length);
    if (err) {
        return BREEZE_ERROR_GATT_NOTIFY;
    } else {
        return BREEZE_SUCCESS;
    }
}


uint32_t ble_ais_send_indication(ble_ais_t *p_ais, uint8_t *p_data,
                                 uint16_t length)
{
    int err;

    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID ||
        !p_ais->is_indication_enabled) {
        return BREEZE_ERROR_INVALID_STATE;
    }

    if (length > p_ais->max_pkt_size) {
        return BREEZE_ERROR_DATA_SIZE;
    }
    err = ble_send_indication(p_data, length);

    if (err) {
        return BREEZE_ERROR_GATT_INDICATE;
    } else {
        return BREEZE_SUCCESS;
    }
}
