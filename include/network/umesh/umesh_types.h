/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_TYPES_H
#define UMESH_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#include "aos/list.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    MESH_VERSION_1 = 1,
};

typedef enum ur_error_s {
    UR_ERROR_NONE          = 0,
    UR_ERROR_FAIL          = 1,
    UR_ERROR_BUSY          = 2,
    UR_ERROR_DROP          = 3,
    UR_ERROR_MEM           = 4,
    UR_ERROR_ROUTE         = 5,
    UR_ERROR_PARSE         = 6,
    UR_ERROR_ADDRESS_QUERY = 7,
    UR_ERROR_BUFFER        = 8,
    UR_ERROR_INVALID_PARM  = 9,
} ur_error_t;

typedef enum media_type_s {
    MEDIA_TYPE_DFL  = 0,
    MEDIA_TYPE_WIFI = 1,
    MEDIA_TYPE_BLE  = 2,
    MEDIA_TYPE_15_4 = 3,
} media_type_t;

#ifndef NULL
#define NULL (void *)0
#endif

enum {
    UR_IP6_ADDR_SIZE   = 16,
    MESH_IP4_ADDR_SIZE = 4,
};

enum {
    SHORT_ADDR_SIZE = 2,
    EXT_ADDR_SIZE   = 8,
    EXT_NETID_SIZE  = 6,
};

typedef struct ur_ip6_addr_s {
    union {
        uint8_t m8[UR_IP6_ADDR_SIZE];
        uint16_t m16[UR_IP6_ADDR_SIZE / sizeof(uint16_t)];
        uint32_t m32[UR_IP6_ADDR_SIZE / sizeof(uint32_t)];
    };
} __attribute__((packed)) ur_ip6_addr_t;

typedef struct ur_ip4_addr_s {
    union {
        uint8_t m8[MESH_IP4_ADDR_SIZE];
        uint16_t m16[MESH_IP4_ADDR_SIZE / sizeof(uint16_t)];
        uint32_t m32;
    };
} __attribute__((packed)) ur_ip4_addr_t;

typedef struct ur_ip6_prefix_s {
    ur_ip6_addr_t prefix;
    uint8_t length;
} __attribute__((packed)) ur_ip6_prefix_t;

enum {
    UR_IP6_HLEN   = 40,
    MESH_IP4_HLEN = 20,
    UR_UDP_HLEN   = 8,
};

typedef struct mac_address_s {
    union {
        uint64_t value;
        uint16_t short_addr;
        uint8_t addr[EXT_ADDR_SIZE];
    };
    uint8_t len;
} mac_address_t;

typedef struct ur_addr_s {
    mac_address_t addr;
    uint16_t netid;
} ur_addr_t;

typedef struct umesh_extnetid_s {
    uint8_t netid[EXT_NETID_SIZE];
    uint8_t len;
} umesh_extnetid_t;

typedef struct frame_s {
    uint8_t *data;
    uint16_t len;
    uint8_t key_index;
} frame_t;

typedef struct frame_info_s {
    mac_address_t peer;
    uint8_t channel;
    int8_t rssi;
    int8_t key_index;
} frame_info_t;

typedef struct channel_s {
    uint16_t channel;
    uint16_t wifi_channel;
    uint16_t hal_ucast_channel;
    uint16_t hal_bcast_channel;
} channel_t;

struct pbuf;
typedef ur_error_t (* adapter_input_t)(struct pbuf *buf);
typedef ur_error_t (* adapter_interface_up_t)(void);
typedef ur_error_t (* adapter_interface_down_t)(void);
typedef ur_error_t (* adapter_interface_update_t)(void);
typedef struct ur_adapter_callback_s {
    slist_t next;
    adapter_input_t input;
    adapter_interface_up_t interface_up;
    adapter_interface_down_t interface_down;
    adapter_interface_update_t interface_update;
} ur_adapter_callback_t;

typedef struct frame_stats_s {
    uint32_t in_frames;
    uint32_t out_frames;
} frame_stats_t;

typedef struct ur_link_stats_s {
    uint32_t in_frames;
    uint32_t in_command;
    uint32_t in_data;
    uint32_t in_filterings;
    uint32_t in_drops;

    uint32_t out_frames;
    uint32_t out_command;
    uint32_t out_data;
    uint32_t out_errors;

    uint16_t send_queue_size;
    uint16_t recv_queue_size;

    bool sending;
    uint16_t sending_timeouts;
} ur_link_stats_t;

enum {
    UMESH_1,          // 0
    MESH_FORWARDER_1, // 1
    MESH_FORWARDER_2, // 2
    MESH_FORWARDER_3, // 3
    MESH_MGMT_1,      // 4
    MESH_MGMT_2,      // 5
    MESH_MGMT_3,      // 6
    MESH_MGMT_4,      // 7
    MESH_MGMT_5,      // 8
    MESH_MGMT_6,      // 9
    MESH_MGMT_7,      // 10
    ADDRESS_MGMT_1,   // 11
    ADDRESS_MGMT_2,   // 12
    ADDRESS_MGMT_3,   // 13
    ADDRESS_MGMT_4,   // 14
    NETWORK_MGMT_1,   // 15
    NETWORK_MGMT_2,   // 16
    LOWPAN6_2,        // 18
    LINK_MGMT_1,      // 19
    LINK_MGMT_2,      // 20
    LINK_MGMT_3,      // 21
    ROUTER_MGR_1,     // 22
    DIAGS_1,          // 23
    DIAGS_2,          // 24
    UT_MSG,           // 25
    UMESH_2,          // 26
    MSG_DEBUG_INFO_SIZE, // 27
};

typedef struct ur_message_stats_s {
    int16_t num;
    int16_t queue_fulls;
    int16_t mem_fails;
    int16_t pbuf_fails;
    int16_t size;

    int16_t debug_info[MSG_DEBUG_INFO_SIZE];
} ur_message_stats_t;

typedef struct ur_mem_stats_s {
    int32_t num;
} ur_mem_stats_t;

enum {
    WHITELIST_ENTRY_NUM = 16,
};

typedef enum node_mode_s {
    MODE_NONE     = 0x00,  // this is for testing that not joining net
    MODE_MOBILE   = 0x01,
    MODE_LOW_MASK = 0x0f,
    MODE_RX_ON    = 0x10,
    MODE_SUPER    = 0x20,
    MODE_LEADER   = 0x40,
    MODE_HI_MASK  = 0xf0,
} node_mode_t;

typedef enum node_state_s {
    DEVICE_STATE_DISABLED     = 0,
    DEVICE_STATE_DETACHED     = 1,
    DEVICE_STATE_ATTACHED     = 2,
    DEVICE_STATE_LEAF         = 3,
    DEVICE_STATE_LEADER       = 4,
    DEVICE_STATE_SUPER_ROUTER = 5,
    DEVICE_STATE_ROUTER       = 6,
} node_state_t;

typedef struct whitelist_entry_s {
    mac_address_t address;
    int8_t rssi;
    bool valid;
    bool constant_rssi;
} whitelist_entry_t;

/* mesh events code */
#define CODE_MESH_STARTED        1
#define CODE_MESH_ATTACHED       2
#define CODE_MESH_DETACHED       3
#define CODE_MESH_CONNECTED      4
#define CODE_MESH_DISCONNECTED   5
#define CODE_MESH_DATA_RECV      6
#define CODE_MESH_PSCHED_UP      7
#define CODE_MESH_ASCHED_UP      8
#define CODE_MESH_SCHED_DOWN     9

#ifdef __cplusplus
}
#endif

#endif  /* UMESH_TYPES_H */
