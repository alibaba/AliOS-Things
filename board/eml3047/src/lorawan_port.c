/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "lorawan_port.h"
#include "hw_msp.h"
#include "hw_rtc.h"
#include "hw_gpio.h"
#include "hw_spi.h"
#include "timeServer.h"


/**
 * @fn     enter_stop_mode
 * @brief  enter stop mode
 * @param  None
 * @rtn    None
 */
static void enter_stop_mode()
{
    HW_EnterStopMode();
}

/**
 * @fn     exit_stop_mode
 * @brief  exit from stop mode
 * @param  None
 * @rtn    None
 */
static void exit_stop_mode()
{
    HW_ExitStopMode();
}

/**
 * @fn     enter_sleep_mode
 * @brief  enter sleep mode
 * @param  None
 * @rtn    None
 */
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

/**
 * @fn     get_timer_elapsed_time
 * @brief  get the low level time since the last alarm was set
 * @param  None
 * @rtn    The Elapsed time in ticks
 */
static uint32_t get_timer_elapsed_time()
{
    return (time_tick_t)HW_RTC_GetTimerElapsedTime();
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

static void set_timer_val(TimerEvent_t *obj, uint32_t value)
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

static TimerTime_t get_current_time(void )
{
    uint32_t now = HW_RTC_GetTimerValue( );
    return  HW_RTC_Tick2ms(now);
}

static TimerTime_t compute_elapsed_time(TimerTime_t time)
{
    uint32_t nowInTicks = HW_RTC_GetTimerValue( );
    uint32_t pastInTicks = HW_RTC_ms2Tick( time );
    /* intentional wrap around. Works Ok if tick duation below 1ms */
    return HW_RTC_Tick2ms( nowInTicks- pastInTicks );
}


static void set_timeout(TimerEvent_t *obj)
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


/**
 * @fn     radio_reset
 * @brief  reset radio by gpio
 * @param  None
 * @rtn    None
 */
static void radio_reset()
{
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode =GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;

    // Set RESET pin to 0
    HW_GPIO_Init( RADIO_RESET_PORT, RADIO_RESET_PIN, &initStruct);
    HW_GPIO_Write( RADIO_RESET_PORT, RADIO_RESET_PIN, 0 );	
}

/**
 * @fn     radio_reset_cfg_input
 * @brief  Configure radio reset pin as input
 * @param  None
 * @rtn    None
 */
static void radio_reset_cfg_input()
{
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode =GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;

    // Configure RESET as input
    initStruct.Mode = GPIO_NOPULL;
    HW_GPIO_Init( RADIO_RESET_PORT, RADIO_RESET_PIN, &initStruct);	
}

/**
 * @fn     radio_rw_en
 * @brief  enable radio data tx/rx
 * @param  None
 * @rtn    None
 */
static void radio_rw_en()
{
	//NSS = 0;
    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
}

/**
 * @fn     radio_rw_dis
 * @brief  disable radio data tx/rx
 * @param  None
 * @rtn    None
 */
static void radio_rw_dis()
{
	//NSS = 1;
    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
}

/**
 * @fn     radio_rw
 * @brief  write and read radio data 
 * @param  None
 * @rtn    None
 */
static uint16_t radio_rw(uint16_t tx_data)
{
	HW_SPI_InOut(tx_data);
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

    .stop_alarm = stop_alarm,
    .set_alarm = set_alarm,
    .set_uc_wakeup_time = set_uc_wakeup_time,

    .set_timeout = set_timeout,
    .compute_elapsed_time = compute_elapsed_time,
    .get_current_time = get_current_time,
    .set_timer_val = set_timer_val,
};

/* LoRaWan radio control */
hal_lrwan_radio_ctrl_t aos_lrwan_radio_ctrl = {
    .radio_reset = radio_reset,
    .radio_reset_cfg_input = radio_reset_cfg_input,
    .radio_rw_en = radio_rw_en,
    .radio_rw_dis = radio_rw_dis,
    .radio_rw = radio_rw,
};

