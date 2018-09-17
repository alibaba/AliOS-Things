/*!
 * \file      rtc-board.c
 *
 * \brief     Target board RTC timer and low power modes management
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <math.h>
#include <time.h>
#include "hw.h"
#include "low_power.h"
#include "timeServer.h"
#include "delay.h"
#include "dbg_port.h"


/**
 * @brief  Type structure, using LL
 * @note   The 1st members correspond exactly to LL_RTC_TimeTypeDef sructure
 *         so that LL_RTC_TIME_Init() can be used easily in the following
 */
typedef struct
{
    uint32_t TimeFormat; /*< as defined in LL_RTC_TimeTypeDef */
    uint8_t Hours; /*< as defined in LL_RTC_TimeTypeDef */
    uint8_t Minutes; /*< as defined in LL_RTC_TimeTypeDef */
    uint8_t Seconds; /*< as defined in LL_RTC_TimeTypeDef */
    uint32_t SubSeconds; /*< subseconds, as LL_RTC_TimeTypeDef does not save them */
} HW_RTC_TimeTypeDef;
typedef LL_RTC_DateTypeDef HW_RTC_DateTypeDef;

/**
 * @brief Timer context struture, containing time and date
 */
typedef struct
{
    TimerTime_t Rtc_Time; /*< Reference time */
    HW_RTC_TimeTypeDef RTC_Calndr_Time; /*< Reference time in calendar format */
    HW_RTC_DateTypeDef RTC_Calndr_Date; /*< Reference date in calendar format */
} RtcTimerContext_t;

/* Private define ------------------------------------------------------------*/

/* MCU Wake Up Time */
#define MIN_ALARM_DELAY               3 /* in ticks */

/* subsecond number of bits */
#define N_PREDIV_S                 10

/* Synchonuous prediv  */
#define PREDIV_S                  ((1 << N_PREDIV_S) - 1)

/* Asynchonuous prediv   */
#define PREDIV_A                  ((1 << (15 - N_PREDIV_S)) - 1)

/* Sub-second mask definition  */
#if  (N_PREDIV_S == 10)
#define HW_RTC_ALARMSUBSECONDMASK_SS14_10     ((uint32_t) (RTC_ALRMASSR_MASKSS_1 | RTC_ALRMASSR_MASKSS_3))
#define HW_RTC_ALARMSUBSECONDMASK HW_RTC_ALARMSUBSECONDMASK_SS14_10
#else
#error "Please define HW_RTC_ALARMSUBSECONDMASK"
#endif

/* RTC Time base in us */
#define USEC_NUMBER               1000000
#define MSEC_NUMBER               ( USEC_NUMBER / 1000 )
#define RTC_ALARM_TIME_BASE       ( USEC_NUMBER >> N_PREDIV_S )

#define COMMON_FACTOR             3
#define CONV_NUMER                ( MSEC_NUMBER >> COMMON_FACTOR )
#define CONV_DENOM                ( 1 << ( N_PREDIV_S - COMMON_FACTOR ) )

#if defined (USE_B_M400)
#define HW_RTC_EXTI_LINE_ALARM_EVENT LL_EXTI_LINE_17
#else
#error Please check EXTI line to use
#endif

/* Private macro -------------------------------------------------------------*/
#define CALENDAR_VALUE(subsec, sec, min, hours, days)  \
  ((((sec) + 60 * ((min) + 60 * ((hours) + 24 * (days)))) << N_PREDIV_S) + (PREDIV_S - (subsec)))

#define ADJUST_TIME(_x_, _y_, _t_) \
  while ((_x_) >= (_t_)) { \
    (_x_) -= (_t_); \
    (_y_)++; \
  }


/*!
 * time constants
 */
#define SECONDS_BY_MINUTES 60
#define MINUTES_BY_HOUR 60
#define SECONDS_BY_HOUR (SECONDS_BY_MINUTES * MINUTES_BY_HOUR)
#define HOURS_BY_DAY 24
#define SECONDS_BY_DAY (SECONDS_BY_HOUR * HOURS_BY_DAY)

/* Private variables ---------------------------------------------------------*/
/**
 * @brief Indicates if the RTC is already Initalized or not
 */
static FlagStatus HW_RTC_Initalized = RESET;

/**
 * @brief compensates MCU wakeup time
 */

static FlagStatus McuWakeUpTimeInitialized = RESET;

/**
 * @brief compensates MCU wakeup time
 */

static int16_t McuWakeUpTimeCal = 0;

/**
 * Number of seconds in a minute
 */
static const uint8_t SecondsInMinute = 60;

/**
 * Number of minutes in an hour
 */
static const uint8_t MinutesInHour = 60;

/**
 * Number of seconds in an hour
 */
static const uint16_t SecondsInHour = 3600;

/**
 * Number of seconds in a day
 */
static const uint32_t SecondsInDay = 86400;

/**
 * Number of hours in a day
 */
static const uint8_t HoursInDay = 24;

/**
 * Number of days in a standard year
 */
static const uint16_t DaysInYear = 365;

/**
 * Number of days in each month on a normal year
 */
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


/*!
 * 2 dimensional array containing the number of seconds elapsed before a given
 * month.
 * The second index map to the month while the first map to the type of year:
 *   - 0: non leap year
 *   - 1: leap year
 */
