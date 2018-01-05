/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "internal/ble_helper_internal.h"

ble_gatt_attr_t *ble_attr_add(uint16_t hdl, uint16_t val_len, const uint8_t *val)
{
    return mico_bt_peripheral_ext_attribute_add(hdl, val_len, val, NULL);
}

void ble_attr_indicate(ble_gatt_attr_t *attr, peripheral_hdl_t hdl,
                       uint16_t len, const uint8_t *data)
{
    peripheral_socket_t *p = g_skt_head;

    for (; p; p = p->next)
        if (p->hdl == hdl) {
            break;
        }

    if (!p) {
        return;
    }

    mico_bt_peripheral_ext_attribute_value_write(attr, len, 0, data);
    mico_bt_peripheral_gatt_indicate_attribute_value(&p->skt, attr);
}

void ble_attr_notify(ble_gatt_attr_t *attr, peripheral_hdl_t hdl,
                     uint16_t len, const uint8_t *data)
{
    peripheral_socket_t *p = g_skt_head;

    for (; p; p = p->next)
        if (p->hdl == hdl) {
            break;
        }

    if (!p) {
        return;
    }

    mico_bt_peripheral_ext_attribute_value_write(attr, len, 0, data);
    mico_bt_peripheral_gatt_notify_attribute_value(&p->skt, attr);
}
