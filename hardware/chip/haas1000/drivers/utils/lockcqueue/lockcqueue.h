/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/***
 * lock cqueue
 * YuLongWang @2015
*/

#ifndef LOCKCQUEUE_H
#define LOCKCQUEUE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "cqueue.h"

struct lockcqueue {
    CQueue cqueue;
    osMutexId queue_mutex_id;
    uint32_t os_mutex_cb_name[3];
    osMutexDef_t os_mutex_def_name;
};

int lockcqueue_init(struct lockcqueue *q, uint32_t size, uint8_t *buf);
int lockcqueue_enqueue(struct lockcqueue *q, uint8_t *buf, uint32_t size);
int lockcqueue_dequeue(struct lockcqueue *q, uint8_t *buf, uint32_t size);

#if defined(__cplusplus)
}
#endif

#endif /* LOCKCQUEUE_H */
