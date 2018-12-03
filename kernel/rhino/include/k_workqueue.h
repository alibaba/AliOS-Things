/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_WORKQUEUE_H
#define K_WORKQUEUE_H

#if (RHINO_CONFIG_WORKQUEUE > 0)
#define WORKQUEUE_WORK_MAX 32

typedef void (*work_handle_t)(void *arg);

typedef struct {
    klist_t       work_node;
    work_handle_t handle;
    void         *arg;
    tick_t        dly;
    ktimer_t     *timer;
    void         *wq;
    uint8_t       work_exit;
} kwork_t;

typedef struct {
    klist_t       workqueue_node;
    klist_t       work_list;
    kwork_t      *work_current; /* current work */
    const name_t *name;
    ktask_t       worker;
    ksem_t        sem;
} kworkqueue_t;

/**
 * This function will creat a workqueue
 * @param[in]  workqueue   the workqueue to be created
 * @param[in]  name        the name of workqueue/worker, which should be unique
 * @param[in]  pri         the priority of the worker
 * @param[in]  stack_buf   the stack of the worker(task)
 * @param[in]  stack_size  the size of the worker-stack
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_workqueue_create(kworkqueue_t *workqueue, const name_t *name,
                                uint8_t pri, cpu_stack_t *stack_buf, size_t stack_size);

/**
 * This function will initialize a work
 * @param[in]  work    the work to be initialized
 * @param[in]  handle  the call back function to run
 * @param[in]  arg     the paraments of the function
 * @param[in]  dly     the ticks to delay before run
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_work_init(kwork_t *work, work_handle_t handle, void *arg,
                         tick_t dly);

/**
 * This function will run a work on a workqueue
 * @param[in]  workqueue  the workqueue to run work
 * @param[in]  work       the work to run
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_work_run(kworkqueue_t *workqueue, kwork_t *work);

/**
 * This function will run a work on the default workqueue
 * @param[in]  work  the work to run
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_work_sched(kwork_t *work);

/**
 * This function will cancel a work
 * @param[in]  work  the work to cancel
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_work_cancel(kwork_t *work);
#endif

#endif /* K_WORKQUEUE_H */

