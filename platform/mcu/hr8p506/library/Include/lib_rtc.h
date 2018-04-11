/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_rtc.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  Timer模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIB_RTC_H__
#define __LIB_RTC_H__

#include <stdint.h>
#include "HR8P506.h"
#include "type.h"

/*RTC时钟源选择*/
typedef enum
{
    RTC_LOSC = 0x0,     //外部32768HZ时钟
    RTC_LRC  = 0x1,     //内部LRC时钟
    RTC_PCLK256 = 0x2,  //PCLK/256
    RTC_PCLK = 0x3,     //PCLK
}RTC_TYPE_CLKS;

/*RTC时钟12/24小时制选择*/
typedef enum
{
    RTC_HOUR12 = 0X0, //12小时制
    RTC_HOUR24 = 0X1, //24小时制
}RTC_TYPE_TIME;


/* RTC中断源选择 */
typedef enum {
    RTC_Interrupt_Source_Second = 0,    // 秒中断
    RTC_Interrupt_Source_Minute = 1,    // 分中断
    RTC_Interrupt_Source_Hour = 2,      // 小时中断
    RTC_Interrupt_Source_Day = 3,       // 天中断
    RTC_Interrupt_Source_Month = 4,     // 月中断
    RTC_Interrupt_Source_DayALE = 5,    // 日闹钟中断
    RTC_Interrupt_Source_WeekALE = 6,   // 周闹钟中断
} RTC_Interrupt_Source;

/********************* RTC模块函数声明 *********************/
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
