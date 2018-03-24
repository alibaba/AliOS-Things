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
#include "ble_ais.h"
#include "ali_common.h"
#include "ble_gatt.h"
#include "ali_core.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <aos/aos.h>
#include <aos/list.h>

#define MOD "ble_ais"

#define BLE_AIS_MAX_RC_CHAR_LEN           BLE_AIS_MAX_DATA_LEN     /**< Maximum length of the "Read Characteristics" Characteristic (in bytes). */

#define MAX_MTU                           247                      /**< MTU defined by Alibaba specification: maximum data length = 240. */

ble_ais_t *g_ais;

typedef uint16_t uuid_val_t;

/**@brief Notify Rx data to higher layer. */
static void notify_data (ble_ais_t * p_ais, uint8_t * p_data, uint16_t length)
{
    ble_ais_event_t evt;

    evt.type                = BLE_AIS_EVT_RX_DATA;
    evt.data.rx_data.p_data = p_data;
    evt.data.rx_data.length = length;
    p_ais->event_handler(p_ais->p_context, &evt);
}

/**@brief Notify number of packets sent to higher layer (i.e. Tx-done). */
static void notify_pkt_sent (ble_ais_t * p_ais, uint8_t pkt_sent)
{
    ble_ais_event_t evt;

    LOGD(MOD, "In %s, %d packets sent.", __func__, pkt_sent);

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
static void notify_svc_enabled (ble_ais_t * p_ais)
{
    ble_ais_event_t evt;

    if (p_ais->is_indication_enabled && p_ais->is_notification_enabled)
    {
        evt.type = BLE_AIS_EVT_SVC_ENABLED;
        p_ais->event_handler(p_ais->p_context, &evt);
    }
}

struct bt_conn *g_conn = NULL;

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printf("Connection failed (err %u)\n", err);
    } else {
        printf("Connected\n");
        g_ais->conn_handle = BLE_CONN_HANDLE_MAGIC;
        g_ali->conn_handle = BLE_CONN_HANDLE_MAGIC;
        g_ais->is_authenticated = false;
        g_ais->is_indication_enabled   = false;
        g_ais->is_notification_enabled = false;
        g_conn = conn;

        ali_auth_on_connected(&g_ali->auth);
        ble_ais_set_auth(g_ais, true);
        notify_evt_no_data(g_ali, ALI_EVT_CONNECTED);
    }
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    printf("Disconnected (reason %u)\n", reason);
    g_ais->conn_handle      = BLE_CONN_HANDLE_INVALID;
    g_ais->is_authenticated = false;
    g_ais->is_indication_enabled   = false;
    g_ais->is_notification_enabled = false;
    notify_evt_no_data(g_ali, ALI_EVT_DISCONNECTED);
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

typedef enum {
    HDLS_AIS = 0x50,

    /* read characteristic */
    HDLC_AIS_RC,
    HDLC_AIS_RC_VALUE,
    HDLC_AIS_RC_DESCRIPTION,
    HDLC_AIS_RC_CCC,
    HDLC_AIS_RC_SCC,

    /* write characteristic */
    HDLC_AIS_WC,
    HDLC_AIS_WC_VALUE,
    HDLC_AIS_WC_DESCRIPTION,
    HDLC_AIS_WC_CCC,
    HDLC_AIS_WC_SCC,

    /* indicate characteristic */
    HDLC_AIS_IC,
    HDLC_AIS_IC_VALUE,
    HDLC_AIS_IC_DESCRIPTION,
    HDLC_AIS_IC_CCC,
    HDLC_AIS_IC_SCC,

    /* write without response characteristic */
    HDLC_AIS_WWNRC,
    HDLC_AIS_WWNRC_VALUE,
    HDLC_AIS_WWNRC_DESCRIPTION,
    HDLC_AIS_WWNRC_CCC,
    HDLC_AIS_WWNRC_SCC,

    /* notify characteristic */
    HDLC_AIS_NC,
    HDLC_AIS_NC_VALUE,
    HDLC_AIS_NC_DESCRIPTION,
    HDLC_AIS_NC_CCC,
    HDLC_AIS_NC_SCC,
} ais_handle_t;

