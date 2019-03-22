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

#ifndef __SWM320_RTC_H__
#define __SWM320_RTC_H__


#define RTC_SUN   0x01
#define RTC_MON   0x02
#define RTC_TUE   0x04
#define RTC_WED   0x08
#define RTC_THU   0x10
#define RTC_FRI   0x20
#define RTC_SAT   0x40


typedef struct {
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Date;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
    uint8_t  SecondIEn;
    uint8_t  MinuteIEn;
} RTC_InitStructure;

typedef struct {
    uint8_t  Days;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
    uint8_t  AlarmIEn;
} RTC_AlarmStructure;

typedef struct {
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Date;
    uint8_t  Day;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
} RTC_DateTime;

void RTC_Init(RTC_TypeDef * RTCx, RTC_InitStructure * initStruct);
void RTC_Start(RTC_TypeDef * RTCx);
void RTC_Stop(RTC_TypeDef * RTCx);

void RTC_GetDateTime(RTC_TypeDef * RTCx, RTC_DateTime * dateTime);

void RTC_AlarmSetup(RTC_TypeDef * RTCx, RTC_AlarmStructure * alarmStruct);

void RTC_IntSecondEn(RTC_TypeDef * RTCx);
void RTC_IntSecondDis(RTC_TypeDef * RTCx);
void RTC_IntSecondClr(RTC_TypeDef * RTCx);
uint32_t RTC_IntSecondStat(RTC_TypeDef * RTCx);
void RTC_IntMinuteEn(RTC_TypeDef * RTCx);
void RTC_IntMinuteDis(RTC_TypeDef * RTCx);
void RTC_IntMinuteClr(RTC_TypeDef * RTCx);
uint32_t RTC_IntMinuteStat(RTC_TypeDef * RTCx);
void RTC_IntHourEn(RTC_TypeDef * RTCx);
void RTC_IntHourDis(RTC_TypeDef * RTCx);
void RTC_IntHourClr(RTC_TypeDef * RTCx);
uint32_t RTC_IntHourStat(RTC_TypeDef * RTCx);
void RTC_IntDateEn(RTC_TypeDef * RTCx);
void RTC_IntDateDis(RTC_TypeDef * RTCx);
void RTC_IntDateClr(RTC_TypeDef * RTCx);
uint32_t RTC_IntDateStat(RTC_TypeDef * RTCx);
void RTC_IntAlarmEn(RTC_TypeDef * RTCx);
void RTC_IntAlarmDis(RTC_TypeDef * RTCx);
void RTC_IntAlarmClr(RTC_TypeDef * RTCx);
uint32_t RTC_IntAlarmStat(RTC_TypeDef * RTCx);

#endif //__SWM320_RTC_H__
