///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_app_func.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include "app_config.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "gpio.h"
#include "sys_app.h"
#include "spi_flash.h"
#include "sound_remind.h"
#include "pcm_sync.h"
#include "clk.h"
#include "dac.h"
#include "pcm_fifo.h"
#include "pcm_transfer.h"
#include "audio_path.h"
#include "mixer.h"
#include "os.h"
#include "recorder_control.h"
#include "bt_app_ui_callback.h"
#include "player_control.h"
#include "bt_stack_api.h"
#include "breakpoint.h"

extern bool FlshUpdateBtLatestDevAddr(uint8_t* BtAddr);

uint8_t gBTRecorder = 0;//蓝牙通话录音控制变量
BT_CONTEXT_DATA		g_bt_app = {0};

///////////////////// misc ////////////////////////
uint8_t sBtLinkFlag     = 0;

bool IsBtLink(void)
{
	return (sBtLinkFlag);
}
void BtSetLinkStatus(bool Status)
{
	sBtLinkFlag = Status;
}


////////////////////bt app func////////////////////

void InitBtApp(void)
{
//	g_bt_app.active_profile = BT_ACTIVE_NONE;
	g_bt_app.hfp_state = BT_HFP_STATE_NONE;
	g_bt_app.a2dp_state = BT_A2DP_STATE_NONE;
	g_bt_app.avrcp_state = BT_AVRCP_STATE_NONE;
	g_bt_app.connected_profile = BT_CONNECTED_NONE_FLAG;
	g_bt_app.mode_status = BT_MODE_NONE_STATUS;
	g_bt_app.hf_mode_state = BT_HF_MODE_STATE_NONE;
	g_bt_app.sco_conneted_flag = FALSE;
#ifdef BT_HF_RECORD_FUNC
	g_bt_app.bt_record_state = BT_RECORD_STATE_NONE;
#endif
#ifdef BT_ADVANCED_AVRCP_SUPPORT
	g_bt_app.adv_avrcp_support_flag = FALSE;
	g_bt_app.adv_avrcp_play_status = CurStatus_ERROR;
#endif
}

void SetBtModeStatus(uint8_t status)
{
	APP_DBG("[BT] SetBtModeStatus status = %d\n", status);
	g_bt_app.mode_status = status;
}

uint8_t GetBtModeStatus(void)
{
	return g_bt_app.mode_status;
}
/*
void SetActiveProfile(uint8_t active_profile)
{
	g_bt_app.active_profile = active_profile;
}

uint8_t GetActiveProfile(void)
{
	return g_bt_app.active_profile;
}
*/
void SetHfpState(uint8_t hfp_state)
{
	APP_DBG("SetHfpState state = %d\n", hfp_state);
	g_bt_app.hfp_state = hfp_state;
}

uint8_t GetHfpState(void)
{
	return g_bt_app.hfp_state;
}

void SetA2dpState(uint8_t a2dp_state)
{
	APP_DBG("SetA2dpState state = %d\n", a2dp_state);
	g_bt_app.a2dp_state = a2dp_state;
}

uint8_t GetA2dpState(void)
{
	return g_bt_app.a2dp_state;	
}

void SetAvrcpState(uint8_t avrcp_state)
{
	APP_DBG("SetAvrcpState state = %d\n", avrcp_state);
	g_bt_app.avrcp_state = avrcp_state;
}

uint8_t GetAvrcpState(void)
{
	return g_bt_app.avrcp_state;
}

void SetHfModeState(uint8_t state)
{
	APP_DBG("[BT] SetHfModeState mode_state = %d\n", state);
	g_bt_app.hf_mode_state = state;
}

uint8_t GetHfModeState(void)
{
	return g_bt_app.hf_mode_state;
}

void ConnectedProfile(uint32_t profile)
{
	g_bt_app.connected_profile |= profile;

	// Set BT LED
	if(GetNumberOfConnectedProfile() >= 1)
	{
		SetBtLedBlue(TRUE);
		SetBtLedRed(FALSE);
	}

	// connect alarm & save bt address
	if(GetNumberOfConnectedProfile() == 1)
	{
		BtSoundTip(BT_SOUND_TIP_CONNECTED);
		FlshUpdateBtLatestDevAddr(GetCurConnectedRemDevAdd());
	}

}

