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
//#include <hci.h>
//#include <conn.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_ADV)
#include "common/log.h"

//#include "hci_core.h"

#include "mesh_def.h"
#include "adv.h"
#include "foundation.h"
#include "../inc/net.h"
#include "beacon.h"
#include "prov.h"
#include "proxy.h"
#include <port/mesh_hal_ble.h>
#include "bt_mesh_custom_log.h"

/* Window and Interval are equal for continuous scanning */
#ifdef ADV_ONE_SHOT  
#define MESH_PROV_SCAN_INTERVAL 0x30
#define MESH_PROV_SCAN_WINDOW   0x20

#define MESH_CTRL_SCAN_INTERVAL 0x60
#define MESH_CTRL_SCAN_WINDOW   0x40

#define MESH_IDLE_SCAN_INTERVAL 0x50
#define MESH_IDLE_SCAN_WINDOW   0x20

#define MESH_A2DP_SCAN_INTERVAL 0x90
#define MESH_A2DP_SCAN_WINDOW   0x20

#define ALI_MESH_SCAN_INTERVAL 0x30
#define ALI_MESH_SCAN_WINDOW   0x20
#else
#define MESH_AGGRESSIVE_SCAN_INTERVAL 0x10
#define MESH_AGGRESSIVE_SCAN_WINDOW   0x10

#define MESH_IDLE_SCAN_INTERVAL 0x90
#define MESH_IDLE_SCAN_WINDOW   0x20

#define MESH_A2DP_SCAN_INTERVAL 0x90
#define MESH_A2DP_SCAN_WINDOW   0x10
#endif
/* Convert from ms to 0.625ms units */
#define ADV_INT(_ms) ((_ms) * 8 / 5)

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#define ADV_INT_DEFAULT  K_MSEC(100)
#define ADV_INT_FAST     K_MSEC(20)

#define MIN(a, b)       ((a)<(b) ? (a) : (b))

/* TinyCrypt PRNG consumes a lot of stack space, so we need to have
 * an increased call stack whenever it's used.
 */
#if defined(CONFIG_BT_TINYCRYPT_ECC)
#define ADV_STACK_SIZE 768
#else
#define ADV_STACK_SIZE 512
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

static u8_t adv_scan_en;
static u16_t ble_scan_interval = MESH_IDLE_SCAN_INTERVAL, ble_scan_window = MESH_IDLE_SCAN_WINDOW;
static bool meshSendLowPowerPacketFlag = false;
static bool g_bt_mesh_scan_a2dp_state = 0;

int bt_mesh_schedule_message_send(int msg_id, void *msg);


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

