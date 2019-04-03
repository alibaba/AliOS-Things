// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <zephyr.h>
#include <errno.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <net/buf.h>
#include <api/mesh.h>
#include "mesh_def.h"


#if CONFIG_BT_MESH
#include "mesh_bt_uuid.h"

#include "common/log.h"

#include "net.h"
#include "beacon.h"
#include "foundation.h"
#include "provisioner_prov.h"
#include "provisioner_proxy.h"
#include "provisioner_beacon.h"

//#include "bt_mesh_custom_log.h"

#if CONFIG_BT_MESH_PROVISIONER

#define PDU_TYPE(data)     (data[0] & BIT_MASK(6))
#define PDU_SAR(data)      (data[0] >> 6)

#define SAR_COMPLETE       0x00
#define SAR_FIRST          0x01
#define SAR_CONT           0x02
#define SAR_LAST           0x03

#define CFG_FILTER_SET     0x00
#define CFG_FILTER_ADD     0x01
#define CFG_FILTER_REMOVE  0x02
#define CFG_FILTER_STATUS  0x03

#define PDU_HDR(sar, type) (sar << 6 | (type & BIT_MASK(6)))

#define SERVER_BUF_SIZE 68

#define ID_TYPE_NET  0x00
#define ID_TYPE_NODE 0x01

#define NODE_ID_LEN  19
#define NET_ID_LEN   11

#define CLOSE_REASON_PROXY 0xFF

static int conn_count;

static struct bt_mesh_proxy_server {
    bt_mesh_conn_t conn;
    /* Provisioner can use filter to double check the dst within mesh messages */
    u16_t filter[CONFIG_BT_MESH_PROXY_FILTER_SIZE];
    enum __packed {
        NONE,
        WHITELIST,
        BLACKLIST,
        PROV,
    } filter_type;
    u8_t msg_type;
    struct net_buf_simple buf;
    u8_t server_buf_data[SERVER_BUF_SIZE];
} servers[CONFIG_BT_MESH_PBG_SAME_TIME];

static struct bt_mesh_proxy_server *find_server(bt_mesh_conn_t conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn == conn) {
            return &servers[i];
        }
    }

    return NULL;
}

static int filter_status(struct bt_mesh_proxy_server *server,
                         struct net_buf_simple *buf)
{
    /* TODO: Deal with received proxy configuration status messages */
    return 0;
}

#if 0
static void send_filter_set(struct bt_mesh_proxy_server *server,
                            struct bt_mesh_net_rx *rx,
                            struct net_buf_simple *buf)
{
    /* TODO: Act as proxy client, send proxy configuration set messages */
}

static void send_filter_add(struct bt_mesh_proxy_server *server,
                            struct bt_mesh_net_rx *rx,
                            struct net_buf_simple *buf)
{
    /* TODO: Act as proxy client, send proxy configuration add messages */
}

static void send_filter_remove(struct bt_mesh_proxy_server *server,
                               struct bt_mesh_net_rx *rx,
                               struct net_buf_simple *buf)
{
    /* TODO: Act as proxy client, send proxy configuration remove messages */
}
#endif

static void proxy_cfg(struct bt_mesh_proxy_server *server)
{
    struct net_buf_simple *buf = NET_BUF_SIMPLE(29);
    struct bt_mesh_net_rx rx;
    u8_t opcode;
    int err;

    /** In order to deal with proxy configuration messages, provisioner should
     *  do sth. like create mesh network after each device is provisioned.
     */
    err = bt_mesh_net_decode(&server->buf, BT_MESH_NET_IF_PROXY_CFG,
                             &rx, buf);
    if (err) {
        BT_ERR("Failed to decode Proxy Configuration (err %d)", err);
        return;
    }

    /* Remove network headers */
    net_buf_simple_pull(buf, BT_MESH_NET_HDR_LEN);

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (buf->len < 1) {
        BT_WARN("Too short proxy configuration PDU");
        return;
    }

    opcode = net_buf_simple_pull_u8(buf);
    switch (opcode) {
    case CFG_FILTER_STATUS:
        filter_status(server, buf);
        break;
    default:
        BT_WARN("Unhandled configuration OpCode 0x%02x", opcode);
        break;
    }
}

