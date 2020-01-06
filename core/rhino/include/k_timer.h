/**
 * @file k_timer.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_TIMER_H
#define K_TIMER_H

/** @addtogroup aos_rhino timer
 *  Timer management
 *
 *  @{
 */

/**
 * Timer operation
 */
enum {
    TIMER_CMD_CB = 0u,
    TIMER_CMD_START,
    TIMER_CMD_STOP,
    TIMER_CMD_CHG,
    TIMER_ARG_CHG,
    TIMER_ARG_CHG_AUTO,
    TIMER_CMD_DEL,
    TIMER_CMD_DYN_DEL
};

/**
 * Timer callback
 */
typedef void (*timer_cb_t)(void *timer, void *arg);

/**
 * Timer object
 */
typedef struct {
    /**<
     *  When the timer is active, timer_list is linked from g_timer_head,
     *  and sort by 'match'(timeout time).
     */
    klist_t       timer_list;
    klist_t      *to_head;
    const name_t *name;
    timer_cb_t    cb;
    void         *timer_cb_arg;
    tick_t        match;        /**< Expected timeout point */
    tick_t        remain;
    tick_t        init_count;
    tick_t        round_ticks;  /**< Timer period */
    void         *priv;
    kobj_type_t   obj_type;
    uint8_t       timer_state;  /**< TIMER_DEACTIVE or TIMER_ACTIVE */
    uint8_t       mm_alloc_flag;
} ktimer_t;

/**
 * Describe a timer operation
 */
typedef struct {
    ktimer_t   *timer;
    uint8_t     cb_num;         /**< TIMER_CMD_START/STOP/... */
    tick_t      first;

    union {
        tick_t  round;
        void   *arg;
    } u;
} k_timer_queue_cb;

typedef enum {
    TIMER_DEACTIVE = 0u,
    TIMER_ACTIVE
} k_timer_state_t;

/**
 * Create a timer.
 *
 * @param[in]  timer     pointer to the timer (the space is provided outside, by user)
 * @param[in]  name      name of the timer
 * @param[in]  cb        callbak of the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the timer period, 0 means one-shot timer
 * @param[in]  arg       the argument of the callback
 * @param[in]  auto_run  auto run or not when the timer is created
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
                            tick_t first, tick_t round, void *arg, uint8_t auto_run);

/**
 * Delete a timer.
 *
 * @param[in]  timer  pointer to a timer
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_del(ktimer_t *timer);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Create and malloc a timer.
 *
 * @param[out] timer     pointer to the timer (the space is provided inside, from heap)
 * @param[in]  name      name of the timer
 * @param[in]  cb        callbak of the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the timer period, 0 means one-shot timer
 * @param[in]  arg       the argument of the callback
 * @param[in]  auto_run  auto run or not when the timer is created
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name, timer_cb_t cb,
                                tick_t first, tick_t round, void *arg, uint8_t auto_run);
/**
 * Delete and free a timer.
 *
 * @param[in]  timer  pointer to a timer
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_dyn_del(ktimer_t *timer);
#endif

/**
 * This function will start a timer.
 *
 * @param[in]  timer  pointer to the timer
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_start(ktimer_t *timer);

/**
 * This function will stop a timer.
 *
 * @param[in]  timer  pointer to the timer
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_stop(ktimer_t *timer);

/**
 * Change attributes of a timer.
 * @note this func should follow the sequence as timer_stop -> timer_change -> timer_start
 *
 * @param[in]  timer     pointer to the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the timer period, 0 means one-shot timer
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_change(ktimer_t *timer, tick_t first, tick_t round);

/**
 * Change attributes of a timer without timer_stop and timer_start.
 *
 * @param[in]  timer     pointer to the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the timer period, 0 means one-shot timer
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_arg_change_auto(ktimer_t *timer, void *arg);

/**
 * Change callback arg attributes of a timer.
 *
 * @param[in]  timer     pointer to the timer
 * @param[in]  arg       timer callback arg
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_arg_change(ktimer_t *timer, void *arg);

/** @} */

#endif /* K_TIMER_H */