static inline void adv_send(struct net_buf *buf)
{
    const struct bt_mesh_send_cb *cb = BT_MESH_ADV(buf)->cb;
    void *cb_data = BT_MESH_ADV(buf)->cb_data;
    u16_t duration, adv_int, adv_cnt;
    struct bt_mesh_data ad;
    struct bt_mesh_le_adv_param param;
    int err = 0;

    /* adv data */
    ad.type = adv_type[BT_MESH_ADV(buf)->type];
    ad.data_len = buf->len;
    ad.data = buf->data;

    /* one-shot interval & duration */
    if(BT_MESH_ADV(buf)->adv_int > 160) {
#ifdef ADV_ONE_SHOT
        meshSendLowPowerPacketFlag = true;
        adv_int = 10;
        adv_cnt = 130;
        duration = adv_int * adv_cnt;
        param.options = 0;
        param.interval_min = ADV_INT(adv_int);
        param.interval_max = param.interval_min;
        param.own_addr = NULL;

        BT_DBG("adv_send: 1.2s activation adv start, adv_int %d", ADV_INT(adv_int));
        adv_send_start(duration, err, cb, cb_data);
        bt_mesh_scan_disable();
        err = bt_mesh_adv_start(&param, &ad, 1, NULL, 0);
        if (err) {
            BT_ERR("adv_send: 1.2 adv start failed: err %d", err);
            goto exit;
        }
        k_sleep(duration);
        meshSendLowPowerPacketFlag = false;
        bt_mesh_adv_stop();
        bt_mesh_scan_enable_setting(MESH_IDLE_SCAN_WINDOW, MESH_IDLE_SCAN_INTERVAL);
#else
        adv_int = 10;
        adv_cnt = 130;
        duration = adv_int * adv_cnt;
        param.options = 0;
        param.interval_min = ADV_INT(adv_int);
        param.interval_max = param.interval_min;
        param.own_addr = NULL;
        
        BT_ERR("adv_send: 1.2s activation adv start, adv_int %d", ADV_INT(adv_int));
        adv_send_start(duration, err, cb, cb_data);
//        bt_mesh_scan_disable();
        err = bt_mesh_adv_start(&param, &ad, 1, NULL, 0);
        if (err) {
            BT_ERR("adv_send: 1.2 adv start failed: err %d", err);
            goto exit;
        }
        k_sleep(duration);
        bt_mesh_adv_stop();
//        bt_mesh_scan_enable_setting(MESH_IDLE_SCAN_WINDOW, MESH_IDLE_SCAN_INTERVAL);
#endif
        BT_DBG("adv_send: 1.2s adv stop\n");   
    } else {
#ifdef ADV_ONE_SHOT
        /* different scan params per prov & access msg */
        if(ad.type == BT_MESH_DATA_MESH_PROV) {
            ble_scan_interval = MESH_PROV_SCAN_INTERVAL;
            ble_scan_window = MESH_PROV_SCAN_WINDOW;
        } else if(ad.type == BT_MESH_DATA_MESH_MESSAGE) {
            ble_scan_interval = MESH_CTRL_SCAN_INTERVAL;
            ble_scan_window = MESH_CTRL_SCAN_WINDOW;
        } else {
            ble_scan_interval = MESH_IDLE_SCAN_INTERVAL;
            ble_scan_window = MESH_IDLE_SCAN_WINDOW;
        }
#endif
        /* thread sleep (ble_scan_interval-10) at least */
        adv_int = max(10, BT_MESH_ADV(buf)->adv_int);
        adv_cnt = BT_MESH_ADV(buf)->count + 1;
        duration = adv_int * adv_cnt;

        BT_DBG("adv_send: len %u: %s\n", buf->len, bt_hex(buf->data, buf->len));
        BT_DBG("adv_send: int %d, cnt %d, duration %d\n", adv_int, adv_cnt, duration);
        adv_send_start(duration, err, cb, cb_data);

#ifdef ADV_ONE_SHOT   
        /* for one shot adv */
        /* normal: disable scan -> set adv data-> one shot adv ->set scan param-> enable scan */
        err = bt_mesh_set_oneshot_data(&ad, 1, NULL, 0);      // one-shotssss
        for(int i=0; i< adv_cnt; i++) {
            BT_DBG("adv_send: one-shot idx[%d] \n", i);
            bt_mesh_scan_disable();
            bt_mesh_send_adv_oneshot(); // oneshot idx 1
            bt_mesh_send_adv_oneshot(); // oneshot idx 2
            bt_mesh_scan_enable_setting(ble_scan_window, ble_scan_interval);
            k_sleep(adv_int);
        }
#else
        param.options = 0;
        param.interval_min = ADV_INT(adv_int);
        param.interval_max = param.interval_min;
        param.own_addr = NULL;
        bt_mesh_adv_start(&param, &ad, 1, NULL, 0);
        k_sleep(duration);
        bt_mesh_adv_stop();
        k_sleep(20);
#endif

#ifdef ADV_ONE_SHOT 
        if (ad.type != BT_MESH_DATA_MESH_PROV) {
            if (bt_mesh_get_aggressiveSetting() == 0) {
                bt_mesh_scan_disable();
                bt_mesh_scan_enable_setting(MESH_IDLE_SCAN_WINDOW, MESH_IDLE_SCAN_INTERVAL);
            }
        }
#endif
    }
exit:
    adv_send_end(err, cb, cb_data);
    net_buf_unref(buf);
    BT_DBG("Advertising stopped");
}

