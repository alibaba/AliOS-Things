/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_MSG_QUEUE_H
#define WSF_MSG_QUEUE_H

#include "wsf_list.h"
#include "aos/aos.h"
#include "wsf_msg.h"

typedef struct wsf_request_queue_t {
    dlist_t list;
    void *mutex;
    aos_sem_t *psem;
    int length;
} wsf_request_queue_t;

void wsf_msg_queue_init(wsf_request_queue_t **ppqueue);
void wsf_msg_queue_flush(wsf_request_queue_t *req_queue);
void wsf_msg_queue_destroy(wsf_request_queue_t *pqueue);
void wsf_request_queue_init(wsf_request_queue_t *req_queue);
int wsf_request_queue_push(wsf_request_queue_t *req_queue,
                           wsf_request_node_t *node);
int wsf_request_queue_pop(wsf_request_queue_t *req_queue,
                          wsf_request_node_t *req_node);
wsf_request_node_t *wsf_request_queue_trigger(wsf_request_queue_t *req_queue,
                                              wsf_msg_t *rsp);
void wsf_msg_request_queue_destroy(wsf_request_queue_t *req_queue);
#endif
