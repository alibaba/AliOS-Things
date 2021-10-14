/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <ble_os.h>
#include "mesh_hal_ble.h"
#include <bt_errno.h>
#include <aos/kernel.h>
#ifndef CONFIG_MESH_STACK_ALONE
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/bluetooth.h>

#include "provisioner_prov.h"

int bt_mesh_adv_start(const struct bt_le_adv_param *param,
                      const struct bt_data *ad, size_t ad_len,
                      const struct bt_data *sd, size_t sd_len)
{
    return bt_le_adv_start((const struct bt_le_adv_param *)param,
                           (const struct bt_data *)ad, ad_len,
                           (const struct bt_data *)sd, sd_len);
}

int bt_mesh_adv_stop(void)
{
    return bt_le_adv_stop();
}

int bt_mesh_scan_start(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb)
{
    return bt_le_scan_start((const struct bt_le_scan_param *)param, cb);
}

int bt_mesh_scan_stop(void)
{
    return bt_le_scan_stop();
}

struct bt_conn_cb conn_callbacks;

void bt_mesh_conn_cb_register(struct bt_conn_cb *cb)
{
    conn_callbacks.connected = cb->connected;
    conn_callbacks.disconnected = cb->disconnected;
    bt_conn_cb_register(&conn_callbacks);
}

struct bt_conn * bt_mesh_conn_ref(struct bt_conn * conn)
{
    return bt_conn_ref((struct bt_conn *)conn);
}

void bt_mesh_conn_unref(struct bt_conn * conn)
{
    bt_conn_unref((struct bt_conn *)conn);
}

int bt_mesh_conn_disconnect(struct bt_conn * conn, uint8_t reason)
{
    return bt_conn_disconnect((struct bt_conn *)conn, reason);
}

#define SVC_ENTRY_MAX 16

struct svc_paire_node {
    struct bt_gatt_service *msvc;
    struct bt_gatt_service svc;
} _svc_paire[SVC_ENTRY_MAX] = {{0}};

/* TODO: manage the services in linked list. */
int bt_mesh_gatt_service_register(struct bt_gatt_service *svc)
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

int bt_mesh_gatt_service_unregister(struct bt_gatt_service *svc)
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