static const uint32_t seconds_before_month[2][12] = {
    {
        0,
        31 * SECONDS_BY_DAY,
        (31 + 28) * SECONDS_BY_DAY,
        (31 + 28 + 31) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31 + 30) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31 + 30 + 31) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31) * SECONDS_BY_DAY,
        (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30) * SECONDS_BY_DAY,
    },
    {
        0,
        31 * SECONDS_BY_DAY,
        (31 + 29) * SECONDS_BY_DAY,
        (31 + 29 + 31) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31 + 30) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31 + 30 + 31) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31) * SECONDS_BY_DAY,
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30) * SECONDS_BY_DAY,
    }
};

/**
 * Keep the value of the RTC timer when the RTC alarm is set
 * Set with the HW_RTC_SetTimerContext function
 * Value is kept as a Reference to calculate alarm
 */
static RtcTimerContext_t RtcTimerContext;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Configures the RTC timer
 * @note   The timer is based on the RTC
 * @param  None
 * @retval None
 * @note   Replicate the behavior of HAL_RTC_Init(), HAL_RTC_SetDate() and HAL_RTC_SetTime()
 */
static void HW_RTC_SetConfig( void );

/**
 * @brief  Configure alarm at init
 * @param  None
 * @retval None
 */
static void HW_RTC_SetAlarmConfig( void );

/**
 * @brief  Start wake up alarm
 * @note   Alarm in RtcTimerContext.Rtc_Time + timeoutValue
 * @param  TimeoutValue in ticks
 * @retval None
 */
static void HW_RTC_StartWakeUpAlarm( uint32_t timeoutValue );

/**
 * @brief  Get current time from calendar in ticks
 * @param  Pointer to RTC_DateStruct
 * @param  Pointer to RTC_TimeStruct
 * @retval Time in ticks
 */
static TimerTime_t HW_RTC_GetCalendarValue( HW_RTC_DateTypeDef *RTC_DateStruct, HW_RTC_TimeTypeDef *RTC_TimeStruct );

/**
 * @brief  BIN to BCD conversion
 * @param  BIN value
 * @retval BCD value
 */
static uint8_t HW_RTC_ByteToBcd2( uint8_t Value );

/**
 * @brief  BCD to BIN conversion
 * @param  BCD value
 * @retval BIN value
 */
static uint8_t HW_RTC_Bcd2ToByte( uint8_t Value );

/**
 * @brief  Deactivate the Alarm-A
 * @param  none
 * @retval none
 * @note   Implementation of HAL_RTC_DeactivateAlarm using LL
 */
static void HW_RTC_DeactivateAlarm( void );

/**
 * @brief  Handle Alarm interrupt request.
 * @param  None
 * @retval None
 * @note   LL implementation of HAL_RTC_AlarmIRQHandler
 * @note   Does not take into account ALARMB
 */
static void HW_RTC_AlarmIRQHandler( void );

/**
 * @brief Initializes the RCC Oscillators according to this specific configuration
 *    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
 *    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
 *    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
 * @param  None
 * @retval None
 */
static void HW_RCC_OscConfig( void );

/**
 * @brief  Initializes the RCC extended peripherals clocks
 * @note   This is a specific implementation with
 *            PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
 *            PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
 * @param  None
 * @retval None
 */
static void HW_RCCEx_PeriphCLKConfig( void );


/*!
 * Compute if a year is a leap year or not.
 *
 * \param year The year to test it shall be in the range [70:138]. Year 0 is
 * translated into year 1900 CE.
 * \return true if the year in input is a leap year and false otherwise.
 * \note - For use by the HAL only
 */
static bool _rtc_is_leap_year(int year);

static time_t _rtc_mktime(const struct tm* time);

static bool _rtc_localtime(time_t timestamp, struct tm* time_info);

/* Exported functions ---------------------------------------------------------*/

void HW_RTC_Init( void )
{
    if ( HW_RTC_Initalized == RESET )
    {
        HW_RTC_SetConfig( );
        HW_RTC_SetAlarmConfig( );
        HW_RTC_SetTimerContext( );
        HW_RTC_Initalized = SET;
    }
}

