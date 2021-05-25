/**
 * Copyright (c) 2015, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _OS_MSG_H_
#define _OS_MSG_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  OS      OSIF
 * \defgroup    Message Message Queue
 *
 * \brief   Exchange messages between tasks in a FIFO-like operation.
 * \details The Message Queue function group allows to control, send, receive, or wait for message.
 *          Message transmission is a basic communication model between tasks that one task sends
 *          data explicitly, while another task receives it. The operation is more like some
 *          kind of I/O rather than a direct access to information to be shared. The data to be
 *          passed can be any type.\n
 *
 * \image html OS-message-queue-overview.jpg "Message Queue Overview" width=617px height=321px
 *
 * \ingroup     OS
 */


bool os_msg_queue_create_intern(void **pp_handle, uint32_t msg_num, uint32_t msg_size,
                                const char *p_func, uint32_t file_line);

bool os_msg_queue_delete_intern(void *p_handle, const char *p_func, uint32_t file_line);

bool os_msg_queue_peek_intern(void *p_handle, uint32_t *p_msg_num,
                              const char *p_func, uint32_t file_line);

bool os_msg_send_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line);

bool os_msg_recv_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line);

bool os_msg_peek_intern(void *p_handle, void *p_msg, uint32_t wait_ms,
                        const char *p_func, uint32_t file_line);

/**
 * os_msg.h
 *
 * \brief   Creates a message queue instance. This allocates the storage required by the
 *          new queue and passes back a handle for the queue.
 *
 * \param[out]  pp_handle  Used to pass back a handle by which the message queue
 *                         can be referenced.
 *
 * \param[in]   msg_num    The maximum number of items that the queue can contain.
 *
 * \param[in]   msg_size   The number of bytes each item in the queue will require. Items
 *                         are queued by copy, not by reference, so this is the number of
 *                         bytes that will be copied for each posted item. Each item on the
 *                         queue must be the same size.
 *
 * \return           The status of the message queue creation.
 * \retval true      Message queue was created successfully.
 * \retval false     Message queue was failed to create.
 *
 * <b>Example usage</b>
 * \code{.c}
 * struct test_msg
 * {
 *     uint8_t id;
 *     uint8_t data[16];
 * }
 *
 * #define MSG_NUM  10
 *
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     // Create a queue capable of containing 10 items of structure test_msg.
 *     if (os_msg_queue_create(&p_handle, MSG_NUM, sizeof(struct test_msg)) == true)
 *     {
 *         // Message queue created successfully.
 *     }
 *     else
 *     {
 *         // Message queue failed to create.
 *         return -1;
 *     }
 *
 *     // Delete the message queue.
 *     os_msg_queue_delete(p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Message
 */
#define os_msg_queue_create(pp_handle, msg_num, msg_size)    \
    os_msg_queue_create_intern(pp_handle, msg_num, msg_size, __func__, __LINE__)

/**
 * os_msg.h
 *
 * \brief   Delete the specified message queue, and free all the memory allocated for
 *          storing of items placed on the queue.
 *
 * \param[in]   p_handle   The handle to the message queue being deleted.
 *
 * \return           The status of the message queue deletion.
 * \retval true      Message queue was deleted successfully.
 * \retval false     Message queue was failed to delete.
 *
 * <b>Example usage</b>
 * \code{.c}
 * struct test_msg
 * {
 *     uint8_t id;
 *     uint8_t data[16];
 * }
 *
 * #define MSG_NUM  10
 *
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     // Create a queue capable of containing 10 items of structure test_msg.
 *     if (os_msg_queue_create(&p_handle, MSG_NUM, sizeof(struct test_msg)) == true)
 *     {
 *         // Message queue created successfully.
 *     }
 *     else
 *     {
 *         // Message queue failed to create.
 *         return -1;
 *     }
 *
 *     // Delete the message queue.
 *     os_msg_queue_delete(p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Message
 */
