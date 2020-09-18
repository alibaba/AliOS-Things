/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef _BT_HAL_OS_H_
#define _BT_HAL_OS_H_

#include <aos/kernel.h>

typedef aos_task_t      hal_task_t;
typedef aos_timer_t     hal_timer_t;
typedef aos_sem_t       hal_sem_t;
typedef aos_queue_t     hal_queue_t;

int hal_os_thread_new(hal_task_t *p_task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio);
int hal_os_thread_delete(const char *name);

int hal_os_create_timer(hal_timer_t *timer_hdl, void (*fn)(void *, void *), void *args, int32_t timer_expire);
int hal_os_timer_start(hal_timer_t *timer_hdl);
int hal_os_timer_stop(hal_timer_t *timer_hdl);
int hal_os_timer_is_valid(void *timer);
int hal_os_timer_change(void *timer, int ms);
int hal_os_timer_change_once(void *timer, int ms);
void hal_os_free_timer(void *timer);

int hal_os_sem_new(hal_sem_t *sem, uint32_t max_count, uint32_t init_count);
void hal_os_sem_give(hal_sem_t *sem);
int hal_os_sem_take(hal_sem_t *sem, uint32_t timeout);
void hal_os_sem_free(hal_sem_t *sem);
int hal_os_sem_is_valid(hal_sem_t *sem);

int hal_os_queue_new(hal_queue_t *queue, void *buf, unsigned int size, int max_msg);
void hal_os_queue_free(hal_queue_t *queue);
int hal_os_queue_send(hal_queue_t *queue, void *msg, unsigned int size);
int hal_os_queue_recv(hal_queue_t *queue, unsigned int timeout, void *msg,
                   unsigned int *size);

void hal_os_delay(int ms);
long long hal_now_ms(void);
void * hal_malloc(int size);
void hal_free(void *data);

#endif
