/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "bt_mesh_gatt.h"

void bt_mesh_conn_cb_register(struct bt_conn_cb *cb)
{
    bt_conn_cb_register(cb);
}

struct bt_conn *bt_mesh_conn_ref(struct bt_conn *conn)
{
    return bt_conn_ref(conn);
}

void bt_mesh_conn_unref(struct bt_conn *conn)
{
    bt_conn_unref(conn);
}

int bt_mesh_conn_disconnect(struct bt_conn *conn, uint8_t reason)
{
    return bt_conn_disconnect(conn, reason);
}

int bt_mesh_gatt_service_register(struct bt_gatt_service *svc)
{
    return bt_gatt_service_register(svc);
}

int bt_mesh_gatt_service_unregister(struct bt_gatt_service *svc)
{
    return bt_gatt_service_unregister(svc);
}

int bt_mesh_gatt_notify(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *data, uint16_t len)
{
    return bt_gatt_notify(conn, attr, data, len);
}

ssize_t bt_mesh_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t buf_len, uint16_t offset,
                               const void *value, uint16_t value_len)
{
    return bt_gatt_attr_read(conn, attr, buf, buf_len, offset, value, value_len);
}

uint16_t bt_mesh_gatt_get_mtu(struct bt_conn *conn)
{
    return bt_gatt_get_mtu(conn);
}
