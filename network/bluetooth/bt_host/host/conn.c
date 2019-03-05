/* conn.c - Bluetooth connection handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <atomic.h>
#include <misc/byteorder.h>
#include <misc/util.h>
#include <misc/slist.h>
#include <misc/stack.h>
#include <misc/__assert.h>

#include <bluetooth/hci.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/hci_driver.h>
#include <bluetooth/att.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_CONN)
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "keys.h"
#include "smp.h"
#include "att_internal.h"

NET_BUF_POOL_DEFINE(acl_tx_pool, CONFIG_BT_L2CAP_TX_BUF_COUNT,
                    BT_L2CAP_BUF_SIZE(CONFIG_BT_L2CAP_TX_MTU),
                    BT_BUF_USER_DATA_MIN, NULL);

#ifdef CONFIG_BLE_LINK_PARAMETERS
#define  SUP_TO_LIMIT         (400)//limit LSP_TO to 4s
#define  CONN_SUP_TIMEOUT     (400)//*10, link superversion timeout
#define  CONN_INTERVAL_MIN    (24)//*1.25 ms,30ms, min connection inverval
#define  CONN_INTERVAL_MAX    (40)//*1.25 ms,50ms, max connection inverval
#endif

extern struct net_buf_pool acl_tx_pool;

/* How long until we cancel HCI_LE_Create_Connection */
#define CONN_TIMEOUT K_SECONDS(3)

#if defined(CONFIG_BT_SMP)
const struct bt_conn_auth_cb *bt_auth;
#endif /* CONFIG_BT_SMP */

static struct bt_conn conns[CONFIG_BT_MAX_CONN];
static struct bt_conn_cb *callback_list;

struct conn_tx_cb {
    bt_conn_tx_cb_t cb;
};

#define conn_tx(buf) ((struct conn_tx_cb *)net_buf_user_data(buf))

static struct bt_conn_tx conn_tx[CONFIG_BT_CONN_TX_MAX];
static sys_slist_t free_tx = SYS_SLIST_STATIC_INIT(&free_tx);

static inline const char *state2str(bt_conn_state_t state)
{
    switch (state) {
        case BT_CONN_DISCONNECTED:
            return "disconnected";
        case BT_CONN_CONNECT_SCAN:
            return "connect-scan";
        case BT_CONN_CONNECT:
            return "connect";
        case BT_CONN_CONNECTED:
            return "connected";
        case BT_CONN_DISCONNECT:
            return "disconnect";
        default:
            return "(unknown)";
    }
}

static void notify_connected(struct bt_conn *conn)
{
    struct bt_conn_cb *cb;

    for (cb = callback_list; cb; cb = cb->_next) {
        if (cb->connected) {
            cb->connected(conn, conn->err);
        }
    }
}

static void notify_disconnected(struct bt_conn *conn)
{
    struct bt_conn_cb *cb;

    for (cb = callback_list; cb; cb = cb->_next) {
        if (cb->disconnected) {
            cb->disconnected(conn, conn->err);
        }
    }
}

void notify_le_param_updated(struct bt_conn *conn)
{
    struct bt_conn_cb *cb;

    for (cb = callback_list; cb; cb = cb->_next) {
        if (cb->le_param_updated) {
            cb->le_param_updated(conn, conn->le.interval, conn->le.latency,
                                 conn->le.timeout);
        }
    }
}

bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    struct bt_conn_cb *cb;

    if (!bt_le_conn_params_valid(param)) {
        return false;
    }

    for (cb = callback_list; cb; cb = cb->_next) {
        if (!cb->le_param_req) {
            continue;
        }

        if (!cb->le_param_req(conn, param)) {
            return false;
        }

        /* The callback may modify the parameters so we need to
         * double-check that it returned valid parameters.
         */
        if (!bt_le_conn_params_valid(param)) {
            return false;
        }
    }

    /* Default to accepting if there's no app callback */
    return true;
}

static void le_conn_update(struct k_work *work)
{
    struct bt_conn_le *le = CONTAINER_OF(work, struct bt_conn_le, update_work);
    struct bt_conn *conn = CONTAINER_OF(le, struct bt_conn, le);
    struct bt_le_conn_param *param;

    if (IS_ENABLED(CONFIG_BT_CENTRAL) && conn->state == BT_CONN_CONNECT) {
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        return;
    }

    param = BT_LE_CONN_PARAM(conn->le.interval_min, conn->le.interval_max,
                             conn->le.latency, conn->le.timeout);

    bt_conn_le_param_update(conn, param);
}

static struct bt_conn *conn_new(void)
{
    struct bt_conn *conn = NULL;
    int             i;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        if (!atomic_get(&conns[i].ref)) {
            conn = &conns[i];
            break;
        }
    }

    if (!conn) {
        return NULL;
    }

    memset(conn, 0, sizeof(*conn));

    atomic_set(&conn->ref, 1);

    return conn;
}

