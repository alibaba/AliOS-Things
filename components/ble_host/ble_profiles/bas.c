#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <aos/osal_debug.h>
#include <aos/ble.h>
#include <yoc/bas.h>

#define TAG "BAS"

enum {
    BAS_IDX_SVC,
    BAS_IDX_LEVEL_CHAR,
    BAS_IDX_LEVEL_VAL,
    BAS_IDX_LEVEL_CCC,

    BAS_IDX_MAX,
};

static slist_t bas_list = {NULL};

gatt_service bas_service;

static gatt_attr_t bas_attrs[BAS_IDX_MAX] = {
    [BAS_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(UUID_BAS),
    [BAS_IDX_LEVEL_CHAR] = GATT_CHAR_DEFINE(UUID_BAS_BATTERY_LEVEL,  GATT_CHRC_PROP_READ | GATT_CHRC_PROP_NOTIFY),
    [BAS_IDX_LEVEL_VAL] = GATT_CHAR_VAL_DEFINE(UUID_BAS_BATTERY_LEVEL, GATT_PERM_READ),
    [BAS_IDX_LEVEL_CCC] = GATT_CHAR_CCC_DEFINE(),
};

static inline bas_t *get_bas(uint16_t bas_svc_handle)
{
    slist_t *tmp;
    bas_t *node;

    slist_for_each_entry_safe(&bas_list, tmp, node, bas_t, next) {
        if (node->bas_svc_handle == bas_svc_handle) {
            return node;
        }
    }

    return NULL;
}

static void event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;

    bas_t *bas = get_bas(e->char_handle - BAS_IDX_LEVEL_VAL);

    if (bas == NULL) {
        return;
    }

    e->data = &bas->battery_level;
    e->len = 1;
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    slist_t *tmp;
    bas_t *node;
    slist_for_each_entry_safe(&bas_list, tmp, node, bas_t, next) {
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
    bas_t *bas = get_bas(e->char_handle - BAS_IDX_LEVEL_CCC);

    if (bas == NULL) {
        return;
    }

    bas->ccc = e->ccc_value;
}

static int bas_event_callback(ble_event_en event, void *event_data)
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
    .callback = bas_event_callback,
};


bas_handle_t bas_init(bas_t *bas)
{
    int ret = 0;

    if (bas == NULL) {
        return NULL;
    }

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        goto err;
    }

    ret = ble_stack_gatt_registe_service(&bas_service, bas_attrs, BLE_ARRAY_NUM(bas_attrs));

    if (ret < 0) {
        goto err;
    }

    bas->conn_handle = 0xFFFF;
    bas->bas_svc_handle = ret;
    bas->battery_level = 0xFF;
    bas->ccc = 0;

    slist_add(&bas->next, &bas_list);
    return bas;

err:
    ///free(bas);
    return NULL;
}

int bas_level_update(bas_handle_t handle, uint8_t level)
{
    if (handle == NULL) {
        return -BLE_STACK_ERR_NULL;
    }

    bas_t *bas = handle;

    if (level != bas->battery_level) {
        bas->battery_level = level;

        if (bas->conn_handle != 0xFFFF && bas->ccc == CCC_VALUE_NOTIFY) {
            return ble_stack_gatt_notificate(bas->conn_handle,
                                             bas->bas_svc_handle + BAS_IDX_LEVEL_VAL,
                                             &level, 1);
        }
    }

    return 0;
}

