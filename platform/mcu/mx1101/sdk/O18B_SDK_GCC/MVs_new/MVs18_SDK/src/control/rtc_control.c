///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: rtc_control.c
// maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "os.h"
#include "rtc_control.h"
#include "rtc.h"
#include "sys_app.h"
#include "recorder_control.h"
#include "dev_detect_driver.h"
#include "sound_remind.h"
#include "nvm.h"
#include "sys_vol.h"

#ifdef FUNC_RTC_EN
RTC_CONTROL* sRtcControl = NULL;
NVM_RTC_INFO sNvmRtcInfo; 
static TIMER RtcAutoOutTimer;

#define RTC_AUTO_OUT_TIME	5000

////////////////////////////////////
#ifdef FUNC_RTC_LUNAR
static void DisplayLunarDate(void)
{
	//农历年份名称
	const uint8_t LunarYearName[12][2] = {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"};
	
	//农历月份名称
	const uint8_t LunarMonthName[12][2] = {"正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "冬", "腊"};
	
	//农历日期名称
	const uint8_t LunarDateName[30][4] = {"初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十", 
									  "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
									  "廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"};
	
	//农历天干名称
	const uint8_t HeavenlyStemName[10][2] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
	
	//农历地支名称
	const uint8_t EarthlyBranchName[12][2] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
	
	RTC_LUNAR_DATE LunarDate;

	SolarToLunar(&sRtcControl->DataTime, &LunarDate);	
	APP_DBG("农历 %d年 ", (uint32_t)LunarDate.Year);
	APP_DBG("%-.2s%-.2s年 ", HeavenlyStemName[GetHeavenlyStem(LunarDate.Year)], 
						  EarthlyBranchName[GetEarthlyBranch(LunarDate.Year)]);
	APP_DBG("%-.2s年 ", LunarYearName[GetEarthlyBranch(LunarDate.Year)]);
	if(LunarDate.IsLeapMonth)
	{
		APP_DBG("闰");
	}
	APP_DBG("%-.2s月", LunarMonthName[LunarDate.Month - 1]);
	
	if(LunarDate.MonthDays == 29)
	{
		APP_DBG("(小)");
	}
	else
	{
		APP_DBG("(大)");
	}
	
	APP_DBG("%-.4s ", LunarDateName[LunarDate.Date - 1]);

	if((LunarDate.Month == 1) && (LunarDate.Date == 1))			//春节
	{
		APP_DBG("春节");
	}
	else if((LunarDate.Month == 1) && (LunarDate.Date == 15))	//元宵节
	{
		APP_DBG("元宵节");
	}
	else if((LunarDate.Month == 5) && (LunarDate.Date == 5))	//端午节
	{
		APP_DBG("端午节");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 7))	//七夕情人节
	{
		APP_DBG("七夕情人节");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 15))	//中元节
	{
		APP_DBG("中元节");
	}
	else if((LunarDate.Month == 8) && (LunarDate.Date == 15))	//中秋节
	{
		APP_DBG("中秋节");
	}
	else if((LunarDate.Month == 9) && (LunarDate.Date == 9))	//重阳节
	{
   		APP_DBG("重阳节");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 8))	//腊八节
	{
	 	APP_DBG("腊八节");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 23))	//小年
	{
		APP_DBG("小年");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == LunarDate.MonthDays))	//除夕
	{
		APP_DBG("除夕");
	}
}
#endif


void RtcTimerCB(uint32_t unused)
{
	RtcGetCurrTime(&sRtcControl->DataTime);
	
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		APP_DBG("RTC set Time....");
	}
	
	APP_DBG("RtcTime(%04d-%02d-%02d %02d:%02d:%02d) Week:%d ",
	        sRtcControl->DataTime.Year, sRtcControl->DataTime.Mon, sRtcControl->DataTime.Date,
	        sRtcControl->DataTime.Hour, sRtcControl->DataTime.Min, sRtcControl->DataTime.Sec,
	        sRtcControl->DataTime.WDay);