/* UUIDs */
//#define BLE_UUID_AIS_SERVICE 0xFEB3 /* The UUID of the Alibaba IOT Service. */
#define BLE_UUID_AIS_RC      0xFED4 /* The UUID of the "Read Characteristics" Characteristic. */
#define BLE_UUID_AIS_WC      0xFED5 /* The UUID of the "Write Characteristics" Characteristic. */
#define BLE_UUID_AIS_IC      0xFED6 /* The UUID of the "Indicate Characteristics" Characteristic. */
#define BLE_UUID_AIS_WWNRC   0xFED7 /* The UUID of the "Write WithNoRsp Characteristics" Characteristic. */
#define BLE_UUID_AIS_NC      0xFED8 /* The UUID of the "Notify Characteristics" Characteristic. */

#define BLE_UUID_AIS_RC_VALUE 0x7e, 0x82, 0xa0, 0xa8, 0x1f, 0x62, 0x40, 0x43, 0xbb, 0xe4, 0x1e, 0x7c, 0xe8, 0xc2, 0x7b, 0xd4
#define BLE_UUID_AIS_WC_VALUE 0xa6, 0x35, 0xf4, 0x33, 0xb0, 0xb8, 0x45, 0xa4, 0x96, 0xc4, 0x05, 0xc1, 0x7f, 0x43, 0xa2, 0x05
#define BLE_UUID_AIS_IC_VALUE 0x12, 0xbe, 0xea, 0x99, 0x3e, 0x27, 0x49, 0x73, 0x99, 0x4b, 0x86, 0xc1, 0xee, 0x37, 0x38, 0x3d
#define BLE_UUID_AIS_WWNRC_VALUE 0x37, 0xd5, 0x6b, 0x3d, 0x6d, 0xcb, 0x44, 0x36, 0x95, 0xbd, 0x46, 0x50, 0xe5, 0xb4, 0xa2, 0x50
#define BLE_UUID_AIS_NC_VALUE 0x00, 0x62, 0xa0, 0x12, 0x06, 0x3e, 0x4d, 0x11, 0xb4, 0x23, 0xed, 0xfc, 0xe2, 0xc4, 0x13, 0x4b

#define BLE_UUID_AIS_IC_CCC 0x43, 0xe4, 0x1e, 0x1c, 0x2a, 0xa0, 0x41, 0xa5, 0x8f, 0x76, 0x61, 0xf2, 0xc3, 0x82, 0x77, 0x93

#define BT_UUID_AIS_SERVICE BT_UUID_DECLARE_16(BLE_UUID_AIS_SERVICE)

#define BT_UUID_AIS_RC BT_UUID_DECLARE_16(BLE_UUID_AIS_RC)
#define BT_UUID_AIS_WC BT_UUID_DECLARE_16(BLE_UUID_AIS_WC)
#define BT_UUID_AIS_IC BT_UUID_DECLARE_16(BLE_UUID_AIS_IC)
#define BT_UUID_AIS_WWNRC BT_UUID_DECLARE_16(BLE_UUID_AIS_WWNRC)
#define BT_UUID_AIS_NC BT_UUID_DECLARE_16(BLE_UUID_AIS_NC)

#define BT_UUID_AIS_RC_VALUE BT_UUID_DECLARE_128(BLE_UUID_AIS_RC_VALUE)
#define BT_UUID_AIS_WC_VALUE BT_UUID_DECLARE_128(BLE_UUID_AIS_WC_VALUE)
#define BT_UUID_AIS_IC_VALUE BT_UUID_DECLARE_128(BLE_UUID_AIS_IC_VALUE)
#define BT_UUID_AIS_WWNRC_VALUE BT_UUID_DECLARE_128(BLE_UUID_AIS_WWNRC_VALUE)
#define BT_UUID_AIS_NC_VALUE BT_UUID_DECLARE_128(BLE_UUID_AIS_NC_VALUE)

