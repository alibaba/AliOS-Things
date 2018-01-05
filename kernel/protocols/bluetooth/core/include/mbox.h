/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MBOX_H
#define MBOX_H

#if defined(__cplusplus)
extern "C"
{
#endif

#define K_MBOX_SIZE 128

#define SYS_ARCH_TIMEOUT 0xffffffffUL
#define SYS_MBOX_EMPTY SYS_ARCH_TIMEOUT

typedef struct k_mbox {
    int first, last;
    void *msgs[K_MBOX_SIZE];
    struct k_sem not_empty;
    struct k_sem not_full;
    struct k_sem mutex;
    int wait_send;
} k_mbox_t;

int k_mbox_new(k_mbox_t **mb, int size);
void k_mbox_free(k_mbox_t *mb);
void k_mbox_post(k_mbox_t *mb, void *msg);
int k_mbox_trypost(k_mbox_t *mb, void *msg);
int k_mbox_fetch(k_mbox_t *mb, void **msg, uint32_t timeout);
int k_mbox_tryfetch(k_mbox_t *mb, void **msg);

#ifdef __cplusplus
}
#endif

#endif /* MBOX_H */

