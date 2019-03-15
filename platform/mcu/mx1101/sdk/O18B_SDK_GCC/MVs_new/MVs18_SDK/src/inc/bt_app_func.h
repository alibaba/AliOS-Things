///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_app_func.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __BT_APP_FUNC_H__
#define __BT_APP_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define BT_CONNECTED_NONE_FLAG		(0)
#define BT_CONNECTED_A2DP_FLAG		(1)
#define BT_CONNECTED_HF_FLAG		(1 << 1)
#define BT_CONNECTED_SPP_FLAG		(1 << 2)
#define BT_CONNECTED_AVRCP_FLAG		(1 << 3)

#ifdef FAST_POWER_ON_OFF_BT
#define BT_SCAN_PAGE_ENABLE			1
#define BT_SCAN_PAGE_DISABLE		0
#define BT_SCAN_PAGE_UNKNOW			-1
#endif

enum
{
	BT_ACTIVE_NONE = 0,
	BT_ACTIVE_HFP,
	BT_ACTIVE_A2DP
};

enum
{
	BT_HFP_STATE_NONE = 0,
	BT_HFP_STATE_CONNECTING,
	BT_HFP_STATE_CONNECTED,
	BT_HFP_STATE_INCOMING_CALL,
	BT_HFP_STATE_OUTGOING_CALL,
	BT_HFP_STATE_ATCTIVE_CALL
};

enum
{
	BT_A2DP_STATE_NONE = 0,
	BT_A2DP_STATE_CONNECTING,
	BT_A2DP_STATE_CONNECTED,
	BT_A2DP_STATE_STREAMING
};

enum
{
	BT_AVRCP_STATE_NONE = 0,
	BT_AVRCP_STATE_CONNECTING,
	BT_AVRCP_STATE_CONNECTED
};

enum
{
	BT_MODE_NONE_STATUS = 0,
	BT_MODE_ACTIVE_STATUS,
	BT_MODE_SNIFF_STATUS
};

enum
{
	BT_HF_MODE_STATE_NONE = 0,
	BT_HF_MODE_STATE_ENTERING,
	BT_HF_MODE_STATE_ACTIVE,
	BT_HF_MODE_STATE_EXITING
};

enum
{
	BT_SOUND_TIP_CONNECTED,
	BT_SOUND_TIP_DISCONNECTED,
	BT_SOUND_TIP_PHONE_CALL_INCOMING
};

#ifdef BT_HF_RECORD_FUNC
enum
{
	BT_RECORD_STATE_NONE,
	BT_RECORD_STATE_INITIALIZING,
	BT_RECORD_STATE_RECORDING,
	BT_RECORD_STATE_PAUSE,
	BT_RECORD_STATE_EXITING
};
#endif

#ifdef FAST_POWER_ON_OFF_BT
enum
{
	BT_SCAN_PAGE_STATE_NONE = 0,
	BT_SCAN_PAGE_STATE_CLOSING,
	BT_SCAN_PAGE_STATE_DISCONNECTING,
	BT_SCAN_PAGE_STATE_DISABLE,
	BT_SCAN_PAGE_STATE_OPENING,
	BT_SCAN_PAGE_STATE_ENABLE
};
#endif

#ifdef BT_RF_POWER_ON_OFF
enum
{
	BT_HW_POWER_CMD_NONE,
	BT_HW_POWER_CMD_POWER_ON,
	BT_HW_POWER_CMD_POWER_OFF,
	BT_HW_POWER_CMD_POWER_OFF_PENDING
};
#endif

typedef enum _SBC_DATA_STATE{
	SBC_DATA_STATE_NONE = 0,
	SBC_DATA_STATE_NEED_MORE,
	SBC_DATA_READY
} SBC_DATA_STATE;

typedef struct
{
//	uint8_t			active_profile;
	
	uint8_t			hfp_state;
	uint8_t			a2dp_state;
	uint8_t			avrcp_state;
	uint8_t			mode_status;
	
	uint32_t		connected_profile;
	bool			sco_conneted_flag;
	uint8_t			hf_mode_state;
#ifdef BT_HF_RECORD_FUNC
	uint8_t			bt_record_state;
#endif
#ifdef BT_ADVANCED_AVRCP_SUPPORT
	bool			adv_avrcp_support_flag;
	uint8_t			adv_avrcp_play_status;
#endif
} BT_CONTEXT_DATA;

// misc
void BtSetLinkStatus(bool Status);
bool IsBtLink(void);

// bt app func

void InitBtApp(void);

void SetBtModeStatus(uint8_t status);

uint8_t GetBtModeStatus(void);

//void SetActiveProfile(uint8_t active_profile);

//uint8_t GetActiveProfile(void);

void SetHfpState(uint8_t hfp_state);

uint8_t GetHfpState(void);

void SetA2dpState(uint8_t a2dp_state);

uint8_t GetA2dpState(void);

void SetAvrcpState(uint8_t avrcp_state);

uint8_t GetAvrcpState(void);

