/*  Bluetooth Mesh */

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ble_os.h>
#include <bt_errno.h>
#include <misc/stack.h>
#include <misc/util.h>

#include <net/buf.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_ADV)
#include "common/log.h"

//  #include "host/hci_core.h"

#include "adv.h"
#include "net.h"
#include "foundation.h"
#include "beacon.h"
#include "prov.h"
#include "proxy.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_main.h"
#include "provisioner_prov.h"
#include "provisioner_beacon.h"
#endif

#ifdef GENIE_ULTRA_PROV
#include "genie_storage.h"
#include "genie_provision.h"
#endif

/* Convert from ms to 0.625ms units */
#define ADV_SCAN_UNIT(_ms) ((_ms) * 8 / 5)

/* Window and Interval are equal for continuous scanning */
#ifndef CONFIG_MESH_SCAN_INTERVAL_MS
#define CONFIG_MESH_SCAN_INTERVAL_MS 30
#endif
#ifndef CONFIG_MESH_SCAN_WINDOW_MS
#define CONFIG_MESH_SCAN_WINDOW_MS 30
#endif

#define MESH_SCAN_INTERVAL ADV_SCAN_UNIT(CONFIG_MESH_SCAN_INTERVAL_MS)
#define MESH_SCAN_WINDOW ADV_SCAN_UNIT(CONFIG_MESH_SCAN_WINDOW_MS)

#ifndef CONFIG_ADV_SCAN_INTERVAL_TIMER
#define CONFIG_ADV_SCAN_INTERVAL_TIMER (1)
#endif

#ifndef CONFIG_ADV_INTERVAL_TIMER
#define CONFIG_ADV_INTERVAL_TIMER (20)
#endif

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#define ADV_INT_DEFAULT_MS 100
#define ADV_INT_FAST_MS 20

#ifdef CONFIG_GENIE_MESH_ENABLE
#ifndef GENIE_DEFAULT_DURATION
#define GENIE_DEFAULT_DURATION 120
#endif
#endif

#ifndef CONFIG_BT_ADV_STACK_SIZE

#if defined(CONFIG_BT_HOST_CRYPTO)
#define ADV_STACK_SIZE 2048
#else
#define ADV_STACK_SIZE 1024
#endif
#else
#define ADV_STACK_SIZE CONFIG_BT_ADV_STACK_SIZE
#endif

// static K_FIFO_DEFINE(adv_queue);
static struct kfifo adv_queue;

static struct k_thread adv_thread_data;
static BT_STACK_NOINIT(adv_thread_stack, ADV_STACK_SIZE);

int bt_mesh_adv_scan_schd_init();

static const u8_t adv_type[] = {
    [BT_MESH_ADV_PROV] = BT_DATA_MESH_PROV,
    [BT_MESH_ADV_DATA] = BT_DATA_MESH_MESSAGE,
    [BT_MESH_ADV_BEACON] = BT_DATA_MESH_BEACON,
    [BT_MESH_ADV_URI] = BT_DATA_URI,
};

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BT_MESH_ADV_BUF_COUNT, BT_MESH_ADV_DATA_SIZE, BT_MESH_ADV_USER_DATA_SIZE,
                    NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BT_MESH_ADV_BUF_COUNT];

#ifdef CONFIG_BT_MESH_PROVISIONER
static const bt_addr_le_t *dev_addr;
#endif

static vendor_beacon_cb g_vendor_beacon_cb = NULL;

int bt_mesh_adv_scan_init(void);

static struct bt_mesh_adv *adv_alloc(int id)
{
    return &adv_pool[id];
}

static inline void adv_send_start(u16_t duration, int err, const struct bt_mesh_send_cb *cb, void *cb_data)
{
    if (cb && cb->start) {
        cb->start(duration, err, cb_data);
    }
}

static inline void adv_send_end(int err, const struct bt_mesh_send_cb *cb, void *cb_data)
{
    if (cb && cb->end) {
        cb->end(err, cb_data);
    }
}