void HW_RTC_setSysTime( uint32_t seconds, uint16_t subSeconds )
{
    struct tm timeinfo;
    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;

    // Add 1 to the seconds in order to compensate the synchronization wait function.
    seconds += 1;

    // Convert the time into a tm
    if( _rtc_localtime( ( time_t )seconds, &timeinfo ) == false )
    {
        return;
    }

    // Fill RTC structures
    if( timeinfo.tm_wday == 0 )
    {
        RTC_DateStruct.WeekDay = 7;
    }
    else
    {
        RTC_DateStruct.WeekDay = timeinfo.tm_wday;
    }
    RTC_DateStruct.Month = timeinfo.tm_mon + 1;
    RTC_DateStruct.Day            = timeinfo.tm_mday;
    RTC_DateStruct.Year           = ( timeinfo.tm_year >= 100 ) ? timeinfo.tm_year - 100 : timeinfo.tm_year;

    RTC_TimeStruct.Hours          = timeinfo.tm_hour;
    RTC_TimeStruct.Minutes        = timeinfo.tm_min;
    RTC_TimeStruct.Seconds        = timeinfo.tm_sec;
    RTC_TimeStruct.SubSeconds     = 0;

    // Wait 1000ms - subSeconds (ms) before syncing the date/time
    uint16_t waitSync = 0;
    waitSync = 1000 - subSeconds;
    DelayMs( waitSync );


    LL_RTC_DisableWriteProtection( RTC );
    LL_RTC_EnterInitMode( RTC );
    LL_RTC_DATE_Config( RTC, RTC_DateStruct.WeekDay, __LL_RTC_CONVERT_BIN2BCD( RTC_DateStruct.Day ),
                        __LL_RTC_CONVERT_BIN2BCD( RTC_DateStruct.Month ),
                        __LL_RTC_CONVERT_BIN2BCD( RTC_DateStruct.Year ) );
    LL_RTC_TIME_Config( RTC, LL_RTC_TIME_FORMAT_AM_OR_24, __LL_RTC_CONVERT_BIN2BCD(RTC_TimeStruct.Hours), __LL_RTC_CONVERT_BIN2BCD(RTC_TimeStruct.Minutes), __LL_RTC_CONVERT_BIN2BCD(RTC_TimeStruct.Seconds) );
    LL_RTC_DisableInitMode( RTC );
    LL_RTC_EnableWriteProtection( RTC );

}

uint32_t HW_RTC_getSysTime( uint16_t *subSeconds )
{
    TimerTime_t current;
    struct tm timeinfo;

    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;



    current = HW_RTC_GetCalendarValue(&RTC_DateStruct, &RTC_TimeStruct);


    // Setup a tm structure based on the RTC
    // tm_wday information is ignored by mktime;
    timeinfo.tm_mon  = RTC_DateStruct.Month - 1;
    timeinfo.tm_mday = RTC_DateStruct.Day;
    timeinfo.tm_year = RTC_DateStruct.Year + 100;
    timeinfo.tm_hour = RTC_TimeStruct.Hours;
    timeinfo.tm_min  = RTC_TimeStruct.Minutes;
    timeinfo.tm_sec  = RTC_TimeStruct.Seconds;
    if( subSeconds != NULL )
    {
        *subSeconds = current % 1000;
    }
    // Convert to timestamp
    return _rtc_mktime( &timeinfo );
}


TimerTime_t HW_RTC_getElapsedAlarmTime( void )
{
    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;

    TimerTime_t CalendarValue = HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct );

    return CalendarValue - RtcTimerContext.Rtc_Time;

}

TimerTime_t HW_RTC_computeFutureEventMs( TimerTime_t futureEventInTime )
{
    return ( HW_RTC_GetTimerMs() + futureEventInTime );
}

TimerTime_t HW_RTC_computeElapsedMs( TimerTime_t eventInTime )
{
    TimerTime_t now;

    // Needed at boot, cannot compute with 0 or elapsed time will be equal to current time
    if( eventInTime == 0 )
    {
        return 0;
    }

    // get current time with ms
    now = HW_RTC_GetTimerMs();

    // compare "eventInTime" with "now" while watching for roll over due to 32-bit
    if( now < eventInTime ) // // roll over of the counter
    {   // due to convertion tick to ms, roll over value is 0x7D000000 (0x7D000000 * 2.048 = 0xFFFFFFFF) // TODO
        return( now + ( 0x7D000000 - eventInTime ) );
    }
    else
    {
        return( now - eventInTime );
    }
}


TimerTime_t HW_RTC_GetAdjustedTimeoutValue( uint32_t timeout )
{
    #if 0   // adjust when set wakeup timer todo
    if( timeout > McuWakeUpTime )
    {   // we have waken up from a GPIO and we have lost "McuWakeUpTime" that we need to compensate on next event
        if( NonScheduledWakeUp == true )
        {
            NonScheduledWakeUp = false;
            timeout -= McuWakeUpTime;
        }
    }

    if( timeout > McuWakeUpTime )
    {   // we don't go in low power mode for delay below 50ms (needed for LEDs)
        if( timeout < 50 ) // 50 ms
        {
            RtcTimerEventAllowsLowPower = false;
        }
        else
        {
            RtcTimerEventAllowsLowPower = true;
            timeout -= McuWakeUpTime;
        }
    }
    #endif
    return  timeout;
}


void HW_RTC_Process( void )
{
    // Not used on this platform.
}

TimerTime_t HW_RTC_TempCompensation( TimerTime_t period, float temperature )
{
    float k = RTC_TEMP_COEFFICIENT;
    float kDev = RTC_TEMP_DEV_COEFFICIENT;
    float t = RTC_TEMP_TURNOVER;
    float tDev = RTC_TEMP_DEV_TURNOVER;
    float interim = 0.0;
    float ppm = 0.0;

    if( k < 0.0 )
    {
        ppm = ( k - kDev );
    }
    else
    {
        ppm = ( k + kDev );
    }
    interim = ( temperature - ( t - tDev ) );
    ppm *=  interim * interim;

    // Calculate the drift in time
    interim = ( ( float ) period * ppm ) / 1e6;
    // Calculate the resulting time period
    interim += period;
    interim = floor( interim );

    if( interim < 0.0 )
    {
        interim = ( float )period;
    }

    // Calculate the resulting period
    return ( TimerTime_t ) interim;
}


