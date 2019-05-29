/***************************************************************************//**
* \file Name: RTC.c
* \version 1.30
*
*  This file provides the source code to the API for the RTC_P4 Component.
*
********************************************************************************
* \copyright
* Copyright 2015-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "RTC.h"

uint8  RTC_initVar;

volatile uint64 RTC_unixTime;
RTC_DATE_TIME RTC_currentTimeDate;

static uint32 RTC_updateTimePeriod;
static uint32 RTC_tickRefOneSec;

uint8  RTC_dstStatus;
static uint32 RTC_dstTimeOffset;
#if(0u != RTC_INITIAL_DST_STATUS)
    static volatile uint64 RTC_unixTimeDstStart;
    static volatile uint64 RTC_unixTimeDstStop;
    RTC_DST_TIME RTC_dstStartTime;
    RTC_DST_TIME RTC_dstStopTime;
#endif /* (0u != RTC_INITIAL_DST_STATUS) */

uint32 RTC_alarmCurStatus;
#if(0u != RTC_INITIAL_ALARM_STATUS)
    uint32  RTC_alarmCfgMask = 0xFFu;
    static volatile uint64 RTC_unixTimeAlarm;
    RTC_DATE_TIME RTC_alarmCfgTimeDate;
    void (*RTC_alarmCallbackPtr)(void) = (void *)0;
#endif /* (0u != RTC_INITIAL_ALARM_STATUS) */

const uint16 RTC_daysBeforeMonthTbl[RTC_MONTHS_PER_YEAR] = {
                                                 (0u), /* JANUARY */
                                                 (0u + 31u), /* FEBRUARY */
                                                 (0u + 31u + 28u), /* MARCH */
                                                 (0u + 31u + 28u + 31u), /* APRIL */
                                                 (0u + 31u + 28u + 31u + 30u), /* MAY */
                                                 (0u + 31u + 28u + 31u + 30u + 31u), /* JUNE */
                                                 (0u + 31u + 28u + 31u + 30u + 31u + 30u), /* JULY */
                                                 (0u + 31u + 28u + 31u + 30u + 31u + 30u + 31u), /* AUGUST */
                                                 (0u + 31u + 28u + 31u + 30u + 31u + 30u + 31u + 31u), /* SEPTEMBER */
                                                 (0u + 31u + 28u + 31u + 30u + 31u + 30u + 31u + 31u + 30u), /* OCTOBER */
                                                 (0u + 31u + 28u + 31u + 30u + 31u + 30u + 31u + 31u + 30u + 31u), /* NOVEMBER */
                                                 (0u + 31u + 28u + 31u + 30u + 31u + 30u + 31u + 31u + 30u + 31u + 30u)}; /* DECEMBER */

const uint8 RTC_daysInMonthTbl[RTC_MONTHS_PER_YEAR] = {RTC_DAYS_IN_JANUARY,
                                                                                 RTC_DAYS_IN_FEBRUARY,
                                                                                 RTC_DAYS_IN_MARCH,
                                                                                 RTC_DAYS_IN_APRIL,
                                                                                 RTC_DAYS_IN_MAY,
                                                                                 RTC_DAYS_IN_JUNE,
                                                                                 RTC_DAYS_IN_JULY,
                                                                                 RTC_DAYS_IN_AUGUST,
                                                                                 RTC_DAYS_IN_SEPTEMBER,
                                                                                 RTC_DAYS_IN_OCTOBER,
                                                                                 RTC_DAYS_IN_NOVEMBER,
                                                                                 RTC_DAYS_IN_DECEMBER};


/*******************************************************************************
* Function Name: RTC_Start
****************************************************************************//**
*
* Performs all the required calculations for the time and date registers and
* initializes the component along with the date and time selected in the
* customizer. 
*
* If "Implement RTC update manually" is disabled in the customizer
* and if WDT or DeepSleep timer is selected as a source in the clocks 
* configuration window (low frequency clocks tab), attaches RTC_Update API to a
* corresponding ISR callback of WDT or DeepSleep Timer.
*
* \note "Implement RTC update manually" checkbox is available for PSoC 4200L / 
* PSoC 4100M / PSoC 4200M / PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4000S / PSoC 
* 4100S and Analog Coprocessor.
*
*******************************************************************************/
void RTC_Start(void)
{
    if(0u == RTC_initVar)
    {
        RTC_Init();
    }

    #if defined(CYDEV_RTC_SOURCE_WDT)
        #if((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE))
            RTC_CySysRtcSetCallback(CYDEV_RTC_SOURCE_WDT);
        #endif /* ((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE)) */
    #endif /* (CYDEV_RTC_SOURCE_WDT) */
}


/*******************************************************************************
* Function Name: RTC_Stop
****************************************************************************//**
*
* Stops the time and date updates.
*
*******************************************************************************/
void RTC_Stop(void)
{
    #if defined(CYDEV_RTC_SOURCE_WDT)
        #if((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE))
            RTC_CySysRtcResetCallback(CYDEV_RTC_SOURCE_WDT);
        #endif /* ((0u != CYDEV_WDT_GENERATE_ISR) && (0u == RTC_INITIAL_UPDATE_MODE)) */
    #endif /* (CYDEV_RTC_SOURCE_WDT) */
}