static inline void adv_send(struct net_buf *buf)
{
    extern int bt_le_hci_version_get();
    const bt_s32_t adv_int_min =
        ((bt_le_hci_version_get() >= BT_HCI_VERSION_5_0) ? ADV_INT_FAST_MS : ADV_INT_DEFAULT_MS);
    const struct bt_mesh_send_cb *cb = BT_MESH_ADV(buf)->cb;
    void *cb_data = BT_MESH_ADV(buf)->cb_data;
    struct bt_le_adv_param param = { 0 };
    u16_t duration, adv_int;
    struct bt_data ad;
    int err;

    adv_int = MAX(adv_int_min, BT_MESH_TRANSMIT_INT(BT_MESH_ADV(buf)->xmit));
#ifdef CONFIG_GENIE_MESH_ENABLE
    duration = GENIE_DEFAULT_DURATION;
#else
    if (BT_MESH_TRANSMIT_COUNT(BT_MESH_ADV(buf)->xmit) == 0) {
        duration = adv_int;
    } else {
        duration = (((BT_MESH_TRANSMIT_COUNT(BT_MESH_ADV(buf)->xmit) + 1) * (adv_int)) + 10);
    }

#endif
    BT_DBG("type %u len %u: %s", BT_MESH_ADV(buf)->type, buf->len, bt_hex(buf->data, buf->len));
    BT_DBG("count %u interval %ums duration %ums", BT_MESH_TRANSMIT_COUNT(BT_MESH_ADV(buf)->xmit) + 1, adv_int,
           duration);

#ifdef GENIE_ULTRA_PROV
    if (BT_MESH_ADV(buf)->tiny_adv == 1) {
        ad.type = GENIE_PROV_ADV_TYPE;
    } else {
        ad.type = adv_type[BT_MESH_ADV(buf)->type];
    }
#else
    ad.type = adv_type[BT_MESH_ADV(buf)->type];
#endif

    ad.data_len = buf->len;
    ad.data = buf->data;

    if (IS_ENABLED(CONFIG_BT_MESH_DEBUG_USE_ID_ADDR)) {
        param.options = BT_LE_ADV_OPT_USE_IDENTITY;
    } else {
        param.options = 0;
    }
    param.id = BT_ID_DEFAULT;
    param.interval_min = ADV_SCAN_UNIT(adv_int);
    param.interval_max = param.interval_min;
    err = bt_mesh_adv_enable(&param, &ad, 1, NULL, 0);
    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        net_buf_unref(buf);
        BT_ERR("Advertising failed: err %d", err);
        return;
    }

    BT_DBG("Advertising started. Sleeping %u ms", duration);

    k_sleep(K_MSEC(duration));

    err = bt_mesh_adv_disable();
    net_buf_unref(buf);
    adv_send_end(err, cb, cb_data);
    if (err) {
        net_buf_unref(buf);
        BT_ERR("Stopping advertising failed: err %d", err);
        return;
    }

    BT_DBG("Advertising stopped");
}

// static void adv_stack_dump(const struct k_thread *thread, void *user_data)
// {
// #if defined(CONFIG_THREAD_STACK_INFO)
// stack_analyze((char *)user_data, (char *)thread->stack_info.start,
//         thread->stack_info.size);
// #endif
// }

static void adv_thread(void *args)
{
    BT_DBG("started");

    while (1) {
        struct net_buf *buf;

        if (IS_ENABLED(CONFIG_BT_MESH_PROXY)) {
            buf = net_buf_get(&adv_queue, K_NO_WAIT);
            while (!buf) {
                bt_s32_t timeout;

                timeout = bt_mesh_proxy_adv_start();
                BT_DBG("Proxy Advertising up to %d ms", timeout);
                buf = net_buf_get(&adv_queue, timeout);
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
            adv_send(buf);
            BT_MESH_ADV(buf)->busy = 0;
        } else {
            net_buf_unref(buf);
        }

        // STACK_ANALYZE("adv stack", adv_thread_stack);
        // k_thread_foreach(adv_stack_dump, "BT_MESH");

        /* Give other threads a chance to run */
        k_yield();
    }
}

void bt_mesh_adv_update(void)
{
    BT_DBG("");
    k_fifo_cancel_wait(&adv_queue);
}

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool, bt_mesh_adv_alloc_t get_id,
                                             enum bt_mesh_adv_type type, u8_t xmit, bt_s32_t timeout)
{
    struct bt_mesh_adv *adv;
    struct net_buf *buf;