static void adv_thread(void *p1, void *p2, void *p3)
{
    BT_DBG("started");
    uint32_t time_start;

    while (1) {
        struct net_buf *buf = NULL;
        if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
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
                        k_sleep(100);
                    }
                }
                bt_mesh_proxy_adv_stop();
            }
        } else {
            while (buf == NULL) {
                buf = net_buf_get(&adv_queue, K_FOREVER);

                if (buf) {
                    break;
                } else {
                    k_sleep(20);
//                    BT_DBG("%s timeout", __func__);
                }
            }
        }

        if (!buf) {
            continue;
        }

        /* busy == 0 means this was canceled */
        if (BT_MESH_ADV(buf)->busy) {
            BT_MESH_ADV(buf)->busy = 0;
            adv_send(buf);
        } else {
             // unref it even if it is canceled.
            net_buf_unref(buf);
        }

//        STACK_ANALYZE("adv stack", adv_thread_stack);

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
                                             u8_t xmit_count, u16_t xmit_int,
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
                                   u16_t xmit_int, s32_t timeout)
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

    if (buf->frags) {
        BT_DBG("FRAG %x, %x", buf, buf->frags);
    }

    net_buf_put(&adv_queue, net_buf_ref(buf));
    BT_DBG("bt_mesh_adv_send end");
}

const bt_addr_le_t *bt_mesh_pba_get_addr(void)
{
    return dev_addr;
}

static void bt_mesh_scan_cb(const bt_mesh_addr_le_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
//    BT_DBG("bt_mesh_scan_cb type:%d, rssi:%d", adv_type, rssi);

    if (adv_type != BT_MESH_LE_ADV_NONCONN_IND && adv_type != BT_MESH_LE_ADV_IND) {
        return;
    }

//    BT_DBG("bt_mesh_scan_cb:len %u: data: %s", buf->len, bt_hex(buf->data, buf->len));

    dev_addr = (const bt_addr_le_t *)addr;

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
        if (adv_type == BT_MESH_LE_ADV_NONCONN_IND) {
            switch (type) {
                case BT_MESH_DATA_MESH_MESSAGE:
                    BT_DBG("bt_mesh_data:len %u: data: %s", buf->len, bt_hex(buf->data, buf->len));
                    bt_mesh_net_recv(buf, rssi, BT_MESH_NET_IF_ADV);
                    break;
    #if defined(CONFIG_BT_MESH_PB_ADV)
                case BT_MESH_DATA_MESH_PROV:    
                    BT_DBG("bt_mesh_prov:len %u: data: %s", buf->len, bt_hex(buf->data, buf->len));
                    bt_mesh_pb_adv_recv(buf);
                    break;
    #endif
                case BT_MESH_DATA_MESH_BEACON:
                    bt_mesh_beacon_recv(buf);
                    break;
                default:
                    break;
            }
        } else if (adv_type == BT_MESH_LE_ADV_IND) {
            switch (type) {
                default:
                    break;
            }
        }

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }
}

void bt_mesh_adv_init(void)
{
    BT_INFO("%s-->\n", __func__);
    k_fifo_init(&adv_queue);
    BT_INFO("k_fifo_init adv_queue done\n", __func__);
    k_lifo_init(&adv_buf_pool.free);
    BT_INFO("k_fifo_init adv_buf_pool done\n", __func__);
    k_thread_create(&adv_thread_data, adv_thread_stack,
                    K_THREAD_STACK_SIZEOF(adv_thread_stack), adv_thread,
                    NULL, NULL, NULL, CONFIG_BT_MESH_ADV_PRIO, 0, K_NO_WAIT);
    BT_INFO("k_thread_create done\n", __func__);
}

