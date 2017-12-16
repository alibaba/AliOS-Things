///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_main.c
//  maintainer Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "bt_app_func.h"
#include "line_in_control.h"
#include "i2s_in_control.h"
#include "gpio.h"
#include "sd_card.h"
#include "recorder_control.h"
#include "radio_control.h"
#include "usb_audio_control.h"
#include "rtc_control.h"
#include "idle_control.h"
#include "lcd.h"
#include "wakeup.h"
#include "clk.h"
#include "os.h"
#include "sys_vol.h"
#include "audio_path.h"
#include "task_decoder.h"
#include "nvm.h"
#include "debug.h"
#include "power_management.h"
#include "breakpoint.h"
#include "sys_app.h"
#include "dac.h"
#include "timer.h"
#include "player_control.h"
#include "recorder_control.h"
#include "dev_detect_driver.h"

void EnterIntoNextMode(void);
#ifdef FUNC_RADIO_DIV_CLK_EN
void SysClkDivison(char NewClkDiv);
#endif

extern bool BtHfControl(void);

extern uint32_t GetNextModeId(uint32_t CurModeId);

extern int32_t BtTaskHandle;


//main entrance of main task
//主任务程序入口，可按需在对应入口处增加对应的模块调用
void GuiTaskEntrance(void)
{
	uint16_t i;
	
	APP_DBG("main task Init...\n");
	SetSysVol();
	OSRescheduleTimeout(500);

	// 消息注册
	MsgAddSet(MSG_MAIN_CLASS);
	MsgAddSet(MSG_DEV_CLASS);
	MsgAddSet(MSG_BT_CLASS);
	MsgAddSet(MSG_USB_DEVICE_CLASS);
	
	APP_DBG("waiting bt dev init.\n");
#ifdef FUNC_BT_EN
	for(i = 0; i < 100; i++)
	{
		//if((BtTaskHandle != 0) && IsBtLink())
		{
			break;
		}
		OSRescheduleTimeout(5);
	}
#endif

	if(gSys.NextModuleID != MODULE_ID_UNKNOWN) // 开机的过程中插入U盘、SD卡，需要做合理的模式切换
	{
		gSys.CurModuleID = gSys.NextModuleID;
	}

#ifdef FUNC_REC_PLAYBACK_EN
	if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY || gSys.CurModuleID == MODULE_ID_RECORDER)
	{
		EnterRecBackPlay(IsUDiskLink() ? MODULE_ID_PLAYER_USB : MODULE_ID_PLAYER_SD, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0 ,0);
	}
#endif

	//Task main loop
	while(1)
	{
		gSys.NextModuleID = MODULE_ID_UNKNOWN;	//init the flag	
		MsgClearClass(MSG_MAIN_CLASS);			//进入一个模式前，先清空消息队列

#ifdef BT_RF_POWER_ON_OFF
		if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
		{
			BtHwPowerOn();
		}
		else if(gSys.CurModuleID == MODULE_ID_BT_HF)
		{
			
		}
		else
		{
			BtHwPowerOff();
		}
#endif

#ifdef FAST_POWER_ON_OFF_BT
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH || gSys.CurModuleID == MODULE_ID_BT_HF)
	{
		if(IsBTScanPageEnable() != BT_SCAN_PAGE_ENABLE)
		{
			FastEnableBT();
			#ifdef BT_POWERON_RECONNECTION
			StartBtReConnection(BT_POR_TRY_COUNTS, BT_POR_INTERNAL_TIME);// reconnect last bt device, try 3 times
			#endif
		}
	}
	else
	{
		if(IsBTScanPageEnable() != BT_SCAN_PAGE_DISABLE)
		{
			FastDisableBT();
		}
	}
