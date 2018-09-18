/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

#ifndef K_CPU_TICKLSEE_H
#define K_CPU_TICKLSEE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <k_api.h>
#include "pwr_state.h"

    typedef pwr_status_t (*one_shot_init_t)(void);
    typedef uint32_t (*one_shot_seconds_max_t)(void);
    typedef pwr_status_t (*one_shot_start_t)(uint64_t planUse);
    typedef pwr_status_t (*one_shot_stop_t)(uint64_t *pPassedUs);

    typedef struct
    {
        one_shot_init_t        one_shot_init;
        one_shot_seconds_max_t one_shot_seconds_max;
        one_shot_start_t       one_shot_start;
        one_shot_stop_t        one_shot_stop;
    } one_shot_timer_t;

    extern pwr_status_t tickless_init(void);
    extern void         tickless_c_states_add(uint32_t c_state_set);
    extern void         tickless_one_shot_timer_save(cpu_cstate_t      c_state,
                                                     one_shot_timer_t *p_timer);


#ifdef __cplusplus
}
#endif

#endif /* K_CPU_TICKLSEE_H */
