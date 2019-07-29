/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_SID_ALLOCATOR_H
#define UR_SID_ALLOCATOR_H

#include "core/topology.h"

enum {
    SUPER_ROUTER_SID = 0,
    LEADER_SID       = 0,
    BCAST_SID        = 0xffff,
    INVALID_SID      = 0xfffe,
};

#define is_unique_sid(sid) (sid != BCAST_SID && sid != INVALID_SID)

enum {
    STRUCTURED_SID   = 0,
    SHORT_RANDOM_SID = 1,
    RANDOM_SID       = 2,
};

enum {
    SID_LEN      = 16,
    SID_MASK     = 0xf,
    SID_MASK_LEN = 4,
};

enum {
    MOBILE_PREFIX   = 0xc,
    LOWPOWER_PREFIX = 0xd,
};

enum {
    PF_SID_PREFIX_MASK   = 0xf,
    PF_SID_PREFIX_OFFSET = 12,
};

enum {
    PF_NODE_NUM = 128,
    RSID_NUM    = 2048,
};

typedef struct sid_node_s {
    slist_t next;
    ur_node_id_t node_id;
    media_type_t type;
} sid_node_t;

typedef struct sid_base_s {
    slist_t node_list;
    int sid_type;
    uint16_t node_num;
} sid_base_t;

typedef struct ssid_allocator_s {
    sid_base_t base;
    int16_t sid_shift;
    uint16_t sid_prefix;
    uint32_t attach_free_bits[1];
    uint16_t pf_node_num;
    uint32_t mobile_free_bits[(PF_NODE_NUM + 31) / 32];
} ssid_allocator_t;

typedef struct rsid_allocator_s {
    sid_base_t base;
    uint32_t free_bits[(RSID_NUM + 31) / 32];
} rsid_allocator_t;

typedef unsigned long allocator_t;

// structured sid
allocator_t allocator_init(uint16_t sid, int sid_type);
void allocator_deinit(allocator_t);
ur_error_t allocate_sid(allocator_t hdl, ur_node_id_t *node_id);
void free_sid(allocator_t, uint16_t sid);
ur_error_t update_sid_mapping(allocator_t, ur_node_id_t *node_id, bool to_add);

uint16_t get_allocated_number(allocator_t);
uint32_t get_allocated_bitmap(allocator_t);
uint16_t get_allocated_pf_number(allocator_t);
uint16_t get_free_number(allocator_t);
slist_t *get_ssid_nodes_list(allocator_t);
bool is_direct_child(allocator_t, uint16_t sid);
sid_node_t *get_allocated_child(allocator_t, neighbor_t *nbr);
bool is_partial_function_sid(uint16_t sid);

// random sid
allocator_t rsid_allocator_init(int sid_type);
void rsid_allocator_deinit(allocator_t);
ur_error_t rsid_allocate_sid(allocator_t, ur_node_id_t *node_id);
ur_error_t rsid_free_sid(allocator_t, ur_node_id_t *node_id);
uint16_t rsid_get_allocated_number(allocator_t);

static inline int find_first_free_bit(uint32_t *bits, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        if (!(bits[i / 32] & (1 << (i % 32)))) {
            continue;
        }

        bits[i / 32] &= ~(1 << (i % 32));
        return i;
    }

    return -1;
}

static inline ur_error_t grab_free_bit(uint32_t *bits, uint32_t len,
                                       uint16_t index)
{
    if (index >= len) {
        return UR_ERROR_FAIL;
    }
    if (bits[index / 32] & (1 << (index % 32))) {
        bits[index / 32] &= ~(1 << (index % 32));
        return UR_ERROR_NONE;
    }
    return UR_ERROR_FAIL;
}

static inline bool release_bit(uint32_t *bits, int len, int i)
{
    if (i >= len) {
        return false;
    }
    if (bits[i / 32] & (1 << (i % 32))) {
        return false;
    }
    bits[i / 32] |= 1 << (i % 32);
    return true;
}

#endif  /* UR_SID_ALLOCATOR_H */
