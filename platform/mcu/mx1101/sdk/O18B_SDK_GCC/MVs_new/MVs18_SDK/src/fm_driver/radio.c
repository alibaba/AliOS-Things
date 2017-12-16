///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: radio.c
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"

#ifdef FUNC_RADIO_EN

#ifdef FUNC_RADIO_RDA5876A_EN
#include "RDA5876A.h"
#endif
#ifdef FUNC_RADIO_BK1080_EN
#include "bk1080.h"
#endif
#ifdef FUNC_RADIO_QN8035_EN
#include "QN8035.h"
#endif
#ifdef FUNC_RADIO_RDA5807_EN
#include "RDA5807.h"
#endif

#include "radio.h"
#include "gpio.h"
#include "dac.h"
#include "clk.h"
#include "audio_adc.h"
#include "sys_app.h"


static uint8_t FmRadioWorkState = 0;
//extern void GpioSwIoConfig(uint8_t ModeSel);


bool RadioValidCheck(void)
{
	Radio_Name = RADIO_NONE;
#ifdef FUNC_RADIO_QN8035_EN
	if(QN8035PowerOn())
	{
		Radio_Name = RADIO_QN8035;
		return TRUE;
	}
#endif
#ifdef FUNC_RADIO_RDA5807_EN
	if(RDA5807PowerOn())
	{
		Radio_Name = RADIO_RDA5807H;
		return TRUE;
	}
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
	
#endif
	return FALSE;
}

// 初始化FM设备
bool RadioInit(void)
{
	if(!IsRadioWorking())
	{
		APP_DBG("not power on fm\n");
		return FALSE;
	}

#ifdef FUNC_RADIO_RDA5876A_EN
	if(Rda5876hpInit())
#endif
#ifdef FUNC_RADIO_BK1080_EN
	if(BK1080Init())
#endif
#ifdef FUNC_RADIO_QN8035_EN
	if(QN8035Init())
#endif
#ifdef FUNC_RADIO_RDA5807_EN
	if(RDA5807Init())	
#endif
	{
		APP_DBG("Radio Init Ok!\n");
		return TRUE;
	}
	else
	{
		APP_DBG("Radio Init Fail!\n");
		return FALSE;
	}
}

// 打开FM设备
bool RadioPowerOn(void)
{
	WaitMs(20);
#ifdef FUNC_RADIO_RDA5807_EN
	if(RDA5807PowerOn())	
#endif	

#ifdef FUNC_RADIO_RDA5876A_EN
	if(Rda5876hpPatchPowerOn())
#endif
#ifdef FUNC_RADIO_BK1080_EN
	if(BK1080PowerOn())
#endif
#ifdef FUNC_RADIO_QN8035_EN
	if(QN8035PowerOn())
#endif
	{
		APP_DBG("Radio PowerOn OK\n");
		FmRadioWorkState = 1;
		return TRUE;
	}
	else
	{
		APP_DBG("Radio PowerOn Fail\n");
		return FALSE;
	}
}

// 关闭FM设备
bool RadioPowerOff(void)
{
#ifdef FUNC_RECORD_EN
	if(gSys.NextModuleID == MODULE_ID_RECORDER)
	{
		return FALSE;
	}
#endif
	if(IsRadioWorking())
	{
		FmRadioWorkState = 0;
#ifdef FUNC_RADIO_RDA5807_EN
		if(RDA5807PowerDown())	
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
		if(Rda5876hpPatchPowerOff())
#endif
#ifdef FUNC_RADIO_BK1080_EN
		if(BK1080PowerDown())
#endif
#ifdef FUNC_RADIO_QN8035_EN
		if(QN8035PowerDown())
#endif
		{
			APP_DBG("Radio PowerOff OK\n");
			FmRadioWorkState = 1;
			return TRUE;
		}
		else
		{
			APP_DBG("Radio PowerOff Fail\n");
			return FALSE;
		}

	}
	return TRUE;
}

// 检查FM设备状态(TRUE - 工作状态、FALSE - 挂起状态)
bool IsRadioWorking(void)
{
	return (FmRadioWorkState == 1);
}

// 设置当前频率
bool RadioSetFreq(uint16_t Freq)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5807_EN
		RDA5807SetFreq(Freq);
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
		Rda5876hpFreqSet(Freq);
#endif
#ifdef FUNC_RADIO_BK1080_EN
		BK1080SetFreq(Freq);
#endif
#ifdef FUNC_RADIO_QN8035_EN
		QN8035SetFreq(Freq);
#endif
	}

	return TRUE;
}

// 指定频率搜索
void RadioSearchSet(uint16_t Freq)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5807_EN
		RDA5807SetFreq(Freq);
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
		Rda5876hpFreqSet(Freq);
#endif
#ifdef FUNC_RADIO_BK1080_EN
		BK1080SetFreq(Freq);
#endif
#ifdef FUNC_RADIO_QN8035_EN
		QN8035SearchSet(Freq);
#endif
	}
}

// 静音切换
bool RadioMute(bool MuteFlag)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5807_EN
		RDA5807Mute(MuteFlag);
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
		Rda5876hpMute(MuteFlag);
#endif
#ifdef FUNC_RADIO_BK1080_EN
		BK1080Mute(MuteFlag);
#endif
#ifdef FUNC_RADIO_QN8035_EN
		QN8035Mute(MuteFlag);
#endif
		return TRUE;
	}
	return FALSE;
}

// 判断信号强度，用于搜台结果查询，如果返回TRUE，表明搜到新的电台，否则未搜到
bool RadioRSSIRead(uint16_t Freq)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5807_EN
		return RDA5807SearchRead(Freq);
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
		return Rda5876hpGetTuneResult(Freq);
#endif
#ifdef FUNC_RADIO_BK1080_EN
		return BK1080SearchRead(Freq);
#endif
#ifdef FUNC_RADIO_QN8035_EN
		return QN8035SearchRead(Freq);
#endif
	}
	return FALSE;
}

//FM通道输出高阻
void RadioLinkOutputHIZ(bool HizFg)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5807_EN
		RDA5807OutputHIZ(HizFg);
#endif
#ifdef FUNC_RADIO_RDA5876A_EN
		Rda5876hpSetHighImpedance(HizFg);
#endif
#ifdef FUNC_RADIO_BK1080_EN
		BK1080OutputHIZ(HizFg);
#endif
#ifdef FUNC_RADIO_QN8035_EN
		QN8035OutputHIZ(HizFg);
#endif
	}
}

// 获取是否立体声
bool RadioIsStereo(void)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5876A_EN
		return Rda5876hpIsStereo();
#endif
	}
	return FALSE;
}

//	设置立体声
void RadioSetStereo(bool StereoFlag)
{
	if(IsRadioWorking())
	{
#ifdef FUNC_RADIO_RDA5876A_EN
		Rda5876hpSetStereo(StereoFlag);
#endif
	}
}

#endif/*FUNC_RADIO_EN*/

