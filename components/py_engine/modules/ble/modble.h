#ifndef _PY_MODBLE_H_
#define _PY_MODBLE_H_

#include <aos/ble.h>
#include <atomic.h>
#include <ble_netconfig.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <netmgr.h>
#include <netmgr_wifi.h>

#include "ulog/ulog.h"

// #define WIFI_DEV_PATH "/dev/wifi0"

// typedef struct amp_wifi_info {
//     char ssid[128];
//     char mac[6];
//     char ip[16];
//     int rssi;
// } amp_wifi_info_t;

// enum ble_err {
//     BLE_ERR_NONE = 0,
//     BLE_ERR_NET_DEV = -1,
//     BLE_ERR_NET_IFCONFIG,
//     BLE_ERR_NET_AP_CNT_INVALID,
//     BLE_ERR_NET_CONFIG,
//     BLE_ERROR_NET_AP_CNT_INVALID,
// };

enum ble_event {
    BLE_EVENT_NET_STATUS = 0,
    BLE_EVENT_CHARA_WRITTEN,
    BLE_EVENT_MAX
};

typedef struct {
    char *dev_name;
    uint16_t conn_num_max;
} amp_bt_host_adapter_init_t;

typedef struct {
    int32_t type;
    char *adv_data;
    char *scan_rsp_data;
    int32_t interval_min;
    int32_t interval_max;
    int32_t channel_map;
} amp_bt_host_adapter_adv_start_t;

typedef struct {
    int32_t type;
    uint16_t interval;
    uint16_t window;
} amp_bt_host_adapter_scan_start_t;

typedef struct {
    const char *char_uuid;
    const char *permission;
    const char *descr_uuid;
    const char *descr_type;
} amp_bt_host_adapter_gatt_chars_t;

typedef struct {
    char *s_uuid;
    uint32_t attr_cnt;
    amp_bt_host_adapter_gatt_chars_t *chars;
} amp_bt_host_adapter_gatts_srvc_t;

typedef struct {
    size_t len;
    uint8_t *data;
} amp_bt_gatts_adapter_usr_data_t;

// GAP
int py_bt_host_adapter_init(amp_bt_host_adapter_init_t *init);
int bt_host_adapter_start_adv(amp_bt_host_adapter_adv_start_t *adv_param);
int bt_host_adapter_stop_adv(void);
// GATTS
void native_bt_host_gatts_handle_write(uint8_t data[], size_t len);
int bt_gatts_adapter_add_service(amp_bt_host_adapter_gatts_srvc_t *srvc);
int bt_gatts_adapter_update_chars(char *uuid, uint8_t *data, size_t len);

#endif
