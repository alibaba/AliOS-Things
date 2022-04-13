/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#ifndef __BLE_H__
#define __BLE_H__

#include <stdint.h>

#include <aos/list.h>
#include <aos/uuid.h>
#include <aos/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_ARRAY_NUM(a) (sizeof(a) / sizeof((a)[0]))
#define BLE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define BLE_CHAR_RANGE_CHECK(a, b, c, d) \
    do { \
        if (((c) >= (a)) && ((c) < ((a) + (b)))) \
        { \
            d = c - a; \
        } \
        else \
        { \
            return 0; \
        } \
    } while(0);

#define ATT_ERR_INVALID_HANDLE          0x01
#define ATT_ERR_READ_NOT_PERMITTED      0x02
#define ATT_ERR_WRITE_NOT_PERMITTED     0x03
#define ATT_ERR_INVALID_PDU             0x04
#define ATT_ERR_AUTHENTICATION          0x05
#define ATT_ERR_NOT_SUPPORTED           0x06
#define ATT_ERR_INVALID_OFFSET          0x07
#define ATT_ERR_AUTHORIZATION           0x08
#define ATT_ERR_PREPARE_QUEUE_FULL      0x09
#define ATT_ERR_ATTRIBUTE_NOT_FOUND     0x0a
#define ATT_ERR_ATTRIBUTE_NOT_LONG      0x0b
#define ATT_ERR_ENCRYPTION_KEY_SIZE     0x0c
#define ATT_ERR_INVALID_ATTRIBUTE_LEN   0x0d
#define ATT_ERR_UNLIKELY                0x0e
#define ATT_ERR_INSUFFICIENT_ENCRYPTION 0x0f
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE  0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES  0x11

typedef struct _conn_param_t {
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
} conn_param_t;

enum {
    BLE_STACK_OK = 0,
    BLE_STACK_ERR_NULL,
    BLE_STACK_ERR_PARAM,
    BLE_STACK_ERR_INTERNAL,
    BLE_STACK_ERR_INIT,
    BLE_STACK_ERR_CONN,
    BLE_STACK_ERR_ALREADY,
};

typedef enum {
    DEV_ADDR_LE_PUBLIC    =  0x00,
    DEV_ADDR_LE_RANDOM    =   0x01,
    DEV_ADDR_LE_PUBLIC_ID  =  0x02,
    DEV_ADDR_LE_RANDOM_ID  =  0x03,
} adv_addr_type_en;

typedef enum {
    ADV_IND =                 0x00,
    ADV_DIRECT_IND =          0x01,
    ADV_SCAN_IND =            0x02,
    ADV_NONCONN_IND =         0x03,
    ADV_DIRECT_IND_LOW_DUTY = 0x04,
} adv_type_en;

#ifndef EVENT_BLE
#define EVENT_BLE 0x05000000
#endif

typedef enum {
    EVENT_STACK_INIT = EVENT_BLE,

    EVENT_GAP_CONN_CHANGE,
    EVENT_GAP_DEV_FIND,
    EVENT_GAP_CONN_PARAM_REQ,
    EVENT_GAP_CONN_PARAM_UPDATE,
    EVENT_GAP_CONN_SECURITY_CHANGE,
    EVENT_GAP_ADV_TIMEOUT,

    EVENT_GATT_NOTIFY,
    EVENT_GATT_INDICATE = EVENT_GATT_NOTIFY,
    EVENT_GATT_CHAR_READ,
    EVENT_GATT_CHAR_WRITE,
    EVENT_GATT_INDICATE_CB,
    EVENT_GATT_CHAR_READ_CB,
    EVENT_GATT_CHAR_WRITE_CB,
    EVENT_GATT_CHAR_CCC_CHANGE,
    EVENT_GATT_CHAR_CCC_WRITE,
    EVENT_GATT_CHAR_CCC_MATCH,
    EVENT_GATT_MTU_EXCHANGE,

    EVENT_GATT_DISCOVERY_SVC,
    EVENT_GATT_DISCOVERY_INC_SVC,
    EVENT_GATT_DISCOVERY_CHAR,
    EVENT_GATT_DISCOVERY_CHAR_DES,
    EVENT_GATT_DISCOVERY_COMPLETE,

    EVENT_SMP_PASSKEY_DISPLAY,
    EVENT_SMP_PASSKEY_CONFIRM,
    EVENT_SMP_PASSKEY_ENTER,
    EVENT_SMP_PAIRING_CONFIRM,
    EVENT_SMP_PAIRING_COMPLETE,
    EVENT_SMP_CANCEL,

    EVENT_STACK_UNKNOWN,
} ble_event_en;

