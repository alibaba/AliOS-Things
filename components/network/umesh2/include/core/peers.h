
#ifndef UMESH_PEERS_H
#define UMESH_PEERS_H

#include <stdint.h>
#include "define.h"

#define UMESH_PEER_RANDOM_LEN 8

enum peers_status {
	PEERS_ERR = -2, /* Peer does not exist */
	PEERS_MISSING = -1, /* Peer does not exist */
	PEERS_OK = 0, /* New peer added */
	PEERS_UPDATE = 1, /* Peer updated */
};

typedef enum {
	UMESH_PEER_MASTER = 0,
	UMESH_PEER_SLAVE = 1,
}umesh_peer_type_t;



typedef enum {
	UMESH_PEER_IDENTIFY_REQ = 0, //被动方收到加入请求后，创建节点，状态先设置为此
	UMESH_PEER_IDENTIFY_RESP = 1, //发送/接收加入允许状态设置为此
	UMESH_PEER_IDENTIFY_FINISH = 2, //完成帧发送/接收到后，状态设置为此
}umesh_identify_step_t;

typedef struct  {
	uint8_t addr_sa[IEEE80211_MAC_ADDR_LEN];
	uint8_t addr_da[IEEE80211_MAC_ADDR_LEN];
	uint8_t addr_ta[IEEE80211_MAC_ADDR_LEN];
	uint8_t addr_ra[IEEE80211_MAC_ADDR_LEN];
}umesh_path_info_t;
/*要考虑内存消耗啊*/
typedef struct umesh_peer {
	umesh_peer_type_t type;
	umesh_identify_step_t step;
	uint8_t addr[IEEE80211_MAC_ADDR_LEN];
	uint16_t heart_interval;
	uint32_t session_id; 
	uint64_t last_update;
    uint8_t  is_lp;
	uint8_t version;
	uint8_t from_random[UMESH_PEER_RANDOM_LEN];
	uint8_t to_random[UMESH_PEER_RANDOM_LEN];	
}umesh_peer_t;

typedef void *umesh_peers_t;

typedef void (*umesh_peer_cb)( umesh_peer_t *, void *arg);

typedef struct umesh_peers_state {
	umesh_peers_t peers;
	uint64_t timeout;
}umesh_peers_state_t;

enum peers_status umesh_peer_add(umesh_peers_t peers, const uint8_t *_addr, 
                                 uint64_t now);

struct umesh_peer * umesh_peer_add2(umesh_peers_t peers, const uint8_t *_addr, 
	umesh_peer_type_t type, umesh_identify_step_t step,
	uint64_t now, uint32_t session_id,
	uint8_t *from_random, uint8_t *to_random);

int umesh_peer_delete(struct umesh_peer * peer);

void umesh_peers_state_init(umesh_peers_state_t *state);
void umesh_peers_state_deinit(umesh_peers_state_t *state);

umesh_peers_t umesh_peers_init();

void umesh_peers_free(umesh_peers_t peers);

int umesh_peers_length(umesh_peers_t peers);

// enum peers_status
// umesh_peer_add(umesh_peers_t peers, const uint8_t *addr, uint64_t now, umesh_peer_cb cb, void *arg);

enum peers_status umesh_peer_remove(umesh_peers_t peers, const uint8_t *addr, umesh_peer_cb cb, void *arg);

int umesh_peer_get(umesh_peers_t peers, const uint8_t *addr,  umesh_peer_t **peer);

int umesh_peer_print(const  umesh_peer_t *peer, char *str, uint32_t len);

/**
 * Apply callback to and then remove all peers matching a filter
 * @param peers the umesh_peers_t instance
 * @param before remove all entries with an last_update timestamp {@code before}
 * @param cb the callback function, can be NULL
 * @param arg will be passed to callback function
 */
void umesh_peers_remove(umesh_peers_t peers, uint64_t before, umesh_peer_cb cb, void *arg);

int umesh_peers_print(umesh_peers_t peers, char *str, uint32_t len);

/* Iterator functions */
typedef void *umesh_peers_it_t;

umesh_peers_it_t umesh_peers_it_new(umesh_peers_t in);

enum peers_status umesh_peers_it_next(umesh_peers_it_t it,  umesh_peer_t **peer);

enum peers_status umesh_peers_it_remove(umesh_peers_it_t it);

void umesh_peers_it_free(umesh_peers_it_t it);

#endif /* ADWL_PEERS_H */
