/***************************************************************************//**
* \file Name: RTC.h
* \version 1.30
*
*  This file provides constants and parameter values for the RTC_P4 Component.
*
********************************************************************************
* Copyright 2015-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_RTC_P4_RTC_H)
#define CY_RTC_P4_RTC_H

#include <cytypes.h>
#include <CyLib.h>


/*******************************************************************************
*    Data types definition
*******************************************************************************/
/**
* \addtogroup group_enums
* \{
*/
/** Daylight saving Time format enumeration */
typedef enum
{
    RTC_DST_DATE_RELATIVE = 0u,  /**< Relative DST format */
    RTC_DST_DATE_FIXED = 1u      /**< Fixed DST format */
}RTC_DST_DATETYPE_ENUM;
/** \} group_enums */

/**
* \addtogroup group_structures
* \{
*/

/** RTC_DATE_TIME structure is the data structure that is used to 
* save the current time and date (RTC_currentTimeDate), and Alarm time and date 
*(RTC_alarmCfgTimeDate)
*
*/
typedef struct
{
    uint32 time;        /**< Time in the format used in API*/
    uint32 date;        /**< Date in the format used in API*/
    uint32 dayOfWeek;   /**< Day of the week, see \ref group_rtc_day_of_the_week */
    uint32 status;      /**< RTC status, see \ref group_rtc_status */
}RTC_DATE_TIME;

/*
* RTC_DST_TIME structure is the data structure that is used to 
* save time and date values for Daylight Savings Time Start and Stop 
* (RTC_dstTimeDateStart and RTC_dstTimeDateStop)
*/
typedef struct
{
    uint32 hour;          /**< Hour value */
    uint32 dayOfWeek;     /**< Day of the week, see \ref group_rtc_day_of_the_week */
    uint32 dayOfMonth;    /**< Day of the month */
    uint32 weekOfMonth;   /**< Week of the month, see \ref group_rtc_dst_week_of_month */
    uint32 month;         /**< Month value, see \ref group_rtc_month */
    uint8  timeFormat;    /**< The DST operation mode, see \ref 
                               RTC_DST_DATETYPE_ENUM */
}RTC_DST_TIME;
/** \} group_structures */

/*******************************************************************************
*    Enumerated Types and Parameters
*******************************************************************************/
/* Time Format setting constants */
#define RTC__HOUR_12 1
#define RTC__HOUR_24 0

/*  Date Format setting constants */
#define RTC__MM_DD_YYYY 0
#define RTC__DD_MM_YYYY 1
#define RTC__YYYY_MM_DD 2

/* Date Type setting constants */
#define RTC__RELATIVE 0
#define RTC__FIXED 1

/* DST Hour setting constants */
#define RTC__H0000 0
#define RTC__H0100 1
#define RTC__H0200 2
#define RTC__H0300 3
#define RTC__H0400 4
#define RTC__H0500 5
#define RTC__H0600 6
#define RTC__H0700 7
#define RTC__H0800 8
#define RTC__H0900 9
#define RTC__H1000 10
#define RTC__H1100 11
#define RTC__H1200 12
#define RTC__H1300 13
#define RTC__H1400 14
#define RTC__H1500 15
#define RTC__H1600 16
#define RTC__H1700 17
#define RTC__H1800 18
#define RTC__H1900 19
#define RTC__H2000 20
#define RTC__H2100 21
#define RTC__H2200 22
#define RTC__H2300 23

/* DST DayOfWeek setting constants */
#define RTC__SUNDAY 1
#define RTC__MONDAY 2
#define RTC__TUESDAY 3
#define RTC__WEDNESDAY 4
#define RTC__THURSDAY 5
#define RTC__FRIDAY 6
#define RTC__SATURDAY 7

/* DST Month setting constants */
#define RTC__JAN 1
#define RTC__FEB 2
#define RTC__MAR 3
#define RTC__APR 4
#define RTC__MAY 5
#define RTC__JUN 6
#define RTC__JUL 7
#define RTC__AUG 8
#define RTC__SEP 9
#define RTC__OCT 10
#define RTC__NOV 11
#define RTC__DEC 12

/* DST WeekOfMonth setting constants */
#define RTC__FIRST 1
#define RTC__SECOND 2
#define RTC__THIRD 3
#define RTC__FOURTH 4
#define RTC__FIFTH 5
#define RTC__LAST 6


#define RTC_INITIAL_DATA_FORMAT     (0u)
#define RTC_INITIAL_TIME_FORMAT     (0u)
#define RTC_INITIAL_UPDATE_MODE     (0u)
#define RTC_INITIAL_SECOND          (0u)
#define RTC_INITIAL_MINUTE          (0u)
#define RTC_INITIAL_HOUR            (0u)
#define RTC_INITIAL_DAY             (1u)
#define RTC_INITIAL_MONTH           (1u)
#define RTC_INITIAL_YEAR            (1970u)

#define RTC_INITIAL_ALARM_STATUS    (1u)

#define RTC_INITIAL_DST_STATUS      (0u)
#define RTC_INITIAL_DST_DATE_TYPE   (0u)
#define RTC_INITIAL_DST_START_MONTH (3uL)
#define RTC_INITIAL_DST_START_WOM   (6uL)
#define RTC_INITIAL_DST_START_DOM   (22uL)
#define RTC_INITIAL_DST_START_DOW   (1uL)
#define RTC_INITIAL_DST_START_HRS   (0uL)
#define RTC_INITIAL_DST_STOP_MONTH  (10uL)
#define RTC_INITIAL_DST_STOP_DOM    (22uL)
#define RTC_INITIAL_DST_STOP_DOW    (1uL)
#define RTC_INITIAL_DST_STOP_WOM    (6uL)
#define RTC_INITIAL_DST_STOP_HRS    (0uL)

/**
* \addtogroup group_constants
* \{
*/

