

#ifndef UMESH_STATE_H_
#define UMESH_STATE_H_

#include <stdint.h>
#include <stdbool.h>

#include "buffer.h"
#include "peers.h"
#include "define.h"
#include "list.h"



struct umesh_state; /* forward declaration for tlv_cb */

typedef void (*umesh_tlv_cb)( umesh_peer_t *, uint8_t, const struct buf *, struct umesh_state *, void *);

typedef struct  {
	uint64_t tx_mng;
	uint64_t tx_data;
	uint64_t tx_data_unicast;
	uint64_t tx_data_multicast;
	uint64_t rx_mng;
	uint64_t rx_data;
	uint64_t rx_unknown;
}umesh_stats_t;

typedef struct {
    void *scan_timer;
	void *heart_timer;
	void *send_mac_timer;
    void *send_ip_timer;
}timer_handle_t;

typedef struct  {
    uint8_t it;
	uint8_t *list;
	uint8_t num;
}channel_info_t;

typedef struct {
    uint16_t master_count;
	uint16_t slave_count;
}peers_count_t;

typedef struct {
    peers_count_t counts;
    uint8_t channel;
    struct list_head linked_list;
}peers_scan_node_t;

typedef enum {
	UMESH_PHASE_SCAN = 0,
	UMESH_PHASE_IDENTIFY = 1,
	UMESH_PHASE_JOINED = 2,
}umesh_phase_t;

/* Complete node state */
typedef struct umesh_state {
	uint8_t running;
	uint8_t self_addr[ IEEE80211_MAC_ADDR_LEN];

	umesh_peer_type_t  self_type;
    umesh_phase_t  self_phase;
	channel_info_t chan_info;
	uint16_t seq_mac;
	uint16_t seq_data;
    uint32_t session_id; /*static in a connection*/
    uint16_t heart_duration;
	umesh_tlv_cb tlv_cb;
	void *tlv_cb_data;

	umesh_peer_cb peer_add_cb;
	/* Allows to hook removing of new neighbor */
	umesh_peer_cb peer_remove_cb;

	int filter_rssi; /* whether or not to filter (default: true) */
	signed char rssi_threshold; /* peers exceeding this threshold are discovered */
	signed char rssi_grace; /* once discovered accept lower RSSI */

 
	umesh_peers_state_t peers_state;
	timer_handle_t timers;
	umesh_stats_t stats; /*statistic data*/
	struct list_head   scaned_peers_list;
	struct list_head   send_to_radio_list;
	struct list_head   send_to_ip_list;
	void  *generic_semap;	
	void  *generic_lock;
	void  *to_radio_list_lock;
	void  *to_ip_list_lock; 
}umesh_state_t;

int umesh_state_init(struct umesh_state *state, uint8_t *self_addr);
int umesh_state_deinit(struct umesh_state *state);

uint16_t umesh_data_next_sequence_number(struct umesh_state *state);

uint16_t umesh_ieee80211_next_sequence_number(struct umesh_state *state);

int umesh_add_scaned_count(struct umesh_state *state,umesh_peer_type_t type);
/*uint64_t clock_time_us();*/

uint8_t umesh_get_final_channel(struct umesh_state *state);

#endif /* UMESH_STATE_H_ */
