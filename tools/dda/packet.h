/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef DDA_PACKET_H
#define DDA_PACKET_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "config_parser.h"

#define DFL_PORT_MASTER 34567
#define DFL_PORT_AGENT  2357
#define MASTER_ID 0x0
#define BCAST_ID -1ULL

/*
 * addressing mode:
 * 0 - special, master
 * 1,- nodes attached to master
 */

typedef struct {
    uint8_t type;
    uint8_t len;
    uint8_t value[0];
} __attribute__((packed)) msg_elm_t;

typedef union {
    struct {
        uint8_t channel;
        uint8_t rssi;
        uint8_t media_type;
        uint8_t key_index;
        uint8_t pad[4];
    } mesh;
    uint64_t opaque;
} cmd_priv_t;

typedef struct {
    struct timeval tv;
    uint64_t dst;
    uint64_t src;
    uint16_t type;
    uint16_t cmd;
    uint16_t payload_len;
    uint32_t sequence;
    cmd_priv_t cmd_priv;
} __attribute__((packed)) ipc_msg_t;

static inline ipc_msg_t *alloc_msg(int type, int cmd, int csize, uint64_t dst, uint64_t src)
{
    ipc_msg_t *cmsg = malloc(csize);
    memset(cmsg, 0, csize);

    gettimeofday(&cmsg->tv, NULL);
    cmsg->dst = dst;
    cmsg->src = src;
    cmsg->type = type;
    cmsg->cmd = cmd;
    cmsg->payload_len = csize - sizeof(ipc_msg_t);

    return cmsg;
}

typedef struct {
    uint64_t agent_id;
    uint64_t mac_addr;
} req_cmd_mgmt_device_up_t;

typedef struct {
    uint32_t sid;
    uint32_t netid;
} agent_status_t;

void dda_handle_message(ipc_msg_t *msg);
#include "msg.h"
#endif