/**
* \defgroup group_rtc_day_of_the_week Day of the week definitions
* \{
* Definitions of days in the week
*/
#define RTC_SUNDAY     (RTC__SUNDAY)       /**< Sequential number of Sunday in the week */
#define RTC_MONDAY     (RTC__MONDAY)       /**< Sequential number of Monday in the week */
#define RTC_TUESDAY    (RTC__TUESDAY)      /**< Sequential number of Tuesday in the week */
#define RTC_WEDNESDAY  (RTC__WEDNESDAY)    /**< Sequential number of Wednesday in the week */
#define RTC_THURSDAY   (RTC__THURSDAY)     /**< Sequential number of Thursday in the week */
#define RTC_FRIDAY     (RTC__FRIDAY)       /**< Sequential number of Friday in the week */
#define RTC_SATURDAY   (RTC__SATURDAY)     /**< Sequential number of Saturday in the week */
/** \} group_rtc_day_of_the_week */

/**
* \defgroup group_rtc_dst_week_of_month DST Week of month setting constants definitions
* \{
* Week of month setting constants definitions for Daylight Saving Time feature
*/
#define RTC_FIRST      (RTC__FIRST)   /**< First week in the month */
#define RTC_SECOND     (RTC__SECOND)  /**< Second week in the month  */
#define RTC_THIRD      (RTC__THIRD)   /**< Third week in the month  */
#define RTC_FOURTH     (RTC__FOURTH)  /**< Fourth week in the month  */
#define RTC_FIFTH      (RTC__FIFTH)   /**< Fifth week in the month  */
#define RTC_LAST       (RTC__LAST)    /**< Last week in the month  */
/** \} group_rtc_dst_week_of_month */

/**
* \defgroup group_rtc_month Month definitions
* \{
* Constants definition for Months
*/
#define RTC_JANUARY    (RTC__JAN)    /**< Sequential number of January in the year */
#define RTC_FEBRUARY   (RTC__FEB)    /**< Sequential number of February in the year */
#define RTC_MARCH      (RTC__MAR)    /**< Sequential number of March in the year */
#define RTC_APRIL      (RTC__APR)    /**< Sequential number of April in the year */
#define RTC_MAY        (RTC__MAY)    /**< Sequential number of May in the year */
#define RTC_JUNE       (RTC__JUN)    /**< Sequential number of June in the year */
#define RTC_JULY       (RTC__JUL)    /**< Sequential number of July in the year */
#define RTC_AUGUST     (RTC__AUG)    /**< Sequential number of August in the year */
#define RTC_SEPTEMBER  (RTC__SEP)    /**< Sequential number of September in the year */
#define RTC_OCTOBER    (RTC__OCT)    /**< Sequential number of October in the year */
#define RTC_NOVEMBER   (RTC__NOV)    /**< Sequential number of November in the year */
#define RTC_DECEMBER   (RTC__DEC)    /**< Sequential number of December in the year */
/** \} group_rtc_month */

/**
* \defgroup group_rtc_am_pm AM/PM status definitions
* \{
* Definitions for 12 hour format for indicating the AM/PM period of day
*/
#define RTC_AM                 (0u)    /**< AM period of day */
#define RTC_PM                 (1u)    /**< PM period of day */
/** \} group_rtc_am_pm */

/**
* \defgroup group_rtc_hour_format Hour format definitions
* \{
* Definitions for hour format
*/
#define RTC_12_HOURS_FORMAT    (RTC__HOUR_12) /**< The 12 hour (AM/PM) format */
#define RTC_24_HOURS_FORMAT    (RTC__HOUR_24) /**< The 24 hour format */
/** \} group_rtc_hour_format */

/**
* \defgroup group_rtc_days_in_month Number of days in month definitions
* \{
* Definition of days in current month
*/
#define RTC_DAYS_IN_JANUARY     (31u)    /**< Number of days in January  */
#define RTC_DAYS_IN_FEBRUARY    (28u)    /**< Number of days in February */
#define RTC_DAYS_IN_MARCH       (31u)    /**< Number of days in March */
#define RTC_DAYS_IN_APRIL       (30u)    /**< Number of days in April */
#define RTC_DAYS_IN_MAY         (31u)    /**< Number of days in May */
#define RTC_DAYS_IN_JUNE        (30u)    /**< Number of days in June */
#define RTC_DAYS_IN_JULY        (31u)    /**< Number of days in July */
#define RTC_DAYS_IN_AUGUST      (31u)    /**< Number of days in August */
#define RTC_DAYS_IN_SEPTEMBER   (30u)    /**< Number of days in September */
#define RTC_DAYS_IN_OCTOBER     (31u)    /**< Number of days in October */
#define RTC_DAYS_IN_NOVEMBER    (30u)    /**< Number of days in November */
#define RTC_DAYS_IN_DECEMBER    (31u)    /**< Number of days in December */
/** \} group_rtc_days_in_month */

/**
* \defgroup group_rtc_status Definitions of the RTC status values
* \{
* Definitions for status software register, which has flags for DST (DST),
* Leap Year (LY), AM/PM (AM_PM).
*/

/** Status of Daylight Saving Time. This bit
* goes high when the current time and date match the DST time and date and the 
* time is incremented. This bit goes low after the DST interval and the time is 
* decremented.
*/
#define RTC_STATUS_DST     (1uL << RTC_STATUS_DST_OFFSET)

/** Status of Leap Year. This bit goes high when the current year is a leap year */
#define RTC_STATUS_LY      (1uL << RTC_STATUS_LY_OFFSET)

/** Status of Current Time. This bit is low from midnight to noon and high from 
*  noon to midnight. */
#define RTC_STATUS_AM_PM   (1uL << RTC_STATUS_AM_PM_OFFSET)
/** \} group_rtc_status */

/**
* \defgroup group_rtc_alarm_mask Definitions for Alarm Mask software register
* \{
* Definitions for Alarm Mask software register. These masks allow 
* matching the alarm value register with the current value register.
*/