void HW_RTC_setMcuWakeUpTime( void )
{
    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;

    uint16_t rtcAlarmSubSeconds;
    uint16_t rtcAlarmSeconds;
    uint16_t rtcAlarmMinutes;
    uint16_t rtcAlarmHours;
    uint16_t rtcAlarmDays;

    TimerTime_t now, hit;
    int16_t McuWakeUpTime;

    if ( (McuWakeUpTimeInitialized == RESET) &&
         (NVIC_GetPendingIRQ( RTC_Alarm_IRQn ) == 1) )
    { /* warning: works ok if now is below 30 days
     it is ok since it's done once at first alarm wake-up*/
        McuWakeUpTimeInitialized = SET;
        now = HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct );

        DBG_GPIO_SET( GPIOB, GPIO_PIN_13 );
        DBG_GPIO_RST( GPIOB, GPIO_PIN_13 );

        rtcAlarmSubSeconds = LL_RTC_ALMA_GetSubSecond( RTC );
        rtcAlarmHours = HW_RTC_Bcd2ToByte( LL_RTC_ALMA_GetHour( RTC ) );
        rtcAlarmMinutes = HW_RTC_Bcd2ToByte( LL_RTC_ALMA_GetMinute( RTC ) );
        rtcAlarmSeconds = HW_RTC_Bcd2ToByte( LL_RTC_ALMA_GetSecond( RTC ) );
        rtcAlarmDays = LL_RTC_ALMA_GetDay( RTC );

        hit = CALENDAR_VALUE( rtcAlarmSubSeconds, rtcAlarmSeconds, rtcAlarmMinutes, rtcAlarmHours, rtcAlarmDays );

        McuWakeUpTime = (int16_t) ((now - hit));
        McuWakeUpTimeCal += McuWakeUpTime;
//        DBG_PRINTF_LV(DBG_LOG_BOARD_NORMAL, "Cal=%d, %d\n", McuWakeUpTimeCal, McuWakeUpTime );
    }
}

uint32_t HW_RTC_GetMinimumTimeout( void )
{
    return (MIN_ALARM_DELAY);
}

uint32_t HW_RTC_ms2Tick( TimerTime_t timeMicroSec )
{
    return (uint32_t) ((((uint64_t) timeMicroSec) * CONV_DENOM) / CONV_NUMER);
}

TimerTime_t HW_RTC_Tick2ms( uint32_t tick )
{
    return (((uint64_t)( tick ) * CONV_NUMER) / CONV_DENOM);
}

void HW_RTC_SetAlarm( uint32_t timeout )
{
    /* we don't go in Low Power mode for timeout below MIN_ALARM_DELAY */
    if ( (MIN_ALARM_DELAY + McuWakeUpTimeCal) < ((timeout - HW_RTC_GetTimerElapsedTime( ))))
    {
        LowPower_Enable( e_LOW_POWER_RTC );
    }
    else
    {
        LowPower_Disable( e_LOW_POWER_RTC );
    }

    if ( LowPower_GetState( ) == 0 )
    {
        LowPower_Enable( e_LOW_POWER_RTC );
        timeout = timeout - McuWakeUpTimeCal;
    }

    HW_RTC_StartWakeUpAlarm( timeout );
}

uint32_t HW_RTC_GetTimerElapsedTime( void )
{
    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;

    TimerTime_t CalendarValue = HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct );

    return ((uint32_t) (CalendarValue - RtcTimerContext.Rtc_Time));
}

uint32_t HW_RTC_GetTimerValue( void )
{
    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;

    uint32_t CalendarValue = (uint32_t) HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct );

    return (CalendarValue);
}

TimerTime_t HW_RTC_GetTimerMs( void )
{
    uint32_t ms;
    TimerTime_t tick;
    HW_RTC_TimeTypeDef RTC_TimeStruct;
    HW_RTC_DateTypeDef RTC_DateStruct;

    tick = HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct );
    ms = HW_RTC_Tick2ms( (uint32_t)tick );

    return (ms);
}

void HW_RTC_StopAlarm( void )
{
    /* Clear RTC Alarm Flag */
    LL_RTC_ClearFlag_ALRA( RTC );

    /* Disable the Alarm A interrupt */
    HW_RTC_DeactivateAlarm( );
}

void HW_RTC_IrqHandler( void )
{
    HW_RTC_AlarmIRQHandler( );
}

void HW_RTC_DelayMs( uint32_t delay )
{
    TimerTime_t delayValue = 0;
    TimerTime_t timeout = 0;

    delayValue = HW_RTC_ms2Tick( delay );

    /* Wait delay ms */
    timeout = HW_RTC_GetTimerValue( );
    while ( ((HW_RTC_GetTimerValue( ) - timeout)) < delayValue )
    {
        //wwdg_feed_check_timer();
        __NOP( );
    }
}

uint32_t HW_RTC_SetTimerContext( void )
{
    RtcTimerContext.Rtc_Time = HW_RTC_GetCalendarValue( &RtcTimerContext.RTC_Calndr_Date,
                                                        &RtcTimerContext.RTC_Calndr_Time );
    return (uint32_t) RtcTimerContext.Rtc_Time;
}

uint32_t HW_RTC_GetTimerContext( void )
{
    return (uint32_t) RtcTimerContext.Rtc_Time;
}

/* Private functions ---------------------------------------------------------*/

