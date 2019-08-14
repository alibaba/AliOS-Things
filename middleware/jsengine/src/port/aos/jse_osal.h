/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* JSEngine OS Abstract Layer */
#ifndef JSE_OSAL_H
#define JSE_OSAL_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    TimerRunOnce     = 0, /* one-shot timer */
    TimerRunPeriodic = 1  /* repeating timer */
} jse_osal_timer_type;

#define osWaitForever 0xFFFFFFFF /* wait forever timeout value */

/* task priority */
#define JSE_TSK_PRIORITY jse_osal_task_get_default_priority() + 5
#define CLI_TSK_PRIORITY jse_osal_task_get_default_priority() + 7
#define MQTT_TSK_PRIORITY jse_osal_task_get_default_priority() + 6
#define MQTTHTTP_TSK_PRIORITY jse_osal_task_get_default_priority() + 4
#define SSDP_TSK_PRIORITY jse_osal_task_get_default_priority() + 3
#define WEBSOCKET_TSK_PRIORITY jse_osal_task_get_default_priority() + 4
#define WIFI_TSK_PRIORITY jse_osal_task_get_default_priority() + 4
#define UPDATE_TSK_PRIORITY jse_osal_task_get_default_priority() + 6
#define ADDON_TSK_PRIORRITY jse_osal_task_get_default_priority() + 3

typedef void *osTimerId;
typedef void *osMessageQId;
typedef void (*os_timer_cb)(void *argument);

typedef void (*jse_osal_task_handler_t)(void *data);
typedef void (*jse_osal_action_call_t)(void *data);
typedef void (*jse_osal_event_cb_t)(void *data);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
uint32_t jse_osal_task_get_default_priority(void);

/**
 * @brief       create task
 *
 * @param[in]   name: task name
 * @param[in]   handler: task function
 * @param[in]   data: task argument
 * @param[in]   size: task stack size
 * @param[in]   task_handle: task handler
 *
 * @return      0: success, -1: failed
 */
int32_t jse_osal_create_task(const char *name, jse_osal_task_handler_t task,
                            void *data, uint32_t size, uint32_t priority,
                            void **task_handle);

/**
 * @brief       delete task
 *
 * @param[in]   handle: task handler
 *
 */
void jse_osal_delete_task(void *handle);

/**
 * @brief       get task name, call this function in task.
 *
 * @return      task name(string), NULL if no task name.
 */
char *jse_osal_get_taskname();

/* TODO */
#if 0 
void jse_osal_get_task_priority();
void jse_osal_set_task_priority();
#endif

/**
 * @brief       get clock time
 *
 * @return      clock time(ms)
 */
uint32_t jse_osal_get_clocktime();

/**
 * @brief       create mutex
 *
 * @return      mutex handler
 */
void *jse_osal_new_mutex();

/**
 * @brief       delete mutex
 *
 * @param[in]   mutex: mutex handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_del_mutex(void *mutex);

/**
 * @brief       lock mutex
 *
 * @param[in]   mutex: mutex handler
 * @param[in]   ms: timeout
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_lock_mutex(void *mutex, uint32_t ms);

/**
 * @brief       unlock mutex
 *
 * @param[in]   mutex: mutex handler
 * @param[in]   ms: timeout
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_unlock_mutex(void *mutex);

/**
 * @brief       delay ms
 *
 * @param[in]   ms: delay time(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_delay(uint32_t ms);

/**
 * @brief       delay us
 *
 * @param[in]   ms: delay time(us)
 *
 * @return      void
 */
void jse_osal_delay10us(void);

/**
 * @brief       create an timer
 *
 * @param[in]   callback: timer timeout callback function
 * @param[in]   type: timer type, one-shot timer or repeating timer
 * @param[in]   arg: callback function argument
 *
 * @return      timer handler
 */
osTimerId jse_osal_timer_create(os_timer_cb callback, jse_osal_timer_type type,
                               void *arg);

/**
 * @brief       start an timer
 *
 * @param[in]   timer_id: timer handler
 * @param[in]   millisec: timeout(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_timer_start(osTimerId timerId, uint32_t millisec);

/**
 * @brief       stop an timer
 *
 * @param[in]   timer_id: timer handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_timer_stop(osTimerId timerId);

/**
 * @brief       delete an timer
 *
 * @param[in]   timer_id: timer handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_timer_delete(osTimerId timerId);

/**
 * @brief       get an timer parameter
 *
 * @param[in]   timerHandle: timer handler
 *
 * @return      timer parameter
 */
void *jse_osal_get_timer_param(void *timerHandle);

/**
 * @brief       create queue
 *
 * @param[in]   queue_length: queue length
 * @param[in]   item_size: queue item size
 *
 * @return      queue handler
 */
osMessageQId jse_osal_messageQ_create(int32_t queue_length, int32_t item_size);

/**
 * @brief       delete queue
 *
 * @param[in]   queue_id: queue handler
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_messageQ_delete(osMessageQId queue_id);

/**
 * @brief       push data into queue
 *
 * @param[in]   queue_id: queue handler
 * @param[in]   p_info: buffer
 * @param[in]   millisec: timeout(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_messageQ_put(osMessageQId queue_id, void *p_info,
                             uint32_t millisec);

/**
 * @brief       get data from queue
 *
 * @param[in]   queue_id: queue handler
 * @param[out]  p_info: store data to buffer
 * @param[in]   millisec: timeout(ms)
 *
 * @return      0 on success, otherwise failed.
 */
int32_t jse_osal_messageQ_get(osMessageQId queue_id, void *p_info,
                             uint32_t millisec);

#if defined(__cplusplus)
}
#endif

#endif /* JSE_OSAL_H */
