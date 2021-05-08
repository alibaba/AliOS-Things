/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <k_api.h>

/** @defgroup cpp_aos_queue
 *  @ingroup cpp_aos_api
 * @{
 */
namespace AOS {

#define Queue_WAIT_FOREVER 0xFFFFFFFFU

/**
 * @brief Queue Class.
 *
 */

    class Queue
    {
      public:
        /**
         * @brief This function will create a buf-queue
         * @param[in]  name     name of the queue
         * @param[in]  buf      pointer to the buf
         * @param[in]  size     size of the buf
         * @param[in]  max_msg  max size of one msg
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t create(const name_t *name, void *buf, size_t size,
                       size_t max_msg);

        /**
         * @brief This function will delete a queue
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t destory(void);

        /**
         * @brief This function will send a msg at the end of queue
         * @param[in]  msg    pointer to msg to be send
         * @param[in]  size   size of the msg
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t send(void *msg, size_t size);

        /**
         * @brief This function will receive msg form aqueue
         * @param[out]  msg    pointer to the buf to save msg
         * @param[out]  size   size of received msg
         * @param[in]   millisec  millisec to wait before receiving msg
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t receive(void *msg, size_t *size, uint32_t millisec);

        /**
         * @brief This function will reset queue
         * @param[in]  NULL
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t flush(void);

        /**
         * @brief This function will get information of a queue
         * @param[out]  free   free size of the queue buf
         * @param[out]  total  total size of the queue buf
         * @return  the operation status, RHINO_SUCCESS is OK, others is error
         */
        kstat_t info_get(kbuf_queue_info_t *info);

        /**
         * @brief This function will get a Queue struct pointer
         * @param[in]  none
         * @return  Queue struct pointer
         */
        kbuf_queue_t *self(void);

      private:
        /**
         * @brief a Queue buffer
         */
        kbuf_queue_t _buf_queue_def;
    };

}
/**
 * @}
 */