#define BT_UUID_AIS_IC_CCC BT_UUID_DECLARE_128(BLE_UUID_AIS_IC_CCC)

static struct bt_gatt_ccc_cfg ais_ic_ccc_cfg[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg ais_nc_ccc_cfg[BT_GATT_CCC_MAX] = {};

static bool ble_srv_is_notification_enabled(uint16_t value)
{
    return value == 1 ? true : false;
}

static bool ble_srv_is_indication_enabled(uint16_t value)
{
    return value == 2 ? true : false;
}

static void ais_ic_ccc_cfg_changed(const struct bt_gatt_attr *attr,  uint16_t value)
{
    LOGD(MOD, "%s (value: %d)", __func__, value);

    if (value == BT_GATT_CCC_NOTIFY)
        LOG("CCC cfg changed to NOTIFY.\r\n");
    if (value == BT_GATT_CCC_INDICATE)
        LOG("CCC cfg changed to INDICATE.\r\n");

    g_ais->is_indication_enabled = ble_srv_is_indication_enabled(value);
    notify_svc_enabled(g_ais);
}

static void ais_nc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    LOGD(MOD, "%s (value: %d)", __func__, value);

    if (value == BT_GATT_CCC_NOTIFY)
        LOG("CCC cfg changed to NOTIFY.\r\n");
    if (value == BT_GATT_CCC_INDICATE)
        LOG("CCC cfg changed to INDICATE.\r\n");

    g_ais->is_notification_enabled = ble_srv_is_notification_enabled(value);
    notify_svc_enabled(g_ais);
}

#define FIXME_STR "FIXME"
#define FIXME_STR_SIZ (sizeof(FIXME_STR)-1)

static ssize_t read_ais_rc(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    printf("Length to read: %d, offset: %d\r\n", len, offset);
    if (len < FIXME_STR_SIZ) return 0;
    memcpy(buf, FIXME_STR, FIXME_STR_SIZ);
    return FIXME_STR_SIZ;
}

static ssize_t read_ais_wc(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    printf("Length to read: %d, offset: %d\r\n", len, offset);
    if (len < FIXME_STR_SIZ) return 0;
    memcpy(buf, FIXME_STR, FIXME_STR_SIZ);
    return FIXME_STR_SIZ;
}

static ssize_t write_ais_wc(struct bt_conn *conn,
                            const struct bt_gatt_attr *attr, const void *buf,
                            uint16_t len, uint16_t offset, uint8_t flags)
{
    notify_data(g_ais, (uint8_t *)buf, len);
    return len;
}

static ssize_t read_ais_ic(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    printf("Length to read: %d, offset: %d\r\n", len, offset);
    if (len < FIXME_STR_SIZ) return 0;
    memcpy(buf, FIXME_STR, FIXME_STR_SIZ);
    return FIXME_STR_SIZ;
}

static ssize_t read_ais_wwnrc(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr, void *buf,
                              uint16_t len, uint16_t offset)
{
    printf("Length to read: %d, offset: %d\r\n", len, offset);
    if (len < FIXME_STR_SIZ) return 0;
    memcpy(buf, FIXME_STR, FIXME_STR_SIZ);
    return FIXME_STR_SIZ;
}

static ssize_t write_ais_wwnrc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, const void *buf,
                               uint16_t len, uint16_t offset, uint8_t flags)
{
    notify_data(g_ais, (uint8_t *)buf, len);
    return len;
}

static ssize_t read_ais_nc(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    printf("Length to read: %d, offset: %d\r\n", len, offset);
    if (len < FIXME_STR_SIZ) return 0;
    memcpy(buf, FIXME_STR, FIXME_STR_SIZ);
    return FIXME_STR_SIZ;
}

static struct bt_gatt_attr ais_attrs[] = {
    /* AIS service */
    BT_GATT_PRIMARY_SERVICE2(BT_UUID_AIS_SERVICE, HDLS_AIS),