#if defined(CONFIG_BT_SMP)
void bt_conn_identity_resolved(struct bt_conn *conn)
{
    const bt_addr_le_t *rpa;
    struct bt_conn_cb * cb;

    if (conn->role == BT_HCI_ROLE_MASTER) {
        rpa = &conn->le.resp_addr;
    } else {
        rpa = &conn->le.init_addr;
    }

    for (cb = callback_list; cb; cb = cb->_next) {
        if (cb->identity_resolved) {
            cb->identity_resolved(conn, rpa, &conn->le.dst);
        }
    }
}

int bt_conn_le_start_encryption(struct bt_conn *conn, u64_t rand, u16_t ediv,
                                const u8_t *ltk, size_t len)
{
    struct bt_hci_cp_le_start_encryption *cp;
    struct net_buf *                      buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_START_ENCRYPTION, sizeof(*cp));
    if (!buf) {
        return -ENOBUFS;
    }

    cp         = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(conn->handle);
    cp->rand   = rand;
    cp->ediv   = ediv;

    memcpy(cp->ltk, ltk, len);
    if (len < sizeof(cp->ltk)) {
        memset(cp->ltk + len, 0, sizeof(cp->ltk) - len);
    }

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_START_ENCRYPTION, buf, NULL);
}
#endif /* CONFIG_BT_SMP */

#if defined(CONFIG_BT_SMP)
u8_t bt_conn_enc_key_size(struct bt_conn *conn)
{
    if (!conn->encrypt) {
        return 0;
    }

    if (IS_ENABLED(CONFIG_BT_SMP)) {
        return conn->le.keys ? conn->le.keys->enc_size : 0;
    }

    return 0;
}

void bt_conn_security_changed(struct bt_conn *conn)
{
    struct bt_conn_cb *cb;

    for (cb = callback_list; cb; cb = cb->_next) {
        if (cb->security_changed) {
            cb->security_changed(conn, conn->sec_level);
        }
    }
}

static int start_security(struct bt_conn *conn)
{
    switch (conn->role) {
#if defined(CONFIG_BT_CENTRAL) && defined(CONFIG_BT_SMP)
        case BT_HCI_ROLE_MASTER: {
            if (!conn->le.keys) {
                conn->le.keys = bt_keys_find(BT_KEYS_LTK_P256, &conn->le.dst);
                if (!conn->le.keys) {
                    conn->le.keys = bt_keys_find(BT_KEYS_LTK, &conn->le.dst);
                }
            }

            if (!conn->le.keys ||
                !(conn->le.keys->keys & (BT_KEYS_LTK | BT_KEYS_LTK_P256))) {
                return bt_smp_send_pairing_req(conn);
            }

            if (conn->required_sec_level > BT_SECURITY_MEDIUM &&
                !atomic_test_bit(conn->le.keys->flags, BT_KEYS_AUTHENTICATED)) {
                return bt_smp_send_pairing_req(conn);
            }

            if (conn->required_sec_level > BT_SECURITY_HIGH &&
                !atomic_test_bit(conn->le.keys->flags, BT_KEYS_AUTHENTICATED) &&
                !(conn->le.keys->keys & BT_KEYS_LTK_P256)) {
                return bt_smp_send_pairing_req(conn);
            }

            /* LE SC LTK and legacy master LTK are stored in same place */
            return bt_conn_le_start_encryption(
              conn, conn->le.keys->ltk.rand, conn->le.keys->ltk.ediv,
              conn->le.keys->ltk.val, conn->le.keys->enc_size);
        }
#endif /* CONFIG_BT_CENTRAL && CONFIG_BT_SMP */
#if defined(CONFIG_BT_PERIPHERAL) && defined(CONFIG_BT_SMP)
        case BT_HCI_ROLE_SLAVE:
            return bt_smp_send_security_req(conn);
#endif /* CONFIG_BT_PERIPHERAL && CONFIG_BT_SMP */
        default:
            return -EINVAL;
    }
}

int bt_conn_security(struct bt_conn *conn, bt_security_t sec)
{
    int err;

    if (conn->state != BT_CONN_CONNECTED) {
        return -ENOTCONN;
    }

    if (IS_ENABLED(CONFIG_BT_SMP_SC_ONLY) && sec < BT_SECURITY_FIPS) {
        return -EOPNOTSUPP;
    }

    /* nothing to do */
    if (conn->sec_level >= sec || conn->required_sec_level >= sec) {
        return 0;
    }

    conn->required_sec_level = sec;

    err = start_security(conn);

    /* reset required security level in case of error */
    if (err) {
        conn->required_sec_level = conn->sec_level;
    }

    return err;
}
#endif /* CONFIG_BT_SMP */

void bt_conn_cb_register(struct bt_conn_cb *cb)
{
    cb->_next     = callback_list;
    callback_list = cb;
}

static void bt_conn_reset_tx_state(struct bt_conn *conn)
{
    if (!conn->tx) {
        return;
    }

    net_buf_unref(conn->tx);
    conn->tx = NULL;
}

static void bt_conn_reset_rx_state(struct bt_conn *conn)
{
    if (!conn->rx_len) {
        return;
    }

    net_buf_unref(conn->rx);
    conn->rx = NULL;
    conn->rx_len = 0;
}