uint8_t GetNumberOfConnectedProfile(void)
{
	uint8_t		number_of_profile = 0;

	if(g_bt_app.connected_profile & BT_CONNECTED_A2DP_FLAG)
	{
		number_of_profile++;
	}

	if(g_bt_app.connected_profile & BT_CONNECTED_HF_FLAG)
	{
		number_of_profile++;
	}

	if(g_bt_app.connected_profile & BT_CONNECTED_SPP_FLAG)
	{
		number_of_profile++;
	}
		
	if(g_bt_app.connected_profile & BT_CONNECTED_AVRCP_FLAG)
	{
		number_of_profile++;
	}

	return 	number_of_profile;
}

void DisconnectProfile(uint32_t profile)
{
	g_bt_app.connected_profile &= ~profile;

	// Set BT LED & disconnect alarm
	if(GetNumberOfConnectedProfile() == 0)
	{
		SetBtLedBlue(FALSE);
		SetBtLedRed(FALSE);

		BtSoundTip(BT_SOUND_TIP_DISCONNECTED);
	}

}

void DisconnectAllProfile(void)
{
	g_bt_app.connected_profile = BT_CONNECTED_NONE_FLAG;
}

bool IsA2dpConnected(void)
{
	return (g_bt_app.connected_profile & BT_CONNECTED_A2DP_FLAG);
}

bool IsHfpConnected(void)
{
	return (g_bt_app.connected_profile & BT_CONNECTED_HF_FLAG);
}

bool IsAvrcpConnected(void)
{
	return (g_bt_app.connected_profile & BT_CONNECTED_AVRCP_FLAG);
}

bool IsSppConnected(void)
{
	return (g_bt_app.connected_profile & BT_CONNECTED_SPP_FLAG);	
}

void SetScoConnectFlag(bool flag)
{
	g_bt_app.sco_conneted_flag = flag;
}

bool IsScoConneted(void)
{
	return g_bt_app.sco_conneted_flag;
}

void BtSoundTip(uint8_t sound_tip)
{
	switch(sound_tip)
	{
		case BT_SOUND_TIP_CONNECTED:
			MsgSend(MSG_BT_CONNECTED);
			break;
		case BT_SOUND_TIP_DISCONNECTED:
			MsgSend(MSG_BT_DISCONNECTED);
			break;
		case BT_SOUND_TIP_PHONE_CALL_INCOMING:
			MsgSend(MSG_BT_PHONE_CALL_INCOMING);
			break;
		default:
			break;
	}
}

#ifdef BT_HF_RECORD_FUNC
bool BtHfRecordStart(void)
{
	InitBtHfRecordPcm();

	InitBtHfRecordBuffer();

	SetBtRecordState(BT_RECORD_STATE_INITIALIZING);

	gBTRecorder = 1;
	if(RecorderInit(WAV_FORMAT) == TRUE)
	{
		if(GetHfpState() > BT_HFP_STATE_CONNECTED)
		{
			if(IsScoConneted())
				SetBtRecordState(BT_RECORD_STATE_RECORDING);
			else
				SetBtRecordState(BT_RECORD_STATE_PAUSE);
		}
		else
		{
			SetBtRecordState(BT_RECORD_STATE_EXITING);
		}
		return TRUE;
	}
	else
	{
		SetBtRecordState(BT_RECORD_STATE_NONE);
		return FALSE;
	}
}

void BtHfRecordExit(void)
{
	SetBtRecordState(BT_RECORD_STATE_EXITING);
}

void SetBtRecordState(uint8_t state)
{
	APP_DBG("[BT] SetBtRecordState = %d\n", state);
	g_bt_app.bt_record_state = state;
	if(state == BT_RECORD_STATE_RECORDING)
	{
		RecordState = RECORD_STATE_REC;
	}
	else if(state == BT_RECORD_STATE_NONE)
	{
		RecordState = RECORD_STATE_INIT;
	}
	else if(state == BT_RECORD_STATE_PAUSE)
	{
		RecordState = RECORD_STATE_REC_PAUSE;
	}
}

