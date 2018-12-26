///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_app_ui_callback.c
//  maintainer: Halley
//BT application UI callback functions. This function is called by the BT stack.
//One can modify/implement this function but MUST NOT change the interface.
//
//用户必需参考下面的实现方式按需实现本函数，蓝牙协议栈内会调用此函数。
//

#include "type.h" 
#include "pcm_sync.h"
#include "app_config.h"
#include "fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "audio_adc.h"
#include "sys_app.h"
#include "sys_vol.h"
#include "bt_app_ui_callback.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "bt_stack_api.h"

#ifdef FUNC_BT_EN

#ifdef MASS_PRODUCTION_TEST_FUNC
static bool BtReadyFlag = FALSE;
void OtherMassTestFunc(void);
bool GetMassTestFlag(void);
#endif

#ifdef BT_RECONNECTION_FUNC
#ifdef BT_POWERON_RECONNECTION
bool power_on_reconnection_flag = TRUE;
#endif
//extern bool BtReConnect(void);
#endif

#ifdef AUTO_MV_BT_TEST_FUNC
static unsigned char MvBtAutoTestFlag = 0; //1 means enable auto test, one MAY implement oneself to enable it.
extern bool FlshUpdateBtLatestDevAddr(uint8_t* BtAddr);
#endif

void BTStackStatusCallBackFunc(uint8_t CBParameter)
{
	switch(CBParameter)
	{
		case BT_UI_CB_STATUS_IDLE:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_IDLE\n");
#ifdef MASS_PRODUCTION_TEST_FUNC
			if(!BtReadyFlag)
			{
				BtReadyFlag = TRUE;

				if(GetMassTestFlag())
				{
					BTDoDeviceInquiry(10); //for i/o connection test function ONLY
					break;
				}
			}
#endif
			APP_DBG("[BT] WaitForPairing/Connection ... \n");

#ifdef FAST_POWER_ON_OFF_BT
			if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
			{
			#ifdef BT_POWERON_RECONNECTION
				power_on_reconnection_flag = FALSE;
			#endif
				FastDisableBT();
			}
#endif
#ifdef BT_RF_POWER_ON_OFF
			if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
			{
			#ifdef BT_POWERON_RECONNECTION
				power_on_reconnection_flag = FALSE;
			#endif
			}
#endif
#ifdef BT_POWERON_RECONNECTION
			if(power_on_reconnection_flag)
			{
#ifdef AUTO_MV_BT_TEST_FUNC
				if(MvBtAutoTestFlag)
				{
					BTDoTestModeDeviceInquiry(30);
					DBG("[BT] MvBtAutoTestFlag = 1 and start inq.\n");
				}
				else
#endif
					StartBtReConnection(BT_POR_TRY_COUNTS, BT_POR_INTERNAL_TIME); //start bt reconnection
				power_on_reconnection_flag = FALSE;
			}
			else
			{
				if(!IsBtReConnectionFinish())
				{
					BtReConnect();
				}
			}
#endif
			break;

		case BT_UI_CB_STATUS_PAIRED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_PAIRED\n");
			break;

		case BT_UI_CB_STATUS_A2DP_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_A2DP_CONNECTED\n");
#ifdef BT_RECONNECTION_FUNC
			StopBtReConnection();
#endif
			if(!IsA2dpConnected())
			{
				APP_DBG("[BT] A2DP Connected\n");
				ConnectedProfile(BT_CONNECTED_A2DP_FLAG);
			}
			BTAudioPlayStart();
			SetA2dpState(BT_A2DP_STATE_CONNECTED);
			break;

		case BT_UI_CB_STATUS_PLAY_CONTROL_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_PLAY_CONTROL_CONNECTED\n");

			if(!IsAvrcpConnected())
			{
				ConnectedProfile(BT_CONNECTED_AVRCP_FLAG);
			}
			SetAvrcpState(BT_AVRCP_STATE_CONNECTED);
			break;

		case BT_UI_CB_STATUS_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_DISCONNECTED\n");
			if(IsA2dpConnected())
			{
				DisconnectProfile(BT_CONNECTED_A2DP_FLAG);

				APP_DBG("[BT] A2DP DisConnected\n");
				BTAudioPlayStop();
				if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
				{
					SetPlayState(PLAYER_STATE_PAUSE);
				}

				SetA2dpState(BT_A2DP_STATE_NONE);
			}

			if(IsAvrcpConnected())
			{
				DisconnectProfile(BT_CONNECTED_AVRCP_FLAG);
				SetAvrcpState(BT_AVRCP_STATE_NONE);
			}
#ifdef BT_ADVANCED_AVRCP_SUPPORT
			SetAdvAvrcpSupportFlag(FALSE);
			SetAdvAvrcpPlayStatus(CurStatus_ERROR);
#endif
#ifdef BT_RECONNECTION_FUNC
			if(!IsBtReConnectionFinish()) 
			{
				BtReConnect();
			}
			else 
			{
				StopBtReConnection();
			}
#endif
			break;

		case BT_UI_CB_STATUS_ERROR:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_ERROR\n");
			break;

		case BT_UI_CB_STATUS_PAGE_TIME_OUT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_PAGE_TIME_OUT\n");
#ifdef BT_RECONNECTION_FUNC
			if(!IsBtReConnectionFinish())
			{
				BtReConnect();
			}
			else
			{
				StopBtReConnection();
			}
#endif
			break;

#ifdef FUNC_BT_HF_EN
////HF////////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_HF_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_DISCONNECTED\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] HF disconnected\n");

				DisconnectProfile(BT_CONNECTED_HF_FLAG);

				if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
				{
					ExitHfMode();
				}
			}
#ifdef BT_RECONNECTION_FUNC
			if(!IsBtReConnectionFinish())
			{
				StopBtReConnection();
			}
#endif
#ifdef BT_HF_NUMBER_REMIND
			if(IsRemindingPhoneNumbers())
				StopRemindingPhoneNumbers();
#endif
			SetHfpState(BT_HFP_STATE_NONE);
			break;

		case BT_UI_CB_STATUS_HF_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CONNECTED\n");
#ifdef BT_RECONNECTION_FUNC
			StopBtReConnection();
#endif
			if(!IsHfpConnected())
			{
				APP_DBG("[BT] HF connected \n");
				ConnectedProfile(BT_CONNECTED_HF_FLAG);
			}

			BTQueryPhonebooks();

			if(GetHfpState() > BT_HFP_STATE_CONNECTED)
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			else
				SetHfpState(BT_HFP_STATE_CONNECTED);
			break;

		case BT_UI_CB_STATUS_HF_INCOME_RING:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_INCOME_RING\n");
			if(IsHfpConnected())
			{
				SetHfpState(BT_HFP_STATE_INCOMING_CALL);
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_NONE:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_NONE\n");
			if(IsHfpConnected())
			{
				switch(GetHfpState())
				{
					case BT_HFP_STATE_INCOMING_CALL:
					case BT_HFP_STATE_OUTGOING_CALL:
						if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
						{
							ExitHfMode();
						}
#ifdef BT_HF_NUMBER_REMIND
						if(IsRemindingPhoneNumbers())
							StopRemindingPhoneNumbers();
#endif
						SetHfpState(BT_HFP_STATE_CONNECTED);
						break;
					case BT_HFP_STATE_ATCTIVE_CALL:
					default:
						break;
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_INCOMING:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_INCOMING\n");
			SetHfpState(BT_HFP_STATE_INCOMING_CALL);
			BtSoundTip(BT_SOUND_TIP_PHONE_CALL_INCOMING);
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_OUTCALL_ALERT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_OUTCALL_ALERT\n");
			SetHfpState(BT_HFP_STATE_OUTGOING_CALL);
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_INCOME_CALL_NUM:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_INCOME_CALL_NUM\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] UI : Call number = %s\n", BTGetCurInCallPhoneNum());
				SetHfpState(BT_HFP_STATE_INCOMING_CALL);
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}

#ifdef BT_HF_NUMBER_REMIND
				if(!IsRemindingPhoneNumbers() && !IsScoConneted())
				{
					RemindPhoneNumbers(BTGetCurInCallPhoneNum(), strlen((char*)BTGetCurInCallPhoneNum()));
				}
#endif
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_CONNECTED\n");
			SetHfpState(BT_HFP_STATE_ATCTIVE_CALL);
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
				else
				{
					if((PcmSyncAecGetStatus() == PCMSYNC_AEC_NONE) || (PcmSyncAecGetStatus() == PCMSYNC_AEC_MV))
					{
						BTCallDisableAEC();
						APP_DBG("[BT] BTCallDisableAEC\n");
					}
				}

				// start bt hf timer
				if(IsBtHfTimerInited())
				{
					StartBtHfTimer();
				}
#ifdef BT_HF_NUMBER_REMIND
				if(IsRemindingPhoneNumbers())
					StopRemindingPhoneNumbers();
#endif
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_DISCONNECTED\n");
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
				{
					ExitHfMode();
				}
#ifdef BT_HF_NUMBER_REMIND
				if(IsRemindingPhoneNumbers())
					StopRemindingPhoneNumbers();
#endif
				SetHfpState(BT_HFP_STATE_CONNECTED);
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_OUT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_OUT\n");
			SetHfpState(BT_HFP_STATE_OUTGOING_CALL);
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;
		case BT_UI_CB_STATUS_HF_PHONE_SPEAKER_VOL:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_PHONE_SPEAKER_VOL vol = %d\n", BtGetRemoteSpeakerVol());
			if(gSys.CurModuleID == MODULE_ID_BT_HF && IsHfpConnected())
			{
				SetBtHfVol(BtGetRemoteSpeakerVol());
			}
			break;
#endif// FUNC_BT_HF_EN
		case BT_UI_CB_STATUS_GET_NAME_PAGE_TIMEOUT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_GET_NAME_PAGE_TIMEOUT\n");
		case BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME : %s\n", BTGetRemoteDevName());
#ifdef AUTO_MV_BT_TEST_FUNC
			if(strstr((const char*)BTGetRemoteDevName(),"MVBTTester01")) /*NOICE: [MVBTTester01] is the KEY string for MV BT tester*/
			{
				BTEnableBluetoothTestMode();
			}

			if(MvBtAutoTestFlag && (strstr((const char*)BTGetRemoteDevName(),"MVBTTester01"))) /*NOICE: [MVBTTester01] is the KEY string for MV BT tester*/
			{              
				APP_DBG("MVBTTester01 is found.\n");
				BTEnableBluetoothTestMode();
				FlshUpdateBtLatestDevAddr(BTGetCurDeviceNamesAddr());
				MvBtAutoTestFlag = 0;//clear
				BTConnect(BTGetCurDeviceNamesAddr());              
			}
			else
			{
				if(MvBtAutoTestFlag)
				{
					//not found MV tester
					//inq another one
					BTDoTestModeDeviceInquiry(30);
					APP_DBG("inq once more.\n");
				}
			}
#endif
			break;
///////////////////// phonebook ////////////////////////////////////////
		case BT_UI_CB_STATUS_PB_SUPPORTED:
			APP_DBG("supported PB bitmap = 0x%02x\n",BTGetSupportedPhonebookStorageType());
			APP_DBG("bit0-7: DC FD LD MC, ME MT RC SM\n\n");
//			BTGetPBListEntry(1,10);//One may select phonebook first, I am here just load the default PB's data for testing purpose.

		break;

		case BT_UI_CB_STATUS_PB_ENTRY_SIZE:
			{
				CurPhonebookSize *info;
				info = (CurPhonebookSize*)BtGetCurrentPbResponseData();
				APP_DBG("pb size==>%d\n",info->IdxLast);
			}
			break;

		case BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA:
			{
				OnePhonebookEntry *data;
				data = (OnePhonebookEntry*)BtGetCurrentPbResponseData();
				APP_DBG("call list==>[%d]%s\n",data->Idx,data->PhoneNumber);
			}
			break;

		case BT_UI_CB_STATUS_CLCC_RESULT:
			APP_DBG("cur call list ==> %s\n",BtGetCurCallListNumberResponseData());
			break;

////////SPP//////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_SPP_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_SPP_CONNECTED\n");
#ifdef BT_RECONNECTION_FUNC
			StopBtReConnection();
#endif
			if(!IsSppConnected())
			{
				APP_DBG("[BT] SPP connected \n");
				ConnectedProfile(BT_CONNECTED_SPP_FLAG);
			}
			break;

		case BT_UI_CB_STATUS_SPP_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_SPP_DISCONNECTED\n");
			if(IsSppConnected())
			{
				APP_DBG("[BT] SPP Disconnected \n");

				DisconnectProfile(BT_CONNECTED_SPP_FLAG);
			}
			break;

		case BT_UI_CB_STATUS_INQUIRY_RESULT:
			//for i/o connection test function ONLY
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_INQUIRY_RESULT\n");
#ifdef MASS_PRODUCTION_TEST_FUNC
			if(GetMassTestFlag())
			{
				OtherMassTestFunc();
			}
#endif
			break;

		case BT_UI_CB_STATUS_CONNECTION_LOST:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_CONNECTION_LOST\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] HF disconnected\n");

				DisconnectProfile(BT_CONNECTED_HF_FLAG);

				if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
				{
					ExitHfMode();
				}
				
				SetHfpState(BT_HFP_STATE_NONE);
			}

			if(IsA2dpConnected())
			{
				DisconnectProfile(BT_CONNECTED_A2DP_FLAG);

				APP_DBG("[BT] A2DP DisConnected\n");
				BTAudioPlayStop();
				if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
				{
					SetPlayState(PLAYER_STATE_PAUSE);
				}

				SetA2dpState(BT_A2DP_STATE_NONE);
			}

			if(IsAvrcpConnected())
			{
				DisconnectProfile(BT_CONNECTED_AVRCP_FLAG);
				SetAvrcpState(BT_AVRCP_STATE_NONE);
			}
			
			if(IsSppConnected())
			{
				APP_DBG("[BT] SPP Disconnected \n");

				DisconnectProfile(BT_CONNECTED_SPP_FLAG);
			}
			DisconnectAllProfile();
