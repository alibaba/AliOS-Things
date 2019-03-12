/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define TIMER_RELOAD_AUTO  1 /* timer reload automatic */
#define TIMER_RELOAD_MANU  2 /* timer reload manual */

typedef void (*hal_timer_cb_t)(void *arg);

typedef struct {
    uint32_t        period; /* us */
    uint8_t         reload_mode;
    hal_timer_cb_t  cb;
    void           *arg;
} timer_config_t;

typedef struct {
    int8_t          port;   /* timer port */
    timer_config_t  config; /* timer config */
    void           *priv;   /* priv data */
} timer_dev_t;

/**
 * init a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_init(timer_dev_t *tim);

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_start(timer_dev_t *tim);

/**
 * stop a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  none
 */
void hal_timer_stop(timer_dev_t *tim);

/**
 * change the config of a hardware timer
 *
 * @param[in]  tim   timer device
 * @param[in]  para  timer config
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para);

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_finalize(timer_dev_t *tim);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TIMER_H */

