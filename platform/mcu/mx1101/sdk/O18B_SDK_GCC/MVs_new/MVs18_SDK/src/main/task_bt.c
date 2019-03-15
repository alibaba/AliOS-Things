///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_bt.c
///////////////////////////////////////////////////////////////////////////////
/*
zhouyi,2012.07.11, initial version
History:2013.03.21 justin modified
maintainer: Halley
*/

#include "type.h"
#include "app_config.h"
#include "os.h"
#include "bt_stack_api.h"
#include "bt_app_func.h"
#include "sys_app.h"
#include "pcm_sync.h"
#include "bt_app_ui_callback.h"
#include "bt_control_api.h"
#include "ble_api.h"

uint32_t	gBtStatckReady = 0;

//extern SW_TIMER BtLedTimer;
//extern int8_t BtAddr[6]; //default btaddr
//extern uint8_t BtDevName[MAX_BT_DEV_NAME_LENGTH];    //default name
extern void BTDevicePinCFG(void);
extern void ObexCbFunc(const ObexCallbackInfo* Info);
//#ifdef FAST_POWER_ON_OFF_BT
////extern void CheckScanPageFunc(void);
//#endif
#ifdef FUNC_BT_EN

static void StartTimerBtTask(void)
{
	StartBtLed();	
}
static void CheckTimerBtTask(void)
{
	CheckTimer(&BtLedTimer);
#ifdef BT_RECONNECTION_FUNC 
	BtReConnectDetect();//bb's reconnect operation check
#endif
#ifdef FAST_POWER_ON_OFF_BT
	CheckScanPageFunc();
#endif
	BleCommandTimerDetect();
}

static void StopTimerBtTask(void)
{
	StopBtLed();
}
static void BTStackLoop(void)
{
	uint16_t		msg_id;

	SetBtModeStatus(BT_MODE_ACTIVE_STATUS);
	MsgAddSet(MSG_DECODR_BT_CLASS);
	while(1)
	{
#ifdef BT_RF_POWER_ON_OFF
		// Check BT HwPowerState
		if(GetBtHwPowerCmdState() == BT_HW_POWER_CMD_POWER_OFF)
		{
			if(GetNumberOfConnectedProfile() > 0)
			{
				BTDisConnect();
				SetBtHwPowerCmdState(BT_HW_POWER_CMD_POWER_OFF_PENDING);
			}
			else
			{
				break;
			}
		}
		else if(GetBtHwPowerCmdState() == BT_HW_POWER_CMD_POWER_OFF_PENDING)
		{
			if(GetNumberOfConnectedProfile() == 0)
			{
				APP_DBG("BT Power Off...\n");
				break;
			}
		}
#endif
		if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
		{
			msg_id = OSQueueMsgRecv(NULL, NULL, 10);
			if(msg_id == MSG_NEED_MORE_SBC_DATA || GetSbcDataState()== SBC_DATA_STATE_NEED_MORE)
			{
				uint8_t retry = 0;
				while(1)
				{
					if(retry++ > 3) 
					{
						OSRescheduleTimeout(1);
						retry = 0;
					}
					BTStackRun();
					if(GetHasSbcDataFlag())
					{
						SetSbcDataState(SBC_DATA_READY);
						OSQueueMsgSend(MSG_SBC_DATA_READY, NULL, 0, MSGPRIO_LEVEL_HI, 0);
						break;
					}

					if(GetA2dpState() != BT_A2DP_STATE_STREAMING || gSys.CurModuleID != MODULE_ID_BLUETOOTH)
					{
						SetSbcDataState(SBC_DATA_STATE_NONE);
						break;
					}
				}
			}
		}
		else if(gSys.CurModuleID == MODULE_ID_BT_HF)
		{
#ifdef BT_HF_RECORD_FUNC
			//此处出让较多CPU是为了尽快建立U盘录音文件。这会导致这几秒钟通话质量下降。
			if(GetBtRecordState() == BT_RECORD_STATE_INITIALIZING)
			{
				OSRescheduleTimeout(4);
			}
			else
#endif
			{
				OSRescheduleTimeout(2);
			}
			PcmSyncDataProcess();
		}
		else
		{
			msg_id = OSQueueMsgRecv(NULL, NULL, 10);
		}
		CheckTimerBtTask();
		BTStackRun();
	}
}