/*******************************************************************************
* Function Name: RTC_Init
****************************************************************************//**
*
* Initializes or restores the component according to the customizer Configure
* dialogue settings.
*
* It is not necessary to call RTC_Init() because RTC_Start() API calls this 
* function and is the preferred method to begin component operation.
*
* All registers are set to values according to the customizer Configure
* dialogue. The default date value, if not set by the user before this function
* call, is 12:00:00 AM January 1, 2000.
*
*******************************************************************************/
void RTC_Init(void)
{
    uint32 tmpDate;
    uint32 tmpTime;

    #if(0u != RTC_INITIAL_DST_STATUS)
        RTC_DST_TIME dstTimeTmp;
    #endif /* (0u != RTC_INITIAL_DST_STATUS) */

    RTC_initVar = 1u;
    RTC_dstTimeOffset = 0uL;
    RTC_currentTimeDate.status = 0uL;

    RTC_dstStatus      = RTC_INITIAL_DST_STATUS;
    RTC_alarmCurStatus = RTC_INITIAL_ALARM_STATUS;

    tmpDate = RTC_ConstructDate(RTC_INITIAL_MONTH,
                                             RTC_INITIAL_DAY,
                                             RTC_INITIAL_YEAR);

    tmpTime = RTC_ConstructTime(RTC_INITIAL_TIME_FORMAT,
                                             0u,
                                            RTC_INITIAL_HOUR,
                                            RTC_INITIAL_MINUTE,
                                            RTC_INITIAL_SECOND);

    #if(0u != RTC_INITIAL_DST_STATUS)
        RTC_dstStatus = 0u;
        RTC_currentTimeDate.status = (1uL << RTC_STATUS_DST_OFFSET);

        dstTimeTmp.timeFormat    = RTC_INITIAL_DST_DATE_TYPE;
        dstTimeTmp.hour          = RTC_INITIAL_DST_START_HRS;
        dstTimeTmp.dayOfWeek     = RTC_INITIAL_DST_START_DOW;
        dstTimeTmp.weekOfMonth   = RTC_INITIAL_DST_START_WOM;
        dstTimeTmp.dayOfMonth    = RTC_INITIAL_DST_START_DOM;
        dstTimeTmp.month         = RTC_INITIAL_DST_START_MONTH;
        RTC_SetDSTStartTime(&dstTimeTmp, 
                                        (RTC_DST_DATETYPE_ENUM)RTC_INITIAL_DST_DATE_TYPE);

        dstTimeTmp.timeFormat    = RTC_INITIAL_DST_DATE_TYPE;
        dstTimeTmp.hour          = RTC_INITIAL_DST_STOP_HRS;
        dstTimeTmp.dayOfWeek     = RTC_INITIAL_DST_STOP_DOW;
        dstTimeTmp.weekOfMonth   = RTC_INITIAL_DST_STOP_WOM;
        dstTimeTmp.dayOfMonth    = RTC_INITIAL_DST_STOP_DOM;
        dstTimeTmp.month         = RTC_INITIAL_DST_STOP_MONTH;
        RTC_SetDSTStopTime(&dstTimeTmp, 
                                       (RTC_DST_DATETYPE_ENUM)RTC_INITIAL_DST_DATE_TYPE);

        RTC_unixTimeDstStart = RTC_GetDstUnixTime(&RTC_dstStartTime);
        RTC_unixTimeDstStop  = RTC_GetDstUnixTime(&RTC_dstStopTime);

        if((RTC_unixTime >= RTC_unixTimeDstStart) &&
           (RTC_unixTime <= RTC_unixTimeDstStop))
        {
            RTC_dstStatus = 1u;
            RTC_dstTimeOffset = RTC_SECONDS_PER_HOUR;
        }
    #endif /* (0u != RTC_INITIAL_DST_STATUS) */

    RTC_SetDateAndTime(tmpTime, tmpDate);
}


/*******************************************************************************
* Function Name: RTC_SetDateAndTime
****************************************************************************//**
*
* Sets the time and date values as the current time and date.
*
* \param inputTime
* The time value in the HH:MM:SS format. <br>
* "HH"- The 2nd 8-bit MSB that denotes the hour value.
* (0-23 for the 24-hour format and 1-12 for the 12-hour format. The MSB bit of 
* the value denotes AM/PM for the 12-hour format (0-AM and 1-PM). <br>
* "MM" - The 3nd 8-bit MSB denotes the minutes value, the valid 
* entries -> 0-59.<br>
* "SS" - The 8-bit LSB denotes the seconds value, the valid entries -> 0-59.
* Each byte is in the BCD format. Invalid time entries retain the
* previously set values.
*
* \param inputDate
* The date value in the format selected in the customizer.
* For the MM/DD/YYYY format: <br>
* "MM" - The 8-bit MSB denotes the month value in BCD, the valid 
* entries -> 1-12<br>
* "DD" - The 2nd 8-bit MSB denotes a day of the month value in BCD, the valid
* entries -> 1-31.<br>
* "YYYY" - The 16-bit LSB denotes a year in BCD, the valid entries -> 1900-2200.
* Each byte is in the BCD format. Invalid date entries retain the
* previously set values.
*
*******************************************************************************/
void RTC_SetDateAndTime(uint32 inputTime, uint32 inputDate)
{
    uint32 tmpDay;
    uint32 tmpMonth;
    uint32 tmpYear;

    tmpDay   = RTC_GetDay(inputDate);
    tmpMonth = RTC_GetMonth(inputDate);
    tmpYear  = RTC_GetYear(inputDate);

    RTC_unixTime = RTC_DateTimeToUnix(inputDate, inputTime);
    RTC_currentTimeDate.date = inputDate;
    RTC_currentTimeDate.time = inputTime;
    RTC_currentTimeDate.dayOfWeek = RTC_GetDayOfWeek(tmpDay, tmpMonth, tmpYear);

    #if(0u != RTC_INITIAL_DST_STATUS)
        RTC_unixTimeDstStart = RTC_GetDstUnixTime(&RTC_dstStartTime);
        RTC_unixTimeDstStop  = RTC_GetDstUnixTime(&RTC_dstStopTime);

        if((RTC_unixTime >= RTC_unixTimeDstStart) &&
           (RTC_unixTime <= RTC_unixTimeDstStop))
        {
            RTC_dstStatus = 1u;
            RTC_dstTimeOffset = RTC_SECONDS_PER_HOUR;
            RTC_unixTime -= RTC_dstTimeOffset;
        }
        else
        {
            RTC_dstStatus = 0u;
            RTC_dstTimeOffset = 0uL;
        }
    #endif /* (0u != RTC_INITIAL_DST_STATUS) */

    #if(0u != RTC_INITIAL_ALARM_STATUS)
        RTC_unixTimeAlarm = RTC_ConstructUnixAlarmTime(&RTC_alarmCfgTimeDate,
                                                                                 (uint8)RTC_alarmCfgMask);
    #endif /* (0u != RTC_INITIAL_ALARM_STATUS) */
}


/*******************************************************************************
* Function Name: RTC_SetUnixTime
****************************************************************************//**
*
* Sets the time in the Unix/Epoch time format - the number of seconds elapsed
* from January 1, 1970 UTC 00:00 hrs.
*
* \param unixTime
* The time value in the Unix time/Epoch time format.
*
*******************************************************************************/
void RTC_SetUnixTime(uint64 unixTime)
{
    RTC_DATE_TIME tmpDateTime;

    RTC_unixTime = unixTime;
    RTC_UnixToDateTime(&tmpDateTime, unixTime, (uint32)RTC_24_HOURS_FORMAT);

    #if(0u != RTC_INITIAL_DST_STATUS)
        RTC_unixTimeDstStart = RTC_GetDstUnixTime(&RTC_dstStartTime);
        RTC_unixTimeDstStop  = RTC_GetDstUnixTime(&RTC_dstStopTime);

        if((RTC_unixTime >= RTC_unixTimeDstStart) &&
           (RTC_unixTime <= RTC_unixTimeDstStop))
        {
            RTC_dstStatus = 1u;
            RTC_dstTimeOffset = RTC_SECONDS_PER_HOUR;
        }
        else
        {
            RTC_dstStatus = 0u;
            RTC_dstTimeOffset = 0uL;
        }
    #endif /* (0u != RTC_INITIAL_DST_STATUS) */
}