    /* RC */
    BT_GATT_CHARACTERISTIC2(BT_UUID_AIS_RC, BT_GATT_CHRC_READ, HDLC_AIS_RC),
    BT_GATT_DESCRIPTOR2(BT_UUID_AIS_RC, BT_GATT_PERM_READ | \
                        BT_GATT_PERM_READ_AUTHEN, read_ais_rc, \
                        NULL, NULL, HDLC_AIS_RC_VALUE),

    /* WC */
    BT_GATT_CHARACTERISTIC2(BT_UUID_AIS_WC, BT_GATT_CHRC_READ | \
                            BT_GATT_CHRC_WRITE, HDLC_AIS_WC),
    BT_GATT_DESCRIPTOR2(BT_UUID_AIS_WC, BT_GATT_PERM_READ | \
                        BT_GATT_PERM_WRITE, \
                        read_ais_wc, write_ais_wc, NULL, HDLC_AIS_WC_VALUE),

    /* IC */
    BT_GATT_CHARACTERISTIC2(BT_UUID_AIS_IC, BT_GATT_CHRC_READ | \
                            BT_GATT_CHRC_INDICATE, HDLC_AIS_IC),
    BT_GATT_DESCRIPTOR2(BT_UUID_AIS_IC, BT_GATT_PERM_READ, \
                        read_ais_ic, NULL, NULL, HDLC_AIS_IC_VALUE),
    BT_GATT_CCC2(ais_ic_ccc_cfg, ais_ic_ccc_cfg_changed, HDLC_AIS_IC_CCC),

    /* WWNRC */
    BT_GATT_CHARACTERISTIC2(BT_UUID_AIS_WWNRC, BT_GATT_CHRC_READ | \
                            BT_GATT_CHRC_WRITE_WITHOUT_RESP, \
                            HDLC_AIS_WWNRC),
    BT_GATT_DESCRIPTOR2(BT_UUID_AIS_WWNRC, BT_GATT_PERM_READ | \
                        BT_GATT_PERM_WRITE, read_ais_wwnrc, \
                        write_ais_wwnrc, NULL, HDLC_AIS_WWNRC_VALUE),

    /* NC */
    BT_GATT_CHARACTERISTIC2(BT_UUID_AIS_NC, BT_GATT_CHRC_READ | \
                            BT_GATT_CHRC_NOTIFY, HDLC_AIS_NC),
    BT_GATT_DESCRIPTOR2(BT_UUID_AIS_NC, BT_GATT_PERM_READ, \
                        read_ais_nc, NULL, NULL, HDLC_AIS_NC_VALUE),
    BT_GATT_CCC2(ais_nc_ccc_cfg, ais_nc_ccc_cfg_changed, HDLC_AIS_NC_CCC),
};

static struct bt_gatt_service ais_svc = BT_GATT_SERVICE(ais_attrs);

static void asign_ais_handles(ble_ais_t * p_ais)
{
    /* AIS Service. */
    p_ais->service_handle = HDLS_AIS;

    /* RC */
    p_ais->rc_handles.chrc_handle = HDLC_AIS_RC;
    p_ais->rc_handles.value_handle = HDLC_AIS_RC_VALUE;

    /* WC */
    p_ais->wc_handles.chrc_handle = HDLC_AIS_WC;
    p_ais->wc_handles.value_handle = HDLC_AIS_WC_VALUE;

    /* IC */
    p_ais->ic_handles.chrc_handle = HDLC_AIS_IC;
    p_ais->ic_handles.value_handle = HDLC_AIS_IC_VALUE;
    p_ais->ic_handles.cccd_handle = HDLC_AIS_IC_CCC;

    /* WWNRC */
    p_ais->wwnrc_handles.chrc_handle = HDLC_AIS_WWNRC;
    p_ais->wwnrc_handles.value_handle = HDLC_AIS_WWNRC_VALUE;

    /* NC */
    p_ais->nc_handles.chrc_handle = HDLC_AIS_NC;
    p_ais->nc_handles.value_handle = HDLC_AIS_NC_VALUE;
    p_ais->nc_handles.cccd_handle = HDLC_AIS_NC_CCC;
}

