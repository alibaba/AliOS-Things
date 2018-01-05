/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TIMER_H
#define TIMER_H

#if defined(__cplusplus)
extern "C"
{
#endif

typedef void (* timeout_handler)(void *arg);

struct mtimer {
    struct mtimer *next;
    uint32_t time;
    timeout_handler h;
    void *arg;
};

struct mtimer *mtimer_start(uint32_t msecs, timeout_handler handler, void *arg);

void mtimer_stop(struct mtimer *t);

void mtimer_mbox_fetch(k_mbox_t *mbox, void **msg);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H */

