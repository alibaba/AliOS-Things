
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "define.h"
#include "peers.h"
#include "hashmap.h"
#include "log.h"
#include "define.h"
#include "utils_time.h"
#include "osal.h"

void umesh_peers_state_init(umesh_peers_state_t *state)
{
    state->peers = umesh_peers_init();
    INIT_LIST_HEAD(&state->remote_peers_list);
    state->joined = 0;
    //state->timeout = UMESH_PEERS_DEFAULT_CLEAN_INTERVAL;
}

void umesh_peers_state_deinit(umesh_peers_state_t *state)
{
    umesh_remote_peer_t *node = NULL, *next_node = NULL;
    /*clean neighbor peers map*/
    umesh_peers_free(state->peers);
    /*clean remote peers list*/
    list_for_each_entry_safe(node, next_node, &state->remote_peers_list, linked_list, umesh_remote_peer_t) {
        list_del(&node->linked_list);
        umesh_free(node);
    }
}
umesh_peers_t umesh_peers_init()
{
    return (umesh_peers_t) hashmap_new(IEEE80211_MAC_ADDR_LEN);
}

void umesh_peers_free(umesh_peers_t peers)
{
    struct umesh_peer *peer;
    map_t map = (map_t) peers;

    /* Remove all allocated peers */
    map_it_t it = hashmap_it_new(map);
    while (hashmap_it_next(it, NULL, (any_t *) &peer) == MAP_OK) {
        hashmap_it_remove(it);
        free(peer);
    }
    hashmap_it_free(it);

    /* Remove hashmap itself */
    hashmap_free(peers);
}

int umesh_peers_num(umesh_peers_t peers)
{
    int len;
    map_t map = (map_t) peers;
    len = hashmap_length(map);
    return len;
}

// static int umesh_peer_is_valid(const struct umesh_peer *peer) {
//  return peer->step; /*!!! */
// }

static struct umesh_peer *umesh_peer_new(const uint8_t *addr)
{
    if (addr == NULL) {
        return NULL;
    }
    struct umesh_peer *peer = (struct umesh_peer *) umesh_malloc(sizeof(struct umesh_peer));
    memset(peer, 0, sizeof(struct umesh_peer));
    memcpy(peer->addr, addr, IEEE80211_MAC_ADDR_LEN);
    peer->version = 0;

    return peer;
}

struct umesh_peer *umesh_peer_add2(umesh_peers_t peers, const uint8_t *_addr,
                                   umesh_peer_type_t type, umesh_identify_step_t step,
                                   uint64_t now, uint32_t session_id,
                                   uint8_t *from_random, uint8_t *to_random)
{
    int status;
    map_t map = (map_t) peers;
    mkey_t addr = (mkey_t) _addr;
    struct umesh_peer *peer = NULL;
    //log_info("--- ready add peer %s (%s)", ether_ntoa(&peer->addr), peer->name==NULL?"NULL":peer->name);
    status = hashmap_get(map, addr, (any_t *) &peer, 0 /* do not remove */);
    if (status == MAP_MISSING) {
        peer = umesh_peer_new(_addr);    /* create new entry */
    }

    if (peer == NULL) {
        return NULL;
    }
    /* update */
    peer->type = type;
    peer->step = step;

    peer->session_id = session_id;
    peer->last_update = now;
    if (from_random != NULL) {
        memcpy(peer->from_random, from_random, UMESH_RANDOM_LEN);
    }
    if (to_random != NULL) {
        memcpy(peer->to_random, to_random, UMESH_RANDOM_LEN);
    }

    if (status == MAP_OK) {
        return peer;
    }

    status = hashmap_put(map, (mkey_t) &peer->addr, peer);
    if (status != MAP_OK) {
        umesh_free(peer);
        return NULL;
    }

    return peer;
}


enum peers_status umesh_peer_remove(umesh_peers_t peers, const uint8_t *_addr, umesh_peer_lost_cb cb, void *arg)
{
    int status;
    map_t map = (map_t) peers;
    mkey_t addr = (mkey_t) _addr;
    struct umesh_peer *peer;
    status = hashmap_get(map, addr, (any_t *) &peer, 1 /* remove */);
    if (status == MAP_MISSING) {
        return UMESH_ERR_PEER_MISSING;
    }

    if (cb) {
        cb(peer->addr, arg);
    }
    umesh_free(peer);
    return 0;
}

int umesh_peer_get(umesh_peers_t peers, const uint8_t *_addr, struct umesh_peer **peer)
{
    int status;
    map_t map = (map_t) peers;
    mkey_t addr = (mkey_t) _addr;
    status = hashmap_get(map, addr, (any_t *) peer, 0 /* keep */);
    if (status == MAP_MISSING) {
        return UMESH_ERR_PEER_MISSING;
    }
    return 0;
}

int umesh_peer_print(const struct umesh_peer *peer, char *str, uint32_t len)
{
    char *cur = str, *const end = str + len;
    cur += snprintf(cur, end - cur, "%02x:%02x:%02x:%02x:%02x:%02x ", peer->addr[0], peer->addr[1], peer->addr[2],
                    peer->addr[3], peer->addr[4], peer->addr[5]);
    return end - cur;
}

int umesh_peers_print(umesh_peers_t peers, char *str, uint32_t len)
{
    char *cur = str, *const end = str + len;
    map_t map = (map_t) peers;
    map_it_t it = hashmap_it_new(map);
    struct umesh_peer *peer;

    while (hashmap_it_next(it, NULL, (any_t *) &peer) == MAP_OK) {
        cur += umesh_peer_print(peer, cur, end - cur);
        cur += snprintf(cur, end - cur, "\n");
    }

    hashmap_it_free(it);
    return end - cur;
}

void umesh_peers_remove(umesh_peers_t peers, uint64_t before, umesh_peer_lost_cb cb, void *arg)
{
    map_t map = (map_t) peers;
    map_it_t it = hashmap_it_new(map);
    struct umesh_peer *peer;

    while (hashmap_it_next(it, NULL, (any_t *) &peer) == MAP_OK) {
        if (peer->last_update < before) {

            if (cb) {
                cb(peer->addr, arg);
            }

            hashmap_it_remove(it);
            umesh_free(peer);
        }
    }
    hashmap_it_free(it);
}

umesh_peers_it_t umesh_peers_it_new(umesh_peers_t in)
{
    return (umesh_peers_it_t) hashmap_it_new((map_t) in);
}

enum peers_status umesh_peers_it_next(umesh_peers_it_t it, struct umesh_peer **peer)
{
    int s = hashmap_it_next((map_it_t) it, 0, (any_t *) peer);
    if (s == MAP_OK) {
        return PEERS_OK;
    } else {
        return PEERS_MISSING;
    }
}

enum peers_status umesh_peers_it_remove(umesh_peers_it_t it)
{
    int s = hashmap_it_remove((map_it_t) it);
    if (s == MAP_OK) {
        return PEERS_OK;
    } else {
        return PEERS_MISSING;
    }
}

void umesh_peers_it_free(umesh_peers_it_t it)
{
    hashmap_it_free((map_it_t) it);
}
