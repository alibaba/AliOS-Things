/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_cmu.h"

//=============================================================================
// Slow Timer (Default Timer)

#ifdef FPGA
#define CONFIG_SYSTICK_HZ_NOMINAL   (32000)
#else
#define CONFIG_SYSTICK_HZ_NOMINAL   (16000)
#endif

//#if (CONFIG_SYSTICK_HZ_NOMINAL % 1000)
//#error "Bad CONFIG_SYSTICK_HZ_NOMINAL configuration"
//#endif

#ifdef CALIB_SLOW_TIMER

#define CONFIG_SLOW_SYSTICK_HZ      hal_sys_timer_systick_hz()

#define __MS_TO_TICKS(ms)           hal_sys_timer_ms_to_ticks(ms)

#define __US_TO_TICKS(us)           hal_sys_timer_us_to_ticks(us)

#define __TICKS_TO_MS(tick)         hal_sys_timer_ticks_to_ms(tick)

#define __TICKS_TO_US(tick)         hal_sys_timer_ticks_to_us(tick)

#else

#define CONFIG_SLOW_SYSTICK_HZ      CONFIG_SYSTICK_HZ_NOMINAL

#define __MS_TO_TICKS(ms)           ((ms) * ((uint32_t)CONFIG_SLOW_SYSTICK_HZ / 1000))

#define __US_TO_TICKS(us)           (((us) * ((uint32_t)CONFIG_SLOW_SYSTICK_HZ / 1000) + 1000 - 1) / 1000 + 1)

#define __TICKS_TO_MS(tick)         ((tick) / ((uint32_t)CONFIG_SLOW_SYSTICK_HZ / 1000))

#define __TICKS_TO_US(tick)         ((tick) * 1000 / ((uint32_t)CONFIG_SLOW_SYSTICK_HZ / 1000))

#endif

/*
 *
 * This is very confused with the common sense, because
 * MS_TO_TICKS is always refer to ms converted to os ticks
 * but here it is converted to a hardware timer's tick
 * which ticks is 16K one second;
 *
 * The same is as US_TO_TICKS, TICKS_TO_MS series;
 * They are reserved for historic reason;
 *
 * Note, don't use these macros, use MS_TO_HWTICKS/US_TO_HWTICKS/HWTICKS_TO_MS
 * alternately
 */
#define MS_TO_TICKS(ms)             __MS_TO_TICKS(ms)

#define US_TO_TICKS(us)             __US_TO_TICKS(us)

#define TICKS_TO_MS(tick)           __TICKS_TO_MS(tick)

#define TICKS_TO_US(tick)           __TICKS_TO_US(tick)

#define MS_TO_HWTICKS(ms)           __MS_TO_TICKS(ms)

#define US_TO_HWTICKS(us)             __US_TO_TICKS(us)

#define HWTICKS_TO_MS(tick)           __TICKS_TO_MS(tick)

#define HWTICKS_TO_US(tick)           __TICKS_TO_US(tick)

#define GET_CURRENT_TICKS()         hal_sys_timer_get()

#define GET_CURRENT_MS()            TICKS_TO_MS(GET_CURRENT_TICKS())

uint32_t hal_systick_timer_get(void);

void hal_sys_timer_open(void);

uint32_t hal_sys_timer_get(void);

uint32_t hal_sys_timer_get_in_sleep(void);

uint32_t hal_sys_timer_get_max(void);

void hal_sys_timer_delay(uint32_t ticks);

void hal_sys_timer_delay_in_sleep(uint32_t ticks);

void hal_sys_timer_delay_us(uint32_t us);

void hal_sys_timer_delay_ns(uint32_t ns);

uint32_t hal_sys_timer_calc_cpu_freq(uint32_t interval_ms, int high_res);

uint32_t flash_hal_sys_timer_get(void);

void flash_hal_sys_timer_delay(uint32_t ticks);

void hal_sys_timer_calib_start(void);

int hal_sys_timer_calib_end(void);

void hal_sys_timer_calib(void);

uint32_t hal_sys_timer_systick_hz(void);

uint32_t hal_sys_timer_ms_to_ticks(uint32_t ms);

uint32_t hal_sys_timer_us_to_ticks(uint32_t us);

uint32_t hal_sys_timer_ticks_to_ms(uint32_t tick);

uint32_t hal_sys_timer_ticks_to_us(uint32_t tick);

uint32_t hal_timer_get_passed_ticks(uint32_t curr_ticks, uint32_t prev_ticks);

//=============================================================================
// Fast Timer

#define CONFIG_FAST_SYSTICK_HZ      (hal_cmu_get_crystal_freq() / 4)

#define MS_TO_FAST_TICKS(ms)        ((uint32_t)(ms) * (CONFIG_FAST_SYSTICK_HZ / 1000))

#define US_TO_FAST_TICKS(us)        ((uint32_t)(us) * (CONFIG_FAST_SYSTICK_HZ / 1000 / 100) / 10)

#define NS_TO_FAST_TICKS(ns)        ((uint32_t)(ns) * (CONFIG_FAST_SYSTICK_HZ / 1000 / 100) / 10 / 1000)

#define FAST_TICKS_TO_MS(tick)      ((uint32_t)(tick) / (CONFIG_FAST_SYSTICK_HZ / 1000))

#define FAST_TICKS_TO_US(tick)      ((uint32_t)(tick) * 10 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))

#define FAST_TICKS_TO_NS(tick)      ((uint32_t)(tick) * 10 * 1000 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))

uint32_t hal_fast_sys_timer_get(void);

#ifndef RTOS
int osDelay(uint32_t ms);
#endif

typedef void (*SYS_TICK_HANDLER)(void);

void hal_systick_timer_open(uint32_t freq, SYS_TICK_HANDLER handler);
void hal_systick_timer_start();
void hal_systick_timer_stop();
#ifdef __cplusplus
}
#endif

#endif

