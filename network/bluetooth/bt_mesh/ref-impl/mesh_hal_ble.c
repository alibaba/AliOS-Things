/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <port/mesh_hal_ble.h>

#ifndef CONFIG_MESH_STACK_ALONE
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/bluetooth.h>

#include "provisioner_prov.h"

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
    struct bt_gatt_service svc;
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
    node->svc.attrs = (struct bt_gatt_attr *)svc->attrs;
    node->svc.attr_count = svc->attr_count;

    return bt_gatt_service_register(&(node->svc));
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

    ret = bt_gatt_service_unregister(&(node->svc));

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

#if CONFIG_BT_MESH_PROVISIONER

#define BT_MESH_GATTC_APP_UUID_BYTE 0x97
struct gattc_prov_info {
    /* Service to be found depends on the type of adv pkt received */
    u16_t          srvc_uuid;
    u8_t           addr[6];
    u8_t           addr_type;
    //struct bt_conn conn;
    u16_t          mtu;
    bool           wr_desc_done;    /* Indicate if write char descriptor event is received */
    u16_t          start_handle;    /* Service attribute start handle */
    u16_t          end_handle;      /* Service attribute end handle */
    u16_t          data_in_handle;  /* Data In Characteristic attribute handle */
    u16_t          data_out_handle; /* Data Out Characteristic attribute handle */
    u16_t          ccc_handle;      /* Data Out Characteristic CCC attribute handle */
} gattc_info[CONFIG_BT_MESH_PBG_SAME_TIME];

bool bt_prov_check_gattc_id(int id, const bt_addr_le_t *addr)
{
    u8_t zero[6] = {0};

    if (!addr || !memcmp(addr->a.val, zero, 6) || (addr->type > BT_ADDR_LE_RANDOM)) {
        //BT_ERR("%s: invalid parameters", __func__);
        return false;
    }

    memcpy(gattc_info[id].addr, addr->a.val, 6);
    gattc_info[id].addr_type = addr->type;

	return true;
}

void bt_gattc_conn_create(int id, u16_t srvc_uuid)
{
	bt_addr_le_t 	peer;
	struct bt_le_conn_param conn_param;
	struct bt_conn *conn;

    /** Min_interval:        250ms
     *  Max_interval:        250ms
     *  Slave_latency:       0x0
     *  Supervision_timeout: 32sec
    */
	conn_param.interval_min	= 0xC8;
	conn_param.interval_max	= 0xC8;
	conn_param.latency		= 0x00;
	conn_param.timeout		= 0xC80;

	peer.type = gattc_info[id].addr_type;

	memcpy(peer.a.val, gattc_info[id].addr, 6);

	printk("type:%u addr: %02x, %02x\r\n", peer.type, peer.a.val[0], peer.a.val[1]);

	if (bt_le_scan_stop())
	{
		printk("scan stop error !!!!!!!!!!!!!\r\n");

		return;
	}

	conn = bt_conn_create_le(&peer, &conn_param);

	if (conn == NULL) {
		printk("conn is NULL NULL NULL NULL\r\n");
	}

	/* Increment pbg_count */
	provisioner_pbg_count_inc();

	/* Service to be found after exhanging mtu size */
	gattc_info[id].srvc_uuid = srvc_uuid;
}


#endif

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