void bt_conn_recv(struct bt_conn *conn, struct net_buf *buf, u8_t flags)
{
    struct bt_l2cap_hdr *hdr;
    u16_t                len;

    BT_DBG("handle %u len %u flags %02x", conn->handle, buf->len, flags);

    /* Check packet boundary flags */
    switch (flags) {
        case BT_ACL_START:
            hdr = (void *)buf->data;
            len = sys_le16_to_cpu(hdr->len);

            BT_DBG("First, len %u final %u", buf->len, len);

            if (conn->rx_len) {
                BT_ERR("Unexpected first L2CAP frame");
                bt_conn_reset_rx_state(conn);
            }

            conn->rx_len = (sizeof(*hdr) + len) - buf->len;
            BT_DBG("rx_len %u", conn->rx_len);
            if (conn->rx_len) {
                conn->rx = buf;
                return;
            }

            break;
        case BT_ACL_CONT:
            if (!conn->rx_len) {
                BT_ERR("Unexpected L2CAP continuation");
                bt_conn_reset_rx_state(conn);
                net_buf_unref(buf);
                return;
            }

            if (buf->len > conn->rx_len) {
                BT_ERR("L2CAP data overflow");
                bt_conn_reset_rx_state(conn);
                net_buf_unref(buf);
                return;
            }

            BT_DBG("Cont, len %u rx_len %u", buf->len, conn->rx_len);

            if (buf->len > net_buf_tailroom(conn->rx)) {
                BT_ERR("Not enough buffer space for L2CAP data");
                bt_conn_reset_rx_state(conn);
                net_buf_unref(buf);
                return;
            }

            net_buf_add_mem(conn->rx, buf->data, buf->len);
            conn->rx_len -= buf->len;
            net_buf_unref(buf);

            if (conn->rx_len) {
                return;
            }

            buf = conn->rx;
            conn->rx = NULL;
            conn->rx_len = 0;

            break;
        default:
            BT_ERR("Unexpected ACL flags (0x%02x)", flags);
            bt_conn_reset_rx_state(conn);
            net_buf_unref(buf);
            return;
    }

    hdr = (void *)buf->data;
    len = sys_le16_to_cpu(hdr->len);

    if (sizeof(*hdr) + len != buf->len) {
        BT_ERR("ACL len mismatch (%u != %u)", len, buf->len);
        net_buf_unref(buf);
        return;
    }

    BT_DBG("%s, Successfully parsed %u byte L2CAP packet", __func__, buf->len);

    bt_l2cap_recv(conn, buf);
}

int bt_conn_send_cb(struct bt_conn *conn, struct net_buf *buf, bt_conn_tx_cb_t cb)
{
    struct net_buf_pool *pool;

    BT_DBG("%s, conn handle %u buf len %u cb %p", __func__, conn->handle, buf->len, cb);

    pool = net_buf_pool_get(buf->pool_id);
    if (pool->user_data_size < BT_BUF_USER_DATA_MIN) {
        BT_ERR("Too small user data size");
        net_buf_unref(buf);
        return -EINVAL;
    }

    if (conn->state != BT_CONN_CONNECTED) {
        BT_ERR("not connected!");
        net_buf_unref(buf);
        return -ENOTCONN;
    }

    conn_tx(buf)->cb = cb;

    net_buf_put(&conn->tx_queue, buf);
    return 0;
}

static void tx_free(struct bt_conn_tx *tx)
{
    tx->cb = NULL;
    sys_slist_prepend(&free_tx, &tx->node);
}

void bt_conn_notify_tx(struct bt_conn *conn)
{
    struct bt_conn_tx *tx;

    BT_DBG("conn %p", conn);

    while ((tx = k_fifo_get(&conn->tx_notify, K_NO_WAIT))) {
        if (tx->cb) {
            tx->cb(conn);
        }

        tx_free(tx);
    }
}

static void notify_tx(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        if (!atomic_get(&conns[i].ref)) {
            continue;
        }

        if (conns[i].state == BT_CONN_CONNECTED ||
            conns[i].state == BT_CONN_DISCONNECT) {
            bt_conn_notify_tx(&conns[i]);
        }
    }
}

static sys_snode_t *add_pending_tx(struct bt_conn *conn, bt_conn_tx_cb_t cb)
{
    sys_snode_t *node;
    unsigned int key;

    BT_DBG("%s, conn %p cb %p", __func__, conn, cb);

    __ASSERT(!sys_slist_is_empty(&free_tx), "No free conn TX contexts");

    node = sys_slist_get_not_empty(&free_tx);
    CONTAINER_OF(node, struct bt_conn_tx, node)->cb = cb;

    key = irq_lock();
    sys_slist_append(&conn->tx_pending, node);
    irq_unlock(key);

    return node;
}

