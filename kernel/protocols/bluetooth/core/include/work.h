/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WORK_H
#define WORK_H
#include "atomic.h"

#if defined(__cplusplus)
extern "C"
{
#endif

struct k_work_q {
    struct k_fifo fifo;
};

int k_work_q_start(const char *name, uint32_t *stack, uint32_t stack_size, int prio);

enum {
    K_WORK_STATE_PENDING,
};
struct k_work;
/* work define*/
typedef void (*k_work_handler_t)(struct k_work *work);
struct k_work {
    k_work_handler_t handler;
    atomic_t flags[1];
};

int k_work_init(struct k_work *work, k_work_handler_t handler);
void k_work_submit(struct k_work *work);

/*delay work define*/
struct k_delayed_work {
    struct k_work work;
    struct k_work_q *work_q;
    k_timer_t timer;
};

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);
int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay);
int k_delayed_work_cancel(struct k_delayed_work *work);


#ifdef __cplusplus
}
#endif

#endif /* WORK_H */