#ifdef BT_ADVANCED_AVRCP_SUPPORT
void SetAdvAvrcpSupportFlag(bool flag);

bool IsSupportAdvAvrcp(void);

void SetAdvAvrcpPlayStatus(uint8_t status);

uint8_t GetAdvAvrcpPlayStatus(void);
#endif

void SetHfModeState(uint8_t state);

uint8_t GetHfModeState(void);

void ConnectedProfile(uint32_t profile);

#ifdef BT_HF_RECORD_FUNC
bool BtHfRecordStart(void);

void BtHfRecordExit(void);

void SetBtRecordState(uint8_t state);

uint8_t GetBtRecordState(void);
#endif
/*
* Get the number of connected profile
*/
uint8_t GetNumberOfConnectedProfile(void);

void DisconnectProfile(uint32_t profile);

void DisconnectAllProfile(void);

bool IsA2dpConnected(void);

bool IsHfpConnected(void);

bool IsAvrcpConnected(void);

bool IsSppConnected(void);

void SetScoConnectFlag(bool flag);

bool IsScoConneted(void);

/*
* Bt sound tip
*/
void BtSoundTip(uint8_t sound_tip);

// bt hf function
void LockHfMode(void);
void UnLockHfMode(void);
bool EnterHfMode(void);
bool ExitHfMode(void);
uint32_t GetHistroyModeId(void);
//bool IsHfModeEvnOK(void);

/*
* phone number reminder
*/
#ifdef BT_HF_NUMBER_REMIND
#define END_OF_PHONE_NUM					0xFF

void RemindPhoneNumbers(uint8_t * phone_numbers, uint8_t phone_numbers_len);

uint8_t GetNextNumber(void);

void RemindNumber(uint8_t number);

void StopRemindingPhoneNumbers(void);

bool IsRemindingPhoneNumbers(void);
#endif

// bt hf timer

bool InitBtHfTimer(void);

bool StartBtHfTimer(void);

void CheckBtHfTimer(void);

bool DeinitBtHfTimer(void);

bool IsBtHfTimerInited(void);

// bt hf volume
void SetBtHfVolume(uint8_t vol);
uint8_t GetBtHfVolume(void);

// bt led status
void SetBtLedBlue(bool OnOffFlag); //tx,blue
void SetBtLedRed(bool OnOffFlag); //Rx,red
bool StartBtLed(void);
void StopBtLed(void);

// bt sbc data state
void SetSbcDataState(SBC_DATA_STATE state);

SBC_DATA_STATE GetSbcDataState(void);


// bt Name & Address
#define MAX_BT_DEV_NAME_LENGTH   50 /*DO NOT Modify this value*/
extern int8_t BtAddr[6];
extern uint8_t BtDevName[MAX_BT_DEV_NAME_LENGTH];
extern SW_TIMER 		BtLedTimer;

extern bool power_on_reconnection_flag;
extern bool BtReConnect(void);
    
void GetBtName(uint8_t* Name, uint32_t NameMaxLen);
void GetBtAddr(int8_t* BtAddr);
uint8_t* GetMassTestDevAddr(void);


// bluetooth audio
void BTAudioPlayStart(void);
void BTAudioPlayStop(void);
bool IsBtAudioPlay(void);

// reconnection
#ifdef BT_RECONNECTION_FUNC
/*
* Start a reconnection to last bt device and set the number of tries with "reconnection_count".
*/
void StartBtReConnection(uint8_t reconnection_count, uint8_t internal_time);

/*
* 
*/
void StopBtReConnection(void);

/*
* check timer
*/
void BtReConnectDetect(void);

/*
* Check whether reconnection is finished 
*/
bool IsBtReConnectionFinish(void);

#endif

#ifdef FAST_POWER_ON_OFF_BT

void FastDisableBT(void);

void FastEnableBT(void);
/*
* Get the BT scan & page status
*
* Returns :
*	BT_SCAN_PAGE_ENABLE - means BT is discoverable & connectable
*	BT_SCAN_PAGE_DISABLE - means BT can not be discovered & can not be connected
*	BT_SCAN_PAGE_UNKNOW - means BT is in a middle statement
*
* Comments:
*	When it returns BT_SCAN_PAGE_UNKNOW, it means the BT is in the process swithing 
*	mode from enable to disable or in opposite process. When get the value 
*	BT_SCAN_PAGE_UNKNOW, one should check this value again.
*/
int8_t IsBTScanPageEnable(void);

#endif

#ifdef BT_RF_POWER_ON_OFF
void BtHwPowerOn(void);

void BtHwPowerOff(void);

void SetBtHwPowerCmdState(uint8_t cmd_state);

uint8_t GetBtHwPowerCmdState(void);

void UserBtRfPowerPlugIn(void);

void UserBtRfPowerPlugOut(void);
#endif

///For BT TEST ONLY/////////////////////////
void SetBtTestHook(void);


void CheckScanPageFunc(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__BT_APP_FUNC_H__*/

