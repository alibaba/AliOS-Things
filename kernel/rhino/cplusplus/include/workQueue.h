/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

namespace AliOS {

    class WorkQueue
    {
      public:
        /**
         * This function will creat a workqueue
         * @param[in]  name        the name of workqueue/worker, which should be
         * unique
         * @param[in]  pri         the priority of the worker
         * @param[in]  stack_buf   the stack of the worker(task)
         * @param[in]  stack_size  the size of the worker-stack
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t create(const name_t *name, uint8_t pri, cpu_stack_t *stack_buf,
                       size_t stack_size);

        /**
         * This function will delete a workqueue
         * @param[in]  none
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t destory(void);

        /**
         * This function will get a workqueue struct pointer
         * @param[in]  none
         * @return  workqueue struct pointer
         */
        kworkqueue_t *self(void);

      private:
        kworkqueue_t _workqueue_def;
    };

    class Work
    {
      public:
        /**
         * This function will initialize a work
         * @param[in]  handle  the call back function to run
         * @param[in]  arg     the paraments of the function
         * @param[in]  dly     the ticks to delay before run
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t init(work_handle_t handle, void *arg, tick_t dly);

        /**
         * This function will run a work on a workqueue
         * @param[in]  workqueue  the workqueue to run work
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t run(kworkqueue_t *workqueue);

        /**
         * This function will run a work on the default workqueue
         * @param[in]  none
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t sched(void);

        /**
         * This function will cancel a work
         * @param[in]  none
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t cancel(void);

        /**
         * This function will get a work struct pointer
         * @param[in]  none
         * @return  work struct pointer
         */
        kwork_t *self(void);

      private:
        kwork_t _work_def;
    };


}
