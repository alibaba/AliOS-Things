/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include "SWM320.h"
#include "SWM320_rtc.h"


static uint32_t calcWeekDay(uint32_t year, uint32_t month, uint32_t date);

void RTC_Init(RTC_TypeDef * RTCx, RTC_InitStructure * initStruct)
{
    SYS->CLKEN |= (1 << SYS_CLKEN_RTCBKP_Pos);

    SYS->LRCCR &= ~(1 << SYS_LRCCR_OFF_Pos);

    SYS->CLKEN |= (1 << SYS_CLKEN_RTC_Pos) |
                  ((uint32_t)1 << SYS_CLKEN_ALIVE_Pos);

    RTC_Stop(RTCx);

    while(RTCx->CFGABLE == 0);

    RTCx->MINSEC = (initStruct->Second << RTC_MINSEC_SEC_Pos) |
                   (initStruct->Minute << RTC_MINSEC_MIN_Pos);

    RTCx->DATHUR = (initStruct->Hour << RTC_DATHUR_HOUR_Pos) |
                   ((initStruct->Date - 1) << RTC_DATHUR_DATE_Pos);

    RTCx->MONDAY = (calcWeekDay(initStruct->Year, initStruct->Month, initStruct->Date) << RTC_MONDAY_DAY_Pos) |
                   ((initStruct->Month - 1) << RTC_MONDAY_MON_Pos);

    RTCx->YEAR = initStruct->Year - 1901;

    RTCx->LOAD = 1 << RTC_LOAD_TIME_Pos;

    RTCx->IF = 0x1F;
    RTCx->IE = (initStruct->SecondIEn << RTC_IE_SEC_Pos) |
               (initStruct->MinuteIEn << RTC_IE_MIN_Pos);

    if(initStruct->SecondIEn | initStruct->MinuteIEn)
    {
        NVIC_EnableIRQ(RTC_IRQn);
    }
    else
    {
        NVIC_DisableIRQ(RTC_IRQn);
    }
}

void RTC_Start(RTC_TypeDef * RTCx)
{
    RTCx->EN = 1;
}

void RTC_Stop(RTC_TypeDef * RTCx)
{
    RTCx->EN = 0;
}

void RTC_GetDateTime(RTC_TypeDef * RTCx, RTC_DateTime * dateTime)
{
    dateTime->Year = RTCx->YEAR + 1901;
    dateTime->Month = ((RTCx->MONDAY & RTC_MONDAY_MON_Msk) >> RTC_MONDAY_MON_Pos) + 1;
    dateTime->Date = ((RTCx->DATHUR & RTC_DATHUR_DATE_Msk) >> RTC_DATHUR_DATE_Pos) + 1;
    dateTime->Day = 1 << ((RTCx->MONDAY & RTC_MONDAY_DAY_Msk) >> RTC_MONDAY_DAY_Pos);
    dateTime->Hour = (RTCx->DATHUR & RTC_DATHUR_HOUR_Msk) >> RTC_DATHUR_HOUR_Pos;
    dateTime->Minute = (RTCx->MINSEC & RTC_MINSEC_MIN_Msk) >> RTC_MINSEC_MIN_Pos;
    dateTime->Second = (RTCx->MINSEC & RTC_MINSEC_SEC_Msk) >> RTC_MINSEC_SEC_Pos;
}

void RTC_AlarmSetup(RTC_TypeDef * RTCx, RTC_AlarmStructure * alarmStruct)
{
    while(RTCx->CFGABLE == 0);

    RTCx->MINSECAL = (alarmStruct->Second << RTC_MINSECAL_SEC_Pos) |
                     (alarmStruct->Minute << RTC_MINSECAL_MIN_Pos);

    RTCx->DAYHURAL = (alarmStruct->Hour << RTC_DAYHURAL_HOUR_Pos) |
                     (alarmStruct->Days << RTC_DAYHURAL_SUN_Pos);

    RTCx->LOAD = 1 << RTC_LOAD_ALARM_Pos;
    while(RTCx->LOAD & RTC_LOAD_ALARM_Msk);

    RTCx->IF = (1 << RTC_IF_ALARM_Pos);
    RTCx->IE &= ~RTC_IE_ALARM_Msk;
    RTCx->IE |= (alarmStruct->AlarmIEn << RTC_IE_ALARM_Pos);

    if(alarmStruct->AlarmIEn)  NVIC_EnableIRQ(RTC_IRQn);
}

