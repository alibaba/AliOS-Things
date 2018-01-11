/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_WORKER_H
#define WSF_WORKER_H

#include "aos/aos.h"
#include "wsf_client.h"
#include "wsf_config.h"

typedef struct request_msg_node {
    dlist_t list_head;
    uint32_t length;
    uint8_t msg[];
} request_msg_node_t;

#ifndef CONFIG_REQMSG_LENGTH
#define CONFIG_REQMSG_LENGTH (10)
#endif

void init_req_glist(void);
void deinit_req_glist(void);

wsf_code wsf_start_worker(wsf_config_t *config);
void wsf_wait_worker(void);
void wsf_stop_worker(void);

#endif
