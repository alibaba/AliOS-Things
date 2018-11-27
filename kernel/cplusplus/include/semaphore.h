/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

namespace AliOS {

#define Semaphore_WAIT_FOREVER 0xFFFFFFFFU

    class Semaphore
    {
      public:
        /**
         * This function will create a semaphore
         * @param[in]  name   name of the semaphore
         * @param[in]  count  the init count of the semaphore
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t create(const name_t *name, sem_count_t count);

        /**
         * This function will delete a semaphore
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t destroy(void);

        /**
         * This function will wait a semaphore
         * @param[in]  millisec  millisec to wait before take
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t wait(uint32_t millisec);

        /**
         * This function will release a semaphore
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t release(void);

        /**
         * This function will release all semaphore
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t release_all(void);

        /**
         * This function will set the count of a semaphore
         * @param[in]  sem_count  count of the semaphore
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t count_set(sem_count_t count);

        /**
         * This function will get count of a semaphore
         * @param[out]  count  count of the semaphore
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t count_get(sem_count_t *count);

        /**
         * This function will get a Semaphore struct pointer
         * @param[in]  none
         * @return  Semaphore struct pointer
         */
        ksem_t *self(void);

      private:
        ksem_t _sem_def;
    };

}