uint8_t GetBtRecordState(void)
{
	return g_bt_app.bt_record_state;
}
#endif
/////////////// hfp ////////////////////
DECLARE_SEMAPHORE(HfModeStateSema, 1);
static uint32_t history_mode_id = MODULE_ID_UNKNOWN;
void LockHfMode(void)
{
	OSDOWN(HfModeStateSema);
}
void UnLockHfMode(void)
{
	OSUP(HfModeStateSema);
}
bool EnterHfMode(void)
{
	APP_DBG("[BT] EnterHfMode current mode = %d\n", gSys.CurModuleID);
//	if(GetHfModeState() != BT_HF_MODE_STATE_NONE)
//	{
//		return FALSE;
//	}
	SetHfModeState(BT_HF_MODE_STATE_ENTERING);

	if(gSys.CurModuleID != MODULE_ID_BT_HF)
	{
		history_mode_id = gSys.CurModuleID;
		MsgSend(MSG_BT_HF_INTO_MODE);
	}

	return TRUE;
}

bool ExitHfMode(void)
{
	uint32_t NextModuleID = history_mode_id;

	APP_DBG("[BT] ExitHfMode history mode = %d\n", history_mode_id);

	/*防止重入*/
	if(GetHfModeState() == BT_HF_MODE_STATE_EXITING || GetHfModeState() == BT_HF_MODE_STATE_NONE)
	{
		return FALSE;
	}
//	DacSoftMuteSet(TRUE, TRUE);

#ifdef BT_HF_RECORD_FUNC
	/*退出蓝牙录音*/
	if(GetBtRecordState() == BT_RECORD_STATE_RECORDING || GetBtRecordState() == BT_RECORD_STATE_PAUSE)
	{
		BtHfRecordExit();
	}
#endif

	history_mode_id    = MODULE_ID_UNKNOWN;

	if(gSys.NextModuleID != MODULE_ID_STANDBY)
		gSys.NextModuleID = NextModuleID;


	// 此处代码保证了与GotoNextMode()一致
	if(gSys.NextModuleID == MODULE_ID_PLAYER_SD)
	{
		gSys.DiskType = SD_TYPE;
	}
	else if(gSys.NextModuleID == MODULE_ID_PLAYER_USB)
	{
		gSys.DiskType = USB_TYPE;
	}
	LockHfMode();
	SetHfModeState(BT_HF_MODE_STATE_EXITING);
	UnLockHfMode();
//	MsgSend(MSG_BT_HF_OUT_MODE);
	return TRUE;
}

uint32_t GetHistroyModeId(void)
{
	return history_mode_id;
}

bool IsHfModeEvnOK(void)
{
	return (GetHfpState() >= BT_HFP_STATE_INCOMING_CALL);
}
////////// phone number reminder //////
#ifdef BT_HF_NUMBER_REMIND
#define PHONE_NUMBER_SIZE			15

static bool	phone_number_is_reminding = FALSE;
static uint8_t phone_numbers_bak[PHONE_NUMBER_SIZE+1];
static uint8_t phone_numbers_len_bak = 0;
static uint8_t pos = 0;	

void RemindPhoneNumbers(uint8_t * phone_numbers, uint8_t phone_numbers_len)
{
	if(phone_numbers == NULL)
		return ;

	phone_numbers_len_bak = (phone_numbers_len <= PHONE_NUMBER_SIZE) ? phone_numbers_len : PHONE_NUMBER_SIZE;
	memset(phone_numbers_bak, 0, sizeof(phone_numbers_bak));
	memcpy(phone_numbers_bak, phone_numbers, phone_numbers_len_bak);
	
	pos = 0;
	phone_number_is_reminding = TRUE;
}

uint8_t GetNextNumber(void)
{
	if(pos < phone_numbers_len_bak)
	{
		return phone_numbers_bak[pos++];
	}
	else
	{
		return END_OF_PHONE_NUM;
	}
}

void RemindNumber(uint8_t number)
{
	if(number < '0' || number > '9')
		return;

	SoundRemind(SOUND_NUM0 + (number - '0'));
}

void StopRemindingPhoneNumbers(void)
{
	APP_DBG("StopRemindingPhoneNumbers...\n");
#ifdef FUNC_SOUND_REMIND
//	if(IsSoundRemindPlaying())
/*去掉此处的判断语句，为防止播报两个数字之间收到停止命令时，此句不执行*/
		StopSoundRemind();
#endif
	memset(phone_numbers_bak, 0, sizeof(phone_numbers_bak));
	pos = 0;
	phone_number_is_reminding = FALSE;
}

bool IsRemindingPhoneNumbers(void)
{
	return phone_number_is_reminding;
}
#endif
////////// hf timer ////////////////
static SW_TIMER 		sBtHfTimer;
static bool 			sBtHfTimerInited = FALSE;