void BTEntrance(void)
{
	BtSetLinkStatus(TRUE);
	gBtStatckReady = 0;
	InitBtApp();
#ifdef MV_ROM_VERSION
	InitBtRom();
#endif

	MsgAddSet(MSGDEV_BUART_CLASS);
//	MsgAddSet(MSGDEV_BT_STACK_CLASS);
//    SpiFlashInfoInit();

//	EnableBtDbgInfo();	DBG("[BT] Enable Dbg info\n");
//	BTDisableSimplePairing(); DBG("[BT] Disable SSP \n");
//	BTDisableAutoSniffMode(); DBG("[BT] Disable Auto Sniff \n");
//	SetMTK6626RF();	DBG("[BT] Set MTK 6626 RF\n");
//	EnableMtkRfAutoDetect();
//	BTEnableRoleSwitch();
	if(BT_DEVICE_TYPE == BTUartDeviceCsrBC6)
	{
		BTDisableBLEEvent();
	}

#ifndef BT_ADVANCED_AVRCP_SUPPORT
	DisableAdvancedAvrcpFeature();
#endif

//  SetBtTestHook(); //for BT TEST or Debug usage ONLY!!!

	while(1)
	{
#ifdef BT_RF_POWER_ON_OFF
		while(1)
		{
			if(GetBtHwPowerCmdState() == BT_HW_POWER_CMD_POWER_ON)
			{
				APP_DBG("BT Power On...\n");
				SetBtHwPowerCmdState(BT_HW_POWER_CMD_NONE);
				break;
			}
			else
			{
				OSRescheduleTimeout(5);
			}
		}
		UserBtRfPowerPlugIn();
#endif
		GetBtAddr(BtAddr);//get bdaddr
		GetBtName(BtDevName, MAX_BT_DEV_NAME_LENGTH); //get name

		APP_DBG("[BT] TK : BTDeviceStart Tick %d\n", auxtmr_count_get());

		BTDevicePinCFG();
		if(BTDeviceInit(BT_DEVICE_TYPE, BtAddr))
		{
			//OSTaskPrioSet(NULL, 2);
			APP_DBG("[BT] TK : BTDeviceOk Tick %d\n", auxtmr_count_get());

#ifdef FUNC_BT_HF_EN
			gBtStatckReady = BTStackRunInit(BT_DEVICE_TYPE, BtDevName, BT_FEATURE_SUPPORTED_NORMAL);
#else
			gBtStatckReady = BTStackRunInit(BT_DEVICE_TYPE, BtDevName, BT_FEATURE_SUPPORTED_HIFI_ONLY);
#endif // FUNC_BT_HF_EN
			if(gBtStatckReady == 0)
			{
				APP_DBG("[BT] TK : BtStackInit Fail!\n");
				while(1)
				{
					OSRescheduleTimeout(5);
				}
			}
			else
			{
				APP_DBG("[BT] TK : BtStackInit Ok!\n");
				SetBtLedBlue(1);
			}
#ifdef BLE_STACK
			{
				extern BLE_APP_CONTEXT			g_playcontrol_app_context;
				extern GATT_SERVER_PROFILE		g_playcontrol_profile;

				InitBlePlaycontrolProfile();
				
				InitBleStack(&g_playcontrol_app_context, &g_playcontrol_profile);
			}
#endif
			BTStackInitObex(ObexCbFunc);
			StartTimerBtTask();
			
			APP_DBG("\n\n free mem size = %d\n\n", OSMMMIoCtl(MMM_IOCTL_GETSZ_INALL, 0));
			
			BTStackLoop();//Loop until exit

			StopTimerBtTask();
			BTStackRunEnd();

			while(!IsBtLink())
			{
				OSRescheduleTimeout(5);
			}

#ifdef BT_RF_POWER_ON_OFF
			// Hw Power control
			UserBtRfPowerPlugOut();
			SetBtHwPowerCmdState(BT_HW_POWER_CMD_NONE);
			gBtStatckReady = 0;
#endif
		}
		else
		{
			APP_DBG("[BT] TK : BTDeviceInit Fail!\n");
			OSRescheduleTimeout(5);
		}
	}
}
#endif