static void remove_pending_tx(struct bt_conn *conn, sys_snode_t *node)
{
    unsigned int key;

    key = irq_lock();
    sys_slist_find_and_remove(&conn->tx_pending, node);
    irq_unlock(key);

    tx_free(CONTAINER_OF(node, struct bt_conn_tx, node));
}

static bool send_frag(struct bt_conn *conn, struct net_buf *buf, u8_t flags,
                      bool always_consume)
{
    struct bt_hci_acl_hdr *hdr;
    bt_conn_tx_cb_t        cb;
    sys_snode_t *          node;
    int                    err;

    BT_DBG("%s, conn %p buf %p len %u flags 0x%02x", __func__, conn, buf, buf->len, flags);

    notify_tx();

    /* Check for disconnection while waiting for pkts_sem */
    if (conn->state != BT_CONN_CONNECTED) {
        goto fail;
    }

    hdr = net_buf_push(buf, sizeof(*hdr));
    hdr->handle = sys_cpu_to_le16(bt_acl_handle_pack(conn->handle, flags));
    hdr->len = sys_cpu_to_le16(buf->len - sizeof(*hdr));
    cb = conn_tx(buf)->cb;
    bt_buf_set_type(buf, BT_BUF_ACL_OUT);

    node = add_pending_tx(conn, cb);

    err = bt_send(buf);
    if (err) {
        BT_ERR("Unable to send to driver (err %d)", err);
        remove_pending_tx(conn, node);
        goto fail;
    }

    return true;

fail:
    if (always_consume) {
        net_buf_unref(buf);
    }
    return false;
}

static inline u16_t conn_mtu(struct bt_conn *conn)
{
    return bt_dev.le.mtu;
}

static struct net_buf *create_frag(struct bt_conn *conn, struct net_buf *buf)
{
    struct net_buf *frag;
    u16_t frag_len;

    frag = bt_conn_create_pdu(NULL, 0);
    if (frag == NULL) {
        return NULL;
    }

    if (conn->state != BT_CONN_CONNECTED) {
        net_buf_unref(frag);
        return NULL;
    }

    /* Fragments never have a TX completion callback */
    conn_tx(frag)->cb = NULL;

    frag_len = min(conn_mtu(conn), net_buf_tailroom(frag));

    net_buf_add_mem(frag, buf->data, frag_len);
    net_buf_pull(buf, frag_len);

    return frag;
}

static bool send_buf(struct bt_conn *conn, struct net_buf *buf)
{
    struct net_buf *frag;

    BT_DBG("%s, conn %p buf %p len %u", __func__,conn, buf, buf->len);

    if (buf->len <= conn_mtu(conn)) {
        conn->tx_frag = false;
        if (send_frag(conn, buf, BT_ACL_START_NO_FLUSH, false) == false) {
            return false;
        }
    } else if (buf->len > conn_mtu(conn)) {
        frag = create_frag(conn, buf);
        if ((frag == NULL) || (send_frag(conn, frag, BT_ACL_START_NO_FLUSH, true) == false)) {
            return false;
        }
        conn->tx_frag = true;
    }

    conn->tx = buf;
    return true;
}

static void conn_cleanup(struct bt_conn *conn)
{
    struct net_buf *buf;

    /* Give back any allocated buffers */
    while ((buf = net_buf_get(&conn->tx_queue, K_NO_WAIT))) {
        net_buf_unref(buf);
    }

    __ASSERT(sys_slist_is_empty(&conn->tx_pending), "Pending TX packets");

    bt_conn_notify_tx(conn);
    bt_conn_reset_tx_state(conn);
    bt_conn_reset_rx_state(conn);

    /* Release the reference we took for the very first
     * state transition.
     */
    bt_conn_unref(conn);
}

int bt_conn_prepare_events(struct k_poll_event events[])
{
    int i, ev_count = 0;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        struct bt_conn *conn = &conns[i];

        if (!atomic_get(&conn->ref)) {
            continue;
        }

        if (conn->state == BT_CONN_DISCONNECTED &&
            atomic_test_and_clear_bit(conn->flags, BT_CONN_CLEANUP)) {
            conn_cleanup(conn);
            continue;
        }

        if (conn->state != BT_CONN_CONNECTED) {
            continue;
        }

        k_poll_event_init(&events[ev_count], K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                          K_POLL_MODE_NOTIFY_ONLY, &conn->tx_notify);
        events[ev_count++].tag = BT_EVENT_CONN_TX_NOTIFY;

        k_poll_event_init(&events[ev_count], K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                          K_POLL_MODE_NOTIFY_ONLY, &conn->tx_queue);
        events[ev_count++].tag = BT_EVENT_CONN_TX_QUEUE;
    }

    return ev_count;
}

void bt_conn_notify_tx_done(struct bt_conn *conn)
{
    struct net_buf *frag;

    if (conn->tx == NULL || conn->tx_frag == false) {
        goto exit;
    }

    if (conn->tx->len > conn_mtu(conn)) {
        frag = create_frag(conn, conn->tx);
        if (!frag) {
            goto exit;
        }

        if (!send_frag(conn, frag, BT_ACL_CONT, true)) {
            goto exit;
        }
        return;
    }

    if (conn->tx->len) {
        conn->tx_frag = false;
        if (!send_frag(conn, conn->tx, BT_ACL_CONT, false)) {
            goto exit;
        }
        return;
    }

exit:
    conn->tx = NULL;
}

