/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_OS_H_
#define _AIS_OS_H_

#include <stdint.h>

typedef struct {
    void *hdl;
} os_hdl_t;

typedef os_hdl_t os_timer_t;

/* Timer callback */
typedef void (*os_timer_cb_t)(void *, void *);

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

/**
 * Generate random number.
 *
 * @return  random value implemented by platform.
 */
int os_rand(void);

#endif
