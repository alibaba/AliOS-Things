/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_MESH_FORWARDER_H
#define UR_MESH_FORWARDER_H

#include "utilities/message.h"
#include "hal/interface_context.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UNCOMPRESSED_DISPATCH      = 0x41,
    LOWPAN_IPHC_DISPATCH       = 0x60,
    LOWPAN_IPHC_DISPATCH_MASK  = 0xe0,
    MCAST_HEADER_DISPATCH      = 0x48,
    MCAST_HEADER_DISPATCH_MASK = 0xfe,
    MESH_HEADER_DISPATCH       = 0x80,
    MESH_HEADER_DISPATCH_MASK  = 0xc0,
    FRAG_HEADER_DISPATCH       = 0xc0,
    FRAG_N_FLAG                = 0x20,
    FRAG_HEADER_DISPATCH_MASK  = 0xc0,
};

/* mesh header control field format */
/* 2bits | 1bit | 3bits | 2bits |    2bits   | 2bits | 1bit | 2bits | 1 bit  | 5 bits | 3 bits */
/* flags | type | hops  |  src  | dest netid |  dest |  ie  | dest2 |  sec   |  rsv   | ver    */

enum {
    MESH_FRAME_TYPE_MASK       = 0x20,
    MESH_FRAME_TYPE_OFFSET     = 5,

    MESH_HOPS_LEFT_MASK        = 0x1c,
    MESH_HOPS_LEFT_OFFSET      = 2,

    MESH_HEADER_SRC_MASK       = 0x03,
    MESH_HEADER_SRC_OFFSET     = 0,

    MESH_HEADER_DESTNETID_MASK = 0xc0,
    MESH_HEADER_DESTNETID_OFFSET = 6,
    MESH_HEADER_DEST_MASK      = 0x30,
    MESH_HEADER_DEST_OFFSET    = 4,
    MESH_HEADER_IES_MASK       = 0x08,
    MESH_HEADER_IES_OFFSET     = 3,
    MESH_HEADER_DEST2_MASK     = 0x06,
    MESH_HEADER_DEST2_OFFSET   = 1,
    MESH_HEADER_SEC_MASK       = 0x01,
    MESH_HEADER_SEC_OFFSET     = 0,

    MESH_HEADER_VER_MASK       = 0x7,
};

enum {
    MESH_FRAME_TYPE_DATA = 0x00,
    MESH_FRAME_TYPE_CMD  = 0x01,
};

enum {
    NO_ADDR_MODE    = 0,
    SHORT_ADDR_MODE = 1,
    EXT_ADDR_MODE   = 2,
    BCAST_ADDR_MODE = 3,
};

enum {
    DISABLE_SEC = 0,
    ENABLE_SEC = 1,
};

enum {
    NO_DEST_NETID    = 0,
    BCAST_DEST_NETID = 1,
    SUB_DEST_NETID   = 2,
    DEST_NETID       = 3,
};

enum {
    FORWARD_HOP_LIMIT = 6,
};

enum {
    MAIN_NETID_MASK = 0xff00,
    SUB_NETID_MASK = 0x00ff,
};

#define get_main_netid(nid) ((nid) & MAIN_NETID_MASK)
#define get_sub_netid(nid) ((nid) & SUB_NETID_MASK)
#define mk_sub_netid(m, s) ((uint16_t)(get_main_netid(m) | (s)))
#define is_subnet(nid) ((nid) & SUB_NETID_MASK)
#define get_leader_sid(nid) (((nid) & SUB_NETID_MASK) >> 2)
#define is_same_subnet(nid1, nid2) (get_sub_netid(nid1) == get_sub_netid(nid2))
#define is_same_mainnet(nid1, nid2) (get_main_netid(nid1) == get_main_netid(nid2))
#define is_unique_netid(netid) (netid != BCAST_NETID && netid != INVALID_NETID)

typedef struct mesh_header_control_s {
    uint8_t control[3];
} __attribute__((packed)) mesh_header_control_t;

typedef struct mesh_short_addr_s {
    uint16_t addr;
} __attribute__((packed)) mesh_short_addr_t;

typedef struct mesh_ext_addr_s {
    uint8_t addr[EXT_ADDR_SIZE];
} __attribute__((packed)) mesh_ext_addr_t;

typedef struct mesh_netid_s {
    uint16_t netid;
} __attribute__((packed)) mesh_netid_t;

typedef struct mesh_subnetid_s {
    uint8_t netid;
} __attribute__((packed)) mesh_subnetid_t;

typedef struct mesh_header_frame_counter_s {
    uint32_t frame_counter;
} __attribute__((packed)) mesh_header_frame_counter_t;

enum {
    HEADER_IES_FLAG      = 1 << 0,
    INSERT_MCAST_FLAG    = 1 << 7,
    ENCRYPT_ENABLE_FLAG  = 1 << 9,

    INSERT_MESH_HEADER   = 1 << 15,
};

ur_error_t mf_init(void);
message_t *mf_build_message(uint8_t type, uint8_t cmd_type, uint8_t *data,
                            uint16_t len, uint8_t debug);
ur_error_t mf_send_message(message_t *message);
const ur_link_stats_t *mf_get_stats(hal_context_t *hal);

#ifdef __cplusplus
}
#endif

#endif  /* UR_MESH_FORWARDER_H */