static void BtHfTimerCB(void* Param)
{
	SW_TIMER* TimerHandle = (SW_TIMER*)Param;

	switch(GetHfpState())
	{
		case BT_HFP_STATE_NONE:
			break;

		case BT_HFP_STATE_CONNECTING:
			break;

		case BT_HFP_STATE_CONNECTED:
			break;

		case BT_HFP_STATE_INCOMING_CALL:
			break;

		case BT_HFP_STATE_OUTGOING_CALL:
			break;

		case BT_HFP_STATE_ATCTIVE_CALL:
		{
			uint32_t Ticks = GetPassTime(TimerHandle);
			Ticks /= 1000;
			APP_DBG("通话中：%02d:%02d:%02d", Ticks / 3600, (Ticks % 3600) / 60, Ticks % 60);
		}
			break;

		default:
			break;
	}
}

bool InitBtHfTimer(void)
{
	bool ret;

	ret = InitTimer(&sBtHfTimer, 200, BtHfTimerCB);

	if(ret)
	{
		sBtHfTimerInited = TRUE;
	}
	
	return ret;
}

bool StartBtHfTimer(void)
{
	return StartTimer(&sBtHfTimer);
}

void CheckBtHfTimer(void)
{
	CheckTimer(&sBtHfTimer);
}

bool DeinitBtHfTimer(void)
{
	bool ret;

	ret = DeinitTimer(&sBtHfTimer);
	
	if(ret)
	{
		sBtHfTimerInited = FALSE;
	}

	return ret;
}

bool IsBtHfTimerInited(void)
{
	return sBtHfTimerInited;
}

//////////////////// advanced avrcp /////////////////
#ifdef BT_ADVANCED_AVRCP_SUPPORT
void SetAdvAvrcpSupportFlag(bool flag)
{
	g_bt_app.adv_avrcp_support_flag = flag;
}

bool IsSupportAdvAvrcp(void)
{
	return g_bt_app.adv_avrcp_support_flag;
}

void SetAdvAvrcpPlayStatus(uint8_t status)
{
	g_bt_app.adv_avrcp_play_status = status;

	OSSchedulerSuspend(); //do not run scheduler
	// set play state
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		if(status == CurStatus_PLAYING)
			SetPlayState(PLAYER_STATE_PLAYING);
		else if(status == CurStatus_PAUSED|| status == CurStatus_STOPPED)
			SetPlayState(PLAYER_STATE_PAUSE);
	}
	OSSchedulerResume();//resume scheduler
}

uint8_t GetAdvAvrcpPlayStatus(void)
{
	return g_bt_app.adv_avrcp_play_status;
}
#endif


////////////////////bt led status////////////////////
#define LED_INTERVAL_TIME	250

SW_TIMER 		BtLedTimer;

static void BlueLedCb(void* Param)
{
	static uint8_t Flag = 0;
	static uint8_t TickCountMax, TickCount = 0;

	uint8_t	number_of_profile = 0;
	
	#define LED_WAITING_FOR_CONNECTING       8
	#define LED_CONNECTED_IDLE               2
	#define LED_PLAYING                      1

	if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
	{
		return;
	}

	number_of_profile = GetNumberOfConnectedProfile();
	if(number_of_profile > 0 && (BT_MODE_ACTIVE_STATUS == GetBtModeStatus()))
	{
		TickCountMax = LED_PLAYING;
	}
	else if(number_of_profile == 0)
	{
		TickCountMax = LED_WAITING_FOR_CONNECTING;
	}
	else
	{
		TickCountMax = LED_CONNECTED_IDLE;
	}

	if(Flag)//if it is on,then put off
	{
		TickCountMax = 1;
	}

	TickCount++;
	if(TickCount >= TickCountMax)
	{
		//SetBtLedBlue(!Flag);
		SetBtLedRed(!Flag);
		Flag = !Flag;

		TickCount = 0;
	}
}

void SetBtLedBlue(bool OnOffFlag) //tx,blue
{
	APP_DBG("[BT] SetBtLedBlue\n");
	
	if(OnOffFlag)
	{
		GpioSetRegOneBit(GPIO_A_OUT, GPIOA25);
	}
	else
	{
		GpioClrRegOneBit(GPIO_A_OUT, GPIOA25);
	}
}
void SetBtLedRed(uint8_t OnOffFlag) //Rx,red
{
//	APP_DBG("[BT] SetBtLedRed OnOffFlag = %d\n", OnOffFlag);
	
	if(OnOffFlag)
	{
		GpioSetRegOneBit(GPIO_A_OUT, GPIOA24);
	}
	else
	{
		GpioClrRegOneBit(GPIO_A_OUT, GPIOA24);
	}
}

