/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <misc/stack.h>
#include <misc/util.h>

#include <net/buf.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_ADV)
#include "common/log.h"

#include "adv.h"
#include "foundation.h"
#include "net.h"
#include "beacon.h"
#include "prov.h"
#include "proxy.h"
#include <port/mesh_hal_ble.h>
#include "provisioner_prov.h"
//#include "bt_mesh_custom_log.h"

/* Window and Interval are equal for continuous scanning */
#define MESH_SCAN_INTERVAL 0x10
#define MESH_SCAN_WINDOW   0x10

/* Convert from ms to 0.625ms units */
#define ADV_INT(_ms) ((_ms) * 8 / 5)

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#define ADV_INT_DEFAULT  K_MSEC(100)
#define ADV_INT_FAST     K_MSEC(20)

/* TinyCrypt PRNG consumes a lot of stack space, so we need to have
 * an increased call stack whenever it's used.
 */
#if defined(CONFIG_BT_HOST_CRYPTO)
#define ADV_STACK_SIZE 768
#else
#define ADV_STACK_SIZE 200
#endif

static K_FIFO_DEFINE(adv_queue);
static struct k_thread adv_thread_data;
static BT_STACK_NOINIT(adv_thread_stack, ADV_STACK_SIZE);

static const u8_t adv_type[] = {
    [BT_MESH_ADV_PROV]   = BT_MESH_DATA_MESH_PROV,
    [BT_MESH_ADV_DATA]   = BT_MESH_DATA_MESH_MESSAGE,
    [BT_MESH_ADV_BEACON] = BT_MESH_DATA_MESH_BEACON,
};

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BT_MESH_ADV_BUF_COUNT,
                    BT_MESH_ADV_DATA_SIZE, BT_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BT_MESH_ADV_BUF_COUNT];

static const bt_addr_le_t *dev_addr;

static struct bt_mesh_adv *adv_alloc(int id)
{
    return &adv_pool[id];
}

static inline void adv_send_start(u16_t duration, int err,
                                  const struct bt_mesh_send_cb *cb,
                                  void *cb_data)
{
    if (cb && cb->start) {
        cb->start(duration, err, cb_data);
    }
}

static inline void adv_send_end(int err, const struct bt_mesh_send_cb *cb,
                                void *cb_data)
{
    if (cb && cb->end) {
        cb->end(err, cb_data);
    }
}

/* HCI version from Assigned Numbers */
#define BT_VERSION_1_0B                     0
#define BT_VERSION_1_1                      1
#define BT_VERSION_1_2                      2
#define BT_VERSION_2_0                      3
#define BT_VERSION_2_1                      4
#define BT_VERSION_3_0                      5
#define BT_VERSION_4_0                      6
#define BT_VERSION_4_1                      7
#define BT_VERSION_4_2                      8
#define BT_VERSION_5_0                      9

static inline void adv_send(struct net_buf *buf)
{
#if (BT_HCI_VERSION >= BT_VERSION_5_0)
    const s32_t adv_int_min = ADV_INT_FAST;
#else
    const s32_t adv_int_min = ADV_INT_DEFAULT;
#endif
    const struct bt_mesh_send_cb *cb = BT_MESH_ADV(buf)->cb;
    void *cb_data = BT_MESH_ADV(buf)->cb_data;
    struct bt_mesh_le_adv_param param;
    u16_t duration, adv_int;
    struct bt_mesh_data ad;
    int err;

    adv_int = max(adv_int_min, BT_MESH_ADV(buf)->adv_int);
    duration = (BT_MESH_ADV(buf)->count + 1) * (adv_int + 10);

    BT_DBG("type %u len %u: %s", BT_MESH_ADV(buf)->type,
           buf->len, bt_hex(buf->data, buf->len));
    BT_DBG("count %u interval %ums duration %ums",
           BT_MESH_ADV(buf)->count + 1, adv_int, duration);

    ad.type = adv_type[BT_MESH_ADV(buf)->type];
    ad.data_len = buf->len;
    ad.data = buf->data;

    param.options = 0;
    param.interval_min = ADV_INT(adv_int);
    param.interval_max = param.interval_min;
    param.own_addr = NULL;

    err = bt_mesh_adv_start(&param, &ad, 1, NULL, 0);
    net_buf_unref(buf);
    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        BT_ERR("Advertising failed: err %d", err);
        goto exit;
    }

    BT_DBG("Advertising started. Sleeping %u ms", duration);

    k_sleep(duration);

exit:
    err = bt_mesh_adv_stop();
    adv_send_end(err, cb, cb_data);
    if (err) {
        BT_ERR("Stopping advertising failed: err %d", err);
        return;
    }

    BT_DBG("Advertising stopped");
}

static void adv_thread(void *p1, void *p2, void *p3)
{
    BT_DBG("started");
    uint32_t time_start;

    while (1) {
        struct net_buf *buf;

        if (IS_ENABLED(CONFIG_BT_MESH_PROXY)) {
            buf = net_buf_get(&adv_queue, K_NO_WAIT);
            while (!buf) {
                s32_t timeout;

                timeout = bt_mesh_proxy_adv_start();
                time_start = k_uptime_get_32();
                BT_DBG("Proxy Advertising up to %d ms",
                       timeout);
                while (buf == NULL) {
                    buf = net_buf_get(&adv_queue, timeout);
                    if (buf || timeout == K_NO_WAIT ||
                        (timeout != -1 && (k_uptime_get_32() - time_start) >= timeout)) {
                        break;
                    } else {
                        k_sleep(1);
                    }
                }
                bt_mesh_proxy_adv_stop();
            }
        } else {
            buf = net_buf_get(&adv_queue, K_FOREVER);
        }

        if (!buf) {
            continue;
        }

        /* busy == 0 means this was canceled */
        if (BT_MESH_ADV(buf)->busy) {
            BT_MESH_ADV(buf)->busy = 0;
            adv_send(buf);
        }

        STACK_ANALYZE("adv stack", adv_thread_stack);

        /* Give other threads a chance to run */
        k_yield();
    }
}

