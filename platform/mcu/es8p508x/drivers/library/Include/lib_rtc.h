/*********************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 **********************************************************/
#ifndef __LIBRTC_H__
#define __LIBRTC_H__

#include "ES8P508x.h"
#include "type.h"

typedef enum
{
    RTC_LOSC = 0x0,     
    RTC_LRC  = 0x1,     
    RTC_PLCK_256 = 0x2,
    RTC_PLCK = 0x3,     
}RTC_TYPE_CLKS;

typedef enum
{
    RTC_HOUR12 = 0x0, 
    RTC_HOUR24 = 0x1, 
}RTC_TYPE_TIME;

typedef enum 
{
    RTC_Interrupt_Source_Second = 0,    
    RTC_Interrupt_Source_Minute = 1,    
    RTC_Interrupt_Source_Hour = 2,      
    RTC_Interrupt_Source_Day = 3,       
    RTC_Interrupt_Source_Month = 4,     
    RTC_Interrupt_Source_DayALE = 5,    
    RTC_Interrupt_Source_WeekALE = 6,  
} RTC_Interrupt_Source;

void RTC_Init(RTC_TYPE_CLKS CLKx,RTC_TYPE_TIME HOURx);
uint32_t RTC_ReadHourmode(void);
uint32_t RTC_ReadSecond(void);
uint32_t RTC_ReadMinute(void);
uint32_t RTC_ReadHour(uint32_t *meridiem);
uint32_t RTC_ReadDay(void);
uint32_t RTC_ReadMonth(void);
uint32_t RTC_ReadYear(void);
uint32_t RTC_ReadWeek(void);

ErrorStatus RTC_WriteSecond(uint32_t second);
ErrorStatus RTC_WriteMinute(uint32_t minute);
ErrorStatus RTC_WriteHour(uint32_t hour, uint32_t meridiem);
ErrorStatus RTC_WriteDay(uint32_t day);
ErrorStatus RTC_WriteMonth(uint32_t month);
ErrorStatus RTC_WriteYear(uint32_t year);
ErrorStatus RTC_WriteWeek(uint32_t week);

uint32_t RTC_ReadWeekAlarmMinute(void);
uint32_t RTC_ReadWeekAlarmHour(uint32_t *meridiem);
uint32_t RTC_ReadWeekAlarmWeek(void);
uint32_t RTC_ReadDayAlarmMinute(void);
uint32_t RTC_ReadDayAlarmHour(uint32_t *meridiem);

ErrorStatus RTC_WriteWeekAlarmMinute(uint32_t minute);
ErrorStatus RTC_WriteWeekAlarmHour(uint32_t hour, uint32_t meridiem);
ErrorStatus RTC_WriteWeekAlarmWeek(uint32_t week);
ErrorStatus RTC_WriteDayAlarmMinute(uint32_t minute);
ErrorStatus RTC_WriteDayAlarmHour(uint32_t hour, uint32_t meridiem);

void RTC_InterruptEnable(RTC_Interrupt_Source src);
void RTC_InterruptDisable(RTC_Interrupt_Source src);
ITStatus RTC_GetITStatus(RTC_Interrupt_Source src);
FlagStatus RTC_GetFlagStatus(RTC_Interrupt_Source src);
void RTC_ClearAllITFlag(void);

#endif

/*************************END OF FILE**********************/
