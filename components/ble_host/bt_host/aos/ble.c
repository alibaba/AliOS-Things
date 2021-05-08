/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>

#include <aos/ble.h>
//#include <aos/osal_debug.h>
#include <string.h>
#include <ble_os.h>
#include <bluetooth/bluetooth.h>
#include "bluetooth/conn.h" //yulong change < to "
#include <bluetooth/hci.h>
#include <bluetooth/addr.h>
//#include "conn.h" // yulong added
#include "host/conn_internal.h"

#include <bluetooth/gatt.h>
#include <settings/settings.h>
#include <hci_api.h>

#define EVENT_BLE_BASE
#define TAG "BLE"
#define BLE_LOGD(fmt, ...) LOGD(TAG, "[%s]"fmt,__FUNCTION__,  ##__VA_ARGS__)
#define BLE_LOGE(fmt, ...) LOGE(TAG, "[%s]"fmt,__FUNCTION__,  ##__VA_ARGS__)

typedef struct _ble_dev_t {
    uint8_t is_init;
    slist_t cb_list;
    uint8_t io_cap;
    init_param_t init;
} ble_dev_t;

struct _params_pool_t {
    struct bt_gatt_write_params write_params[3];
    union {
        struct bt_uuid_16 uuid16;
        struct bt_uuid_32 uuid32;
        struct bt_uuid_128 uuid128;
    } uuid;
    struct bt_gatt_discover_params disc_params[1];
    struct bt_gatt_read_params read_params[4];

} ble_param_pool = {0};

ble_dev_t ble_dev = {0};

typedef struct _ble_attr_notify_t {
    struct bt_conn *conn;
    const uint8_t *data;
    uint16_t len;
    int err;
} ble_attr_notify_t;

void ble_adv_timeout_cb();

static inline uuid_t *uuid_covert(uuid_t *uuid, const struct bt_uuid *btuuid)
{
    uuid->type = btuuid->type;

    if (btuuid->type == BT_UUID_TYPE_16) {
        UUID16(uuid) = BT_UUID_16(btuuid)->val;
    } else if (btuuid->type == BT_UUID_TYPE_32) {
        UUID32(uuid)  = BT_UUID_32(btuuid)->val;
    } else if (btuuid->type == BT_UUID_TYPE_128) {
        memcpy(UUID128(uuid), BT_UUID_128(btuuid)->val, 16);
    }

    return uuid;
}

static inline struct bt_uuid *btuuid_covert(struct bt_uuid *btuuid, const uuid_t *uuid)
{
    btuuid->type = uuid->type;

    if (uuid->type == UUID_TYPE_16) {
        BT_UUID_16(btuuid)->val =  UUID16(uuid);
    } else if (uuid->type == UUID_TYPE_32) {
        BT_UUID_32(btuuid)->val = UUID32(uuid);
    } else if (uuid->type == UUID_TYPE_128) {
        memcpy(BT_UUID_128(btuuid)->val, UUID128(uuid), 16);
    }

    return btuuid;
}

uint8_t UUID_EQUAL(uuid_t *uuid1, uuid_t *uuid2)
{
    if (UUID_TYPE(uuid1) != UUID_TYPE(uuid2)) {
        return false;
    }

    if (uuid1->type == BT_UUID_TYPE_16) {
        return (0 == memcmp(&((struct bt_uuid_16 *)uuid1)->val, &((struct bt_uuid_16 *)uuid2)->val, UUID_LEN(uuid1)));
    } else if (uuid1->type == BT_UUID_TYPE_32) {
        return (0 == memcmp(&((struct bt_uuid_32 *)uuid1)->val, &((struct bt_uuid_32 *)uuid2)->val, UUID_LEN(uuid1)));
    } else if (uuid1->type == BT_UUID_TYPE_128) {
        return (0 == memcmp(&((struct bt_uuid_128 *)uuid1)->val, &((struct bt_uuid_128 *)uuid2)->val, UUID_LEN(uuid1)));
    }

    return false;
}

uint8_t *get_uuid_val(uuid_t *uuid)
{
    if (uuid->type == BT_UUID_TYPE_16) {
        return (uint8_t *) & ((struct bt_uuid_16 *)uuid)->val;
    } else if (uuid->type == BT_UUID_TYPE_32) {
        return (uint8_t *) & ((struct bt_uuid_32 *)uuid)->val;
    } else if (uuid->type == BT_UUID_TYPE_128) {
        return (uint8_t *) & ((struct bt_uuid_128 *)uuid)->val;
    } else {
        return NULL;
    }
}

static int ble_stack_event_callback(ble_event_en event, void *event_data, uint32_t event_len)
{
    slist_t               *tmp;
    ble_event_cb_t *node;


    slist_for_each_entry_safe(&ble_dev.cb_list, tmp, node, ble_event_cb_t, next) {
        if (node->callback) {
            node->callback(event, event_data);
        }
    }

    return 0;
}

static void connected(struct bt_conn *conn, u8_t err)
{
    evt_data_gap_conn_change_t event_data;
    event_data.conn_handle = bt_conn_index(conn);

    if (err) {
        event_data.err = err;
        event_data.connected = DISCONNECTED;
    } else {
        event_data.err = 0;
        event_data.connected = CONNECTED;
    }

    ble_stack_event_callback(EVENT_GAP_CONN_CHANGE, &event_data, sizeof(event_data));
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    evt_data_gap_conn_change_t event_data;

    if (reason == BT_HCI_ERR_ADV_TIMEOUT)
    {
        ble_adv_timeout_cb();
    }
    else
    {
        event_data.conn_handle = bt_conn_index(conn);
        event_data.connected = DISCONNECTED;
        event_data.err = reason;
        ble_stack_event_callback(EVENT_GAP_CONN_CHANGE, &event_data, sizeof(event_data));

        memset(&ble_param_pool, 0, sizeof(ble_param_pool));
    }
}