void bt_conn_process_tx(struct bt_conn *conn)
{
    struct net_buf *buf;

    BT_DBG("%s, conn %p", __func__, conn);

    if (conn->state == BT_CONN_DISCONNECTED &&
        atomic_test_and_clear_bit(conn->flags, BT_CONN_CLEANUP)) {
        BT_DBG("handle %u disconnected - cleaning up", conn->handle);
        conn_cleanup(conn);
        return;
    }

    if (conn->tx) {
        return;
    }

    /* Get next ACL packet for connection */
    buf = net_buf_get(&conn->tx_queue, K_NO_WAIT);
    if (buf && (!send_buf(conn, buf))) {
        net_buf_unref(buf);
    }
}

struct bt_conn *bt_conn_add_le(const bt_addr_le_t *peer)
{
    struct bt_conn *conn = conn_new();

    if (!conn) {
        return NULL;
    }

    bt_addr_le_copy(&conn->le.dst, peer);
#if defined(CONFIG_BT_SMP)
    conn->sec_level = BT_SECURITY_LOW;
    conn->required_sec_level = BT_SECURITY_LOW;
#endif /* CONFIG_BT_SMP */
    conn->type = BT_CONN_TYPE_LE;
    conn->le.interval_min = BT_GAP_INIT_CONN_INT_MIN;
    conn->le.interval_max = BT_GAP_INIT_CONN_INT_MAX;

    k_delayed_work_init(&conn->le.update_work, le_conn_update);

    return conn;
}

static void process_unack_tx(struct bt_conn *conn)
{
    /* Return any unacknowledged packets */
    while (1) {
        sys_snode_t *node;
        unsigned int key;

        key  = irq_lock();
        node = sys_slist_get(&conn->tx_pending);
        irq_unlock(key);

        if (!node) {
            break;
        }

        tx_free(CONTAINER_OF(node, struct bt_conn_tx, node));
        bt_conn_notify_tx_done(conn);
    }
}

void bt_conn_set_state(struct bt_conn *conn, bt_conn_state_t state)
{
    bt_conn_state_t old_state;

    BT_DBG("%s, %s -> %s", __func__, state2str(conn->state), state2str(state));

    if (conn->state == state) {
        BT_WARN("no transition");
        return;
    }

    old_state = conn->state;
    conn->state = state;

    /* Actions needed for exiting the old state */
    switch (old_state) {
        case BT_CONN_DISCONNECTED:
            /* Take a reference for the first state transition after
             * bt_conn_add_le() and keep it until reaching DISCONNECTED
             * again.
             */
            bt_conn_ref(conn);
            break;
        case BT_CONN_CONNECT:
            if (IS_ENABLED(CONFIG_BT_CENTRAL) &&
                conn->type == BT_CONN_TYPE_LE) {
                k_delayed_work_cancel(&conn->le.update_work);
            }
            break;
        default:
            break;
    }

    /* Actions needed for entering the new state */
    switch (conn->state) {
        case BT_CONN_CONNECTED:
            if (conn->type == BT_CONN_TYPE_SCO) {
                /* TODO: Notify sco connected */
                break;
            }
            k_fifo_init(&conn->tx_queue);
            k_fifo_init(&conn->tx_notify);

            sys_slist_init(&conn->channels);

            bt_l2cap_connected(conn);
            notify_connected(conn);
            break;
        case BT_CONN_DISCONNECTED:
            if (conn->type == BT_CONN_TYPE_SCO) {
                /* TODO: Notify sco disconnected */
                bt_conn_unref(conn);
                break;
            }
            /* Notify disconnection and queue a dummy buffer to wake
             * up and stop the tx thread for states where it was
             * running.
             */
            if (old_state == BT_CONN_CONNECTED ||
                old_state == BT_CONN_DISCONNECT) {
                bt_l2cap_disconnected(conn);
                notify_disconnected(conn);
                process_unack_tx(conn);

                /* Cancel Connection Update if it is pending */
                if (conn->type == BT_CONN_TYPE_LE) {
                    k_delayed_work_cancel(&conn->le.update_work);
                }

                atomic_set_bit(conn->flags, BT_CONN_CLEANUP);
                /* The last ref will be dropped by the tx_thread */
            } else if (old_state == BT_CONN_CONNECT) {
                /* conn->err will be set in this case */
                notify_connected(conn);
                bt_conn_unref(conn);
            } else if (old_state == BT_CONN_CONNECT_SCAN) {
                /* this indicate LE Create Connection failed */
                if (conn->err) {
                    notify_connected(conn);
                }

                bt_conn_unref(conn);
            }

            break;
        case BT_CONN_CONNECT_SCAN:
            break;
        case BT_CONN_CONNECT:
            if (conn->type == BT_CONN_TYPE_SCO) {
                break;
            }
            /*
             * Timer is needed only for LE. For other link types controller
             * will handle connection timeout.
             */
            if (IS_ENABLED(CONFIG_BT_CENTRAL) &&
                conn->type == BT_CONN_TYPE_LE) {
                k_delayed_work_submit(&conn->le.update_work, CONN_TIMEOUT);
            }

            break;
        case BT_CONN_DISCONNECT:
            break;
        default:
            BT_WARN("no valid (%u) state was set", state);

            break;
    }
}