static void ble_event_handler(input_event_t *event, void *priv_data)
{
    LOG("%s", __func__);

    if (event->type != EV_BLE) {
        return;
    }

    switch (event->code) {
        case CODE_BLE_TX_COMPLETED:
            notify_pkt_sent(g_ais, event->value);
            break;
        default:
            break;
    }
}

extern int hci_driver_init();

uint32_t ble_ais_init(ble_ais_t * p_ais, const ble_ais_init_t * p_ais_init)
{
    LOGD(MOD, "ble_ais_init entry.");

    VERIFY_PARAM_NOT_NULL(p_ais);
    VERIFY_PARAM_NOT_NULL(p_ais_init);
    VERIFY_PARAM_NOT_NULL(p_ais_init->event_handler);
    if (p_ais_init->mtu < GATT_MTU_SIZE_DEFAULT || p_ais_init->mtu > MAX_MTU)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    aos_register_event_filter(EV_BLE, ble_event_handler, NULL);

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

    asign_ais_handles(p_ais);

    bt_conn_cb_register(&conn_callbacks);

    bt_gatt_service_register(&ais_svc);

    LOGD(MOD, "ble_ais_init exit.");

    return NRF_SUCCESS;
}

uint32_t ble_ais_set_auth(ble_ais_t * p_ais, bool is_authenticated)
{
    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    p_ais->is_authenticated = is_authenticated;
    return NRF_SUCCESS;
}

uint32_t ble_ais_set_mtu(ble_ais_t * p_ais, uint16_t mtu)
{
    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (mtu > MAX_MTU || mtu < GATT_MTU_SIZE_DEFAULT)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    p_ais->max_pkt_size = mtu - 3;
    return NRF_SUCCESS;
}

uint32_t ble_ais_send_notification(ble_ais_t * p_ais, uint8_t * p_data, uint16_t length)
{
    int err;

    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID || !p_ais->is_notification_enabled)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (length > p_ais->max_pkt_size)
    {
        return NRF_ERROR_DATA_SIZE;
    }

    err = bt_gatt_notify(NULL, &ais_attrs[11], (const void *)p_data, length);

    if (err) {
        return NRF_ERROR_GATT_NOTIFY;
    } else {
        return NRF_SUCCESS;
    }
}

slist_t params_list;

typedef struct bt_gatt_indicate_param_s {
    slist_t next;
    struct bt_gatt_indicate_params *ind_params;
} bt_gatt_indicate_param_t;

static void indicate_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        uint8_t err)
{
    bt_gatt_indicate_param_t *param;

    param = slist_first_entry(&params_list, bt_gatt_indicate_param_t, next);
    if (param) {
        aos_free(param->ind_params);
        slist_del(&param->next, &params_list);
        aos_free(param);
    }
}

uint32_t ble_ais_send_indication(ble_ais_t * p_ais, uint8_t * p_data, uint16_t length)
{
    int err;
    struct bt_gatt_indicate_params *ind_params;
    bt_gatt_indicate_param_t *param;

    VERIFY_PARAM_NOT_NULL(p_ais);

    if (p_ais->conn_handle == BLE_CONN_HANDLE_INVALID || !p_ais->is_indication_enabled)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (length > p_ais->max_pkt_size)
    {
        return NRF_ERROR_DATA_SIZE;
    }

    param = aos_malloc(sizeof(bt_gatt_indicate_param_t));
    if (param == NULL) {
        return NRF_ERROR_NULL;
    }
    ind_params = aos_malloc(sizeof(struct bt_gatt_indicate_params));
    if (ind_params == NULL) {
        aos_free(param);
        return NRF_ERROR_NULL;
    }
    param->ind_params = ind_params;
    slist_add_tail(&param->next, &params_list);

    ind_params->attr = &ais_attrs[6];
    ind_params->func = indicate_cb;
    ind_params->data = p_data;
    ind_params->len = length;

    err = bt_gatt_indicate(NULL, ind_params);

    if (err) {
        return NRF_ERROR_GATT_INDICATE;
    } else {
        return NRF_SUCCESS;
    }
}
