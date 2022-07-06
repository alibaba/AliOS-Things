/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <unistd.h>
#include <stdio.h>
#include "k_api.h"
#include "kv_conf.h"
#include "kv_adapt.h"
#include <aos/flashpart.h>
#include <aos/hal/flash.h>

static aos_flashpart_ref_t kv_flashpart_ref = AOS_DEV_REF_INIT_VAL;

static aos_status_t kv_flash_open(void)
{
    return aos_flashpart_get(&kv_flashpart_ref, KV_PARTITION);
}

int32_t kv_flash_read(uint32_t offset, void *buf, uint32_t nbytes)
{
    aos_status_t r;

    if (!aos_dev_ref_is_valid(&kv_flashpart_ref) && kv_flash_open())
        return -1;

    r = aos_flashpart_read(&kv_flashpart_ref, offset, buf, nbytes);

    return (r >= 0 && r < AOS_FLASH_ECC_ERROR) ? 0 : -1;
}

int32_t kv_flash_write(uint32_t offset, void *buf, uint32_t nbytes)
{
    if (!aos_dev_ref_is_valid(&kv_flashpart_ref) && kv_flash_open())
        return -1;

    return aos_flashpart_write(&kv_flashpart_ref, offset, buf, nbytes) ? -1 : 0;
}

int32_t kv_flash_erase(uint32_t offset, uint32_t size)
{
    if (!aos_dev_ref_is_valid(&kv_flashpart_ref) && kv_flash_open())
        return -1;

    return aos_flashpart_erase(&kv_flashpart_ref, offset, size) ? -1 : 0;
}

void *kv_lock_create(void)
{
    int32_t   ret;
    kmutex_t *m;

    m = krhino_mm_alloc(sizeof(kmutex_t));
    if (m == NULL) {
        return NULL;
    }

    ret = krhino_mutex_create(m, "KV");
    if (ret != RHINO_SUCCESS) {
        krhino_mm_free(m);
        return NULL;
    }

    return (void *)m;
}

int32_t kv_lock_free(void *lock)
{
    int32_t ret;

    kmutex_t *m = (kmutex_t *)lock;

    if (m == NULL) {
        return -1;
    }

    ret = krhino_mutex_del(m);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    krhino_mm_free(m);

    return ret;
}

int32_t kv_lock(void *lock)
{
    return krhino_mutex_lock((kmutex_t *)lock, RHINO_WAIT_FOREVER);
}

int32_t kv_unlock(void *lock)
{
    return krhino_mutex_unlock((kmutex_t *)lock);
}

void *kv_sem_create(void)
{
    int32_t ret;
    ksem_t *s;

    s = krhino_mm_alloc(sizeof(ksem_t));
    if (s == NULL) {
        return NULL;
    }

    ret = krhino_sem_create(s, "KV", 0);
    if (ret != RHINO_SUCCESS) {
        krhino_mm_free(s);
        return NULL;
    }

    return (void *)s;
}

int32_t kv_sem_free(void *sem)
{
    int32_t ret;

    ksem_t *m = (ksem_t *)sem;

    if (m == NULL) {
        return -1;
    }

    ret = krhino_sem_del(m);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    krhino_mm_free(m);

    return ret;
}

int32_t kv_sem_wait(void *sem)
{
    return krhino_sem_take((ksem_t *)sem, RHINO_WAIT_FOREVER);
}

int32_t kv_sem_post_all(void *sem)
{
    return krhino_sem_give_all((ksem_t *)sem);
}

int32_t kv_start_task(const char *name, void (*fn)(void *), void *arg,
                      uint32_t stack)
{
    ktask_t *task_handle = NULL;
    return krhino_task_dyn_create(&task_handle, name, arg, KV_TASK_PRIORITY, 0,
                                  stack / sizeof(cpu_stack_t), fn, 1u);
}

void kv_delete_task(void)
{
    krhino_task_dyn_del(NULL);
}

void *kv_malloc(uint32_t size)
{
    return krhino_mm_alloc(size);
}

void kv_free(void *ptr)
{
    krhino_mm_free(ptr);
}
