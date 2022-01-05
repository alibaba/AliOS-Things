/**
 * @file timer.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_HAL_TIMER_INTERNAL_H
#define AOS_HAL_TIMER_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_timer TIMER
 *  timer hal API.
 *
 *  @{
 */

#include <stdint.h>

#define TIMER_RELOAD_AUTO  1 /**< timer reload automatic */
#define TIMER_RELOAD_MANU  0 /**< timer reload manual */

/* Define timer handle function type */
typedef void (*hal_timer_cb_t)(void *arg);

/* Define timer config args */
typedef struct {
    uint64_t        period;         /**< timer period, us */
    uint8_t         reload_mode;    /**< auto reload or not */
    void            *handle;
    hal_timer_cb_t  cb;             /**< timer handle when expired */
    void           *arg;            /**< timer handle args */
} aos_hal_timer_config_t;

/* Define timer dev handle */
typedef struct {
    int8_t          port;   /**< timer port */
    aos_hal_timer_config_t  config; /**< timer config */
    void           *priv;   /**< priv data */
} aos_hal_timer_dev_t;

/**
 * init a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_timer_init(aos_hal_timer_dev_t *tim);

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_timer_start(aos_hal_timer_dev_t *tim);

/**
 * stop a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  none
 */
void aos_hal_timer_stop(aos_hal_timer_dev_t *tim);

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_timer_reload(aos_hal_timer_dev_t *tim);

/**
 * change the config of a hardware timer
 *
 * @param[in]  tim   timer device
 * @param[in]  para  timer config
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_timer_para_chg(aos_hal_timer_dev_t *tim, aos_hal_timer_config_t para);

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t aos_hal_timer_finalize(aos_hal_timer_dev_t *tim);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_HAL_TIMER_H */