static void HW_RTC_SetConfig( void )
{
    /*
     * clock initialization
     * - Enable LSE Oscillator
     * - Select LSI as RTC clock source
     * - Enable RTC Clock
     * - Configure the NVIC for RTC Alarm
     */
    HW_RCC_OscConfig( );
    HW_RCCEx_PeriphCLKConfig( );
    LL_RCC_EnableRTC( );
    NVIC_SetPriority( RTC_Alarm_IRQn, 0x0 );
    NVIC_EnableIRQ( RTC_Alarm_IRQn );

    /* Disable write protection so that RTC can be initialized */
    LL_RTC_DisableWriteProtection( RTC );

    /* Enter init mode */
    LL_RTC_EnterInitMode( RTC );

    /* Clear RTC_CR FMT, OSEL and POL Bits */
    LL_RTC_SetHourFormat( RTC, LL_RTC_HOURFORMAT_24HOUR );
    LL_RTC_SetAlarmOutEvent( RTC, RTC_OUTPUT ); /* RTC_OUTPUT is defined to LL_RTC_ALARMOUT_xxx */
    LL_RTC_SetOutputPolarity( RTC, LL_RTC_OUTPUTPOLARITY_PIN_HIGH );

    /* Configure the RTC PRER */
    LL_RTC_SetSynchPrescaler( RTC, PREDIV_S );
    LL_RTC_SetAsynchPrescaler( RTC, PREDIV_A );

    /* Exit Initialization mode */
    LL_RTC_DisableInitMode( RTC );

    LL_RTC_DisableOutRemap( RTC );
    LL_RTC_SetAlarmOutputType( RTC, LL_RTC_ALARM_OUTPUTTYPE_OPENDRAIN );

    /* Enter init mode */
    LL_RTC_EnterInitMode( RTC );

    /* Friday 1st January 2000, at 0:0:0 */
    LL_RTC_DATE_Config( RTC, LL_RTC_WEEKDAY_SATURDAY, __LL_RTC_CONVERT_BIN2BCD( 1 ),
                        __LL_RTC_CONVERT_BIN2BCD( 1 ),
                        __LL_RTC_CONVERT_BIN2BCD( 0 ) );
    LL_RTC_TIME_Config( RTC, LL_RTC_TIME_FORMAT_AM_OR_24, 0, 0, 0 );

    /* Exit init mode */
    LL_RTC_DisableInitMode( RTC );

    /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if ( LL_RTC_IsShadowRegBypassEnabled( RTC ) == 0U )
    {
        LL_RTC_WaitForSynchro( RTC );
    }

    /* Enable Direct Read of the calendar registers (not through Shadow) */
    LL_RTC_EnableShadowRegBypass( RTC );

    /* Enable write protection so that ISR cannot be modified */
    LL_RTC_EnableWriteProtection( RTC );
}

static void HW_RTC_SetAlarmConfig( void )
{
    HW_RTC_DeactivateAlarm( );
}

