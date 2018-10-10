/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "mdal_hal_os.h"

#include "iotx_log.h"

#define MDAL_OS_TAG "MDAL_HAL_OS"
#define MDAL_LOGE(format, ...) log_err(MDAL_OS_TAG, format, ##__VA_ARGS__)

static mdal_os_t *g_mdal_hal_os = NULL;

int mdal_os_resgiter(mdal_os_t *hal_os)
{
    if (NULL == hal_os) {
        MDAL_LOGE("mdal hal os register invalid input\n");
        return -1;
    }

    if (NULL != g_mdal_hal_os) {
        MDAL_LOGE("mdal hal os have already registered\n");
        return -1;
    }

    g_mdal_hal_os = hal_os;

    return 0;
}

void *mdal_malloc(unsigned int size)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return NULL;
    }

    if (NULL == g_mdal_hal_os->hal_malloc) {
        MDAL_LOGE("hal_malloc function in mdal hal os is null \n");
        return NULL;
    }

    return g_mdal_hal_os->hal_malloc(size);
}

void mdal_free(void *ptr)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_free) {
        MDAL_LOGE("hal_free function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_free(ptr);
}

long long mdal_uptime_ms(void)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return -1;
    }

    if (NULL == g_mdal_hal_os->hal_uptime_ms) {
        MDAL_LOGE("hal_uptime_ms function in mdal hal os is null \n");
        return -1;
    }

    return g_mdal_hal_os->hal_uptime_ms();
}

void mdal_sleep_ms(unsigned int ms)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_sleep_ms) {
        MDAL_LOGE("hal_sleep_ms function in mdal hal os is null \n");
        return;
    }

    return g_mdal_hal_os->hal_sleep_ms(ms);
}

void *mdal_mutex_create(void)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return NULL;
    }

    if (NULL == g_mdal_hal_os->hal_mutex_create) {
        MDAL_LOGE("hal_mutex_create function in mdal hal os is null \n");
        return NULL;
    }

    return g_mdal_hal_os->hal_mutex_create();
}

void mdal_mutex_destroy(void *mutex)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_mutex_destroy) {
        MDAL_LOGE("hal_mutex_destroy function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_mutex_destroy(mutex);
}

void mdal_mutex_lock(void *mutex)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_mutex_lock) {
        MDAL_LOGE("hal_mutex_lock function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_mutex_lock(mutex);
}

void mdal_mutex_unlock(void *mutex)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_mutex_unlock) {
        MDAL_LOGE("hal_mutex_unlock function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_mutex_unlock(mutex);
}

int mdal_mutex_valid(void *mutex)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return 0;
    }

    if (NULL == g_mdal_hal_os->hal_mutex_valid) {
        MDAL_LOGE("hal_mutex_valid function in mdal hal os is null \n");
        return 0;
    }

    return g_mdal_hal_os->hal_mutex_valid(mutex);
}

void *mdal_semaphore_create(void)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return NULL;
    }

    if (NULL == g_mdal_hal_os->hal_semaphore_create) {
        MDAL_LOGE("hal_semaphore_create function in mdal hal os is null \n");
        return NULL;
    }

    return g_mdal_hal_os->hal_semaphore_create();
}

void mdal_semaphore_destroy(void *sem)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_semaphore_destroy) {
        MDAL_LOGE("hal_semaphore_destroy function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_semaphore_destroy(sem);
}

int mdal_semaphore_wait(void *sem, unsigned int timeout_ms)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return -1;
    }

    if (NULL == g_mdal_hal_os->hal_semaphore_wait) {
        MDAL_LOGE("hal_semaphore_wait function in mdal hal os is null \n");
        return -1;
    }

    return g_mdal_hal_os->hal_semaphore_wait(sem, timeout_ms);
}

void mdal_semaphore_post(void *sem)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_semaphore_post) {
        MDAL_LOGE("hal_semaphore_post function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_semaphore_post(sem);
}

int mdal_semaphore_valid(void *sem)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return 0;
    }

    if (NULL == g_mdal_hal_os->hal_semaphore_valid) {
        MDAL_LOGE("hal_semaphore_valid function in mdal hal os is null \n");
        return 0;
    }

    return g_mdal_hal_os->hal_semaphore_valid(sem);
}

void *mdal_queue_create(void *buf, unsigned int size, int max_msg)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return NULL;
    }

    if (NULL == g_mdal_hal_os->hal_queue_create) {
        MDAL_LOGE("hal_queue_create function in mdal hal os is null \n");
        return NULL;
    }

    return g_mdal_hal_os->hal_queue_create(buf, size, max_msg);
}

void mdal_queue_destroy(void *queue)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return;
    }

    if (NULL == g_mdal_hal_os->hal_queue_destroy) {
        MDAL_LOGE("hal_queue_destroy function in mdal hal os is null \n");
        return;
    }

    g_mdal_hal_os->hal_queue_destroy(queue);
}

int mdal_queue_send(void *queue, void *msg, unsigned int size)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return -1;
    }

    if (NULL == g_mdal_hal_os->hal_queue_send) {
        MDAL_LOGE("hal_queue_send function in mdal hal os is null \n");
        return -1;
    }

    return g_mdal_hal_os->hal_queue_send(queue, msg, size);
}

int mdal_queue_recv(void *queue, unsigned int ms, void *msg,
                    unsigned int *size)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return -1;
    }

    if (NULL == g_mdal_hal_os->hal_queue_recv) {
        MDAL_LOGE("hal_queue_recv function in mdal hal os is null \n");
        return -1;
    }

    return g_mdal_hal_os->hal_queue_recv(queue, ms, msg, size);
}

void *mdal_queue_buf_ptr(void *queue)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return NULL;
    }

    if (NULL == g_mdal_hal_os->hal_queue_buf_ptr) {
        MDAL_LOGE("hal_queue_buf_ptr function in mdal hal os is null \n");
        return NULL;
    }

    return g_mdal_hal_os->hal_queue_buf_ptr(queue);
}

int mdal_queue_valid(void *queue)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return 0;
    }

    if (NULL == g_mdal_hal_os->hal_queue_valid) {
        MDAL_LOGE("hal_queue_valid function in mdal hal os is null \n");
        return 0;
    }

    return g_mdal_hal_os->hal_queue_valid(queue);
}

int mdal_task_new_ext(void **task_hdl, char *name, void (*fn)(void *),
                      void *arg, int stack_size, int prio)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return -1;
    }

    if (NULL == g_mdal_hal_os->hal_task_new_ext) {
        MDAL_LOGE("hal_task_new_ext function in mdal hal os is null \n");
        return -1;
    }

    return g_mdal_hal_os->hal_task_new_ext(task_hdl, name, fn, arg, stack_size, prio);
}

int mdal_get_task_default_priority(void)
{
    if (NULL == g_mdal_hal_os) {
        MDAL_LOGE("there is no mdal hal os registered yet \n");
        return -1;
    }

    if (NULL == g_mdal_hal_os->hal_get_task_default_priority) {
        MDAL_LOGE("hal_get_task_default_priority function in mdal hal os is null \n");
        return -1;
    }

    return g_mdal_hal_os->hal_get_task_default_priority();
}
