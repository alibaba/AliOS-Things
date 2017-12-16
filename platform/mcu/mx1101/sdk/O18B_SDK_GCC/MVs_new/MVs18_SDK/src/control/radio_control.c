///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: radio_control.c
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "clk.h"
#include "radio_control.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "breakpoint.h"
#include "recorder_control.h"
#include "sys_app.h"
#include "nvm.h"
#include "dev_detect_driver.h"
#include "eq_params.h"
#include "gpio.h"
#include "mixer.h"
#include "bt_app_func.h"

#ifdef FUNC_RADIO_EN

void RadioSeekStart(void);
void RadioSeekEnd(void);
bool RadioChannelAdd(void);
static uint16_t RadioGetBaseFreq(void);

RADIO_CONTROL* sRadioControl = NULL;// 电台列表增减
RADIO_NAME 	Radio_Name = RADIO_NONE;// 区分FM的型号

// 根据当前频段，获取基准频率
static uint16_t RadioGetBaseFreq(void)
{
	switch(sRadioControl->CurFreqArea)
	{
		case FM_AREA_CHINA:
			return CHINA_BASE_FREQ;
		case FM_AREA_JAPAN:
			return JAPAN_BASE_FREQ;
		case FM_AREA_RUSSIA:
			return RUSSIA_BASE_FREQ;
		default:
			return OTHER_BASE_FREQ;
	}
}

#ifdef FUNC_BREAKPOINT_EN
void RadioGetBreakPoint(void)
{	
	BP_RADIO_INFO *pBpRadioInfo;
	BP_SYS_INFO *pBpSysInfo;
		
	pBpRadioInfo = (BP_RADIO_INFO *)BP_GetInfo(BP_RADIO_INFO_TYPE);
	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);

	gSys.Volume = BP_GET_ELEMENT(pBpSysInfo->Volume);
    gSys.Eq = BP_GET_ELEMENT(pBpSysInfo->Eq);
	sRadioControl->CurFreqArea = BP_GET_ELEMENT(pBpRadioInfo->CurBandIdx);
	sRadioControl->Freq = BP_GET_ELEMENT(pBpRadioInfo->CurFreq);
	sRadioControl->ChlCount = BP_GET_ELEMENT(pBpRadioInfo->StationCount);
	if(sRadioControl->ChlCount > 0)
	{
		uint32_t i;
		for(i = 0; i < sRadioControl->ChlCount; i++)
		{
			sRadioControl->Channel[i] = (uint16_t)(BP_GET_ELEMENT(pBpRadioInfo->StationList[i]));
			if((sRadioControl->Channel[i] + RadioGetBaseFreq()) == sRadioControl->Freq)
			{
				sRadioControl->CurStaIdx = (uint8_t)i;
			}
		}
	}
}

void RadioSaveBreakPoint(void)
{
	uint32_t i;

	BP_RADIO_INFO *pBpRadioInfo;
	BP_SYS_INFO *pBpSysInfo;
		
	pBpRadioInfo = (BP_RADIO_INFO *)BP_GetInfo(BP_RADIO_INFO_TYPE);
	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
	
	BP_SET_ELEMENT(pBpSysInfo->Volume, gSys.Volume);
    BP_SET_ELEMENT(pBpSysInfo->Eq, gSys.Eq);
	BP_SET_ELEMENT(pBpRadioInfo->CurBandIdx, sRadioControl->CurFreqArea<<6);
	BP_SET_ELEMENT(pBpRadioInfo->CurFreq, sRadioControl->Freq);
	BP_SET_ELEMENT(pBpRadioInfo->StationCount, sRadioControl->ChlCount);

	if(sRadioControl->ChlCount > 0)
	{	
		for(i = 0; i < sRadioControl->ChlCount; i++)
		{
			BP_SET_ELEMENT(pBpRadioInfo->StationList[i], sRadioControl->Channel[i]);
		}
	}
	
	BP_SaveInfo(BP_SAVE2NVM);

#ifdef BP_SAVE_TO_FLASH	
	BP_SaveInfo(BP_SAVE2FLASH);	
#endif
}
#endif

