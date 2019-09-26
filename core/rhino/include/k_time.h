/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TIME_H
#define K_TIME_H

/**
 * This function will handle systick routine
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
void krhino_tick_proc(void);

/**
 * This function will get time of the system in ms
 * @return  system time
 */
sys_time_t krhino_sys_time_get(void);

/**
 * This function will get ticks of the system
 * @return  the system ticks
 */
sys_time_t krhino_sys_tick_get(void);

/**
 * This function will convert ms to ticks
 * @param[in]  ms  ms which will be converted to ticks
 * @return  the ticks of the ms
 */
tick_t krhino_ms_to_ticks(sys_time_t ms);

/**
 * This function will convert ticks to ms
 * @param[in]  ticks  ticks which will be converted to ms
 * @return  the ms of the ticks
 */
sys_time_t krhino_ticks_to_ms(tick_t ticks);

#endif /* K_TIME_H */

