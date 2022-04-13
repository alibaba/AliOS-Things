/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <aos/osal_debug.h>
#include <aos/ble.h>
#include <yoc/hids.h>

#define TAG "HIDS"

typedef struct hids_info_t {
    uint16_t bcdHID;
    uint8_t countryCode;
    uint8_t flags;
} hids_info_t;

typedef struct hids_report_ref_t {
    uint8_t id;
    uint8_t type;
} hids_report_ref_t;

enum {
    HIDS_INPUT = 0x01,
    HIDS_OUTPUT = 0x02,
    HIDS_FEATURE = 0x03,
};

enum {
    HIDS_REMOTE_WAKE = 0x01,
    HIDS_NORMALLY_CONNECTABLE = 0x02,
};

static hids_info_t g_hids_info = {
    0x0101,
    0x00,
    HIDS_NORMALLY_CONNECTABLE | HIDS_REMOTE_WAKE,
};

static hids_report_ref_t report_input_ref = {
    0x00,
    HIDS_INPUT,
};

static hids_report_ref_t report_output_ref = {
    0x00,
    HIDS_OUTPUT,
};

//static hids_report_ref_t report_feature_ref = {
//    0x00,
//    HIDS_FEATURE,
//};

typedef struct reportdata_array {
    uint8_t *map_array;
    uint16_t map_len;
    int8_t s_flag;
} data_report_array;

static data_report_array  data_in_out[] = {
    {NULL, 0, -1}, //REPORT_MAP
    {NULL, 0, -1}, //REPORT_INPUT
    {NULL, 0, -1}, //REPORT_OUTPUT
};

gatt_service hids_service;


static uint8_t *get_data_map_data(uint8_t u_type);
static uint16_t  get_data_map_len(uint8_t u_type);

typedef struct _hids_t {
    uint16_t conn_handle;
    uint16_t svc_handle;
    uint16_t input_ccc;
    uint16_t boot_input_ccc;
    uint8_t protocol_mode;
} hids_t;

hids_t g_hids = {0};


typedef struct _register_hids_event_ {
    int16_t  idx;
    hids_event_cb cb;
} register_hids_event ;
register_hids_event  hids_event_arr[HIDS_IDX_MAX] = {0};

//static struct bt_gatt_ccc_cfg_t ccc_data1[2] = {};
//static struct bt_gatt_ccc_cfg_t ccc_data2[2] = {};

gatt_attr_t  hids_attrs[] = {
    GATT_PRIMARY_SERVICE_DEFINE(UUID_HIDS),
    /* REPORT MAP */
    GATT_CHAR_DEFINE(UUID_HIDS_REPORT_MAP, GATT_CHRC_PROP_READ),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_REPORT_MAP, GATT_PERM_READ),

    /* REPORT INPUT  */
    GATT_CHAR_DEFINE(UUID_HIDS_REPORT, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_NOTIFY),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_REPORT, GATT_PERM_READ | GATT_PERM_READ_AUTHEN),
    GATT_CHAR_DESCRIPTOR_DEFINE(UUID_HIDS_REPORT_REF, GATT_PERM_READ),
    GATT_CHAR_CCC_DEFINE(),

    /* REPORT OUTPUT */
    GATT_CHAR_DEFINE(UUID_HIDS_REPORT, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_REPORT, GATT_PERM_READ | GATT_PERM_READ_AUTHEN | GATT_PERM_WRITE),
    GATT_CHAR_DESCRIPTOR_DEFINE(UUID_HIDS_REPORT_REF, GATT_PERM_READ),
#if 0
    /* REPORT FEATURE */
    GATT_CHAR_DEFINE(UUID_HIDS_REPORT, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_REPORT, GATT_PERM_READ | GATT_PERM_READ_ENCRYPT | GATT_PERM_WRITE | GATT_PERM_WRITE_ENCRYPT),
    GATT_CHAR_DESCRIPTOR_DEFINE(UUID_HIDS_REPORT_REF, GATT_PERM_READ),