#ifdef FUNC_RTC_LUNAR
	DisplayLunarDate();
#endif
	APP_DBG("\n");
	
#ifdef FUNC_RTC_ALARM	
	if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{
		APP_DBG("闹钟号:%d ", sRtcControl->AlarmNum);
		switch(sRtcControl->AlarmMode)
		{
				case ALARM_MODE_ONCE_ONLY:
					APP_DBG("闹钟模式:单次(once only) ");
					APP_DBG("%04d-%02d-%02d (星期%02d) ", 
					sRtcControl->AlarmTime.Year, 
					sRtcControl->AlarmTime.Mon, 
					sRtcControl->AlarmTime.Date, 
					sRtcControl->AlarmTime.WDay);
					break;
	
				case ALARM_MODE_PER_DAY:
					APP_DBG("闹钟模式:每天一次(every day)");
					break;
	
				case ALARM_MODE_PER_WEEK:
					APP_DBG("闹钟模式:每周一次(every week) 星期%02d", sRtcControl->AlarmTime.WDay);
					break;
				case ALARM_MODE_WORKDAY:
					APP_DBG("闹钟模式:工作日(weekday)");
					break;
				case ALARM_MODE_USER_DEFINED:
					APP_DBG("闹钟模式:客户自定义 每周%02x", sRtcControl->AlarmData);
					break;
	
				default:
					APP_DBG(("模式错误(mode error)\n"));
					break;
		}	
			
		APP_DBG("AlarmTime(%02d:%02d:%02d) \n",
	        sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec);	
	}
#endif
}