#define os_msg_queue_delete(p_handle) \
    os_msg_queue_delete_intern(p_handle, __func__, __LINE__)

/**
 * os_msg.h
 *
 * \brief    Peek the number of items sent and resided on the message queue.
 *
 * \param[in]   p_handle   The handle to the message queue being peeked.
 *
 * \param[out]  p_msg_num  Used to pass back the number of items residing on the message queue.
 *
 * \return           The status of the message queue peek.
 * \retval true      Message queue was peeked successfully.
 * \retval false     Message queue was failed to peek.
 *
 * <b>Example usage</b>
 * \code{.c}
 * struct test_msg
 * {
 *     uint8_t id;
 *     uint8_t data[16];
 * }
 *
 * #define MSG_NUM  10
 *
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     uint32_t msg_num;
 *
 *     // Create a queue capable of containing 10 items of structure test_msg.
 *     if (os_msg_queue_create(&p_handle, MSG_NUM, sizeof(struct test_msg)) == true)
 *     {
 *         // Message queue created successfully.
 *     }
 *     else
 *     {
 *         // Message queue failed to create.
 *         return -1;
 *     }
 *
 *     // Peek the number of items sent on this message queue.
 *     os_msg_queue_peek(p_handle, &msg_num);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Message
 */
#define os_msg_queue_peek(p_handle, p_msg_num)  \
    os_msg_queue_peek_intern(p_handle, p_msg_num, __func__, __LINE__)

/**
 * os_msg.h
 *
 * \brief   Send an item to the back of the specified message queue. The item is
 *          queued by copy, not by reference.
 *
 * \param[in]   p_handle The handle to the message queue on which the item is to be sent.
 *
 * \param[in]   p_msg    Pointer to the item that is to be sent on the queue. The referenced
 *                       item rather than pointer itself will be copied on the queue.
 *
 * \param[in]   wait_ms  The maximum amount of time in milliseconds that the task should
 *                       block waiting for the item to sent on the queue.
 * \arg \c 0           No blocking and return immediately.
 * \arg \c 0xFFFFFFFF  Block infinitely until the item sent.
 * \arg \c others      The timeout value in milliseconds.
 *
 * \return           The status of the message item sent.
 * \retval true      Message item was sent successfully.
 * \retval false     Message item was failed to send.
 *
 * <b>Example usage</b>
 * \code{.c}
 * struct test_msg
 * {
 *     uint8_t id;
 *     uint8_t data[16];
 * }
 *
 * #define MSG_NUM  10
 *
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *     struct test_msg msg;
 *
 *     // Create a queue capable of containing 10 items of structure test_msg.
 *     if (os_msg_queue_create(&p_handle, MSG_NUM, sizeof(struct test_msg)) == true)
 *     {
 *         // Message queue created successfully.
 *     }
 *     else
 *     {
 *         // Message queue failed to create.
 *         return -1;
 *     }
 *
 *     // Send the item to this message queue.
 *     msg.id = 1;
 *     msg.data[0] = 0;
 *     os_msg_send(p_handle, &msg, 0);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Message
 */
#define os_msg_send(p_handle, p_msg, wait_ms) \
    os_msg_send_intern(p_handle, p_msg, wait_ms, __func__, __LINE__)