#endif
    /* Boot Keyboard Input Report */
    GATT_CHAR_DEFINE(UUID_HIDS_BOOT_KB_IN_REPORT, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_NOTIFY),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_BOOT_KB_IN_REPORT, GATT_PERM_READ | GATT_PERM_READ_AUTHEN | GATT_PERM_WRITE),
    GATT_CHAR_CCC_DEFINE(),

    /* Boot Keyboard Output Report */
    GATT_CHAR_DEFINE(UUID_HIDS_BOOT_KB_OUT_REPORT, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_BOOT_KB_OUT_REPORT, GATT_PERM_READ | GATT_PERM_READ_AUTHEN | GATT_PERM_WRITE), //

    /* HID Information */
    GATT_CHAR_DEFINE(UUID_HIDS_INFO, GATT_CHRC_PROP_READ),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_INFO, GATT_PERM_READ),

    /* HID Control Point */
    GATT_CHAR_DEFINE(UUID_HIDS_CTRL_POINT, GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_CTRL_POINT, GATT_PERM_WRITE),

    /* Protocol Mode *///low power Suspend mode ,0x00 Boot Protocol Mode 0x01 report protocol mode
    GATT_CHAR_DEFINE(UUID_HIDS_PROTOCOL_MODE, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    GATT_CHAR_VAL_DEFINE(UUID_HIDS_PROTOCOL_MODE, GATT_PERM_READ | GATT_PERM_WRITE),
};

static void read_report(evt_data_gatt_char_read_t *e, void *data, uint16_t len)
{
    e->data = data;
    e->len = len;
}

static void event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;

    if (g_hids.conn_handle == 0xFFFF || e->char_handle < g_hids.svc_handle || e->char_handle >= g_hids.svc_handle + HIDS_IDX_MAX) {
        return;
    }

    uint16_t hids_char_idx = e->char_handle - g_hids.svc_handle;

    switch (hids_char_idx) {
        case HIDS_IDX_REPORT_MAP_VAL:
            read_report(e, get_data_map_data(REPORT_MAP), get_data_map_len(REPORT_MAP));
            break;

        case HIDS_IDX_REPORT_INPUT_VAL:
            read_report(e, get_data_map_data(REPORT_INPUT), get_data_map_len(REPORT_INPUT));
            break;

        case HIDS_IDX_REPORT_INPUT_REF:
            read_report(e, &report_input_ref, sizeof(report_input_ref));
            break;

        case HIDS_IDX_REPORT_OUTPUT_VAL:
            read_report(e, get_data_map_data(REPORT_OUTPUT), get_data_map_len(REPORT_OUTPUT));
            break;

        case HIDS_IDX_REPORT_OUTPUT_REF:
            read_report(e, &report_output_ref, sizeof(report_output_ref));
            break;

        case HIDS_IDX_INFO_VAL:
            read_report(e, &g_hids_info, sizeof(g_hids_info));
            break;

        case HIDS_IDX_PROTOCOL_MODE_VAL:
            read_report(e, &g_hids.protocol_mode, 1);
            break;

        default:
            //LOGI(TAG, "unhandle event:%x\r\n\r\n", hids_char_idx);
            break;
    }
}

int  init_hids_call_func(int32_t idx, hids_event_cb cb_event)
{
    if (idx < 0 || idx >= HIDS_IDX_MAX) {
        return -1;
    }

    if (hids_event_arr[idx].cb != NULL) {
        hids_event_arr[idx].cb = cb_event;
    }

    return  0;
}

int execute_hids_call_func(int32_t idx, void *event_data)
{
    if (idx < 0 || idx >= HIDS_IDX_MAX) {
        return -1;
    }

    if (hids_event_arr[idx].cb != NULL) {
        hids_event_arr[idx].cb(idx, event_data);
    }

    return 0;
}