void RadioTimerCB(void* unused)
{
	switch(sRadioControl->State)
	{
		case RADIO_STATUS_PLAYING:
			break;
 
		case RADIO_STATUS_SEEKING:
			if(RadioRSSIRead(sRadioControl->Freq) && (sRadioControl->Freq < sRadioControl->MaxFreq))// 判断搜到新电台
			{
				if(RadioChannelAdd())
				{
					APP_DBG("Found new chl %d and start preview\n", sRadioControl->Freq);
					sRadioControl->State = RADIO_STATUS_PREVIEW;// 进入电台预览状态
					RadioSetFreq(sRadioControl->Freq);// 调用驱动层，预览电台
					RadioMute(FALSE);
					ChangeTimerPeriod(&sRadioControl->TimerHandle, MIN_TIMER_PERIOD);// 电台预览时间设置
				}
				else// 电台列表满时，停止自动搜台
				{
					RadioSeekEnd();
					MsgClear(MSG_PLAY_PAUSE);// 清除多余的搜台消息
				}
			}
			else
			{
				if(sRadioControl->Freq >= sRadioControl->MaxFreq)// 到达最大频率，停止搜台
				{
					RadioSeekEnd();
					MsgClear(MSG_PLAY_PAUSE);// 清除多余的搜台消息
					break;
				}
				else// 继续下一轮的搜台
				{
					sRadioControl->Freq++;
					RadioSeekStart();
				}
			}
			break;
			
		case RADIO_STATUS_PREVIEW: // 电台预览时间控制
			{
				static uint32_t LastTime = 0;
				if(LastTime == 0)
				{
					LastTime = TICKS_TO_MSECS(OSSysTickGet()) - MIN_TIMER_PERIOD;
				}

				if((TICKS_TO_MSECS(OSSysTickGet()) - LastTime) >= FM_PERVIEW_TIMEOUT)
				{
					APP_DBG("Stop preview\n");
					RadioMute(TRUE);
					sRadioControl->State = RADIO_STATUS_SEEKING;
					if(sRadioControl->Freq < sRadioControl->MaxFreq)
					{
						sRadioControl->Freq++;
						RadioSeekStart();
					}
					else
					{
						RadioSeekEnd();
						MsgClear(MSG_PLAY_PAUSE);	// 清除多余的搜台消息
					}
					LastTime = 0;
				}
			}
			break;
			
		case RADIO_STATUS_IDLE:
		default:
			break;
	}
}


#ifdef RADIO_DELAY_SWITCH_CHANNEL
void RadioDelaySwithChlCB(void* unused)
{
	StopTimer((SW_TIMER*)unused);
	
	// 播放新的频道
	sRadioControl->Freq = sRadioControl->Channel[sRadioControl->CurStaIdx] + RadioGetBaseFreq();
	APP_DBG("Play Chl %d\n", sRadioControl->Freq);
	RadioMute(TRUE);
	
	if(sRadioControl->Freq/1000)
	{
		SoundRemind(SOUND_NUM0 + sRadioControl->Freq / 1000);  	
	}
	SoundRemind(SOUND_NUM0 + ((sRadioControl->Freq % 1000) / 100));  
	SoundRemind(SOUND_NUM0 + ((sRadioControl->Freq % 100) / 10));  
	SoundRemind(SOUND_POINT);
	SoundRemind(SOUND_NUM0 + (sRadioControl->Freq % 10)); 

	RadioSetFreq(sRadioControl->Freq);
	RadioMute(FALSE);
	sRadioControl->State = RADIO_STATUS_PLAYING;
	
#ifdef FUNC_BREAKPOINT_EN
	RadioSaveBreakPoint();
#endif	
}
#endif

