/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

namespace AliOS {

#define Thread_WAIT_FOREVER 0xFFFFFFFFU

    class Thread
    {
      public:
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
        kstat_t create(const name_t *name, void *arg, uint8_t prio,
                       tick_t ticks, size_t stack_size, task_entry_t entry,
                       uint8_t autorun);

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
        kstat_t create_smp(const name_t *name, void *arg, uint8_t prio,
                           tick_t ticks, size_t stack_size, task_entry_t entry,
                           uint8_t cpu_num, uint8_t autorun);

#endif

        /**
         * This function will dynamic terminate current thread
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t terminate(void);

#endif

#if (RHINO_CONFIG_TASK_SUSPEND > 0)
        /**
         * This function will start current thread
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t start(void);

        /**
         * This function will stop current thread
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t stop(void);

#endif

        /**
         * This function will cause a task to sleep for some millisec
         * @param[in]  millisec the time t0 sleep
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t sleep(uint32_t millisec);

        /**
         * This function will yield current thread
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t yield(void);

        /**
         * This function will return current thread
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        ktask_t *self(void);

        /**
         * This function will change the prio of task
         * @param[in]   task     the task to be changed prio
         * @param[in]   pri      the prio to be changed.
         * @param[out]  old_pri  the old task prio to be filled with
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t prio_change(uint8_t pri);

#if (RHINO_CONFIG_CPU_NUM > 1)

        /**
         * This function will bind task to cpu
         * @param[in]  cpu_num    the cpu_num of task to run
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t cpu_bind(uint8_t cpu_num);

        /**
         * This function will bind task to cpu
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t cpu_unbind(void);

#endif

      private:
        ktask_t *p_thread_def;
    };

}
