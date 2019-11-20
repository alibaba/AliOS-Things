/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_OS_H
#define __USB_HOST_OS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <k_api.h>

/* Define USB Host OS related Type */
typedef kbuf_queue_t usbh_os_queue_t;
typedef ktask_t      usbh_os_task_t;
typedef cpu_stack_t  usbh_os_stack_t;
typedef kmutex_t     usbh_os_lock_t;
typedef uint16_t     usbh_os_event_t;

#define usbh_os_malloc  krhino_mm_alloc
#define usbh_os_free    krhino_mm_free

/**
 * @brief Create USB Host Message Queue
 * 
 * @param[out]  pqueue pointer to the message queue
 * 
 * @return 0: success, otherwise is failed
 */
int usbh_os_queue_create(usbh_os_queue_t *pqueue);

/**
 * @brief Receive Message via the USB Host Message Queue
 * 
 * @param[in]   pqueue  pointer to the message queue
 * @param[out]  msg     pointer to the message which will be received
 * @param[in]   size    the length of the message
 * @param[in]   timeout the timeout of waiting
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_queue_recv(usbh_os_queue_t *pqueue, void *msg, uint32_t size, 
                       uint32_t timeout);

/**
 * @brief Send Message via the USB Host Message Queue
 * 
 * @param[in]  pqueue pointer to the message queue
 * @param[in]  msg    pointer to the message which will be send
 * @param[in]  size   the length of the message
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_queue_send(usbh_os_queue_t *pqueue, void *msg, uint32_t size);

/**
 * @brief Delete USB Host Message Queue
 * 
 * @param[in]  pqueue pointer to the message queue
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_queue_del(usbh_os_queue_t *pqueue);

/**
 * @brief Start the USB Host Task
 *
 * @param[in] ptask pointer to the USB Host main task
 * @param[in] fn    pointer to the task entry
 * @param[in] arg   pointer to the arguments of the task entry
 *
 * @return 0:success, otherwise is failed
 */
int usbh_os_start_task(usbh_os_task_t *ptask, void (*fn)(void *), void *arg);

/**
 * @brief Create The Global USB Host Lock
 * 
 * @param[out] plock pointer to the global host lock
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_lock_create(usbh_os_lock_t *plock);

/**
 * @brief Lock The USB Host Instance
 * 
 * @param[in]  plock pointer to the global host lock
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_lock(usbh_os_lock_t *plock);

/**
 * @brief Unlock The USB Host Instance
 * 
 * @param[in]  plock pointer to the global host lock
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_unlock(usbh_os_lock_t *plock);

/**
 * @brief Delete The Global USB Host Lock
 * 
 * @param[out] plock pointer to the global host lock
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_os_lock_del(usbh_os_lock_t *plock);

/**
 * @brief Milliseconds sleep
 * 
 * @param[in] ms sleep time in milliseconds
 * 
 * @return none
 */
void usbh_os_delay(int ms);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_OS_H */
