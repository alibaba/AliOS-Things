///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: rtc_control.h
// maintainer: Sam
///////////////////////////////////////////////////////////////////////////////

#ifndef __RTC_CONTROL_H__
#define __RTC_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "rtc.h"
#include "breakpoint.h"
//// 铃声类型
//#define INTER_RING_TYPE 0 // 内置铃声
//#define USB_RING_TYPE   1 // U盘铃声
//#define SD_RING_TYPE    2 // SD卡铃声

//// 闹钟提醒方式
//#define PLAYER_REMIND 0// 来闹钟时，直接进入播放模式，并播放铃声
//#define FM_REMIND     1// 来闹钟时，直接进入FM模式
//#define POPUP_REMIND  2// 来闹钟时，弹出特定的闹钟提醒窗口

////////////////////////////////////////////////////

typedef enum _RTC_STATE_
{
	RTC_STATE_IDLE = 0,
	RTC_STATE_SET_TIME,
	RTC_STATE_SET_ALARM,
	
} RTC_STATE;

typedef enum _RTC_SUB_STATE_
{
    RTC_SET_IDLE = 0,	// 空闲中
	RTC_SET_YEAR,    	// 设置年份
    RTC_SET_MON,     	// 设置月份
    RTC_SET_DAY,     	// 设置日期
	RTC_SET_WEEK,		// 设置周
    RTC_SET_HOUR,    	// 设置小时
    RTC_SET_MIN,     	// 设置分钟
    RTC_SET_SEC,     	// 设置秒钟
	RTC_SET_ALARM_NUM,
	RTC_SET_ALARM_MODE,
//    RTC_SET_MAX, 		//
	
} RTC_SUB_STATE;


typedef enum _RTC_WEEK_
{
    SUNDAY,  
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,

} RTC_WEEK;

typedef struct _RTC_CONTROL_
{
	uint8_t  		State;          // RTC当前状态,IDLE, Set Time, Set Alarm
	uint8_t  		SubState;       // RTC当前子状态,set year/mon/...
	
	RTC_DATE_TIME 	DataTime;  		// 时间日期	
	
	uint8_t	 		AlarmNum;		// 闹钟编号
	uint8_t	 		AlarmMode;		// 闹钟模式
	RTC_DATE_TIME 	AlarmTime; 		// 闹钟时间
	uint8_t  		AlarmData;		// 自定义闹钟模式下，闹钟时间选择，bit0：星期天，bit1：星期一，...,bit6：星期六,置位表示选择  
	uint8_t  		CurAlarmNum;	// 当前响应的闹钟编号
	
	RTC_LUNAR_DATE	LuarDate; 		// 农历

	SW_TIMER TimerHandle;
} RTC_CONTROL;


///////////////////////////////////////////////////////////
//掉电记忆相关，该部分代码仅仅保存于NVM，不会保存于Flash
//标志SDK未去实现闹钟响铃处理，该部分代码由客户自己添加完成
typedef struct _NVM_ALARM_INFO_
{	
	uint8_t RingType : 3; // 铃声类型 INTER_RING_TYPE - 内置铃声、USB_RING_TYPE - U盘铃声、SD_RING_TYPE - SD卡铃声
	uint8_t Duration : 2; // 闹铃持续时间 0 - 30s、1 - 1分钟、2 - 2分钟、3 - 3分钟
	uint8_t RepeatCnt: 3; // 重复次数
	uint32_t FileAddr;    // 文件扇区号
	uint8_t CRC8;         // 文件名校验码
	
} ALARM_RING_INFO;

// 默认 自动闹铃3次、闹铃间隔5分钟
typedef struct _NVM_RTC_INFO_
{
	uint8_t AlarmVolume;        // 闹钟铃声音量
	ALARM_RING_INFO AlarmRingList[MAX_ALARM_NUM]; // 闹钟闹铃信息，这部分由客户自行完成
	ALARM_TIME_INFO AlarmTimeList[MAX_ALARM_NUM];// 闹钟时间信息
	
} NVM_RTC_INFO;


#ifdef FUNC_BREAKPOINT_EN
#define RTC_NVM_START_ADDR		(sizeof(BP_INFO))
#else 
#define RTC_NVM_START_ADDR		0
#endif
///////////////////////////////////////////

bool RtcControl(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif



