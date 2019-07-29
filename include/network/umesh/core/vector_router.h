/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_VECTOR_ROUTER_H
#define UR_VECTOR_ROUTER_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UNVISITED    = 0x00,
    VISITED      = 0x01,
};

enum {
    TOPOLOGY_SYNC_TIMEOUT = 800,
    STARTUP_TIMEOUT       = 2000,
    HEARTBEAT_TIMEOUT     = 20000, /* 20 seconds */
    MAX_HEARTBEAT_MESSAGE_INTERVAL = 12 * HEARTBEAT_TIMEOUT, /* 4 minutes */
    VERTEX_ALIVE_TIMEOUT  = 30 * HEARTBEAT_TIMEOUT, /* 10 minutes */
};

enum {
    STATUS_DOWN          = 0x00,
    STATUS_SYNC_TOPOLOGY = 0x01,
    STATUS_UP            = 0x02,
};

enum {
    TOPOLOGY_SYNC_IDLE                  = 0x00,
    TOPOLOGY_SYNC_CLIENT_RECEIVING_DATA = 0x01,
    TOPOLOGY_SYNC_SERVER_SENDING_VERTEX = 0x02,
    TOPOLOGY_SYNC_SERVER_SENDING_EDGE   = 0x03,
    TOPOLOGY_SYNC_SERVER_EXITING        = 0x04,
};

enum {
    COMMAND_TOPOLOGY_SYNC_REQUEST = 0x01,
    COMMAND_TOPOLOGY_SYNC_SELECT  = 0x02,
    COMMAND_TOPOLOGY_SYNC_DATA    = 0x03,
    COMMAND_TOPOLOGY_SYNC_ACK     = 0x04,
    COMMAND_VERTEX_UPDATE         = 0x11,
    COMMAND_VERTEX_DELETE         = 0x12,
    COMMAND_EDGE_UPDATE           = 0x20,
    COMMAND_HEARTBEAT             = 0x30,
};

enum {
    TYPE_NONE   = 0x00,
    TYPE_VERTEX = 0x01,
    TYPE_EDGE   = 0x02,
};

typedef struct router_command_s {
    uint8_t cmd;
    uint8_t seq;
    uint16_t sid;
} __attribute__((packed)) router_command_t;

typedef struct none_tv_s {
    uint8_t type;
} __attribute__((packed)) none_tv_t;

typedef struct vertex_tv_s {
    uint8_t type;
    uint16_t sid;
    uint8_t uuid[8];
} __attribute__((packed)) vertex_tv_t;

typedef struct edge_tv_s {
    uint8_t type;
    uint8_t cost;
    uint16_t src;
    uint16_t dst;
} __attribute__((packed)) edge_tv_t;

typedef struct edge_tuple_s {
    uint16_t dst;
    uint8_t cost;
} __attribute__((packed)) edge_tuple_t;

struct vertex_s;

typedef struct edge_s {
    uint8_t cost;
    struct vertex_s *dst;
    struct edge_s *next;
} edge_t;

typedef struct vertex_flag_s {
    unsigned int timeout : 7;
    unsigned int visit : 1;
} vertex_flag_t;

typedef struct vertex_s {
    slist_t next;
    uint16_t sid;
    uint8_t seq;
    vertex_flag_t flag; /* vertex flags */
    uint8_t uuid[8];
    uint16_t dist;      /* minimum distance from source to this vertex */
    uint16_t nexthop;   /* next hop to send to for reaching this node */
    edge_t *edges;    /* outgoing edges from this vertex */
    struct vertex_s *prevhop; /* previous hop in optimal path from source */
} vertex_t;

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_ROUTER_H */