static void event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    int ires = 0;

    if (g_hids.conn_handle == 0xFFFF || e->char_handle < g_hids.svc_handle || e->char_handle >= g_hids.svc_handle + HIDS_IDX_MAX) {
        return;
    }

    uint16_t hids_char_idx = e->char_handle - g_hids.svc_handle;

    switch (hids_char_idx) {
        case HIDS_IDX_REPORT_OUTPUT_VAL:
            ires = execute_hids_call_func(HIDS_IDX_REPORT_OUTPUT_VAL, e);
            break;

        case HIDS_IDX_BOOT_KB_INPUT_REPORT_VAL:
            ires = execute_hids_call_func(HIDS_IDX_BOOT_KB_INPUT_REPORT_VAL, e);
            break;

        case HIDS_IDX_BOOT_KB_OUTPUT_REPORT_VAL:
            ires = execute_hids_call_func(HIDS_IDX_BOOT_KB_OUTPUT_REPORT_VAL, e);
            break;

        case HIDS_IDX_CTRL_VAL:
            //LOGI(TAG, "control cmd %d, %s\n", e->data[0], e->data[0] == 0x00 ? " Suspend" :
            //     "Exit Suspend");
            ires = execute_hids_call_func(HIDS_IDX_CTRL_VAL, e);
            break;

        case HIDS_IDX_PROTOCOL_MODE_VAL:
            g_hids.protocol_mode = e->data[0];
            break;

        default:
            break;
    }

    if (ires != 0) {
        //LOGI(TAG, "event_char_write execute err\r\n");
    }
}

static void event_char_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;

    if (g_hids.conn_handle == 0xFFFF || e->char_handle < g_hids.svc_handle || e->char_handle >= g_hids.svc_handle + HIDS_IDX_MAX) {
        return;
    }

    uint16_t hids_char_idx = e->char_handle - g_hids.svc_handle;

    switch (hids_char_idx) {
        case HIDS_IDX_REPORT_INPUT_CCC:
            g_hids.input_ccc = e->ccc_value;
            break;

        case HIDS_IDX_BOOT_KB_INPUT_REPORT_CCC:
            g_hids.boot_input_ccc = e->ccc_value;
            break;
    }
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        g_hids.conn_handle = e->conn_handle;
    } else {
        g_hids.conn_handle = 0xFFFF;
    }
}

static int hids_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GATT_CHAR_READ:
            event_char_read(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            event_char_write(event, event_data);
            break;

        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_CHANGE:
            event_char_ccc_change(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = hids_event_callback,
};

hids_handle_t hids_init(uint8_t     mode)
{
    int ret = 0;

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        return NULL;
    }

    ret = ble_stack_gatt_registe_service(&hids_service,hids_attrs, BLE_ARRAY_NUM(hids_attrs));

    if (ret < 0) {
        return NULL;
    }

    g_hids.conn_handle = 0xFFFF;
    g_hids.svc_handle = ret;
    g_hids.protocol_mode = mode;

    memset(hids_event_arr, 0x0, sizeof(hids_event_arr));
    return &g_hids;
}

int hids_notify_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len)
{
    hids_t *hids = handle;

    if (handle == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    if (hids->protocol_mode == HIDS_REPORT_PROTOCOL_MODE && hids->input_ccc == CCC_VALUE_NOTIFY) {
        ble_stack_gatt_notificate(hids->conn_handle, hids->svc_handle + HIDS_IDX_REPORT_INPUT_VAL, key_code, us_len);
    }

    return 0;
}
int hids_key_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len)
{
    hids_t *hids = handle;

    if (handle == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    ble_stack_gatt_notificate(hids->conn_handle, hids->svc_handle + HIDS_IDX_REPORT_INPUT_VAL, key_code, us_len);
    return 0;
}

int set_data_map(uint8_t u_data[], uint16_t len, uint8_t u_type)
{
    if (u_type >= REPORT_MAX) {
        return -1;
    }

    //data_in_out[u_type].map_array = (uint8_t *)malloc(sizeof(uint8_t) * len);

    //if (data_in_out[u_type].map_array == NULL) {
    //    return -1;
    //}

    data_in_out[u_type].map_array = u_data;
    data_in_out[u_type].map_len = len;
    data_in_out[u_type].s_flag = 0;

    return 0;
}
static uint8_t *get_data_map_data(uint8_t u_type)
{
    if (u_type >= REPORT_MAX) {
        return NULL;
    }

    if (data_in_out[u_type].s_flag == -1) {
        return NULL;
    }

    return (uint8_t *)data_in_out[u_type].map_array;
}

static uint16_t  get_data_map_len(uint8_t u_type)
{
    if (u_type >= REPORT_MAX) {
        return -1;
    }

    if ((data_in_out[u_type].s_flag == -1) || (data_in_out[u_type].map_len > 512)) {
        return -1;
    }

    return  data_in_out[u_type].map_len;
}