    if (atomic_test_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
        /*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_GENIE_MESH_GLP
        BT_WARN("tx need resume stack while suspended");
        bt_mesh_resume();
#else
        BT_WARN("Refusing to allocate buffer while suspended");
        return NULL;
#endif
        /*[Genie end] add by wenbing.cwb at 2021-01-21*/
    }

    buf = net_buf_alloc(pool, timeout);
    if (!buf) {
        return NULL;
    }

    adv = get_id(net_buf_id(buf));
    BT_MESH_ADV(buf) = adv;

    memset(adv, 0, sizeof(*adv));

    adv->type = type;
    adv->xmit = xmit;

    return buf;
}

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit, bt_s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&adv_buf_pool, adv_alloc, type, xmit, timeout);
}

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb, void *cb_data)
{
    if (NULL == buf) {
        BT_WARN("buf is null");
        return;
    }

    BT_DBG("type 0x%02x len %u: %s", BT_MESH_ADV(buf)->type, buf->len, bt_hex(buf->data, buf->len));

    BT_MESH_ADV(buf)->cb = cb;
    BT_MESH_ADV(buf)->cb_data = cb_data;
    BT_MESH_ADV(buf)->busy = 1;

    net_buf_put(&adv_queue, net_buf_ref(buf));
}

#ifdef CONFIG_BT_MESH_PROVISIONER
const bt_addr_le_t *bt_mesh_pba_get_addr(void)
{
    return dev_addr;
}
#endif

static void bt_mesh_scan_cb(const bt_addr_le_t *addr, s8_t rssi, u8_t adv_type, struct net_buf_simple *buf)
{
#if defined(CONFIG_BT_MESH_PROVISIONER) || defined(GENIE_ULTRA_PROV)
    if (adv_type != BT_LE_ADV_NONCONN_IND && adv_type != BT_LE_ADV_IND) {
#else
    if (adv_type != BT_LE_ADV_NONCONN_IND) {
#endif
        return;
    }
    // BT_DBG("len %u: %s", buf->len, bt_hex(buf->data, buf->len));
#ifdef CONFIG_BT_MESH_PROVISIONER
    dev_addr = addr;
#endif

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        u8_t len, type;

        len = net_buf_simple_pull_u8(buf);
        /* Check for early termination */
        if (len == 0) {
            return;
        }

        if (len > buf->len) {
            // BT_WARN("AD malformed");
            return;
        }

        net_buf_simple_save(buf, &state);

        type = net_buf_simple_pull_u8(buf);

        buf->len = len - 1;
        if (adv_type == BT_LE_ADV_NONCONN_IND) {
            switch (type) {
            case BT_DATA_MESH_MESSAGE:
#if defined(CONFIG_BT_MESH_RELAY_SRC_DBG)
                net_buf_trace.buf = buf;
                memcpy(net_buf_trace.addr, addr->a.val, 6);
                net_buf_trace.addr[6] = addr->type;
#endif

                bt_mesh_net_recv(buf, rssi, BT_MESH_NET_IF_ADV);
                break;
#if defined(CONFIG_BT_MESH_PB_ADV)
            case BT_DATA_MESH_PROV:
#ifdef CONFIG_BT_MESH_PROVISIONER
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_pb_adv_recv(buf);
                    break;
                } else
#endif
                {
                    bt_mesh_pb_adv_recv(buf);
                }

                break;
#endif
            case BT_DATA_MESH_BEACON:
#ifdef CONFIG_BT_MESH_PROVISIONER
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_beacon_recv(buf);
                } else
#endif
                {
                    bt_mesh_beacon_recv(buf);
                }

                break;

            case BT_DATA_MANUFACTURER_DATA:
                if (g_vendor_beacon_cb != NULL) {
                    g_vendor_beacon_cb(addr, rssi, adv_type, (void *)buf, buf->len);
                }

                break;
            default:
                break;
            }
        } else if (adv_type == BT_LE_ADV_IND) {
            u16_t uuid = 0;
#ifdef GENIE_ULTRA_PROV
            genie_provision_ultra_prov_handle(type, (void *)buf);
#endif
            switch (type) {
#if defined(CONFIG_BT_MESH_PROVISIONER) && defined(CONFIG_BT_MESH_PB_GATT)
            case BT_DATA_FLAGS:
                if (bt_mesh_is_provisioner_en()) {
                    if (!provisioner_flags_match(buf)) {
                        BT_DBG("Flags mismatch, ignore this adv pkt");
                        return;
                    }
                }
                break;
            case BT_DATA_UUID16_ALL:
                if (bt_mesh_is_provisioner_en()) {
                    uuid = provisioner_srv_uuid_recv(buf);
                    if (!uuid) {
                        BT_DBG("Service UUID mismatch, ignore this adv pkt");
                        return;
                    }
                }
                break;
            case BT_DATA_SVC_DATA16:
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_srv_data_recv(buf, addr, uuid);
                }
                break;
#endif
            default:
                break;
            }
            (void)uuid;
        }

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }
}

