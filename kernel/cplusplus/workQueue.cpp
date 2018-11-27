/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "workQueue.h"

using namespace AliOS;

/**
 * This function will creat a workqueue
 * @param[in]  name        the name of workqueue/worker, which should be unique
 * @param[in]  pri         the priority of the worker
 * @param[in]  stack_buf   the stack of the worker(task)
 * @param[in]  stack_size  the size of the worker-stack
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t WorkQueue::create(const name_t *name, uint8_t pri, cpu_stack_t *stack_buf, size_t stack_size)
{
    return krhino_workqueue_create(&_workqueue_def, name, pri, stack_buf, stack_size / sizeof(cpu_stack_t));
}

/**
 * This function will delete a workqueue
 * @param[in]  none
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t WorkQueue::destory(void)
{
    return kstat_t(0);
}

/**
 * This function will get a workqueue struct pointer
 * @param[in]  none
 * @return  workqueue struct pointer
 */
kworkqueue_t *WorkQueue::self(void)
{
	return &_workqueue_def;
}

/**
 * This function will initialize a work
 * @param[in]  handle  the call back function to run
 * @param[in]  arg     the paraments of the function
 * @param[in]  dly     the ticks to delay before run
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Work::init(work_handle_t handle, void *arg, tick_t dly)
{
    return krhino_work_init(&_work_def, handle, arg, dly);
}

/**
 * This function will run a work on a workqueue
 * @param[in]  workqueue  the workqueue to run work
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Work::run(kworkqueue_t *workqueue)
{
    return krhino_work_run(workqueue, &_work_def);
}

/**
 * This function will run a work on the default workqueue
 * @param[in]  none
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Work::sched(void)
{
    return krhino_work_sched(&_work_def);
}

/**
 * This function will cancel a work
 * @param[in]  none
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Work::cancel(void)
{
    return krhino_work_cancel(&_work_def);
}

/**
 * This function will get a Work struct pointer
 * @param[in]  none
 * @return  Work struct pointer
 */
kwork_t *Work::self(void)
{
	return &_work_def;
}