/** The second alarm mask allows matching the alarm second register with the 
*   current second register.
*/
#define RTC_ALARM_SEC_MASK         (0x00000001uL)

/** The minute alarm mask allows matching the alarm minute register with the 
*   current minute register.
*/
#define RTC_ALARM_MIN_MASK         (0x00000002uL)

/** The hour alarm mask allows matching the alarm hour register with the 
*   current hour register.
*/
#define RTC_ALARM_HOUR_MASK        (0x00000004uL)

/** The day of the week alarm mask allows matching the alarm hour register with 
*   the current day of the week register.
*/
#define RTC_ALARM_DAYOFWEEK_MASK   (0x00000008uL)

/** The day of the Month alarm mask allows matching the alarm hour register with 
*   the current day of the Month register.
*/
#define RTC_ALARM_DAYOFMONTH_MASK  (0x00000010uL)

/** The month alarm mask allows matching the alarm hour register with the
*   current month register.
*/
#define RTC_ALARM_MONTH_MASK       (0x00000020uL)

/** The year alarm mask allows matching the alarm hour register with the
*   current year register.
*/
#define RTC_ALARM_YEAR_MASK        (0x00000040uL)

/** \} group_rtc_alarm_mask */
/** \} group_constants */

#define RTC_DAYS_PER_WEEK       (7u)

#define RTC_MONTHS_PER_YEAR     (12uL)

#define RTC_HOURS_PER_DAY       (24uL)
#define RTC_HOURS_PER_HALF_DAY  (12uL)

#define RTC_SECONDS_PER_MINUTE  (60uL)
#define RTC_SECONDS_PER_HOUR    (3600uL)
#define RTC_SECONDS_PER_DAY     (24uL * 3600uL)

#define RTC_SECONDS_PER_LEAP_YEAR    (366uL * 24uL * 3600uL)
#define RTC_SECONDS_PER_NONLEAP_YEAR (365uL * 24uL * 3600uL)

#define RTC_UNIX_TIME_PM        ((12uL * 3600uL) + 1uL)

/* Unix time begins in 1970 year */
#define RTC_YEAR_0             (1970u)

/* Definition of date register fields */
#if(RTC_INITIAL_DATA_FORMAT == RTC__MM_DD_YYYY)
    #define RTC_10_MONTH_OFFSET   (28u)
    #define RTC_MONTH_OFFSET      (24u)
    #define RTC_10_DAY_OFFSET     (20u)
    #define RTC_DAY_OFFSET        (16u)
    #define RTC_1000_YEAR_OFFSET  (12u)
    #define RTC_100_YEAR_OFFSET   (8u)
    #define RTC_10_YEAR_OFFSET    (4u)
    #define RTC_YEAR_OFFSET       (0u)
#elif(RTC_INITIAL_DATA_FORMAT == RTC__DD_MM_YYYY)
    #define RTC_10_MONTH_OFFSET   (20u)
    #define RTC_MONTH_OFFSET      (16u)
    #define RTC_10_DAY_OFFSET     (28u)
    #define RTC_DAY_OFFSET        (24u)
    #define RTC_1000_YEAR_OFFSET  (12u)
    #define RTC_100_YEAR_OFFSET   (8u)
    #define RTC_10_YEAR_OFFSET    (4u)
    #define RTC_YEAR_OFFSET       (0u)
#else
    #define RTC_10_MONTH_OFFSET   (12u)
    #define RTC_MONTH_OFFSET      (8u)
    #define RTC_10_DAY_OFFSET     (4u)
    #define RTC_DAY_OFFSET        (0u)
    #define RTC_1000_YEAR_OFFSET  (28u)
    #define RTC_100_YEAR_OFFSET   (24u)
    #define RTC_10_YEAR_OFFSET    (20u)
    #define RTC_YEAR_OFFSET       (16u)
#endif /* (RTC_INITIAL_DATA_FORMAT == RTC__MM_DD_YYYY) */

#define RTC_10_MONTH_MASK          (0x00000001uL << RTC_10_MONTH_OFFSET)
#define RTC_MONTH_MASK             (0x0000000FuL << RTC_MONTH_OFFSET)
#define RTC_10_DAY_MASK            (0x00000003uL << RTC_10_DAY_OFFSET)
#define RTC_DAY_MASK               (0x0000000FuL << RTC_DAY_OFFSET)
#define RTC_1000_YEAR_MASK         (0x00000003uL << RTC_1000_YEAR_OFFSET)
#define RTC_100_YEAR_MASK          (0x0000000FuL << RTC_100_YEAR_OFFSET)
#define RTC_10_YEAR_MASK           (0x0000000FuL << RTC_10_YEAR_OFFSET)
#define RTC_YEAR_MASK              (0x0000000FuL << RTC_YEAR_OFFSET)

#define  RTC_MONTH_FULL_MASK       (RTC_10_MONTH_MASK  | RTC_MONTH_MASK)
#define  RTC_DAY_FULL_MASK         (RTC_10_DAY_MASK    | RTC_DAY_MASK)
#define  RTC_YEAR_FULL_MASK        (RTC_1000_YEAR_MASK | RTC_100_YEAR_MASK |\
                                                 RTC_10_YEAR_MASK   | RTC_YEAR_MASK)

/* Definition of time register fields */
#define RTC_TIME_FORMAT_OFFSET     (23u)
#define RTC_PERIOD_OF_DAY_OFFSET   (22u)
#define RTC_10_HOURS_OFFSET        (20u)
#define RTC_HOURS_OFFSET           (16u)
#define RTC_10_MINUTES_OFFSET      (12u)
#define RTC_MINUTES_OFFSET         (8u)
#define RTC_10_SECONDS_OFFSET      (4u)
#define RTC_SECONDS_OFFSET         (0u)

