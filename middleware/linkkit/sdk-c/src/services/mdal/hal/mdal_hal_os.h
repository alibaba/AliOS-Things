/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_HAL_OS_H_
#define _MDAL_HAL_OS_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MDAL_WAIT_INFINITE (~0)

typedef struct mdal_os_s
{
	void *(*hal_malloc)(unsigned int size);

    void (*hal_free)(void *ptr);

    long long (*hal_uptime_ms)(void);

    void (*hal_sleep_ms)(unsigned int ms);

    void *(*hal_mutex_create)(void);

    void (*hal_mutex_destroy)(void *mutex);

    void (*hal_mutex_lock)(void *mutex);

    void (*hal_mutex_unlock)(void *mutex);

    int (*hal_mutex_valid)(void *mutex);

    void *(*hal_semaphore_create)(void);

    void (*hal_semaphore_destroy)(void *sem);

    int (*hal_semaphore_wait)(void *sem, unsigned int timeout_ms);

    void (*hal_semaphore_post)(void *sem);

    int (*hal_semaphore_valid)(void *sem);

    void *(*hal_queue_create)(void *buf, unsigned int size, int max_msg);

    void (*hal_queue_destroy)(void *queue);

    int (*hal_queue_send)(void *queue, void *msg, unsigned int size);

    int (*hal_queue_recv)(void *queue, unsigned int ms, void *msg,
                          unsigned int *size);

    void *(*hal_queue_buf_ptr)(void *queue);

    int (*hal_queue_valid)(void *queue);
} mdal_os_t;

int mdal_os_resgiter(mdal_os_t *os);

void *mdal_malloc(unsigned int size);

void mdal_free(void *ptr);

long long mdal_uptime_ms(void);

void mdal_sleep_ms(unsigned int ms);

void *mdal_mutex_create(void);

void mdal_mutex_destroy(void *mutex);

void mdal_mutex_lock(void *mutex);

void mdal_mutex_unlock(void *mutex);

int mdal_mutex_valid(void *mutex);

void *mdal_semaphore_create(void);

void mdal_semaphore_destroy(void *sem);

int mdal_semaphore_wait(void *sem, unsigned int timeout_ms);

void mdal_semaphore_post(void *sem);

int mdal_semaphore_valid(void *sem);

void *mdal_queue_create(void *buf, unsigned int size, int max_msg);

void mdal_queue_destroy(void *queue);

int mdal_queue_send(void *queue, void *msg, unsigned int size);

int mdal_queue_recv(void *queue, unsigned int ms, void *msg,
                    unsigned int *size);

void *mdal_queue_buf_ptr(void *queue);

int mdal_queue_valid(void *queue);

#ifdef __cplusplus
}
#endif

#endif // _MDAL_HAL_OS_H_
