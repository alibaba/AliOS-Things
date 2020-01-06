/**
 * @file k_sem.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_SEM_H
#define K_SEM_H

/** @addtogroup aos_rhino sem
 *  Semaphore can be used for mutual exclusion between tasks, or tasks synchronize.
 *
 *  @{
 */

#define WAKE_ONE_SEM 0u
#define WAKE_ALL_SEM 1u

/**
 * Semaphore object
 */
typedef struct sem_s {
    /**<
     *  Manage blocked tasks
     *  List head is this semaphore, list node is task, which are blocked in this semaphore
     */
    blk_obj_t   blk_obj;
    sem_count_t count;
    sem_count_t peak_count;
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t     sem_item;   /**< kobj list for statistics */
#endif
    uint8_t     mm_alloc_flag;  /**< buffer from internal malloc or caller input */
} ksem_t;

/**
 * Create a semaphore.
 *
 * @param[in]  sem      pointer to the semaphore (the space is provided outside, by user)
 * @param[in]  name     name of the semaphore
 * @param[in]  count    the init count of the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_create(ksem_t *sem, const name_t *name, sem_count_t count);

/**
 * Delete a semaphore.
 *
 * @param[in]  sem      pointer to the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_del(ksem_t *sem);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Create and malloc a semaphore.
 *
 * @param[out]  sem     pointer to the semaphore (the space is provided inside, from heap)
 * @param[in]   name    name of the semaphore
 * @param[in]   count   the init count of the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_dyn_create(ksem_t **sem, const name_t *name, sem_count_t count);

/**
 * Delete and free a semaphore.
 *
 * @param[in]  sem      pointer to the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_dyn_del(ksem_t *sem);
#endif

/**
 * Give a semaphore.
 *
 * @param[in]  sem      pointer to the semphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_give(ksem_t *sem);

/**
 * Give a semaphore and wakeup all pending task.
 *
 * @param[in]  sem  pointer to the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_give_all(ksem_t *sem);

/**
 * Take a semaphore, task may be blocked.
 *
 * @param[in]  sem    pointer to the semaphore
 * @param[in]  ticks  ticks to wait before take
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks);

/**
 * Set the count of a semaphore.
 *
 * @param[in]  sem        pointer to the semaphore
 * @param[in]  sem_count  count of the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_count_set(ksem_t *sem, sem_count_t  count);

/**
 * Get count of a semaphore.
 *
 * @param[in]   sem    pointer to the semaphore
 * @param[out]  count  count of the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_count_get(ksem_t *sem, sem_count_t *count);

/** @} */

#endif /* K_SEM_H */