static void HW_RTC_StartWakeUpAlarm( uint32_t timeoutValue )
{
    uint16_t rtcAlarmSubSeconds;
    uint16_t rtcAlarmSeconds;
    uint16_t rtcAlarmMinutes;
    uint16_t rtcAlarmHours;
    uint16_t rtcAlarmDays;
    uint8_t day_in_month;
    HW_RTC_TimeTypeDef RTC_TimeStruct = RtcTimerContext.RTC_Calndr_Time;
    HW_RTC_DateTypeDef RTC_DateStruct = RtcTimerContext.RTC_Calndr_Date;

    /* fix by liwp : calculate from current time  */
    //HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct );

    HW_RTC_StopAlarm( );

    /*reverse counter */
    rtcAlarmSubSeconds = PREDIV_S - RTC_TimeStruct.SubSeconds;
    rtcAlarmSubSeconds += (timeoutValue & PREDIV_S);
    /* convert timeout  to seconds */
    timeoutValue >>= N_PREDIV_S; /* convert timeout  in seconds */

    /*convert microsecs to RTC format and add to 'Now' */
    rtcAlarmDays = RTC_DateStruct.Day;
    ADJUST_TIME( timeoutValue, rtcAlarmDays, SecondsInDay );

    /* calc hours */
    rtcAlarmHours = RTC_TimeStruct.Hours;
    ADJUST_TIME( timeoutValue, rtcAlarmHours, SecondsInHour );

    /* calc minutes */
    rtcAlarmMinutes = RTC_TimeStruct.Minutes;
    ADJUST_TIME( timeoutValue, rtcAlarmMinutes, SecondsInMinute );

    /* calc seconds */
    rtcAlarmSeconds = RTC_TimeStruct.Seconds + timeoutValue;

    /***** correct for modulo********/
    ADJUST_TIME( rtcAlarmSubSeconds, rtcAlarmSeconds, (PREDIV_S + 1) );
    ADJUST_TIME( rtcAlarmSeconds, rtcAlarmMinutes, SecondsInMinute );
    ADJUST_TIME( rtcAlarmMinutes, rtcAlarmHours, MinutesInHour );
    ADJUST_TIME( rtcAlarmHours, rtcAlarmDays, HoursInDay );

    /* Day in month, adjusted to take into account leap years */
    day_in_month = DaysInMonth[RTC_DateStruct.Month - 1];
    if ( ((RTC_DateStruct.Year % 4) == 0) && (RTC_DateStruct.Month == 2) )
    {
        day_in_month++;
    }

    while ( rtcAlarmDays > day_in_month )
    {
        rtcAlarmDays = (rtcAlarmDays - day_in_month);
    }

    /* Set RTC_AlarmStructure with calculated values*/
    /* Code from HAL_RTC_SetAlarm_IT / LL_RTC_ALMA_Init */
    LL_RTC_DisableWriteProtection( RTC );

    /* Disable the Alarm A interrupt */
    LL_RTC_ALMA_Disable( RTC );
    LL_RTC_ClearFlag_ALRA( RTC );
    /* Wait till RTC ALRxWF flag is set */
    while ( !LL_RTC_IsActiveFlag_ALRAW( RTC ) )
    {
        ;
    }

    LL_RTC_ALMA_SetSubSecond( RTC, PREDIV_S - rtcAlarmSubSeconds );
    LL_RTC_ALMA_SetSubSecondMask( RTC, ((HW_RTC_ALARMSUBSECONDMASK) >> (RTC_POSITION_ALMA_MASKSS )) );
    LL_RTC_ALMA_ConfigTime( RTC, RTC_TimeStruct.TimeFormat,
                            HW_RTC_ByteToBcd2( rtcAlarmHours ),
                            HW_RTC_ByteToBcd2( rtcAlarmMinutes ),
                            HW_RTC_ByteToBcd2( rtcAlarmSeconds ) );
    LL_RTC_ALMA_DisableWeekday( RTC );
    LL_RTC_ALMA_SetDay( RTC, HW_RTC_ByteToBcd2( rtcAlarmDays ) );
    LL_RTC_ALMA_SetMask( RTC, LL_RTC_ALMA_MASK_NONE );
    LL_RTC_TIME_DisableDayLightStore( RTC );

    /* Configure the Alarm state: Enable Alarm */
    LL_RTC_ALMA_Enable( RTC );

    /* Configure the Alarm interrupt */
    LL_RTC_EnableIT_ALRA( RTC );

    /* RTC Alarm Interrupt Configuration: EXTI configuration */
    LL_EXTI_EnableIT_0_31( HW_RTC_EXTI_LINE_ALARM_EVENT );
    LL_EXTI_EnableRisingTrig_0_31( HW_RTC_EXTI_LINE_ALARM_EVENT );

    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection( RTC );

    /* Debug Printf*/
    DBG( HW_RTC_GetCalendarValue( &RTC_DateStruct, &RTC_TimeStruct )
    ;);
#if 0
  DBG_PRINTF("it's %d:%d:%d:%d ", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds, ((PREDIV_S - RTC_TimeStruct.SubSeconds) * 1000) >> N_PREDIV_S);
#endif
#if 0
  DBG_PRINTF_LV(DBG_LOG_BOARD_NORMAL, "WU@ %d:%d:%d:%d\r\n", rtcAlarmHours, rtcAlarmMinutes, rtcAlarmSeconds,
                 (rtcAlarmSubSeconds * 1000) >> N_PREDIV_S );
#endif
}

