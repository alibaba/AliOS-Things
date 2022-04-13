/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UTIMER_H
#define UTIMER_H

#include <stdint.h>
#include <aos/list.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*timer_cb_t)(void *timer, void *arg);

typedef struct {
    dlist_t       timer_list;
    dlist_t      *to_head;
    const name_t *name;
    timer_cb_t    cb;
    void         *timer_cb_arg;
    uint64_t      match;        /**< Expected timeout point */
    uint64_t      init_count;
    uint64_t      round_ms;  /**< Timer period */
    void         *priv;
    kobj_type_t   obj_type;
    uint8_t       timer_state;
    uint8_t       mm_alloc_flag;
} utimer_t;

typedef struct {
    utimer_t   *timer;
    uint8_t     cb_num;         /**< TIMER_CMD_START/STOP/... */
    uint64_t    first;

    union {
        uint64_t  round;
        void      *arg;
    } u;
} u_timer_queue_cb;


int utimer_create(utimer_t *timer, const char *name, timer_cb_t cb,
                  uint64_t first, uint64_t round, void *arg,
                  uint8_t auto_run);

int utimer_dyn_create(utimer_t **timer, const char *name,
                      timer_cb_t cb, uint64_t first, uint64_t round,
                      void *arg, uint8_t auto_run);

int utimer_dyn_del(utimer_t *timer);

int utimer_start(utimer_t *timer);

int utimer_stop(utimer_t *timer);

int utimer_del(utimer_t *timer);

int utimer_change(utimer_t *timer, uint64_t first, uint64_t round);

int utimer_arg_change(utimer_t *timer, void *arg);

int utimer_arg_change_auto(utimer_t *timer, void *arg);

int timer_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio, size_t msg_num);

#ifdef __cplusplus
}
#endif

#endif /* UTIMER_H */