#define RTC_TIME_FORMAT_MASK       (0x00000001uL << RTC_TIME_FORMAT_OFFSET)
#define RTC_PERIOD_OF_DAY_MASK     (0x00000001uL << RTC_PERIOD_OF_DAY_OFFSET)
#define RTC_10_HOURS_MASK          (0x00000003uL << RTC_10_HOURS_OFFSET)
#define RTC_HOURS_MASK             (0x0000000FuL << RTC_HOURS_OFFSET)
#define RTC_10_MINUTES_MASK        (0x00000007uL << RTC_10_MINUTES_OFFSET)
#define RTC_MINUTES_MASK           (0x0000000FuL << RTC_MINUTES_OFFSET)
#define RTC_10_SECONDS_MASK        (0x00000007uL << RTC_10_SECONDS_OFFSET)
#define RTC_SECONDS_MASK           (0x0000000FuL << RTC_SECONDS_OFFSET)

#define RTC_HOURS_FULL_MASK        (RTC_10_HOURS_MASK   | RTC_HOURS_MASK)
#define RTC_MINUTES_FULL_MASK      (RTC_10_MINUTES_MASK | RTC_MINUTES_MASK)
#define RTC_SECONDS_FULL_MASK      (RTC_10_SECONDS_MASK | RTC_SECONDS_MASK)

#define RTC_STATUS_DST_OFFSET      (1u)
#define RTC_STATUS_ALARM_OFFSET    (2u)
#define RTC_STATUS_LY_OFFSET       (3u)
#define RTC_STATUS_AM_PM_OFFSET    (4u)

/* Number of bits per one BCD digit */
#define RTC_BCD_NUMBER_SIZE        (4u)
#define RTC_BCD_ONE_DIGIT_MASK     (0x0000000FuL)


/*******************************************************************************
*    External Variables
*******************************************************************************/
/**
* \addtogroup group_globals
* \{
*/

/**
* Indicates whether the RTC has been initialized; The variable is initialized to
* 0 and set to 1 the first time RTC_Start() is called. This allows the component
* to restart without reinitialization after the first call to the RTC_Start() 
* routine.
*/
extern uint8  RTC_initVar;

/** The DST start/stop status */
extern uint8  RTC_dstStatus;

/**
* The uint64 variable represents the standard Unix time (number of seconds 
* elapsed from January 1, 1970 00:00 hours UTC) in 64-bit 
*/
extern volatile uint64 RTC_unixTime;

/** The values for the time and date of the DST start */
extern RTC_DST_TIME RTC_dstStartTime;

/** The values for the time and date of the DST stop */
extern RTC_DST_TIME RTC_dstStopTime;

/**
* The last updated time and date values are stored in this structure (update 
* happens in Get time/date APIs)
*/
extern RTC_DATE_TIME RTC_currentTimeDate;

/** The alarm time and date values are stored in this variable */
extern RTC_DATE_TIME RTC_alarmCfgTimeDate;

/**
* This variable is used to mask alarm events; mask seconds alarm, mask minutes 
* alarm, and so on. It will have bit masks for each time item masking that item 
* for alarm generation
*/
extern uint32  RTC_alarmCfgMask;

/**
* This variable is used to indicate current active alarm status per time item 
* used in the alarm; whether seconds alarm is active, minute's alarm is active, 
* and so on. It will have bit masks for each time item (seconds, minutes, hours,
* day, and so on) showing the status
*/
extern uint32 RTC_alarmCurStatus;

/** \} group_globals */

extern void (*RTC_alarmCallbackPtr)(void);

extern const uint16 RTC_daysBeforeMonthTbl[RTC_MONTHS_PER_YEAR];
extern const uint8  RTC_daysInMonthTbl[RTC_MONTHS_PER_YEAR];


/*******************************************************************************
*    Function Prototypes
*******************************************************************************/
/**
* \addtogroup group_general
* \{
*/
void   RTC_Start(void);
void   RTC_Stop(void);
void   RTC_Init(void);
void   RTC_SetUnixTime(uint64 unixTime);
uint64 RTC_GetUnixTime(void);
void   RTC_SetPeriod(uint32 ticks, uint32 refOneSecTicks);
uint32 RTC_GetPeriod(void);
uint32 RTC_GetRefOneSec(void);
void   RTC_SetDateAndTime(uint32 inputTime, uint32 inputDate);
void   RTC_GetDateAndTime(RTC_DATE_TIME* dateTime);
uint32 RTC_GetTime(void);
uint32 RTC_GetDate(void);
void   RTC_SetAlarmDateAndTime(const RTC_DATE_TIME* alarmTime);
void   RTC_GetAlarmDateAndTime(RTC_DATE_TIME* alarmTimeDate);
void   RTC_SetAlarmMask(uint32 mask);
uint32 RTC_GetAlarmMask(void);
uint32 RTC_ReadStatus(void);
uint32 RTC_GetAlarmStatus(void);
void   RTC_ClearAlarmStatus(void);
void   RTC_SetDSTStartTime(const RTC_DST_TIME* dstStartTime,
                                              RTC_DST_DATETYPE_ENUM type);
void   RTC_SetDSTStopTime(const RTC_DST_TIME* dstStopTime,
                                             RTC_DST_DATETYPE_ENUM type);
uint32 RTC_ConvertBCDToDec(uint32 bcdNum);
uint32 RTC_ConvertDecToBCD(uint32 decNum);
void   RTC_Update(void);
void*  RTC_SetAlarmHandler(void (*CallbackFunction)(void));

static uint32 RTC_ConstructDate(uint32 month, uint32 day, uint32 year);
static uint32 RTC_ConstructTime(uint32 timeFormat, uint32 stateAmPm, uint32 hour, uint32 min, uint32 sec);
/** \} group_general */

static uint32 RTC_GetTimeFormat(uint32 inputTime);
static uint32 RTC_SetTimeFormat(uint32 inputTime, uint32 timeFormat);

