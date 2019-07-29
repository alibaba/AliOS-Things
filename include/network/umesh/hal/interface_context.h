/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_INTERFACE_CONTEXT_H
#define UR_INTERFACE_CONTEXT_H

#include "umesh_hal.h"
#include "umesh_types.h"
#include "core/topology.h"
#include "core/sid_allocator.h"
#include "utilities/message.h"
#include "utilities/timer.h"

typedef enum interface_state_s {
    INTERFACE_UP,
    INTERFACE_DOWN,
    INTERFACE_DOWN_PSID_CHANGED,  // parent sid changed
    INTERFACE_DOWN_PNETID_CHANGED,  // parent netid changed
    INTERFACE_DOWN_ATTACH_FAIL,  //  attaching fail
    INTERFACE_DOWN_DISCOVER_FAIL,  // discovering fail
    INTERFACE_DOWN_MESH_START,  // mesh started
    INTERFACE_DOWN_MESH_STOP,  // mesh stopped
    INTERFACE_DOWN_LEADER_MODE,  // leader mode is larger then local
    INTERFACE_DOWN_LEADER_REBOOT,  // leader reboot
    INTERFACE_DOWN_AUTH_FAILURE,  // auth fails
} interface_state_t;

typedef enum auth_state_s {
    AUTH_IDLE,
    AUTH_REQUEST_START,
    AUTH_RELAY_CHALLENGE,
    AUTH_RELAY_AUTH_CODE,
    AUTH_DONE,
} auth_state_t;

typedef struct channel_list_s {
    const uint8_t *channels;
    uint8_t num;
} channel_list_t;

typedef void (*auth_handler_t)(neighbor_t *nbr, bool result);

typedef struct network_data_s {
    uint8_t version;
    uint16_t size;
} network_data_t;

enum {
    CMD_QUEUE,
    DATA_QUEUE,
    PENDING_QUEUE,
    QUEUE_SIZE = 3,
};

enum {
    EVENTS_NUM = 2,
};

typedef struct router_cb_s {
    ur_error_t (*start)(void);
    ur_error_t (*stop)(void);
    ur_error_t (*handle_neighbor_updated)(neighbor_t *);
    ur_error_t (*handle_message_received)(const uint8_t *data,
                                          uint16_t length);
    ur_error_t (*handle_subscribe_event)(uint8_t event, uint8_t *data,
                                         uint8_t len);
    uint16_t (*get_next_hop_sid)(uint16_t dest_sid);
} router_cb;

typedef struct subscribe_events_s {
    uint8_t events[EVENTS_NUM];
    uint8_t num;
} subscribe_events_t;

struct network_context_s;
typedef struct router_s {
    uint8_t id;
    uint8_t sid_type;
    router_cb cb;
    subscribe_events_t events;
    slist_t next;
    struct network_context_s *network;
} router_t;

typedef struct frag_info_s {
    uint16_t tag;
    uint16_t offset;
} frag_info_t;

typedef struct hal_context_s {
    slist_t next;
    umesh_hal_module_t *module;
    channel_list_t channel_list;
    uint8_t def_channel;
    uint8_t channel;
    mac_address_t mac_addr;

    // queue
    message_queue_t send_queue[QUEUE_SIZE];
    message_queue_t recv_queue;
    ur_timer_t sending_timer;
    message_t *send_message;
    frag_info_t frag_info;
    frame_t frame;
    ur_link_stats_t link_stats;

    // neighbors
    slist_t neighbors_list;
    uint8_t neighbors_num;
    ur_timer_t update_nbr_timer;
    ur_timer_t link_quality_update_timer;

    // hal configurations
    uint32_t discovery_interval;
    uint32_t link_quality_update_interval;
    uint32_t neighbor_alive_interval;
    uint32_t advertisement_interval;
    uint32_t leader_delay_interval; // random delay before becoming leader
    uint32_t auth_request_interval;
    uint32_t auth_relay_interval;
    uint32_t auth_response_interval;

    int last_sent;  // 0 success, -1 fail
} hal_context_t;

typedef struct network_context_s {
    slist_t next;
    uint8_t index;
    uint8_t flags;

    hal_context_t *hal;
    interface_state_t state;
    uint16_t meshnetid;
    ur_timer_t advertisement_timer;
    ur_timer_t leader_delay_timer;
    network_data_t network_data;
    allocator_t sid_base;
    router_t *router;
} network_context_t;

#endif  /* UR_INTERFACE_CONTEXT_H */