bool StartBtLed(void)
{
	if(InitTimer(&BtLedTimer, LED_INTERVAL_TIME, BlueLedCb))
	{
		StartTimer(&BtLedTimer);
	}

	return TRUE;
}

void StopBtLed(void)
{
	StopTimer(&BtLedTimer);
	DeinitTimer(&BtLedTimer);
}

//////////////////// SBC Data State////////////////////

DECLARE_SEMAPHORE(SbcDataSema, 1);
SBC_DATA_STATE		sbc_data_state = SBC_DATA_STATE_NONE;
void SetSbcDataState(SBC_DATA_STATE state)
{
	OSDOWN(SbcDataSema);
	sbc_data_state = state;
	OSUP(SbcDataSema);
}
SBC_DATA_STATE GetSbcDataState(void)
{
	return sbc_data_state;
}

////////////////////BT Name & Address ////////////////////

int8_t BtAddr[6] = {3, 4, 1, 1, 5, 5}; //default btaddr
uint8_t BtDevName[MAX_BT_DEV_NAME_LENGTH] =    //default name
{
	//0xE4, 0xB8, 0x8A, 0xE6, 0xB5, 0xB7, 0xE5, 0xB1, 0xB1, 0xE6, 0x99, 0xAF, 0x42, 0x54, 0x0
	"MV Bluetooth"
};
extern bool GetUsrDefinedBtDevName(uint8_t* Name);

void GetBtName(uint8_t* Name, uint32_t NameMaxLen)
{
	uint32_t Offset, Size;

	if(GetUsrDefinedBtDevName(Name))
	{
		return;
	}

	if(SpiFlashConstGetInfo(*(uint32_t*)"LYMC", &Size, &Offset) >= 0)
	{
		if(Size >= NameMaxLen)
		{
			Size = NameMaxLen;
		}
		else
		{
			Name[Size] = 0x00;
		}

		SpiFlashConstDataRead(Name, Size, Offset);
		return;
	}

	return; //not found, use the default(not changed name)
}

void GetBtAddr(int8_t* BtAddr)
{
	int8_t* ADDR = (int8_t*)(0x7C0000 + 0x7F6C);
	uint32_t Offset, Size;

	//first get info from flash
	if(SpiFlashConstGetInfo(*(uint32_t*)"LYDZ", &Size, &Offset) >= 0)
	{
		if(Size == 6)
		{
			SpiFlashConstDataRead((uint8_t*)BtAddr, Size, Offset);
			return;
		}
	}

	//get otp
	//if all zero neans not found
	if(ADDR[0] | ADDR[1] | ADDR[2] | ADDR[3] | ADDR[4] | ADDR[5])
	{
		*BtAddr++ = ADDR[0];
		*BtAddr++ = ADDR[1];
		*BtAddr++ = ADDR[2];
		*BtAddr++ = ADDR[3];
		*BtAddr++ = ADDR[4];
		*BtAddr++ = ADDR[5];
		return;
	}

	//get s/n
	ADDR = (int8_t*)(0x7C0000 + 0x7FDA);
	if(ADDR[0] | ADDR[1] | ADDR[2] | ADDR[3] | ADDR[4] | ADDR[5])
	{
		*BtAddr++ = ADDR[5];
		*BtAddr++ = ADDR[4];
		*BtAddr++ = ADDR[3];
		*BtAddr++ = ADDR[2];
		*BtAddr++ = ADDR[1];
		*BtAddr++ = ADDR[0];
		return;
	}

	return;
}

uint8_t* GetMassTestDevAddr(void)
{
	GetBtAddr(BtAddr);

	return (uint8_t*)BtAddr;
}

//////////////////// bluetooth audio ////////////////////
static bool			g_bt_audio_play_flag = FALSE;

void BTAudioPlayStart(void)
{
	APP_DBG("[BT] BTAudioPlayStart\n");
	g_bt_audio_play_flag = TRUE;
}

void BTAudioPlayStop(void)
{
	APP_DBG("[BT] BTAudioPlayStop\n");
	g_bt_audio_play_flag = FALSE;
}

bool IsBtAudioPlay(void)
{
	return g_bt_audio_play_flag;
}