typedef enum {
    DISCONNECTED,
    CONNECTED,
} conn_state_en;

#pragma pack(1)

typedef struct _dev_addr_t {
    uint8_t type;
    uint8_t val[6];
} dev_addr_t;

typedef struct _evt_data_gap_conn_change_t {
    int16_t conn_handle;
    int16_t err;
    conn_state_en connected;
} evt_data_gap_conn_change_t;

typedef struct _evt_data_gap_conn_param_req_t {
    int16_t conn_handle;
    int8_t accept;
    conn_param_t param;
} evt_data_gap_conn_param_req_t;

typedef struct _evt_data_gap_conn_param_update_t {
    int16_t conn_handle;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
} evt_data_gap_conn_param_update_t;

typedef struct _evt_data_gap_security_change_t {
    int16_t conn_handle;
    uint16_t level;
    uint8_t err;
} evt_data_gap_security_change_t;

typedef struct _evt_data_gap_dev_find_t {
    dev_addr_t dev_addr;
    adv_type_en adv_type;
    uint8_t adv_len;
    int8_t rssi;
    uint8_t adv_data[31];
} evt_data_gap_dev_find_t;

typedef struct _evt_data_gatt_notify_t {
    int16_t conn_handle;
    int16_t char_handle;
    uint8_t len;
    const uint8_t *data;
} evt_data_gatt_notify_t;

typedef evt_data_gatt_notify_t evt_data_gatt_indicate_t;

typedef struct _evt_data_gatt_indicate_cb_t {
    int16_t conn_handle;
    int16_t char_handle;
    int err;
} evt_data_gatt_indicate_cb_t;

typedef struct _evt_data_gatt_write_cb_t {
    int16_t conn_handle;
    int16_t char_handle;
    int err;
} evt_data_gatt_write_cb_t;

typedef struct _evt_data_gatt_read_cb_t {
    int16_t conn_handle;
    int err;
    uint16_t len;
    const uint8_t *data;
} evt_data_gatt_read_cb_t;


typedef struct _evt_data_gatt_char_read_t {
    int16_t conn_handle;
    int16_t char_handle;
    int32_t len;
    uint8_t *data;
    uint16_t offset;
} evt_data_gatt_char_read_t;

typedef struct _evt_data_gatt_char_write_t {
    int16_t conn_handle;
    int16_t char_handle;
    int32_t len;
    const uint8_t *data;
    uint16_t offset;
    uint8_t flag;
} evt_data_gatt_char_write_t;

typedef struct _evt_data_gatt_char_ccc_change_t {
    int16_t char_handle;
    ccc_value_en ccc_value;
} evt_data_gatt_char_ccc_change_t;

typedef struct _evt_data_gatt_char_ccc_write_t {
    int16_t conn_handle;
    int16_t char_handle;
    ccc_value_en ccc_value;
    int8_t allow_write;
} evt_data_gatt_char_ccc_write_t;

typedef struct _evt_data_gatt_char_ccc_match_t {
    int16_t conn_handle;
    int16_t char_handle;
    int8_t is_matched;
} evt_data_gatt_char_ccc_match_t;

typedef struct _evt_data_gatt_mtu_exchange_t {
    int16_t conn_handle;
    int err;
} evt_data_gatt_mtu_exchange_t;

typedef struct _evt_data_smp_passkey_display_t {
    int16_t conn_handle;
    dev_addr_t peer_addr;
    char *passkey;
} evt_data_smp_passkey_display_t;

typedef struct _evt_data_smp_passkey_confirm_t {
    int16_t conn_handle;
    dev_addr_t peer_addr;
    char *passkey;
} evt_data_smp_passkey_confirm_t;

typedef struct _evt_data_smp_passkey_enter_t {
    int16_t conn_handle;
    dev_addr_t peer_addr;
} evt_data_smp_passkey_enter_t;

typedef struct _evt_data_smp_pairing_confirm_t {
    int16_t conn_handle;
    dev_addr_t peer_addr;
} evt_data_smp_pairing_confirm_t;

typedef struct _evt_data_smp_pairing_complete_t {
    int16_t conn_handle;
    dev_addr_t peer_addr;
    int8_t bonded;
    int16_t err;
} evt_data_smp_pairing_complete_t;

typedef struct _evt_data_smp_cancel_t {
    int16_t conn_handle;
    dev_addr_t peer_addr;
} evt_data_smp_cancel_t;

typedef struct _evt_data_gatt_discovery_svc_t {
    int16_t conn_handle;
    uint16_t start_handle;
    uint16_t end_handle;
    union {
        uuid_t uuid;
        struct ut_uuid_16 uuid16;
        struct ut_uuid_32 uuid32;
        struct ut_uuid_128 uuid128;
    };
} evt_data_gatt_discovery_svc_t;

typedef struct _evt_data_gatt_discovery_inc_svc_t {
    int16_t conn_handle;
    uint16_t attr_handle;
    uint16_t start_handle;
    uint16_t end_handle;
    union {
        uuid_t uuid;
        struct ut_uuid_16 uuid16;
        struct ut_uuid_32 uuid32;
        struct ut_uuid_128 uuid128;
    };
} evt_data_gatt_discovery_inc_svc_t;

typedef struct _evt_data_gatt_discovery_char_t {
    int16_t conn_handle;
    uint16_t attr_handle;
    uint16_t val_handle;
    uint16_t props;
    union {
        uuid_t uuid;
        struct ut_uuid_16 uuid16;
        struct ut_uuid_32 uuid32;
        struct ut_uuid_128 uuid128;
    };
} evt_data_gatt_discovery_char_t;

typedef struct _evt_data_gatt_discovery_char_des_t {
    int16_t conn_handle;
    uint16_t attr_handle;
    union {
        uuid_t uuid;
        struct ut_uuid_16 uuid16;
        struct ut_uuid_32 uuid32;
        struct ut_uuid_128 uuid128;
    };
} evt_data_gatt_discovery_char_des_t;

typedef struct _evt_data_gatt_discovery_complete_t {
    int16_t conn_handle;
    int err;
} evt_data_gatt_discovery_complete_t;

#pragma pack()

typedef int (*event_callback_func_t)(ble_event_en event, void *event_data);

typedef struct _ble_event_cb_t {
    event_callback_func_t callback;
    slist_t next;
} ble_event_cb_t;

typedef struct _init_param_t {
    char *dev_name;
    dev_addr_t *dev_addr;
    uint16_t conn_num_max;
} init_param_t;