void bt_mesh_adv_init(void)
{
    BT_INFO("enter %s\n", __func__);

    k_fifo_init(&adv_queue);

    NET_BUF_POOL_INIT(adv_buf_pool);

    bt_mesh_adv_scan_schd_init();

    k_thread_spawn(&adv_thread_data, "mesh adv", adv_thread_stack, K_THREAD_STACK_SIZEOF(adv_thread_stack), adv_thread,
                   NULL, 7);
}

extern int bt_le_adv_start_instant(const struct bt_le_adv_param *param, uint8_t *ad_data, size_t ad_len,
                                   uint8_t *sd_data, size_t sd_len);
extern int bt_le_adv_stop_instant(void);

#define SCHD_LOGD(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define SCHD_LOGE(...) LOGE("ADV", ##__VA_ARGS__)

#define CONN_ADV_DATA_TIEMOUT (10)
#define NOCONN_ADV_DATA_TIEMOUT (5)

typedef enum {
    SCHD_IDLE = 0,
    SCHD_ADV,
    SCHD_SCAN,
    SCHD_ADV_SCAN,

    SCHD_INVAILD,
} adv_scan_schd_state_en;

typedef enum {
    ADV_ON = 0,
    ADV_OFF,
    SCAN_ON,
    SCAN_OFF,

    ACTION_INVAILD,
} adv_scan_schd_action_en;