void RadioNumKeyTimerCB(void* Param)
{
	StopTimer(&sRadioControl->NumKeyTimer);

	if(sRadioControl->InputNumber <= sRadioControl->ChlCount)
	{
		sRadioControl->CurStaIdx = (sRadioControl->InputNumber - 1);
		sRadioControl->InputNumber = 0;
		// 播放新的频道
		sRadioControl->Freq = sRadioControl->Channel[sRadioControl->CurStaIdx] + RadioGetBaseFreq();
		APP_DBG("Play Chl %d\n", sRadioControl->Freq);
		RadioMute(TRUE);
		RadioSetFreq(sRadioControl->Freq);
		RadioMute(FALSE);
		sRadioControl->State = RADIO_STATUS_PLAYING;
		
#ifdef FUNC_BREAKPOINT_EN
		RadioSaveBreakPoint();
#endif
	}
	else if((sRadioControl->InputNumber >= sRadioControl->MinFreq) && (sRadioControl->InputNumber <= sRadioControl->MaxFreq))
	{
		sRadioControl->Freq = sRadioControl->InputNumber;
		sRadioControl->InputNumber = 0;
		APP_DBG("Play Chl %d\n", sRadioControl->Freq);
		RadioMute(TRUE);
		RadioSetFreq(sRadioControl->Freq);
		RadioMute(FALSE);
		sRadioControl->State = RADIO_STATUS_PLAYING;

#ifdef FUNC_BREAKPOINT_EN
		RadioSaveBreakPoint();
#endif		
	}
	else
	{
		sRadioControl->InputNumber = 0;
	}
}

// 开始自动搜台
void RadioSeekStart(void)
{
	APP_DBG("Seeking...\n");
	
	RadioMute(TRUE);
	StopTimer(&sRadioControl->TimerHandle);
	if(!(RADIO_STATUS_SEEKING == sRadioControl->State || RADIO_STATUS_PREVIEW == sRadioControl->State))
	{
		sRadioControl->Freq = sRadioControl->MinFreq;
		sRadioControl->State = RADIO_STATUS_SEEKING;
		sRadioControl->ChlCount = 0;
		sRadioControl->CurStaIdx = 0;
		memset(sRadioControl->Channel, 0, sizeof(sRadioControl->Channel));

		// 调用驱动接口，开始搜台
		if(!IsRadioWorking())
		{
			// 初始化FM设备
			if(!RadioPowerOn() || !RadioInit())
			{
				RadioSeekEnd();
				MsgClear(MSG_PLAY_PAUSE);	// 清除多余的搜台消息
				return;
			}
		}
	}
	RadioSearchSet(sRadioControl->Freq);

	// 50ms 搜台超时时间
	ChangeTimerPeriod(&sRadioControl->TimerHandle, FM_CHL_SEARCH_TIME_UNIT/*MIN_TIMER_PERIOD*/);
	StartTimer(&sRadioControl->TimerHandle);
}


// 停止自动搜台
void RadioSeekEnd(void)
{
	APP_DBG("End Seek\n");
	
	StopTimer(&sRadioControl->TimerHandle);
	if(sRadioControl->ChlCount > 0)
	{
		sRadioControl->Freq = sRadioControl->Channel[sRadioControl->CurStaIdx] + RadioGetBaseFreq();
	}
	else
	{
		sRadioControl->Freq = sRadioControl->MinFreq;
	}

	// 这里清除残留的MSG_PLAY_PAUSE, MSG_STOP消息
	MsgClear(MSG_PLAY_PAUSE);
	MsgClear(MSG_STOP);
	RadioPlay(); //搜台停止后播放第一个电台
}

// 电台列表增加
bool RadioChannelAdd(void)
{
	APP_DBG("RadioChannelAdd\n");
	if(sRadioControl->ChlCount == MAX_RADIO_CHANNEL_NUM)
	{
		APP_DBG("Fail to add new chl, Channel list is full\n");
		return FALSE;
	}
	sRadioControl->Channel[sRadioControl->ChlCount++] = sRadioControl->Freq - RadioGetBaseFreq();
	
#ifdef FUNC_BREAKPOINT_EN
	RadioSaveBreakPoint();
#endif

	return TRUE;
}

