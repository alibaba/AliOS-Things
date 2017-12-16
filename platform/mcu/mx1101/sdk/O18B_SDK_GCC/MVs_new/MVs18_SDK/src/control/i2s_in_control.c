///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2014, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: i2s_in_control.c
//  maintainer: YunWang
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "breakpoint.h"
#include "sys_app.h"
#include "nvm.h"
#include "i2s.h"
#include "audio_path.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "i2s_in_control.h"
#include "mixer.h"
#include "dac.h"
#include "eq_params.h"

#ifdef FUNC_I2SIN_EN
void I2sInLedCb(void* Param)
{

}
static TIMER I2SFreqAdjustTimer;

void I2sFreqAdjust(void)
{
	uint16_t Samples;

	if(!IsTimeOut(&I2SFreqAdjustTimer))
	{
		return;
	}
	TimeOutSet(&I2SFreqAdjustTimer, 500);

	Samples	= MixerGetRemainSamples(MIXER_SOURCE_ANA_STERO);
	DacSampRateAdjust(TRUE, (Samples - 2048) * 200);
	//APP_DBG("%d\n", Samples);
}

bool I2sInControl(void)
{
    uint16_t Msg = 0;
	SW_TIMER I2sInTimer;
    
    APP_DBG("Enter I2sInControl...\n");
	if(!IsI2sInLink())
	{
		APP_DBG("device no ready! exit I2sInControl\n");
		return FALSE;
	}
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
    // 消息注册
    if(gSys.MuteFlag)
	{
    	gSys.MuteFlag = 0;
		AudioPlayerMute(gSys.MuteFlag);
	}
    //SoundRemind(SOUND_I2SIN);
    
    AudioAnaSetChannel(AUDIO_CH_I2SIN);	
  
    
#ifdef FUNC_TREB_BASS_EN
	if(gSys.EqStatus == 0)
	{
		TrebBassSet(gSys.TrebVal, gSys.BassVal);
	}
#endif

	InitTimer(&I2sInTimer, 250, I2sInLedCb);
	StartTimer(&I2sInTimer);
    
    TimeOutSet(&I2SFreqAdjustTimer, 500);
    
	AudioSysInfoSetBreakPoint();

    SetModeSwitchState(MODE_SWITCH_STATE_DONE);
    
    while(Msg != MSG_COMMON_CLOSE)
	{
		CheckTimer(&I2sInTimer);
		Msg = MsgRecv(5);
        
#ifndef FUNC_I2S_MASTER_EN  
  #if (!defined (OUTPUT_CHANNEL_CLASSD)) && (!defined (OUTPUT_CHANNEL_DAC_CLASSD))//usb 模式下微调
      #ifndef FUNC_SOFT_ADJUST_EN
        I2sFreqAdjust();            //硬件微调  
      #endif
  #endif
#endif
		switch(Msg)
		{			
			case MSG_MODE:        //working mode changing
				Msg = MSG_COMMON_CLOSE;
				break;
            
            case MSG_POWER:       //Standy mode
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;

			default:
				CommonMsgProccess(Msg);
                break;
		}
	}
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	{
		BP_SYS_INFO *pBpSysInfo;
		pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
		BP_SET_ELEMENT(pBpSysInfo->Volume, gSys.Volume);
        BP_SET_ELEMENT(pBpSysInfo->Eq, gSys.Eq);
		BP_SaveInfo(BP_SAVE2NVM);
	}
#endif

#if (!defined (OUTPUT_CHANNEL_CLASSD)) && (!defined (OUTPUT_CHANNEL_DAC_CLASSD))//usb 模式下微调
    DacSampRateAdjust(FALSE, 0);
#endif
	AudioAnaSetChannel(AUDIO_CH_NONE);	
	APP_DBG("leave I2sInControl...\n");
	return TRUE;
        
    
}
#endif