void bt_mesh_adv_update(void)
{
    BT_DBG("");

    k_fifo_cancel_wait(&adv_queue);
}

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
                                             bt_mesh_adv_alloc_t get_id,
                                             enum bt_mesh_adv_type type,
                                             u8_t xmit_count, u8_t xmit_int,
                                             s32_t timeout)
{
    struct bt_mesh_adv *adv;
    struct net_buf *buf;

    buf = net_buf_alloc(pool, timeout);
    if (!buf) {
        return NULL;
    }

    adv = get_id(net_buf_id(buf));
    BT_MESH_ADV(buf) = adv;

    memset(adv, 0, sizeof(*adv));

    adv->type         = type;
    adv->count        = xmit_count;
    adv->adv_int      = xmit_int;

    return buf;
}

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit_count,
                                   u8_t xmit_int, s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&adv_buf_pool, adv_alloc, type,
                                        xmit_count, xmit_int, timeout);
}

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                      void *cb_data)
{
    BT_DBG("type 0x%02x len %u: %s", BT_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BT_MESH_ADV(buf)->cb = cb;
    BT_MESH_ADV(buf)->cb_data = cb_data;
    BT_MESH_ADV(buf)->busy = 1;

    net_buf_put(&adv_queue, net_buf_ref(buf));
}

const bt_addr_le_t *bt_mesh_pba_get_addr(void)
{
	return dev_addr;
}

static void bt_mesh_scan_cb(const bt_mesh_addr_le_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
#if CONFIG_BT_MESH_PROVISIONER
	u16_t uuid = 0;
#endif

    if (adv_type != BT_MESH_LE_ADV_NONCONN_IND && adv_type != BT_LE_ADV_IND) {
        return;
    }

    BT_DBG("len %u: %s", buf->len, bt_hex(buf->data, buf->len));

	dev_addr = addr;

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        u8_t len, type;

        len = net_buf_simple_pull_u8(buf);
        /* Check for early termination */
        if (len == 0) {
            return;
        }

        if (len > buf->len || buf->len < 1) {
            BT_WARN("AD malformed");
            return;
        }

        net_buf_simple_save(buf, &state);

        type = net_buf_simple_pull_u8(buf);

        buf->len = len - 1;

        switch (type) {
			if (adv_type == BT_LE_ADV_NONCONN_IND) {
            case BT_MESH_DATA_MESH_MESSAGE:
                bt_mesh_net_recv(buf, rssi, BT_MESH_NET_IF_ADV);
                break;
#if defined(CONFIG_BT_MESH_PB_ADV)
            case BT_MESH_DATA_MESH_PROV:
			#if CONFIG_BT_MESH_PROVISIONER
				if (bt_mesh_is_provisioner_en()) {
					provisioner_pb_adv_recv(buf);

					break;
				}
			#else
                bt_mesh_pb_adv_recv(buf);
			#endif
                break;
#endif
            case BT_MESH_DATA_MESH_BEACON:
			#if CONFIG_BT_MESH_PROVISIONER
				if (bt_mesh_is_provisioner_en()) {
					provisioner_beacon_recv(buf);
				}
			#else
                bt_mesh_beacon_recv(buf);
			#endif
                break;
        	} else if (adv_type == BT_LE_ADV_IND) {
#if CONFIG_BT_MESH_PROVISIONER && CONFIG_BT_MESH_PB_GATT
			case BT_DATA_FLAGS:
                if (bt_mesh_is_provisioner_en()) {
                    if (!provisioner_flags_match(buf)) {
                        BT_DBG("Flags mismatch, ignore this adv pkt");
                        return;
                    }
                }
				break;
			case BT_DATA_SERVICE_UUID:
                if (bt_mesh_is_provisioner_en()) {
                    uuid = provisioner_srv_uuid_recv(buf);
                    if (!uuid) {
                        BT_DBG("Service UUID mismatch, ignore this adv pkt");
                        return;
                    }
                }
				break;
			case BT_DATA_SERVICE_DATA:
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_srv_data_recv(buf, addr, uuid);
                }
				break;
#endif
			}
			default:
				break;
    	}

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }
}

void bt_mesh_adv_init(void)
{
    k_fifo_init(&adv_queue);
    k_lifo_init(&adv_buf_pool.free);
    k_thread_create(&adv_thread_data, adv_thread_stack,
                    K_THREAD_STACK_SIZEOF(adv_thread_stack), adv_thread,
                    NULL, NULL, NULL, CONFIG_BT_MESH_ADV_PRIO, 0, K_NO_WAIT);
}

int bt_mesh_scan_enable(void)
{
    struct bt_mesh_le_scan_param scan_param = {
        .type       = BT_MESH_HCI_LE_SCAN_PASSIVE,
        .filter_dup = BT_MESH_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval   = MESH_SCAN_INTERVAL,
        .window     = MESH_SCAN_WINDOW
    };

    BT_DBG("%s", __func__);

    return bt_mesh_scan_start(&scan_param, bt_mesh_scan_cb);
}

int bt_mesh_scan_disable(void)
{
    BT_DBG("");

    return bt_mesh_scan_stop();
}