// RTC初始化
bool RtcInitialize(void)
{
	APP_DBG("RtcInitialize\n");
#ifdef FUNC_RTC_ALARM
	NvmRead(RTC_NVM_START_ADDR, (uint8_t*)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
	RtcInit((void *)(sNvmRtcInfo.AlarmTimeList), sizeof(ALARM_TIME_INFO) * MAX_ALARM_NUM);//硬件初始化，不要删除
#else
	RtcInit(NULL, 0); // 该函数必须调用
#endif	
	if(sRtcControl != NULL)
	{
		APP_DBG("Init fail: RTC is running\n");
		return FALSE;
	}

	sRtcControl = APP_MMM_MALLOC(sizeof(RTC_CONTROL), MMM_ALLOC_NORMAL);
	ASSERT(sRtcControl != NULL);
	memset(sRtcControl, 0, sizeof(RTC_CONTROL));

	// 读取eprom或nvram中的：时间日期等信息
	RtcGetCurrTime(&sRtcControl->DataTime);
	
#ifdef FUNC_RTC_ALARM
	NVIC_EnableIRQ(RTC_IRQn);           // 打开闹钟提醒中断,必须在sRtcControl内存分配之后
#endif
	
	// 定时器
	InitTimer((SW_TIMER*)&sRtcControl->TimerHandle, 1000, (TIMER_CALLBACK)RtcTimerCB);
	StartTimer(&sRtcControl->TimerHandle);
#ifdef FUNC_RTC_ALARM	
	NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	return TRUE;
}

// 去初始化
bool RtcDeinitialize(void)
{
	APP_DBG("RtcDeinitialize\n");
	if(sRtcControl == NULL)
	{
		return FALSE;
	}

	DeinitTimer(&sRtcControl->TimerHandle);

	// 取消时间日期设置
	if(sRtcControl->State != RTC_STATE_IDLE)
	{

	}
	// 调用驱动接口，停止相关工作

	// 释放资源
	APP_MMM_FREE(sRtcControl);
	sRtcControl = NULL;

	return TRUE;
}


void RtcSubStateDbg(void)
{
	switch(sRtcControl->SubState)
	{
		case RTC_SET_YEAR:
			APP_DBG("设置“年”\n");
			break;
		case RTC_SET_MON:
			APP_DBG("设置“月”\n");
			break;
		case RTC_SET_DAY:
			APP_DBG("设置“日”\n");
			break;
#ifdef FUNC_RTC_ALARM
		case RTC_SET_WEEK:
			APP_DBG("设置“周”\n");
			break;
#endif
		case RTC_SET_HOUR:
			APP_DBG("设置“时”\n");
			break;
		case RTC_SET_MIN:
			APP_DBG("设置“分”\n");
			break;
		case RTC_SET_SEC:
			APP_DBG("不支持设置“秒”\n");
			break;
#ifdef FUNC_RTC_ALARM
		case RTC_SET_ALARM_NUM:
			APP_DBG("设置“闹钟编号”\n");
			break;
		case RTC_SET_ALARM_MODE:
			APP_DBG("设置“闹钟模式”\n");
			break;
#endif
		default:
			break;
	}
}

// 切换时间设置项
bool RtcSwitchNextSubState(void)
{
	switch(sRtcControl->State)
	{
		case RTC_STATE_IDLE:
			APP_DBG("Failed to switch focus\n");
			return FALSE;
		
		case RTC_STATE_SET_TIME:
			APP_DBG("Switch to next value\n");
			sRtcControl->SubState++;
			if(sRtcControl->SubState > RTC_SET_SEC)
			{
				sRtcControl->SubState = RTC_SET_YEAR;
			}
			else if(sRtcControl->SubState == RTC_SET_WEEK)//时间设置不支持周时间设置
			{
				sRtcControl->SubState++;
			}
			APP_DBG("sRtcControl->SubState:%d\n", sRtcControl->SubState);
			break;
			
#ifdef FUNC_RTC_ALARM	
		case RTC_STATE_SET_ALARM:
			APP_DBG("Switch to next value\n");
			sRtcControl->SubState++;					
			if(sRtcControl->SubState > RTC_SET_ALARM_MODE)
			{
				sRtcControl->SubState = RTC_SET_YEAR;
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_ONCE_ONLY)
			{
				if(sRtcControl->SubState == RTC_SET_WEEK)//单次闹钟不支持周时间设置
				{
					sRtcControl->SubState++;
				}
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_PER_DAY)//每天闹钟不支持年月日周设置
			{
				if(sRtcControl->SubState < RTC_SET_HOUR)
				{
					sRtcControl->SubState = RTC_SET_HOUR;
				}
			}
			else//周闹钟不支持年月日设置
			{
				if(sRtcControl->SubState < RTC_SET_WEEK)
				{
					sRtcControl->SubState = RTC_SET_WEEK;
				}
			}
			
			APP_DBG("sRtcControl->SubState:%d\n", sRtcControl->SubState);
			break;
#endif
			
		default:
			break;
	}
	
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
	return TRUE;
}

void RtcControlSetTime(void)
{
	if(sRtcControl->State == RTC_STATE_IDLE)
	{
		APP_DBG("Set Rtc Time Mode\n");
		TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
		sRtcControl->State = RTC_STATE_SET_TIME;
		sRtcControl->SubState = RTC_SET_YEAR;
		RtcSubStateDbg();
	}
	else if(sRtcControl->State == RTC_STATE_SET_TIME)
	{
		RtcSwitchNextSubState();
		RtcSubStateDbg();
	}
}

#ifdef FUNC_RTC_ALARM
void RtcControlSetAlarm(void)
{
	if(sRtcControl->State == RTC_STATE_IDLE)
	{
		APP_DBG("Set Alarm Time Mode\n");
		if((sRtcControl->AlarmNum == 0) || (sRtcControl->AlarmNum > MAX_ALARM_NUM))
		{
			sRtcControl->AlarmNum = 1;
		}
		RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		APP_DBG("Alarm Time Mode: %d\n", sRtcControl->AlarmMode);
		if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_MAX))
		{
			sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
		}
		
		TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
		sRtcControl->State = RTC_STATE_SET_ALARM;
		sRtcControl->SubState = RTC_SET_ALARM_NUM;
		RtcSubStateDbg();
	}
	else if(sRtcControl->State == RTC_STATE_SET_ALARM)
	{
		RtcSwitchNextSubState();
		RtcSubStateDbg();
	}
}
#endif

