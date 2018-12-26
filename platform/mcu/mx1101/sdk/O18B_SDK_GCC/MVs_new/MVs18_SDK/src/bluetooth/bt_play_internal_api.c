/*
bt_play_internal_api.c
maintainer: Halley
*/

#include "type.h"

#include "app_config.h"
#include "fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "bt_play_internal_api.h"
#include "bt_device_type.h"
#include "bt_stack_api.h"
#include "bt_app_func.h"
#include "pcm_sync.h"
#include "sys_app.h"
#include "recorder_control.h"

#ifdef FUNC_BT_EN
void BTStackDataStatusCallBackFunc(uint8_t CBParameter,
                                   uint8_t* HistoryStatus/*This value will be AUTOMATICALLY reset to zero when new BT device connected*/)
{

	switch(CBParameter)
	{
		case BT_DS_A2DP_STREAM_START:
			APP_DBG("[BT] DS : BT_DS_A2DP_STREAM_START\n");
			SetBtModeStatus(BT_MODE_ACTIVE_STATUS);
			SetA2dpState(BT_A2DP_STATE_STREAMING);
			if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
			{
				SetPlayState(PLAYER_STATE_PLAYING);
			}
			//tbd...
			break;
		case BT_DS_A2DP_STREAM_SUSPEND:
			APP_DBG("[BT] DS : BT_DS_A2DP_STREAM_SUSPEND\n");
			SetBtModeStatus(BT_MODE_ACTIVE_STATUS);
			SetA2dpState(BT_A2DP_STATE_CONNECTED);
			if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
			{
				SetPlayState(PLAYER_STATE_PAUSE);
			}
			break;

#ifdef FUNC_BT_HF_EN
		case BT_DS_SCO_STREAM_START:
			//tbd...
			APP_DBG("[BT] DS : BT_DS_SCO_STREAM_START\n");
			SetScoConnectFlag(TRUE);
#ifdef BT_HF_NUMBER_REMIND
			if(IsRemindingPhoneNumbers())
				StopRemindingPhoneNumbers();
#endif
#ifdef BT_HF_RECORD_FUNC
			if(GetBtRecordState() == BT_RECORD_STATE_PAUSE)
			{
				RecordResume(WAV_FORMAT);
				SetBtRecordState(BT_RECORD_STATE_RECORDING);
			}
#endif
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
 			break;

		case BT_DS_CALL_SCO_STREAM_PAUSE:
			//tbd...
			APP_DBG("[BT] DS : BT_DS_CALL_SCO_STREAM_PAUSE\n");
			SetScoConnectFlag(FALSE);
#ifdef BT_HF_RECORD_FUNC
			if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
			{
				RecordPause(WAV_FORMAT);
				SetBtRecordState(BT_RECORD_STATE_PAUSE);
			}
#endif
			if(GetHfpState() == BT_HFP_STATE_ATCTIVE_CALL || GetHfpState() == BT_HFP_STATE_INCOMING_CALL || GetHfpState() == BT_HFP_STATE_OUTGOING_CALL )
			{
				break;
			}

			if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
			{
				ExitHfMode();
			}

 			break;

		case BT_DS_CALL_START:
			//tbd...
			APP_DBG("[BT] DS : BT_DS_CALL_START\n");
			break;

		case BT_DS_CALL_END:
			//tbd...
			//pls check "IsISOHistoryFlag"
			APP_DBG("[BT] DS : BT_DS_CALL_END\n");
			break;

		case BT_DS_CALL_AEC_SUPPORTED:
			APP_DBG("[BT] DS : BT_DS_CALL_AEC_SUPPORTED\n");
			break;
#endif//FUNC_BT_HF_EN

		case BT_DS_SNIFF_MODE:
			APP_DBG("[BT] DS : BT_DS_SNIFF_MODE\n");
			SetBtModeStatus(BT_MODE_SNIFF_STATUS);
			break;

		case BT_DS_ACTIVE_MODE:
			APP_DBG("[BT] DS : BT_DS_ACTIVE_MODE\n");
			SetBtModeStatus(BT_MODE_ACTIVE_STATUS);

			break;

		default:
			break;
	}
}

#else

void BTStackDataStatusCallBackFunc(uint8_t CBParameter,
                                   uint8_t* HistoryStatus/*This value will be AUTOMATICALLY reset to zero when new BT device connected*/)
{
	
}

#endif