static uint32_t calcWeekDay(uint32_t year, uint32_t month, uint32_t date)
{
    uint32_t i, cnt = 0;
    const uint32_t daysOfMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for(i = 1; i < month; i++) cnt += daysOfMonth[i];

    cnt += date;

    if((year%4 == 0) && ((year%100 != 0) || (year%400 == 0)) && (month >= 3)) cnt += 1;

    cnt += (year - 1901) * 365;

    for(i = 1901; i < year; i++)
    {
        if((i%4 == 0) && ((i%100 != 0) || (i%400 == 0))) cnt += 1;
    }

    return (cnt+1) % 7;
}

void RTC_IntSecondEn(RTC_TypeDef * RTCx)
{
    RTCx->IE |= (1 << RTC_IE_SEC_Pos);
}

void RTC_IntSecondDis(RTC_TypeDef * RTCx)
{
    RTCx->IE &= ~(1 << RTC_IE_SEC_Pos);
}

void RTC_IntSecondClr(RTC_TypeDef * RTCx)
{
    RTCx->IF = (1 << RTC_IF_SEC_Pos);
}

uint32_t RTC_IntSecondStat(RTC_TypeDef * RTCx)
{
    return (RTCx->IF & RTC_IF_SEC_Msk) ? 1 : 0;
}

void RTC_IntMinuteEn(RTC_TypeDef * RTCx)
{
    RTCx->IE |= (1 << RTC_IE_MIN_Pos);
}

void RTC_IntMinuteDis(RTC_TypeDef * RTCx)
{
    RTCx->IE &= ~(1 << RTC_IE_MIN_Pos);
}

void RTC_IntMinuteClr(RTC_TypeDef * RTCx)
{
    RTCx->IF = (1 << RTC_IF_MIN_Pos);
}

uint32_t RTC_IntMinuteStat(RTC_TypeDef * RTCx)
{
    return (RTCx->IF & RTC_IF_MIN_Msk) ? 1 : 0;
}

void RTC_IntHourEn(RTC_TypeDef * RTCx)
{
    RTCx->IE |= (1 << RTC_IE_HOUR_Pos);
}

void RTC_IntHourDis(RTC_TypeDef * RTCx)
{
    RTCx->IE &= ~(1 << RTC_IE_HOUR_Pos);
}

void RTC_IntHourClr(RTC_TypeDef * RTCx)
{
    RTCx->IF = (1 << RTC_IF_HOUR_Pos);
}

uint32_t RTC_IntHourStat(RTC_TypeDef * RTCx)
{
    return (RTCx->IF & RTC_IF_HOUR_Msk) ? 1 : 0;
}

void RTC_IntDateEn(RTC_TypeDef * RTCx)
{
    RTCx->IE |= (1 << RTC_IE_DATE_Pos);
}

void RTC_IntDateDis(RTC_TypeDef * RTCx)
{
    RTCx->IE &= ~(1 << RTC_IE_DATE_Pos);
}

void RTC_IntDateClr(RTC_TypeDef * RTCx)
{
    RTCx->IF = (1 << RTC_IF_DATE_Pos);
}

uint32_t RTC_IntDateStat(RTC_TypeDef * RTCx)
{
    return (RTCx->IF & RTC_IF_DATE_Msk) ? 1 : 0;
}

void RTC_IntAlarmEn(RTC_TypeDef * RTCx)
{
    RTCx->IE |= (1 << RTC_IE_ALARM_Pos);
}

void RTC_IntAlarmDis(RTC_TypeDef * RTCx)
{
    RTCx->IE &= ~(1 << RTC_IE_ALARM_Pos);
}

void RTC_IntAlarmClr(RTC_TypeDef * RTCx)
{
    RTCx->IF = (1 << RTC_IF_ALARM_Pos);
}

uint32_t RTC_IntAlarmStat(RTC_TypeDef * RTCx)
{
    return (RTCx->IF & RTC_IF_ALARM_Msk) ? 1 : 0;
}