/*******************************************************************************
* Function Name: RTC_GetUnixTime
****************************************************************************//**
*
* Returns the time in the Unix/Epoch time format - the number of seconds 
* elapsed from January 1, 1970 UTC 00:00 hrs.
*
* \return
* Time The time value in the Unix time/Epoch time format.
*
*******************************************************************************/
uint64 RTC_GetUnixTime(void)
{
    return (RTC_unixTime);
}


/*******************************************************************************
* Function Name: RTC_SetPeriod
****************************************************************************//**
*
* Sets the RTC time update API period.
*
* The user needs to pass the period as
* a number of ticks and also a reference number of ticks taken by the same 
* clock source for one second. For instance, for a 32 kHz clock source and RTC
* period of 100 ms, the "ticks" value is 3200 and the "refOneSecTicks" value
* is 32000. This value is used to increment the time every time
* RTC_Update() API is called.
*
* \param ticks
* The clock period taken as a number of ticks.
*
* \param refOneSecTicks
* The reference number of ticks taken by the same clock source
* for one second (the input clock frequency in Hz).
*
*******************************************************************************/
void RTC_SetPeriod(uint32 ticks, uint32 refOneSecTicks)
{
    RTC_updateTimePeriod = ticks;
    RTC_tickRefOneSec    = refOneSecTicks;
}


/*******************************************************************************
* Function Name: RTC_SetPeriod
****************************************************************************//**
*
* Gets the RTC time update API period.
*
* \return
* Period The clock period taken as a number of ticks.
*
*******************************************************************************/
uint32 RTC_GetPeriod(void)
{
    return(RTC_updateTimePeriod);
}


/*******************************************************************************
* Function Name: RTC_GetRefOneSec
****************************************************************************//**
*
* Gets the RTC time update API period.
*
* \return
* Period The reference number of ticks taken by the RTC clock source for one
* second.
*
*******************************************************************************/
uint32 RTC_GetRefOneSec(void)
{
    return(RTC_tickRefOneSec);
}


/*******************************************************************************
* Function Name: RTC_GetDateAndTime
****************************************************************************//**
*
* Reads the current time and date.
*
* \param dateTime
* The pointer to the RTC_date_time structure to which time and date is returned.
*
*******************************************************************************/
void RTC_GetDateAndTime(RTC_DATE_TIME* dateTime)
{
    RTC_UnixToDateTime(dateTime, (RTC_unixTime  + RTC_dstTimeOffset),
                                                                            RTC_INITIAL_TIME_FORMAT);
}


/*******************************************************************************
* Function Name: RTC_GetTime
****************************************************************************//**
*
* Reads the current time.
*
* \return
* time The time value in the format selected by the user (12/24 hr);
* The time value is available in the BCD format.
*
* \warning
* Using RTC_GetTime and RTC_GetDate API separately might result
* in errors when the time wraps around the end of the day. To avoid this,
* use RTC_GetDateAndTime API.
*
*******************************************************************************/
uint32 RTC_GetTime(void)
{
    RTC_UnixToDateTime(&RTC_currentTimeDate,
                                    (RTC_unixTime  + RTC_dstTimeOffset),
                                     RTC_INITIAL_TIME_FORMAT);

    return(RTC_currentTimeDate.time);
}


/*******************************************************************************
* Function Name: RTC_GetDate
****************************************************************************//**
*
* Reads the current date.
*
* \return
* date The value of date in the user selected format. The date value is 
* available in the BCD format.
*
* \note
* Using RTC_GetTime and RTC_GetDate API separately
* might result in errors when the time wraps around the end of the day. To
* avoid this, use RTC_GetDateAndTime API.
*
*******************************************************************************/
uint32 RTC_GetDate(void)
{
    RTC_UnixToDateTime(&RTC_currentTimeDate,
                                    (RTC_unixTime  + RTC_dstTimeOffset),
                                     RTC_INITIAL_TIME_FORMAT);

    return(RTC_currentTimeDate.date);
}