#endif
		switch(gSys.CurModuleID)//init value can be M0
		{	
			case MODULE_ID_PLAYER_SD:  // enter into module player
			case MODULE_ID_PLAYER_USB: // enter into module player
			case MODULE_ID_BLUETOOTH:  // BT shares the same entry func.
#if defined(FUNC_USB_EN) || defined(FUNC_CARD_EN) || defined(FUNC_BT_EN)
				PlayerControl();
#endif
				break;
			
#ifdef FUNC_BT_HF_EN
			case MODULE_ID_BT_HF:
				//BtHfControl();
				break;
#endif
			
//			case MODULE_ID_ALARM_REMIND: // Alarm remind shares the same entry func
//				break;

#ifdef FUNC_LINEIN_EN
			case MODULE_ID_LINEIN:
				LineInControl();
				break;
#endif

#ifdef FUNC_I2SIN_EN
            
            case MODULE_ID_I2SIN:
                I2sInControl();
                break;
#endif
            
#ifdef FUNC_RECORD_EN
			case MODULE_ID_RECORDER:
				RecorderControl();
				break;
#endif

#ifdef FUNC_RADIO_EN
			case MODULE_ID_RADIO:
#ifdef FUNC_SOUND_REMIND
#ifdef FUNC_RADIO_DIV_CLK_EN	
				{
					ClkSysClkSel(1); // 降频至80M
					SysClkDivison(gSys.FmSysClkDivFactor);	//ljg, call directly
					gSys.IsSysClk96MHz = 0;  
				}
#endif
#endif
				RadioControl();
#ifdef FUNC_SOUND_REMIND
#ifdef FUNC_RADIO_DIV_CLK_EN
				if(0 == gSys.IsSysClk96MHz)
				{
					ClkSysClkSel(0); // 再恢复到96M
					SysClkDivison(1);	//ljg, call directly
					gSys.IsSysClk96MHz = 1;  
				}
#endif
#endif
				break;
#endif

#if (defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN))
			case MODULE_ID_USB_AUDIO:
			case MODULE_ID_USB_READER:
			case MODULE_ID_USB_AUDIO_READER:
				//UsbAudioControl();
				break;
#endif

#ifdef FUNC_RTC_EN
			case MODULE_ID_RTC:
				RtcControl();
				break;
#endif
			
#ifdef FUNC_REC_PLAYBACK_EN
            case MODULE_ID_REC_BACK_PLAY:
				RecBackPlayControl();
				break;
#endif

			case MODULE_ID_POWEROFF:
				SystemPowerOffControl();
				break;
		
			case MODULE_ID_STANDBY:
				SystemStandbyControl();
				break;
		
			default:
				IdleControl();
				//APP_DBG("error");
				break;
		}
		
		//from cache list or top-frame, update gCurBackModuleID for two UI task's mode
		//use module top diagram for one UI task
		//new device plugin, then switch to the related module:
		//set NextModuleID and gCurModuleID at the interrupt or detection part
		if(gSys.NextModuleID == MODULE_ID_UNKNOWN)
		{
			gSys.NextModuleID = GetNextModeId(gSys.CurModuleID);
			APP_DBG("Next ModuleId = %d\n", gSys.NextModuleID);
		}

		gSys.CurModuleID = gSys.NextModuleID;//set mode to the next mode
		// quick response
		SetQuickResponseFlag(FALSE);
#ifdef FUNC_BREAKPOINT_EN		
		if((gSys.CurModuleID != MODULE_ID_POWEROFF) 
			&& (gSys.CurModuleID != MODULE_ID_STANDBY) 
			&& (gSys.CurModuleID != MODULE_ID_BT_HF)) 
		{
			BP_SYS_INFO *pSysInfo;
			pSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
			if(gSys.CurModuleID == MODULE_ID_RECORDER)
			{
#ifdef FUNC_REC_PLAYBACK_EN
				BP_SET_ELEMENT(pSysInfo->CurModuleId, MODULE_ID_REC_BACK_PLAY);
#endif
			}
			else
			{
				BP_SET_ELEMENT(pSysInfo->CurModuleId, gSys.CurModuleID);
			}
			APP_DBG("pSysInfo->CurModuleId:%d\n", pSysInfo->CurModuleId);
			BP_SaveInfo(BP_SAVE2NVM);
#ifdef BP_SAVE_TO_FLASH // 掉电记忆
			BP_SaveInfo(BP_SAVE2FLASH);
#endif
		}
#endif
	}
}

#ifdef FUNC_RADIO_DIV_CLK_EN
void SysClkDivison(char NewClkDiv)
{
	extern int calibrate_delay(void);
	extern int systick_config(void);
//	extern void SWUartBuadRateAutoAdap(char PreFq, char CurFq);	
//	extern void ClkModuleDivSetNotify(unsigned int PreFq, unsigned int CurFq);

	char CurClkDiv = ClkModuleDivGet();
	
	OSSchedulerSuspend();
	SpiFlashClktoSysClk();
	
	ClkModuleDivSetNotify(CurClkDiv, NewClkDiv);
	systick_config();
	Timer0Adjust(CurClkDiv, NewClkDiv);
	Timer1Adjust(CurClkDiv, NewClkDiv);
	//SWUartBuadRateAutoAdap(CurClkDiv, NewClkDiv);
	calibrate_delay();
	
	SpiFlashClkRestore();
	OSSchedulerResume();
}
#endif
