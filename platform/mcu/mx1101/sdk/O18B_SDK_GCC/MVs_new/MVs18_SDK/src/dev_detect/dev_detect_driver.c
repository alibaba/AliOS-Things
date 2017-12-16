////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:dev_detect_driver.c
//
//            maintainer: Halley
//
//		Description	:
//					a device detect driver(realtime to check sd/usb/linein/.. plugin or plugout event)
//
//		Changelog	:
//					2013-06-13	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "app_config.h"
#include "os.h"
#include "msgq.h"
#include "gpio.h"
#include "sd_card.h"
#include "irqs.h"
#include "dev_detect_driver.h"
#include "device_hcd.h"
#include "host_hcd.h"
#include "dac.h"
#include "clk.h"
#include "audio_adc.h"
#include "radio_control.h"
#include "recorder_control.h"
#include "sys_app.h"
#include "bt_control_api.h"

static bool IsCardLinkFlag = TRUE;
static bool IsUDiskLinkFlag = TRUE;
static bool IsLineInLinkFlag = TRUE;
static bool IsPcLinkFlag = TRUE;
#ifdef FUNC_MIC_EN
static bool IsMicLinkFlag = FALSE;
#endif
DECLARE_SEMAPHORE(GpioSdClkSema, 1);

void LockSdClk(void)
{
	OSDOWN(GpioSdClkSema);
}

void UnLockSdClk(void)
{
	OSUP(GpioSdClkSema);
}

#ifdef FUNC_CARD_EN
static __INLINE void CardDetectDelay(void)
{
	//__nop();
	//__nop();
	//__nop();
	//__nop();
	//__nop();
}
#endif
bool IsCardLink(void)
{
#ifdef FUNC_CARD_EN
	bool TempFlag;
	LockSdClk();
	GpioSdIoConfig(RESTORE_TO_GENERAL_IO);

	GpioClrRegBits(CARD_DETECT_PORT_PU, CARD_DETECT_BIT_MASK);
	GpioClrRegBits(CARD_DETECT_PORT_PD, CARD_DETECT_BIT_MASK);
	GpioClrRegBits(CARD_DETECT_PORT_OE, CARD_DETECT_BIT_MASK);

	GpioSetRegBits(CARD_DETECT_PORT_IE, CARD_DETECT_BIT_MASK);

	CardDetectDelay(); // ??? Disable和设置为IE 后，可能需要时延

	if(GpioGetReg(CARD_DETECT_PORT_IN) & CARD_DETECT_BIT_MASK)
	{
		TempFlag = FALSE;
	}
	else
	{
		TempFlag = TRUE;
	}
	
	if(TempFlag)
	{
		GpioSdIoConfig(SD_PORT_NUM);
	}

	UnLockSdClk();

	return TempFlag;
#else
	return FALSE;
#endif
}


bool IsUDiskLink(void)
{
#ifdef FUNC_USB_EN
#if (UDISK_PORT_NUM == 1)
	return UsbHost1IsLink();
#endif

#if (UDISK_PORT_NUM == 2)
	return UsbHost2IsLink();
#endif
#else
	return FALSE;
#endif
}

bool CheckCardLinkFlag(void)
{
	return IsCardLinkFlag;
}
bool CheckUSBLinkFlag(void)
{
	return IsUDiskLinkFlag;
}
// Check if any disk link
bool CheckAllDiskLinkFlag(void)
{
	return (IsUDiskLinkFlag || IsCardLinkFlag);
}

bool IsPcLink(void)
{
#if (PC_PORT_NUM == 1)
	return UsbDevice1IsLink();
#endif

#if (PC_PORT_NUM == 2)
	return UsbDevice2IsLink();
#endif
}

//bool IsUsbAudioLink()
//{
//	//return UsbDevice2IsLink();
//}
bool IsI2sInLink(void)
{
    
#ifdef FUNC_I2SIN_EN
    GpioI2sIoConfig(I2SIN_I2S_IO_PORT);     //i2s port sel
#ifdef FUNC_I2S_MASTER_EN
    GpioMclkIoConfig(I2SIN_MCLK_IO_PORT);   //mclk port sel:output
#else
    GpioMclkIoConfig(I2SIN_MCLK_IO_PORT+2);   //mclk port sel:input
#endif
    return TRUE;
#else
    return FALSE;
#endif
}

