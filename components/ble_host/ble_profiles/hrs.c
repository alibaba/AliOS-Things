/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <aos/osal_debug.h>
#include <aos/ble.h>
#include <yoc/hrs.h>

#define TAG "HRS"

enum {
    HRS_IDX_SVC,
    HRS_IDX_MEA_CHAR,
    HRS_IDX_MEA_VAL,
    HRS_IDX_MEA_CCC,
    HRS_IDX_BODY_CHAR,
    HRS_IDX_BODY_VAL,
    HRS_IDX_CONTROL_CHAR,
    HRS_IDX_CONTROL_VAL,

    HRS_IDX_MAX,
};

static slist_t hrs_list = {NULL};
gatt_service hrs_service;

//static struct bt_gatt_ccc_cfg_t ccc_data[2] = {};

static gatt_attr_t hrs_attrs[HRS_IDX_MAX] = {
    [HRS_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(UUID_HRS),
    [HRS_IDX_MEA_CHAR] = GATT_CHAR_DEFINE(UUID_HRS_MEASUREMENT, GATT_CHRC_PROP_NOTIFY),
    [HRS_IDX_MEA_VAL] = GATT_CHAR_VAL_DEFINE(UUID_HRS_MEASUREMENT, GATT_PERM_NONE),
    [HRS_IDX_MEA_CCC] = GATT_CHAR_CCC_DEFINE(),
    [HRS_IDX_BODY_CHAR] = GATT_CHAR_DEFINE(UUID_HRS_BODY_SENSOR, GATT_CHRC_PROP_READ),
    [HRS_IDX_BODY_VAL]  = GATT_CHAR_VAL_DEFINE(UUID_HRS_BODY_SENSOR, GATT_PERM_NONE),
    [HRS_IDX_CONTROL_CHAR] = GATT_CHAR_DEFINE(UUID_HRS_CONTROL_POINT, GATT_CHRC_PROP_WRITE),
    [HRS_IDX_CONTROL_VAL] = GATT_CHAR_VAL_DEFINE(UUID_HRS_CONTROL_POINT, GATT_PERM_NONE),
};

static inline hrs_t *get_hrs(uint16_t hrs_svc_handle)
{
    slist_t *tmp;
    hrs_t *node;

    slist_for_each_entry_safe(&hrs_list, tmp, node, hrs_t, next) {
        if (node->hrs_svc_handle == hrs_svc_handle) {
            return node;
        }
    }

    return NULL;
}



static void event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;

    hrs_t *hrs = get_hrs(e->char_handle - HRS_IDX_MEA_VAL);

    if (hrs == NULL) {
        return;
    }

    LOGD(TAG, "event_char_read conn handle %d char handle 0x%04x, len %d, offset %d",
         e->conn_handle, e->char_handle, e->len, e->offset);

    e->data = &hrs->hrs_mea_level;
    e->len = 1;
}


static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    slist_t *tmp;
    hrs_t *node;
    slist_for_each_entry_safe(&hrs_list, tmp, node, hrs_t, next) {
        if (e->connected == CONNECTED) {
            node->conn_handle = e->conn_handle;
        } else {
            node->conn_handle = 0xFFFF;
        }
    }
}



static void event_char_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;
    hrs_t *hrs = get_hrs(e->char_handle - HRS_IDX_MEA_CCC);

    if (hrs == NULL) {
        return;
    }

    hrs->mea_ccc = e->ccc_value;
}



static int hrs_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ:
            event_char_read(event, event_data);
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
    .callback = hrs_event_callback,
};

hrs_handle_t hrs_init(hrs_t *hrs)
{
    int ret = 0;

    if (hrs == NULL) {
        return NULL;
    }

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        goto err;
    }

    ret = ble_stack_gatt_registe_service(&hrs_service,hrs_attrs, BLE_ARRAY_NUM(hrs_attrs));

    if (ret < 0) {
        goto err;
    }

    hrs->conn_handle = 0xFFFF;
    hrs->hrs_svc_handle = ret;
    hrs->hrs_mea_level = 0xFF;
    hrs->mea_ccc = 0;

    slist_add(&hrs->next, &hrs_list);
    return hrs;

err:
    return NULL;
}


int hrs_measure_level_update(hrs_handle_t handle, uint8_t *data, uint8_t length)
{
    if (handle == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    hrs_t *hrs = handle;

    if (data[1] != hrs->hrs_mea_level) {
        hrs->hrs_mea_level = data[1];

        if (hrs->conn_handle != 0xFFFF && hrs->mea_ccc == CCC_VALUE_NOTIFY) {
            LOGD(TAG, "data:%x", data[1]);
            return ble_stack_gatt_notificate(hrs->conn_handle,
                                             hrs->hrs_svc_handle + HRS_IDX_MEA_VAL,
                                             data, length);
        }
    }

    return 0;
}




