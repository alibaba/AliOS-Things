/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BLE_APP_FRAMEWORK_DEF_H_
#define _BLE_APP_FRAMEWORK_DEF_H_

#include <stdint.h>
#include "ble_gatt_defs.h"
#include "ble_adv_defs.h"

#ifndef UUID_SERVCLASS_GATT_SERVER
#define UUID_SERVCLASS_GATT_SERVER 0x1801
#endif
#ifndef UUID_SERVCLASS_GAP_SERVER
#define UUID_SERVCLASS_GAP_SERVER  0x1800
#endif
#ifndef UUID_SERVCLASS_DEVICE_INFO
#define UUID_SERVCLASS_DEVICE_INFO 0x180A
#endif
#ifndef GATT_UUID_MANU_NAME
#define GATT_UUID_MANU_NAME        0x2A29
#endif
#ifndef GATT_UUID_MODEL_NUMBER_STR
#define GATT_UUID_MODEL_NUMBER_STR 0x2A24
#endif
#ifndef GATT_UUID_SYSTEM_ID
#define GATT_UUID_SYSTEM_ID        0x2A23
#endif

typedef struct peripheral_init_s {
    const char * dev_name;
    uint8_t client_links;
    uint8_t server_links;
} peripheral_init_t;

typedef uint8_t ble_gatt_request_type_t;
typedef uint8_t ble_gatt_status_t;

#pragma pack(1)
typedef struct dlist_node {
    void              *data;
    struct dlist_node *next;
    struct dlist_node *prev;
} dlist_node_t;
#pragma pack()

typedef struct ble_gatt_attr_s ble_gatt_attr_t;
typedef ble_gatt_status_t \
        (*ble_peripheral_attr_handler)\
        (ble_gatt_attr_t *attribute,\
        ble_gatt_request_type_t op);

struct ble_gatt_attr_s {
    dlist_node_t                this_node;
    uint16_t                    handle;
    uint16_t                    value_length;
    uint16_t                    value_buffer_length;
    uint8_t                     *p_value;
    ble_peripheral_attr_handler attribute_handler;
};

typedef uint32_t peripheral_hdl_t;

typedef int (*ble_peripheral_conn_cb_t)(void);
typedef int (*ble_peripheral_disconn_cb_t)(void);
typedef int (*ble_adv_complete_cb_t)(void *arg);

#endif