static TimerTime_t HW_RTC_GetCalendarValue( HW_RTC_DateTypeDef *RTC_DateStruct, HW_RTC_TimeTypeDef *RTC_TimeStruct )
{
    TimerTime_t calendarValue;
    TimerTime_t nb_days;
    uint32_t i;
    uint32_t ssr;
    uint32_t tr;
    uint32_t dr;

    /*
     * as shadow registers are not used, we must read RTC->SSR, TR and DR registers
     * and ensure they have not changed between 2 reads to ensure time and date are coherent
     * Then we must use these values directly instead of readnig them again and again
     * with LL_RTC_TIME_Getxxx() functions
     */
    do
    {
        ssr = RTC->SSR;
        tr = RTC->TR;
        dr = RTC->DR;
    } while ( ssr != RTC->SSR );

    /* RTC_TimeStruct->SubSeconds = LL_RTC_TIME_GetSubSecond(RTC); */
    RTC_TimeStruct->SubSeconds = READ_BIT( ssr, RTC_SSR_SS );

    /* RTC_TimeStruct->Hours = HW_RTC_Bcd2ToByte(LL_RTC_TIME_GetHour(RTC)); */
    RTC_TimeStruct->Hours = HW_RTC_Bcd2ToByte( (uint8_t) ((tr & (RTC_TR_HT | RTC_TR_HU)) >> 16U) );

    /* RTC_TimeStruct->Minutes = HW_RTC_Bcd2ToByte(LL_RTC_TIME_GetMinute(RTC)); */
    RTC_TimeStruct->Minutes = HW_RTC_Bcd2ToByte( (uint8_t) ((tr & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U) );

    /* RTC_TimeStruct->Seconds = HW_RTC_Bcd2ToByte(LL_RTC_TIME_GetSecond(RTC)); */
    RTC_TimeStruct->Seconds = HW_RTC_Bcd2ToByte( (uint8_t) (tr & (RTC_TR_ST | RTC_TR_SU)) );

    /* RTC_TimeStruct->TimeFormat = LL_RTC_TIME_GetFormat(RTC); */
    RTC_TimeStruct->TimeFormat = READ_BIT( tr, RTC_TR_PM );

    /* RTC_DateStruct->WeekDay = LL_RTC_DATE_GetWeekDay(RTC); */
    RTC_DateStruct->WeekDay = (uint32_t) (READ_BIT(dr, RTC_DR_WDU) >> RTC_POSITION_DR_WDU );

    /* RTC_DateStruct->Month = HW_RTC_Bcd2ToByte(LL_RTC_DATE_GetMonth(RTC)); */
    RTC_DateStruct->Month = HW_RTC_Bcd2ToByte( (uint8_t) ((dr & (RTC_DR_MT | RTC_DR_MU)) >> 8U) );

    /* RTC_DateStruct->Day = HW_RTC_Bcd2ToByte(LL_RTC_DATE_GetDay(RTC)); */
    RTC_DateStruct->Day = HW_RTC_Bcd2ToByte( (uint8_t) (dr & (RTC_DR_DT | RTC_DR_DU)) );

    /* RTC_DateStruct->Year = HW_RTC_Bcd2ToByte(LL_RTC_DATE_GetYear(RTC)); */
    RTC_DateStruct->Year = HW_RTC_Bcd2ToByte( (uint8_t) ((dr & (RTC_DR_YT | RTC_DR_YU)) >> 16U) );

    /* years (calc valid up to year 2099)*/
    nb_days = RTC_DateStruct->Year * DaysInYear;
    nb_days += (RTC_DateStruct->Year + 3) / 4; /* we add 1 day for full-year 00 (which is 2000), 1 day for full-year 2004,...) */
    /* Day in month, adjusted to take into account leap years */
    for ( i = 0; i < (RTC_DateStruct->Month - 1); i++ )
    {
        nb_days += DaysInMonth[i];
    }
    if ( ((RTC_DateStruct->Year % 4) == 0) && (RTC_DateStruct->Month >= 3) )
    {
        nb_days++;
    }

    /* days */
    nb_days += (RTC_DateStruct->Day - 1);

    calendarValue = CALENDAR_VALUE( RTC_TimeStruct->SubSeconds, RTC_TimeStruct->Seconds,
                                    RTC_TimeStruct->Minutes,
                                    RTC_TimeStruct->Hours, nb_days );

    return (calendarValue);
}

static uint8_t HW_RTC_ByteToBcd2( uint8_t Value )
{
    return __LL_RTC_CONVERT_BIN2BCD( Value );
}

static uint8_t HW_RTC_Bcd2ToByte( uint8_t Value )
{
    return __LL_RTC_CONVERT_BCD2BIN( Value );
}

static void HW_RTC_DeactivateAlarm( void )
{
    /* Disable the Alarm A interrupt */
    LL_RTC_DisableWriteProtection( RTC );
    LL_RTC_ALMA_Disable( RTC );
    LL_RTC_DisableIT_ALRA( RTC );

    /* Wait till RTC ALRxWF flag is set */
    while ( !LL_RTC_IsActiveFlag_ALRAW( RTC ) )
    {
        ;
    }

    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection( RTC );
}
static void HW_RTC_AlarmIRQHandler( void )
{
#if defined(M400_LORAWAN)
    /* enable low power at irq*/
    LowPower_Enable( e_LOW_POWER_RTC );
#endif
    /* Get the pending status of the AlarmA Interrupt */
    if ( LL_RTC_IsActiveFlag_ALRA( RTC ) )
    {
        /* Clear the AlarmA interrupt pending bit */
        LL_RTC_ClearFlag_ALRA( RTC );
        /* Clear the EXTI's line Flag for RTC Alarm */
        LL_EXTI_ClearFlag_0_31( HW_RTC_EXTI_LINE_ALARM_EVENT );

#if defined(M400_LORAWAN)
        /* AlarmA callback */
        TimerIrqHandler( );
#endif
    }
}

static void HW_RCC_OscConfig( void )
{
    FlagStatus pwrclkchanged = RESET;
    FlagStatus backupchanged = RESET;

    /* Update LSE configuration in Backup Domain control register    */
    /* Requires to enable write access to Backup Domain of necessary */
    if ( LL_APB1_GRP1_IsEnabledClock( LL_APB1_GRP1_PERIPH_PWR ) == RESET )
    {
        LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_PWR );

        pwrclkchanged = SET;
    }

    if ( LL_PWR_IsEnabledBkUpAccess( ) == RESET )
    {
        /* Enable write access to Backup domain */
        LL_PWR_EnableBkUpAccess( );
        backupchanged = SET;

        /* Wait for Backup domain Write protection disable */
        while ( LL_PWR_IsEnabledBkUpAccess( ) == RESET )
        {
            ;
        }
    }

    /* Set the new LSE configuration -----------------------------------------*/
    LL_RCC_LSE_Enable( );

    /* Wait till LSE is ready */
    while ( (LL_RCC_ReadReg(CSR) & RCC_CSR_LSERDY_Msk) == RESET )
    {
        ;
    }

    /* Requires to disable write access to Backup Domain of necessary */
    if ( backupchanged == SET )
    {
        LL_PWR_DisableBkUpAccess( );
    }
    if ( pwrclkchanged == SET )
    {
        LL_APB1_GRP1_DisableClock( LL_APB1_GRP1_PERIPH_PWR );
    }
}

