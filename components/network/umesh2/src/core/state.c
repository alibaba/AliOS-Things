
#include <time.h>
#include <string.h>
#include "define.h"
#include "state.h"
#include "peers.h"
#include "list.h"
#include "frame.h"
#include "log.h"
#include "osal.h"
#include "hal_wifi.h"

const uint8_t ETHER_BROADCAST[IEEE80211_MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const uint8_t deamon_key[] = {0x01, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00};
static int umesh_channel_init(channel_info_t *info);

void umesh_stats_init(umesh_stats_t *stats)
{
    stats->tx_mng = 0;
    stats->tx_data = 0;
    stats->tx_data_unicast = 0;
    stats->tx_data_multicast = 0;
    stats->rx_mng = 0;
    stats->rx_data = 0;
    stats->rx_unknown = 0;
}

int umesh_state_init(struct umesh_state *state, uint8_t *self_addr, umesh_peer_update_cb peer_update_cb,
                     umesh_peer_lost_cb peer_lost_cb,  get_raw_data_cb data_cb)
{
    if (state == NULL || self_addr == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    memset(state, 0, sizeof(struct umesh_state));

    memcpy(state->self_addr, self_addr, IEEE80211_MAC_ADDR_LEN);
    memcpy(state->aes_key, deamon_key, UMESH_AES_KEY_LEN);
    state->heart_duration = UMESH_HEART_DURATION;
    state->filter_rssi = 1;

    state->peer_add_cb = peer_update_cb;
    state->peer_remove_cb = peer_lost_cb;
    state->raw_data_cb = data_cb;
    state->rssi_threshold = UMESH_RSSI_THRESHOLD_DEFAULT;
    state->rssi_grace = UMESH_RSSI_GRACE_DEFAULT;
    state->session_id = umesh_rand();
    state->to_radio_list_lock = umesh_mutex_new();

    if (state->to_radio_list_lock == NULL) {
        goto fail;
    }
    state->to_ip_list_lock = umesh_mutex_new();
    if (state->to_ip_list_lock == NULL) {
        goto fail;
    }
    state->generic_lock = umesh_mutex_new();
    if (state->generic_lock == NULL) {
        goto fail;
    }

    state->generic_semap = umesh_semaphore_new();
    if (state->generic_semap == NULL) {
        goto fail;
    }
    INIT_LIST_HEAD(&state->send_to_radio_list);
    INIT_LIST_HEAD(&state->send_to_ip_list);
    INIT_LIST_HEAD(&state->scaned_peers_list);

    umesh_channel_init(&state->chan_info);
    umesh_peers_state_init(&state->peers_state);
    umesh_stats_init(&state->stats);
    state->running = 1;
    return 0;
fail:
    if (state->to_radio_list_lock != NULL) {
        umesh_mutex_free(state->to_radio_list_lock);
    }
    if (state->to_ip_list_lock != NULL) {
        umesh_mutex_free(state->to_ip_list_lock);
    }
    if (state->generic_lock != NULL) {
        umesh_mutex_free(state->generic_lock);
    }
    if (state->generic_semap != NULL) {
        umesh_semaphore_free(state->generic_semap);
    }
    return UMESH_ERR_MALLOC_FAILED;
}

int umesh_state_set_zero_config_cb(struct umesh_state *state, ap_info_cb cb)
{
    if (state == NULL || cb == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    state->get_ap_info_cb = cb;
    return 0;
}
int umesh_state_deinit(struct umesh_state *state)
{
    peers_scan_node_t *node1 = NULL, *next_node1 = NULL;
    umesh_buf_t *node2 = NULL, *next_node2 = NULL;
    umesh_mutex_lock(state->generic_lock);

    list_for_each_entry_safe(node1, next_node1, &state->scaned_peers_list, linked_list, peers_scan_node_t) {
        list_del(&node1->linked_list);
        umesh_free(node1);
    }

    list_for_each_entry_safe(node2, next_node2, &state->send_to_radio_list, linked_list, umesh_buf_t) {
        list_del(&node2->linked_list);
        buf_free(node2);
    }

    list_for_each_entry_safe(node2, next_node2, &state->send_to_ip_list, linked_list, umesh_buf_t) {
        list_del(&node2->linked_list);
        buf_free(node2);
    }


    umesh_peers_state_deinit(&state->peers_state);
    umesh_mutex_unlock(state->generic_lock);

    umesh_mutex_free(state->to_radio_list_lock);
    umesh_mutex_free(state->to_ip_list_lock);
    umesh_semaphore_free(state->generic_semap);
    memset(state, 0, sizeof(struct umesh_state));
    return 0;
}
static int umesh_channel_init(channel_info_t *info)
{
    int ret;
    if (info == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    ret = umesh_wifi_get_channel_list((const uint8_t **)&info->list, &info->num);
    info->it = 0;
    return ret;
}

uint16_t umesh_data_next_sequence_number(struct umesh_state *state)
{
    return state->seq_data++;
};



uint16_t umesh_ieee80211_next_sequence_number(struct umesh_state *state)
{
    uint16_t seq = state->seq_mac;
    state->seq_mac += 1;
    state->seq_mac &= 0x0fff; /* seq has only 12 bits */
    return seq;
};


int umesh_add_scaned_count(struct umesh_state *state, umesh_peer_type_t type)
{
    int ret = UMESH_WIFI_NO_CHAN_LIST;
    uint8_t find = 0;
    peers_scan_node_t *node, *next;

    if (state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    if (state->chan_info.list == NULL) {
        return UMESH_WIFI_NO_CHAN_LIST;
    }
    uint8_t cur_channel = state->chan_info.list[state->chan_info.it];
    log_i("umesh_add_scaned_count ,cur channel = %d", cur_channel);
    umesh_mutex_lock(state->generic_lock);
    list_for_each_entry_safe(node, next, &state->scaned_peers_list, linked_list, peers_scan_node_t) {
        if (node->channel == cur_channel) {
            find = 1;
            if (type == UMESH_PEER_MASTER) {
                node->counts.master_count++;
                log_i("master_count = %d", node->counts.master_count);
            } else {
                node->counts.slave_count++;
                log_i("slave_count = %d", node->counts.slave_count);
            }
            break;
            ret = 0;
        }
    }
    umesh_mutex_unlock(state->generic_lock);

    if (find != 0) {
        return ret;
    }

    node = umesh_malloc(sizeof(peers_scan_node_t));
    if (node == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    memset(node, 0, sizeof(peers_scan_node_t));
    if (type == UMESH_PEER_MASTER) {
        node->counts.master_count++;
        log_i("master_count = %d", node->counts.master_count);
    } else {
        node->counts.slave_count++;
        log_i("slave_count = %d", node->counts.slave_count);
    }
    node->channel = cur_channel;
    umesh_mutex_lock(state->generic_lock);
    list_add_tail(&node->linked_list, &state->scaned_peers_list);
    umesh_mutex_unlock(state->generic_lock);
    return ret;
}

typedef struct {
    uint16_t count;
    uint16_t channel;
} chan_peer_count_t;

uint8_t umesh_get_final_channel(struct umesh_state *state)
{
    chan_peer_count_t master_max;
    chan_peer_count_t slave_max;
    peers_scan_node_t *node, *next;
    uint8_t final_chan;
    int i;
    if (state == NULL) {
        return 0;
    }

    memset(&master_max, 0, sizeof(chan_peer_count_t));
    memset(&slave_max, 0, sizeof(chan_peer_count_t));
    umesh_mutex_lock(state->generic_lock);
    list_for_each_entry_safe(node, next, &state->scaned_peers_list, linked_list, peers_scan_node_t) {
        if (node->counts.master_count > master_max.count) {
            master_max.count = node->counts.master_count;
            master_max.channel = node->channel;
        }
        if (node->counts.slave_count > slave_max.count) {
            slave_max.count = node->counts.slave_count;
            slave_max.channel = node->channel;
        }
        list_del(&node->linked_list);
        umesh_free(node);

    }
    umesh_mutex_unlock(state->generic_lock);
    log_i("master_count = %d,chan = %d", master_max.count, master_max.channel);
    log_i("slave_count = %d,chan = %d", slave_max.count, slave_max.channel);
    if (master_max.count > 0) {
        final_chan = master_max.channel;
    } else if (slave_max.count > 0) {
        final_chan = slave_max.channel;
    } else {
        return 0;
    }

    for (i = 0; i < state->chan_info.num; i ++) {
        if (final_chan == state->chan_info.list[i]) {
            return final_chan;
        }
    }
    return 0;
}

const char *umesh_tlv_as_str(umesh_tlvs_t type)
{
    switch (type) {
        case UMESH_VENDOR_TLV:
            return "vendor";
            break;
        case UMESH_JOIN_TLV:
            return "join params";
            break;
        case UMESH_HEART_BEAT_TLV:
            return "heart beat";
            break;
        case UMESH_DATA_TLV:
            return "data";
            break;
        case UMESH_ZERO_REQ_TLV:
            return "zero config req";
            break;
        case UMESH_ZERO_RESP_TLV:
            return "zero config resp";
            break;
        case UMESH_ZERO_FINISH_TLV:
            return "zero config finish";
            break;
        default:
            break;
    }
    return "unknow tlv";
}

int umesh_heart_update_time(struct umesh_state *state)
{

    if (state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    umesh_time_countdown_ms(&state->next_heart_time, (uint32_t)(state->heart_duration >> 2));
    return 0;
}

int umesh_get_self_type(struct umesh_state *state)
{
    if (state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    return (int)state->self_type;
}