int bt_mesh_gatt_notify(struct bt_conn * conn, const struct bt_gatt_attr *attr,
                        const void *data, uint16_t len)
{
    return bt_gatt_notify((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, data, len);
}

int bt_mesh_gatt_attr_read(struct bt_conn * conn, const struct bt_gatt_attr *attr,
                           void *buf, uint16_t buf_len, uint16_t offset,
                           const void *value, uint16_t value_len)
{
    return bt_gatt_attr_read((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, buf, buf_len, offset, value, value_len);
}

uint16_t bt_mesh_gatt_get_mtu(struct bt_conn * conn)
{
    return bt_gatt_get_mtu((struct bt_conn *)conn);
}

int bt_mesh_gatt_attr_read_service(struct bt_conn * conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_service((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, buf, len, offset);
}

int bt_mesh_gatt_attr_read_chrc(struct bt_conn * conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_chrc((struct bt_conn *)conn, (const struct bt_gatt_attr *)attr, buf, len, offset);
}

#ifdef CONFIG_BT_MESH_PROVISIONER

typedef void (*gatt_open_complete_cb)(struct bt_conn *conn);

#define BT_MESH_GATTC_APP_UUID_BYTE 0x97
struct gattc_prov_info {
    /* Service to be found depends on the type of adv pkt received */
    u16_t          srvc_uuid;
    u8_t           addr[6];
    u8_t           addr_type;
    struct bt_conn *conn;
    u16_t          mtu;
    bool           wr_desc_done;    /* Indicate if write char descriptor event is received */
    u16_t          start_handle;    /* Service attribute start handle */
    u16_t          end_handle;      /* Service attribute end handle */
    u16_t          data_in_handle;  /* Data In Characteristic attribute handle */
    u16_t          data_out_handle; /* Data Out Characteristic attribute handle */
    u16_t          ccc_handle;      /* Data Out Characteristic CCC attribute handle */
} gattc_info[CONFIG_BT_MESH_PBG_SAME_TIME];

static struct bt_conn *gatt_conn;
static struct bt_gatt_exchange_params mtu_param;
static struct bt_gatt_discover_params  discov_param;
static struct bt_gatt_subscribe_params subscribe_param;
static gatt_open_complete_cb bt_mesh_gatt_open_complete;
static gatt_recv_cb          bt_mesh_gatt_recv;

static struct bt_uuid_16 prov_primary_uuid = BT_UUID_INIT_16(0x1827);
static struct bt_uuid_16 prov_character_data_in_uuid = BT_UUID_INIT_16(0x2adb);
static struct bt_uuid_16 prov_descriptor_data_in_uuid = BT_UUID_INIT_16(0x2adb);
static struct bt_uuid_16 prov_character_data_out_uuid = BT_UUID_INIT_16(0x2adc);
//static struct bt_uuid_16 prov_descriptor_data_out_uuid = BT_UUID_INIT_16(0x2adc);
static struct bt_uuid_16 prov_descriptor_ccc_uuid = BT_UUID_INIT_16(0x2902);

static struct bt_uuid_16 proxy_primary_uuid = BT_UUID_INIT_16(0x1828);
static struct bt_uuid_16 proxy_character_data_in_uuid = BT_UUID_INIT_16(0x2add);
static struct bt_uuid_16 proxy_descriptor_data_in_uuid = BT_UUID_INIT_16(0x2add);
static struct bt_uuid_16 proxy_character_data_out_uuid = BT_UUID_INIT_16(0x2ade);
//static struct bt_uuid_16 proxy_descriptor_data_out_uuid = BT_UUID_INIT_16(0x2ade);
static struct bt_uuid_16 proxy_descriptor_ccc_uuid = BT_UUID_INIT_16(0x2902);


int bt_prov_get_gattc_id(u8_t *addr)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (!memcmp(addr, gattc_info[i].addr, 6)) {
            return i;
        }
    }

    return -1;
}

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

void set_my_addr(u8_t index, const u8_t *addr, u8_t type)
{
    gattc_info[index].addr_type = type;
    memcpy(gattc_info[index].addr, addr, 6);
}

int bt_gattc_conn_create(int id, u16_t srvc_uuid)
{
    int err;
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

	//BT_DBG("type:%u addr: %02x, %02x\r\n", peer.type, peer.a.val[0], peer.a.val[1]);
	extern int bt_mesh_scan_disable(void);
	err = bt_mesh_scan_disable();
	if (err && err != -EALREADY)
	{
		return err;
	}
	//add relay to ensure the scan has been disabled
	aos_msleep(10);
	conn = bt_conn_create_le(&peer, &conn_param);

	if (conn == NULL) {
		return -EIO;
	}
    else
    {
        bt_conn_unref(conn);
    }

    gattc_info[id].conn = conn;

	/* Increment pbg_count */
	provisioner_pbg_count_inc();

	/* Service to be found after exhanging mtu size */
	gattc_info[id].srvc_uuid = srvc_uuid;

    gatt_conn = conn;

    return 0;
}

struct bt_conn * bt_mesh_get_curr_conn(void)
{
    return gatt_conn;
}

u16_t bt_mesh_get_srvc_uuid(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            break;
        }
    }

    if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
        return 0;
    }

    return gattc_info[i].srvc_uuid;
}

void bt_gatt_prov_set_mtu(struct bt_conn *conn, u16_t mtu)
{
    int i;

    if (mtu > 40) {
        mtu = 40;
    }

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            gattc_info[i].mtu = mtu;
        }
    }
}

u16_t bt_gatt_prov_get_mtu(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            return gattc_info[i].mtu;
        }
    }

    return 0;
}

void bt_gatt_set_data_in_handle(struct bt_conn *conn, u16_t handle)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            gattc_info[i].data_in_handle = handle;

            break;
        }
    }
}

u16_t bt_gatt_get_data_in_handle(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            return gattc_info[i].data_in_handle;
        }
    }

    return 0;
}

void bt_gatt_set_data_out_handle(struct bt_conn *conn, u16_t handle)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            gattc_info[i].data_out_handle = handle;

            break;
        }
    }
}

u16_t bt_gatt_get_data_out_handle(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            return gattc_info[i].data_out_handle;
        }
    }

    return 0;
}

