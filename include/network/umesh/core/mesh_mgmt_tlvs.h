/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_MM_TLVS_H
#define UR_MM_TLVS_H

#include "ip/ip.h"
#ifdef CONFIG_AOS_MESH_AUTH
#include "auth_dot1x.h"
#endif

/* mesh managememt TLV types. */
enum {
    TYPE_LENGTH_FIXED_FLAG = 0x80,
};

typedef enum {
    TYPE_VERSION           = 0x80,  /* version TLV */
    TYPE_SRC_UUID          = 0x81,  /* source UUID TLV */
    TYPE_DEST_UUID         = 0x82,  /* dest UUID TLV */
    TYPE_ATTACH_NODE_UUID  = 0x83,  /* attach node UUID TLV */
    TYPE_SRC_SID           = 0x84,  /* source sid TLV */
    TYPE_DEST_SID          = 0x85,  /* dest sid TLV */
    TYPE_ALLOCATE_SID      = 0x86,  /* newly allocated sid TLV */
    TYPE_ATTACH_NODE_SID   = 0x87,  /* attach node sid TLV */
    TYPE_MODE              = 0x88,  /* node mode TLV */
    TYPE_NETWORK_SIZE      = 0x89,  /* network size TLV */
    TYPE_PATH_COST         = 0x8a,  /* path cost TLV */
    TYPE_LINK_COST         = 0x8b,  /* link cost TLV */
    TYPE_LOWPAN_CONTEXT    = 0x8c,  /* lowpan context TLV */
    TYPE_ROUTE_TYPE        = 0x8d,  /* route type TLV */
    TYPE_SCAN_MASK         = 0x8e,  /* scan mask TLV */
    TYPE_SRC_MAC_ADDR      = 0x8f,  /* src MAC address TLV */
    TYPE_WEIGHT            = 0x92,  /* mesh partition weight */
    TYPE_NODE_TYPE         = 0x93,  /* node type TLV */
    TYPE_NETWORK_INFO      = 0x94,  /* packet sequence number */
    TYPE_MCAST_ADDR        = 0x95,  /* multicast address */
    TYPE_TARGET_UUID       = 0x96,  /* target UUID TLV */
    TYPE_TARGET_SID        = 0x97,  /* target sid TLV */
    TYPE_REVERSE_RSSI      = 0x98,  /* reverse RSSI */
    TYPE_FORWARD_RSSI      = 0x99,  /* forward RSSI */
    TYPE_SID_TYPE          = 0x9a,  /* SID allocate type */
    TYPE_ADDR_QUERY        = 0x9b,  /* address query type */
    TYPE_NODE_ID           = 0x9c,  /* node id */
    TYPE_ATTACH_NODE_ID    = 0x9d,  /* attach node id */
    TYPE_DEF_HAL_TYPE      = 0x9e,  /* default hal type */
    TYPE_TIMESTAMP         = 0x9f,  /* timestamp */
    TYPE_SYMMETRIC_KEY     = 0xa0,  /* symmetric key */
    TYPE_SSID_INFO         = 0xa1,  /* structured sid info */
    TYPE_STATE_FLAGS       = 0xa2,  /* node state flags */
    TYPE_UCAST_CHANNEL     = 0xa3,  /* ucast channel */
    TYPE_BCAST_CHANNEL     = 0xa4,  /* bcast channel */
    TYPE_TIME_SLOT         = 0xa5,  /* time slot */
    TYPE_BUFQUEUE_SIZE     = 0xa6,  /* buffer queue size */
#ifdef CONFIG_AOS_MESH_AUTH
    TYPE_NODE_ID2          = 0xa7,  /* node ID2 */
    TYPE_ID2_CHALLENGE     = 0xa8,  /* ID2 challenge number */
    TYPE_ID2_AUTH_RESULT   = 0xa9,  /* ID2 auth result */
    TYPE_ID2_AUTH_CODE     = 0x7e,  /* ID2 auth code */
#endif
#ifdef CONFIG_AOS_MESH_BCAST
    TYPE_BCAST_DATA        = 0x7d,  /* broadcast data TLV */
    TYPE_KEEP_ALIVE        = 0xaa,  /* keep alive TLV */
#endif
    TYPE_MESH_PREFIX       = 0x0,   /* mesh prefix TLV */
    TYPE_TLV_REQUEST       = 0x1,   /* TLV requests TLV */
    TYPE_HEADER_IES_TERMINATOR = 0x7f,  /* header TLVs terminator */
    TYPE_INVALID           = 0xff,
} mm_tlv_type_t;

typedef struct mesh_mgmt_tlv_s {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) mm_tlv_t;

typedef struct mesh_mgmt_tv_s {
    uint8_t type;
} __attribute__((packed)) mm_tv_t;

typedef struct mesh_mgmt_uuid_tv_s {
    mm_tv_t base;
    uint8_t uuid[8];
} __attribute__((packed)) mm_uuid_tv_t;

typedef struct mesh_mgmt_sid_tv_s {
    mm_tv_t base;
    uint16_t sid;
} __attribute__((packed)) mm_sid_tv_t;

typedef struct mesh_mgmt_mode_tv_s {
    mm_tv_t base;
    uint8_t mode;
} __attribute__((packed)) mm_mode_tv_t;

typedef struct mesh_mgmt_size_tv_s {
    mm_tv_t base;
    uint16_t size;
} __attribute__((packed)) mm_size_tv_t;