////////////////////reconnection////////////////////
#ifdef BT_RECONNECTION_FUNC
extern bool FlshGetAliveAddr(uint8_t* Addr);

static SW_TIMER BtReConnectTimerHandle;
static uint8_t 	sBtReConnectionTries = 0;
static uint8_t	sBtReConnectionCounts = 0;
static bool		bBtReConnectTimer = FALSE;
static uint8_t	sBtReconnectInternalTime = 0;

static void CancelBtReConnectionTimer(void)
{
//	APP_DBG("[BT] CancelBtReConnectionTimer\n");

	bBtReConnectTimer = FALSE;
	StopTimer(&BtReConnectTimerHandle);
	DeinitTimer(&BtReConnectTimerHandle);
}

static void BtTryToConnect(void)
{
	uint8_t	bdAddr[6] = {0};
	
	if(FlshGetAliveAddr(bdAddr))
	{
		APP_DBG("[BT] Last Dev:%x:%x:%x:%x:%x:%x\n", bdAddr[0], bdAddr[1], bdAddr[2], bdAddr[3], bdAddr[4], bdAddr[5]);
		BTConnect(bdAddr);
		sBtReConnectionTries++;
		APP_DBG("[BT] BtTryToConnect try times = %d\n", sBtReConnectionTries);
	}
	else
	{
		StopBtReConnection();
		APP_DBG("[BT] StartBtReConnection FlshGetAliveAddr failed\n");
	}
	
}

static void BtReConnectTimerCB(void* Param)
{
	APP_DBG("[BT] BtReConnectTimerCB\n");
	
	CancelBtReConnectionTimer();
	BtTryToConnect();
}

void StartBtReConnection(uint8_t reconnection_counts, uint8_t internal_time)
{
	APP_DBG("[BT] StartBtReConnection counts = %d, internal time = %d\n", reconnection_counts, internal_time);

	StopBtReConnection();

	if(reconnection_counts == 0)
	{		
		return;
	}

	sBtReConnectionTries = 0;
	sBtReConnectionCounts = reconnection_counts;
	sBtReconnectInternalTime = internal_time;
	BtTryToConnect();
}

bool BtReConnect(void)
{
	APP_DBG("[BT] BtReConnect will be started in %d seconds\n", sBtReconnectInternalTime);

	if(sBtReconnectInternalTime == 0)
	{
		BtTryToConnect();
	}
	else
	{
		if(InitTimer(&BtReConnectTimerHandle, 1000 * sBtReconnectInternalTime, BtReConnectTimerCB))
		{
			StartTimer(&BtReConnectTimerHandle);
			bBtReConnectTimer = TRUE;
		}
		else
		{
			APP_DBG("[BT] StartBtReConnection Failed\n");
			return FALSE;
		}
	}
	
	return TRUE;
}

void StopBtReConnection(void)
{
	APP_DBG("[BT] StopBtReConnection\n");
	
	if(!IsBtReConnectionFinish())
	{
		CancelBtReConnectionTimer();
		sBtReConnectionTries = 0;
		sBtReConnectionCounts = 0;
	}
}

void BtReConnectDetect(void)
{
	if(bBtReConnectTimer)
	{
		CheckTimer(&BtReConnectTimerHandle);
	}
}

