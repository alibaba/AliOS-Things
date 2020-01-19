/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/kernel.h>
#include <hal/wifi.h>
#include "state.h"
#include "define.h"
#include "frame.h"
#include "osal.h"
#include "hal_wifi.h"
#include "log.h"
#include "loop.h"
#include "umesh.h"
#include "pack.h"
#include "unpack.h"
#include "peers.h"
#include "utils.h"
#include "adapter_lwip.h"

extern const uint8_t BCAST_ADDR[];
const static uint8_t channel_scan_times[] = {2, 2, 2, 2, 2, 20, 2, 2, 2, 2, 2, 2, 2};

static void umesh_main_task(void *context);
static int umesh_set_phase(struct umesh_state *state, umesh_phase_t phase);

static void umesh_peer_identify(void *para)
{
    int ret = 0;
    static uint8_t send_cnt = 0;
    struct umesh_state *state = (struct umesh_state *)para;
    int duration = UMESH_IDENTIFY_REQUEST_DURATION + (umesh_rand() % (UMESH_IDENTIFY_REQUEST_DURATION >> 2));
    if (state == NULL) {
        return;
    }

    if (state->self_phase < UMESH_PHASE_IDENTIFY) {
        umesh_timer_cancel(&state->timers.scan_timer, umesh_peer_identify, para);
        return;
    }
    ret = umesh_send_frame_join_request(state);

    if (ret != 0) {
        log_e("umesh_peer_identify err:%d", ret);
    }

    if (send_cnt++ > UMESH_IDENTIFY_MAX_CNT) { /*no peer resp cur chan, so go to peers discovery phase */
        send_cnt = 0;
        umesh_timer_cancel(&state->timers.scan_timer, umesh_peer_identify, para);
        if (state->peers_state.joined == 0) {
            umesh_set_phase(state, UMESH_PHASE_SCAN);
        } else {
            umesh_set_phase(state, UMESH_PHASE_JOINED);
            umesh_heart_update_time(state);
        }
        return;
    }

    umesh_timer_start(&state->timers.scan_timer, duration, umesh_peer_identify, para);
    return;
}

static void umesh_peer_scan(void *para)
{
    int ret = 0;
    static uint8_t send_cnt = 0;
    int cur_chan_cnt = 0;
    int scan_finish = 0;
    struct umesh_state *state = (struct umesh_state *)para;
    int duration = UMESH_SCAN_PEER_DURATION + (umesh_rand() % (UMESH_SCAN_PEER_DURATION >> 2));
    if (state == NULL) {
        log_e("state is null!");
        return;
    }

    if (state->self_phase != UMESH_PHASE_SCAN) {
        umesh_timer_cancel(&state->timers.scan_timer, umesh_peer_identify, para);
        return;
    }

    if (umesh_wifi_get_connect_state() == 0) { /*should scan all channel  */
        if (state->chan_info.list == NULL) {
            log_e("channel list is null!");
            return;
        }

        if (state->chan_info.list[state->chan_info.it] == UMESH_SCAN_MAIN_CHANNEL) {
            cur_chan_cnt = UMESH_SCAN_BASE_CNT * UMESH_SCAN_MAIN_CHANNEL_PRIO;

        } else {
            cur_chan_cnt = UMESH_SCAN_BASE_CNT;
        }
        if (send_cnt == cur_chan_cnt) {
            uint8_t it_next;
            uint8_t channel;

            it_next = state->chan_info.it + 1;

            if (it_next == state->chan_info.num) {
                it_next = 0;
            }

            channel = state->chan_info.list[it_next];
            umesh_wifi_set_channel(channel);

            if (state->chan_info.it < state->chan_info.num - 1) {
                state->chan_info.it++;
            } else {
                state->chan_info.it = 0;
                scan_finish = 1;
            }

            send_cnt = 0;
        }
    } else if (send_cnt > UMESH_MASTER_SCAN_MAX_CNT) { /* master device */
        send_cnt = 0;
        scan_finish = 1;
    }

    if (scan_finish == 1) {
        /*如果已经有扫码到的节点，那么切换对应信道，加入他们*/
        int fina_chan = umesh_get_final_channel(state);
        log_i("final_chan = %d", fina_chan);
        if (fina_chan == 0) { /*重新扫描*/
            state->chan_info.it = 0;
        } else {
            umesh_wifi_set_channel(fina_chan);
            umesh_timer_cancel(&state->timers.scan_timer, umesh_peer_identify, para);
            ret = umesh_set_phase(state, UMESH_PHASE_IDENTIFY);
            return;
        }
    }
    ret = umesh_send_frame_discovery(state);
    if (ret != 0) {
        log_e("umesh_send_discovery err:%d", ret);
    }

    send_cnt++;
    umesh_timer_start(&state->timers.scan_timer, duration, umesh_peer_scan, para);

}