static void proxy_complete_pdu(struct bt_mesh_proxy_server *server)
{
    switch (server->msg_type) {
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    case BT_MESH_PROXY_NET_PDU:
        BT_DBG("Mesh Network PDU");
        bt_mesh_net_recv(&server->buf, 0, BT_MESH_NET_IF_PROXY);
        break;
    case BT_MESH_PROXY_BEACON:
        BT_DBG("Mesh Beacon PDU");
        provisioner_beacon_recv(&server->buf);
        break;
    case BT_MESH_PROXY_CONFIG:
        BT_DBG("Mesh Configuration PDU");
        proxy_cfg(server);
        break;
#endif
#if defined(CONFIG_BT_MESH_PB_GATT)
    case BT_MESH_PROXY_PROV:
        BT_DBG("Mesh Provisioning PDU");
        provisioner_pb_gatt_recv(server->conn, &server->buf);
        break;
#endif
    default:
        BT_WARN("Unhandled Message Type 0x%02x", server->msg_type);
        break;
    }

    net_buf_simple_init(&server->buf, 0);
}

#define ATTR_IS_PROV(uuid) (uuid == BT_UUID_MESH_PROV_VAL)

static ssize_t proxy_recv(bt_mesh_conn_t conn,
                          const struct bt_mesh_gatt_attr *attr, const void *buf,
                          u16_t len, u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_server *server = find_server(conn);
    const u8_t *data = buf;
    u16_t srvc_uuid = 0;

    if (!server) {
        return -ENOTCONN;
    }

    if (len < 1) {
        BT_WARN("Too small Proxy PDU");
        return -EINVAL;
    }

    srvc_uuid = bt_mesh_get_srvc_uuid(conn);
    if (!srvc_uuid) {
        BT_ERR("No service uuid found");
        return -ENOTCONN;
    }

    if (ATTR_IS_PROV(srvc_uuid) != (PDU_TYPE(data) == BT_MESH_PROXY_PROV)) {
        BT_WARN("Proxy PDU type doesn't match GATT service uuid");
        return -EINVAL;
    }

    if (len - 1 > net_buf_simple_tailroom(&server->buf)) {
        BT_WARN("Too big proxy PDU");
        return -EINVAL;
    }

    switch (PDU_SAR(data)) {
    case SAR_COMPLETE:
        if (server->buf.len) {
            BT_WARN("Complete PDU while a pending incomplete one");
            return -EINVAL;
        }

        server->msg_type = PDU_TYPE(data);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        proxy_complete_pdu(server);
        break;

    case SAR_FIRST:
        if (server->buf.len) {
            BT_WARN("First PDU while a pending incomplete one");
            return -EINVAL;
        }

        server->msg_type = PDU_TYPE(data);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        break;

    case SAR_CONT:
        if (!server->buf.len) {
            BT_WARN("Continuation with no prior data");
            return -EINVAL;
        }

        if (server->msg_type != PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in continuation");
            return -EINVAL;
        }

        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        break;

    case SAR_LAST:
        if (!server->buf.len) {
            BT_WARN("Last SAR PDU with no prior data");
            return -EINVAL;
        }

        if (server->msg_type != PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in last SAR PDU");
            return -EINVAL;
        }

        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        proxy_complete_pdu(server);
        break;
    }

    return len;
}

extern struct k_sem proxy_complete_sem;
static void proxy_open_complete(bt_mesh_conn_t conn)
{
    struct bt_mesh_proxy_server *server;

    server = find_server(conn);
    if (!server) {
        BT_ERR("No Proxy Server found");

        return;
    }

    if (server->filter_type == NONE) {
        server->filter_type = WHITELIST;
    }

    k_sem_give(&proxy_complete_sem);

    return;
}

static void prov_open_complete(bt_mesh_conn_t conn)
{
    struct bt_mesh_proxy_server *server;

    server = find_server(conn);
    if (!server) {
        BT_ERR("No Proxy Server found");

        return;
    }

    if (server->filter_type == NONE) {
        server->filter_type = PROV;

        provisioner_pb_gatt_open(conn, NULL);
    }
}

