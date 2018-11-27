/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

namespace AliOS {

#define Mutex_WAIT_FOREVER 0xFFFFFFFFU

    class Mutex
    {
      public:
        /**
         * This function will create a mutex
         * @param[in] name   name of the mutex
         * @return the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t create(const name_t *name);

        /**
         * This function will delete a mutex
         * @param[in] NULL
         * @return the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t destroy(void);

        /**
         * This function will lock mutex
         * @param[in]  millisec  millisec to be wait for before lock
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t lock(uint32_t millisec);

        /**
         * This function will unlock a mutex
         * @param[in] NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t unlock(void);

        /**
         * This function will get a Mutex struct pointer
         * @param[in]  none
         * @return  Mutex struct pointer
         */
        kmutex_t *self(void);

      private:
        kmutex_t _mutex_def;
    };

}