// FM Radio初始化
bool RadioInitialize(void)
{
	APP_DBG("RadioInitialize\n");	
	if(sRadioControl != NULL)
	{
		APP_DBG("Init fail: Fm radio is running\n");
		return FALSE;
	}

	sRadioControl = APP_MMM_MALLOC(sizeof(RADIO_CONTROL), MMM_ALLOC_NORMAL);

	ASSERT(sRadioControl != NULL);
	memset(sRadioControl, 0, sizeof(RADIO_CONTROL));
	sRadioControl->State = RADIO_STATUS_IDLE;

#ifdef FUNC_BREAKPOINT_EN
	RadioGetBreakPoint(); //获取系统记忆信息
#endif

	switch(sRadioControl->CurFreqArea)
	{
		case FM_AREA_CHINA:
			sRadioControl->MinFreq = RADIO_DEF_MIN_FREQ_VALUE;
			sRadioControl->MaxFreq = RADIO_DEF_MAX_FREQ_VALUE;
			break;
		case FM_AREA_JAPAN:
			sRadioControl->MinFreq = RADIO_JAP_MIN_FREQ_VALUE;
			sRadioControl->MaxFreq = RADIO_JAP_MAX_FREQ_VALUE;
			break;
		case FM_AREA_RUSSIA:
			sRadioControl->MinFreq = RADIO_RUS_MIN_FREQ_VALUE;
			sRadioControl->MaxFreq = RADIO_RUS_MAX_FREQ_VALUE;
			break;
		default:
			sRadioControl->MinFreq = RADIO_OTH_MIN_FREQ_VALUE;
			sRadioControl->MaxFreq = RADIO_OTH_MAX_FREQ_VALUE;
			break;
	}
	
	//GPIOB30 输出32K频率方波,如果模组已经自带晶体，可以不使去掉该函数调用
	GpioClk32kIoConfig(1);
	if(!RadioPowerOn())
	{
		APP_DBG("RadioPowerOn Fail!\n");
		RadioDeinitialize();
		return FALSE;
	}
	// 初始化FM设备
	if(!RadioInit())
	{
		APP_DBG("RadioInit Fail!\n");
		RadioDeinitialize();
		return FALSE;
	}
	
	if(!InitTimer(&sRadioControl->TimerHandle, 1000, RadioTimerCB))
	{
		APP_DBG("Init fail: create timer error\n");
		RadioDeinitialize();
		return FALSE;
	}
	
	//AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_FM : AUDIO_CH_FM);

	if(sRadioControl->ChlCount == 0)
	{
		// 当前如果没有电台，则开始自动搜台
		RadioSeekStart();
	}
	else
	{
		// 播放断点信息中指定的电台
		if(sRadioControl->Freq < 875)
		{
			sRadioControl->Freq = 875;
		}
		else if(sRadioControl->Freq > 1080)
		{
			sRadioControl->Freq = 1080;
		}
		RadioMute(TRUE);
		RadioSetFreq(sRadioControl->Freq);
		RadioMute(FALSE);
	}
	
	return TRUE;
}

// FM Radio去初始化
bool RadioDeinitialize(void)
{
	APP_DBG("RadioDeinitialize\n");
	if(sRadioControl == NULL)
	{
		return FALSE;
	}
	
#ifdef FUNC_RADIO_DIV_CLK_EN
	ClkModuleEn(ALL_MODULE_CLK_SWITCH);
#endif
	
	if(gSys.MicEnable)
	{
		MixerMute(MIXER_SOURCE_ANA_MONO);
		MixerMute(MIXER_SOURCE_MIC);
	}
	else
	{
		MixerMute(MIXER_SOURCE_ANA_STERO);
	}
	WaitMs(40);
	
#ifdef FUNC_RECORD_EN
	if(gSys.NextModuleID != MODULE_ID_RECORDER)
#endif
	{
		RadioMute(TRUE);
	}
	
	// 关闭FM设备
	if(IsRadioWorking())
	{
		RadioPowerOff();
	}
	// 停止搜台
	if(sRadioControl->State == RADIO_STATUS_SEEKING)
	{
		StopTimer(&sRadioControl->TimerHandle);
		// 这里清除残留的MSG_PLAY_PAUSE, MSG_STOP消息
		MsgClear(MSG_PLAY_PAUSE);
		MsgClear(MSG_STOP);
		//RadioSeekEnd();	
	}

	DeinitTimer(&sRadioControl->TimerHandle);
#ifdef RADIO_DELAY_SWITCH_CHANNEL
	DeinitTimer(&sRadioControl->DelayDoTimer);
#endif

#ifdef FUNC_BREAKPOINT_EN
	RadioSaveBreakPoint();
#endif

	//GpioClk32kIoConfig(0xFF);//restore the pins used for Clk32 output to general IO
	// 释放资源
	APP_MMM_FREE(sRadioControl);
	sRadioControl = NULL;
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);
	
	if(gSys.MicEnable)
	{
		MixerUnmute(MIXER_SOURCE_ANA_MONO);
		MixerUnmute(MIXER_SOURCE_MIC);
	}
	else
	{
		MixerUnmute(MIXER_SOURCE_ANA_STERO);
	}
	
	return TRUE;
}

