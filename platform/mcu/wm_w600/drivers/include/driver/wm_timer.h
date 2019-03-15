/**
 * @file    wm_timer.h
 *
 * @brief   Timer Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_TIMER_H
#define WM_TIMER_H

/** invalid timer id */
#define WM_TIMER_ID_INVALID              0xFF

/** timer interrupt callback */
typedef void (*tls_timer_irq_callback)(void *arg);

/** timer unit */
enum tls_timer_unit{
    TLS_TIMER_UNIT_US = 0,    /**< microsecond level(us) */
    TLS_TIMER_UNIT_MS         /**< millisecond level(ms) */
};

/** timer configuration */
struct tls_timer_cfg {
    enum tls_timer_unit unit;           /**< timer accuracy */
    u32  timeout;                       /**< timeout period */
    bool is_repeat;                     /**< cycle timer */
    tls_timer_irq_callback callback;    /**< timeout callback function */
    void *arg;                          /**< parameter fot the timeout callback function */
};

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup TIMER_Driver_APIs TIMER Driver APIs
 * @brief TIMER driver APIs
 */

/**
 * @addtogroup TIMER_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to create the timer
 *
 * @param[in]      cfg     timer configuration
 *
 * @retval         WM_TIMER_ID_INVALID     failed
 * @retval         other                   timer id
 *
 * @note
 * user not need clear interrupt flag.
 * timer callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
u8 tls_timer_create(struct tls_timer_cfg *cfg);

/**
 * @brief          This function is used to start the timer
 *
 * @param[in]      timer_id    timer id
 *
 * @return         None
 *
 * @note           None
 */
void tls_timer_start(u8 timer_id);

/**
 * @brief          This function is used to stop the timer
 *
 * @param[in]      timer_id    timer id
 *
 * @return         None
 *
 * @note           None
 */
void tls_timer_stop(u8 timer_id);

/**
 * @brief          This function is used to delete the timer
 *
 * @param[in]      timer_id    timer id
 *
 * @return         None
 *
 * @note           None
 */
void tls_timer_destroy(u8 timer_id);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_TIMER_H */

