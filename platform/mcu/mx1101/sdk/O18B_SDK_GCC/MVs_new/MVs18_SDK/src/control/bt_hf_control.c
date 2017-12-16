///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_hf_control.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "sys_app.h"
//#include "player_control.h"
#include "bt_stack_api.h"
#include "bt_control_api.h"
#include "bt_app_func.h"
#include "pcm_fifo.h"
#include "dev_detect_driver.h"
#include "sd_card.h"
#include "dac.h"
#include "pcm_sync.h"
#include "sys_vol.h"
#include "recorder_control.h"
#include "mixer.h"
#include "sound_remind.h"

#ifdef FUNC_BT_HF_EN

bool BtHfControl(void)
{
	uint16_t 	Msg = 0;
	bool		bt_hf_audio_enable = FALSE;
	
	APP_DBG("[BT] BtHfControl Entry\n");
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
	LockHfMode();
	if(GetHfModeState() == BT_HF_MODE_STATE_ENTERING)
	{
		SetHfModeState(BT_HF_MODE_STATE_ACTIVE);
	}
	UnLockHfMode();
	InitBtHfTimer();

	/*如果进入HF之前，手机就处于电话状态，启动通话计时器*/
	if(GetHfpState() == BT_HFP_STATE_ATCTIVE_CALL && IsBtHfTimerInited())
	{
		StartBtHfTimer();
	}

//	while(Msg != MSG_COMMON_CLOSE)
	while(GetHfModeState() != BT_HF_MODE_STATE_EXITING)
	{
		CheckBtHfTimer();

#ifdef BT_HF_NUMBER_REMIND
		/*来电报号*/
		if(IsRemindingPhoneNumbers() && !IsScoConneted()&& !IsSoundRemindPlaying())
		{
			uint8_t			number;
			number = GetNextNumber();
			if(number != END_OF_PHONE_NUM)
			{
				RemindNumber(number);
			}
			else
			{
				StopRemindingPhoneNumbers();
			}
		}

		/*
		* 为防止停止播报号码导致SoundRemind停止标志未被清理
		*/
		if(!IsRemindingPhoneNumbers())
		{
			ResetSoundRemindStopFlag();
		}
#endif
		/*SCO建立后，打开通路。*/
		if(!bt_hf_audio_enable && IsScoConneted())
		{
			PcmSyncInit(MV_MCU_AS_PCMSYNC_CLK_SLAVE);
			WaitMs(50); // 消除POP音
			CodecDacMuteSet(FALSE, FALSE);
			/*设置HF音量*/
			SetBtHfVol(GetBtHfVol());
			bt_hf_audio_enable = TRUE;
		}

#ifdef BT_HF_RECORD_FUNC
		/* 录音写盘，每2K数据写一次盘 */
		if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
		{
			EncodedDataBufferToFile(2048);
			Msg = MsgRecv(2);
		}
		else if(GetBtRecordState() == BT_RECORD_STATE_EXITING)
		{
			/* 将最后buff中的数据写入盘*/
//			extern uint32_t EncodedBufDataLen;
			
			APP_DBG("Exiting BT Recording\n");
			EncodedDataBufferToFile(EncodedBufDataLen);
			/*写好退出录音*/
			RecoderExit(WAV_FORMAT);
			SetBtRecordState(BT_RECORD_STATE_NONE);
			Msg = MsgRecv(5);
		}
		else
#endif
		{
			Msg = MsgRecv(20);
		}

		switch(Msg)
		{
			case MSG_STOP:
				switch(GetHfpState())
				{
					case BT_HFP_STATE_INCOMING_CALL:
					case BT_HFP_STATE_OUTGOING_CALL:
						BTInCallReject();
						break;

					case BT_HFP_STATE_ATCTIVE_CALL:
						BTCallHangUp();
						break;

					default:
						break;
				}
				break;
				
			case MSG_PLAY_PAUSE: 
				switch(GetHfpState())
				{
					case BT_HFP_STATE_INCOMING_CALL:
						BTInCallAccept();
						break;

					case BT_HFP_STATE_OUTGOING_CALL:
						BTInCallReject();
						break;

					case BT_HFP_STATE_ATCTIVE_CALL:
						BTCallHangUp();
						break;

					default:
						break;

				}
				break;

			case MSG_VOL_UP:
				APP_DBG("[BT] MSG_VOL_UP\n");
				{
					uint8_t vol = GetBtHfVol();
					if(vol < MAX_BTHF_VOLUME)
						vol++;
					SetBtHfVol(vol);
					BTPhoneCallVolSyncSet(vol);
				}
				break;
			case MSG_VOL_DW:
				APP_DBG("[BT] MSG_VOL_DW\n");
				{
					uint8_t vol = GetBtHfVol();
					if(vol > 0)
						vol--;
					SetBtHfVol(vol);
					BTPhoneCallVolSyncSet(vol);
				}
				break;
			case MSG_PRE:
			case MSG_NEXT:
				APP_DBG("[BT] MSG_MODE or MSG_PRE or MSG_NEXT\n");
				{
					BTStartHfTransfer();
				}
				break;

#ifdef BT_HF_RECORD_FUNC
			case MSG_REC:
				APP_DBG("[BT] MSG_REC\n");
				if(GetBtRecordState() == BT_RECORD_STATE_NONE)
				{
					if(GetHfpState() > BT_HFP_STATE_CONNECTED)
						BtHfRecordStart();
				}
				else
				{
					BtHfRecordExit();
				}
				break;
#endif
			case MSG_POWER:
				APP_DBG("[BT] MSG_POWER\n");
				BTDisConnect();
				gSys.NextModuleID = MODULE_ID_STANDBY;
				break;

			default:
				break;
		}
	}
	APP_DBG("[BT] end BtHfControl \n");

#ifdef BT_HF_RECORD_FUNC
	if(GetBtRecordState() == BT_RECORD_STATE_EXITING)
	{
		/* 将最后buff中的数据写入盘*/
//		extern uint32_t EncodedBufDataLen;

		APP_DBG("Exiting BT Recording\n");
		EncodedDataBufferToFile(EncodedBufDataLen);
		/*写好退出录音*/
		RecoderExit(WAV_FORMAT);
		SetBtRecordState(BT_RECORD_STATE_NONE);
	}
#endif

	DacSoftMuteSet(TRUE, TRUE);
	DeinitBtHfTimer();
	SetHfModeState(BT_HF_MODE_STATE_NONE);
	PcmSyncUnInit();
	AudioAnaInit();	
	/*恢复Mixer状态*/
	MixerInit((void*)PCM_FIFO_ADDR, PCM_FIFO_LEN);
//	if(GetHistroyModeId() != MODULE_ID_UNKNOWN && gSys.NextModuleID != MODULE_ID_STANDBY)
//	{
//		gSys.NextModuleID = GetHistroyModeId();
//	}
    AudioOutputInit();//输出通道初始化
//	CodecDacMuteSet(FALSE, FALSE);

	return TRUE;
}

#endif/*FUNC_BT_HF_EN*/