void bt_mesh_scan_cb_result(const bt_mesh_addr_le_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
    struct scan_result_msg *msg;
    int ret, msg_id;

    msg = (struct scan_result_msg *)k_malloc(sizeof(struct scan_result_msg));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return;
    }

    msg->buf.data = msg->adv_buf;

    memcpy(&msg->addr, addr, sizeof(bt_mesh_addr_le_t));
    msg->rssi = rssi;
    msg->adv_type = adv_type;
    msg->buf.size = buf->size;
    msg->buf.len = buf->len;
    memcpy(msg->buf.data, buf->data, MIN(32, buf->len));

    msg_id = MSG_ID(MSG_GROUP_ADV, MSG_ID_SCAN_RESULT);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }
}

int bt_mesh_schedule_adv_handle(int msg_id, void *pMsg)
{
    switch(msg_id) {
    case MSG_ID_SCAN_RESULT:
        {
            struct scan_result_msg *msg = (struct scan_result_msg *)pMsg;

            bt_mesh_scan_cb((const bt_mesh_addr_le_t *)&msg->addr, msg->rssi, msg->adv_type, &msg->buf);
        }
        break;
    default:
        BT_ERR("%s: unknow message %d", msg_id);
        break;
    }

    return 0;
}

u8_t bt_mesh_get_scan_en(void)
{
    return adv_scan_en;
}

void bt_mesh_set_scan_en(u8_t en)
{
    adv_scan_en = en;
}

int bt_mesh_scan_enable(void)
{
    struct bt_mesh_le_scan_param scan_param = {
        .type       = BT_MESH_HCI_LE_SCAN_PASSIVE,
        .filter_dup = BT_MESH_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval   = MESH_IDLE_SCAN_INTERVAL,
        .window     = MESH_IDLE_SCAN_WINDOW
    };

    BT_DBG("%s, interval=0x%x, window=0x%x", __func__, ble_scan_interval, ble_scan_window);

    bt_mesh_set_scan_en(1);
    return bt_mesh_scan_start(&scan_param, bt_mesh_scan_cb_result);
}

int bt_mesh_scan_enable_setting(u16_t window, u16_t interval)
{
    struct bt_mesh_le_scan_param scan_param = {
        .type       = BT_MESH_HCI_LE_SCAN_PASSIVE,
        .filter_dup = BT_MESH_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval   = interval,
        .window     = window
    };
    if(meshSendLowPowerPacketFlag == false) {
        BT_DBG("%s, interval=0x%x, window=0x%x", __func__, interval, window);
        bt_mesh_set_scan_en(1);
        return bt_mesh_scan_start(&scan_param, bt_mesh_scan_cb_result);
    } else {
        BT_DBG("%s, skip enable scan as during 1.2s activation period.", __func__);
        return 0;
    }
}

void bt_mesh_scan_agressive_setting(bool enable)
{
    u16_t window = MESH_IDLE_SCAN_WINDOW;
    u16_t interval = MESH_IDLE_SCAN_INTERVAL;

    if (enable) {
        window = MESH_AGGRESSIVE_SCAN_WINDOW;
        interval = MESH_AGGRESSIVE_SCAN_INTERVAL;
    }

    if (g_bt_mesh_scan_a2dp_state) {
        window = MESH_A2DP_SCAN_WINDOW;
        interval = MESH_A2DP_SCAN_INTERVAL;
    }

    if(bt_mesh_get_scan_en())
        bt_mesh_scan_disable();

    bt_mesh_scan_enable_setting(window, interval);
}

void bt_mesh_scan_a2dp_state_setting(bool state)
{
    g_bt_mesh_scan_a2dp_state = state;
    if (bt_mesh_get_aggressiveSetting() == 0) {
        u16_t window = MESH_IDLE_SCAN_WINDOW;
        u16_t interval = MESH_IDLE_SCAN_INTERVAL;
        if (state) {
            window = MESH_A2DP_SCAN_WINDOW;
            interval = MESH_A2DP_SCAN_INTERVAL;
        }
        if(bt_mesh_get_scan_en())
            bt_mesh_scan_disable();

        bt_mesh_scan_enable_setting(window, interval);
    }
}

int bt_mesh_scan_disable(void)
{
    BT_DBG("");

    bt_mesh_set_scan_en(0);
    return bt_mesh_scan_stop();
}
