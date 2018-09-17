/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_OS_H_
#define _AIS_OS_H_

#include <stdint.h>

typedef struct
{
    void *hdl;
} os_hdl_t;

typedef os_hdl_t os_timer_t;

/* Timer callback */
typedef void (*os_timer_cb_t)(void *, void *);

typedef enum
{
    OS_EV_BLE,
    OS_EV_COMBO,
    OS_EV_AUTH,
    /* Add more event type hereafter */
} os_event_type_t;

typedef enum
{
    /* BLE event code, reserved 0 to 0x000f */
    OS_EV_CODE_BLE_TX_COMPLETED = 0,
    /* COMBO event code, reserved 0x0010 - 0x001f */
    OS_EV_CODE_COMBO_AP_INFO_READY = 0x0010,
    /* AUTH event code, reserved 0x0020 - 0x002f */
    OS_EV_CODE_AUTH_DONE = 0x0020,
    OS_EV_CODE_AUTH_KEY_UPDATE,
    OS_EV_CODE_AUTH_ERROR,
    /* Add more event code hereafter */
    OS_EV_CODE_MAX = 0xffff
} os_event_code_t;

typedef struct
{
    /* Event type, os_event_type_t */
    uint16_t type;
    /* Event code, os_event_code_t */
    uint16_t code;
    /* User data, defined according to type/code */
    unsigned long value;
} os_event_t;

/* Asynchronous event callback */
typedef void (*os_event_cb_t)(os_event_t *event, void *private_data);

/**
 * This function will create a timer.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 * @param[in]  auto_run  run auto or not when the timer is created.
 *
 * @return  0: success.
 */
int os_timer_new(os_timer_t *timer, os_timer_cb_t cb, void *arg, int ms);

/**
 * This function will start a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int os_timer_start(os_timer_t *timer);

/**
 * This function will stop a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int os_timer_stop(os_timer_t *timer);

/**
 * This function will delete a timer.
 *
 * @param[in]  timer  pointer to a timer.
 */
void os_timer_free(os_timer_t *timer);

/**
 * Reboot system.
 */
void os_reboot();

/**
 * Msleep.
 *
 * @param[in]  ms  sleep time in milliseconds.
 */
void os_msleep(int ms);

/**
 * Get current time in mini seconds.
 *
 * @return  elapsed time in mini seconds from system starting.
 */
long long os_now_ms();

/**
 * Post an asynchronous event. The implementation must ensure
 * the event can be receviced by the event filter.
 *
 * @param[in]  type   event type.
 * @param[in]  code   event code.
 * @param[in]  value  event value.
 *
 * @return  the operation status, 0 is OK, others is error.
 */
int os_post_event(os_event_type_t type, os_event_code_t code,
                  unsigned long value);

/**
 * Register system event filter callback. The callback is expected to be
 * called once the event is post/triggered.
 *
 * Note: it's implementation dependent how the events are dispatched.
 *       e.g. event dispatcher as a distinguished thread, or in a main loop.
 *
 * @param[in]  type  event type interested.
 * @param[in]  cb    system event callback.
 * @param[in]  priv  private data past to cb.
 *
 * @return  the operation status, 0 is OK, others is error.
 */
int os_register_event_filter(os_event_type_t type, os_event_cb_t cb,
                             void *priv);

/**
 * Post a delayed action to be executed in event thread.
 *
 * @param[in]  ms      milliseconds to wait, -1 means don't care.
 * @param[in]  action  action to be executed.
 * @param[in]  arg     private data past to action.
 */
void os_post_delayed_action(int ms, void (*action)(void *arg), void *arg);

/**
 * Start a event thread.
 */
void os_start_event_dispatcher();

/**
 * Add a new KV pair.
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  value  the value of the KV pair.
 * @param[in]  len    the length of the value.
 * @param[in]  sync   save the KV pair to flash right now (should always be 1).
 *
 * @return  0 on success, negative error on failure.
 */
int os_kv_set(const char *key, const void *value, int len, int sync);

/**
 * Get the KV pair's value stored in buffer by its key.
 *
 * @note: the buffer_len should be larger than the real length of the value,
 *        otherwise buffer would be NULL.
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     buffer      the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int os_kv_get(const char *key, void *buffer, int *buffer_len);

/**
 * Delete the KV pair by its key.
 *
 * @param[in]  key  the key of the KV pair to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int os_kv_del(const char *key);

#endif