typedef enum {
    AD_DATA_TYPE_FLAGS =          0x01,  /* AD flags */
    AD_DATA_TYPE_UUID16_SOME =    0x02,  /* 16-bit UUID, more available */
    AD_DATA_TYPE_UUID16_ALL =     0x03,  /* 16-bit UUID, all listed */
    AD_DATA_TYPE_UUID32_SOME =    0x04,  /* 32-bit UUID, more available */
    AD_DATA_TYPE_UUID32_ALL =     0x05,  /* 32-bit UUID, all listed */
    AD_DATA_TYPE_UUID128_SOME =   0x06,  /* 128-bit UUID, more available */
    AD_DATA_TYPE_UUID128_ALL =    0x07,  /* 128-bit UUID, all listed */
    AD_DATA_TYPE_NAME_SHORTENED = 0x08,  /* Shortened name */
    AD_DATA_TYPE_NAME_COMPLETE =  0x09,  /* Complete name */
    AD_DATA_TYPE_TX_POWER =       0x0a,  /* Tx Power */
    AD_DATA_TYPE_SOLICIT16 =      0x14,  /* Solicit UUIDs, 16-bit */
    AD_DATA_TYPE_SOLICIT128 =     0x15,  /* Solicit UUIDs, 128-bit */
    AD_DATA_TYPE_SVC_DATA16 =     0x16,  /* Service data, 16-bit UUID */
    AD_DATA_TYPE_GAP_APPEARANCE = 0x19,  /* GAP appearance */
    AD_DATA_TYPE_SOLICIT32 =      0x1f,  /* Solicit UUIDs, 32-bit */
    AD_DATA_TYPE_SVC_DATA32 =     0x20,  /* Service data, 32-bit UUID */
    AD_DATA_TYPE_SVC_DATA128 =    0x21,  /* Service data, 128-bit UUID */
    AD_DATA_TYPE_URI =            0x24,  /* URI */
    AD_DATA_TYPE_MESH_PROV =      0x29,  /* Mesh Provisioning PDU */
    AD_DATA_TYPE_MESH_MESSAGE =   0x2a,  /* Mesh Networking PDU */
    AD_DATA_TYPE_MESH_BEACON =    0x2b,  /* Mesh Beacon */

    AD_DATA_TYPE_MANUFACTURER_DATA = 0xff,  /* Manufacturer Specific Data */
} ad_date_type_en;

typedef enum {
    AD_FLAG_LIMITED = 0x01, /* Limited Discoverable */
    AD_FLAG_GENERAL = 0x02, /* General Discoverable */
    AD_FLAG_NO_BREDR = 0x04, /* BR/EDR not supported */
} ad_flag_en;

typedef enum {
    ADV_FAST_INT_MIN_1   = 0x0030,  /* 30 ms    */
    ADV_FAST_INT_MAX_1   = 0x0060,  /* 60 ms    */
    ADV_FAST_INT_MIN_2   = 0x00a0,  /* 100 ms   */
    ADV_FAST_INT_MAX_2   = 0x00f0,  /* 150 ms   */
    ADV_SLOW_INT_MIN     = 0x0640,  /* 1 s      */
    ADV_SLOW_INT_MAX     = 0x0780,  /* 1.2 s    */
    ADV_SCAN_INT_MIN_1   = 0x0120,  /* 112.5 ms   */
    ADV_SCAN_INT_MAX_1   = 0x0140,  /* 200 ms   */
} adv_interval_en;

typedef struct _ad_data_t {
    uint8_t type;
    uint8_t len;
    uint8_t *data;
} ad_data_t;

typedef enum {
    ADV_FILTER_POLICY_ANY_REQ   = 0,  /* any scan request or connect request */
    ADV_FILTER_POLICY_SCAN_REQ  = 1,  /* all connect request, white list scan request */
    ADV_FILTER_POLICY_CONN_REQ  = 2,  /* all scan request, white list connect request */
    ADV_FILTER_POLICY_ALL_REQ   = 3,  /* white list scan request and connect request */
} adv_filter_policy_en;
typedef enum {
    ADV_CHAN_37 = 0x01,
    ADV_CHAN_38 = 0x02,
    ADV_CHAN_39 = 0x04,
} adv_chan_en;
#define ADV_DEFAULT_CHAN_MAP (ADV_CHAN_37 | ADV_CHAN_38 | ADV_CHAN_39)
typedef struct _adv_param_t {
    adv_type_en type;
    ad_data_t *ad;
    ad_data_t *sd;
    uint8_t ad_num;
    uint8_t sd_num;
    uint16_t interval_min;
    uint16_t interval_max;
    adv_filter_policy_en filter_policy;
    adv_chan_en channel_map;
    dev_addr_t direct_peer_addr;
} adv_param_t;

typedef enum {
    SCAN_PASSIVE = 0x00,
    SCAN_ACTIVE = 0x01,
} scan_type_en;

