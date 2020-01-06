/**
 * @file k_task_sem.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_TASK_SEM_H
#define K_TASK_SEM_H

/** @addtogroup aos_rhino task_sem
 *  Task-semaphore can be used for tasks synchronize.
 *
 *  @{
 */

/**
 * Create a semaphore and attach it to the task.
 *
 * @param[in]  task   pointer to the task
 * @param[in]  sem    pointer to the semaphore
 * @param[in]  name   name of the task-semaphore
 * @param[in]  count  count of the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_create(ktask_t *task, ksem_t *sem, const name_t *name, size_t count);

/**
 * Delete a task-semaphore.
 *
 * @param[in]  task  pointer to the task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_del(ktask_t *task);

/**
 * Give a task-semaphore.
 *
 * @param[in]  task  pointer to the task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_give(ktask_t *task);

/**
 * Take a task-semaphore, task may be blocked.
 *
 * @param[in]  ticks  ticks to wait before take
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_take(tick_t ticks);

/**
 * Set the count of a task-semaphore.
 *
 * @param[in]  task   pointer to the task
 * @param[in]  count  count of the semaphre to set
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_count_set(ktask_t *task, sem_count_t count);

/**
 * Get task-semaphore count.
 *
 * @param[in]   task   pointer to the semphore
 * @param[out]  count  count of the semaphore
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_count_get(ktask_t *task, sem_count_t *count);

/** @} */

#endif /* K_TASK_SEM_H */

