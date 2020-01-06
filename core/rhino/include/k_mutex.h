/**
 * @file k_mutex.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_MUTEX_H
#define K_MUTEX_H

/** @addtogroup aos_rhino mutex
 *  Mutex can be used for mutual exclusion between tasks.
 *
 *  @{
 */

/**
 * mutex object
 */
typedef struct mutex_s {
    /**<
     *  Manage blocked tasks
     *  List head is this mutex, list node is task, which are blocked in this mutex
     */
    blk_obj_t       blk_obj;
    ktask_t        *mutex_task; /**< Pointer to the owner task */
    /**<
     *  Manage mutexs owned by the task
     *  List head is task, list node is mutex, which are owned by the task
     */
    struct mutex_s *mutex_list;
    mutex_nested_t  owner_nested;
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t         mutex_item; /**< kobj list for statistics */
#endif
    uint8_t         mm_alloc_flag;  /**< buffer from internal malloc or caller input */
} kmutex_t;

/**
 * Create a mutex.
 *
 * @param[in] mutex     pointer to the mutex (the space is provided outside, by user)
 * @param[in] name      name of the mutex
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_create(kmutex_t *mutex, const name_t *name);

/**
 * Delete a mutex.
 *
 * @param[in] mutex     pointer to the mutex
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_del(kmutex_t *mutex);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Malloc and create a mutex.
 *
 * @param[out] mutex    pointer to the mutex (the space is provided inside, from heap)
 * @param[in]  name     name of the mutex
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_dyn_create(kmutex_t **mutex, const name_t *name);

/**
 * Delete and free a mutex.
 *
 * @param[in] mutex     pointer to the mutex
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_dyn_del(kmutex_t *mutex);
#endif

/**
 * Lock mutex, task may be blocked.
 *
 * @param[in]  mutex    pointer to the mutex
 * @param[in]  ticks    ticks to be wait for before lock
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex    pointer to the mutex
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_unlock(kmutex_t *mutex);

/** @} */

#endif /* K_MUTEX_H */

