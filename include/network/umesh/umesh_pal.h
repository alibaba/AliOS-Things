/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_PAL_H
#define UMESH_PAL_H

#include <stdint.h>

int umesh_pal_kv_get(const char *key, void *buf, int *len);
int umesh_pal_kv_set(const char *key, void *buf, int len, int sync);
void *umesh_pal_malloc(int sz);
void umesh_pal_free(void *);
uint32_t umesh_pal_now_ms(void);
uint32_t umesh_pal_get_timestamp(void);
void umesh_pal_set_timestamp(uint32_t timestamp);
void umesh_pal_post_event(int code, unsigned long value);
void umesh_pal_log(const char *fmt, ...);

typedef long pal_sem_hdl_t;
int umesh_pal_sem_new(pal_sem_hdl_t *hdl, int count);
int umesh_pal_sem_wait(pal_sem_hdl_t *hdl, int ms);
void umesh_pal_sem_signal(pal_sem_hdl_t *hdl);
void umesh_pal_sem_free(pal_sem_hdl_t *hdl);

int umesh_pal_schedule_call(void (*task)(void *), void *arg);
int umesh_pal_post_delayed_action(int ms, void (*handler)(void *arg), void *arg);
void umesh_pal_cancel_delayed_action(int ms, void (*handler)(void *arg), void *arg);
void umesh_pal_task_init(void);
void umesh_pal_task_entry(void);

void umesh_pal_init(void);
void umesh_pal_ready(void);

void umesh_pal_radio_sleep(void);
void umesh_pal_radio_wakeup(void);
#endif