static void proxy_prov_connected(bt_mesh_conn_t conn, int id)
{
    struct bt_mesh_proxy_server *server = NULL;

    conn_count++;

    if (!servers[id].conn) {
        server = &servers[id];
    }

    if (!server) {
        BT_ERR("No matching Proxy Client objects");
        /** Disconnect current connection, clear part of prov_link
         *  information, like uuid, dev_addr, linking flag, etc.
         */

        return;
    }

    server->conn = bt_conn_ref(conn);
    server->filter_type = NONE;
    memset(server->filter, 0, sizeof(server->filter));
    net_buf_simple_init(&server->buf, 0);

    if (provisioner_set_prov_conn(server->conn, id)) {
        BT_ERR("%s: Fail to set prov_conn", __func__);
        bt_mesh_conn_disconnect(server->conn, BT_MESH_HCI_ERR_REMOTE_USER_TERM_CONN);
        return;
    }

    if (BT_UUID_MESH_PROXY_VAL == bt_mesh_get_srvc_uuid(server->conn)) {
        bt_mesh_gatt_conn_open(server->conn, proxy_open_complete);
    } else if (BT_UUID_MESH_PROV_VAL == bt_mesh_get_srvc_uuid(server->conn)) {
        bt_mesh_gatt_conn_open(server->conn, prov_open_complete);
    }

}

static void proxy_prov_disconnected(bt_mesh_conn_t conn, u8_t reason)
{
    int i;

    BT_DBG("conn %p, handle is %d, reason 0x%02x", conn, conn->handle, reason);

    if (conn_count) {
        conn_count--;
    }

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn == conn) {
            if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
                    server->filter_type == PROV) {
                provisioner_pb_gatt_close(conn, reason);
            }
            server->conn = NULL;
            break;
        }
    }
}

#if defined(CONFIG_BT_MESH_PB_GATT)
static ssize_t prov_write_ccc_descr(bt_mesh_conn_t conn, u8_t *addr)
{
    struct bt_mesh_proxy_server *server;

    server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server found");
        return -ENOTCONN;
    }

    if (server->filter_type == NONE) {
        server->filter_type = PROV;
        return provisioner_pb_gatt_open(conn, addr);
    }

    return -EINVAL;
}

static ssize_t prov_notification(bt_mesh_conn_t conn, u8_t *data, u16_t len)
{
    struct bt_mesh_proxy_server *server;

    server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server found");
        return -ENOTCONN;
    }

    if (server->filter_type == PROV) {
        return proxy_recv(conn, NULL, data, len, 0, 0);
    }

    return -EINVAL;
}

int provisioner_pb_gatt_enable(void)
{
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn) {
            servers[i].filter_type = PROV;
        }
    }

    return 0;
}

int provisioner_pb_gatt_disable(void)
{
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn && server->filter_type == PROV) {
            bt_mesh_conn_disconnect(server->conn, BT_MESH_HCI_ERR_REMOTE_USER_TERM_CONN);
            server->filter_type = NONE;
        }
    }

    return 0;
}

#endif /* CONFIG_BT_MESH_PB_GATT */

#if defined(CONFIG_BT_MESH_GATT_PROXY)
static ssize_t proxy_write_ccc_descr(bt_mesh_conn_t conn)
{
    struct bt_mesh_proxy_server *server;

    server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server found");
        return -ENOTCONN;
    }

    if (server->filter_type == NONE) {
        server->filter_type = WHITELIST;
        return 0;
    }

    return -EINVAL;
}

static ssize_t proxy_notification(bt_mesh_conn_t conn, u8_t *data, u16_t len)
{
    return proxy_recv(conn, NULL, data, len, 0, 0);
}

/** Currently provisioner does't need bt_mesh_provisioner_proxy_enable()
 *  and bt_mesh_provisioner_proxy_disable() functions, and once they are
 *  used, provisioner can be enabled to parse node_id_adv and net_id_adv
 *  in order to support proxy client role.
 *  And if gatt_proxy is disabled, provisioner can stop dealing with
 *  these two kinds of connectable advertising packets.
 */
int bt_mesh_provisioner_proxy_enable(void)
{
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn) {
            servers[i].filter_type = WHITELIST;
        }
    }

    /** TODO: Once at leat one device has been provisioned, provisioner
     *  can be set to allow receiving and parsing node_id & net_id adv
     *  packets, and we may use a global flag to indicate this.
     */

    return 0;
}

static void bt_mesh_proxy_gatt_proxy_disconnect(void)
{
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn && (server->filter_type == WHITELIST ||
                             server->filter_type == BLACKLIST)) {
            server->filter_type = NONE;
            bt_conn_disconnect(server->conn, 0x13);
        }
    }
}

int bt_mesh_provisioner_proxy_disable(void)
{
    BT_DBG("%s", __func__);

    /** TODO: Once this function is invoked, provisioner shall stop
     *  receiving and parsing node_id & net_id adv packets, and if
     *  proxy connection exists, we should disconnect it.
     */

    bt_mesh_proxy_gatt_proxy_disconnect();

    return 0;
}