#if(0u != RTC_INITIAL_ALARM_STATUS)
    /*******************************************************************************
    * Function Name: RTC_SetAlarmDateAndTime
    ****************************************************************************//**
    *
    * Writes the time and date values as the current alarm time and date.
    *
    * \param alarmTime
    * The pointer to the RTC_date_time global structure 
    * where new values of the alarm time and date are stored, 
    * see \ref RTC_DATE_TIME.
    *
    * \note
    * Invalid time entries are written with "00:00:00:00" for the 24-hour format 
    * and "AM 12:00:00:00" for the 12-hour format. Invalid date entries are 
    * written with a date equivalent to 01-JAN-2000.
    *
    *******************************************************************************/
    void RTC_SetAlarmDateAndTime(const RTC_DATE_TIME* alarmTime)
    {
        RTC_alarmCfgTimeDate = *alarmTime;
        RTC_unixTimeAlarm = 
        RTC_ConstructUnixAlarmTime(alarmTime, (uint8)RTC_alarmCfgMask);
    }


    /*******************************************************************************
    * Function Name: RTC_GetAlarmDateAndTime
    ****************************************************************************//**
    *
    * Reads the current alarm time and date.
    *
    * \param alarmTimeDate
    * The pointer to the RTC_date_time structure to which the alarm 
    * date and time are returned, see \ref RTC_DATE_TIME.
    *
    *******************************************************************************/
    void RTC_GetAlarmDateAndTime(RTC_DATE_TIME* alarmTimeDate)
    {
        *alarmTimeDate = RTC_alarmCfgTimeDate;
    }


    /*******************************************************************************
    * Function Name: RTC_SetAlarmMask
    ****************************************************************************//**
    *
    * Writes the Alarm Mask software register with one bit per time/date entry.
    * The alarm is true when all masked time/date values match the Alarm values.
    * Generated only if the alarm functionality is enabled.
    *
    * \param mask
    * The Alarm Mask software register value. The values shown below can be
    * OR'ed and passed as an argument as well, see \ref group_rtc_alarm_mask.
    *
    *******************************************************************************/
    void RTC_SetAlarmMask(uint32 mask)
    {
        RTC_alarmCfgMask = (uint8)mask;
        RTC_unixTimeAlarm = 
        RTC_ConstructUnixAlarmTime(&RTC_alarmCfgTimeDate, (uint8)mask);
    }


    /*******************************************************************************
    * Function Name: RTC_GetAlarmMask
    ****************************************************************************//**
    *
    * Reads the Alarm Mask software register. Generated only if the alarm 
    * functionality is enabled, see \ref group_rtc_alarm_mask.
    *
    *******************************************************************************/
    uint32 RTC_GetAlarmMask(void)
    {
        return((uint32)RTC_alarmCfgMask);
    }


    /*******************************************************************************
    * Function Name: RTC_GetAlarmStatus
    ****************************************************************************//**
    *
    *  Returns the alarm status of RTC.
    *
    * \return
    * The Alarm active status. This bit is high when the current time and date 
    * match the alarm time and date. <br>
    * 0 - The Alarm status is not active <br>
    * 1 - The Alarm status is active.
    *
    *******************************************************************************/
    uint32 RTC_GetAlarmStatus(void)
    {
        return((0uL != (RTC_currentTimeDate.status & (1uL << RTC_STATUS_ALARM_OFFSET))) ?
                1uL : 
                0uL);
    }


    /*******************************************************************************
    * Function Name: RTC_ClearAlarmStatus
    ****************************************************************************//**
    *
    *  Clears the alarm status of RTC.
    *
    * \note
    * The Alarm active (AA) flag clears after read. This bit will be set in the next
    * alarm match event only. If Alarm is set on only minutes and the alarm
    * minutes is 20 minutes - the alarm triggers once every 20th minute of
    * every hour.
    *
    *******************************************************************************/
    void RTC_ClearAlarmStatus(void)
    {
        RTC_currentTimeDate.status &= (~(1uL << RTC_STATUS_ALARM_OFFSET));
        RTC_alarmCurStatus = 0u;
    }


    /*******************************************************************************
    * Function Name: RTC_SetAlarmHandler
    ****************************************************************************//**
    *
    *  This API sets the function to be called when the alarm goes off / triggers.
    *  This API is generated only if the alarm functionality is enabled in the
    *  customizer.
    *
    * \param CallbackFunction
    * The callback function address.
    *
    * \return
    * A previous callback function address.
    *
    *******************************************************************************/
    void* RTC_SetAlarmHandler(void (*CallbackFunction)(void))
    {
        void (*tmpCallbackPtr)(void);

        tmpCallbackPtr = RTC_alarmCallbackPtr;
        RTC_alarmCallbackPtr = CallbackFunction;

        return((void*)tmpCallbackPtr);
    }


    /*******************************************************************************
    * Function Name: RTC_GetNexAlarmTime
    ****************************************************************************//**
    *
    *  This is an internal function that calculates the time of the next alarm
    *  in the UNIX format taking into account the current "Alarm Config Mask".
    *
    * \param curUnixTime
    * The current time value in the UNIX format.
    *
    * \param alarmCfgMask
    * The current Alarm Config Mask.
    *
    * \return
    * Returns time of the next alarm in the UNIX format.
    *
    *******************************************************************************/
    uint64 RTC_GetNexAlarmTime(uint64 curUnixTime, uint8 alarmCfgMask)
    {
        uint32 prevVal;
        uint32 nextVal;
        uint32 carryFlag;
        uint32 daysInMonth;
        RTC_DATE_TIME curDateTime;

        carryFlag = 1u;
        RTC_UnixToDateTime(&curDateTime, curUnixTime, (uint32)RTC_24_HOURS_FORMAT);

        /* Calculates Second value of next alarm time based on current time and Alarm Config Mask */
        if(0uL != ((~((uint32)alarmCfgMask)) & RTC_ALARM_SEC_MASK))
        {
            prevVal = RTC_GetSecond(curDateTime.time);
            nextVal = RTC_GetNextMinSec(prevVal);
            curDateTime.time = RTC_SetSecond(curDateTime.time, nextVal);
            carryFlag = (nextVal < prevVal) ? 1u : 0u;
        }

        /* Calculates Minute value of next alarm time based on current time and Alarm Config Mask */
        if((0uL != carryFlag) && (0uL != ((~((uint32)alarmCfgMask)) & RTC_ALARM_MIN_MASK)))
        {
            prevVal = RTC_GetMinutes(curDateTime.time);
            nextVal = RTC_GetNextMinSec(prevVal);
            curDateTime.time = RTC_SetMinutes(curDateTime.time, nextVal);
            carryFlag = (nextVal < prevVal) ? 1u : 0u;
        }

        /* Calculates Hour value of next alarm time based on current time and Alarm Config Mask */
        if((0uL != carryFlag) && (0uL != ((~((uint32)alarmCfgMask)) & RTC_ALARM_HOUR_MASK)))
        {
            prevVal = RTC_GetHours(curDateTime.time);
            nextVal = RTC_GetNextHour(prevVal);
            curDateTime.time = RTC_SetHours(curDateTime.time, nextVal);
            carryFlag = (nextVal < prevVal) ? 1u : 0u;
        }

        /* Calculates Day value of next alarm time based on current time and Alarm Config Mask */
        if((0uL != carryFlag) && (0uL != ((~((uint32)alarmCfgMask)) & RTC_ALARM_DAYOFMONTH_MASK)))
        {
            prevVal = RTC_GetDay(curDateTime.date);
            if(0u != (alarmCfgMask & RTC_ALARM_DAYOFWEEK_MASK))
            {
                daysInMonth = RTC_DaysInMonth(RTC_GetMonth(curDateTime.date),
                                                           RTC_GetYear(curDateTime.date));
                nextVal = prevVal + RTC_DAYS_PER_WEEK;
                nextVal = (nextVal > daysInMonth) ? (nextVal - daysInMonth) : nextVal;
            }
            else
            {
                nextVal = RTC_GetNextDay(RTC_GetYear(curDateTime.date),
                                                      RTC_GetMonth(curDateTime.date),
                                                      RTC_GetDay(curDateTime.date),
                                                      (~((uint32)alarmCfgMask)));
            }
            curDateTime.date = RTC_SetDay(curDateTime.date, nextVal);
            carryFlag = (nextVal < prevVal) ? 1u : 0u;
        }

        /* Calculates Month value of next alarm time based on current time and Alarm Config Mask */
        if((0uL != carryFlag) && (0uL != ((~((uint32)alarmCfgMask)) & RTC_ALARM_MONTH_MASK)))
        {
            prevVal = RTC_GetMonth(curDateTime.date);
            nextVal = RTC_GetNextMonth(prevVal);
            curDateTime.date = RTC_SetMonth(curDateTime.date, nextVal);
            carryFlag = (nextVal < prevVal) ? 1u : 0u;
        }

        /* Calculates Year value of next alarm time based on current time and Alarm Config Mask */
        if((0uL != carryFlag) && (0uL != ((~((uint32)alarmCfgMask)) & RTC_ALARM_YEAR_MASK)))
        {
            prevVal = RTC_GetYear(curDateTime.date);
            nextVal = RTC_GetNextYear(prevVal);
            curDateTime.date = RTC_SetYear(curDateTime.date, nextVal);
        }

        return(RTC_DateTimeToUnix(curDateTime.date, curDateTime.time));
    }


    /*******************************************************************************
    * Function Name: RTC_ConstructUnixAlarmTime
    ****************************************************************************//**
    *
    * This is an internal function that calculates the time of the first alarm
    * in the UNIX format taking into account the current "Alarm Config Mask".
    *
    * \param alarmTime
    * Desired alarm time in the regular time format, 
    * see \ref RTC_DATE_TIME
    *
    * \param alarmCfgMask
    * The current Alarm Config Mask.
    *
    * \return
    * time of the first alarm in the UNIX format.
    *
    *******************************************************************************/
    uint64 RTC_ConstructUnixAlarmTime(const RTC_DATE_TIME* alarmTime, uint8 alarmCfgMask)
    {
        uint32 tmpCurVal;
        uint32 tmpNextVal;
        uint32 carryFlag;
        uint32 tmpAlarmDate;
        uint32 tmpAlarmTime;
        uint32 daysInMonth;
        RTC_DATE_TIME curDateTime;

        tmpAlarmDate = 0u;
        tmpAlarmTime = 0u;
        RTC_UnixToDateTime(&curDateTime, RTC_unixTime, 
                                       (uint32)RTC_24_HOURS_FORMAT);

        /* Calculates Seconds value of first Alarm based on current time and Alarm Config Mask */
        tmpCurVal =  RTC_GetSecond(curDateTime.time);
        tmpNextVal = RTC_GetSecond(alarmTime->time);
        carryFlag = (tmpCurVal > tmpNextVal) ? 1u : 0u;
        if(0u != ((~((uint32)alarmCfgMask)) & RTC_ALARM_SEC_MASK))
        {
            tmpNextVal = (0u != carryFlag) ? 0u : tmpCurVal;
        }
        tmpAlarmTime = RTC_SetSecond(tmpAlarmTime, tmpNextVal);

        /* Calculates Minutes value of first Alarm based on current time and Alarm Config Mask */
        tmpCurVal  = RTC_GetMinutes(curDateTime.time);
        if(0u != ((~((uint32)alarmCfgMask)) & RTC_ALARM_MIN_MASK))
        {
            tmpNextVal = (0u != carryFlag) ? RTC_GetNextMinSec(tmpCurVal) : tmpCurVal;
        }
        else
        {
            tmpNextVal = RTC_GetMinutes(alarmTime->time);
        }
        carryFlag  = (tmpNextVal < tmpCurVal) ? 1u : 0u;
        tmpAlarmTime =  RTC_SetMinutes(tmpAlarmTime, tmpNextVal);

        /* Calculates Hours value of first Alarm based on current time and Alarm Config Mask */
        tmpCurVal  = RTC_GetHours(curDateTime.time);
        if(0u != ((~((uint32)alarmCfgMask)) & RTC_ALARM_HOUR_MASK))
        {
            tmpNextVal = (0u != carryFlag) ? RTC_GetNextHour(tmpCurVal) : tmpCurVal;
        }
        else
        {
            tmpNextVal = RTC_GetHours(alarmTime->time);
            if((uint32)RTC_24_HOURS_FORMAT != RTC_GetTimeFormat(alarmTime->time))
            {
                if((uint32)RTC_AM != RTC_GetAmPm(alarmTime->time))
                {
                    tmpNextVal += RTC_HOURS_PER_HALF_DAY;
                }
            }
        }
        carryFlag  = (tmpNextVal < tmpCurVal) ? 1u : 0u;
        tmpAlarmTime =  RTC_SetHours(tmpAlarmTime, tmpNextVal);

        /* Calculates Day value of first Alarm based on current time and Alarm Config Mask */
        tmpCurVal  = RTC_GetDay(curDateTime.date);
        tmpNextVal = RTC_GetDay(alarmTime->date);
        if(0u != ((~((uint32)alarmCfgMask)) & RTC_ALARM_DAYOFMONTH_MASK))
        {
            if(0u != (alarmCfgMask & RTC_ALARM_DAYOFWEEK_MASK))
            {
                daysInMonth = RTC_DaysInMonth(RTC_GetMonth(curDateTime.date),
                                                           RTC_GetYear(curDateTime.date));
                tmpNextVal = 
                (curDateTime.dayOfWeek <= alarmTime->dayOfWeek) ? (alarmTime->dayOfWeek - curDateTime.dayOfWeek) :
                                ((RTC_DAYS_PER_WEEK - curDateTime.dayOfWeek) + alarmTime->dayOfWeek);
                tmpNextVal = tmpCurVal + tmpNextVal;
                tmpNextVal = (tmpNextVal > daysInMonth) ? (tmpNextVal - daysInMonth) : tmpNextVal;
            }
            else
            {
                tmpNextVal = 
                (0u == carryFlag) ? tmpCurVal : RTC_GetNextDay(RTC_GetYear(curDateTime.date),
                                                RTC_GetMonth(curDateTime.date),
                                                tmpCurVal,
                                                RTC_ALARM_DAYOFMONTH_MASK);
            }
        }
        carryFlag  = (tmpNextVal < tmpCurVal) ? 1u : 0u;
        tmpAlarmDate =  RTC_SetDay(tmpAlarmDate, tmpNextVal);

        /* Calculates Month value of first Alarm based on current time and Alarm Config Mask */
        tmpCurVal  = RTC_GetMonth(curDateTime.date);
        if(0u != ((~((uint32)alarmCfgMask)) & RTC_ALARM_MONTH_MASK))
        {
            tmpNextVal = (0u != carryFlag) ? RTC_GetNextMonth(tmpCurVal) : tmpCurVal;
        }
        else
        {
            tmpNextVal = RTC_GetMonth(alarmTime->date);
        }
        carryFlag  = (tmpNextVal < tmpCurVal) ? 1u : 0u;
        tmpAlarmDate =  RTC_SetMonth(tmpAlarmDate, tmpNextVal);

        /* Calculates Year value of first Alarm based on current time and Alarm Config Mask */
        tmpCurVal  = RTC_GetYear(curDateTime.date);
        if(0u != ((~((uint32)alarmCfgMask)) & RTC_ALARM_MONTH_MASK))
        {
            tmpNextVal = (0u != carryFlag) ? RTC_GetNextYear(tmpCurVal) : tmpCurVal;
        }
        else
        {
            tmpNextVal = RTC_GetYear(alarmTime->date);
        }
        tmpAlarmDate =  RTC_SetYear(tmpAlarmDate, tmpNextVal);

        return(RTC_DateTimeToUnix(tmpAlarmDate, tmpAlarmTime));
    }
