/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TIMER_H
#define K_TIMER_H

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

typedef void (*timer_cb_t)(void *timer, void *arg);

typedef struct {
    klist_t       timer_list;
    klist_t      *to_head;
    const name_t *name;
    timer_cb_t    cb;
    void         *timer_cb_arg;
    sys_time_t    match;
    sys_time_t    remain;
    sys_time_t    init_count;
    sys_time_t    round_ticks;
    void         *priv;
    kobj_type_t   obj_type;
    uint8_t       timer_state;
    uint8_t       mm_alloc_flag;
} ktimer_t;

typedef struct {
    ktimer_t  *timer;
    uint8_t    cb_num;
    sys_time_t first;
    union {
        sys_time_t round;
        void      *arg;
    } u;
} k_timer_queue_cb;

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
                            sys_time_t first, sys_time_t round, void *arg, uint8_t auto_run);

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
                                sys_time_t first, sys_time_t round, void *arg, uint8_t auto_run);
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
kstat_t krhino_timer_change(ktimer_t *timer, sys_time_t first, sys_time_t round);

/**
 * This function will change attributes of a timer without stop and start
 * @param[in]  timer  pointer to the timer
 * @param[in]  first  ticks of the first timer triger
 * @param[in]  round  ticks of the normal timer triger
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_arg_change_auto(ktimer_t *timer, void *arg);

/**
 * This function will change callback arg attributes of a timer
 * @param[in]  timer  pointer to the timer
 * @param[in]  arg timer callback arg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_timer_arg_change(ktimer_t *timer, void *arg);

#endif /* K_TIMER_H */