struct bt_conn *bt_conn_lookup_handle(u16_t handle)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        if (!atomic_get(&conns[i].ref)) {
            continue;
        }

        /* We only care about connections with a valid handle */
        if (conns[i].state != BT_CONN_CONNECTED &&
            conns[i].state != BT_CONN_DISCONNECT) {
            continue;
        }

        if (conns[i].handle == handle) {
            return bt_conn_ref(&conns[i]);
        }
    }

    return NULL;
}

int bt_conn_addr_le_cmp(const struct bt_conn *conn, const bt_addr_le_t *peer)
{
    /* Check against conn dst address as it may be the identity address */
    if (!bt_addr_le_cmp(peer, &conn->le.dst)) {
        return 0;
    }

    /* Check against initial connection address */
    if (conn->role == BT_HCI_ROLE_MASTER) {
        return bt_addr_le_cmp(peer, &conn->le.resp_addr);
    }

    return bt_addr_le_cmp(peer, &conn->le.init_addr);
}

struct bt_conn *bt_conn_lookup_addr_le(const bt_addr_le_t *peer)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        if (!atomic_get(&conns[i].ref)) {
            continue;
        }

        if (conns[i].type != BT_CONN_TYPE_LE) {
            continue;
        }

        if (!bt_conn_addr_le_cmp(&conns[i], peer)) {
            return bt_conn_ref(&conns[i]);
        }
    }

    return NULL;
}

struct bt_conn *bt_conn_lookup_state_le(const bt_addr_le_t *  peer,
                                        const bt_conn_state_t state)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        if (!atomic_get(&conns[i].ref)) {
            continue;
        }

        if (conns[i].type != BT_CONN_TYPE_LE) {
            continue;
        }

        if (peer && bt_conn_addr_le_cmp(&conns[i], peer)) {
            continue;
        }

        if (conns[i].state == state) {
            return bt_conn_ref(&conns[i]);
        }
    }

    return NULL;
}

void bt_conn_disconnect_all(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(conns); i++) {
        struct bt_conn *conn = &conns[i];

        if (!atomic_get(&conn->ref)) {
            continue;
        }

        if (conn->state == BT_CONN_CONNECTED) {
            bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        }
    }
}

struct bt_conn *bt_conn_ref(struct bt_conn *conn)
{
    atomic_inc(&conn->ref);

    BT_DBG("%s, handle %u ref %u", __func__, conn->handle, atomic_get(&conn->ref));

    return conn;
}

void bt_conn_unref(struct bt_conn *conn)
{
    atomic_dec(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, atomic_get(&conn->ref));
}

const bt_addr_le_t *bt_conn_get_dst(const struct bt_conn *conn)
{
    return &conn->le.dst;
}

int bt_conn_get_info(const struct bt_conn *conn, struct bt_conn_info *info)
{
    info->type = conn->type;
    info->role = conn->role;

    switch (conn->type) {
        case BT_CONN_TYPE_LE:
            if (conn->role == BT_HCI_ROLE_MASTER) {
                info->le.src = &conn->le.init_addr;
                info->le.dst = &conn->le.resp_addr;
            } else {
                info->le.src = &conn->le.resp_addr;
                info->le.dst = &conn->le.init_addr;
            }
            info->le.interval = conn->le.interval;
            info->le.latency  = conn->le.latency;
            info->le.timeout  = conn->le.timeout;
            return 0;
    }

    return -EINVAL;
}

static int bt_hci_disconnect(struct bt_conn *conn, u8_t reason)
{
    struct net_buf *             buf;
    struct bt_hci_cp_disconnect *disconn;
    int                          err;

    buf = bt_hci_cmd_create(BT_HCI_OP_DISCONNECT, sizeof(*disconn));
    if (!buf) {
        return -ENOBUFS;
    }

    disconn = net_buf_add(buf, sizeof(*disconn));
    disconn->handle = sys_cpu_to_le16(conn->handle);
    disconn->reason = reason;

    err = bt_hci_cmd_send(BT_HCI_OP_DISCONNECT, buf);
    if (err) {
        return err;
    }

    bt_conn_set_state(conn, BT_CONN_DISCONNECT);

    return 0;
}

