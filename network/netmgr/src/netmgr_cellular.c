/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <network/network.h>

#include <hal/cellular.h>

#ifdef WITH_LWIP
#include <lwip/priv/tcp_priv.h>
#include <lwip/udp.h>
#endif

#include "netmgr.h"

#define TAG "NETMGR_CELLULAR"

typedef struct {
    hal_cellular_module_t *hal_mod;
    int32_t                      ipv4_owned;
    int8_t                       disconnected_times;
    bool                         ip_available;
} netmgr_cxt_t;

static netmgr_cxt_t g_netmgr_cxt;

#if defined(WITH_LWIP) || defined(WITH_VENDOR_LWIP)
static void randomize_tcp_local_port();
#endif

static void format_ip(uint32_t ip, char *buf)
{
    int i = 0;

    unsigned char octet[4] = { 0, 0, 0, 0 };

    for (i = 0; i < 4; i++) {
        octet[i] = (ip >> ((3 - i) * 8)) & 0xFF;
    }

    sprintf(buf, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);
}


static int32_t translate_addr(char *str)
{
    int32_t a, b, c, d;
    int32_t address = 0;
    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    address |= d << 24;
    address |= c << 16;
    address |= b << 8;
    address |= a;

    return address;
}

static void netmgr_ip_got_event(hal_cellular_module_t *m, hal_cellular_ip_stat_t *pnet,
                                void *arg)
{
    LOGI(TAG, "Got ip : %s, gw : %s, mask : %s", pnet->ip, pnet->gate,
         pnet->mask);

#if defined(WITH_LWIP) || defined(WITH_VENDOR_LWIP)
    randomize_tcp_local_port();
#endif

    g_netmgr_cxt.ipv4_owned   = translate_addr(pnet->ip);
    g_netmgr_cxt.ip_available = true;
    aos_post_event(EV_CELLULAR, CODE_CELLULAR_ON_GOT_IP, 0u);
}
#if defined(WITH_LWIP) || defined(WITH_VENDOR_LWIP)
#ifdef LOCAL_PORT_ENHANCED_RAND

#define TCP_LOCAL_PORT_SEED "lport_seed"
#define SEED_HISTORAY_MAX 10

typedef struct
{
    unsigned int hist[SEED_HISTORAY_MAX];
    int          total_num;
    int          start_idx;
} seed_history_t;

static void ensure_different_seed(unsigned int *seed, seed_history_t *history)
{
    int i, idx = 0;

    if (!history || !seed) {
        return;
    }

    for (i = 0; i < history->total_num; i++) {
        idx = (i + history->start_idx) % SEED_HISTORAY_MAX;

        if (history->hist[idx] == *seed) {
            LOGD(TAG, "Same seed found %d\r\n", history->hist[idx]);
            *seed = *seed + (unsigned int)aos_now();
        }
    }
}

static void update_seed_history(seed_history_t *history, unsigned int new_seed)
{
    int idx = 0;

    if (!history) {
        return;
    }

    if (history->total_num >= SEED_HISTORAY_MAX) {
        LOGD("netmgr", "History is full, let's replace the first entry.");
        history->hist[history->start_idx] = new_seed;
        history->start_idx++;
        history->start_idx %= SEED_HISTORAY_MAX;
    } else {
        idx = (history->start_idx + history->total_num) % SEED_HISTORAY_MAX;
        history->hist[idx] = new_seed;
        history->total_num++;
    }
}

static void dump_seed_history(seed_history_t *history)
{
#ifdef DEBUG
    int i;

    if (!history) {
        return;
    }

    for (i = 0; i < history->total_num; i++) {
        LOGD(TAG, "%d ",
               history->hist[(history->start_idx + i) % SEED_HISTORAY_MAX]);
    }

    LOGD(TAG, "\r\n");
#endif
}
#endif /* LOCAL_PORT_ENHANCED_RAND */

static void randomize_tcp_local_port()
{
    unsigned int   ts        = (unsigned int)aos_now();
    static uint8_t rand_flag = 0;
#ifdef LOCAL_PORT_ENHANCED_RAND
    int            ret, len;
    seed_history_t seed_history;
#endif

    if (0 == rand_flag) { // Do the rand operation only once
#ifdef LOCAL_PORT_ENHANCED_RAND
        LOGD("netmgr", "The ts generated from system time is %d", ts);
        len = sizeof(seed_history);
        memset(&seed_history, 0, sizeof(seed_history));
        ret = aos_kv_get(TCP_LOCAL_PORT_SEED, &seed_history, &len);
        if (ret == 0) {
            LOGD("netmgr", "Seed found in kv.");
            dump_seed_history(&seed_history);
            ensure_different_seed(&ts, &seed_history);
        }
#endif /* LOCAL_PORT_ENHANCED_RAND */
        LOGD("netmgr", "The final seed to use is %d", ts);
        srand(ts);
        tcp_init();
        udp_init();
#ifdef LOCAL_PORT_ENHANCED_RAND
        update_seed_history(&seed_history, ts);
        LOGD("netmgr", "The new seed history to be saved:");
        dump_seed_history(&seed_history);
        ret = aos_kv_set(TCP_LOCAL_PORT_SEED, &seed_history,
                         sizeof(seed_history), 1);
#endif /* LOCAL_PORT_ENHANCED_RAND */
        rand_flag = 1;
    }
}
#endif /* LWIP */

static void netmgr_connect_fail_event(hal_cellular_module_t *m, int err, void *arg)
{}

static void netmgr_fatal_err_event(hal_cellular_module_t *m, void *arg)
{}

static void netmgr_disconnected_event(hal_cellular_module_t *m, void *arg)
{
    g_netmgr_cxt.disconnected_times++;
    aos_post_event(EV_CELLULAR, CODE_CELLULAR_ON_DISCONNECT, NULL);
}

static const hal_cellular_event_cb_t g_cellular_hal_event = {
    .connect_fail = netmgr_connect_fail_event,
    .ip_got = netmgr_ip_got_event,
    .disconnected = netmgr_disconnected_event,
    .fatal_err = netmgr_fatal_err_event,
};

static void netmgr_events_executor(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_CELLULAR) {
        return;
    }

    switch (eventinfo->code) {
        case CODE_CELLULAR_ON_DISCONNECT:
            g_netmgr_cxt.ip_available = false;
        default:
            break;
    }
}

bool netmgr_get_ip_state()
{
    return g_netmgr_cxt.ip_available;
}

int netmgr_cellular_init(void)
{
    hal_cellular_module_t *module;

    aos_register_event_filter(EV_CELLULAR, netmgr_events_executor, NULL);
    module = hal_cellular_get_default_module();
    memset(&g_netmgr_cxt, 0, sizeof(g_netmgr_cxt));
    g_netmgr_cxt.ip_available = false;
    g_netmgr_cxt.hal_mod = module;
    hal_cellular_install_event(g_netmgr_cxt.hal_mod, &g_cellular_hal_event);
    return 0;
}

void netmgr_cellular_deinit(void)
{
    memset(&g_netmgr_cxt, 0, sizeof(g_netmgr_cxt));
}

int netmgr_cellular_start(bool autoconfig)
{
    return 0;
}