#endif /* CONFIG_BT_MESH_GATT_PROXY */

static int proxy_send(bt_mesh_conn_t conn, const void *data, u16_t len)
{
    BT_DBG("%u bytes: %s", len, bt_hex(data, len));

#if defined(CONFIG_BT_MESH_GATT_PROXY) || defined(CONFIG_BT_MESH_PB_GATT)

    u16_t handle = bt_gatt_get_data_in_handle(conn);

    return bt_gatt_write_without_response(conn, handle, data, len, false);
#endif

    return 0;
}

static int proxy_prov_segment_and_send(bt_mesh_conn_t conn, u8_t type,
                                       struct net_buf_simple *msg)
{
    u16_t mtu;

    BT_DBG("conn %p type 0x%02x len %u: %s", conn, type, msg->len,
           bt_hex(msg->data, msg->len));

    mtu = bt_gatt_prov_get_mtu(conn);
    if (!mtu) {
        BT_ERR("Conn used to get mtu does not exist");
        return -ENOTCONN;
    }

    /* ATT_MTU - OpCode (1 byte) - Handle (2 bytes) */
    mtu -= 3;
    if (mtu > msg->len) {
        net_buf_simple_push_u8(msg, PDU_HDR(SAR_COMPLETE, type));
        return proxy_send(conn, msg->data, msg->len);
    }

    net_buf_simple_push_u8(msg, PDU_HDR(SAR_FIRST, type));
    proxy_send(conn, msg->data, mtu);
    net_buf_simple_pull(msg, mtu);

    while (msg->len) {
        if (msg->len + 1 < mtu) {
            net_buf_simple_push_u8(msg, PDU_HDR(SAR_LAST, type));
            proxy_send(conn, msg->data, msg->len);
            break;
        }

        net_buf_simple_push_u8(msg, PDU_HDR(SAR_CONT, type));
        proxy_send(conn, msg->data, mtu);
        net_buf_simple_pull(msg, mtu);
    }

    return 0;
}

int provisioner_proxy_send(bt_mesh_conn_t conn, u8_t type,
                           struct net_buf_simple *msg)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server found");
        return -ENOTCONN;
    }

    if ((server->filter_type == PROV) != (type == BT_MESH_PROXY_PROV)) {
        BT_ERR("Invalid PDU type for PROV Client");
        return -EINVAL;
    }

    if ((server->filter_type == WHITELIST) != (type == BT_MESH_PROXY_NET_PDU)) {
        BT_ERR("Invalid PDU type for PROXY Client %u", server->filter_type);
        return -EINVAL;
    }

    return proxy_prov_segment_and_send(conn, type, msg);
}

int provisioner_proxy_pdu_send(struct net_buf_simple *msg)
{
    bt_mesh_conn_t conn;

    conn = bt_mesh_get_curr_conn();

    if (conn == NULL) {
		return -EINVAL;
	}

    return provisioner_proxy_send(conn, BT_MESH_PROXY_NET_PDU, msg);
}

static struct bt_mesh_conn_cb conn_callbacks = {
    .connected = proxy_prov_connected,
    .disconnected = proxy_prov_disconnected,
};

void provisioner_proxy_srv_data_recv(struct net_buf_simple *buf, const bt_mesh_addr_le_t *addr)
{
    /** TODO: Parse node_id_adv or net_id_adv pkts. Currently we
     *  don't support this function, and if realized later, proxy
     *  client need to check if there is server structure left
     *  before create connection with a server.
     *  check conn_count & CONFIG_BT_MESH_PBG_SAME_TIME
     */
    if (provisioner_is_node_provisioned(addr->a.val)) {
        int gattc_id = 0;

        gattc_id = bt_prov_get_gattc_id(addr->a.val);

        if (gattc_id >= 0) {
            bt_gattc_conn_create(gattc_id, BT_UUID_MESH_PROXY_VAL);
        }
    }

    return;
}

int provisioner_proxy_init(void)
{
    int i;

    /* Initialize the server receive buffers */
    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];
        server->conn     = NULL;
        server->buf.size = SERVER_BUF_SIZE;
    }

    bt_mesh_conn_cb_register(&conn_callbacks);

    bt_mesh_gatt_recv_callback(proxy_recv);

    return 0;
}

#endif /* CONFIG_BT_MESH_PROVISIONER */

#endif /* CONFIG_BT_MESH */