void RtcTimeUp(RTC_DATE_TIME *Time)
{
	switch(sRtcControl->SubState)
	{
		case RTC_SET_YEAR:
			Time->Year++;
			if(Time->Year > 2099)
			{
				Time->Year = 1980;	
			}
			break;
		case RTC_SET_MON:
			Time->Mon++;
			if(Time->Mon > 12)
			{
				Time->Mon = 1;
			}
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_WEEK:
			Time->WDay++;
			if(Time->WDay > 6)
			{
				Time->WDay = 0;	
			}
			break;
		case RTC_SET_DAY:	
			Time->Date++;
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = 1;
			}
			break;
		case RTC_SET_HOUR:
			Time->Hour++;
			Time->Sec = 0;
			if(Time->Hour > 23)
			{
				Time->Hour = 0;
			}
			break;
		case RTC_SET_MIN:
			Time->Min++;
			Time->Sec = 0;
			if(Time->Min > 59)
			{
				Time->Min = 0;
			}
			break;
		default:
			break;
	}
}

void RtcTimeDown(RTC_DATE_TIME *Time)
{
	switch(sRtcControl->SubState)
	{	
		case RTC_SET_YEAR:
			Time->Year--;
			if(Time->Year < 1980)
			{
				Time->Year = 2099;	
			}
			break;
		case RTC_SET_MON:
			Time->Mon--;
			if(Time->Mon == 0)
			{
				Time->Mon = 12;
			}
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_WEEK:
			Time->WDay--;			
			if(Time->WDay > 6)
			{
				Time->WDay = 6;	
			}
			break;
		case RTC_SET_DAY:	
			Time->Date--;
			if(Time->Date == 0)
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_HOUR:
			Time->Hour--;
			Time->Sec = 0;
			if(Time->Hour > 23)
			{
				Time->Hour = 23;
			}
			break;
		case RTC_SET_MIN:
			Time->Min--;
			Time->Sec = 0;
			if(Time->Min > 59)
			{
				Time->Min = 59;
			}
			break;
		default:
			break;
	}
}

