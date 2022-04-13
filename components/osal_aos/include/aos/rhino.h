/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_RHINO_H
#define AOS_RHINO_H

#include <stdint.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get a task pthread control block.
 *
 * @param[in]   task        task handle
 * @param[out]  ptcb        the returned ptcb handle
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb);

/**
 * Set a task pthread control block.
 *
 * @param[in]   task        task handle
 * @param[in]   ptcb        the ptcb handle
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb);

/**
 * Change a task's schedule priority
 *
 * @param[in]   task        task handle
 * @param[in]   pri         the new task priority
 * @param[out]  old_pri     the returned old task priority
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri);

/**
 * Get a task's schedule priority
 *
 * @param[in]   task        task handle
 * @param[out]  priority    the returned old task priority
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority);

/**
 * Set a task's schedule policy
 *
 * @param[in]   task        task handle
 * @param[in]   policy      the new task's policy
 * @param[in]   pri         the new task's priority
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri);

/**
 * Get a task's schedule policy
 *
 * @param[in]   task        task handle
 * @param[out]  policy      the returned task's policy
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy);

/**
 * Get a task's default schedule policy
 *
 * @return  KSCHED_CFS or KSCHED_RR
 */
uint32_t aos_task_sched_policy_get_default();

/**
 * Set a task's schedule time slice
 *
 * @param[in]   task        task handle
 * @param[in]   slice       the new schedule time slice
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice);

/**
 * Get a task's schedule time slice
 *
 * @param[in]   task        task handle
 * @param[out]  slice       the returned task's time slice
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice);

/**
 * Get the maximum number of system's schedule priority
 *
 * @param[in]   task        task handle
 * @param[in]   policy      the task's schedule policy
 *
 * @return  maximum schedule priority
 */
uint32_t aos_sched_get_priority_max(uint32_t policy);

/**
 * Get the minimum number of system's schedule priority
 *
 * @param[in]   task        task handle
 * @param[in]   policy      the task's schedule policy
 *
 * @return  minimum schedule priority
 */
static inline uint32_t aos_sched_get_priority_min(uint32_t policy)
{
    return 1;
}

/**
 * Get the timer's time vaule.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[out] value  store the returned time.
 *
 * @return  0: success, otherwise error.
 */
aos_status_t aos_timer_gettime(aos_hdl_t *timer, uint64_t value[]);

#ifdef __cplusplus
}
#endif
#endif /*AOS_RHINO_H*/
