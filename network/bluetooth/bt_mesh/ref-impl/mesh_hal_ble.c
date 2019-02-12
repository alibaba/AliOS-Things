/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <port/mesh_hal_ble.h>

#ifndef CONFIG_MESH_STACK_ALONE
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/bluetooth.h>

int bt_mesh_adv_start(const struct bt_mesh_le_adv_param *param,
                      const struct bt_mesh_data *ad, size_t ad_len,
                      const struct bt_mesh_data *sd, size_t sd_len)
{
    return bt_le_adv_start((const struct bt_le_adv_param *)param,
                           (const struct bt_data *)ad, ad_len,
                           (const struct bt_data *)sd, sd_len);
}

int bt_mesh_adv_stop(void)
{
    return bt_le_adv_stop();
}

int bt_mesh_scan_start(const struct bt_mesh_le_scan_param *param, bt_mesh_le_scan_cb_t cb)
{
    return bt_le_scan_start((const struct bt_le_scan_param *)param, cb);
}

int bt_mesh_scan_stop(void)
{
    return bt_le_scan_stop();
}

struct bt_conn_cb conn_callbacks;

void bt_mesh_conn_cb_register(struct bt_mesh_conn_cb *cb)
{
    conn_callbacks.connected = cb->connected;
    conn_callbacks.disconnected = cb->disconnected;
    bt_conn_cb_register(&conn_callbacks);
}

bt_mesh_conn_t bt_mesh_conn_ref(bt_mesh_conn_t conn)
{
    return bt_conn_ref((struct bt_conn *)conn);
}

void bt_mesh_conn_unref(bt_mesh_conn_t conn)
{
    bt_conn_unref((struct bt_conn *)conn);
}

int bt_mesh_conn_disconnect(bt_mesh_conn_t conn, uint8_t reason)
{
    return bt_conn_disconnect((struct bt_conn *)conn, reason);
}

#define SVC_ENTRY_MAX 16

struct svc_paire_node {
    struct bt_mesh_gatt_service *msvc;
    struct bt_gatt_service *svc;
} _svc_paire[SVC_ENTRY_MAX] = {{0}};

/* TODO: manage the services in linked list. */
int bt_mesh_gatt_service_register(struct bt_mesh_gatt_service *svc)
{
    struct svc_paire_node *node = &_svc_paire[0];
    int i = 0;

    while (i < SVC_ENTRY_MAX) {
        if (node->msvc != NULL) {
            node++;
            i++;
        } else {
            break;
        }
    }

    if (i >= SVC_ENTRY_MAX) {
        printf("Error: no space left for service register.");
        return -1;
    }

    node->msvc = svc;
    node->svc = (struct bt_gatt_service *)malloc(sizeof(struct bt_gatt_service));
    if (node->svc == NULL) {
        return -1;
    }

    node->svc->attrs = (struct bt_gatt_attr *)svc->attrs;
    node->svc->attr_count = svc->attr_count;

    return bt_gatt_service_register(node->svc);
}

int bt_mesh_gatt_service_unregister(struct bt_mesh_gatt_service *svc)
{
    struct svc_paire_node *node = &_svc_paire[0];
    int ret, i = 0;

    while (i < SVC_ENTRY_MAX) {
        if (node->msvc != svc) {
            node++;
            i++;
        } else {
            break;
        }
    }

    if (i >= SVC_ENTRY_MAX) {
        return 0;
    }

    ret = bt_gatt_service_unregister(node->svc);
    free(node->svc);

    return ret;
}

int bt_mesh_gatt_notify(bt_mesh_conn_t conn, const struct bt_mesh_gatt_attr *attr,
                        const void *data, uint16_t len)
{
    return bt_gatt_notify((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, data, len);
}

int bt_mesh_gatt_attr_read(bt_mesh_conn_t conn, const struct bt_mesh_gatt_attr *attr,
                           void *buf, uint16_t buf_len, uint16_t offset,
                           const void *value, uint16_t value_len)
{
    return bt_gatt_attr_read((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, buf, buf_len, offset, value, value_len);
}

uint16_t bt_mesh_gatt_get_mtu(bt_mesh_conn_t conn)
{
    return bt_gatt_get_mtu((struct bt_conn *)conn);
}

int bt_mesh_gatt_attr_read_service(bt_mesh_conn_t conn,
                                   const struct bt_mesh_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_service((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, buf, len, offset);
}

int bt_mesh_gatt_attr_read_chrc(bt_mesh_conn_t conn,
                               const struct bt_mesh_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_chrc((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, buf, len, offset);
}
#else
void bt_mesh_conn_cb_register(struct bt_mesh_conn_cb *cb)
{
    return;
}

bt_mesh_conn_t bt_mesh_conn_ref(bt_mesh_conn_t conn)
{
    return conn;
}

void bt_mesh_conn_unref(bt_mesh_conn_t conn)
{
    return;
}

int bt_mesh_conn_disconnect(bt_mesh_conn_t conn, uint8_t reason)
{
    return 0;
}

int bt_mesh_gatt_service_register(struct bt_mesh_gatt_service *svc)
{
    return 0;
}

int bt_mesh_gatt_service_unregister(struct bt_mesh_gatt_service *svc)
{
    return 0;
}

int bt_mesh_gatt_notify(bt_mesh_conn_t conn, const struct bt_mesh_gatt_attr *attr,
                        const void *data, uint16_t len)
{
    return 0;
}

int bt_mesh_gatt_attr_read(bt_mesh_conn_t conn, const struct bt_mesh_gatt_attr *attr,
                           void *buf, uint16_t buf_len, uint16_t offset,
                           const void *value, uint16_t value_len)
{
    return 0;
}

uint16_t bt_mesh_gatt_get_mtu(bt_mesh_conn_t conn)
{
    return 0;
}

int bt_mesh_gatt_attr_read_service(bt_mesh_conn_t conn,
                                   const struct bt_mesh_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset)
{
    return 0;
}

int bt_mesh_gatt_attr_read_chrc(bt_mesh_conn_t conn,
                               const struct bt_mesh_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset)
{
    return 0;
}

int bt_mesh_adv_start(const struct bt_mesh_le_adv_param *param,
                      const struct bt_mesh_data *ad, size_t ad_len,
                      const struct bt_mesh_data *sd, size_t sd_len)
{
    return 0;
}

int bt_mesh_adv_stop(void)
{
    return 0;
}

int bt_mesh_scan_start(const struct bt_mesh_le_scan_param *param, bt_mesh_le_scan_cb_t cb)
{
    return 0;
}

int bt_mesh_scan_stop(void)
{
    return 0;
}
#endif