#ifdef BT_BB_LOST_RECONNECTION
			StartBtReConnection(BT_BLR_TRY_COUNTS, BT_BLR_INTERNAL_TIME);
#endif
			break;
////Hid///////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_HID_OUT_DATA_IND:
			APP_DBG("[BT] BT_UI_CB_STATUS_HID_OUT_DATA_IND \n");
			break;
		case BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN:
			APP_DBG("[BT] BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN OK\n");
			break;
		case BT_UI_CB_STATUS_BT_MFi_DATA_IND:
			APP_DBG("[BT] BT_UI_CB_STATUS_BT_MFi_DATA_IND \n");
			{
				uint8_t *Data;
				int32_t Len;
				BtGetMfiDataInfo(&Data,&Len);//get the incoming data
			}
			break;
		case BT_UI_CB_STATUS_BT_MFi_DATA_SENT:
			APP_DBG("[BT] BT_UI_CB_STATUS_BT_MFi_DATA_SENT\n");//data has just sent, so one can send another package if any.
			break;

//////////////////// advanced avrcp //////////////////////////
#ifdef BT_ADVANCED_AVRCP_SUPPORT
		case BT_UI_CB_STATUS_ADV_AVRCP_SUPPORTED:
			APP_DBG("BT_UI_CB_STATUS_ADV_AVRCP_SUPPORTED\n");
			SetAdvAvrcpSupportFlag(TRUE);
			break;
		case BT_UI_CB_STATUS_ADV_AVRCP_NOT_SUPPORTED:
			APP_DBG("BT_UI_CB_STATUS_ADV_AVRCP_NOT_SUPPORTED\n");
			SetAdvAvrcpSupportFlag(FALSE);
			break;
		case BT_UI_CB_STATUS_PLAYING_STATUS:
			{
				AdvAvrcpPlayStatus *CurStatus;
				CurStatus = (AdvAvrcpPlayStatus *)BtGetAdvAvrcpResponse();
				if(CurStatus)
				{
					SetAdvAvrcpPlayStatus(CurStatus->CurStatus);

					switch(CurStatus->CurStatus)
					{
						case CurStatus_STOPPED:
							break;
							
						case CurStatus_PLAYING:
							APP_DBG("Playing > %02d:%02d / %02d:%02d \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						case CurStatus_PAUSED:
							APP_DBG("Paused || %02d:%02d / %02d:%02d  \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						case CurStatus_FWD_SEEK:
							APP_DBG("FF >> %02d:%02d / %02d:%02d  \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						case CurStatus_REV_SEEK:
							APP_DBG("FB << %02d:%02d / %02d:%02d  \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						default:
							break;
					}
				}
			}
			break;
		case BT_UI_CB_STATUS_TRACK_INFO:
			APP_DBG("BT_UI_CB_STATUS_TRACK_INFO\n");
			break;
#endif
///////////////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_BB_NO_RESPONSE:
			APP_DBG("[BT] BT_UI_CB_STATUS_BB_NO_RESPONSE, One MAY reboot...\n");
			break;
			
        case BT_UI_CB_STATUS_RSSI_LINK_QUALITY_IND:
            APP_DBG("rssi = %d dB; linkq = %d.\n", BtGetRssiValue(),BtGetLinkqualityValue());
            break;

		default:
			APP_DBG("[BT] UI : unsupported code = 0x%02x\n", CBParameter);
			break;

	}
}

#else

void BTStackStatusCallBackFunc(uint8_t CBParameter)
{
	
}

#endif


