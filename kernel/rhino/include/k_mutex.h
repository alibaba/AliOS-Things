/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MUTEX_H
#define K_MUTEX_H

typedef struct mutex_s {
    blk_obj_t       blk_obj;
    ktask_t        *mutex_task; /* mutex owner task */
    struct mutex_s *mutex_list; /* task mutex list */
    mutex_nested_t  owner_nested;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_t         mutex_item;
#endif

    uint8_t         mm_alloc_flag;
} kmutex_t;

/**
 * This function will create a mutex
 * @param[in] mutex  pointer to the mutex(the space is provided by user)
 * @param[in] name   name of the mutex
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_create(kmutex_t *mutex, const name_t *name);

/**
 * This function will delete a mutex
 * @param[in] mutex pointer to the mutex
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_del(kmutex_t *mutex);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will create a dyn mutex
 * @param[in]  mutex  pointer to the mutex(the space is provided by user)
 * @param[in]  name   name of the mutex
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_dyn_create(kmutex_t **mutex, const name_t *name);

/**
 * This function will delete a dyn mutex
 * @param[in] mutex  pointer to the mutex
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_dyn_del(kmutex_t *mutex);
#endif

/**
 * This function will lock mutex
 * @param[in]  mutex  pointer to the mutex
 * @param[in]  ticks  ticks to be wait for before lock
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks);

/**
 * This function will unlock a mutex
 * @param[in]  mutex  pointer to the mutex
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_unlock(kmutex_t *mutex);

#endif /* K_MUTEX_H */