typedef struct mesh_mgmt_scan_mask_tv_s {
    mm_tv_t base;
    uint8_t mask;
} __attribute__((packed)) mm_scan_mask_tv_t;

typedef struct mesh_mgmt_cost_tv_s {
    mm_tv_t base;
    uint16_t cost;
} __attribute__((packed)) mm_cost_tv_t;

typedef struct mesh_mgmt_mac_addr_tv_s {
    mm_tv_t base;
    mac_address_t addr;
} __attribute__((packed)) mm_mac_addr_tv_t;

typedef struct mesh_mgmt_weight_tv_s {
    mm_tv_t base;
    uint16_t weight;
} __attribute__((packed)) mm_weight_tv_t;

typedef struct mesh_mgmt_node_type_tv_s {
    mm_tv_t base;
    uint8_t type;
} __attribute__((packed)) mm_node_type_tv_t;

typedef struct mesh_mgmt_netinfo_tv_s {
    mm_tv_t base;
    uint8_t stable_version;
    uint8_t version;
    uint16_t size: 13;
    uint16_t subnet_size_1: 3;
    uint8_t subnet_size_2;
    uint8_t leader_mode;
} __attribute__((packed)) mm_netinfo_tv_t;

typedef struct mesh_mgmt_ssid_info_tv_s {
    mm_tv_t base;
    uint16_t child_num;
    uint8_t free_slots;
} __attribute__((packed)) mm_ssid_info_tv_t;

typedef struct mesh_mgmt_sid_type_tv_s {
    mm_tv_t base;
    uint8_t type;
} __attribute__((packed)) mm_sid_type_tv_t;

typedef struct mesh_mgmt_addr_query_tv_s {
    mm_tv_t base;
    uint8_t query_type;
} __attribute__((packed)) mm_addr_query_tv_t;

typedef struct mesh_mgmt_node_id_tv_s {
    mm_tv_t base;
    uint16_t meshnetid;
    uint16_t sid;
    uint8_t mode;
} __attribute__((packed)) mm_node_id_tv_t;

typedef struct mesh_mgmt_mcast_addr_tv_s {
    mm_tv_t base;
    ur_ip6_addr_t mcast;
} __attribute__((packed)) mm_mcast_addr_tv_t;

typedef struct mesh_mgmt_rssi_tv_s {
    mm_tv_t base;
    int8_t rssi;
} __attribute__((packed)) mm_rssi_tv_t;

typedef struct mesh_mgmt_hal_type_s {
    mm_tv_t base;
    uint8_t type;
} __attribute__((packed)) mm_hal_type_tv_t;

typedef struct mesh_mgmt_timestamp_tv_s {
    mm_tv_t base;
    uint32_t timestamp;
} __attribute__((packed)) mm_timestamp_tv_t;

typedef struct mesh_mgmt_symmetric_key_tv_s {
    mm_tv_t base;
    uint8_t symmetric_key[16];
} __attribute__((packed)) mm_symmetric_key_tv_t;

typedef struct mesh_mgmt_state_flags_tv_s {
    mm_tv_t base;
    uint8_t flags;
} __attribute__((packed)) mm_state_flags_tv_t;

typedef struct mesh_mgmt_channel_tv_s {
    mm_tv_t base;
    uint8_t channel;
} __attribute__((packed)) mm_channel_tv_t;

typedef struct mesh_mgmt_time_slot_s {
    mm_tv_t base;
    uint8_t slot_num;
    uint32_t offset;
} __attribute__((packed)) mm_time_slot_tv_t;

typedef struct mesh_mgmt_bufqueue_size_tv_s {
    mm_tv_t base;
    int16_t size;
} __attribute__((packed)) mm_bufqueue_size_tv_t;

typedef struct mesh_mgmt_prefix_tlv_s {
    mm_tlv_t base;
    ur_ip6_prefix_t prefix;
} __attribute__((packed)) mm_prefix_tlv_t;

typedef struct mesh_mgmt_tlv_request_tlv_s {
    mm_tlv_t base;
} __attribute__((packed)) mm_tlv_request_tlv_t;

#ifdef CONFIG_AOS_MESH_AUTH
typedef struct mesh_mgmt_id2_auth_code_s {
    mm_tlv_t base;
} __attribute__((packed)) mm_id2_auth_code_tlv_t;

typedef struct mesh_mgmt_node_id2_tv_s {
    mm_tv_t base;
    uint8_t device_id[ID2_ID_LEN];
} __attribute__((packed)) mm_node_id2_tv_t;

typedef struct mesh_mgmt_id2_challenge_s {
    mm_tv_t base;
    uint8_t challenge[ID2_CHALLENGE_LEN];
} __attribute__((packed)) mm_id2_challenge_tv_t;

typedef struct mesh_mgmt_id2_auth_result_s {
    mm_tv_t base;
    bool result;
} __attribute__((packed)) mm_id2_auth_result_tv_t;
#endif

#ifdef CONFIG_AOS_MESH_BCAST
typedef struct mesh_mgmt_bcast_data_s {
    mm_tlv_t base;
} __attribute__((packed)) mm_bcast_data_tlv_t;

typedef struct mesh_leader_keep_alive_s {
    mm_tv_t base;
    uint32_t seq_num;
} __attribute__((packed)) mm_keep_alive_tv_t;
#endif

#endif  /* UR_MM_TLVS_H */