// 播放
void RadioPlay(void)
{
	if(sRadioControl->State == RADIO_STATUS_SEEKING || sRadioControl->State == RADIO_STATUS_PREVIEW)
	{
		RadioMute(TRUE);
		sRadioControl->State = RADIO_STATUS_IDLE;
	}
	
	APP_DBG("Play Chl %d\n", sRadioControl->Freq);
	// 调用驱动接口，播放电台
	if(!IsRadioWorking())
	{
		RadioPowerOn();
		RadioInit();
	}
	RadioMute(TRUE);
	RadioSetFreq(sRadioControl->Freq);
	RadioMute(FALSE);
	sRadioControl->State = RADIO_STATUS_PLAYING;
	ChangeTimerPeriod(&sRadioControl->TimerHandle, 1000);
	StartTimer(&sRadioControl->TimerHandle);
}

// 电台切换（延迟300ms(时间可调)执行，对连续按键进行累加合并）
// 【规则】
//   1. 按MSG_RADIO_DOWN键 切换电台数递增，执行延迟
//   2. 按MSG_RADIO_UP键   切换电台数递减，执行延迟
//   3. 连续的切换电台有时间限制，超时后合并成一个命令，后续的连续按键在下一次再合并
// 例子（假如终端用户按键盘足够快）:
//   1. 连续按3次向前切电台：                CurStaIdx += 3，       切换操作只执行一次
//   2. 连续按两次向后切换电台：             CurStaIdx -= 2，       切换操作只执行一次
//   3. 连续按2次向前切换，再按1次向后切换： CurStaIdx = 2 - 1 = 1，切换操作只执行一次
//   4. 如果连续按键时间600ms，前300ms按了3次前切、后切，后300ms又按了3次，
//      则按300ms的超时时间，操作合并两次，分两次执行
bool RadioSwitchChannel(uint32_t Msg)
{
	if(sRadioControl->State == RADIO_STATUS_SEEKING)
	{
		APP_DBG("Radio is seeking\n");
		return FALSE;
	}
	if(sRadioControl->ChlCount == 0)
	{
		APP_DBG("Failed to switchchannel: channel list is empty\n");
		return FALSE;
	}

	if(Msg == MSG_NEXT)
	{
		APP_DBG("Next channel\n");
		sRadioControl->CurStaIdx++;
		sRadioControl->CurStaIdx %= sRadioControl->ChlCount;
	}
	else
	{
		APP_DBG("Pre channel\n");
		if(sRadioControl->CurStaIdx > 0)
		{
			sRadioControl->CurStaIdx--;
		}
		else
		{
			sRadioControl->CurStaIdx = (uint8_t)(sRadioControl->ChlCount - 1);
		}
	}

#ifdef RADIO_DELAY_SWITCH_CHANNEL
	if(!sRadioControl->DelayDoTimer.IsRunning)
	{
		// Timer超时时间不会随着有新按键的到来而重置(避免持续的延迟超时时间)，
		// 这样可以限制连续按键合并个数, 改善及时响应速度
		InitTimer(&sRadioControl->DelayDoTimer, 300, RadioDelaySwithChlCB);
		StartTimer(&sRadioControl->DelayDoTimer);
	}	
#else
	RadioPlay();
#endif

#ifdef FUNC_BREAKPOINT_EN
	RadioSaveBreakPoint();
#endif

	return TRUE;
}

void RadioLedCb(void* Param)
{
	static uint8_t Flag = 0;

	if(sRadioControl)
	{
		if(sRadioControl->State == RADIO_STATUS_SEEKING)
		{
			//SetBtLedRed(Flag);
			Flag = !Flag;
			return;
		}
	}
	
	//SetBtLedRed(1);
}