static int umesh_peers_keeplive(struct umesh_state *state)
{
    int ret = 0;
    int left_peers = 0;
    uint32_t timeout = 0;
    struct umesh_peer *peer;
    umesh_peers_it_t it = umesh_peers_it_new(state->peers_state.peers);
    if (it == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    while (umesh_peers_it_next(it, &peer) == PEERS_OK) {
        if (peer->step == UMESH_PEER_IDENTIFY_FINISH) {
            left_peers ++;
            timeout = (uint32_t)umesh_time_spend(peer->last_update);
            if (peer->heart_interval == 0) {
                log_e("!!!peer->heart_interval = 0");
            }
            if (timeout > peer->heart_interval * UMESH_HEART_TIMEOUT_CNT) {
                /*long time not get data from the peer ,delete it*/
                log_e("peer lost , timeout = %d", timeout);
                log_hex("peer lost", peer->addr, IEEE80211_MAC_ADDR_LEN);
                ret = umesh_peer_remove(state->peers_state.peers, peer->addr, state->peer_remove_cb, (void *)state);
                left_peers --;
            }
        }
    }
    umesh_peers_it_free(it);
    /*all peers lost ,need goto scan phase*/
    if (left_peers == 0) {
        ret = umesh_set_phase(state, UMESH_PHASE_SCAN);
    }
    return ret;
}

static void umesh_heart_beat(void *context)
{
    struct umesh_state *state = (struct umesh_state *)context;
    if (state == NULL) {
        return;
    }

    if (state->self_phase != UMESH_PHASE_JOINED) {
        log_w("In heart_beat timer,but phase = %d", state->self_phase);
        umesh_timer_cancel(&state->timers.heart_timer, umesh_heart_beat, context);
        return;
    }

    /* if there is no ping_timer timeout, then return success */
    if (umesh_time_is_expired(state->next_heart_time)) {
        log_d("~~~~~~~~~~~send heart beat~~~~~~~~~~~");
        umesh_heart_update_time(state);
        umesh_send_frame_heart_beat(state);
        /*if not get connect to wifi ,then ask ap info, beta...*/
        if(umesh_wifi_get_connect_state() == 0) {
            umesh_send_frame_zero_req(state, BCAST_ADDR);
        }
    }

    umesh_peers_keeplive(state);
    umesh_timer_start(&state->timers.heart_timer, UMESH_HEART_DURATION, umesh_heart_beat, context);
}


// static void umesh_send_data_to_mac(void *context)
// {

// }

// static void umesh_send_data_to_ip(void *context)
// {

// }


// int umesh_schedule_start(struct umesh_state *state)
// {
//     int ret = 0;
//     ret = umesh_timer_start(&state->timers.scan_timer, UMESH_SCAN_PEER_DURATION,umesh_peer_scan , state);
//     if(ret < 0) {
//         goto fail;
//     }

// fail:
//     return ret;
// }



void umesh_network_deinit(void *handle)
{
    struct umesh_state *state = handle;
    if (state == NULL) {
        return;
    }
    umesh_timer_cancel(&state->timers.scan_timer, umesh_peer_identify, NULL);
    umesh_timer_cancel(&state->timers.scan_timer, umesh_peer_scan, NULL);
    umesh_timer_cancel(&state->timers.heart_timer, umesh_heart_beat, NULL);
#if LWIP_IPV6 
    umesh_adapter_interface_deinit(state);
#endif
    umesh_state_deinit(state);
    umesh_free(state);
}

static int umesh_send_data_to_radio(struct umesh_state *state)
{
    int ret = UMESH_NO_ACTION_REQUIRED;
    umesh_buf_t *node = NULL, *next = NULL;
    list_for_each_entry_safe(node, next, &state->send_to_radio_list, linked_list, umesh_buf_t) {
        log_d("send data to radio,len = %d", node->len);
        ret = umesh_wifi_send(node->data, node->len);
        list_del(&node->linked_list);
        buf_free(node);
    }
    return ret;
}

static int umesh_send_data_to_ip(struct umesh_state *state)
{
    int ret = UMESH_NO_ACTION_REQUIRED;
    umesh_buf_t *node = NULL, *next = NULL;

    list_for_each_entry_safe(node, next, &state->send_to_ip_list, linked_list, umesh_buf_t) {
        /* todo ,send ip data to lwip*/
        list_del(&node->linked_list);
        buf_free(node);
    }

    return ret;
}

static void umesh_main_task(void *context)
{

    struct umesh_state *state = (struct umesh_state *)context;
    if (state == NULL) {
        return;
    }

    while (state->running) {
        umesh_semaphore_wait(state->generic_semap, UMESH_POLL_TIMEOUOT);
        switch (state->self_phase) {
            case UMESH_PHASE_SCAN:
                break;
            case UMESH_PHASE_IDENTIFY:
                break;
            case UMESH_PHASE_JOINED:
                umesh_send_data_to_ip(state);

                break;
            default:
                break;
        }

        umesh_send_data_to_radio(state);

    }
}

static int recieve_raw_data(const uint8_t *data, uint16_t len, const uint8_t *ta, const uint8_t *sa,  const uint8_t *da,
                            void *context)
{
    int ret;

    struct umesh_state *state = (struct umesh_state *)context;
    if (state == NULL || data == NULL || sa == NULL || da == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    /*da == self or da = bcast ,send to ip */
    if (memcmp(da, state->self_addr, IEEE80211_MAC_ADDR_LEN) == NULL) {  /*data to self */
        log_hex("get data from", sa, IEEE80211_MAC_ADDR_LEN);
        log_hex("ucast data", data, len);
        if(state->data_to_ip_cb) {
            state->data_to_ip_cb(data, len);
        }
        return 0;
    } else if (memcmp(da, BCAST_ADDR, IEEE80211_MAC_ADDR_LEN) == NULL) { /*data to self */
        log_hex("get data from", sa, IEEE80211_MAC_ADDR_LEN);
        log_hex("bcast data", data, len);
        if(state->data_to_ip_cb) {
            state->data_to_ip_cb(data, len);
        }
        return 0;
    } else { /*data to others */
        /* only support hop = 1*/
        ret = umesh_send_frame_data(state, da, sa, da, data, len);
    }
    return ret;
}
// static int recieve_raw_data(const uint8_t *data, uint16_t len, const uint8_t *from, void *context)
// {
//     log_hex("get data from", from, 6);
//     log_hex("data", data, len);
//     return 0;
// }

static void umesh_peer_lost(const uint8_t *addr, void *arg)
{
    log_i("------------------peer lost!-----------------");
    log_hex("peer addr", addr, 6);
}


static void umesh_peer_update(const uint8_t *addr, umesh_identify_step_t step, void *arg)
{
    log_i("+++++++++++++++++peer update,step =%d++++++++++++++++", step);
    log_hex("peer addr", addr, 6);
}

static void get_ap_info(const char *ssid, const char *pwd, const uint8_t *bssid)
{
    log_d("++++++++++get ssid = %s, pwd = %s++++++++++++",ssid, pwd);
}
void *umesh_network_init()
{
    int ret = 0;
    /*init state*/
    struct umesh_state *state = NULL;
    uint8_t mac[IEEE80211_MAC_ADDR_LEN];
    ret = umesh_wifi_get_mac(mac);
    if (ret != 0) {
        goto err;
    }
    state = umesh_malloc(sizeof(struct umesh_state));
    if (state == NULL) {
        ret = UMESH_ERR_MALLOC_FAILED;
        goto err;
    }

    ret = umesh_state_init(state, mac, umesh_peer_update, umesh_peer_lost, recieve_raw_data);

    if (ret != 0) {
        goto err;
    }

    ret = umesh_wifi_register_monitor_cb(0, umesh_wifi_rx_frame, state);

    if (ret != 0) {
        goto err;
    }

    ret = umesh_state_set_zero_config_cb(state, get_ap_info);
    ret = umesh_task_start(umesh_main_task, (void *)state, UMESH_TASK_STACK_SIZE, UMESH_TASK_HIGH_PRIO);
    if (ret != 0) {

        goto err;
    }

    ret = umesh_set_phase(state, UMESH_PHASE_SCAN);

    /*ret = umesh_schedule_start(state);*/
    if (ret != 0) {
        goto err;
    }
#if LWIP_IPV6 
    ret = umesh_adapter_interface_init(state);
    if (ret != 0) {
        goto err;
    }
#endif
    return state;
err:
    if (state != NULL) {
        umesh_network_deinit((void *)state);
#if LWIP_IPV6 
        umesh_adapter_interface_deinit(state);
#endif
        state = NULL;
    }
    log_e("umesh init err, ret = %d", ret);
    return (void *)state;
    /* */
}

static int umesh_set_phase(struct umesh_state *state, umesh_phase_t phase)
{
    int ret = 0;
    if (state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    state->self_phase = phase;

    /*start identify schedule*/
    switch (phase) {
        case UMESH_PHASE_SCAN:
            ret = umesh_timer_start(&state->timers.scan_timer, UMESH_SCAN_PEER_DURATION, umesh_peer_scan, state);
            break;
        case UMESH_PHASE_IDENTIFY:
            ret = umesh_timer_start(&state->timers.scan_timer, UMESH_IDENTIFY_REQUEST_DURATION, umesh_peer_identify, state);
            break;
        case UMESH_PHASE_JOINED:
            ret = umesh_timer_start(&state->timers.heart_timer, UMESH_HEART_DURATION, umesh_heart_beat, state);
            break;
        default:
            ret = UMESH_NO_ACTION_REQUIRED;
            break;
    }
    return ret;
}

int umesh_recv_ip_data(struct umesh_state *state, const uint8_t *data, uint16_t data_len, uint8_t *dst_addr)
{
    int ret = 0;
    struct umesh_peer *to_peer = NULL;
    const uint8_t *ra = NULL;
    const uint8_t *da = NULL;
    if (data == NULL || data == NULL || dst_addr == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    da = dst_addr;
    /*search if da in neighbor list */
    ret = umesh_peer_get(state->peers_state.peers, dst_addr, &to_peer);
    if (ret < 0) {
        /*todo:search if da in remote peers list */
        ra = BCAST_ADDR;
    } else {
        ra = dst_addr;
    }

    ret = umesh_send_frame_data(state, ra, state->self_addr, da, data, data_len);
    return ret;
}