bool IsLineInLink(void)
{
#ifdef LINEIN_DETECT_BIT_MASK
#define LINEIN_JETTER_TIMES		10	//连接检测消抖时间：10次，100ms

	static uint8_t LineInLinkState = 0;

	//设为输入，带上拉
	GpioClrRegBits(LINEIN_DETECT_PORT_OE, LINEIN_DETECT_BIT_MASK);
	GpioClrRegBits(LINEIN_DETECT_PORT_PU, LINEIN_DETECT_BIT_MASK);
	GpioClrRegBits(LINEIN_DETECT_PORT_PD, LINEIN_DETECT_BIT_MASK);

	GpioSetRegBits(LINEIN_DETECT_PORT_IE, LINEIN_DETECT_BIT_MASK);

	if(GpioGetReg(LINEIN_DETECT_PORT_IN) & LINEIN_DETECT_BIT_MASK)
	{
		LineInLinkState = 0;						//断开状态不做消抖处理
	}
	else
	{
		if(LineInLinkState < LINEIN_JETTER_TIMES)	//连接状态做消抖处理
		{
			LineInLinkState++;
		}
	}
	return (LineInLinkState >= LINEIN_JETTER_TIMES);
#else
    return TRUE;
#endif
}

#ifdef FUNC_MIC_EN
bool IsMicInLink(void)
{
#ifdef FUNC_MIC_DET_EN
	static uint8_t MicLinkState = 0;
	#define MICIN_JETTER_TIMES		10	//连接检测消抖时间：10次，100ms
	//设为输入，打开上拉
	GpioClrRegOneBit(MICIN_DETECT_PORT_OE, MICIN_DETECT_BIT);
	GpioClrRegOneBit(MICIN_DETECT_PORT_PU, MICIN_DETECT_BIT);
	GpioClrRegOneBit(MICIN_DETECT_PORT_PD, MICIN_DETECT_BIT);
	GpioSetRegBits(MICIN_DETECT_PORT_IE, MICIN_DETECT_BIT);

	if(GpioGetReg(MICIN_DETECT_PORT_IN) & MICIN_DETECT_BIT)
	{
		MicLinkState = 0;						//断开状态不做消抖处理
	}
	else
	{
		if(MicLinkState < MICIN_JETTER_TIMES)	//连接状态做消抖处理
		{
			MicLinkState++;
		}
	}
	gSys.MicEnable = (MicLinkState >= MICIN_JETTER_TIMES);
	return gSys.MicEnable;
#else
	gSys.MicEnable = TRUE;
	return TRUE;
#endif
}
#endif

#ifdef FUNC_RADIO_EN
//检查是否有插入Radio 模块
bool IsRadioLink(void)
{
	if(Radio_Name == RADIO_NONE)
	{
		RadioValidCheck();
	}
	return (Radio_Name != RADIO_NONE);
}
#endif


DECLARE_SEMAPHORE(QuickRespSema, 1);
static volatile bool quick_response_flag = FALSE;

void SetQuickResponseFlag(bool flag)
{
	OSDOWN(QuickRespSema);
	quick_response_flag = flag;
	APP_DBG("Quick Response : %d\n", quick_response_flag);
	OSUP(QuickRespSema);
}

bool GetQuickResponseFlag(void)
{
	if(quick_response_flag)
	{
		SetQuickResponseFlag(FALSE);
		return TRUE;
	}

	return FALSE;
}

void DeviceDetect(void)
{
	int32_t Msg;

	if(gSys.CurModuleID == MODULE_ID_BT_HF)
	{
		// 蓝牙免提通话时，插入或拔除SD、U盘等设备，不切换模式，以免中断通话
		return;
	}

#ifdef FUNC_USB_EN
	//U盘插拔检测
	if((!IsUDiskLinkFlag) && IsUDiskLink())
	{
		IsUDiskLinkFlag = TRUE;
		DBG("UDiskPlugIn\n");		
		Msg = MSG_USB_PLUGIN;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_USB_PLUGIN);
		MsgClear(MSG_USB_PLUGOUT);
		MsgSend(Msg);
		return;
	}
	else if((IsUDiskLinkFlag) && (!IsUDiskLink()))
	{
//        if(gSys.CurModuleID == MODULE_ID_PLAYER_USB)
//        {
//                DacSoftMuteSet(TRUE, TRUE);
//        }
		IsUDiskLinkFlag = FALSE;
		DBG("UDiskPlugOut\n");
		Msg = MSG_USB_PLUGOUT;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_USB_PLUGIN);
		MsgClear(MSG_USB_PLUGOUT);
		MsgSend(Msg);
		return;
	}
#endif
	
	//PC插拔检测
	if((!IsPcLinkFlag) && IsPcLink())
	{
		IsPcLinkFlag = TRUE;
		DBG("PcPlugIn\n");
		Msg = MSG_PC_PLUGIN;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_PC_PLUGIN);
		MsgClear(MSG_PC_PLUGOUT);
		MsgSend(Msg);
		return;
	}
	else if((IsPcLinkFlag) && (!IsPcLink()))
	{
		IsPcLinkFlag = FALSE;
		DBG("PcPlugOut\n");
		Msg = MSG_PC_PLUGOUT;
		//为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_PC_PLUGIN);
		MsgClear(MSG_PC_PLUGOUT);
		MsgSend(Msg);
		return;
	}

