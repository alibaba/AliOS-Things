/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    lorawan_timer_port.c
 * @brief   LoRaWan timer port
 * @version V1.0.0
 * @author  liucp
 * @date    20180404
 * @warning none
 * @note <b>history:</b>
 * 1. create file
 *
 */
 
/*******************************************************************************
 * INCLUDES
 */
//#include "lorawan_timer_port.h"
#include "lorawan_port.h"
#include "hw_msp.h"
#include "hw_rtc.h"
#include "timeServer.h"

/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */



/*******************************************************************************
 * TYPEDEFS
 */

/**
 * \defgroup GROUP_LOCAL_FUNCTIONS DRIVER:BOARD:LOCAL_FUNCTIONS
 */


/*******************************************************************************
 * CONSTANTS
 */
/*******************************************************************************
 * LOCAL FUNCTIONS DECLEAR
 */

/**
 * \brief set timer value
 * \param [in] obj - timer object
 * \param [in] value - timer value
 * \return none
 */
static void set_timer_val(TimerEvent_t *obj, uint32_t value);

/**
 * \brief get current time
 * \return current time in ms
 */
static TimerTime_t get_current_time(void );

/**
 * \brief compute elapsed time from time specific
 * \param [in] time - specific time
 * \return elapsed time in ms
 */
static TimerTime_t compute_elapsed_time(TimerTime_t time);

/**
 * \brief set timeout
 * \param [in] obj - time object
 * \return none
 */
static void set_timeout(TimerEvent_t *obj);

/**
 * \brief get elapsed time from last alarm time
 * \return elapsed time from last alarm time in ms
 */
static TimerTime_t get_elapsed_alarm_time(void);

/**
 * \brief delay ms
 * \param [in] delay - time delay in ms
 */
static void delay_ms(time_ms_t delay);

/**
 * \brief set timer context, not used now
 * \return context time im ms
 */
static uint32_t set_timer_context(void);

/**
 * \brief get timer context, not used now
 * \return context time in ms
 */
static uint32_t get_timer_context(void);

/**
 * \brief get timer delta value
 * \return context time in ms
 */
uint32_t get_delta_context(uint32_t now, uint32_t old);

/**
 * \brief get timer elapsed time
 * \return elapsed time
 */
static uint32_t get_timer_elapsed_time(void);

/**
 * \brief set wakeup time
 * \return none
 */
static void set_uc_wakeup_time(void);

/**
 * \brief set alarm tick
 * \param [in] timeout - timeout value
 */
static void set_alarm(uint32_t timeout);

/**
 * \brief stop alarm
 * \return none
 */
static void stop_alarm(void);

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/* LoRaWan time and timer interface */
hal_lrwan_time_itf_t aos_lrwan_time_itf = {
    .delay_ms = delay_ms,
    .set_timer_context = set_timer_context,
    .get_timer_context = get_timer_context,
    .get_delta_context = get_delta_context,
    .get_timer_elapsed_time = get_timer_elapsed_time,
    .stop_alarm = stop_alarm,
    .set_alarm = set_alarm,
    .set_uc_wakeup_time = set_uc_wakeup_time,
    .set_timeout = set_timeout,
    .compute_elapsed_time = compute_elapsed_time,
    .get_current_time = get_current_time,
    .set_timer_val = set_timer_val
};

/*******************************************************************************
 * STATIC VARIABLES
 */
/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;




/*******************************************************************************
 * EXTERNAL VARIABLES
 */


/*******************************************************************************
 *  GLOBAL FUNCTIONS IMPLEMENTATION
 */


/*******************************************************************************
 * LOCAL FUNCTIONS IMPLEMENTATION
 */

void set_timer_val(TimerEvent_t *obj, uint32_t value)
{
    uint32_t minValue = 0;
    uint32_t ticks = HW_RTC_ms2Tick( value );    

    minValue = HW_RTC_GetMinimumTimeout( );

    if( ticks < minValue )
    {
        ticks = minValue;
    }

    obj->Timestamp = ticks;
    obj->ReloadValue = ticks;
}

TimerTime_t get_current_time(void )
{
    uint32_t now = HW_RTC_GetTimerValue( );
    return  HW_RTC_Tick2ms(now);
}

TimerTime_t compute_elapsed_time(TimerTime_t time)
{
    uint32_t nowInTicks = HW_RTC_GetTimerValue( );
    uint32_t pastInTicks = HW_RTC_ms2Tick( time );
    /* intentional wrap around. Works Ok if tick duation below 1ms */
    return HW_RTC_Tick2ms( nowInTicks- pastInTicks );
}

void set_timeout(TimerEvent_t *obj)
{
    int32_t minTicks = HW_RTC_GetMinimumTimeout( );
    obj->IsRunning = true; 

    //in case deadline too soon
    if(obj->Timestamp  < (aos_lrwan_time_itf.get_timer_elapsed_time(  ) + minTicks) )
    {
        obj->Timestamp = aos_lrwan_time_itf.get_timer_elapsed_time(  ) + minTicks;
    }
    aos_lrwan_time_itf.set_alarm( obj->Timestamp );
}

TimerTime_t get_elapsed_alarm_time(void)
{
    /* by fix liwp ：convert to ms for operation */
    return HW_RTC_Tick2ms( HW_RTC_getElapsedAlarmTime() );
}

void delay_ms(time_ms_t delay)
{
    HW_RTC_DelayMs(delay);
}

uint32_t set_timer_context(void)
{
    return (time_tick_t)HW_RTC_SetTimerContext();
}

uint32_t get_timer_context(void)
{
    return (time_tick_t)HW_RTC_GetTimerContext();
}

uint32_t get_delta_context(uint32_t now, uint32_t old)
{
    uint32_t delta_ticks;

    if (now >= old) {
        delta_ticks = now - old;
    } else {
        delta_ticks = 0xFFFFFFFF -(old - now);
    }

    return delta_ticks;
}

uint32_t get_timer_elapsed_time(void)
{
    return (time_tick_t)HW_RTC_GetTimerElapsedTime();
}

void set_uc_wakeup_time(void)
{
    HW_RTC_setMcuWakeUpTime();
}

void set_alarm(uint32_t timeout)
{
    HW_RTC_SetAlarm(timeout);
}

void stop_alarm(void)
{
    HW_RTC_StopAlarm();
}
