/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

/** @defgroup cpp_aos_api cplusplus
 *  @ingroup aos_components
 * @{
 */

/**
 * @}
 */

/** @defgroup cpp_aos_mutex
 *  @ingroup cpp_aos_api
 * @{
 */
namespace AOS {

#define Mutex_WAIT_FOREVER 0xFFFFFFFFU

/**
 * @brief Mutex Class.
 *
 */

    class Mutex
    {
      public:
        /**
         * @brief This function will create a mutex
         * @param[in] name   name of the mutex
         * @return the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t create(const name_t *name);

        /**
         * @brief This function will delete a mutex
         * @param[in] NULL
         * @return the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t destroy(void);

        /**
         * @brief This function will lock mutex
         * @param[in]  millisec  millisec to be wait for before lock
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t lock(uint32_t millisec);

        /**
         * @brief This function will unlock a mutex
         * @param[in] NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t unlock(void);

        /**
         * @brief This function will get a Mutex struct pointer
         * @param[in]  none
         * @return  Mutex struct pointer
         */
        kmutex_t *self(void);

      private:
        /**
         * @brief a Queue buffer
         */
        kmutex_t _mutex_def;
    };

}
/**
 * @}
 */
