/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TASK_SEM_H
#define K_TASK_SEM_H

/**
 * This function will create a task-semaphore
 * @param[in]  task   pointer to the task
 * @param[in]  sem    pointer to the semaphore
 * @param[in]  name   name of the task-semaphore
 * @param[in]  count  count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_create(ktask_t *task, ksem_t *sem, const name_t *name,
                               size_t count);

/**
 * This function will delete a task-semaphore
 * @param[in]  task  pointer to the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_del(ktask_t *task);

/**
 * This function will give up a task-semaphore
 * @param[in]  task  pointer to the task
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_give(ktask_t *task);

/**
 * This function will take a task-semaphore
 * @param[in]  ticks  ticks to wait before take the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_take(tick_t ticks);

/**
 * This function will set the count of a task-semaphore
 * @param[in]  task   pointer to the task
 * @param[in]  count  count of the semaphre to set
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_count_set(ktask_t *task, sem_count_t count);

/**
 * This function will get task-semaphore count
 * @param[in]   task   pointer to the semphore
 * @param[out]  count  count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sem_count_get(ktask_t *task, sem_count_t *count);

#endif /* K_TASK_SEM_H */

