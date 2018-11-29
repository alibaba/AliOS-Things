/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_SEM_H
#define K_SEM_H

#define WAKE_ONE_SEM 0u
#define WAKE_ALL_SEM 1u

typedef struct sem_s {
    blk_obj_t   blk_obj;
    sem_count_t count;
    sem_count_t peak_count;
#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_t     sem_item;
#endif
    uint8_t mm_alloc_flag;
} ksem_t;

/**
 * This function will create a semaphore
 * @param[in]  sem    pointer to the semaphore(the space is provided by user)
 * @param[in]  name   name of the semaphore
 * @param[in]  count  the init count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_create(ksem_t *sem, const name_t *name, sem_count_t count);

/**
 * This function will delete a semaphore
 * @param[in]  sem  pointer to the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_del(ksem_t *sem);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will create a dyn-semaphore
 * @param[out]  sem    pointer to the semaphore(the space is provided by kernel)
 * @param[in]   name   name of the semaphore
 * @param[in]   count  the init count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_dyn_create(ksem_t **sem, const name_t *name,
                              sem_count_t count);

/**
 * This function will delete a dyn-semaphore
 * @param[in]  sem  pointer to the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_dyn_del(ksem_t *sem);
#endif

/**
 * This function will give a semaphore
 * @param[in]  sem  pointer to the semphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_give(ksem_t *sem);

/**
 * This function will give a semaphore and wakeup all thee waiting task
 * @param[in]  sem  pointer to the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_give_all(ksem_t *sem);

/**
 * This function will take a semaphore
 * @param[in]  sem    pointer to the semaphore
 * @param[in]  ticks  ticks to wait before take
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks);

/**
 * This function will set the count of a semaphore
 * @param[in]  sem        pointer to the semaphore
 * @param[in]  sem_count  count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_count_set(ksem_t *sem, sem_count_t  count);

/**
 * This function will get count of a semaphore
 * @param[in]   sem    pointer to the semaphore
 * @param[out]  count  count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sem_count_get(ksem_t *sem, sem_count_t *count);

#endif /* K_SEM_H */