// 系统时间当前设置项值的增
void RtcSetTimeUp(void)
{
	APP_DBG("RtcSetValueUp(state = %d, substate = %d)\n", sRtcControl->State, sRtcControl->SubState);
	if(RTC_STATE_IDLE == sRtcControl->State)
	{
		return;
	}
	
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		RtcTimeUp(&sRtcControl->DataTime);
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
#ifdef FUNC_RTC_ALARM
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{
		if(RTC_SET_ALARM_NUM == sRtcControl->SubState)
		{
			sRtcControl->AlarmNum++;
			if(sRtcControl->AlarmNum > MAX_ALARM_NUM)
			{
				sRtcControl->AlarmNum = 1;
			}
			RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		}
		else if(RTC_SET_ALARM_MODE == sRtcControl->SubState)
		{
			sRtcControl->AlarmMode++;
			if(sRtcControl->AlarmMode >= ALARM_MODE_MAX)
			{
				sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmData = 0x4A; // 客户可以自己修改
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
			{
				sRtcControl->AlarmData = 0x3E;
			}
			else
			{
				sRtcControl->AlarmData = 0x0;
			}
		}
		else
		{
			RtcTimeUp(&sRtcControl->AlarmTime);
		}
		APP_DBG("+++sRtcControl->ModeData;%x\n", sRtcControl->AlarmData);
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
}

// 系统时间当前设置项值的减
void RtcSetTimeDown(void)
{
	APP_DBG("RtcSetValueDown(state = %d, substate = %d)\n", sRtcControl->State, sRtcControl->SubState);

	if(RTC_STATE_IDLE == sRtcControl->State)
	{
		return;
	}
	
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		RtcTimeDown(&sRtcControl->DataTime);
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
#ifdef FUNC_RTC_ALARM
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{	
		if(RTC_SET_ALARM_NUM == sRtcControl->SubState)
		{
			sRtcControl->AlarmNum--;
			if((sRtcControl->AlarmNum == 0) || (sRtcControl->AlarmNum > MAX_ALARM_NUM))
			{
				sRtcControl->AlarmNum = MAX_ALARM_NUM;
			}
			RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		}
		else if(RTC_SET_ALARM_MODE == sRtcControl->SubState)
		{
			sRtcControl->AlarmMode--;
			if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_MAX))
			{
				sRtcControl->AlarmMode = (ALARM_MODE_MAX - 1);
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmData = 0x4A; // 客户可以自己修改
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
			{
				sRtcControl->AlarmData = 0x3E;
			}
			else
			{
				sRtcControl->AlarmData = 0x0;
			}
		}
		else
		{
			RtcTimeDown(&sRtcControl->AlarmTime);
		}
		APP_DBG("---sRtcControl->ModeData;%x\n", sRtcControl->AlarmData);
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
}

bool RtcControl(void)
{
	uint16_t Msg  = 0;

	APP_DBG("Enter RTC\n");
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
    
	//初始化
	if(!RtcInitialize())
	{
		return FALSE;
	}
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		CheckTimer(&sRtcControl->TimerHandle);

		Msg = MsgRecv(20);
#ifdef FUNC_RTC_ALARM	
		if(sRtcControl->State == RTC_STATE_IDLE)
		{
			if(sRtcControl->CurAlarmNum)
			{
				APP_DBG("RTC ALARM(%d) COME!\n", sRtcControl->CurAlarmNum);
				RtcAlarmArrivedProcess();
				//响玲处理
				sRtcControl->CurAlarmNum = 0;
#ifdef FUNC_RTC_ALARM				
				NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
			}
		}
#endif
		switch(Msg)
		{
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;
			
			case MSG_MODE:
				APP_DBG("Exit RTC\n");
				Msg = MSG_COMMON_CLOSE;
				break;

			case MSG_RTC_SET_TIME:
				RtcControlSetTime();
				break;
			
#ifdef FUNC_RTC_ALARM			
			case MSG_RTC_SET_ALARM:
				RtcControlSetAlarm();
				break;
#endif						
				// 设置项的值增加
			case MSG_VOL_UP:
				RtcSetTimeUp();
				break;
			
				// 设置项的值减少
			case MSG_VOL_DW:
				RtcSetTimeDown();
				break;
			
#ifdef FUNC_RECORD_EN
			case MSG_REC:  //录音
				gSys.ModuleIDBackUp = gSys.CurModuleID;
				gSys.NextModuleID = MODULE_ID_RECORDER;
				gSys.RecordState = RECORD_STATE_REC;
				gSys.RecordSource = RECORD_SOURCE_LINEIN;
				Msg = MSG_COMMON_CLOSE;
				break;
#endif
           
#ifdef FUNC_REC_PLAYBACK_EN
			case MSG_REC_PLAYBACK:
				EnterRecBackPlay(gSys.CurModuleID, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0, 0);
				break;
#endif			

			default:
				break;
		}

		if((sRtcControl->State != RTC_STATE_IDLE) && (IsTimeOut(&RtcAutoOutTimer)))
		{
			APP_DBG("Rtc Normer Mode\n");
			sRtcControl->State = RTC_STATE_IDLE;
		}
	}
	RtcDeinitialize();//模式切换，退出RTC
	return TRUE;
}


#ifdef FUNC_RTC_ALARM
__attribute__((section(".driver.isr"), weak)) void RtcInterrupt(void)
{
	sRtcControl->CurAlarmNum = RtcCheckAlarmFlag();
	RtcAlarmIntClear();// 清除闹钟提醒中断
}
#endif

#endif