static bool le_param_req_cb(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    evt_data_gap_conn_param_req_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.param = *(conn_param_t *)param;
    event_data.accept = 1;
    ble_stack_event_callback(EVENT_GAP_CONN_PARAM_REQ, &event_data, sizeof(event_data));

    if (event_data.accept) {
        return true;
    } else {
        return false;
    }
}

static void le_param_update_cb(struct bt_conn *conn, u16_t interval, u16_t latency, u16_t timeout)
{
    evt_data_gap_conn_param_update_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.interval = interval;
    event_data.latency = latency;
    event_data.timeout = timeout;
    ble_stack_event_callback(EVENT_GAP_CONN_PARAM_UPDATE, &event_data, sizeof(event_data));
}

#ifdef CONFIG_BT_SMP
static void security_changed_cb(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    evt_data_gap_security_change_t  event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.level = level;
    event_data.err = err;
    ble_stack_event_callback(EVENT_GAP_CONN_SECURITY_CHANGE, &event_data, sizeof(event_data));
}
#endif

static void scan_cb(const bt_addr_le_t *addr, s8_t rssi, u8_t adv_type, struct net_buf_simple *buf)
{
    evt_data_gap_dev_find_t event_data;

    memcpy(&event_data.dev_addr, addr, sizeof(dev_addr_t));
    event_data.adv_type = adv_type;

    if (buf->len > 31) {
        return;
    }

    memcpy(event_data.adv_data, buf->data, buf->len);
    event_data.adv_len = buf->len;
    event_data.rssi = rssi;

    ble_stack_event_callback(EVENT_GAP_DEV_FIND, &event_data, sizeof(event_data));
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char passkey_str[7] = {0};
    evt_data_smp_passkey_display_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));
    snprintf(passkey_str, 7, "%06u", passkey);

    event_data.conn_handle = bt_conn_index(conn);
    event_data.passkey = passkey_str;
    ble_stack_event_callback(EVENT_SMP_PASSKEY_DISPLAY, &event_data, sizeof(event_data));
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char passkey_str[7] = {0};
    evt_data_smp_passkey_confirm_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));
    snprintf(passkey_str, 7, "%06u", passkey);

    event_data.conn_handle = bt_conn_index(conn);
    event_data.passkey = passkey_str;
    ble_stack_event_callback(EVENT_SMP_PASSKEY_CONFIRM, &event_data, sizeof(event_data));
}

static void auth_passkey_entry(struct bt_conn *conn)
{
    evt_data_smp_passkey_enter_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    ble_stack_event_callback(EVENT_SMP_PASSKEY_ENTER, &event_data, sizeof(event_data));
}

static void auth_cancel(struct bt_conn *conn)
{
    evt_data_smp_cancel_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    ble_stack_event_callback(EVENT_SMP_CANCEL, &event_data, sizeof(event_data));
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    evt_data_smp_pairing_confirm_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    ble_stack_event_callback(EVENT_SMP_PAIRING_CONFIRM, &event_data, sizeof(event_data));
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    evt_data_smp_pairing_complete_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    event_data.bonded = bonded;
    event_data.err = 0;
    ble_stack_event_callback(EVENT_SMP_PAIRING_COMPLETE, &event_data, sizeof(event_data));
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    evt_data_smp_pairing_complete_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    event_data.err = reason;
    ble_stack_event_callback(EVENT_SMP_PAIRING_COMPLETE, &event_data, sizeof(event_data));
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_req = le_param_req_cb,
    .le_param_updated = le_param_update_cb,
#ifdef CONFIG_BT_SMP
    .identity_resolved = NULL,
    .security_changed = security_changed_cb,
#endif
};

static struct bt_conn_auth_cb auth_callbacks = {
    .cancel = auth_cancel,
    .pairing_failed = auth_pairing_failed,
    .pairing_complete = auth_pairing_complete,
};