#endif/* (0u != RTC_INITIAL_ALARM_STATUS) */


/*******************************************************************************
* Function Name: RTC_ReadStatus
****************************************************************************//**
*
* Reads the Status software register, which has flags for DST (DST),
* Leap Year (LY), AM/PM (AM_PM).
*
* \return
* The values shown below are OR'ed and returned if more than one status
* bits are set, see \ref group_rtc_status.
*
* \note
* Reading the status without sync with the date and time read may cause an
* error due to a roll-over at AM/PM, the end of a year, the end of a day;
* RTC_GetDateAndTime() API is used to obtain the status and
* the status member of the returned structure can be checked with the masks.
*
*******************************************************************************/
uint32 RTC_ReadStatus(void)
{
    uint32 tmpYear;
    RTC_DATE_TIME tmpTimeDate;

    RTC_GetDateAndTime(&tmpTimeDate);
    tmpYear = RTC_GetYear(tmpTimeDate.date);

    if(0uL != RTC_LeapYear(tmpYear))
    {
        RTC_currentTimeDate.status |= (1uL << RTC_STATUS_LY_OFFSET);
    }
    else
    {
        RTC_currentTimeDate.status &= ~(1uL << RTC_STATUS_LY_OFFSET);
    }


    if(0uL != RTC_GetAmPm(tmpTimeDate.time))
    {
        RTC_currentTimeDate.status |= (1uL << RTC_STATUS_AM_PM_OFFSET);
    }
    else
    {
        RTC_currentTimeDate.status &= ~(1uL << RTC_STATUS_AM_PM_OFFSET);
    }

    return(RTC_currentTimeDate.status);
}