void bt_gatt_set_ccc_handle(struct bt_conn *conn, u16_t handle)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            gattc_info[i].ccc_handle = handle;
        }
    }
}

u16_t bt_gatt_get_ccc_handle(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == gattc_info[i].conn) {
            return gattc_info[i].ccc_handle;
        }
    }

    return 0;
}

static u8_t proxy_prov_notify_func(struct bt_conn *conn,
            struct bt_gatt_subscribe_params *param,
            const void *buf, u16_t len)
{
    if (bt_mesh_gatt_recv) {
        bt_mesh_gatt_recv(conn, NULL, buf, len, 0, 0);
    }

    return BT_GATT_ITER_CONTINUE;
}

static u8_t proxy_prov_discover_func(struct bt_conn *conn,
					const struct bt_gatt_attr *attr,
					struct bt_gatt_discover_params *params)
{
    if (params->uuid == &prov_primary_uuid.uuid) {
        discov_param.uuid = &prov_character_data_in_uuid.uuid;
        discov_param.start_handle = attr->handle + 1;
        discov_param.type = BT_GATT_DISCOVER_CHARACTERISTIC;

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &prov_character_data_in_uuid.uuid) {
        discov_param.uuid = &prov_descriptor_data_in_uuid.uuid;
        discov_param.start_handle = attr->handle + 1;
        discov_param.type = BT_GATT_DISCOVER_DESCRIPTOR;

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &prov_descriptor_data_in_uuid.uuid) {
        discov_param.uuid = &prov_character_data_out_uuid.uuid;
        discov_param.start_handle = attr->handle + 1;
        discov_param.type = BT_GATT_DISCOVER_CHARACTERISTIC;

        bt_gatt_set_data_in_handle(conn, attr->handle);

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &prov_character_data_out_uuid.uuid) {
        discov_param.uuid = &prov_descriptor_ccc_uuid.uuid;
        discov_param.start_handle = attr->handle + 2;
        discov_param.type = BT_GATT_DISCOVER_DESCRIPTOR;
        subscribe_param.value_handle = attr->handle + 1;

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &prov_descriptor_ccc_uuid.uuid) {
        bt_gatt_set_ccc_handle(conn, attr->handle);

        int err;

        subscribe_param.notify = proxy_prov_notify_func;
        subscribe_param.value = BT_GATT_CCC_NOTIFY;
        subscribe_param.ccc_handle = attr->handle;

        bt_gatt_set_ccc_handle(conn, attr->handle);

        err = bt_gatt_subscribe(conn, &subscribe_param);
        if (err) {
            printf("Subscribe failed (err %d)\r\n", err);

            return 0;
        }

        u16_t   open_data = 0x0001;

		bt_gatt_write_without_response(conn, bt_gatt_get_ccc_handle(conn), &open_data, 2, false);

        if (bt_mesh_gatt_open_complete) {
            bt_mesh_gatt_open_complete(conn);
        }
	}

    return 0;
}

static u8_t proxy_discover_func(struct bt_conn *conn,
					const struct bt_gatt_attr *attr,
					struct bt_gatt_discover_params *params)
{
    if (params->uuid == &proxy_primary_uuid.uuid) {
        discov_param.uuid = &proxy_character_data_in_uuid.uuid;
        discov_param.start_handle = attr->handle + 1;
        discov_param.type = BT_GATT_DISCOVER_CHARACTERISTIC;

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &proxy_character_data_in_uuid.uuid) {
        discov_param.uuid = &proxy_descriptor_data_in_uuid.uuid;
        discov_param.start_handle = attr->handle + 1;
        discov_param.type = BT_GATT_DISCOVER_DESCRIPTOR;

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &proxy_descriptor_data_in_uuid.uuid) {
        discov_param.uuid = &proxy_character_data_out_uuid.uuid;
        discov_param.start_handle = attr->handle + 1;
        discov_param.type = BT_GATT_DISCOVER_CHARACTERISTIC;

        bt_gatt_set_data_in_handle(conn, attr->handle);

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &proxy_character_data_out_uuid.uuid) {
        discov_param.uuid = &proxy_descriptor_ccc_uuid.uuid;
        discov_param.start_handle = attr->handle + 2;
        discov_param.type = BT_GATT_DISCOVER_DESCRIPTOR;
        subscribe_param.value_handle = attr->handle + 1;

        bt_gatt_discover(conn, &discov_param);
    } else if (params->uuid == &proxy_descriptor_ccc_uuid.uuid) {
        bt_gatt_set_ccc_handle(conn, attr->handle);

        int err;

        subscribe_param.notify = proxy_prov_notify_func;
        subscribe_param.value = BT_GATT_CCC_NOTIFY;
        subscribe_param.ccc_handle = attr->handle;

        bt_gatt_set_ccc_handle(conn, attr->handle);

        err = bt_gatt_subscribe(conn, &subscribe_param);
        if (err) {
            printf("Subscribe failed (err %d)\r\n", err);

            return 0;
        }

        if (bt_mesh_gatt_open_complete) {
            bt_mesh_gatt_open_complete(conn);
        }
    }