int ble_stack_init(init_param_t *param)
{
    int ret = BLE_STACK_OK;

    if (param == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    if (ble_dev.is_init) {
        return -BLE_STACK_ERR_ALREADY;
    }

    hci_h4_driver_init();
    ble_dev.init = *param;

    if (ble_dev.init.dev_addr) {
        bt_addr_le_t addr = *(bt_addr_le_t *)ble_dev.init.dev_addr;

        if (addr.type == BT_ADDR_LE_RANDOM) {
            ret = bt_set_id_addr(&addr);

            if (ret) {
                ret = -BLE_STACK_ERR_PARAM;
                return ret;
            }
        } else if (addr.type == BT_ADDR_LE_PUBLIC) {
            extern int bt_set_bdaddr(const bt_addr_le_t *addr);
            ret = bt_set_bdaddr(&addr);

            if (ret) {
                ret = -BLE_STACK_ERR_PARAM;
                return ret;
            }
        } else if (addr.type == BT_ADDR_LE_PUBLIC_ID) {
        } else if (addr.type == BT_ADDR_LE_RANDOM_ID) {
        } else {
            ret = -BLE_STACK_ERR_PARAM;
            return ret;
        }
    }

    bt_conn_cb_register(&conn_callbacks);

    ret = bt_enable(NULL);

    if (ret) {
        return ret;
    }

    if (ble_dev.init.dev_name) {
        ret = bt_set_name(ble_dev.init.dev_name);

        if (ret) {
            ret = -BLE_STACK_ERR_PARAM;
            return ret;
        }
    }

    slist_init(&ble_dev.cb_list);

    ble_stack_setting_load();
    #if AOS_COMP_CLI
    cli_reg_cmd_ble();
    #endif
    ble_dev.is_init = 1;

    return ret;
}

int ble_stack_iocapability_set(uint8_t io_cap)
{
    int ret = 0;

    int data = (io_cap & (IO_CAP_IN_NONE | IO_CAP_IN_YESNO | IO_CAP_IN_KEYBOARD));

    if ((data != IO_CAP_IN_NONE) && (data != IO_CAP_IN_YESNO) && (data != IO_CAP_IN_KEYBOARD)) {
        return -BLE_STACK_ERR_PARAM;
    }

    int sdata = (io_cap & (IO_CAP_OUT_DISPLAY | IO_CAP_OUT_NONE));

    if ((sdata != IO_CAP_OUT_DISPLAY) && (sdata != IO_CAP_OUT_NONE)) {
        return -BLE_STACK_ERR_PARAM;
    }

    if ((io_cap & IO_CAP_IN_NONE) && (io_cap & IO_CAP_OUT_DISPLAY)) {
        auth_callbacks.passkey_display = auth_passkey_display;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    } else if ((io_cap & IO_CAP_IN_YESNO) && (io_cap & IO_CAP_OUT_DISPLAY)) {
        auth_callbacks.passkey_display = auth_passkey_display;
        auth_callbacks.passkey_confirm = auth_passkey_confirm;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    } else if ((io_cap & IO_CAP_IN_KEYBOARD) && (io_cap & IO_CAP_OUT_DISPLAY)) {
        auth_callbacks.passkey_display = auth_passkey_display;
        auth_callbacks.passkey_entry = auth_passkey_entry;
        auth_callbacks.passkey_confirm = auth_passkey_confirm;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    } else if ((io_cap & IO_CAP_IN_KEYBOARD) && (io_cap & IO_CAP_OUT_NONE)) {
        auth_callbacks.passkey_entry = auth_passkey_entry;
        auth_callbacks.passkey_confirm = auth_passkey_confirm;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    }

    if (auth_callbacks.cancel != NULL) {
        ret = bt_conn_auth_cb_register(&auth_callbacks);

        if (ret) {
            return ret;
        }
    }

    ble_dev.io_cap = io_cap;
    return ret;
}

int ble_stack_event_register(ble_event_cb_t *callback)
{
    if (!ble_dev.is_init) {
        return -BLE_STACK_ERR_INIT;
    }

    if (NULL == callback) {
        return -BLE_STACK_ERR_NULL;
    }

    slist_add(&callback->next, &ble_dev.cb_list);
    return 0;
}

int ble_stack_adv_start(adv_param_t *param)
{
    struct bt_le_adv_param p = {0};

    if (!ble_dev.is_init) {
        return -BLE_STACK_ERR_INIT;
    }

    if (param == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    if ((param->ad == NULL || (param->ad != NULL && param->ad_num == 0))
        || ((param->sd == NULL && param->sd_num != 0) || (param->sd != NULL && param->sd_num == 0))
       ) {
        return -BLE_STACK_ERR_PARAM;
    }

    if (param->channel_map > ADV_DEFAULT_CHAN_MAP) {
        return -BLE_STACK_ERR_PARAM;
    }

    if (param->filter_policy > ADV_FILTER_POLICY_ALL_REQ) {
        return -BLE_STACK_ERR_PARAM;
    }

    if (param->type == ADV_IND || param->type == ADV_DIRECT_IND || param->type == ADV_DIRECT_IND_LOW_DUTY) {
        p.options |= BT_LE_ADV_OPT_CONNECTABLE;
    } else if (param->type == ADV_NONCONN_IND || param->type == ADV_SCAN_IND) {
        p.options |= BT_LE_ADV_OPT_NONE;
    } else {
        return -BLE_STACK_ERR_PARAM;
    }

    if (param->type == ADV_DIRECT_IND_LOW_DUTY) {
        p.options |= BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY;
    }

    p.options |= BT_LE_ADV_OPT_ONE_TIME;

    if (ble_dev.init.dev_name) {
        p.options |= BT_LE_ADV_OPT_USE_NAME;
    }

    if (param->filter_policy == ADV_FILTER_POLICY_ALL_REQ)
    {
        p.options |= (BT_LE_ADV_OPT_FILTER_SCAN_REQ | BT_LE_ADV_OPT_FILTER_CONN);
    }
    else if (param->filter_policy == ADV_FILTER_POLICY_CONN_REQ)
    {
        p.options |= BT_LE_ADV_OPT_FILTER_CONN;
    }
    else if (param->filter_policy == ADV_FILTER_POLICY_SCAN_REQ)
    {
        p.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
    }

    p.options |= BT_LE_ADV_OPT_USE_IDENTITY;

    p.id = 0;
    p.interval_min = param->interval_min;
    p.interval_max = param->interval_max;
    p.channel_map = param->channel_map;

    if (param->type == ADV_DIRECT_IND_LOW_DUTY || param->type == ADV_DIRECT_IND) {
        if (!bt_addr_le_cmp((const bt_addr_le_t *)&param->direct_peer_addr, BT_ADDR_LE_ANY)) {
            return -BLE_STACK_ERR_PARAM;
        }

        param->ad = NULL;
        param->ad_num = 0;
        param->sd = NULL;
        param->sd_num = 0;
    }

    bt_addr_le_t peer_addr = {0};

    if (param->type == ADV_DIRECT_IND_LOW_DUTY || param->type == ADV_DIRECT_IND)
    {
        p.peer = &peer_addr;
        peer_addr.type = param->direct_peer_addr.type;
        memcpy(peer_addr.a.val, param->direct_peer_addr.val, 6);
    }

    return bt_le_adv_start(&p, (struct bt_data *)param->ad, param->ad_num,
                            (struct bt_data *)param->sd, param->sd_num);;
}

int ble_stack_adv_stop()
{
    int ret;

    if (!ble_dev.is_init) {
        return -BLE_STACK_ERR_INIT;
    }

    ret = bt_le_adv_stop();

    return ret;
}

int ble_stack_scan_start(const scan_param_t *param)
{
    int ret;
    struct bt_le_scan_param p = {0};

    if (param == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    if (!ble_dev.is_init) {
        return -BLE_STACK_ERR_INIT;
    }

    if (param->scan_filter > SCAN_FILTER_POLICY_WHITE_LIST) {
        return  -BLE_STACK_ERR_PARAM;
    }

    p.type = param->type;
    p.interval = param->interval;
    p.window = param->window;

    if (param->filter_dup == SCAN_FILTER_DUP_ENABLE)
    {
        p.options |= BT_LE_SCAN_OPT_FILTER_DUPLICATE;
    }

    if (param->scan_filter == SCAN_FILTER_POLICY_WHITE_LIST)
    {
        p.options |= BT_LE_SCAN_OPT_FILTER_WHITELIST;
    }

    ret = bt_le_scan_start(&p, scan_cb);

    return ret;
}

int ble_stack_scan_stop()
{
    int ret;

    if (!ble_dev.is_init) {
        return -BLE_STACK_ERR_INIT;
    }

    ret = bt_le_scan_stop();

    return ret;
}

static ssize_t gatt_read_handle(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr, void *buf,
                                u16_t len, u16_t offset)
{
    evt_data_gatt_char_read_t event_data = {0};

    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = attr->handle;
    event_data.offset = offset;

    ble_stack_event_callback(EVENT_GATT_CHAR_READ, &event_data, sizeof(event_data));

    if (event_data.len < 0) {
        return event_data.len;
    }

    if (event_data.data && event_data.len > 0) {
        return bt_gatt_attr_read(conn, attr, buf, len, offset, event_data.data, event_data.len);
    }

    return 0;
}

static ssize_t gatt_write_handle(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, u16_t len, u16_t offset,
                                 u8_t flags)
{
    evt_data_gatt_char_write_t event_data;

    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = attr->handle;
    event_data.data = buf;
    event_data.len = len;
    event_data.offset = offset;
    event_data.flag = flags;

    ble_stack_event_callback(EVENT_GATT_CHAR_WRITE, &event_data, sizeof(event_data));

    return event_data.len;
}

static int gatt_cfg_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, u16_t value)
{
    evt_data_gatt_char_ccc_write_t event_data;

    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = attr->handle;
    event_data.ccc_value = value;
    event_data.allow_write = true;

    ble_stack_event_callback(EVENT_GATT_CHAR_CCC_WRITE, &event_data, sizeof(event_data));

    return event_data.allow_write? true:false;
}

static bool gatt_cfg_match(struct bt_conn *conn, const struct bt_gatt_attr *attr)
{
    evt_data_gatt_char_ccc_match_t event_data;

    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = attr->handle;
    event_data.is_matched = true;

    ble_stack_event_callback(EVENT_GATT_CHAR_CCC_MATCH, &event_data, sizeof(event_data));

    return event_data.is_matched? true:false;
}

static void cfg_changed_cb(const struct bt_gatt_attr *attr,
                           u16_t value)
{
    evt_data_gatt_char_ccc_change_t event_data;

    event_data.char_handle = attr->handle;
    event_data.ccc_value = value;

    ble_stack_event_callback(EVENT_GATT_CHAR_CCC_CHANGE, &event_data, sizeof(event_data));
}

int uuid_compare(const uuid_t *uuid1, const uuid_t *uuid2)
{
    return bt_uuid_cmp((struct bt_uuid *)uuid1, (struct bt_uuid *)uuid2);
}

int ble_stack_gatt_registe_service(gatt_service *s, gatt_attr_t attrs[], uint16_t attr_num)
{
    int ret;
    int i;
    struct bt_gatt_service  *ss;

    ss = (struct bt_gatt_service *)s;

    if (!ble_dev.is_init) {
        return -BLE_STACK_ERR_INIT;
    }

    if (attrs == NULL || attr_num == 0) {
        return -BLE_STACK_ERR_NULL;
    }

    ss->attr_count = attr_num;
    ss->attrs = (struct bt_gatt_attr *)attrs;

    for (i = 0; i < attr_num; i++) {
        if (UUID_EQUAL(attrs[i].uuid, UUID_GATT_PRIMARY) || UUID_EQUAL(attrs[i].uuid, UUID_GATT_SECONDARY)
            || UUID_EQUAL(attrs[i].uuid, UUID_GATT_INCLUDE)) {
            ss->attrs[i].read = bt_gatt_attr_read_service;
        } else if (UUID_EQUAL(attrs[i].uuid, UUID_GATT_CHRC)) {
            if (i + 1 >= attr_num) {
                return -BLE_STACK_ERR_PARAM;
            }

            ss->attrs[i].read = bt_gatt_attr_read_chrc;
            ss->attrs[i + 1].read  = gatt_read_handle;
            ss->attrs[i + 1].write = gatt_write_handle;
            i++;
        } else if (UUID_EQUAL(attrs[i].uuid, UUID_GATT_CCC)) {
            struct _bt_gatt_ccc *val = NULL;
            val = (struct _bt_gatt_ccc *)(attrs[i].user_data);
            val->cfg_changed = cfg_changed_cb;
            val->cfg_write = gatt_cfg_write;
            val->cfg_match = gatt_cfg_match;
            ss->attrs[i].perm = GATT_PERM_READ | GATT_PERM_WRITE;
            ss->attrs[i].read = bt_gatt_attr_read_ccc;
            ss->attrs[i].write = bt_gatt_attr_write_ccc;
            ss->attrs[i].user_data = val;
        } else if (UUID_EQUAL(attrs[i].uuid, UUID_GATT_CEP)) {
            ss->attrs[i].read = bt_gatt_attr_read_cep;
        } else if (UUID_EQUAL(attrs[i].uuid, UUID_GATT_CUD)) {
            ss->attrs[i].read = gatt_read_handle;
            ss->attrs[i].write = gatt_write_handle;
        } else if (UUID_EQUAL(attrs[i].uuid, UUID_GATT_CPF)) {
            ss->attrs[i].read = bt_gatt_attr_read_cpf;
        } else {
            ss->attrs[i].read = gatt_read_handle;
            ss->attrs[i].write = gatt_write_handle;
        }
    }

    ret = bt_gatt_service_register(ss);

    if (ret < 0) {
        return ret;
    }

    return ss->attrs[0].handle;
}

static uint8_t ble_gatt_attr_notify(const struct bt_gatt_attr *attr, void *user_data)
{
    ble_attr_notify_t *notify = (ble_attr_notify_t *)user_data;
    notify->err = bt_gatt_notify(notify->conn, attr, notify->data, notify->len);

    return BT_GATT_ITER_STOP;
}

static void ble_gatt_indicate_cb(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 u8_t err)
{
    evt_data_gatt_indicate_cb_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = attr->handle;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GATT_INDICATE_CB, &event_data, sizeof(event_data));
}


static uint8_t ble_gatt_attr_indicate(const struct bt_gatt_attr *attr, void *user_data)
{
    ble_attr_notify_t *notify = (ble_attr_notify_t *)user_data;
    static struct bt_gatt_indicate_params param = {0};
    param.attr = attr;
    param.func = ble_gatt_indicate_cb;
    param.data = notify->data;
    param.len = notify->len;

    notify->err = bt_gatt_indicate(notify->conn, &param);

    return BT_GATT_ITER_STOP;
}


int ble_stack_gatt_notificate(int16_t conn_handle, uint16_t char_handle, const uint8_t *data, uint16_t len)
{
    ble_attr_notify_t notify;
    struct bt_conn *conn;

    if (conn_handle < 0 || data == NULL || len == 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    notify.conn = conn;
    notify.data = data;
    notify.len = len;
    notify.err = 0;

    bt_gatt_foreach_attr(char_handle, char_handle,
                         ble_gatt_attr_notify, &notify);

    if (notify.conn) {
        bt_conn_unref(notify.conn);
    }

    return notify.err;
}

int ble_stack_gatt_indicate(int16_t conn_handle, int16_t char_handle, const uint8_t *data, uint16_t len)
{
    ble_attr_notify_t notify;
    struct bt_conn *conn;

    if (conn_handle < 0 || data == NULL || len == 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    notify.conn = conn;
    notify.data = data;
    notify.len = len;
    notify.err = 0;

    bt_gatt_foreach_attr(char_handle, char_handle,
                         ble_gatt_attr_indicate, &notify);

    if (notify.conn) {
        bt_conn_unref(notify.conn);
    }

    return notify.err;
}

void ble_adv_timeout_cb()
{
    ble_stack_event_callback(EVENT_GAP_ADV_TIMEOUT, NULL, 0);
}

void mtu_exchange_cb(struct bt_conn *conn, u8_t err,
                     struct bt_gatt_exchange_params *params)
{
    evt_data_gatt_mtu_exchange_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.err = err;

    ble_stack_event_callback(EVENT_GATT_MTU_EXCHANGE, &event_data, sizeof(event_data));
}

int ble_stack_gatt_mtu_get(int16_t conn_handle)
{
    int ret;
    struct bt_conn *conn = NULL;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    ret = bt_gatt_get_mtu(conn);

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_gatt_mtu_exchange(int16_t conn_handle)
{
    static struct bt_gatt_exchange_params params;
    int ret;
    struct bt_conn *conn;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    params.func = mtu_exchange_cb;
    ret = bt_gatt_exchange_mtu(conn, &params);

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

static u8_t discover_func(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          struct bt_gatt_discover_params *params)
{
    int event = EVENT_STACK_UNKNOWN;
    union {
        evt_data_gatt_discovery_svc_t svc;
        evt_data_gatt_discovery_inc_svc_t svc_inc;
        evt_data_gatt_discovery_char_t char_c;
        evt_data_gatt_discovery_char_des_t char_des;
        evt_data_gatt_discovery_complete_t comp;
    } event_data;

    if (attr == NULL) {
        if (params) {
            params->func = NULL;
        }

        event_data.comp.conn_handle = bt_conn_index(conn);
        event_data.comp.err = 0;
        event = EVENT_GATT_DISCOVERY_COMPLETE;
        ble_stack_event_callback(event, &event_data, sizeof(event_data));
        return BT_GATT_ITER_STOP;
    }

    if (params->type == BT_GATT_DISCOVER_PRIMARY) {
        struct bt_gatt_service_val *value = attr->user_data;
        event_data.svc.conn_handle = bt_conn_index(conn);
        event_data.svc.start_handle = attr->handle;
        event_data.svc.end_handle = value->end_handle;
        uuid_covert(&event_data.svc.uuid, value->uuid);
        event = EVENT_GATT_DISCOVERY_SVC;
    } else if (params->type == BT_GATT_DISCOVER_INCLUDE) {
        struct bt_gatt_include *inc_val = (struct bt_gatt_include *)attr->user_data;
        event_data.svc_inc.conn_handle = bt_conn_index(conn);
        event_data.svc_inc.attr_handle = attr->handle;
        event_data.svc_inc.start_handle = inc_val->start_handle;
        event_data.svc_inc.end_handle = inc_val->end_handle;
        uuid_covert(&event_data.svc_inc.uuid, inc_val->uuid);
        event = EVENT_GATT_DISCOVERY_INC_SVC;
    } else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
        struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;
        event_data.char_c.conn_handle = bt_conn_index(conn);
        event_data.char_c.attr_handle = attr->handle;
        event_data.char_c.val_handle = attr->handle + 1;
        event_data.char_c.props = chrc->properties;
        uuid_covert(&event_data.char_c.uuid, chrc->uuid);
        event = EVENT_GATT_DISCOVERY_CHAR;
    } else if (params->type == BT_GATT_DISCOVER_DESCRIPTOR) {
        event_data.char_des.conn_handle = bt_conn_index(conn);
        event_data.char_des.attr_handle = attr->handle;
        uuid_covert(&event_data.char_des.uuid, params->uuid);
        event = EVENT_GATT_DISCOVERY_CHAR_DES;
    }

    ble_stack_event_callback(event, &event_data, sizeof(event_data));

    return BT_GATT_ITER_CONTINUE;
}

int ble_stack_gatt_discovery(int16_t conn_handle,
                             gatt_discovery_type_en type,
                             uuid_t *uuid,
                             uint16_t start_handle,
                             uint16_t end_handle)
{
    int ret;
    struct bt_conn *conn;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    if (ble_param_pool.disc_params[0].func) {
        return -BLE_STACK_ERR_ALREADY;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    struct bt_gatt_discover_params *params = &ble_param_pool.disc_params[0];

    struct bt_uuid *u;

    if (uuid) {
        u = (struct bt_uuid *)&ble_param_pool.uuid;
        params->uuid = btuuid_covert(u, uuid);
    } else {
        params->uuid = NULL;
    }

    if (type == GATT_FIND_PRIMARY_SERVICE) {
        params->type = BT_GATT_DISCOVER_PRIMARY;
    } else if (type == GATT_FIND_INC_SERVICE) {
        params->type = BT_GATT_DISCOVER_INCLUDE;
    } else if (type == GATT_FIND_CHAR) {
        params->type = BT_GATT_DISCOVER_CHARACTERISTIC;
    } else if (type == GATT_FIND_CHAR_DESCRIPTOR) {
        params->type = BT_GATT_DISCOVER_DESCRIPTOR;
    } else {
        return -BLE_STACK_ERR_PARAM;
    }

    params->start_handle = start_handle;
    params->end_handle = end_handle;
    params->func = discover_func;

    ret = bt_gatt_discover(conn, params);

    if (ret) {
        params->func = NULL;
    }

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

static void gatt_write_cb(struct bt_conn *conn, u8_t err, struct bt_gatt_write_params *params)
{
    if (NULL == params) {
        return;
    }

    evt_data_gatt_write_cb_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.err = err;
    event_data.char_handle = params->handle;

    if (params) {
        params->func = NULL;
    }

    ble_stack_event_callback(EVENT_GATT_CHAR_WRITE_CB, &event_data, sizeof(event_data));
}

int ble_stack_gatt_notify_cb(int16_t conn_handle, uint16_t attr_handle, const uint8_t *data, uint16_t len)
{
    evt_data_gatt_notify_t event_data;
    event_data.conn_handle = conn_handle;
    event_data.char_handle = attr_handle;
    event_data.data = data;
    event_data.len = len;

    ble_stack_event_callback(EVENT_GATT_NOTIFY, &event_data, sizeof(event_data));
    return 0;
}

int ble_stack_gatt_write(int16_t conn_handle, uint16_t attr_handle, uint8_t *data, uint16_t len, uint16_t offset, gatt_write_en type)
{
    int ret = -BLE_STACK_ERR_INTERNAL;
    int i;
    struct bt_gatt_write_params *params = NULL;

    if ((data == NULL) || (len == 0)) {
        return -BLE_STACK_ERR_PARAM;
    }

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_CONN;
    }

    if (attr_handle <= 0) {
        return -BLE_STACK_ERR_NULL;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    if (type == GATT_WRITE) {

        for (i = 0; i < sizeof(ble_param_pool.write_params) / (sizeof(struct bt_gatt_write_params)); i++) {
            if (ble_param_pool.write_params[i].func == NULL) {
                break;
            }
        }

        if (i == sizeof(ble_param_pool.write_params) / (sizeof(struct bt_gatt_write_params))) {
            return -BLE_STACK_ERR_NULL;
        }

        params = &ble_param_pool.write_params[i];

        params->handle = attr_handle;
        params->data = data;
        params->length = len;
        params->offset = offset;
        params->func = gatt_write_cb;
        ret = bt_gatt_write(conn, params);
    } else if (type == GATT_WRITE_WITHOUT_RESPONSE || type == GATT_WRITE_SINGED) {
        ret = bt_gatt_write_without_response(conn, attr_handle, data, len, type == GATT_WRITE_SINGED);
    }

    if (ret && params) {
        params->func = NULL;
    }

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

u8_t gatt_read_cb(struct bt_conn *conn, u8_t err,
                  struct bt_gatt_read_params *params,
                  const void *data, u16_t length)
{
    evt_data_gatt_read_cb_t event_data = {0};

    event_data.conn_handle = bt_conn_index(conn);

    if (err || data == NULL || length == 0) {
        if (params) {
            params->func = NULL;
        }
        event_data.err = err;
        event_data.data = NULL;
        event_data.len = 0;
        ble_stack_event_callback(EVENT_GATT_CHAR_READ_CB, &event_data, sizeof(event_data));
        return BT_GATT_ITER_STOP;
    }

    event_data.data = data;
    event_data.len = length;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GATT_CHAR_READ_CB, &event_data, sizeof(event_data));

    return BT_GATT_ITER_CONTINUE;
}


int ble_stack_gatt_read(int16_t conn_handle, uint16_t attr_handle, uint16_t offset)
{
    int ret = -BLE_STACK_ERR_INTERNAL;
    struct bt_conn *conn;
    int i;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    for (i = 0; i < sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params)); i++) {
        if (ble_param_pool.read_params[i].func == NULL) {
            break;
        }
    }

    if (i == sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params))) {
        return -BLE_STACK_ERR_NULL;
    }

    struct bt_gatt_read_params *params = &ble_param_pool.read_params[i];

    params->func = gatt_read_cb;

    params->handle_count = 1;

    params->single.handle = attr_handle;

    params->single.offset = offset;

    ret = bt_gatt_read(conn, params);

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_gatt_read_multiple(int16_t conn_handle, uint16_t attr_count, uint16_t attr_handle[])
{
    int ret = -BLE_STACK_ERR_INTERNAL;
    //struct bt_gatt_read_params params;
    struct bt_conn *conn;
    //uint16_t *attr_list;
    int i;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    if (attr_handle == NULL || attr_count <= 1) {
        return -BLE_STACK_ERR_PARAM;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn == NULL) {
        return -BLE_STACK_ERR_CONN;
    }

    for (i = 0; i < sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params)); i++) {
        if (ble_param_pool.read_params[i].func == NULL) {
            break;
        }
    }

    if (i == sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params))) {
        return -BLE_STACK_ERR_NULL;
    }

    struct bt_gatt_read_params *params = &ble_param_pool.read_params[i];

    params->func = gatt_read_cb;

    params->handle_count = attr_count;

    params->handles = attr_handle;

    ret = bt_gatt_read(conn, params);

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_get_local_addr(dev_addr_t *addr)
{
    struct bt_le_oob oob;

    if (addr == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    bt_le_oob_get_local(0, &oob);
    memcpy(addr, &oob.addr, sizeof(dev_addr_t));

    return 0;
}

int ble_stack_connect(dev_addr_t *peer_addr, conn_param_t *param, uint8_t auto_connect)
{
    int ret;
    struct bt_conn *conn = NULL;
    int16_t conn_handle;
    bt_addr_le_t peer;
    struct bt_le_conn_param conn_param;

    if (peer_addr == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

#if !defined(CONFIG_BT_WHITELIST)
    if (auto_connect) {
        return -BLE_STACK_ERR_PARAM;
    }
#endif
    if (auto_connect > 1) {
        return -BLE_STACK_ERR_PARAM;
    }

    peer = *(bt_addr_le_t *)peer_addr;

    if (param) {
        conn_param = *(struct bt_le_conn_param *)param;

        if (auto_connect) {
#if !defined(CONFIG_BT_WHITELIST)
            ret = bt_le_set_auto_conn(&peer, &conn_param);

            if (ret) {
                return ret;
            }

            conn = bt_conn_lookup_addr_le(0, &peer);
#endif
        } else {
            conn = bt_conn_create_le(&peer, &conn_param);
        }
    } else if (!auto_connect) {
#if !defined(CONFIG_BT_WHITELIST)
        ret = bt_le_set_auto_conn(&peer, NULL);

        if (ret) {
            return ret;
        }
#endif
    } else {
        return -BLE_STACK_ERR_PARAM;
    }

    if (conn) {
        conn_handle = bt_conn_index(conn);
        bt_conn_unref(conn);
        return conn_handle;
    }
    (void)ret;
    return -BLE_STACK_ERR_CONN;
}

int ble_stack_disconnect(int16_t conn_handle)
{
    int ret = -BLE_STACK_ERR_PARAM;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_connect_info_get(int16_t conn_handle, connect_info_t *info)
{
    if (conn_handle < 0 || NULL == info) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        info->conn_handle = conn_handle;
        info->role = conn->role;
        info->interval = conn->le.interval;
        info->latency = conn->le.latency;
        info->timeout = conn->le.timeout;

        if (conn->role == BT_HCI_ROLE_MASTER) {
            info->local_addr.type = conn->le.init_addr.type;
            memcpy(info->local_addr.val, conn->le.init_addr.a.val, 6);
            info->peer_addr.type = conn->le.resp_addr.type;
            memcpy(info->peer_addr.val, conn->le.resp_addr.a.val, 6);
        } else {
            info->local_addr.type = conn->le.resp_addr.type;
            memcpy(info->local_addr.val, conn->le.resp_addr.a.val, 6);
            info->peer_addr.type = conn->le.init_addr.type;
            memcpy(info->peer_addr.val, conn->le.init_addr.a.val, 6);
        }

        bt_conn_unref(conn);
    } else {
        return -BLE_STACK_ERR_PARAM;
    }

    return 0;
}

int ble_stack_check_conn_params(const conn_param_t *param)
{
    if (param->interval_min > param->interval_max ||
        param->interval_min < 6 || param->interval_max > 3200) {
        return false;
    }

    if (param->latency > 499) {
        return false;
    }

    if (param->timeout < 10 || param->timeout > 3200 ||
        ((4 * param->timeout) <=
         ((1 + param->latency) * param->interval_max))) {
        return false;
    }

    return true;
}

int ble_stack_security(int16_t conn_handle, security_en level)
{
    int ret = -BLE_STACK_ERR_PARAM;
    struct bt_conn *conn;

    if (conn_handle < 0) {
        return ret;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_security(conn, level);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_connect_param_update(int16_t conn_handle, conn_param_t *param)
{
    int ret = -BLE_STACK_ERR_PARAM;
    struct bt_conn *conn;

    if (param == NULL || conn_handle < 0) {
        return ret;
    }

    conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_le_param_update(conn, (struct bt_le_conn_param *)param);
        bt_conn_unref(conn);
    }

    return ret;
}


int ble_stack_smp_passkey_entry(int16_t conn_handle, uint32_t passkey)
{
    int ret = -BLE_STACK_ERR_PARAM;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_auth_passkey_entry(conn, passkey);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_smp_cancel(int16_t conn_handle)
{
    int ret = -BLE_STACK_ERR_PARAM;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_auth_cancel(conn);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_smp_passkey_confirm(int16_t conn_handle)
{
    int ret = -BLE_STACK_ERR_PARAM;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_auth_passkey_confirm(conn);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_smp_pairing_confirm(int16_t conn_handle)
{
    int ret = -BLE_STACK_ERR_PARAM;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
        ret = bt_conn_auth_pairing_confirm(conn);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_enc_key_size_get(int16_t conn_handle)
{
    int ret = -BLE_STACK_ERR_PARAM;

    if (conn_handle < 0) {
        return -BLE_STACK_ERR_PARAM;
    }

    struct bt_conn *conn = bt_conn_lookup_index(conn_handle);

    if (conn) {
#if defined(CONFIG_BT_SMP)
        ret = bt_conn_enc_key_size(conn);
#else
        ret = 0;
#endif
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_setting_load()
{
#ifdef CONFIG_BT_SETTINGS

    int ret = 0;
    static uint8_t is_setting_loaded = 0;
    if (is_setting_loaded)
    {
        return 0;
    }

    ret = settings_load();
    if (!ret)
    {
        is_setting_loaded = 1;
    }

    return ret;
#else
    return 0;
#endif
}

int ble_stack_dev_unpair(dev_addr_t *peer_addr)
{
    bt_addr_le_t peer;

    if (peer_addr == NULL) {
        return bt_unpair(0, NULL);
    }

    peer = *(bt_addr_le_t *)peer_addr;

    return bt_unpair(0, &peer);
}

int ble_stack_white_list_clear()
{
    return bt_le_whitelist_clear();
}

int ble_stack_white_list_add(dev_addr_t *peer_addr)
{
    return bt_le_whitelist_add((bt_addr_le_t *)peer_addr);
}

int ble_stack_white_list_remove(dev_addr_t *peer_addr)
{
    return bt_le_whitelist_rem((bt_addr_le_t *)peer_addr);
}

int ble_stack_white_list_size()
{
    uint8_t size = 0;
    int ret;
    ret = bt_le_whitelist_size(&size);

    if (ret) {
        return -ret;
    }

    return size;
}

int ble_stack_set_name(const char *name)
{
    return bt_set_name(name);
}

char *uuid_str(uuid_t *uuid)
{
    static char uuid_str[37];
    uint32_t tmp1, tmp5;
    uint16_t tmp0, tmp2, tmp3, tmp4;

    switch (uuid->type) {
        case UUID_TYPE_16:
            snprintf(uuid_str, 37, "%04x", UUID16(uuid));
            break;

        case UUID_TYPE_32:
            snprintf(uuid_str, 37, "%04x", UUID32(uuid));
            break;

        case UUID_TYPE_128:
            memcpy(&tmp0, &UUID128(uuid)[0], sizeof(tmp0));
            memcpy(&tmp1, &UUID128(uuid)[2], sizeof(tmp1));
            memcpy(&tmp2, &UUID128(uuid)[6], sizeof(tmp2));
            memcpy(&tmp3, &UUID128(uuid)[8], sizeof(tmp3));
            memcpy(&tmp4, &UUID128(uuid)[10], sizeof(tmp4));
            memcpy(&tmp5, &UUID128(uuid)[12], sizeof(tmp5));

            snprintf(uuid_str, 37, "%08x-%04x-%04x-%04x-%08x%04x",
                     tmp5, tmp4, tmp3, tmp2, tmp1, tmp0);
            break;

        default:
            memset(uuid_str, 0, 37);
    }

    return uuid_str;
}