#if(0u != RTC_INITIAL_DST_STATUS)
    /*******************************************************************************
    * Function Name: RTC_SetDSTStartTime
    ****************************************************************************//**
    *
    * Stores the DST Start time.
    *
    * Only generated if DST is enabled. The date passed can be relative or fixed. 
    * For a relative date, the user needs to provide a valid day of a week, a 
    * week of a month and a month in the dstStartTime structure.
    * For a fixed date, the user needs to enter a valid day of a month and a month 
    * in the dstStartTime structure. The hour value is optional and if invalid
    * taken as 00 hrs. Invalid entries are not stored and the DST start date
    * retains a previous value or no value at all.
    *
    * \param dstStartTime
    * The DST Start time register value, see \ref RTC_DST_TIME
    *
    * \param type
    * Defines the DST operation mode, see \ref RTC_DST_DATETYPE_ENUM.
    *
    *******************************************************************************/
    void RTC_SetDSTStartTime(const RTC_DST_TIME* dstStartTime,
                                                RTC_DST_DATETYPE_ENUM type)
    {
        RTC_dstStartTime.timeFormat    = (uint8)type;
        RTC_dstStartTime.hour          = dstStartTime->hour;
        RTC_dstStartTime.dayOfWeek     = dstStartTime->dayOfWeek;
        RTC_dstStartTime.weekOfMonth   = dstStartTime->weekOfMonth;
        RTC_dstStartTime.dayOfMonth    = dstStartTime->dayOfMonth;
        RTC_dstStartTime.month         = dstStartTime->month;

        RTC_unixTimeDstStart = RTC_GetDstUnixTime(&RTC_dstStartTime);

        if((RTC_unixTime >= RTC_unixTimeDstStart) &&
           (RTC_unixTime <= RTC_unixTimeDstStop))
        {
            RTC_dstStatus = 1u;
            RTC_dstTimeOffset = RTC_SECONDS_PER_HOUR;
        }
        else
        {
            RTC_dstStatus = 0u;
            RTC_dstTimeOffset = 0uL;
        }
    }


    /*******************************************************************************
    * Function Name: RTC_SetDSTStopTime
    ****************************************************************************//**
    *
    * Stores the DST Stop time.
    * 
    * Only generated if DST is enabled. The date passed can be relative or fixed. 
    * For a relative date, the user needs to provide a valid day of a week, a week 
    * of a month and a month in the dstStopTime structure.
    * For a fixed date, the user needs to enter a valid day of a month and a month
    * in the dstSoptTime structure. The hour value is optional and if invalid 
    * taken as 00 hrs. Invalid entries are not stored and the DST start date 
    * retains a previous value or no value at all.
    *
    * \param dstStopTime
    * DST Stop time register values, see \ref RTC_DST_TIME
    *
    * \param type
    * Defines the DST operation mode, see \ref RTC_DST_DATETYPE_ENUM.
    *
    *******************************************************************************/
    void RTC_SetDSTStopTime(const RTC_DST_TIME* dstStopTime,
                                               RTC_DST_DATETYPE_ENUM type)
    {

        RTC_dstStopTime.timeFormat  = (uint8)type;
        RTC_dstStopTime.hour           = dstStopTime->hour;
        RTC_dstStopTime.dayOfWeek      = dstStopTime->dayOfWeek;
        RTC_dstStopTime.weekOfMonth = dstStopTime->weekOfMonth;
        RTC_dstStopTime.dayOfMonth     = dstStopTime->dayOfMonth;
        RTC_dstStopTime.month         = dstStopTime->month;

        RTC_unixTimeDstStop  = RTC_GetDstUnixTime(&RTC_dstStopTime);

        if((RTC_unixTime >= RTC_unixTimeDstStart) &&
           (RTC_unixTime <= RTC_unixTimeDstStop))
        {
            RTC_dstStatus = 1u;
            RTC_dstTimeOffset = RTC_SECONDS_PER_HOUR;
        }
        else
        {
            RTC_dstStatus = 0u;
            RTC_dstTimeOffset = 0uL;
        }
    }


    /*******************************************************************************
    * Function Name: RTC_GetDstUnixTime
    ****************************************************************************//**
    *
    * Calculates the DST Start/Stop time in the UNIX format.
    *
    * \param dstTime
    * The DST Start/Stop time in the regular time format,
    * see \ref RTC_DST_TIME
    *
    * \return
    * The DST Start/Stop time in the UNIX format.
    *
    *******************************************************************************/
    uint64 RTC_GetDstUnixTime(const RTC_DST_TIME* dstTime)
    {
        uint32 tmpYear;
        uint32 tmpDate;
        uint64 dstUnixTime;

        RTC_DATE_TIME tmpTimeDate;

        RTC_UnixToDateTime(&tmpTimeDate, RTC_unixTime, RTC_INITIAL_TIME_FORMAT);
        tmpYear = RTC_GetYear(tmpTimeDate.date);

        if(dstTime->timeFormat == (uint8)RTC_DST_DATE_FIXED)
        {
            tmpDate =  RTC_ConstructDate(dstTime->month, dstTime->dayOfMonth, tmpYear);
        }
        else
        {
            tmpDate = 
            RTC_RelativeToFixed(dstTime->dayOfWeek, dstTime->weekOfMonth, dstTime->month, tmpYear);
        }

        tmpTimeDate.time = 
        RTC_ConstructTime((uint32)RTC_24_HOURS_FORMAT, 0u, dstTime->hour, 0u, 0u);
        dstUnixTime = RTC_DateTimeToUnix(tmpDate, tmpTimeDate.time);

        return(dstUnixTime);
    }

#endif /* (0u != RTC_INITIAL_DST_STATUS) */


