/**
 *******************************************************************************
 * @file    sys_app.c
 * @author  Richard
 * @version V1.0.0
 * @date    17-12-2013
 * @brief   system application
 * @maintainer: Sam
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "type.h"
#include "app_config.h"
#include "msgq.h"
#include "app_msg.h"
#include "uart.h"
#include "cache.h"
#include "gpio.h"
#include "wakeup.h"
#include "clk.h"
#include "timer.h"
#include "sys_app.h"
#include "ir.h"
#include "nvm.h"
#include "sound_remind.h"
#include "breakpoint.h"
#include "fsinfo.h"


SYS_INFO gSys;


//全局变量的初始化，开机时调用
void SysVarInit(void)
{
	memset(&gSys, 0, sizeof(gSys));

#ifdef FUNC_RADIO_DIV_CLK_EN
	//system clock frequency divide into 4
	gSys.FmSysClkDivFactor = 2;
	gSys.IsSysClk96MHz = 1;
#endif
		
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
	gSys.AudioDRCEffectFlag = 0;
#endif	
	
#ifdef FUNC_AUDIO_3D_EFFECT_EN
	gSys.Audio3dEffectFlag = 0;
#endif
    
#ifdef FUNC_AUDIO_VB_EFFECT_EN
    gSys.AudioVbEffectFlag = 0;
#endif	
	
#ifdef FUNC_TREB_BASS_EN
	gSys.BassVal = DEFAULT_BASS_VAL;
	gSys.TrebVal = DEFAULT_TREB_VAL;
	gSys.EqStatus = 1; // set default effect to EQ
#endif
	
	gSys.AudioSetMode = AUDIO_SET_MODE_MAIN_VOL;
	gSys.Volume = DEFAULT_VOLUME;
#ifdef FUNC_MIC_EN
	gSys.MicVolume = DEFAULT_MICIN_VOLUME;
#endif

#ifdef FUNC_BT_HF_EN
	gSys.HfVolume = MAX_BTHF_VOLUME;
#endif

#ifdef FUNC_SOUND_REMIND
	gSys.SoundRemindOn = TRUE;
	gSys.LanguageMode = LANG_ZH;
#endif


	// 装载断点信息，设置信息
#ifdef FUNC_BREAKPOINT_EN
{
	BP_SYS_INFO *pBpSysInfo;
	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
	gSys.CurModuleID = BP_GET_ELEMENT(pBpSysInfo->CurModuleId);
	gSys.DiskType = BP_GET_ELEMENT(pBpSysInfo->DiskType);
	gSys.Volume = BP_GET_ELEMENT(pBpSysInfo->Volume);
    gSys.Eq = BP_GET_ELEMENT(pBpSysInfo->Eq);
	if(gSys.Volume > MAX_VOLUME)
	{
		gSys.Volume = DEFAULT_VOLUME;
	}
#ifdef FUNC_BT_HF_EN
	gSys.HfVolume = BP_GET_ELEMENT(pBpSysInfo->HfVolume);    
#endif

#ifdef FUNC_SOUND_REMIND
	gSys.SoundRemindOn = BP_GET_ELEMENT(pBpSysInfo->SoundRemindOn);    
	if(gSys.SoundRemindOn > 1)
	{
		gSys.SoundRemindOn = TRUE;
	}
        
	gSys.LanguageMode = BP_GET_ELEMENT(pBpSysInfo->LanguageMode);    
	if(gSys.LanguageMode >= LANG_SUM)
	{
		gSys.LanguageMode = LANG_ZH;
	}
#endif	
}
#endif	
}

//全局变量的保存，关机时调用
void SysVarDeinit(void)
{
#ifdef FUNC_BREAKPOINT_EN
	BP_SaveInfo(BP_SAVE2NVM_FLASH);
#endif	
}

//系统开机判断，如果不满足开机条件，则本函数中继续进入睡眠模式
void SystemOn(void)
{
	APP_DBG("*******SystemOn************\n");

	//获得从SLEEP状态唤醒的触发源标志
	APP_DBG(">>WakeUpOp(), gWakeupTrigFlg:%x\n", gWakeUpFlag);
#ifdef FUNC_IR_KEYPAD_EN
	//系统定义了IR唤醒功能，IR接收头连接于GPIOC2，IR信号唤醒系统后，则判断键值
	if(gWakeUpFlag == WAKEUP_FLAG_GPIOC2)
	{
		TIMER	WaitTimer;
		uint32_t IrKeyData;

		TimeOutSet(&WaitTimer, 3500);
		//如果在规定时间内未发现预定的IR键值，则继续睡眠
		while(!IsTimeOut(&WaitTimer))
		{
			IrKeyData = IrGetKeyCode();
			APP_DBG("IrKeyData = : %X\n", IrKeyData);

			//如果检测到预定的IR键值，则开始运行系统
			if(IrKeyData == 0xB847FF00)  // 不同遥控器需要更改该值
			{
				APP_DBG("WAKE UP FROM SLEEP BY IR!\n");
				return;
			}
			else
			{
				APP_DBG("ERROR IR KEY!\n");
				break;
			}
		}

		APP_DBG("TIMEOUT!\n");
		if(IR_KEY_PORT == IR_USE_GPIOC2)
		{
			APP_DBG("*GPIO_C_IN:%x\n", GpioGetReg(GPIO_C_IN));

			APP_DBG("StandBy Mode\n");
			SysVarDeinit();
			SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_C2, WAKEUP_POLAR_C2_LOW, WAKEUP_TMODE_1MS);
			while((GpioGetReg(GPIO_C_IN) & GPIOC2) != GPIOC2)
			{
				//APP_DBG("请确保进入Sleep状态前WakeUp条件不成立！(GPIO_C_IN:%bx(GPIOC3 shoule be 0))\n", GetGpioReg(GPIO_B_IN));
				WaitMs(10);
			}
		}
//		SysGotoDeepSleepCfg();
		ClkSwitchDpllToRc();
		ClkDpllClose(); //close dpll

		SysGotoDeepSleep(); //power down, 仅软开关和硬开关有效，bypass模式无效。
		OSRescheduleTimeout(0xFFFFFFFF);
		APP_DBG("<<WakeUpOp()\n");
		return;
	}
#endif
}


void AudioSysInfoSetBreakPoint(void)
{
#ifdef FUNC_BREAKPOINT_EN
	BP_SYS_INFO *pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);

	pBpSysInfo->VaildFlag = BP_SIGNATURE;
	pBpSysInfo->CurModuleId = gSys.CurModuleID;
	pBpSysInfo->Volume = gSys.Volume;
    pBpSysInfo->Eq = gSys.Eq;
#ifdef FUNC_SOUND_REMIND
	pBpSysInfo->SoundRemindOn = gSys.SoundRemindOn;
	pBpSysInfo->LanguageMode = gSys.LanguageMode;
#endif
	BP_SaveInfo(BP_SAVE2NVM);

#ifdef BP_SAVE_TO_FLASH // 掉电记忆 test
	BP_SaveInfo(BP_SAVE2FLASH); //Save BPInfoToFlash();
#endif
	APP_DBG("AudioSysInfoSetBreakPoint\n");
	APP_DBG("*  Signature:%X\n", pBpSysInfo->VaildFlag);
	APP_DBG("*  Cur Mode:%X\n", pBpSysInfo->CurModuleId);
	APP_DBG("*  DiskType:%X\n", pBpSysInfo->DiskType);
	APP_DBG("*\n");
	APP_DBG("**********************************\n");
#endif
}

#if 0
void AudioLoadInfoFromFlash(void)
{
#ifdef FUNC_BREAKPOINT_EN
	BP_SYS_INFO NvmSysInfo;

//#ifdef BP_SAVE_TO_FLASH // 掉电记忆 test
	//Load BPInfoFromFlash();//LoadNvmFromFlash();
	//Save BPInfoToFlash();//SaveNvmToFlash();
//#endif

	//BP_LoadSysInfo(&NvmSysInfo);
	APP_DBG("AudioLoadInfoFromFlash\n");
	APP_DBG("*  Signature:%X\n", NvmSysInfo.VaildFlag);
	APP_DBG("*  Cur Mode:%X\n", NvmSysInfo.CurModuleId);
	APP_DBG("*  DiskType:%X\n", NvmSysInfo.DiskType);
	APP_DBG("*\n");
	APP_DBG("**********************************\n");
#endif

}
#endif

//Flash 解锁
//参数: 无
//返回值: 解锁成功返回TRUE，否则返回FALSE
bool FlashUnlock(void)
{
	char cmd[3] = "\x35\xBA\x69";
	
	// unlock flash
	if(SpiFlashIOCtl(IOCTL_FLASH_UNPROTECT, cmd, sizeof(cmd)) != FLASH_NONE_ERR)
	{
		return FALSE;
	}

	return TRUE;
	
}


//Flash加锁
//参数:lock_range ，Flash加锁范围: 
//		 FLASH_LOCK_RANGE_HALF : 			加锁1/2 Flash 空间(从0开始，下同)
//		FLASH_LOCK_RANGE_THREE_QUARTERS: 	加锁3/4 Flash 空间
//		FLASH_LOCK_RANGE_SEVENTH_EIGHT:	加锁7/8 Flash 空间
//		FLASH_LOCK_RANGE_ALL:				加锁全部Flash 空间
//注意: 需要根据定义的USER BANK 的地址来决定加锁的范围，否则可能将USER bank空间加锁而无法写数据
//返回值: 加锁成功返回TRUE，否则返回FALSE
bool FlashLock(SPI_FLASH_LOCK_RANGE lock_range)
{
	if(SpiFlashIOCtl(IOCTL_FLASH_PROTECT, lock_range) != FLASH_NONE_ERR)
	{
		return FALSE;
	}

	return TRUE;
}

//从消息队列中接收消息
//有消息立即返回消息值
//无消息等待WaitTime毫秒
uint16_t MsgRecv(uint16_t WaitTime)
{
	extern bool SysDeviceMsgCtrl(uint16_t Msg);
	extern int32_t MainTaskHandle;	//MAIN TASK的ID号
	extern int32_t* hcurrent;		//当前TASK的ID号
	
	uint16_t Msg = (uint16_t)OSQueueMsgRecv(NULL, NULL, WaitTime);

	if(*hcurrent == MainTaskHandle)
	{
		if(SysDeviceMsgCtrl(Msg))
		{
			return MSG_NONE;
		}
	}
	
	return Msg;
}


//向消息队列中发送指定消息
void MsgSend(uint16_t Msg)
{
#ifdef FUNC_SOUND_REMIND
	if(Msg != MSG_USB_DEVICE_INTERRUPT_CB)
	{
		if(IsSoundRemindPlaying())
		{
			StopSoundRemind();
		}
	}
#endif
	if(Msg == MSG_SD_PLUGOUT
		|| Msg == MSG_USB_PLUGOUT
		|| Msg == MSG_PC_PLUGOUT
		|| Msg == MSG_LINEIN_PLUGOUT
		|| Msg == MSG_RTC_STOP_ALARM_REMIND
		|| Msg == MSG_MIC_PLUGOUT)
		OSQueueMsgSend(Msg, NULL, 0, MSGPRIO_LEVEL_HI, 0);
	else
		OSQueueMsgSend(Msg, NULL, 0, MSGPRIO_LEVEL_MD, 0);
}

bool MsgCheck(uint16_t Msg)
{
	int32_t		ret = 0;
	ret = OSQueueMsgIOCtl(MSGQ_IOCTL_PEEK_TASKMSG, Msg);
	return (ret == 0 ? FALSE:TRUE);
}

//从消息队列中删除指定消息
void MsgClear(uint16_t Msg)
{
	OSQueueMsgIOCtl(MSGQ_IOCTL_DEL_SPECMSG, Msg);
}


//从消息队列中删除指定消息集的所有消息
void MsgClearClass(uint16_t MsgClass)
{
	uint32_t i;

	for(i = 0; i <= 0xFF; i++)
	{
		MsgClear((MsgClass & 0xFFFFFF00) + i);
	}
}

///////////// mode switch state ///////////
static MODE_SWITCH_STATE	gModeSwitchState = MODE_SWITCH_STATE_DONE;
void SetModeSwitchState(MODE_SWITCH_STATE state)
{
	gModeSwitchState = state;
}

MODE_SWITCH_STATE GetModeSwitchState(void)
{
	return gModeSwitchState;
}


