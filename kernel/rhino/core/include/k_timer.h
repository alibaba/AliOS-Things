/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TIMER_H
#define K_TIMER_H

typedef void (*timer_cb_t)(void *timer, void *arg);

typedef struct {
    klist_t       timer_list;
    klist_t      *to_head;
    const name_t *name;
    timer_cb_t    cb;
    void         *timer_cb_arg;
    tick_t        match;
    tick_t        remain;
    tick_t        init_count;
    tick_t        round_ticks;
    void         *timeout_param;
    kobj_type_t   obj_type;
    uint8_t       timer_state;
    uint8_t       mm_alloc_flag;
} ktimer_t;

typedef enum {
    TIMER_DEACTIVE = 0u,
    TIMER_ACTIVE
} k_timer_state_t;

/**
 * This function will create a timer
 * @param[in]  timer     pointer to the timer(the space is provided by user)
 * @param[in]  name      name of the timer
 * @param[in]  cb        callbak of the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the normal timer triger
 * @param[in]  arg       the argument of the callback
 * @param[in]  auto_run  auto run or not when the timer is created
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
                            tick_t first, tick_t round, void *arg, uint8_t auto_run);

/**
 * This function will delete a timer
 * @param[in]  timer  pointer to a timer
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_del(ktimer_t *timer);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will create a dyn-timer
 * @param[in]  timer     pointer to the timer
 * @param[in]  name      name of the timer
 * @param[in]  cb        callbak of the timer
 * @param[in]  first     ticks of the first timer triger
 * @param[in]  round     ticks of the normal timer triger
 * @param[in]  arg       the argument of the callback
 * @param[in]  auto_run  auto run or not when the timer is created
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name,
                                timer_cb_t cb,
                                tick_t first, tick_t round, void *arg, uint8_t auto_run);
/**
 * This function will delete a dyn-timer
 * @param[in]  timer  pointer to a timer
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_dyn_del(ktimer_t *timer);
#endif

/**
 * This function will start a timer
 * @param[in]  timer  pointer to the timer
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_start(ktimer_t *timer);

/**
 * This function will stop a timer
 * @param[in]  timer  pointer to the timer
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_stop(ktimer_t *timer);

/**
 * This function will change attributes of a timer
 * @param[in]  timer  pointer to the timer
 * @param[in]  first  ticks of the first timer triger
 * @param[in]  round  ticks of the normal timer triger
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_change(ktimer_t *timer, tick_t first, tick_t round);

/**
 * This function will change callback arg attributes of a timer
 * @param[in]  timer  pointer to the timer
 * @param[in]  arg timer callback arg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_arg_change(ktimer_t *timer, void *arg);

#endif /* K_TIMER_H */