typedef int (*adv_scan_schd_func_t)(adv_scan_schd_state_en st);
static int adv_scan_schd_idle_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_idle_exit(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_exit(adv_scan_schd_state_en st);
static int adv_scan_schd_scan_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_scan_exit(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_scan_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_scan_exit(adv_scan_schd_state_en st);

struct {
    adv_scan_schd_func_t enter;
    adv_scan_schd_func_t exit;
} adv_scan_schd_funcs[] = {
    { adv_scan_schd_idle_enter, adv_scan_schd_idle_exit },
    { adv_scan_schd_adv_enter, adv_scan_schd_adv_exit },
    { adv_scan_schd_scan_enter, adv_scan_schd_scan_exit },
    { adv_scan_schd_adv_scan_enter, adv_scan_schd_adv_scan_exit },
};

adv_scan_schd_state_en adv_scan_schd_st_change_map[4][4] = {
    { SCHD_ADV, SCHD_IDLE, SCHD_SCAN, SCHD_IDLE },
    { SCHD_ADV, SCHD_IDLE, SCHD_ADV_SCAN, SCHD_ADV },
    { SCHD_ADV_SCAN, SCHD_SCAN, SCHD_SCAN, SCHD_IDLE },
    { SCHD_ADV_SCAN, SCHD_SCAN, SCHD_ADV_SCAN, SCHD_ADV },
};

struct adv_scan_data_t {
    uint8_t ad_data[31];
    size_t ad_len;
    uint8_t sd_data[31];
    size_t sd_len;
    struct bt_le_adv_param adv_param;
    struct bt_le_scan_param scan_param;
    bt_le_scan_cb_t *scan_cb;
};

#define FLAG_RESTART 1
#define FLAG_STOP 2

struct {
    struct k_mutex mutex;
    k_timer_t timer;
    uint8_t flag;
    adv_scan_schd_state_en cur_st;
    struct adv_scan_data_t param;
} adv_scan_schd = { 0 };

static int adv_scan_schd_idle_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("idle enter\n");
    memset(&adv_scan_schd.param, 0, sizeof(struct adv_scan_data_t));
    return 0;
}

static int adv_scan_schd_idle_exit(adv_scan_schd_state_en st)
{
    SCHD_LOGD("idle exit\n");
    //  do nothing
    return 0;
}

static int adv_scan_schd_adv_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("adv on enter\n");

    if (st == SCHD_IDLE || st == SCHD_ADV_SCAN || st == SCHD_ADV) {
        if (adv_scan_schd.param.ad_len) {
            adv_scan_schd.flag = FLAG_RESTART;
            k_timer_start(&adv_scan_schd.timer, 1);
            return 0;
        }
    }

    return -EINVAL;
    ;
}