int bt_conn_le_param_update(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    BT_DBG("conn %p features 0x%02x params (%d-%d %d %d)", conn,
           conn->le.features[0], param->interval_min, param->interval_max,
           param->latency, param->timeout);

#ifdef CONFIG_BLE_LINK_PARAMETERS
    if(param->timeout > SUP_TO_LIMIT){
       param->timeout = CONN_SUP_TIMEOUT;
       param->interval_min = CONN_INTERVAL_MIN;
       param->interval_max = CONN_INTERVAL_MAX;
    }
#endif

    /* Check if there's a need to update conn params */
    if (conn->le.interval >= param->interval_min &&
        conn->le.interval <= param->interval_max &&
        conn->le.latency == param->latency &&
        conn->le.timeout == param->timeout) {
        return -EALREADY;
    }

    /* Cancel any pending update */
    k_delayed_work_cancel(&conn->le.update_work);

    /* Use LE connection parameter request if both local and remote support
     * it; or if local role is master then use LE connection update.
     */
    if ((BT_FEAT_LE_CONN_PARAM_REQ_PROC(bt_dev.le.features) &&
         BT_FEAT_LE_CONN_PARAM_REQ_PROC(conn->le.features)) ||
        (conn->role == BT_HCI_ROLE_MASTER)) {
        return bt_conn_le_conn_update(conn, param);
    }

    /* If remote master does not support LL Connection Parameters Request
     * Procedure
     */
    return bt_l2cap_update_conn_param(conn, param);
}

int bt_conn_disconnect(struct bt_conn *conn, u8_t reason)
{
    /* Disconnection is initiated by us, so auto connection shall
     * be disabled. Otherwise the passive scan would be enabled
     * and we could send LE Create Connection as soon as the remote
     * starts advertising.
     */
    if (IS_ENABLED(CONFIG_BT_CENTRAL) && conn->type == BT_CONN_TYPE_LE) {
        bt_le_set_auto_conn(&conn->le.dst, NULL);
    }

    switch (conn->state) {
        case BT_CONN_CONNECT_SCAN:
            conn->err = reason;
            bt_conn_set_state(conn, BT_CONN_DISCONNECTED);
            bt_le_scan_update(false);
            return 0;
        case BT_CONN_CONNECT:
            if (IS_ENABLED(CONFIG_BT_CENTRAL)) {
                k_delayed_work_cancel(&conn->le.update_work);
                return bt_hci_cmd_send(BT_HCI_OP_LE_CREATE_CONN_CANCEL, NULL);
            }
            return 0;
        case BT_CONN_CONNECTED:
            return bt_hci_disconnect(conn, reason);
        case BT_CONN_DISCONNECT:
            return 0;
        case BT_CONN_DISCONNECTED:
        default:
            return -ENOTCONN;
    }
}

#if defined(CONFIG_BT_CENTRAL)
static void bt_conn_set_param_le(struct bt_conn *conn, const struct bt_le_conn_param *param)
{
    conn->le.interval_max = param->interval_max;
    conn->le.latency = param->latency;
    conn->le.timeout = param->timeout;
}

struct bt_conn *bt_conn_create_le(const bt_addr_le_t *peer, const struct bt_le_conn_param *param)
{
    struct bt_conn *conn;

    if (!bt_le_conn_params_valid(param)) {
        return NULL;
    }

    if (atomic_test_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN)) {
        return NULL;
    }

    conn = bt_conn_lookup_addr_le(peer);
    if (conn) {
        switch (conn->state) {
            case BT_CONN_CONNECT_SCAN:
                bt_conn_set_param_le(conn, param);
                return conn;
            case BT_CONN_CONNECT:
            case BT_CONN_CONNECTED:
                return conn;
            default:
                bt_conn_unref(conn);
                return NULL;
        }
    }

    conn = bt_conn_add_le(peer);
    if (!conn) {
        return NULL;
    }

    /* Set initial address - will be updated later if necessary. */
    bt_addr_le_copy(&conn->le.resp_addr, peer);

    bt_conn_set_param_le(conn, param);

    bt_conn_set_state(conn, BT_CONN_CONNECT_SCAN);

    bt_le_scan_update(true);

    return conn;
}

int bt_le_set_auto_conn(bt_addr_le_t *addr, const struct bt_le_conn_param *param)
{
    struct bt_conn *conn;

    if (param && !bt_le_conn_params_valid(param)) {
        return -EINVAL;
    }

    conn = bt_conn_lookup_addr_le(addr);
    if (!conn) {
        conn = bt_conn_add_le(addr);
        if (!conn) {
            return -ENOMEM;
        }
    }

    if (param) {
        bt_conn_set_param_le(conn, param);

        if (!atomic_test_and_set_bit(conn->flags, BT_CONN_AUTO_CONNECT)) {
            bt_conn_ref(conn);
        }
    } else {
        if (atomic_test_and_clear_bit(conn->flags, BT_CONN_AUTO_CONNECT)) {
            bt_conn_unref(conn);
            if (conn->state == BT_CONN_CONNECT_SCAN) {
                bt_conn_set_state(conn, BT_CONN_DISCONNECTED);
            }
        }
    }

    if (conn->state == BT_CONN_DISCONNECTED &&
        atomic_test_bit(bt_dev.flags, BT_DEV_READY)) {
        if (param) {
            bt_conn_set_state(conn, BT_CONN_CONNECT_SCAN);
        }
        bt_le_scan_update(false);
    }

    bt_conn_unref(conn);

    return 0;
}
#endif /* CONFIG_BT_CENTRAL */