typedef enum {
    SCAN_FILTER_DUP_DISABLE = 0x00,
    SCAN_FILTER_DUP_ENABLE = 0x01,
} scan_filter_en;

typedef enum {
    SCAN_FAST_INTERVAL   = 0x0060,  /* 60 ms    */
    SCAN_SLOW_INTERVAL_1 = 0x0800,  /* 1.28 s   */
    SCAN_SLOW_INTERVAL_2 = 0x1000,  /* 2.56 s   */
} scan_interval_en;

typedef enum {
    SCAN_FAST_WINDOW     = 0x0030,  /* 30 ms    */
    SCAN_SLOW_WINDOW     = 0x0012,  /* 11.25 ms */
} scan_window_en;

typedef enum {
    SCAN_FILTER_POLICY_ANY_ADV        = 0,  /* any adv packages  */
    SCAN_FILTER_POLICY_WHITE_LIST     = 1,  /* white list adv packages */
} scan_filter_policy_en;
typedef struct _scan_param_t {
    scan_type_en  type;

    scan_filter_en  filter_dup;

    uint16_t interval;

    uint16_t window;
    scan_filter_policy_en scan_filter;
} scan_param_t;

typedef enum {
    CONN_INT_MIN_INTERVAL = 0x0018, /* 30 ms    */
    CONN_INT_MAX_INTERVAL = 0x0028, /* 50 ms    */
} conn_interval_en;

typedef enum {
    IO_CAP_IN_NONE = 0x01,
    IO_CAP_IN_YESNO = 0x02,
    IO_CAP_IN_KEYBOARD = 0x04,

    IO_CAP_OUT_NONE = 0x08,
    IO_CAP_OUT_DISPLAY = 0x10,
} io_cap_en;

typedef struct _connect_info_t {
    int16_t conn_handle;
    uint8_t role;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    dev_addr_t local_addr;
    dev_addr_t peer_addr;
} connect_info_t;

typedef enum {
    /** Only for BR/EDR special cases, like SDP */
    SECURITY_NONE,
    /** No encryption and no authentication. */
    SECURITY_LOW,
    /** Encryption and no authentication (no MITM). */
    SECURITY_MEDIUM,
    /** Encryption and authentication (MITM). */
    SECURITY_HIGH,
    /** Authenticated Secure Connections */
    SECURITY_FIPS,
} security_en;

int ble_stack_init(init_param_t *param);
int ble_stack_set_name(const char *name);
int ble_stack_iocapability_set(uint8_t io_cap);

int ble_stack_event_register(ble_event_cb_t *callback);

int ble_stack_adv_start(adv_param_t *param);
int ble_stack_adv_stop();

int ble_stack_scan_start(const scan_param_t *param);
int ble_stack_scan_stop();

int ble_stack_get_local_addr(dev_addr_t *addr);

int ble_stack_connect(dev_addr_t *peer_addr, conn_param_t *param, uint8_t auto_connect);
int ble_stack_disconnect(int16_t conn_handle);
int ble_stack_connect_info_get(int16_t conn_handle, connect_info_t *info);
int ble_stack_security(int16_t conn_handle, security_en level);
int ble_stack_connect_param_update(int16_t conn_handle, conn_param_t *param);

int ble_stack_smp_passkey_entry(int16_t conn_handle, uint32_t passkey);

int ble_stack_smp_cancel(int16_t conn_handle);

int ble_stack_smp_passkey_confirm(int16_t conn_handle);

int ble_stack_smp_pairing_confirm(int16_t conn_handle);

int ble_stack_setting_load();

int ble_stack_dev_unpair(dev_addr_t *peer_addr);

int ble_stack_enc_key_size_get(int16_t conn_handle);

int ble_stack_white_list_clear();
int ble_stack_white_list_add(dev_addr_t *peer_addr);
int ble_stack_white_list_remove(dev_addr_t *peer_addr);
int ble_stack_white_list_size();
int ble_stack_check_conn_params(const conn_param_t *param);


#ifdef __cplusplus
}
#endif

#endif   /* __BLE_H__ */

