/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

namespace AliOS {

#define Timer_WAIT_FOREVER 0xFFFFFFFFU

    class Timer
    {
      public:
        /**
         * This function will create a timer
         * @param[in]  name      name of the timer
         * @param[in]  cb        callbak of the timer
         * @param[in]  millisec  millisec of timer period
         * @param[in]  arg       the argument of the callback
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t create(const name_t *name, timer_cb_t cb, uint32_t millisec,
                       void *arg);

        /**
         * This function will delete a timer
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t destory(void);

        /**
         * This function will start a timer
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t start(void);

        /**
         * This function will stop a timer
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t stop(void);

        /**
         * This function will get a Semaphore struct pointer
         * @param[in]  none
         * @return  Semaphore struct pointer
         */
        ktimer_t *self(void);

      private:
        ktimer_t _timer_def;
    };

}
