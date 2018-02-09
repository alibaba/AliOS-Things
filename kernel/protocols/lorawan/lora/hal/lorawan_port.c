/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "lorawan_port.h"
#include "hw_msp.h"
#include "hw_rtc.h"


static void enter_stop_mode()
{
    HW_EnterStopMode();
}

static void exit_stop_mode()
{
    HW_ExitStopMode();
}

static void enter_sleep_mode()
{
    HW_EnterSleepMode();
}

/**
 * @fn     delay_ms
 * @brief  Delay of delay ms 
 * @param  Delay in ms
 * @rtn    None
 */
static void delay_ms(time_ms_t delay)
{
    HW_RTC_DelayMs(delay);
}

/**
 * @fn     set_timer_context
 * @brief  set timer reference 
 * @param  None
 * @rtn    Timer Reference Value in Ticks
 */
static uint32_t set_timer_context()
{
    return (time_tick_t)HW_RTC_SetTimerContext();
}

/**
 * @fn     get_timer_context
 * @brief  get timer reference 
 * @param  None
 * @rtn    Timer Reference Value in Ticks
 */
static uint32_t get_timer_context()
{
    return (time_tick_t)HW_RTC_GetTimerContext();
}

/*  */
/**
 * @fn     get_timer_elapsed_time
 * @brief  Gget the low level time since the last alarm was set
 * @param  None
 * @rtn    The Elapsed time in ticks
 */
static uint32_t get_timer_elapsed_time()
{
    return (time_tick_t)HW_RTC_GetTimerElapsedTime();
}

/**
 * @fn     ms2tick
 * @brief  Converts time in ms to time in ticks
 * @param  Time in milliseconds
 * @rtn    Returns time in timer ticks
 */
static uint32_t ms2tick(uint32_t u_sec)
{
    return (time_tick_t)HW_RTC_ms2Tick(u_sec);

}

/**
 * @fn     tick2ms
 * @brief  Converts time in ticks to time in ms
 * @param  Time in ticks
 * @rtn    Time in timer milliseconds
 */
static uint32_t tick2ms(uint32_t tick) 
{
    return (time_ms_t)HW_RTC_Tick2ms(tick);
}


/**
 * @fn     get_min_timeout
 * @brief  Return the minimum timeout(uC wake up time) 
 *	       the low lower timer is able to handle
 * @param  None
 * @rtn    Minimum value for a timeout
 */
static uint32_t get_min_timeout()
{
    return (time_tick_t)HW_RTC_GetMinimumTimeout(); 
}

/**
 * @fn     get_timer_val
 * @brief  Get the low level timer value
 * @param  None
 * @rtn    Timer value in ticks
 */
static uint32_t get_timer_val()
{
    return (time_tick_t)HW_RTC_GetTimerValue();
}

/**
 * @fn     set_uc_wakeup_time
 * @brief  Calculates the wake up time between wake up and mcu start
 * @param  None
 * @rtn    None
 */
static void set_uc_wakeup_time()
{
    HW_RTC_setMcuWakeUpTime();
}

/**
 * @fn     set_alarm
 * @brief  Set the alarm
 * @param  Timeout Duration of the Timer ticks
 * @rtn    None
 */
static void set_alarm(uint32_t timeout)
{
    HW_RTC_SetAlarm(timeout);
}

/**
 * @fn     stop_alarm
 * @brief  Stop the Alarm
 * @param  None
 * @rtn    None
 */
static void stop_alarm()
{
    HW_RTC_StopAlarm();
}

/* the struct is for changing the device working mode */
hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode = {
    .enter_stop_mode  = enter_stop_mode,
    .exit_stop_mode   = exit_stop_mode, 
    .enter_sleep_mode = enter_sleep_mode,
};

/* LoRaWan time and timer interface */
hal_lrwan_time_itf_t aos_lrwan_time_itf = {
    .delay_ms = delay_ms,
    .set_timer_context = set_timer_context,
    .get_timer_context = get_timer_context,
    .get_timer_elapsed_time = get_timer_elapsed_time,
    .ms2tick = ms2tick,
    .tick2ms = tick2ms,
    .get_min_timeout = get_min_timeout,
    .get_timer_val = get_timer_val,
    .stop_alarm = stop_alarm,
    .set_alarm = set_alarm,
    .set_uc_wakeup_time = set_uc_wakeup_time,
};


