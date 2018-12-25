/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WORK_H
#define WORK_H
#include "atomic.h"
#include "zephyr.h"
#include "queue.h"

struct k_work_q {
    struct k_queue queue;
};

int k_work_q_start();

enum {
    K_WORK_STATE_PENDING,
};
struct k_work;
/* work define*/
typedef void (*k_work_handler_t)(struct k_work *work);

struct k_work {
    void *_reserved;
    k_work_handler_t handler;
    atomic_t flags[1];
    uint32_t start_ms;
    uint32_t timeout;
};

#define _K_WORK_INITIALIZER(work_handler) \
        { \
        ._reserved = NULL, \
        .handler = work_handler, \
        .flags = { 0 } \
        }

#define K_WORK_INITIALIZER DEPRECATED_MACRO _K_WORK_INITIALIZER

int k_work_init(struct k_work *work, k_work_handler_t handler);
void k_work_submit(struct k_work *work);


/*delay work define*/
struct k_delayed_work {
    struct k_work work;
};

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);
int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay);
int k_delayed_work_cancel(struct k_delayed_work *work);
s32_t k_delayed_work_remaining_get(struct k_delayed_work *work);

#endif /* WORK_H */
