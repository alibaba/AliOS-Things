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

const static uint8_t channel_scan_times[] = {2, 2, 2, 2, 2, 20, 2, 2, 2, 2, 2, 2, 2};

static void umesh_main_task(void * context);
static int umesh_set_phase(struct umesh_state *state, umesh_phase_t phase);

static void umesh_peer_identify(void *para)
{
    int ret = 0;
    static uint8_t send_cnt = 0;
    int max_cnt = 0;
    struct umesh_state *state = (struct umesh_state *)para;
    int duration = UMESH_IDENTIFY_REQUEST_DURATION + (umesh_rand()%(UMESH_IDENTIFY_REQUEST_DURATION>>2));
    if(state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    ret = umesh_send_frame_join_request(state);
    umesh_timer_start(&state->timers.scan_timer, duration ,umesh_peer_identify , para);
    if(ret != 0) {
        log_e("umesh_peer_identify err:%d", ret);
    }

    if(send_cnt++ > UMESH_IDENTIFY_MAX_CNT) { /*no peer resp cur chan, so go to peers discovery phase */
        send_cnt = 0;
        umesh_timer_cancel(&state->timers.scan_timer ,umesh_peer_identify , para);
        if(state->self_phase == UMESH_PHASE_IDENTIFY) {
            umesh_set_phase(state, UMESH_PHASE_SCAN);
        }
    }
    return ret;
}

static void umesh_peer_scan(void *para)
{
    int ret = 0;
    static uint8_t send_cnt = 0;
    int cur_chan_cnt = 0;
    int scan_finish = 0;
    struct umesh_state *state = (struct umesh_state *)para;
    int duration = UMESH_SCAN_PEER_DURATION + (umesh_rand()%(UMESH_SCAN_PEER_DURATION>>2));
    if(state == NULL) {
        log_e("state is null!");
        return UMESH_ERR_NULL_POINTER;
    }

    if(umesh_wifi_get_connect_state() == 0) { /*should scan all channel  */
        if(state->chan_info.list == NULL) {
            log_e("channel list is null!");
            return UMESH_WIFI_NO_CHAN_LIST;
        }

        if(state->chan_info.list[state->chan_info.it] == UMESH_SCAN_MAIN_CHANNEL) {
            cur_chan_cnt = UMESH_SCAN_BASE_CNT * UMESH_SCAN_MAIN_CHANNEL_PRIO;

        } else {
            cur_chan_cnt = UMESH_SCAN_BASE_CNT;
        }
        if(send_cnt == cur_chan_cnt) {
            uint8_t channel = state->chan_info.list[state->chan_info.it];
            umesh_wifi_set_channel(channel);
            if(state->chan_info.it < state->chan_info.num - 1) { 
                state->chan_info.it++;
            } else {
                state->chan_info.it = 0;
                scan_finish = 1;
            }
            
            send_cnt = 0;
        }
    } else if(send_cnt > UMESH_MASTER_SCAN_MAX_CNT){ /* master device */
         send_cnt = 0;
         scan_finish = 1;
    }

    if(scan_finish == 1) {
        /*如果已经有扫码到的节点，那么切换对应信道，加入他们*/
        int fina_chan = umesh_get_final_channel(state);
        log_i("final_chan = %d", fina_chan);
        if(fina_chan == 0) { /*重新扫描*/
            state->chan_info.it = 0;
        } else {
            umesh_wifi_set_channel(fina_chan);
            umesh_timer_cancel(&state->timers.scan_timer ,umesh_peer_identify , para);
            ret = umesh_set_phase(state, UMESH_PHASE_IDENTIFY);
            return ret;
        }
    }
    ret = umesh_send_frame_discovery(state);
    if(ret != 0) {
        log_e("umesh_send_discovery err:%d", ret);
    }

    send_cnt++;
    umesh_timer_start(&state->timers.scan_timer, duration ,umesh_peer_scan , para);

    return ret;
}


static void umesh_heart_beat(void *context)
{
    struct umesh_state * state = (struct umesh_state *)context;
    if(state == NULL) {
        return;
    }
    umesh_send_heart_beat(state);
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
    if(state == NULL) {
        return;
    }

    umesh_state_deinit(state);
    umesh_free(state);

}

static int umesh_send_data_to_radio(struct umesh_state * state)
{
    int ret = UMESH_NO_ACTION_REQUIRED;
    umesh_buf_t *node = NULL, *next = NULL;
    log_d("send data to radio....");
    list_for_each_entry_safe(node, next, &state->send_to_radio_list, linked_list, umesh_buf_t) {
        log_d("send data to radio,len = %d",node->len);
        ret = umesh_wifi_send(node->data, node->len);
	    list_del(&node->linked_list);
        buf_free(node);    
    }
    return ret;
}

static int umesh_send_data_to_ip(struct umesh_state * state)
{
    int ret = UMESH_NO_ACTION_REQUIRED;
    umesh_buf_t *node = NULL, *next = NULL;
    
    list_for_each_entry_safe(node, next, &state->send_to_radio_list, linked_list, umesh_buf_t) {
        /* todo ,send ip data to lwip*/
	    list_del(&node->linked_list);
        buf_free(node);    
    }

    return ret;
}

static void umesh_main_task(void * context)
{
    int ret = 0;
    struct umesh_state *state = (struct umesh_state *)context;
    if(state == NULL) {
        return;
    }

    while(state->running) {
        umesh_semaphore_wait(state->generic_semap, UMESH_POLL_TIMEOUOT);
        switch(state->self_phase){
            case UMESH_PHASE_SCAN:
                //umesh_peer_scan(state);
            break;
            case UMESH_PHASE_IDENTIFY:
                //umesh_join_request(state);
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

void *umesh_network_init() {
    int ret = 0;
    /*init state*/
    struct umesh_state *state = NULL;
    uint8_t mac[IEEE80211_MAC_ADDR_LEN];
    ret = umesh_wifi_get_mac(mac);
    if(ret != 0) {
        goto err;
    }
    state = umesh_malloc(sizeof(struct umesh_state));
    if(state == NULL) {
        ret = UMESH_ERR_MALLOC_FAILED;
        goto err;
    }

    ret = umesh_state_init(state, mac);

    if(ret != 0) {
        goto err;
    }
    
    ret = umesh_wifi_register_monitor_cb(0, NULL, umesh_wifi_rx_frame);

    if(ret != 0) {
        goto err;
    }
    ret = umesh_task_start(umesh_main_task, (void *)state, UMESH_TASK_STACK_SIZE, UMESH_TASK_HIGH_PRIO);
    if(ret != 0) {
        goto err;
    }

    ret = umesh_set_phase(state, UMESH_PHASE_SCAN);

    /*ret = umesh_schedule_start(state);*/
    if(ret != 0) {
        umesh_network_deinit((void *)state);
        state = NULL;
        goto err;
    }
    return state; 
err:
    log_e("umesh init err, ret = %d", ret);
    return (void *)state;
    /* */
} 

static int umesh_set_phase(struct umesh_state *state, umesh_phase_t phase)
{
    int ret = 0;
	if(state == NULL) {
		return UMESH_ERR_NULL_POINTER;
	}
	state->self_phase = phase;
    
    /*start identify schedule*/
    switch(phase) {
        case UMESH_PHASE_SCAN:
            ret = umesh_timer_start(&state->timers.scan_timer, UMESH_SCAN_PEER_DURATION,umesh_peer_scan , state);
        break;
        case UMESH_PHASE_IDENTIFY:
            ret = umesh_timer_start(&state->timers.scan_timer, UMESH_IDENTIFY_REQUEST_DURATION,umesh_peer_identify , state);
        break;
        case UMESH_PHASE_JOINED:
            /*should not stop send identify request immediately when switched to this phase */
        break;
        default:
        ret = UMESH_NO_ACTION_REQUIRED;
        break;
    }
    return ret;
}