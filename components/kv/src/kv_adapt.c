/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include "k_api.h"
#include "kv_conf.h"
#include "kv_adapt.h"
#include "aos/hal/flash.h"
#include "vfsdev/flash_dev.h"
#if (CONFIG_U_FLASH_CORE > 0)
#include <aos/mtd.h>
#endif

static int32_t g_kv_flash_fd = -1;

static int32_t kv_flash_fd_open(void)
{
    int fd;
    char dev_str[16] = {0};
    if(g_kv_flash_fd >= 0){
        return g_kv_flash_fd;
    }

#if (CONFIG_U_FLASH_CORE > 0)
    snprintf(dev_str,16-1,"/dev/mtdblock%d",KV_PARTITION);
#else
    snprintf(dev_str,16-1,"/dev/flash%d",KV_PARTITION);
#endif

    fd = open(dev_str, 0);
    if(fd < 0){
        return fd;
    }
    g_kv_flash_fd = fd;

    return g_kv_flash_fd;
}

int32_t kv_flash_read(uint32_t offset, void *buf, uint32_t nbytes)
{
    int32_t ret;
    int fd;
    if(kv_flash_fd_open() < 0){
        return -1;
    }
    fd = g_kv_flash_fd;
    ret = lseek(fd, offset, SEEK_SET);
    if(ret < 0){
        return ret;
    }
    ret = read(fd, buf, nbytes);
    if(ret > 0){
       return 0;
    }else{
        return -1;
    }
}

int32_t kv_flash_write(uint32_t offset, void *buf, uint32_t nbytes)
{
    int32_t ret;
    int fd;
    if(kv_flash_fd_open() < 0){
        return -1;
    }
    fd = g_kv_flash_fd;
    ret = lseek(fd, offset, SEEK_SET);
    if(ret < 0){
        return ret;
    }
    ret = write(fd, buf, nbytes);
    if(ret > 0){
       return 0;
    }else{
        return -1;
    }
}

int32_t kv_flash_erase(uint32_t offset, uint32_t size)
{
    int32_t ret;
    int fd;
#if (CONFIG_U_FLASH_CORE > 0)
    struct mtd_erase_info erase_arg;
#endif

    if(kv_flash_fd_open() < 0){
        return -1;
    }
    fd = g_kv_flash_fd;
    ret = lseek(fd, offset, SEEK_SET);
    if(ret < 0){
        return ret;
    }

#if (CONFIG_U_FLASH_CORE > 0)
    erase_arg.offset = offset;
    erase_arg.length = size;
    ret = ioctl(fd, IOC_MTD_ERASE, &erase_arg);
#else
    ret = ioctl(fd, IOC_FLASH_ERASE_FLASH, size);
#endif

    return ret;
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