/**
* \addtogroup group_general
* \{
*/
static uint32 RTC_LeapYear(uint32 year);
static uint32 RTC_IsBitSet(uint32 var, uint32 mask);
static uint32 RTC_GetSecond(uint32 inputTime);
static uint32 RTC_GetMinutes(uint32 inputTime);
static uint32 RTC_GetHours(uint32 inputTime);
static uint32 RTC_GetAmPm(uint32 inputTime);
static uint32 RTC_GetDay(uint32 date);
static uint32 RTC_GetMonth(uint32 date);
static uint32 RTC_GetYear(uint32 date);
/** \} group_general */

static uint32 RTC_SetSecond(uint32 inputTime, uint32 inputSecond);
static uint32 RTC_SetMinutes(uint32 inputTime, uint32 inputMinute);
static uint32 RTC_SetHours(uint32 inputTime, uint32 inputHours);
static uint32 RTC_SetAmPm(uint32 inputTime, uint32 periodOfDay);
static uint32 RTC_SetDay(uint32 inputDate, uint32 inputDay);
static uint32 RTC_SetMonth(uint32 inputDate, uint32 inputMonth);
static uint32 RTC_SetYear(uint32 inputDate, uint32 inputYear);

uint64 RTC_ConstructUnixAlarmTime(const RTC_DATE_TIME* alarmTime, uint8 alarmCfgMask);
uint64 RTC_GetDstUnixTime(const RTC_DST_TIME* dstTime);
uint64 RTC_GetNexAlarmTime(uint64 curUnixTime, uint8 alarmCfgMask);

static uint32 RTC_GetNextMinSec(uint32 curVal);
static uint32 RTC_GetNextHour(uint32 curVal);
static uint32 RTC_GetNextDay(uint32 curYear, uint32 curMonth, uint32 curDay, uint32 alarmCfgMask);
static uint32 RTC_GetNextMonth(uint32 curMonth);
static uint32 RTC_GetNextYear(uint32 curYear);
static uint32 RTC_GetDayOfWeek(uint32 day, uint32 month, uint32 year);
static uint32 RTC_DaysBeforeMonth(uint32 month, uint32 year);

uint32 RTC_DaysInMonth(uint32 month, uint32 year);

/**
* \addtogroup group_general
* \{
*/
void   RTC_UnixToDateTime(RTC_DATE_TIME* dateTime, uint64 unixTime, uint32 timeFormat);
uint64 RTC_DateTimeToUnix(uint32 inputDate, uint32 inputTime);

#if defined(CYDEV_RTC_SOURCE_WDT)
    #if((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE))
        static void RTC_CySysRtcSetCallback(uint32 wdtNumber);
        static void RTC_CySysRtcResetCallback(uint32 wdtNumber);
    #endif /* ((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE)) */
#endif /* (CYDEV_RTC_SOURCE_WDT) */

/** \} group_general */

uint32 RTC_RelativeToFixed(uint32 dayOfWeek, uint32 weekOfMonth, uint32 month, uint32 year);