#if defined(CONFIG_BT_PERIPHERAL)
struct bt_conn *bt_conn_create_slave_le(const bt_addr_le_t *          peer,
                                        const struct bt_le_adv_param *param)
{
    return NULL;
}
#endif /* CONFIG_BT_PERIPHERAL */

int bt_conn_le_conn_update(struct bt_conn *conn, const struct bt_le_conn_param *param)
{
    struct hci_cp_le_conn_update *conn_update;
    struct net_buf *buf;

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CONN_UPDATE, sizeof(*conn_update));
    if (!buf) {
        return -ENOBUFS;
    }

    conn_update = net_buf_add(buf, sizeof(*conn_update));
    memset(conn_update, 0, sizeof(*conn_update));
    conn_update->handle              = sys_cpu_to_le16(conn->handle);
    conn_update->conn_interval_min   = sys_cpu_to_le16(param->interval_min);
    conn_update->conn_interval_max   = sys_cpu_to_le16(param->interval_max);
    conn_update->conn_latency        = sys_cpu_to_le16(param->latency);
    conn_update->supervision_timeout = sys_cpu_to_le16(param->timeout);

    return bt_hci_cmd_send(BT_HCI_OP_LE_CONN_UPDATE, buf);
}

struct net_buf *bt_conn_create_pdu(struct net_buf_pool *pool, size_t reserve)
{
    struct net_buf *buf = NULL;

    if (!pool) {
        pool = &acl_tx_pool;
    }

    buf = net_buf_alloc(pool, K_FOREVER);
    if (buf) {
        reserve += sizeof(struct bt_hci_acl_hdr) + CONFIG_BT_HCI_RESERVE;
        net_buf_reserve(buf, reserve);
    }

    return buf;
}

#if defined(CONFIG_BT_SMP)
int bt_conn_auth_cb_register(const struct bt_conn_auth_cb *cb)
{
    if (!cb) {
        bt_auth = NULL;
        return 0;
    }

    /* cancel callback should always be provided */
    if (!cb->cancel) {
        return -EINVAL;
    }

    if (bt_auth) {
        return -EALREADY;
    }

    bt_auth = cb;
    return 0;
}

int bt_conn_auth_passkey_entry(struct bt_conn *conn, unsigned int passkey)
{
    if (!bt_auth) {
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BT_SMP) && conn->type == BT_CONN_TYPE_LE) {
        bt_smp_auth_passkey_entry(conn, passkey);
        return 0;
    }

    return -EINVAL;
}

int bt_conn_auth_passkey_confirm(struct bt_conn *conn)
{
    if (!bt_auth) {
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BT_SMP) && conn->type == BT_CONN_TYPE_LE) {
        return bt_smp_auth_passkey_confirm(conn);
    }

    return -EINVAL;
}

int bt_conn_auth_cancel(struct bt_conn *conn)
{
    if (!bt_auth) {
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BT_SMP) && conn->type == BT_CONN_TYPE_LE) {
        return bt_smp_auth_cancel(conn);
    }

    return -EINVAL;
}

int bt_conn_auth_pairing_confirm(struct bt_conn *conn)
{
    if (!bt_auth) {
        return -EINVAL;
    }

    switch (conn->type) {
#if defined(CONFIG_BT_SMP)
        case BT_CONN_TYPE_LE:
            return bt_smp_auth_pairing_confirm(conn);
#endif /* CONFIG_BT_SMP */
        default:
            return -EINVAL;
    }
}
#endif /* CONFIG_BT_SMP */

u8_t bt_conn_get_id(struct bt_conn *conn)
{
    return conn - conns;
}

struct bt_conn *bt_conn_lookup_id(u8_t id)
{
    struct bt_conn *conn;

    if (id >= ARRAY_SIZE(conns)) {
        return NULL;
    }

    conn = &conns[id];

    if (!atomic_get(&conn->ref)) {
        return NULL;
    }

    return bt_conn_ref(conn);
}

int bt_conn_init(void)
{
    int err, i;

    k_lifo_init(&acl_tx_pool.free);

    for (i = 0; i < ARRAY_SIZE(conn_tx); i++) {
        sys_slist_prepend(&free_tx, &conn_tx[i].node);
    }

    bt_att_init();

    err = bt_smp_init();
    if (err) {
        return err;
    }

    bt_l2cap_init();

    /* Initialize background scan */
    if (IS_ENABLED(CONFIG_BT_CENTRAL)) {
        for (i = 0; i < ARRAY_SIZE(conns); i++) {
            struct bt_conn *conn = &conns[i];

            if (!atomic_get(&conn->ref)) {
                continue;
            }

            if (atomic_test_bit(conn->flags, BT_CONN_AUTO_CONNECT)) {
                bt_conn_set_state(conn, BT_CONN_CONNECT_SCAN);
            }
        }
    }

    return 0;
}