bool IsBtReConnectionFinish(void)
{
	if(sBtReConnectionTries >= sBtReConnectionCounts)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#endif

#ifdef FAST_POWER_ON_OFF_BT

static uint8_t sBtScanPageState = BT_SCAN_PAGE_STATE_NONE;

static void SetScanPageState(uint8_t state)
{
	sBtScanPageState = state;
}

static uint8_t GetScanPageState(void)
{
	return sBtScanPageState;
}

void CheckScanPageFunc(void)
{
	static uint8_t			disconnect_count = 0;
	switch(sBtScanPageState)
	{
		case BT_SCAN_PAGE_STATE_CLOSING:
#ifdef BT_RECONNECTION_FUNC
			// If there is a reconnectiong process, stop it
			if(!IsBtReConnectionFinish())
			{
				StopBtReConnection();
			}
#endif
			// If there is a bt link, disconnect it
			if(GetCurBtConnectedFlag())
			{
				BTDisConnect();
				SetScanPageState(BT_SCAN_PAGE_STATE_DISCONNECTING);
				break;
			}

			if(BTSetScanDisable())
			{
				SetScanPageState(BT_SCAN_PAGE_STATE_DISABLE);
			}
			break;
			
		case BT_SCAN_PAGE_STATE_DISCONNECTING:
			if(disconnect_count > 50)	// wait 50 cycles (about 100ms)
			{
				if(BTSetScanDisable())
				{
					SetScanPageState(BT_SCAN_PAGE_STATE_DISABLE);
				}
				else
				{
					if(GetCurBtConnectedFlag())
					{
						BTDisConnect();
						SetScanPageState(BT_SCAN_PAGE_STATE_DISCONNECTING);
					}
				}
				disconnect_count = 0;
			}
			else
				disconnect_count++;
			break;
			
		case BT_SCAN_PAGE_STATE_DISABLE:
			// double check wether there is a bt link, if any, disconnect again
			if(GetCurBtConnectedFlag())
			{
				BTDisConnect();
				SetScanPageState(BT_SCAN_PAGE_STATE_DISCONNECTING);
			}
			break;

		case BT_SCAN_PAGE_STATE_OPENING:
			if(BTSetScanEnable())
			{
				SetScanPageState(BT_SCAN_PAGE_STATE_ENABLE);
			}
			break;
			
		case BT_SCAN_PAGE_STATE_ENABLE:
			break;
			
		default:
			break;
	}
}

void FastDisableBT(void)
{
	APP_DBG("[BT] FastDisableBT\n");
	SetScanPageState(BT_SCAN_PAGE_STATE_CLOSING);
}

void FastEnableBT(void)
{
	APP_DBG("[BT] FastEnableBT\n");
	SetScanPageState(BT_SCAN_PAGE_STATE_OPENING);
}

int8_t IsBTScanPageEnable(void)
{
	if(GetScanPageState() == BT_SCAN_PAGE_STATE_ENABLE)
	{
		return BT_SCAN_PAGE_ENABLE;
	}
	else if(GetScanPageState() == BT_SCAN_PAGE_STATE_DISABLE)
	{
		return BT_SCAN_PAGE_DISABLE;
	}
	else
	{
		return BT_SCAN_PAGE_UNKNOW;
	}
}
#endif

///////////////////////////HW Power On/Off ////////////////////////
#ifdef BT_RF_POWER_ON_OFF
static uint8_t bt_hw_power_cmd_state = BT_HW_POWER_CMD_NONE;
//#ifdef BT_POWERON_RECONNECTION
//extern bool power_on_reconnection_flag;
//#endif

void BtHwPowerOn(void)
{
	SetBtHwPowerCmdState(BT_HW_POWER_CMD_POWER_ON);
#ifdef BT_POWERON_RECONNECTION
	power_on_reconnection_flag = TRUE;
#endif
}

void BtHwPowerOff(void)
{
	SetBtHwPowerCmdState(BT_HW_POWER_CMD_POWER_OFF);
}

void SetBtHwPowerCmdState(uint8_t cmd_state)
{
	bt_hw_power_cmd_state = cmd_state;
}

uint8_t GetBtHwPowerCmdState(void)
{
	return bt_hw_power_cmd_state;
}

void UserBtRfPowerPlugIn(void)
{
    //example
//    EnableBtRfPowerVCC33(); //one may rewrite this function based on real hardware case.
}

void UserBtRfPowerPlugOut(void)
{
    //example
//    DisableBtRfPowerVCC33(); //one may rewrite this function based on real hardware case.
}

#endif

///For BT TEST ONLY/////////////////////////
///hci log function
uint8_t TestRxFunc(uint8_t*Data, int32_t Len)
{
    int32_t i;
    APP_DBG("RX:");
    
    if(Len > 40)Len = 40;
    
    for(i = 0 ; i < Len ; i ++)
    {
        APP_DBG("%02x ",Data[i]);
    }
    
    APP_DBG("\n");
    
    return 0;
}

uint8_t TestTxFunc(uint8_t*Data, int32_t Len)
{
    int32_t i;
    APP_DBG("TX:");
    
    if(Len > 40)Len = 40;
    
    for(i = 0 ; i < Len ; i ++)
    {
        APP_DBG("%02x ",Data[i]);
    }
    
    APP_DBG("\n");  
    
    return 0;
}

void SetBtTestHook(void)
{
    BTStackSetSendDataHook(TestTxFunc);
    BTStackSetReceivedDataHook(TestRxFunc);
}
////////////////////////////////////////////