/*******************************************************************************
* Function Name: RTC_ConvertBCDToDec
****************************************************************************//**
*
* Converts a 4-byte BCD number into a 4-byte hexadecimal number. Each byte is
* converted individually and returned as an individual byte in the 32-bit
* variable.
*
* \param bcdNum
* A 4-byte BCD number. Each byte represents BCD.
* 0x11223344 -> 4 bytes 0x11, 0x22, 0x33 and 0x44 the in BCD format.
*
* \return
* decNum A 4-byte hexadecimal equivalent number of the BCD number.
* BCD number 0x11223344 -> returned hexadecimal number 0x0B16212C.
*
*******************************************************************************/
uint32 RTC_ConvertBCDToDec(uint32 bcdNum)
{
    uint32 i;
    uint32 mult;
    uint32 retVal;

    mult   = 1u;
    retVal = 0u;

    for(i = 0u; i < 16u; i++)
    {
        retVal += (bcdNum & RTC_BCD_ONE_DIGIT_MASK) * mult;
        bcdNum >>= RTC_BCD_NUMBER_SIZE;
        mult *= 10u;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: RTC_ConvertDecToBCD
****************************************************************************//**
*
* Converts a 4-byte hexadecimal number into a 4-byte BCD number. Each byte
* is converted individually and returned as an individual byte in the 32-bit
* variable.
*
* \param decNum
* A 4-byte hexadecimal number. Each byte is represented in hex.
* 0x11223344 -> 4 bytes 0x11, 0x22, 0x33 and 0x44 in the hex format.
*
* \return
* bcdNum - A 4-byte BCD equivalent of the passed hexadecimal number. 
* Hexadecimal number 0x11223344 -> returned BCD number 0x17345168.
*
*******************************************************************************/
uint32 RTC_ConvertDecToBCD(uint32 decNum)
{
    uint32 shift;
    uint32 tmpVal;
    uint32 retVal;

    shift  = 0u;
    retVal = 0u;
    tmpVal = decNum;

    do
    {
        retVal |= ((tmpVal % 10u) << shift);
        tmpVal /= 10u;
        shift  += RTC_BCD_NUMBER_SIZE;
    }
    while(tmpVal >= 10u);

    retVal |= (tmpVal << shift);

    return(retVal);
}


/*******************************************************************************
* Function Name: RTC_Update
****************************************************************************//**
*
* This API updates the time registers and performs alarm/DST check.
*
* This function increments the time/date registers by an input clock period.
* The period is set by RTC_SetPeriod() API or WDT period selected 
* for RTC in the clocks configuration window (low frequency clocks tab) 
* interface every time it is called.
*
* API is automatically mapped to the callback slot of WDT or DeepSleep Timer 
* and period, if the configuration is as follows:
* 1) Option "Implement RTC update manually" in the customizer is unchecked
* 2) One of WDTs or DeeSleep Timers is selected in the "Use for RTC" panel of 
* the low frequency clocks tab
* 3) Option "Implementation by IDE" is selected in the "Timer (WDT) ISR" panel.
*
* If option "Implement RTC update manually" is checked in the customizer or
* option "None" is selected in the "Use for RTC" panel, it is the user's
* responsibility:
* 1) to call this API from the clock ISR to be used as the RTC's input
* 2) set the period of the RTC through RTC_SetPeriod() API.
*
* \note Updates the Unix time register, updates the alarm and DST status.
*
*******************************************************************************/
void RTC_Update(void)
{
    static uint32 RTC_currentTickNumber = 0u;
    RTC_currentTickNumber += RTC_updateTimePeriod;

    if(RTC_currentTickNumber >= RTC_tickRefOneSec)
    {
        RTC_unixTime++;
        RTC_currentTickNumber = 
        (uint32) RTC_currentTickNumber - RTC_tickRefOneSec;

    #if(0u != RTC_INITIAL_DST_STATUS)
        if(RTC_unixTime == RTC_unixTimeDstStart)
        {
            RTC_dstStatus = 1u;
            RTC_dstTimeOffset = RTC_SECONDS_PER_HOUR;
        }

        if(RTC_unixTime == (RTC_unixTimeDstStop - RTC_dstTimeOffset))
        {
            RTC_dstStatus = 0u;
            RTC_dstTimeOffset = 0u;
            RTC_unixTimeDstStart = RTC_GetDstUnixTime(&RTC_dstStartTime);
            RTC_unixTimeDstStop  = RTC_GetDstUnixTime(&RTC_dstStopTime);
        }
    #endif /* (0u != RTC_INITIAL_DST_STATUS) */

    #if(0u != RTC_INITIAL_ALARM_STATUS)
        if((RTC_unixTime + RTC_dstTimeOffset) == RTC_unixTimeAlarm)
        {
            RTC_currentTimeDate.status |= (1uL << RTC_STATUS_ALARM_OFFSET);
            RTC_alarmCurStatus = RTC_alarmCfgMask;

            RTC_unixTimeAlarm = 
            RTC_GetNexAlarmTime(RTC_unixTime + RTC_dstTimeOffset,
                                                                   (uint8)RTC_alarmCfgMask);

            /* Call callback function only after new alarm time is obtained.
            * Cypress ticket #264756
            */
            if (RTC_alarmCallbackPtr != NULL)
            {
                RTC_alarmCallbackPtr();
            }
        }
    #endif/* (0u != RTC_INITIAL_ALARM_STATUS) */
    }
}


/*******************************************************************************
* Function Name: RTC_DateTimeToUnix
****************************************************************************//**
*
* This is an internal function to convert the date and time from the regular 
* time format into the UNIX time format.
*
* \param inputDate
* The date in the selected in the customizer "date format"
*
* \param inputTime
* The time in the defined "time format"
*
* \return
* Returns the date and time in the UNIX format
*
*******************************************************************************/
uint64 RTC_DateTimeToUnix(uint32 inputDate, uint32 inputTime)
{
    uint32 i;
    uint32 tmpYear;
    uint32 tmpMonth;
    uint32 tmpVal;
    uint64 unixTime;

    unixTime = 0u;
    tmpYear = RTC_GetYear(inputDate);

    /* Calculate seconds from epoch start up to (but not including) current year */
    for(i = RTC_YEAR_0; i < tmpYear; i++)
    {
        if(0u != RTC_LeapYear(i))
        {
            unixTime += RTC_SECONDS_PER_LEAP_YEAR;
        }
        else
        {
            unixTime += RTC_SECONDS_PER_NONLEAP_YEAR;
        }
    }

    /* Calculates how many seconds had elapsed in this year prior to the current month */
    tmpMonth = RTC_GetMonth(inputDate);
    tmpVal = RTC_DaysBeforeMonth(tmpMonth, tmpYear);

    /* Calculates how many seconds are in current month days prior to today */
    unixTime += (uint64) tmpVal * RTC_SECONDS_PER_DAY;
    unixTime += ((uint64) RTC_GetDay(inputDate) - 1u) * RTC_SECONDS_PER_DAY;

    /* Calculates how many seconds have elapsed today up to the current hour */
    tmpVal = RTC_GetHours(inputTime);
    if ((uint32)RTC_24_HOURS_FORMAT != RTC_GetTimeFormat(inputTime))
    {
        uint32 curAmpm;

        curAmpm = RTC_GetAmPm(inputTime);

        /* Add 12 hours as it is past midday period */
        if (((uint32)RTC_AM != curAmpm) && (tmpVal < 12u))
        {
            tmpVal += RTC_HOURS_PER_HALF_DAY;
        }
        /* Set hours to zero as it is a midnight */
        else if (((uint32)RTC_AM == curAmpm) && (tmpVal == 12u))
        {
            tmpVal = 0U;
        }
        else
        {
            /* Do nothing */
        }
    }
    unixTime += (uint64)tmpVal * RTC_SECONDS_PER_HOUR;

    /* Calculates how many seconds have elapsed today up to the current minute */
    unixTime += ((uint64) RTC_GetMinutes(inputTime)) * RTC_SECONDS_PER_MINUTE;

    /* Add remaining seconds of current minute */
    unixTime += RTC_GetSecond(inputTime);

    return(unixTime);
}


/*******************************************************************************
* Function Name: RTC_UnixToDateTime
****************************************************************************//**
*
* This is an internal function to convert the date and time from the UNIX time 
* format into the regular time format
*
* \param RTC_DATE_TIME* dateTime
* The time and date structure which will be updated time from unixTime value
*
* \param unixTime
* The time in unix format
*
* \param timeFormat
* dst format of time, see \ref RTC_DST_DATETYPE_ENUM
*
*******************************************************************************/
void RTC_UnixToDateTime(RTC_DATE_TIME* dateTime, uint64 unixTime, uint32 timeFormat)
{
    uint32 tmpMinute;
    uint32 tmpHour;
    uint32 tmpAmPmState;

    uint32 tmpDay;
    uint32 tmpMonth;
    uint32 tmpYear;

    uint32 tmpVar;

    tmpAmPmState = 0u;
    tmpYear = RTC_YEAR_0;
    tmpVar = RTC_SECONDS_PER_NONLEAP_YEAR;

    /* Calculates current year value. Variable tmpYear
    *  increments while it contains value greater than number
    *  of seconds in current year.
    */
    while(unixTime >= tmpVar)
    {
        unixTime -= tmpVar;
        tmpYear++;

        if(0u != RTC_LeapYear(tmpYear))
        {
            tmpVar = RTC_SECONDS_PER_LEAP_YEAR;
        }
        else
        {
            tmpVar = RTC_SECONDS_PER_NONLEAP_YEAR;
        }
    }

    /* Calculates current month value. The tmpMonth variable increments while 
    *  unixTime variable value is greater than time interval from beginning 
    *  of current year to beginning of current month
    */
    tmpMonth = (uint32)RTC_JANUARY;
    tmpVar = RTC_DaysInMonth(tmpMonth, tmpYear) * RTC_SECONDS_PER_DAY;

    while(unixTime >= tmpVar)
    {
        unixTime -= tmpVar;
        tmpMonth++;
        tmpVar = RTC_DaysInMonth(tmpMonth, tmpYear) * RTC_SECONDS_PER_DAY;
    }

    /* Calculates current day value */
    tmpDay = (uint32) (unixTime / RTC_SECONDS_PER_DAY);
    tmpVar = tmpDay * RTC_SECONDS_PER_DAY;
    unixTime -= (unixTime >= tmpVar) ? tmpVar : 0u;
    tmpDay += 1u;

    /* Calculates current hour value. If function works in 12-Hour mode,
     * it converts time to 12-Hours mode and calculates AmPm status */
    tmpHour = (uint32) (unixTime / RTC_SECONDS_PER_HOUR);
    tmpVar  = tmpHour * RTC_SECONDS_PER_HOUR;
    if((uint32)RTC_24_HOURS_FORMAT != timeFormat)
    {
        if(unixTime > RTC_UNIX_TIME_PM)
        {
            tmpAmPmState = RTC_PM;
            tmpHour = (tmpHour > 12u) ? (tmpHour - 12u) : tmpHour;
        }
        else
        {
            tmpAmPmState = RTC_AM;
            tmpHour = (0u != tmpHour) ? tmpHour : 12u;
        }
    }
    unixTime -= (unixTime >= tmpVar) ? tmpVar : 0u;

    /* Calculates current minute */
    tmpMinute = (uint32) (unixTime / RTC_SECONDS_PER_MINUTE);
    tmpVar = tmpMinute * RTC_SECONDS_PER_MINUTE;

    /* Calculates current second */
    unixTime -= (unixTime >= (uint64) tmpVar) ? (uint64) tmpVar : 0ul;

    dateTime->date = RTC_ConstructDate(tmpMonth, tmpDay, tmpYear);
    dateTime->time = RTC_ConstructTime(timeFormat, tmpAmPmState, tmpHour, tmpMinute, (uint32) unixTime);
    dateTime->dayOfWeek = RTC_GetDayOfWeek(tmpDay, tmpMonth, tmpYear);
}


/*******************************************************************************
* Function Name: RTC_RelativeToFixed
****************************************************************************//**
*
* This is an internal function to convert a relative date into a fixed date.
*
* \param dayOfWeek
* A day of a week, see \ref group_rtc_day_of_the_week
*
* \param weekOfMonth
* A week of a month, see \ref group_rtc_dst_week_of_month
*
* \param month
* A month of a year, see \ref group_rtc_month
*
* \param year
* A year value
*
* \return A date in the "date format"
*
*******************************************************************************/
uint32 RTC_RelativeToFixed(uint32 dayOfWeek, uint32 weekOfMonth, uint32 month, uint32 year)
{
    uint32 curDay;
    uint32 curWeek;
    uint32 daysInMonth;
    uint32 dayOfMonthTmp;

    uint32 retVal;

    curDay      = 1u;
    curWeek     = (uint32)RTC_FIRST;
    daysInMonth = RTC_DaysInMonth(month, year);
    dayOfMonthTmp = curDay;

    while((curWeek <= weekOfMonth) && (curDay <= daysInMonth))
    {
        if(dayOfWeek == RTC_GetDayOfWeek(curDay, month, year))
        {
            dayOfMonthTmp = curDay;
            curWeek++;
        }
        curDay++;
    }

    retVal = RTC_ConstructDate(month, dayOfMonthTmp, year);

    return(retVal);
}


/*******************************************************************************
* Function Name: RTC_DaysInMonth
****************************************************************************//**
*
* Returns a number of days in a month passed through the parameters.
*
* \param month
* A month of a year, see \ref group_rtc_month.
*
* \param year
* A year value.
*
* \return
* A number of days in a month in the year passed through the parameters
*
*******************************************************************************/
uint32 RTC_DaysInMonth(uint32 month, uint32 year)
{
    uint32 retVal;

    retVal = RTC_daysInMonthTbl[month - 1u];
    if((uint32)RTC_FEBRUARY == month)
    {
        if(0u != RTC_LeapYear(year))
        {
            retVal++;
        }
    }

    return(retVal);
}


/* [] END OF FILE */