    return 0;
}

static void proxy_prov_get_mtu_response(struct bt_conn *conn, u8_t err,
                                                 struct bt_gatt_exchange_params *params)
{
    u16_t mtu;
    //u8_t  i;

    if (err) {
        //printf("%s mtu response failed (err %u)\n", __func__, err);
    } else {
        mtu = bt_gatt_get_mtu(conn);

        bt_gatt_prov_set_mtu(conn, mtu);

        if (bt_mesh_get_srvc_uuid(conn) == BT_UUID_MESH_PROXY_VAL) {
            discov_param.uuid = &proxy_primary_uuid.uuid;
            discov_param.func = proxy_discover_func;
        } else if (bt_mesh_get_srvc_uuid(conn) == BT_UUID_MESH_PROV_VAL) {
            discov_param.uuid = &prov_primary_uuid.uuid;
            discov_param.func = proxy_prov_discover_func;
        }

		discov_param.start_handle = 0x0001;
		discov_param.end_handle = 0xffff;
		discov_param.type = BT_GATT_DISCOVER_PRIMARY;

        bt_gatt_discover(conn, &discov_param);
    }
}

void bt_mesh_gatt_conn_open(struct bt_conn *conn, void (*open_complete)(struct bt_conn *conn))
{
    mtu_param.func = proxy_prov_get_mtu_response;

    bt_mesh_gatt_open_complete = open_complete;

    bt_gatt_exchange_mtu(conn, &mtu_param);

    return;
}

void bt_mesh_gatt_recv_callback(gatt_recv_cb recv_callback)
{
    bt_mesh_gatt_recv = recv_callback;

    return;
}

#endif

#else
void bt_mesh_conn_cb_register(struct bt_mesh_conn_cb *cb)
{
    return;
}

struct bt_conn * bt_mesh_conn_ref(struct bt_conn * conn)
{
    return conn;
}

void bt_mesh_conn_unref(struct bt_conn * conn)
{
    return;
}

int bt_mesh_conn_disconnect(struct bt_conn * conn, uint8_t reason)
{
    return 0;
}

int bt_gatt_service_register(struct bt_gatt_service *svc)
{
    return 0;
}

int bt_gatt_service_unregister(struct bt_gatt_service *svc)
{
    return 0;
}

int bt_mesh_gatt_notify(struct bt_conn * conn, const struct bt_gatt_attr *attr,
                        const void *data, uint16_t len)
{
    return 0;
}

int bt_mesh_gatt_attr_read(struct bt_conn * conn, const struct bt_gatt_attr *attr,
                           void *buf, uint16_t buf_len, uint16_t offset,
                           const void *value, uint16_t value_len)
{
    return 0;
}

uint16_t bt_mesh_gatt_get_mtu(struct bt_conn * conn)
{
    return 0;
}

int bt_mesh_gatt_attr_read_service(struct bt_conn * conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset)
{
    return 0;
}

int bt_mesh_gatt_attr_read_chrc(struct bt_conn * conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset)
{
    return 0;
}

int bt_mesh_adv_start(const struct bt_le_adv_param *param,
                      const struct bt_data *ad, size_t ad_len,
                      const struct bt_data *sd, size_t sd_len)
{
    return 0;
}

int bt_mesh_adv_stop(void)
{
    return 0;
}

int bt_mesh_scan_start(const struct bt_le_scan_param *param, bt_mesh_le_scan_cb_t cb)
{
    return 0;
}

int bt_mesh_scan_stop(void)
{
    return 0;
}
#endif