static int adv_scan_schd_adv_exit(adv_scan_schd_state_en st)
{
    SCHD_LOGD("adv on exit\n");
    int ret = 0;
    if (st == SCHD_ADV_SCAN || st == SCHD_IDLE || st == SCHD_ADV) {
        adv_scan_schd.flag = FLAG_STOP;
        k_timer_stop(&adv_scan_schd.timer);
        ret = bt_le_adv_stop_instant();
        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("adv stop err %d\n", ret);
            return ret;
        }
        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_scan_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("scan on enter\n");
    int ret = 0;
    if (st == SCHD_IDLE || st == SCHD_ADV_SCAN || st == SCHD_SCAN) {
        ret = bt_le_scan_start(&adv_scan_schd.param.scan_param, adv_scan_schd.param.scan_cb);
        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("scan start err %d\n", ret);
            return ret;
        }
        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_scan_exit(adv_scan_schd_state_en st)
{
    SCHD_LOGD("scan on exit\n");
    int ret = 0;
    if (st == SCHD_ADV_SCAN || st == SCHD_IDLE || st == SCHD_SCAN) {
        ret = bt_le_scan_stop();
        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("scan stop err %d in %s\n", ret, __func__);
            return ret;
        }
        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_adv_scan_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("adv scan on enter\n");

    if (st == SCHD_ADV || st == SCHD_SCAN || st == SCHD_ADV_SCAN || st == SCHD_IDLE) {
        adv_scan_schd.flag = FLAG_RESTART;
        k_timer_start(&adv_scan_schd.timer, 1);
        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_adv_scan_exit(adv_scan_schd_state_en st)
{
    int ret;
    SCHD_LOGD("adv scan on exit\n");

    if (st == SCHD_ADV || st == SCHD_SCAN || st == SCHD_ADV_SCAN) {
        adv_scan_schd.flag = FLAG_STOP;
        k_timer_stop(&adv_scan_schd.timer);

        ret = bt_le_scan_stop();
        // SCHD_LOGE("EALREADY =  %d in %s\n", EALREADY, __func__);
        if (ret && (ret != -114)) {
            SCHD_LOGE("scan stop err %d in %s\n", ret, __func__);
            return ret;
        }

        ret = bt_le_adv_stop();

        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("adv stop err %d\n", ret);
            return ret;
        }

        return 0;
    }

    return -EINVAL;
}

int bt_mesh_adv_scan_schd(adv_scan_schd_state_en st)
{
    int ret;
    SCHD_LOGD("%d->%d\n", adv_scan_schd.cur_st, st);

    if (st < SCHD_INVAILD) {
        ret = adv_scan_schd_funcs[adv_scan_schd.cur_st].exit(st);

        if (ret) {
            return ret;
        }

        adv_scan_schd.cur_st = SCHD_IDLE;

        ret = adv_scan_schd_funcs[st].enter(adv_scan_schd.cur_st);

        if (ret) {
            return ret;
        }

        adv_scan_schd.cur_st = st;

        return 0;
    }

    return -EINVAL;
}

int bt_mesh_adv_scan_schd_action(adv_scan_schd_action_en action)
{
    int ret;

    if (action < ACTION_INVAILD) {
        adv_scan_schd_state_en cur_st = adv_scan_schd.cur_st;
        adv_scan_schd_state_en target_st = adv_scan_schd_st_change_map[cur_st][action];
        k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
        ret = bt_mesh_adv_scan_schd(target_st);
        k_mutex_unlock(&adv_scan_schd.mutex);
        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("action %d, cur_st %d target_st %d, ret %d in %s\n", action, cur_st, target_st, ret, __func__);
        }

        return ret;
    }

    return -EINVAL;
}

void adv_scan_timer(void *timer, void *arg)
{
    int ret;
    static enum {
        ADV = 0,
        SCAN,
        ADV_IDLE,
    } next_state = ADV;
    static int adv_time = 0;

    uint32_t next_time = 0;
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);

    if (adv_scan_schd.flag == FLAG_RESTART) {
        next_state = ADV;
        adv_scan_schd.flag = 0;
    } else if (adv_scan_schd.flag == FLAG_STOP) {
        k_mutex_unlock(&adv_scan_schd.mutex);
        return;
    }

    SCHD_LOGD("adv_time %d, next_state %d, flag %d\n", adv_time, next_state, adv_scan_schd.flag);

    if (next_state == ADV) {
        ret = bt_le_scan_stop();

        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("scan stop err %d in %s\n", ret, __func__);
        }

        struct bt_le_adv_param param = adv_scan_schd.param.adv_param;

        param.interval_min = BT_GAP_ADV_SLOW_INT_MIN;

        param.interval_max = param.interval_min;

        ret = bt_le_adv_start_instant(&param, adv_scan_schd.param.ad_data, adv_scan_schd.param.ad_len,
                                      adv_scan_schd.param.sd_data, adv_scan_schd.param.sd_len);

        if (ret && (ret != -EALREADY) && (ret != -ENOMEM)) {
            SCHD_LOGE("adv start err %d\n", ret);
        }

        if (adv_scan_schd.cur_st == SCHD_ADV_SCAN) {
            next_state = SCAN;
        } else if (adv_scan_schd.cur_st == SCHD_ADV) {
            next_state = ADV_IDLE;
        }

        adv_time = (!(param.options & BT_LE_ADV_OPT_CONNECTABLE)) ? NOCONN_ADV_DATA_TIEMOUT : CONN_ADV_DATA_TIEMOUT;
        next_time = adv_time;
    } else if (next_state == SCAN) {
        ret = bt_le_adv_stop();

        if (ret && (ret != -EALREADY)) {
            SCHD_LOGE("adv stop err %d\n", ret);
        }

        /* Here, we define the adv window of each package in adv duration (120ms or xmit related time)*/
        if (adv_scan_schd.param.adv_param.options & BT_LE_ADV_OPT_CONNECTABLE) {
            next_time = adv_scan_schd.param.adv_param.interval_min * 5 / 8 - adv_time;
        } else {
            next_time =
                CONFIG_ADV_INTERVAL_TIMER - adv_time; // adv_scan_schd.param.adv_param.interval_min * 5 / 8 - adv_time;
        }

        if (next_time > CONFIG_ADV_SCAN_INTERVAL_TIMER) {
            ret = bt_le_scan_start(&adv_scan_schd.param.scan_param, adv_scan_schd.param.scan_cb);

            if (ret) {
                SCHD_LOGE("scan err %d\n", ret);
            }
        }
        adv_time = 0;
        next_state = ADV;
    } else if (next_state == ADV_IDLE) {
        if (adv_scan_schd.param.adv_param.options & BT_LE_ADV_OPT_CONNECTABLE) {
            next_time = adv_scan_schd.param.adv_param.interval_min * 5 / 8 - adv_time;
        } else {
            next_time =
                CONFIG_ADV_INTERVAL_TIMER - adv_time; // adv_scan_schd.param.adv_param.interval_min * 5 / 8 - adv_time;
        }
        adv_time = 0;
        next_state = ADV;
    }
    k_mutex_unlock(&adv_scan_schd.mutex);
    k_timer_start(&adv_scan_schd.timer, next_time);
}

