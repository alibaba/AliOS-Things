/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "thread.h"

using namespace AliOS;

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

/**
 * This function will initialize a task
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  prio       the prio of task
 * @param[in]  ticks      the time slice if there are same prio task
 * @param[in]  stack_size the size of thread stack
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::create(const name_t *name,
                      void *arg,
                      uint8_t prio,
                      tick_t ticks,
                      size_t stack_size,
                      task_entry_t entry,
                      uint8_t autorun)
{
    return krhino_task_dyn_create(&p_thread_def,
                                 name,
                                 arg,
                                 prio,
                                 ticks,
                                 (stack_size/4),
                                 entry,
                                 autorun);
}

thread::thread(const name_t *name,
        void *arg,
        uint8_t prio,
        tick_t ticks,
        size_t stack_size,
        task_entry_t entry,
        uint8_t autorun)
{
    krhino_task_dyn_create(&p_thread_def, name, arg, prio,
            ticks, (stack_size/4), entry, autorun);
}


thread::thread()
{
}


#if (RHINO_CONFIG_CPU_NUM > 1)

/**
 * This function will initialize a task in SMP
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  prio       the prio of task
 * @param[in]  ticks      the time slice if there are same prio task
 * @param[in]  stack_size the size of thread stack
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 * @param[in]  cpu_num    the cpu_num of task to run
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::create_smp(const name_t *name,
                          void *arg,
                          uint8_t prio,
                          tick_t ticks,
                          size_t stack_size,
                          task_entry_t entry,
                          uint8_t cpu_num,
                          uint8_t autorun)
{
    return krhino_task_cpu_dyn_create(&p_thread_def,
                                     name,
                                     arg,
                                     prio,
                                     ticks,
                                     (stack_size/4),
                                     entry,
                                     cpu_num,
                                     autorun);
}

#endif

/**
 * This function will dynamic terminate current thread
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::terminate(void)
{
    return krhino_task_dyn_del(p_thread_def);
}

thread::~thread(void)
{
    krhino_task_dyn_del(p_thread_def);
}

#endif

/**
 * This function will start current thread
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::start(void)
{
    return krhino_task_resume(p_thread_def);
}

/**
 * This function will stop current thread
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::stop(void)
{
    return krhino_task_suspend(p_thread_def);
}

/**
 * This function will cause a task to sleep for some millisec
 * @param[in]  millisec the time t0 sleep
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::sleep(uint32_t millisec)
{
    tick_t ticks = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == Thread_WAIT_FOREVER){
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(millisec);
    }

    return krhino_task_sleep(ticks ? ticks : 1); /* Minimum delay = 1 tick */
}

/**
 * This function will yield current thread
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t thread::yield(void)
{
    return krhino_task_yield();
}

/**
 * This function will return current thread
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
ktask_t *thread::self(void)
{
    return krhino_cur_task_get();
}

/**
* This function will change the prio of task
* @param[in]   task     the task to be changed prio
* @param[in]   pri      the prio to be changed.
* @param[out]  old_pri  the old task prio to be filled with
* @return  the operation status, RHINO_SUCCESS is OK, others is error
*/
kstat_t thread::prio_change(uint8_t pri)
{
    uint8_t old_pri = 0;

    return krhino_task_pri_change(p_thread_def, pri, &old_pri);
}

#if (RHINO_CONFIG_CPU_NUM > 1)

/**
* This function will bind task to cpu
* @param[in]  cpu_num    the cpu_num of task to run
* @return  the operation status, RHINO_SUCCESS is OK, others is error
*/
kstat_t thread::cpu_bind(uint8_t cpu_num)
{
    return krhino_task_cpu_bind(p_thread_def, cpu_num);
}

/**
* This function will bind task to cpu
* @param[in]  NULL
* @return  the operation status, RHINO_SUCCESS is OK, others is error
*/
kstat_t thread::cpu_unbind(void)
{
    return krhino_task_cpu_unbind(p_thread_def);
}

#endif