/*******************************************************************************
* Function Name: RTC_IsBitSet
****************************************************************************//**
*
* Checks the state of a bit passed through parameter.
*
* \param var
* The variable to be checked.
*
* \param mask
* The mask for a bit to be checked.
*
* \return
*  0u - Bit is not set. <br> 1u - Bit is set.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_IsBitSet(uint32 var, uint32 mask)
{
    return ((mask == (var & mask)) ? 1Lu : 0Lu);
}


/*******************************************************************************
* Function Name: RTC_LeapYear
****************************************************************************//**
*
* Checks whether the year passed through the parameter is leap or no.
*
* \param year
* The year to be checked.
*
* \return
* 0u - The year is not leap <br> 1u - The year is leap.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_LeapYear(uint32 year)
{
    uint32 retVal;

    if(((0u == (year % 4Lu)) && (0u != (year % 100Lu))) || (0u == (year % 400Lu)))
    {
        retVal = 1uL;
    }
    else
    {
        retVal = 0uL;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: RTC_GetSecond
****************************************************************************//**
*
* Returns the seconds value from the time value that is passed as a/the 
* parameter.
*
* \param inputTime
* The time value.
*
* \return
* The seconds value.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetSecond(uint32 inputTime)
{
    uint32 retVal;

    retVal  = ((inputTime & RTC_10_SECONDS_MASK) >> RTC_10_SECONDS_OFFSET) * 10u;
    retVal += (inputTime & RTC_SECONDS_MASK) >> RTC_SECONDS_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_GetMinutes
****************************************************************************//**
*
* Returns the minutes value from the time value that is passed as a/the 
* parameter.
*
* \param inputTime
* The time value.
*
* \return
* The minutes value.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetMinutes(uint32 inputTime)
{
    uint32 retVal;

    retVal  = ((inputTime & RTC_10_MINUTES_MASK) >> RTC_10_MINUTES_OFFSET) * 10u;
    retVal += (inputTime & RTC_MINUTES_MASK) >> RTC_MINUTES_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_GetHours
****************************************************************************//**
*
* Returns the hours value from the time value that is passed as a/the parameter.
*
* \param inputTime
* The time value.
*
* \return
* The hours value.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetHours(uint32 inputTime)
{
    uint32 retVal;

    retVal  = ((inputTime & RTC_10_HOURS_MASK) >> RTC_10_HOURS_OFFSET) * 10u;
    retVal += (inputTime & RTC_HOURS_MASK) >> RTC_HOURS_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_GetAmPm
****************************************************************************//**
*
* Returns the AM/PM status from the time value that is passed as parameter.
*
* \param inputTime
* The time value.
*
* \return
* The am/pm period of day, see \ref group_rtc_am_pm.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetAmPm(uint32 inputTime)
{
    return (RTC_IsBitSet(inputTime, RTC_PERIOD_OF_DAY_MASK));
}


/*******************************************************************************
* Function Name: RTC_GetDay
****************************************************************************//**
*
* Returns the day value from the date value that is passed as parameter.
*
* \param date
* The date value.
*
* \return
* The day value.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetDay(uint32 date)
{
    uint32 retVal;

    retVal  = ((date & RTC_10_DAY_MASK) >> RTC_10_DAY_OFFSET) * 10u;
    retVal += (date & RTC_DAY_MASK) >> RTC_DAY_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_GetMonth
****************************************************************************//**
*
* Returns the month value from the date value that is passed as parameter.
*
* \param date
* The date value.
*
* \return
* The month value.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetMonth(uint32 date)
{
    uint32 retVal;

    retVal  = ((date & RTC_10_MONTH_MASK) >> RTC_10_MONTH_OFFSET) * 10u;
    retVal += (date & RTC_MONTH_MASK) >> RTC_MONTH_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_GetYear
****************************************************************************//**
*
* Returns the year value from the date value that is passed as parameter.
*
* \param date
* The date value.
*
* \return
* The year value.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_GetYear(uint32 date)
{
    uint32 retVal;

    retVal  = ((date & RTC_1000_YEAR_MASK) >> RTC_1000_YEAR_OFFSET) * 1000u;
    retVal += ((date & RTC_100_YEAR_MASK) >> RTC_100_YEAR_OFFSET) * 100u;
    retVal += ((date & RTC_10_YEAR_MASK) >> RTC_10_YEAR_OFFSET) * 10u;
    retVal += (date & RTC_YEAR_MASK) >> RTC_YEAR_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_SetSecond
****************************************************************************//**
* \internal
* Updates the time with the new second value.
*
* \param inputTime
* The current date.
*
* \param inputSecond
* The seconds value to be set to the time variable.
*
* \return
* The updated time variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetSecond(uint32 inputTime, uint32 inputSecond)
{
    inputTime &= ~(RTC_SECONDS_FULL_MASK);

    inputTime |= (inputSecond / 10u) << RTC_10_SECONDS_OFFSET;
    inputTime |= (inputSecond % 10u) << RTC_SECONDS_OFFSET;

    return(inputTime);
}


/*******************************************************************************
* Function Name: RTC_SetMinutes
****************************************************************************//**
* \internal
*  Updates the time with the new minute value.
*
* \param inputTime
* The current date.
*
* \param inputMinute
* The minutes value to be set to the time variable.
*
* \return
* The updated time variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetMinutes(uint32 inputTime, uint32 inputMinute)
{
    inputTime &= ~(RTC_MINUTES_FULL_MASK);

    inputTime |= (inputMinute / 10u) << RTC_10_MINUTES_OFFSET;
    inputTime |= (inputMinute % 10u) << RTC_MINUTES_OFFSET;

    return(inputTime);
}


/*******************************************************************************
* Function Name: RTC_SetHours
****************************************************************************//**
* \internal
*  Updates the time with the new hour value.
*
* \param inputTime
* The current date.
*
* \param inputHours
* The hours value to be set to the time variable.
*
* \return
* The updated time variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetHours(uint32 inputTime, uint32 inputHours)
{
    inputTime &= ~(RTC_HOURS_FULL_MASK);

    inputTime |= (inputHours / 10u) << RTC_10_HOURS_OFFSET;
    inputTime |= (inputHours % 10u) << RTC_HOURS_OFFSET;

    return(inputTime);
}


/*******************************************************************************
* Function Name: RTC_SetAmPm
****************************************************************************//**
* \internal
* Updates the time variable with the AmPm status.
*
* \param inputTime
* The current date.
*
* \param periodOfDay
* The AmPm status to be set to the time variable.
*
* \return
* The updated time variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetAmPm(uint32 inputTime, uint32 periodOfDay)
{
    if(0u != periodOfDay)
    {
        inputTime &= ~(RTC_PERIOD_OF_DAY_MASK);
    }
    else
    {
        inputTime |= RTC_PERIOD_OF_DAY_MASK;
    }

    return(inputTime);
}


/*******************************************************************************
* Function Name: RTC_SetDay
****************************************************************************//**
* \internal
* Updates the date variable with the new day value.
*
* \param inputDate
* The current date.
*
* \param inputDay
* day The month to be set to the date variable.
*
* \return
* The updated date variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetDay(uint32 inputDate, uint32 inputDay)
{
    inputDate &= ~(RTC_DAY_FULL_MASK);

    inputDate |= (inputDay / 10u) << RTC_10_DAY_OFFSET;
    inputDate |= (inputDay % 10u) << RTC_DAY_OFFSET;

    return(inputDate);
}


/*******************************************************************************
* Function Name: RTC_SetMonth
****************************************************************************//**
* \internal
* Updates the date variable with the new month value.
*
* \param inputDate
* The current date.
*
* \param inputMonth
* The month that to be set to the date variable.
*
* \return
* The updated date variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetMonth(uint32 inputDate, uint32 inputMonth)
{
    inputDate &= ~(RTC_MONTH_FULL_MASK);

    inputDate |= (inputMonth / 10u) << RTC_10_MONTH_OFFSET;
    inputDate |= (inputMonth % 10u) << RTC_MONTH_OFFSET;

    return(inputDate);
}


/*******************************************************************************
* Function Name: RTC_SetYear
****************************************************************************//**
*\internal
* Updates the date variable with the new year value.
*
* \param inputDate
* The current date.
*
* \param inputYear
* The year to be set to the date variable.
*
* \return
* The updated date variable.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetYear(uint32 inputDate, uint32 inputYear)
{
    inputDate  &= ~(RTC_YEAR_FULL_MASK);

    inputDate |= (inputYear / 1000u) << RTC_1000_YEAR_OFFSET;
    inputYear %= 1000u;

    inputDate |= (inputYear / 100u) << RTC_100_YEAR_OFFSET;
    inputYear %= 100u;

    inputDate |= (inputYear / 10u) << RTC_10_YEAR_OFFSET;
    inputDate |= (inputYear % 10u) << RTC_YEAR_OFFSET;

    return(inputDate);
}


/*******************************************************************************
* Function Name: RTC_GetNextMinSec
****************************************************************************//**
*\internal
* This is an internal function that calculates the value of the next Second/Minute
* that follows after the current Minute/Second.
*
* \param curVal
* The current Second/Minute value.
*
* \return
* Returns the Second/Minute which follows after the current Second/Minute.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetNextMinSec(uint32 curVal)
{
    return((curVal < 59u) ? (curVal + 1u) : 0u);
}


/*******************************************************************************
* Function Name: RTC_GetNextHour
****************************************************************************//**
* \internal
*  This is an internal function that calculates the value of the next Hour
*  that follows after the current Hour.
*
* \param curVal
* The current Hour value.
*
* \return
* Returns the Hour which follows after the current Hour.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetNextHour(uint32 curVal)
{
    return((curVal < 23u) ? (curVal + 1u) : 0u);
}


/*******************************************************************************
* Function Name: RTC_GetNextDay
****************************************************************************//**
* \internal
*  This is an internal function that calculates the value of the next Day
*  that follows after the current Day.
*
* \param curYear
* The current year.
*
* \param curMonth
* The current month.
*
* \param curDay
* The current day.
*
* \param alarmCfgMask
* Alarm Config Mask.
*
* \return
* Returns the day which follows after the current Day.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetNextDay(uint32 curYear, uint32 curMonth,
                                                    uint32 curDay, uint32 alarmCfgMask)
{
    uint32 daysInMonth;
    uint32 tmpVal;

    daysInMonth = RTC_DaysInMonth(curMonth, curYear);

    if(0u != (alarmCfgMask & RTC_ALARM_DAYOFWEEK_MASK))
    {
        tmpVal = curDay + RTC_DAYS_PER_WEEK;
        tmpVal = (tmpVal > daysInMonth) ? (tmpVal - daysInMonth) : tmpVal;
    }
    else
    {
        tmpVal = (curDay < daysInMonth) ? (curDay + 1u) : 1u;
    }

    return(tmpVal);
}


/*******************************************************************************
* Function Name: RTC_GetNextMonth
****************************************************************************//**
* \internal
* This is an internal function that calculates the value of the next month
* that follows after the current month.
*
* \param curMonth
* The current month.
*
* \return
* Returns the month which follows after the current month.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetNextMonth(uint32 curMonth)
{
    return((curMonth < 12u) ? (curMonth + 1u) : 1u);
}


/*******************************************************************************
* Function Name: RTC_GetNextYear
****************************************************************************//**
* \internal
* This is an internal function that calculates the value of the next year
* that follows after the current year.
*
* \param curYear
* The current year.
*
* \return
* Returns the year which follows after the current year.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetNextYear(uint32 curYear)
{
    return(curYear + 1u);
}


/*******************************************************************************
* Function Name: RTC_SetTimeFormat
****************************************************************************//**
* \internal
* Updates the "Time Format" value in the variable that contains time in the
* "HH:MM:SS" format.
*
* \param inputTime
* The current value of the time in the "HH:MM:SS" format.
*
* \param timeFormat
* Required time format
*
* \return
* Returns the updated value of the time in the "HH:MM:SS" format.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_SetTimeFormat(uint32 inputTime, uint32 timeFormat)
{
    inputTime &= ~(RTC_TIME_FORMAT_MASK);

    if((uint32)RTC_12_HOURS_FORMAT != timeFormat)
    {
        inputTime |= RTC_TIME_FORMAT_MASK;
    }

    return(inputTime);
}


/*******************************************************************************
* Function Name: RTC_GetTimeFormat
****************************************************************************//**
* \internal
* Reads the time format from the variable that contains time in the
* "HH:MM:SS" format.
*
* \param inputTime
* The current value of the time in the "HH:MM:SS" format.
*
* \return
* Returns the time format that is stored in the variable that contains time
* in the "HH:MM:SS" format.
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetTimeFormat(uint32 inputTime)
{
    return ((0uL != (inputTime & (1uL << RTC_TIME_FORMAT_OFFSET))) ?
                                       (uint32)RTC_12_HOURS_FORMAT :
                                       (uint32)RTC_24_HOURS_FORMAT);
}


/*******************************************************************************
* Function Name: RTC_ConstructTime
****************************************************************************//**
*
* Returns the time in the format used in APIs from individual elements
* passed (hour, min, sec etc)
*
* \param timeFormat
* The 12/24 hours time format, see \ref group_rtc_hour_format
*
* \param stateAmPm 
* The AM/PM status, see \ref group_rtc_am_pm.
*
* \param hour
* The hour.
*
* \param min
* The minute.
*
* \param sec
* The second.
*
* \return
* Time in the format used in API.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_ConstructTime(uint32 timeFormat, uint32 stateAmPm, 
                                                       uint32 hour, uint32 min, uint32 sec)
{
    uint32 retVal;

    retVal  = timeFormat << RTC_TIME_FORMAT_OFFSET;
    retVal |= stateAmPm << RTC_PERIOD_OF_DAY_OFFSET;

    retVal |= (hour / 10u) << RTC_10_HOURS_OFFSET;
    retVal |= (hour % 10u) << RTC_HOURS_OFFSET;

    retVal |= (min / 10u) << RTC_10_MINUTES_OFFSET;
    retVal |= (min % 10u) << RTC_MINUTES_OFFSET;

    retVal |= (sec / 10u) << RTC_10_SECONDS_OFFSET;
    retVal |= (sec % 10u) << RTC_SECONDS_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_ConstructDate
****************************************************************************//**
*
*  Returns the date in the format used in APIs from individual elements
*  passed (day. Month and year)
*
* \param month
* The month.
*
* \param day
* The day.
*
* \param year
* The year.
*
* \return
* The date in the format used in API.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_ConstructDate(uint32 month, uint32 day, uint32 year)
{
    uint32 retVal;
    uint32 tmpVal = year;

    retVal  = (month / 10u) << RTC_10_MONTH_OFFSET;
    retVal |= (month % 10u) << RTC_MONTH_OFFSET;

    retVal |= (day / 10u) << RTC_10_DAY_OFFSET;
    retVal |= (day % 10u) << RTC_DAY_OFFSET;

    retVal |= (year / 1000u) << RTC_1000_YEAR_OFFSET;
    tmpVal %= 1000u;

    retVal |= (tmpVal / 100u) << RTC_100_YEAR_OFFSET;
    tmpVal %= 100u;

    retVal |= (tmpVal / 10u) << RTC_10_YEAR_OFFSET;
    retVal |= (tmpVal % 10u) << RTC_YEAR_OFFSET;

    return (retVal);
}


/*******************************************************************************
* Function Name: RTC_GetDayOfWeek
****************************************************************************//**
* \internal
*
*  Returns a day of the week for a year, month, and day of month that are passed
*  through parameters. Zeller's congruence is used to calculate the day of
*  the week.
*
*  For the Georgian calendar, Zeller's congruence is:
*  h = (q + [13 * (m + 1)] + K + [K/4] + [J/4] - 2J) mod 7;
*
*  h - The day of the week (0 = Saturday, 1 = Sunday, 2 = Monday, ..., 
*  6 = Friday).
*  q - The day of the month.
*  m - The month (3 = March, 4 = April, 5 = May, ..., 14 = February).
*  K - The year of the century (year \mod 100).
*  J - The zero-based century (actually [year/100]) For example, the zero-based
*  centuries for 1995 and 2000 are 19 and 20 respectively (not to be
*  confused with the common ordinal century enumeration which indicates
*  20th for both cases).
*
* \note
* In this algorithm January and February are counted as months 13 and 14
* of the previous year.
*
* \param day
* The day of the month(1..31)
*
* \param month
* The month of the year, see \ref group_rtc_month
*
* \param year
* The year value.
*
* \return
* Returns a day of the week, see \ref group_rtc_day_of_the_week.
*
* \endinternal
*******************************************************************************/
static CY_INLINE uint32 RTC_GetDayOfWeek(uint32 day, uint32 month, uint32 year)
{
    uint32 retVal;

    /* Converts month number from regular convention
     * (1=January,..., 12=December) to convention required for this
     * algorithm(January and February are counted as months 13 and 14 of
     * previous year).
    */
    if(month < (uint32)RTC_MARCH)
    {
        month = 12u + month;
        year--;
    }

    /* Calculates Day of Week using Zeller's congruence algorithms */
    retVal = (day + (((month + 1u) * 26u) / 10u) + year + (year / 4u) + (6u * (year / 100u)) + (year / 400u)) % 7u;

    /* Makes correction for Saturday. Saturday number should be 7 instead of 0. */
    if(0u == retVal)
    {
        retVal = (uint32)RTC_SATURDAY;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: RTC_DaysBeforeMonth
****************************************************************************//**
*
* Calculates how many days elapsed from the beginning of the year to the
* beginning of the current month.
*
* \param month
* A month of a year, see \ref group_rtc_month
*
* \param year
* A year value.
*
* \return
* A number of days elapsed from the beginning of the year to the
* beginning of the current month passed through the parameters.
*
*******************************************************************************/
static CY_INLINE uint32 RTC_DaysBeforeMonth(uint32 month, uint32 year)
{
    uint32 retVal;

    retVal = RTC_daysBeforeMonthTbl[month - 1u];
    if((0u != RTC_LeapYear(year)) && (month > (uint32)RTC_FEBRUARY))
    {
        retVal++;
    }

    return(retVal);
}

#if defined(CYDEV_RTC_SOURCE_WDT)
    #if((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE))

        /*******************************************************************************
        * Function Name: RTC_CySysRtcSetCallback
        ****************************************************************************//**
        *
        *  This is an internal function that registers a callback for the
        *  RTC_Update() function by address "0".
        *
        * \param wdtNumber
        * The number of the WDT or DeepSleep Timer to be used to pull the
        * RTC_Update() function.
        *
        * The callback registered before by address "0" is replaced
        * by the RTC_Update() function.
        *
        *******************************************************************************/
        static CY_INLINE void RTC_CySysRtcSetCallback(uint32 wdtNumber)
        {
            #if((0u != CY_IP_WCO_WDT_EN) && (0u != CY_IP_SRSSLT))
                (void)CySysTimerSetInterruptCallback(wdtNumber, &RTC_Update);
            #else
                (void)CySysWdtSetIsrCallback(wdtNumber, &RTC_Update);
            #endif /* ((0u != CY_IP_WCO_WDT_EN) && (0u != CY_IP_SRSSLT)) */
        }


        /*******************************************************************************
        * Function Name: RTC_CySysRtcResetCallback
        ****************************************************************************//**
        *
        *  This is an internal function that clears a callback by address "0".
        *
        * \param wdtNumber
        * The number of the WDT or DeeSleep Timer to be cleared callback for.
        *
        * The callback registered before by address "0" is replaced
        * by the NULL pointer.
        *
        *******************************************************************************/
        static CY_INLINE void RTC_CySysRtcResetCallback(uint32 wdtNumber)
        {
            #if((0u != CY_IP_WCO_WDT_EN) && (0u != CY_IP_SRSSLT))
                (void)CySysTimerSetInterruptCallback(wdtNumber, (void *)0);
            #else
                (void)CySysWdtSetIsrCallback(wdtNumber, (void *)0);
            #endif /* ((0u != CY_IP_WCO_WDT_EN) && (0u != CY_IP_SRSSLT)) */
        }

    #endif /* ((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE)) */
#endif /* (CYDEV_RTC_SOURCE_WDT) */

#endif /* CY_RTC_P4_RTC_H */


/* [] END OF FILE */
