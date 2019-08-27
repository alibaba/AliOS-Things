#ifndef __PMU_H
#define __PMU_H

#include "stdint.h"

typedef enum pmu_state
{
    PMU_STATE_ACTIVE = 0,
    PMU_STATE_MODEMSLEEP,
    PMU_STATE_LIGHTSLEEP,
    PMU_STATE_DEEPSLEEP,
    PMU_STATE_LAST
}pmu_state_t;

/** \brief  Wait For Interrupt

    Wait For Interrupt is a hint instruction that suspends execution
    until one of a number of events occurs.
 */
__attribute__( ( always_inline ) ) static inline void __WFI__(void)
{
    __asm volatile( "dsb" );
    __asm volatile( "wfi" );
    __asm volatile( "isb" );
}

/** @brief  get current RTC counter, only used when RTC enabled.
 *
 *  @return   : current RTC counter
 */
uint32_t lega_drv_rtc_counter_get();

/** @brief  get MAX RTC counter we can set, only used when RTC enabled.
 *
 *  @return   : MAX RTC counter we can set
 */
uint32_t lega_drv_rtc_max_ticks_get();

/** @brief  get MAX RTC counter we can set, only used when RTC enabled.
 *  @param    cc_counter: RTC counter to set.
 *  @param    whitch state you want goto, default modemsleep.
 *  @return   0: set OK.
 *  @return   other: set error.
 */
uint16_t lega_drv_rtc_cc_set(uint32_t cc_counter, pmu_state_t pmu_state);

/** @brief  enable RTC clock, used after lega_drv_rtc_cc_set.
 */
void lega_drv_rtc_enable();

/** @brief  goto sleep state.
 *  @param  pmu_state: state to set:MODEMSLEEP/LIGHTSLEEP/DEEPSLEEP.
 *  @param  pmu_state: state to set:MODEMSLEEP/LIGHTSLEEP/DEEPSLEEP.
 */
void lega_drv_goto_sleep(pmu_state_t pmu_state);

/** @brief  goto active state, call when waked from SLEEP states.
 */
void lega_drv_goto_active();
#endif //__PMU_H





