#ifdef FUNC_CARD_EN
	//CARD插拔检测
	if((!IsCardLinkFlag) && IsCardLink())
	{
		IsCardLinkFlag = TRUE;
		DBG("CardPlugIn\n");
		Msg = MSG_SD_PLUGIN;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_SD_PLUGIN);
		MsgClear(MSG_SD_PLUGOUT);
		MsgSend(Msg);
		return;
	}
	else if((IsCardLinkFlag) && (!IsCardLink()))
	{
//        if(gSys.CurModuleID == MODULE_ID_PLAYER_SD)
//        {
//            DacSoftMuteSet(TRUE, TRUE);
//        }
		IsCardLinkFlag = FALSE;
		DBG("CardPlugOut\n");
		Msg = MSG_SD_PLUGOUT;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_SD_PLUGIN);
		MsgClear(MSG_SD_PLUGOUT);
		MsgSend(Msg);
		return;
	}
#endif
	
	//LINEIN插拔检测
	if((!IsLineInLinkFlag) && IsLineInLink())
	{
		IsLineInLinkFlag = TRUE;
		DBG("LineInPlugIn\n");
		Msg = MSG_LINEIN_PLUGIN;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_LINEIN_PLUGIN);
		MsgClear(MSG_LINEIN_PLUGOUT);
		MsgSend(Msg);
		return;
	}
	else if((IsLineInLinkFlag) && (!IsLineInLink()))
	{
		IsLineInLinkFlag = FALSE;
		DBG("LineInPlugOut\n");
		Msg = MSG_LINEIN_PLUGOUT;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_LINEIN_PLUGIN);
		MsgClear(MSG_LINEIN_PLUGOUT);
		MsgSend(Msg);
		return;
	}
#ifdef FUNC_MIC_EN
#ifdef FUNC_MIC_DET_EN
	if((!IsMicLinkFlag) && IsMicInLink())
	{
		//MIC插入
        IsMicLinkFlag = TRUE;
        DBG("MicInPlugIn\n");

		Msg = MSG_MIC_PLUGIN;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_LINEIN_PLUGIN);
		MsgClear(MSG_LINEIN_PLUGOUT);
		MsgSend(Msg);
	}
	else if((IsMicLinkFlag) && (!IsMicInLink()))
	{
		//MIC拔出
		IsMicLinkFlag = FALSE;
		DBG("MicInPlugOut\n");
		Msg = MSG_MIC_PLUGOUT;
		// 为避免多个插拔事件的排队现象，所以每次收到新的插拔事件都清除前面未处理的事件，遵循后插先播原则
		MsgClear(MSG_LINEIN_PLUGIN);
		MsgClear(MSG_LINEIN_PLUGOUT);
		MsgSend(Msg);
	}
#else
	if(!IsMicLinkFlag && IsMicInLink())
	{
		IsMicLinkFlag = TRUE;
		Msg = MSG_MIC_PLUGIN;
		MsgClear(MSG_LINEIN_PLUGIN);
		MsgClear(MSG_LINEIN_PLUGOUT);
		MsgSend(Msg);
	}
#endif
#endif
}

// 上电时，硬件扫描消抖
void InitDeviceDetect(void)
{
	TIMER Timer;

	// GpioLcdIoConfig(RESTORE_TO_GENERAL_IO); // ??? 当前linein gpio c6 与 lcd touch 存在复用关系，所以先设置io为普通io口
	//TimeOutSet(&Timer, 800);

	//set the device detection time upto 500 ms by Robert. -20140114
	if(OSSysAuxtmrGet() + 200 + 130 + 60 < 500)
	{
		/*
		 * if device detection(especial USB) time less than 500ms,complement the left time to delete the device jitter
		 */
		TimeOutSet(&Timer, 500 - 60 - 130 - 200 - OSSysAuxtmrGet());
	}
	else
	{
		/*
		 * give 20 ms enough to device detection
		 */
		TimeOutSet(&Timer, 20);
	}
	while(!IsTimeOut(&Timer))
	{
#ifdef FUNC_USB_EN
		IsUDiskLink();
#endif
		IsPcLink();

#ifdef FUNC_CARD_EN
		IsCardLink();
#endif
		IsLineInLink();
#ifdef FUNC_MIC_DET_EN
		IsMicLinkFlag = IsMicInLink();
#endif
	}
}