bool RadioControl(void)
{
	uint16_t   Msg  = 0;
	static SW_TIMER RadioTimer;

	APP_DBG("Enter fm radio\n");
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
	
	//初始化。如是没有台，则开始搜索：RadioState = RADIO_STATUS_SEEKING;
	if(!RadioInitialize())
	{
		APP_DBG("Exit fm radio\n");
		return FALSE;
	}
	SoundRemind(SOUND_RADIO_MODE);
	
	InitTimer(&RadioTimer, 250, RadioLedCb);
	StartTimer(&RadioTimer);
	
	if(gSys.MicEnable)
	{
		MixerMute(MIXER_SOURCE_ANA_MONO);
		MixerMute(MIXER_SOURCE_MIC);
	}
	else
	{
		MixerMute(MIXER_SOURCE_ANA_STERO);
	}
	WaitMs(20);
//	RadioMute(TRUE);
	
#ifdef FUNC_TREB_BASS_EN
	if(gSys.EqStatus == 0)
	{
		TrebBassSet(gSys.TrebVal, gSys.BassVal);
	}
#endif
	
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_FM : AUDIO_CH_FM);
	
    if(gSys.MuteFlag)
	{
    	gSys.MuteFlag = 0;
		AudioPlayerMute(gSys.MuteFlag);
	}  
	
//	RadioMute(FALSE);
	if(gSys.MicEnable)
	{
		MixerUnmute(MIXER_SOURCE_ANA_MONO);
		MixerUnmute(MIXER_SOURCE_MIC);
	}
	else
	{
		MixerUnmute(MIXER_SOURCE_ANA_STERO);
	}

	
#ifdef FUNC_BREAKPOINT_EN
	AudioSysInfoSetBreakPoint();
#endif

	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	
	while(Msg != MSG_COMMON_CLOSE)
	{
		CheckTimer(&sRadioControl->TimerHandle);
#ifdef RADIO_DELAY_SWITCH_CHANNEL
		CheckTimer(&sRadioControl->DelayDoTimer);
#endif
		CheckTimer(&RadioTimer);
		CheckTimer(&sRadioControl->NumKeyTimer);


		Msg = MsgRecv(5);
		switch(Msg)
		{			
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
			case MSG_MODE:
				APP_DBG("Exit fm radio\n");
				Msg = MSG_COMMON_CLOSE;
				break;
			
			// 搜台，停止
			case MSG_PLAY_PAUSE:
				if(sRadioControl->State == RADIO_STATUS_SEEKING)
				{
					RadioSeekEnd();
				}
				else
				{
					SoundRemind(SOUND_AUTO_SCAN);  	
					RadioSeekStart();
				}
				break;
				
			// 上下 电台切换
			case MSG_PRE:
			case MSG_NEXT:
				RadioSwitchChannel(Msg);
				break;

#ifdef FUNC_RECORD_EN
			case MSG_REC:
				if(sRadioControl->State == RADIO_STATUS_SEEKING)
				{
					break;
				}	
				gSys.ModuleIDBackUp = gSys.CurModuleID;
				gSys.NextModuleID = MODULE_ID_RECORDER;
				gSys.RecordState = RECORD_STATE_REC;
				gSys.RecordSource = RECORD_SOURCE_FM;
				APP_DBG("Exit fm radio\n");
				Msg = MSG_COMMON_CLOSE;
				break;

#ifdef FUNC_REC_PLAYBACK_EN
			case MSG_REC_PLAYBACK:
				EnterRecBackPlay(gSys.CurModuleID, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0, 0);
				break;
#endif
#endif

			case MSG_NUM_0:
			case MSG_NUM_1:
			case MSG_NUM_2:
			case MSG_NUM_3:
			case MSG_NUM_4:
			case MSG_NUM_5:
			case MSG_NUM_6:
			case MSG_NUM_7:
			case MSG_NUM_8:
			case MSG_NUM_9:
				if(!sRadioControl->NumKeyTimer.IsRunning)
				{
					InitTimer(&sRadioControl->NumKeyTimer, 1000, RadioNumKeyTimerCB);
				}
				else
				{
					ResetTimer(&sRadioControl->NumKeyTimer);
				}
				StartTimer(&sRadioControl->NumKeyTimer);

				sRadioControl->InputNumber = (sRadioControl->InputNumber * 10) + (Msg - MSG_NUM_0);
				
				SoundRemind(SOUND_NUM0 + (Msg - MSG_NUM_0));  	
				break;
				
			default:
				CommonMsgProccess(Msg);
				break;
		}
	}
	RadioDeinitialize();
	return TRUE;
}

#endif