int bt_mesh_adv_scan_schd_init()
{
    memset(&adv_scan_schd, 0, sizeof(adv_scan_schd));
    k_timer_init(&adv_scan_schd.timer, adv_scan_timer, &adv_scan_schd);
    k_mutex_init(&adv_scan_schd.mutex);
    return 0;
}

static int set_ad_data(uint8_t *data, const struct bt_data *ad, size_t ad_len)
{
    int i;
    int set_len = 0;

    for (i = 0; i < ad_len; i++) {
        int len = ad[i].data_len;
        u8_t type = ad[i].type;

        /* Check if ad fit in the remaining buffer */
        if (set_len + len + 2 > 31) {
            len = 31 - (set_len + 2);

            if (type != BT_DATA_NAME_COMPLETE || len <= 0) {
                return -EINVAL;
            }

            type = BT_DATA_NAME_SHORTENED;
        }

        data[set_len++] = len + 1;
        data[set_len++] = type;

        memcpy(&data[set_len], ad[i].data, len);
        set_len += len;
    }

    return set_len;
}
int bt_mesh_adv_enable(const struct bt_le_adv_param *param, const struct bt_data *ad, size_t ad_len,
                       const struct bt_data *sd, size_t sd_len)
{
    if (param == NULL) {
        return -EINVAL;
    }

    BT_ERR("enter %s\n", __func__);
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
    adv_scan_schd.param.adv_param = *param;
    adv_scan_schd.param.ad_len = set_ad_data(adv_scan_schd.param.ad_data, ad, ad_len);
    adv_scan_schd.param.sd_len = set_ad_data(adv_scan_schd.param.sd_data, sd, sd_len);
    k_mutex_unlock(&adv_scan_schd.mutex);
    bt_mesh_adv_scan_schd_action(ADV_ON);
    return 0;
}

int bt_mesh_adv_disable()
{
    BT_ERR("enter %s\n", __func__);
    bt_mesh_adv_scan_schd_action(ADV_OFF);
    return 0;
}

int bt_mesh_scan_enable(void)
{
    struct bt_le_scan_param scan_param = { .type = BT_HCI_LE_SCAN_PASSIVE,
                                           .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
                                           .interval = MESH_SCAN_INTERVAL,
                                           .window = MESH_SCAN_WINDOW };

    BT_ERR("enter %s\n", __func__);
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
    adv_scan_schd.param.scan_param = scan_param;
    adv_scan_schd.param.scan_cb = bt_mesh_scan_cb;
    k_mutex_unlock(&adv_scan_schd.mutex);
    bt_mesh_adv_scan_schd_action(SCAN_ON);
    return 0;
}

int bt_mesh_scan_disable(void)
{
    BT_ERR("");
    bt_mesh_adv_scan_schd_action(SCAN_OFF);
    return 0;
}

int bt_mesh_adv_vnd_scan_register(vendor_beacon_cb bacon_cb)
{
    if (!bacon_cb) {
        return -EINVAL;
    }
    g_vendor_beacon_cb = bacon_cb;
    return 0;
}
