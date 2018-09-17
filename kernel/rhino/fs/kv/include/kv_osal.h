/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _KV_OSAL_H
#define _KV_OSAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <k_api.h>

#define KV_WAIT_FOREVER RHINO_WAIT_FOREVER

typedef ksem_t      kv_sem_t;
typedef kmutex_t    kv_mutex_t;

/**
 * @brief Read KV flash partition
 * 
 * @param[in]   offset the offset of the partition
 * @param[out]  buf    the store buffer for read
 * @param[in]   nbytes the length to read
 * 
 * @return 0 on success, otherwise will be failed
 */
int kv_os_partition_read(uint32_t offset, void *buf, size_t nbytes);

/**
 * @brief Write KV flash partition
 * 
 * @param[in]   offset the offset of the partition
 * @param[in]   buf    the store buffer for write
 * @param[in]   nbytes the length to write
 * 
 * @return 0 on success, otherwise will be failed
 */
int kv_os_partition_write(uint32_t offset, void *buf, size_t nbytes);

/**
 * @brief Erase KV flash partition
 * 
 * @param[in]  offset the offset of the partition
 * @param[in]  size   the length to erase
 * 
 * @return 0 on success, otherwise will be failed
 */
int kv_os_partition_erase(uint32_t offset, uint32_t size);

/**
 * @brief Create OS mutex
 * 
 * @param[in]  mutex pointer to the os mutex
 * @param[in]  name  the mutex name
 * 
 * @return 0 on success, otherwise is failed
 * 
 * int kv_os_mutex_create(kv_mutex_t *mutex, const char *name);
 */
#define kv_os_mutex_create krhino_mutex_create

/**
 * @breif Delete OS mutex
 * 
 * @param[in]  mutex pointer to the os mutex
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_mutex_del(kv_mutex_t *mutex);
 */
#define kv_os_mutex_del krhino_mutex_del

/**
 * @brief Lock the mutex
 * 
 * @param[in]  mutex   pointer to the os mutex
 * @param[in]  timeout timeout for waiting
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_mutex_lock(kv_mutex_t *mutex, uint64_t timeout);
 */
#define kv_os_mutex_lock krhino_mutex_lock

/**
 * @brief Unlock the mutex
 * 
 * @param[in]  mutex pointer to the os mutex
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_mutex_unlock(kv_mutex_t *mutex);
 */
#define kv_os_mutex_unlock krhino_mutex_unlock

/**
 * @brief Create OS semaphore
 * 
 * @param[in]  sem   pointer to the os semaphore
 * @param[in]  name  name of the semaphore
 * @param[in]  count the init count of the semaphore
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_sem_create(kv_sem_t *sem, const char *name, uint32_t count);
 */
#define kv_os_sem_create krhino_sem_create

/**
 * @brief Delete OS semaphore
 * 
 * @param[in]  sem pointer to the os semaphore
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_sem_del(kv_sem_t *sem);
 */
#define kv_os_sem_del krhino_sem_del

/**
 * @brief Take a semaphore
 * 
 * @param[in]  sem     pointer to the semaphore
 * @param[in]  timeout timeout for waiting
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_sem_take(kv_sem_t *sem, uint64_t timeout);
 */
#define kv_os_sem_take krhino_sem_take

/**
 * @brief Give a semaphore and wakeup all waiting task
 * 
 * @param[in]  sem pointer to the semaphore
 * 
 * @return 0 on success, otherwise is failed
 *
 * int kv_os_sem_give_all(kv_sem_t *sem);
 */
#define kv_os_sem_give_all krhino_sem_give_all

/**
 * @brief Create a task
 *
 * @param[in] name name of the task
 * @param[in] fn   the entry function of task
 * @param[in] arg  the parameter of task entry function
 * @param[in] stack the size of task stack
 *
 * @return 0 on success, otherwise is failed
 */
int kv_os_task_start(const char *name, void (*fn)(void *), void *arg, 
                     int stack);

/**
 * @brief Delete a task
 * 
 * @return none
 */
void kv_os_task_del(void);

/**
 * @brief wrapper of MM allocation
 * 
 * @param[in]  size size of the mem to alloc
 * 
 * @return NULL is error, other is memory address
 *
 * void *kv_os_malloc(size_t size);
 */
#define kv_os_malloc krhino_mm_alloc

/**
 * @brief wrapper of MM free
 * 
 * @param[in] ptr address point of the mem
 *
 * void kv_os_free(void *ptr);
 */
#define kv_os_free krhino_mm_free

#ifdef __cplusplus
}
#endif
#endif