/**
 * os_msg.h
 *
 * \brief   Receive an item from the specified message queue. The item is received by
 *          copy rather than by reference, so a buffer of adequate size must be provided.
 *
 * \param[in]   p_handle The handle to the message queue from which the item is to be received.
 *
 * \param[out]  p_msg    Pointer to the buffer into which the received item will be copied.
 *                       item rather than pointer itself will be copied on the queue.
 *
 * \param[in]   wait_ms  The maximum amount of time in milliseconds that the task should
 *                       block waiting for an item to be received from the queue.
 * \arg \c 0           No blocking and return immediately.
 * \arg \c 0xFFFFFFFF  Block infinitely until the item received.
 * \arg \c others      The timeout value in milliseconds.
 *
 * \return           The status of the message item received.
 * \retval true      Message item was received successfully.
 * \retval false     Message item was failed to receive.
 *
 * <b>Example usage</b>
 * \code{.c}
 * struct test_msg
 * {
 *     uint8_t id;
 *     uint8_t data[16];
 * }
 *
 * #define MSG_NUM  10
 *
 * void *p_handle = NULL;
 *
 * void send_msg(void)
 * {
 *     struct test_msg msg;
 *
 *     // Create a queue capable of containing 10 items of structure test_msg.
 *     if (os_msg_queue_create(&p_handle, MSG_NUM, sizeof(struct test_msg)) == true)
 *     {
 *         // Message queue created successfully.
 *     }
 *     else
 *     {
 *         // Message queue failed to create.
 *         return -1;
 *     }
 *
 *     // Send the item to this message queue.
 *     msg.id = 1;
 *     msg.data[0] = 0;
 *     os_msg_send(p_handle, &msg, 0);
 *
 *     return 0;
 * }
 *
 * void receive_msg(void)
 * {
 *     struct test_msg msg;
 *
 *     // Receive the message queue item.
 *     if (os_msg_recv(p_handle, &msg, 0) == true)
 *     {
 *         // Message item received successfully.
 *     }
 *     else
 *     {
 *         // Message item failed to receive.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Message
 */
#define os_msg_recv(p_handle, p_msg, wait_ms) \
    os_msg_recv_intern(p_handle, p_msg, wait_ms, __func__, __LINE__)

/**
 * os_msg.h
 *
 * \brief   Receive but not remove an item from the specified message queue. The item is
 *          received by copy rather than by reference, so a buffer of adequate size must
 *          be provided.
 *
 * \param[in]   p_handle The handle to the message queue on which the item is to be peeked.
 *
 * \param[out]  p_msg    Pointer to the buffer into which the received item will be copied.
 *                       item rather than pointer itself will be copied on the queue.
 *
 * \param[in]   wait_ms  The maximum amount of time in milliseconds that the task should
 *                       block waiting for an item to be received from the queue.
 * \arg \c 0           No blocking and return immediately.
 * \arg \c 0xFFFFFFFF  Block infinitely until the item received.
 * \arg \c others      The timeout value in milliseconds.
 *
 * \return           The status of the message item received.
 * \retval true      Message item was received successfully.
 * \retval false     Message item was failed to receive.
 *
 * <b>Example usage</b>
 * \code{.c}
 * struct test_msg
 * {
 *     uint8_t id;
 *     uint8_t data[16];
 * }
 *
 * #define MSG_NUM  10
 *
 * void *p_handle = NULL;
 *
 * void send_msg(void)
 * {
 *     struct test_msg msg;
 *
 *     // Create a queue capable of containing 10 items of structure test_msg.
 *     if (os_msg_queue_create(&p_handle, MSG_NUM, sizeof(struct test_msg)) == true)
 *     {
 *         // Message queue created successfully.
 *     }
 *     else
 *     {
 *         // Message queue failed to create.
 *         return -1;
 *     }
 *
 *     // Send the item to this message queue.
 *     msg.id = 1;
 *     msg.data[0] = 0;
 *     os_msg_send(p_handle, &msg, 0);
 *
 *     return 0;
 * }
 *
 * void peek_msg(void)
 * {
 *     struct test_msg msg;
 *
 *     // Peek the message queue item.
 *     if (os_msg_peek(p_handle, &msg, 0) == true)
 *     {
 *         // Message item peeked successfully.
 *     }
 *     else
 *     {
 *         // Message item failed to peek.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  Message
 */
#define os_msg_peek(p_handle, p_msg, wait_ms) \
    os_msg_peek_intern(p_handle, p_msg, wait_ms, __func__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* _OS_MSG_H_ */