static void HW_RCCEx_PeriphCLKConfig( void )
{
    uint32_t tmpreg = 0U;

    /* Enable Power Clock*/
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_PWR );

    /* Enable write access to Backup domain */
    LL_PWR_EnableBkUpAccess( );
    while ( LL_PWR_IsEnabledBkUpAccess( ) == RESET )
    {
        ;
    }

    /* Reset the Backup domain only if the RTC Clock source selection is modified */
    if ( LL_RCC_GetRTCClockSource( ) != LL_RCC_RTC_CLKSOURCE_LSE )
    {
        /* Store the content of CSR register before the reset of Backup Domain */
        tmpreg = (LL_RCC_ReadReg(CSR) & ~(RCC_CSR_RTCSEL));
        /* RTC Clock selection can be changed only if the Backup Domain is reset */
        LL_RCC_ForceBackupDomainReset( );
        LL_RCC_ReleaseBackupDomainReset( );

        /* Restore the Content of CSR register */
        LL_RCC_WriteReg( CSR, tmpreg );

        /* Wait for LSERDY if LSE was enabled */
        if ( (tmpreg & RCC_CSR_LSERDY) != RESET )
        {
            /* Wait till LSE is ready */
            while ( (LL_RCC_ReadReg(CSR) & RCC_CSR_LSERDY_Msk) == RESET )
            {
                ;
            }

            /* RTC Clock update - based on __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE) */
            CLEAR_BIT( RCC->CR, RCC_CR_RTCPRE );
            LL_RCC_SetRTCClockSource( LL_RCC_RTC_CLKSOURCE_LSE );
        }
    }
}



/*!
 * Compute if a year is a leap year or not.
 *
 * \param year The year to test it shall be in the range [70:138]. Year 0 is
 * translated into year 1900 CE.
 * \return true if the year in input is a leap year and false otherwise.
 * \note - For use by the HAL only
 */
static bool _rtc_is_leap_year(int year) {
    /*
     * since in practice, the value manipulated by this algorithm lie in the
     * range [70 : 138], the algorith can be reduced to: year % 4.
     * The algorithm valid over the full range of value is:

        year = 1900 + year;
        if (year % 4) {
            return false;
        } else if (year % 100) {
            return true;
        } else if (year % 400) {
            return false;
        }
        return true;

     */
    return (year) % 4 ? false : true;
}

static time_t _rtc_mktime(const struct tm* time) {
    // partial check for the upper bound of the range
    // normalization might happen at the end of the function
    // this solution is faster than checking if the input is after the 19th of
    // january 2038 at 03:14:07.
    if ((time->tm_year < 70) || (time->tm_year > 138)) {
        return ((time_t) -1);
    }

    uint32_t result = time->tm_sec;
    result += time->tm_min * SECONDS_BY_MINUTES;
    result += time->tm_hour * SECONDS_BY_HOUR;
    result += (time->tm_mday - 1) * SECONDS_BY_DAY;
    result += seconds_before_month[_rtc_is_leap_year(time->tm_year)][time->tm_mon];

    if (time->tm_year > 70) {
        // valid in the range [70:138]
        uint32_t count_of_leap_days = ((time->tm_year - 1) / 4) - (70 / 4);
        result += (((time->tm_year - 70) * 365) + count_of_leap_days) * SECONDS_BY_DAY;
    }

    if (result > INT32_MAX) {
        return (time_t) -1;
    }

    return result;
}

static bool _rtc_localtime(time_t timestamp, struct tm* time_info) {
    if (((int32_t) timestamp) < 0) {
        return false;
    }

    time_info->tm_sec = timestamp % 60;
    timestamp = timestamp / 60;   // timestamp in minutes
    time_info->tm_min = timestamp % 60;
    timestamp = timestamp / 60;  // timestamp in hours
    time_info->tm_hour = timestamp % 24;
    timestamp = timestamp / 24;  // timestamp in days;

    // compute the weekday
    // The 1st of January 1970 was a Thursday which is equal to 4 in the weekday
    // representation ranging from [0:6]
    time_info->tm_wday = (timestamp + 4) % 7;

    // years start at 70
    time_info->tm_year = 70;
    while (true) {
        if (_rtc_is_leap_year(time_info->tm_year) && timestamp >= 366) {
            ++time_info->tm_year;
            timestamp -= 366;
        } else if (!_rtc_is_leap_year(time_info->tm_year) && timestamp >= 365) {
            ++time_info->tm_year;
            timestamp -= 365;
        } else {
            // the remaining days are less than a years
            break;
        }
    }

    time_info->tm_yday = timestamp;

    // convert days into seconds and find the current month
    timestamp *= SECONDS_BY_DAY;
    time_info->tm_mon = 11;
    bool leap = _rtc_is_leap_year(time_info->tm_year);
    for (uint32_t i = 0; i < 12; ++i) {
        if ((uint32_t) timestamp < seconds_before_month[leap][i]) {
            time_info->tm_mon = i - 1;
            break;
        }
    }

    // remove month from timestamp and compute the number of days.
    // note: unlike other fields, days are not 0 indexed.
    timestamp -= seconds_before_month[leap][time_info->tm_mon];
    time_info->tm_mday = (timestamp / SECONDS_BY_DAY) + 1;

    return true;
}
