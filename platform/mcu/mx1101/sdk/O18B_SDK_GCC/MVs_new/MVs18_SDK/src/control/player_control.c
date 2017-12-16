///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: player_control.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "task_decoder.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "dev_detect_driver.h"
#include "sd_card.h"
#include "eq.h"
#include "eq_params.h"
#include "clk.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "mixer.h"
#include "breakpoint.h"
#include "recorder_control.h"
#include "nvm.h"
#include "sys_app.h"
#include "browser.h"
#include "lrc.h"
#include "eq.h"
#include "eq_params.h"
#include "timer_api.h"
#include "string_convert.h"
#include "player_control.h"
#include "random.h"

extern uint32_t GetNextModeId(uint32_t CurModeId);
extern uint32_t GetNextModeIdByProi(uint32_t CurModeId);

#if defined(FUNC_USB_EN) || defined(FUNC_CARD_EN) || defined(FUNC_BT_EN)

#ifdef FUNC_FFFB_END_WITHOUT_AUTO_SWITCH
bool IsFFFBEndAutoSwitch = TRUE;	// 快进快退到文件头/尾是否自动切歌
#endif

bool AudioPlayerSongOpen(void);

AUDIO_PLAYER_CONTROL*  gPlayContrl = NULL;// __attribute__((section("data"))) = {0};

bool IsPlayerEnvironmentOk(void);

#define FF_FB_COUNT    2000

// 获取随机数
#define GetRandNum(MaxValue) ((OSSysTickGet()%MaxValue) + 1)

uint8_t TempBuf1[128];
uint8_t TempBuf2[128];

void SwitchPlayerDisk(uint32_t Index/*磁盘号*/)
{
	APP_DBG("PlayCtrl:SwitchPlayerDisk\n");
	if(Index == USB_TYPE)
	{
#ifdef FUNC_USB_EN
		if(IsUDiskLink())
		{
			gSys.NextModuleID = MODULE_ID_PLAYER_USB;
		}
#endif
	}
	else
	{
#ifdef FUNC_CARD_EN
		if(IsCardLink())
		{
			gSys.NextModuleID = MODULE_ID_PLAYER_SD;
		}
#endif
	}
	MsgSend(MSG_COMMON_CLOSE);
}

#ifdef FUNC_LRC_EN
static bool PlayerParseLrc(void)
{
	uint32_t SeekTime = gPlayContrl->CurPlayTime;

	if((gPlayContrl->LrcRow.StartTime + gPlayContrl->LrcRow.Duration) <= SeekTime 
	&& (gPlayContrl->LrcRow.MaxLrcLen = (int16_t)GetLrcTextLength(SeekTime)) >= 0)
	{
		TEXT_ENCODE_TYPE CharSet;
		bool bCheckFlag = FALSE;
		if(gPlayContrl->LrcRow.MaxLrcLen > 128)
		{
			bCheckFlag = TRUE;
			gPlayContrl->LrcRow.MaxLrcLen = 128;
		}

		memset(gPlayContrl->LrcRow.LrcText, 0, 128);

		GetLrcInfo(&gPlayContrl->LrcRow, SeekTime, 0);

		// ??? 下面的代码需要确认执行时间，如果时间较长，影响定时的精确度，需要移到外面执行
		// ??? 编码转换
		CharSet = GetLrcEncodeType();
		if(gPlayContrl->LrcRow.MaxLrcLen > 0 
		  && !(CharSet == ENCODE_UNKNOWN || CharSet == ENCODE_GBK || CharSet == ENCODE_ANSI))
		{
#ifdef FUNC_STRING_CONVERT_EN
			uint8_t  ConvertType = UNICODE_TO_GBK;
			uint8_t* TmpStr = TempBuf2;

			if(CharSet == ENCODE_UTF8)
			{
				ConvertType = UTF8_TO_GBK;
			}
			else if(CharSet == ENCODE_UTF16_BIG)
			{
				ConvertType = UNICODE_BIG_TO_GBK;
			}

			memset(TmpStr, 0, 128);

			gPlayContrl->LrcRow.MaxLrcLen = (uint16_t)StringConvert(TmpStr,
			                                (uint32_t)gPlayContrl->LrcRow.MaxLrcLen,
			                                gPlayContrl->LrcRow.LrcText,
			                                (uint32_t)gPlayContrl->LrcRow.MaxLrcLen,
			                                ConvertType);

			memcpy(gPlayContrl->LrcRow.LrcText, TmpStr, gPlayContrl->LrcRow.MaxLrcLen);
			memset(TmpStr, 0, 128);

			memset((void*)(gPlayContrl->LrcRow.LrcText + gPlayContrl->LrcRow.MaxLrcLen),
			               0, 128 - gPlayContrl->LrcRow.MaxLrcLen);
#endif
			//gPlayContrl->LrcRow.LrcText[gPlayContrl->LrcRow.MaxLrcLen] = '\0';
		}

		// ??? 末尾汉字完整性检查
		if(bCheckFlag)
		{
			uint32_t i = 0;
			while(i < (uint32_t)gPlayContrl->LrcRow.MaxLrcLen)
			{
				if(gPlayContrl->LrcRow.LrcText[i] > 0x80)
				{
					i += 2;
				}
				else
				{
					i++;
				}
			}

			if(i >= (uint32_t)gPlayContrl->LrcRow.MaxLrcLen)
			{
				gPlayContrl->LrcRow.MaxLrcLen--;
				gPlayContrl->LrcRow.LrcText[gPlayContrl->LrcRow.MaxLrcLen] = '\0';
			}
		}

		APP_DBG("<%s>\r\n", gPlayContrl->LrcRow.LrcText);

		return TRUE;
	}

	if(gPlayContrl->LrcRow.MaxLrcLen < 0)
	{
		gPlayContrl->LrcRow.MaxLrcLen = 0;
	}

	return FALSE;	
}

void SearchAndOpenLrcFile(void)
{
	int32_t i, FileNameLen;
	FOLDER CurFolder;
	//if(gPlayContrl->LrcFlag)
	{
		if(FolderOpenByNum(&CurFolder, NULL, gPlayContrl->PlayerFile.FolderNum))
		{
			memset(TempBuf2, 0, sizeof(TempBuf2));
			if(FileGetLongName(&gPlayContrl->PlayerFile, TempBuf2, sizeof(TempBuf2)))
			{
				FileNameLen = sizeof(TempBuf2);
				for(i = (sizeof(TempBuf2) - 1); i >= 1; i -= 2)
				{
					if(TempBuf2[i - 1] != 0 || TempBuf2[i] != 0)
					{
						while(i >= 1 && !(TempBuf2[i - 1] == (uint8_t)'.' && TempBuf2[i] == 0))
						{
							i -= 2;
						}

						if(i > 0 && (i + 6) <= sizeof(TempBuf2))
						{
							TempBuf2[i + 1] = (uint8_t)'l';
							TempBuf2[i + 2] = 0;
							TempBuf2[i + 3] = (uint8_t)'r';
							TempBuf2[i + 4] = 0;
							TempBuf2[i + 5] = (uint8_t)'c';
							TempBuf2[i + 6] = 0;
						}
						FileNameLen = i + 7;
						break;
					}
				}
			}
			else
			{
				memcpy(TempBuf2, (void*)gPlayContrl->PlayerFile.ShortName, 8);
				memcpy((TempBuf2 + 8), (void*)"lrc", 3);
				FileNameLen = 11;
			}

			if(OpenLrcByFileName(&CurFolder, (uint8_t*)(&TempBuf2), FileNameLen, NULL, 0))
			{
				gPlayContrl->LrcRow.LrcText = TempBuf1; // 歌词内存区映射
				gPlayContrl->IsLrcRunning = TRUE;
			}
		}
	}
}

#endif

bool AudioPlayerOpenSongFile(void)
{
	APP_DBG("PlayCtrl:AudioPlayerOpenSongFile\n");
	if(FsGetDriveType() == DEV_ID_NONE || !IsPlayerEnvironmentOk())
	{
		APP_DBG("no active fs\n");
		return FALSE;
	}
	if(gPlayContrl->PlayFolderFlag)
	{
		// 文件夹播放，使用相对路径
		gPlayContrl->FileOpenSuccessFlag = FileOpenByNum(&gPlayContrl->PlayerFile, NULL, (uint16_t)(gPlayContrl->CurFileIndex + gPlayContrl->FolderHandle.StartFileNum));	
	}
	else
	{
		// 全盘播放使用全盘ID
		gPlayContrl->FileOpenSuccessFlag = FileOpenByNum(&gPlayContrl->PlayerFile, NULL, gPlayContrl->CurFileIndex);
	}

	APP_DBG("AudioPlayerOpenSongFile[FolderNum = %d, FileNum = %d]\n",
	gPlayContrl->FolderHandle.FolderNum, gPlayContrl->CurFileIndex);

	if(!gPlayContrl->FileOpenSuccessFlag)
	{
		APP_DBG("Open Error\n");
		return FALSE;
	}
	
#ifdef FUNC_LRC_EN
	SearchAndOpenLrcFile();
#endif
	
	return TRUE;
}


void AudioPlayerCloseSongFile(void)
{
	APP_DBG("PlayCtrl:AudioPlayerCloseSongFile\n");
	if(gPlayContrl->FileOpenSuccessFlag)
	{
		APP_DBG("AudioPlayerCloseSongFile[FileNum = %d]\n", gPlayContrl->PlayerFile.FileNumInDisk);
		FileClose(&gPlayContrl->PlayerFile);
		gPlayContrl->FileOpenSuccessFlag = FALSE;
	}
	else
	{
		APP_DBG("File Handle Invalid!\n");
	}
}


static void PlayerSetSongInfoText(void)
{
#ifdef FUNC_STRING_CONVERT_EN
	uint32_t    TextLen = 0;
	uint32_t    i;

	if((gSys.CurModuleID == MODULE_ID_BLUETOOTH) || (gPlayContrl->CurSongInfo == NULL))
	{
		gPlayContrl->CurSongTitlePtr = NULL;
		return;
	}

	gPlayContrl->CurSongTitlePtr = TempBuf1;
	memset(TempBuf1, 0, sizeof(TempBuf1));

	if(CHAR_SET_UNKOWN != gPlayContrl->CurSongInfo->char_set)
	{
		uint8_t ConvertType = UNICODE_TO_GBK;

		// *** 歌词功能未打开时，可以使用gSys.WorkBuffer 做其它用途
		// ??? 是否需要先判断源字符串编码类型(ID3 编码类型 : gPlayContrl->CurSongInfo->char_set)
		if(CHAR_SET_UTF_8 == gPlayContrl->CurSongInfo->char_set)
		{
			ConvertType = UTF8_TO_GBK;
		}
		else if(CHAR_SET_ISO_8859_1 == gPlayContrl->CurSongInfo->char_set)
		{
			ConvertType = UNKNOWN_CONVERT;
		}

		if(strlen((const char*)gPlayContrl->CurSongInfo->title) == 0)
		{
			TextLen = sizeof(gPlayContrl->PlayerFile.ShortName);
			memcpy(TempBuf1, gPlayContrl->PlayerFile.ShortName, TextLen);
		}
		else
		{
//#ifdef FUNC_STRING_CONVERT_EN
			StringConvert(TempBuf1, sizeof(TempBuf1),
			              gPlayContrl->CurSongInfo->title,
			              sizeof(gPlayContrl->CurSongInfo->title), ConvertType);
//#endif
			TextLen = strlen((const char*)TempBuf1);
		}

		if(strlen((const char*)gPlayContrl->CurSongInfo->artist) > 0)
		{
			*(TempBuf1 + TextLen++) = '/';
//#ifdef FUNC_STRING_CONVERT_EN
			StringConvert(TempBuf1 + TextLen, sizeof(TempBuf1) - TextLen,
			              gPlayContrl->CurSongInfo->artist,
			              sizeof(gPlayContrl->CurSongInfo->artist), ConvertType);
//#endif
			TextLen += strlen((const char*)TempBuf1 + TextLen);
		}

		if(strlen((const char*)gPlayContrl->CurSongInfo->album) > 0)
		{
			*(TempBuf1 + TextLen++) = '/';
//#ifdef FUNC_STRING_CONVERT_EN
			StringConvert(TempBuf1 + TextLen, sizeof(TempBuf1) - TextLen,
			              gPlayContrl->CurSongInfo->album,
			              sizeof(gPlayContrl->CurSongInfo->album), ConvertType);
//#endif
			TextLen += strlen((const char*)(TempBuf1 + TextLen));
		}
	}
	else
	{
		if(gPlayContrl->FileOpenSuccessFlag			
		&& FileGetLongName(&gPlayContrl->PlayerFile, TempBuf2, sizeof(TempBuf2)))
		{
//#ifdef FUNC_STRING_CONVERT_EN
			i = 0;
			while(!(((const char*)TempBuf2)[i] == '\0' && ((const char*)TempBuf2)[i + 1] == '\0'))
			{
				i += 2;
			}
			StringConvert(TempBuf1, sizeof(TempBuf1), TempBuf2, i, UNICODE_TO_GBK);
//#endif
			TextLen = strlen((const char*)TempBuf1);
			memset(TempBuf2, 0, sizeof(TempBuf2));
		}
	}

	ASSERT(TextLen <= sizeof(TempBuf2));

#endif
}


void UpdatePlayingTime(void)
{	
	if(gPlayContrl->PlayFolderFlag)
	{
		APP_DBG("F(%d, %d/%d) ", 
			gPlayContrl->CurFolderIndex, 
			gPlayContrl->CurFileIndex, 
			gPlayContrl->FolderHandle.IncFileCnt);
	}
	else
	{
		APP_DBG("D(%d, %d/%d) ", 
			gPlayContrl->CurFolderIndex, 
			gPlayContrl->CurFileIndex, 
			gPlayContrl->PlayerFile.FsContext->gFsInfo.FileSum);
	}
	
	APP_DBG("%-.8s.%-.3s %02d:%02d ", 
			&gPlayContrl->PlayerFile.ShortName[0],
			&gPlayContrl->PlayerFile.ShortName[8],
			(gPlayContrl->CurPlayTime / 1000) / 60,
			(gPlayContrl->CurPlayTime / 1000) % 60);
	
	switch(gPlayContrl->CurPlayMode)
	{
		case PLAY_MODE_REPEAT_ONE:
			APP_DBG("ONE ");
			break;
		case PLAY_MODE_REPEAT_ALL:
			APP_DBG("ALL ");
			break;
		case PLAY_MODE_REPEAT_FOLDER:
			APP_DBG("RP_FOLDER ");
			break;
		case PLAY_MODE_RANDOM_FOLDER:
			APP_DBG("RDM_FOLDER ");
			break;
		case PLAY_MODE_RANDOM:
			APP_DBG("RANDOM ");
			break;
		case PLAY_MODE_PREVIEW_PLAY:
			APP_DBG("PREVIRW ");
			break;
	}
#ifdef FUNC_TREB_BASS_EN
	if(gSys.EqStatus == 0)
	{
		APP_DBG("TREB:%d/BASS:%d", gSys.TrebVal, gSys.BassVal);
	}
	else
#endif
	{
	/* 需要根据eq_params.h中定义的来修改此处
		switch(gSys.Eq)
		{
			case EQ_STYLE_FLAT:
				APP_DBG("EQ_STYLE_FLAT");
				break;

			case EQ_STYLE_CLASSICAL:
				APP_DBG("EQ_STYLE_CLASSICAL");
				break;

			case EQ_STYLE_POP:
				APP_DBG("EQ_STYLE_POP");
				break;

			case EQ_STYLE_ROCK:
				APP_DBG("EQ_STYLE_ROCK");
				break;

			case EQ_STYLE_JAZZ:
				APP_DBG("EQ_STYLE_JAZZ");
				break;

			default:
				APP_DBG("EQ Closed");
				break;
		}
	*/
	}
	APP_DBG("\n");
	gPlayContrl->LastPlayTime = gPlayContrl->CurPlayTime;
}

void PlayerTimerCB(void)
{
	if(GetPlayState() == PLAYER_STATE_PLAYING)
	{
		if((gPlayContrl->CurPlayTime / 1000 - gPlayContrl->LastPlayTime / 1000 >= 1))
		{
			// 降低刷新频率
			UpdatePlayingTime();
		}

		if(gPlayContrl->RepeatAB.RepeatFlag == REPEAT_OPENED &&
		        gPlayContrl->CurPlayTime >= (gPlayContrl->RepeatAB.StartTime + gPlayContrl->RepeatAB.Times))
		{
			if(gPlayContrl->RepeatAB.LoopCount == 0)
			{
				APP_DBG("Repeat Mode Over\n");
				gPlayContrl->RepeatAB.RepeatFlag = REPEAT_CLOSED;
			}
			else
			{
				//DecoderTaskFB(gPlayContrl->RepeatAB.Times);
				gPlayContrl->RepeatAB.LoopCount--;
				APP_DBG("Repeat Mode Loop %d\n", gPlayContrl->RepeatAB.LoopCount);
			}
		}

		// 检查预览模式是否预览超时
		if(PLAY_MODE_PREVIEW_PLAY == gPlayContrl->CurPlayMode && gPlayContrl->CurPlayTime >= 8000)
		{
			MsgSend(MSG_NEXT_SONG);
		}

		// 歌词显示
#ifdef FUNC_LRC_EN
		if(gPlayContrl->IsLrcRunning)
		{
			PlayerParseLrc();
		}
		else
#endif // FUNC_LRC_EN
		{
			if(gPlayContrl->CurSongTitlePtr == NULL)
			{
				PlayerSetSongInfoText();
			}
		}

#ifdef PLAYER_SPECTRUM
		if(gPlayContrl->SpecTrumFlag)// 频谱采样、刷新
		{
			UpdateFFTData();
			// 频谱刷新很快，所以不使用发送刷新消息的方式
			UpdateSpecTrumUI();
		}
#endif // PLAYER_SPECTRUM
	}
}


void NumKeyTimerCB(void* Param)
{
	APP_DBG("PlayCtrl:NumKeyTimerCB\n");
	StopTimer(&gPlayContrl->NumKeyTimer);

	if((gPlayContrl->InputNumber > gPlayContrl->TotalFileNumInDisk) || (gPlayContrl->InputNumber == 0))
	{   	     
		gPlayContrl->InputNumber = 0;
#ifdef FUNC_SOUND_REMIND
		if(gSys.SoundRemindOn)	
		{
			// 停止播放
			AudioPlayerStop();
			AudioPlayerSongOpen();
		}
#endif
		return;
   	}
	
	// 停止播放
	AudioPlayerStop();
	gPlayContrl->CurFileIndex = gPlayContrl->InputNumber;
#ifndef FUNC_PLAY_RECORD_FOLDER_EN
	if(gPlayContrl->CurFileIndex >= gPlayContrl->RecFileStartId
	&& gPlayContrl->CurFileIndex <= gPlayContrl->RecFileEndId)
	{
		gPlayContrl->CurFileIndex = gPlayContrl->RecFileEndId + 1;
	}
	if(gPlayContrl->CurFileIndex > gPlayContrl->TotalFileNumInDisk)
	{
		gPlayContrl->CurFileIndex = 1;
	}
#endif

	gPlayContrl->InputNumber = 0;
	AudioPlayerSongOpen();
}

//检测对应设备是否被拔除
bool IsPlayerEnvironmentOk(void)
{
	switch(gSys.CurModuleID)
	{
#ifdef FUNC_CARD_EN
		case MODULE_ID_PLAYER_SD:
			return IsCardLink();
#endif
#ifdef FUNC_USB_EN
		case MODULE_ID_PLAYER_USB:
			return IsUDiskLink();
#endif
		case MODULE_ID_BLUETOOTH:
			return 0;;IsBtLink();
		default:
#ifdef FUNC_REC_PLAYBACK_EN
			if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
			{
				if(IsUDiskLink() || IsCardLink())
				{
					return TRUE;
				}
			}
#endif           
			break;
	}
	return FALSE;
}

// player 断点信息设置
void AudioPlayerSetBreakPoint(void)
{
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	BP_SYS_INFO *pBpSysInfo;
	BP_PLAYER_INFO *BpPlayInfo;
	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
	BpPlayInfo = (BP_PLAYER_INFO *)BP_GetInfo(BP_PLAYER_INFO_TYPE);
	
	APP_DBG("PlayCtrl:AudioPlayerSetBreakPoint\n");

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		;//return;
	}
	APP_DBG("Save Player BreakPoint Info\n");

// 调用eprom读写接口，设置当前播放状态信息，如PlayFolderIndex, CurFolderIndex, CurFileIndex, Volume, EQ等
	if(gPlayContrl->FileOpenSuccessFlag
#ifdef FUNC_REC_PLAYBACK_EN
		&& !gPlayContrl->RecBackPlayFlag
#endif  	
	) 

	{
		uint8_t Crc8Data[15];
#ifdef FUNC_MATCH_PLAYER_BP
		int8_t CurPlayDiskBPIdx = GetMatchPlayerBPRet()->BPIndex;
#else
		int8_t CurPlayDiskBPIdx = 0;
#endif
		if(CurPlayDiskBPIdx == -1)
		{
			uint32_t i;
			// 如果没有匹配的断点，则默认将第一条断点信息删除，后面的往前挪，空出最后一项存放最新的断点
			for(i = 1; i < PLAYER_DISK_BP_CNT; i++)
			{
				memcpy(&BpPlayInfo->PlayDiskInfo[i - 1], &BpPlayInfo->PlayDiskInfo[i], sizeof(BP_PLAY_DISK_INFO));
			}
			CurPlayDiskBPIdx = (PLAYER_DISK_BP_CNT - 1);
			
#ifdef FUNC_MATCH_PLAYER_BP            
			SetMatchPlayerBPRet(CurPlayDiskBPIdx, gPlayContrl->CurFileIndex, gPlayContrl->CurFolderIndex);
#endif
		}

		memcpy(Crc8Data, gPlayContrl->PlayerFile.ShortName, 11);
		memcpy((void*)(Crc8Data + 11), (void*)(&gPlayContrl->PlayerFile.Size), 4);

#ifdef FUNC_BREAKPOINT_EN
		BP_SET_ELEMENT(BpPlayInfo->PlayDiskInfo[CurPlayDiskBPIdx].PlayTime, (uint16_t)(gPlayContrl->CurPlayTime / 1000));
		BP_SET_ELEMENT(BpPlayInfo->PlayDiskInfo[CurPlayDiskBPIdx].FileAddr, (gPlayContrl->PlayerFile.DirSecNum + gPlayContrl->PlayerFile.DirOffset));
		BP_SET_ELEMENT(BpPlayInfo->PlayDiskInfo[CurPlayDiskBPIdx].CRC8, GetCrc8CheckSum(Crc8Data, 15));
		BP_SET_ELEMENT(BpPlayInfo->PlayDiskInfo[CurPlayDiskBPIdx].FolderEnFlag, gPlayContrl->PlayFolderFlag);
#endif
	}

	BP_SET_ELEMENT(BpPlayInfo->PlayMode, gPlayContrl->CurPlayMode);
#ifdef FUNC_LRC_EN
	BP_SET_ELEMENT(BpPlayInfo->LrcFlag, gPlayContrl->LrcFlag);
#endif

	BP_SET_ELEMENT(pBpSysInfo->Eq, gSys.Eq);
	BP_SET_ELEMENT(pBpSysInfo->Volume, gSys.Volume);
	
	BP_SaveInfo(BP_SAVE2NVM);
#ifdef BP_SAVE_TO_FLASH // 掉电记忆 test
	BP_SaveInfo(BP_SAVE2FLASH); //SaveNvmToFlash();
#endif
#endif	//FUNC_BREAKPOINT_EN	&& (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))	
}
#if 0
void DisplaySongInfoLog(void)
{
	uint32_t i;
	uint8_t* TmpStr = TempBuf2;
	SongInfo* PlayingSongInfo = audio_decoder_get_song_info();
	uint16_t FileCnt = gPlayContrl->FolderHandle.RecFileCnt;

	APP_DBG("PlayCtrl:DisplaySongInfoLog\n");
	if(PlayingSongInfo == NULL)
	{
		return;
	}

	if(gPlayContrl->PlayFolderFlag)
	{
		FileCnt = gPlayContrl->FolderHandle.IncFileCnt;
	}

	APP_DBG("---------Folder Info--------\n");
	APP_DBG("Folder Opened:%d\n", gPlayContrl->FolderHandle.FolderNum);
	APP_DBG("Folder Name:%s\n", gPlayContrl->FolderHandle.ShortName);
	memset(TmpStr, 0, sizeof(TempBuf1));
	if(FolderGetLongName(&gPlayContrl->FolderHandle, TmpStr, sizeof(TempBuf1)))
	{
		memset(TempBuf1, 0, sizeof(TempBuf1));
#ifdef FUNC_STRING_CONVERT_EN
		StringConvert(TempBuf1, sizeof(TempBuf1), TmpStr, sizeof(TempBuf1), UNICODE_TO_GBK);
#endif
		APP_DBG("Folder LongName:%s\n", TempBuf1);
	}
	else
	{
		APP_DBG("Folder LongName is invalid!\n");
	}
	APP_DBG("Folder Sum:%d\n", gPlayContrl->FolderHandle.RecFolderCnt);
	APP_DBG("Folder Includes Files:%d\n", FileCnt);
	APP_DBG("----------------------------\n");
	memset(TmpStr, 0, sizeof(TempBuf1));
	if(FileGetLongName(&gPlayContrl->PlayerFile, TmpStr, sizeof(TempBuf1)))
	{
		APP_DBG("Song LongName:\n");
		for(i = 0; i < (sizeof(TempBuf1) - 2) && !(TmpStr[i] == 0 && TmpStr[i + 1] == 0); i += 2)
		{
			APP_DBG("%02x %02x ", TmpStr[i], TmpStr[i + 1]);
		}
		APP_DBG("\n");
	}
	else
	{
		APP_DBG("File LongName is invalid!\n");
	}
#ifdef FUNC_LRC_EN
	APP_DBG("LRC:%d\n", gPlayContrl->IsLrcRunning);
#else
	APP_DBG("LRC:0\n");
#endif
	APP_DBG("----------TAG Info----------\n");

	APP_DBG("CharSet:");
	switch(PlayingSongInfo->char_set)
	{
		case CHAR_SET_ISO_8859_1:
			APP_DBG("CHAR_SET_ISO_8859_1\n");
			break;
		case CHAR_SET_UTF_16:
			APP_DBG("CHAR_SET_UTF_16\n");
			break;
		case CHAR_SET_UTF_8:
			APP_DBG("CHAR_SET_UTF_8\n");
			break;
		default:
			APP_DBG("CHAR_SET_UNKOWN\n");
			break;
	}
	
#ifdef FUNC_STRING_CONVERT_EN
	if(PlayingSongInfo->char_set == CHAR_SET_UTF_8)
	{
		StringConvert(PlayingSongInfo->title,     MAX_TAG_LEN, PlayingSongInfo->title,     MAX_TAG_LEN, UTF8_TO_GBK);
		StringConvert(PlayingSongInfo->artist,    MAX_TAG_LEN, PlayingSongInfo->artist,    MAX_TAG_LEN, UTF8_TO_GBK);
		StringConvert(PlayingSongInfo->album,     MAX_TAG_LEN, PlayingSongInfo->album,     MAX_TAG_LEN, UTF8_TO_GBK);
		StringConvert(PlayingSongInfo->comment,   MAX_TAG_LEN, PlayingSongInfo->comment,   MAX_TAG_LEN, UTF8_TO_GBK);
		StringConvert(PlayingSongInfo->genre_str, MAX_TAG_LEN, PlayingSongInfo->genre_str, MAX_TAG_LEN, UTF8_TO_GBK);
	}
	else if(PlayingSongInfo->char_set == CHAR_SET_UTF_16)
	{
		StringConvert(PlayingSongInfo->title,     MAX_TAG_LEN, PlayingSongInfo->title,     MAX_TAG_LEN, UNICODE_TO_GBK);
		StringConvert(PlayingSongInfo->artist,    MAX_TAG_LEN, PlayingSongInfo->artist,    MAX_TAG_LEN, UNICODE_TO_GBK);
		StringConvert(PlayingSongInfo->album,     MAX_TAG_LEN, PlayingSongInfo->album,     MAX_TAG_LEN, UNICODE_TO_GBK);
		StringConvert(PlayingSongInfo->comment,   MAX_TAG_LEN, PlayingSongInfo->comment,   MAX_TAG_LEN, UNICODE_TO_GBK);
		StringConvert(PlayingSongInfo->genre_str, MAX_TAG_LEN, PlayingSongInfo->genre_str, MAX_TAG_LEN, UNICODE_TO_GBK);
	}
	else if((PlayingSongInfo->char_set & 0xF0000000) == CHAR_SET_DIVERSE)
	{
		uint32_t type = PlayingSongInfo->char_set & 0xF;
		if(type > 1)  
		{
			StringConvert(PlayingSongInfo->title,     MAX_TAG_LEN, PlayingSongInfo->title,     MAX_TAG_LEN, (type - 2) * 2); 
		}
		type = (audio_decoder->song_info->char_set >> 4)  & 0xF;
		if(type > 1)  
		{
			StringConvert(PlayingSongInfo->artist,    MAX_TAG_LEN, PlayingSongInfo->artist,    MAX_TAG_LEN, (type - 2) * 2);
		}
		type = (audio_decoder->song_info->char_set >> 8)  & 0xF;
		if(type > 1)  
		{
			StringConvert(PlayingSongInfo->album,     MAX_TAG_LEN, PlayingSongInfo->album,     MAX_TAG_LEN, (type - 2) * 2);
		}
		type = (audio_decoder->song_info->char_set >> 12) & 0xF;
		if(type > 1)  
		{
			StringConvert(PlayingSongInfo->comment,   MAX_TAG_LEN, PlayingSongInfo->comment,   MAX_TAG_LEN, (type - 2) * 2);
		}
		type = (audio_decoder->song_info->char_set >> 16) & 0xF;
		if(type > 1)  
		{
			StringConvert(PlayingSongInfo->genre_str, MAX_TAG_LEN, PlayingSongInfo->genre_str, MAX_TAG_LEN, (type - 2) * 2);
		}
	}
#endif
	
	APP_DBG("title: %s\n", PlayingSongInfo->title);
	APP_DBG("artist: %s\n", PlayingSongInfo->artist);
	APP_DBG("Album: %s\n", PlayingSongInfo->album);
	APP_DBG("comment: %s\n", PlayingSongInfo->comment);
	APP_DBG("genre: %d %s\n", PlayingSongInfo->genre, PlayingSongInfo->genre_str);
	APP_DBG("year: %s\n", PlayingSongInfo->year);
	
	APP_DBG("\n");
	APP_DBG("----------------------------\n");
	APP_DBG("**********Song Info*********\n");
	APP_DBG("SongType:");
	switch(PlayingSongInfo->stream_type)
	{
		case STREAM_TYPE_MP2:
			APP_DBG("MP2");
			break;
		case STREAM_TYPE_MP3:
			APP_DBG("MP3");
			break;
		case STREAM_TYPE_WMA:
			APP_DBG("WMA");
			break;
		case STREAM_TYPE_SBC:
			APP_DBG("SBC");
			break;
		case STREAM_TYPE_PCM:
			APP_DBG("PCM");
			break;
		case STREAM_TYPE_ADPCM:
			APP_DBG("ADPCM");
			break;
		case STREAM_TYPE_FLAC:
			APP_DBG("FLAC");
			break;
		case STREAM_TYPE_AAC:
			APP_DBG("AAC");
			break;
		default:
			APP_DBG("UNKNOWN");
			break;
	}
	APP_DBG("\n");
	APP_DBG("Chl Num:%d\n", PlayingSongInfo->num_channels);
	APP_DBG("SampleRate:%d\n", PlayingSongInfo->sampling_rate);
	APP_DBG("BitRate:%d\n", PlayingSongInfo->bitrate);
	APP_DBG("File Size:%d\n", PlayingSongInfo->file_size);
	APP_DBG("TotalPlayTime:%dms\n", PlayingSongInfo->duration);
	APP_DBG("CurPlayTime:%dms\n", gPlayContrl->CurPlayTime);
	APP_DBG("IsVBR:%d\n", PlayingSongInfo->vbr_flag);
	APP_DBG("MpegVer:");
	switch(audio_decoder_get_mpeg_version())
	{
		case MPEG_VER2d5:
			APP_DBG("MPEG_2_5");
			break;
		case MPEG_VER1:
			APP_DBG("MPEG_1");
			break;
		case MPEG_VER2:
			APP_DBG("MPEG_2");
			break;
		default:
			APP_DBG("MPEG_UNKNOWN");
			break;
	}
	APP_DBG("\n");
	APP_DBG("Id3Ver:%d\n", audio_decoder_get_id3_version());

	APP_DBG("**************************\n");
}

#endif

bool AudioPlayerInitialize(int32_t FileIndex, uint32_t FolderIndex, bool IsNeedInitFs)
{
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	BP_PLAYER_INFO *BpPlayInfo;
	BP_SYS_INFO *pBpSysInfo;
	BpPlayInfo = (BP_PLAYER_INFO *)BP_GetInfo(BP_PLAYER_INFO_TYPE);
	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
#endif	
	
	if(gPlayContrl != NULL)
	{
		if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
		{
			APP_DBG("Init fail: player is running\n");
			// 这里禁止重入
			return FALSE;
		}
		else
		{
			APP_DBG("player is reopened\n");
			AudioPlayerSetBreakPoint(); // 重入时，先保存上一次的设置数据
			AudioPlayerStop();
		}
	}
	else
	{
		gPlayContrl = APP_MMM_MALLOC(sizeof(AUDIO_PLAYER_CONTROL), MMM_ALLOC_NORMAL);
		ASSERT(gPlayContrl != NULL);
	}

	memset(gPlayContrl, 0, sizeof(AUDIO_PLAYER_CONTROL));

	SetPlayState(PLAYER_STATE_IDLE);
#ifdef FUNC_LRC_EN
	gPlayContrl->LrcRow.LrcText      = TempBuf1; // 歌词内存区映射
#endif
	gPlayContrl->RepeatAB.RepeatFlag = REPEAT_CLOSED;

	// 恢复断点信息
	//AudioPlayerGetBreakPoint();
	
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	gPlayContrl->CurPlayMode = BP_GET_ELEMENT(BpPlayInfo->PlayMode);
	if(gPlayContrl->CurPlayMode == PLAY_MODE_REPEAT_FOLDER || gPlayContrl->CurPlayMode == PLAY_MODE_RANDOM_FOLDER)
	{
		gPlayContrl->PlayFolderFlag = TRUE;
	}
	else
	{
		gPlayContrl->PlayFolderFlag = FALSE;
	}
	
	#ifdef FUNC_LRC_EN
		gPlayContrl->LrcFlag = BP_GET_ELEMENT(BpPlayInfo->LrcFlag);
	#endif
		gSys.Eq = BP_GET_ELEMENT(pBpSysInfo->Eq);
		gSys.Volume = BP_GET_ELEMENT(pBpSysInfo->Volume);
#endif

	SetSysVol();

	if(FileIndex != -1)
	{
		gPlayContrl->CurFolderIndex = (uint16_t)FolderIndex;
		gPlayContrl->CurFileIndex = (uint16_t)FileIndex;
		gPlayContrl->CurPlayTime  = 0;
	}

#if (defined (FUNC_USB_EN) || defined (FUNC_CARD_EN)) 
	if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
	{
#ifdef FUNC_REC_PLAYBACK_EN
		if(gSys.CurModuleID != MODULE_ID_REC_BACK_PLAY)
#endif
		{
			if(IsNeedInitFs)
			{
				DEV_ID NewDevId = (gSys.CurModuleID == MODULE_ID_PLAYER_USB ? DEV_ID_USB : DEV_ID_SD);
				FSDeInit(DEV_ID_SD);
				FSDeInit(DEV_ID_USB);

				LockSdClk();
				if(NewDevId == DEV_ID_SD)
				{
					SdControllerInit();
				}
				UnLockSdClk();

				if((NewDevId == DEV_ID_USB && !HostStorInit()) ||
						(NewDevId == DEV_ID_SD && SdCardInit() != NONE_ERR))
				{
					APP_DBG("device init error!\n");
					return FALSE;
				}

				if(!FSInit(NewDevId))
				{
					APP_DBG("FSInit() error!\n");
					return FALSE;
				}
			}

			gPlayContrl->CurFolderIndex = 1; // 根目录

			// FS初始化成功时，根据播放断点的扇区号、CRC等信息，恢复播放断点FileIndex, FolderIndex
			{
#if defined(FUNC_MATCH_PLAYER_BP) && defined(FUNC_BREAKPOINT_EN)
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))	
				PLAYER_BP_RET* BPRet = GetMatchPlayerBPRet();
				gPlayContrl->IsBPMached = TRUE;
				if(BPRet->BPIndex != -1)
				{
					gPlayContrl->CurFileIndex = BPRet->FileId;
					gPlayContrl->CurPlayTime = (BP_GET_ELEMENT(BpPlayInfo->PlayDiskInfo[BPRet->BPIndex].PlayTime)*1000);
					//gPlayContrl->PlayFolderFlag = BP_GET_ELEMENT(BpPlayInfo->PlayDiskInfo[BPRet->BPIndex].FolderEnFlag);
				}
				else
#endif					
#endif
				{
					gPlayContrl->CurFileIndex = 1;

				}
			}

			// 打开播放文件夹，默认全盘播放
			if(!FolderOpenByNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex) 
			|| gPlayContrl->FolderHandle.RecFileCnt == 0)
			{
				APP_DBG("Folder open fail, Or no song founded\n");
				return FALSE;
			}
			
			gPlayContrl->TotalFileNumInDisk = gPlayContrl->FolderHandle.RecFileCnt;
#ifndef FUNC_PLAY_RECORD_FOLDER_EN
			//if(gPlayContrl->PlayFolderFlag == FALSE)
			{
				FOLDER   FolderHandle;
				FAT_FILE FileHandle;
				memset(&FolderHandle, 0, sizeof(FolderHandle));
				if(FolderOpenByName(&FolderHandle, &gPlayContrl->FolderHandle, "record")
				&& FileOpenByNum(&FileHandle, &FolderHandle, 1))
				{
					gPlayContrl->RecFileStartId = FileHandle.FileNumInDisk;
					gPlayContrl->RecFileEndId   = gPlayContrl->RecFileStartId + FolderHandle.RecFileCnt;
					gPlayContrl->RecFolderId    = FolderHandle.FolderNum;
					FileClose(&FileHandle);
				}
				if(FolderHandle.RecFileCnt == gPlayContrl->FolderHandle.RecFileCnt)
				{
					APP_DBG("only record folder!\n");
					return FALSE;
				}
			}
#endif
		}
			
#ifdef FUNC_REC_PLAYBACK_EN
		if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		{
			gPlayContrl->RecBackPlayFlag = 1;
		}
		else
		{
			gPlayContrl->RecBackPlayFlag = 0;
		}
		if(gPlayContrl->RecBackPlayFlag == 1)
		{
			gPlayContrl->PlayFolderFlag = TRUE;
		}
#endif		
		gPlayContrl->ValidFolderCnt = gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum;
		if(gPlayContrl->PlayFolderFlag)
		{
#ifdef FUNC_REC_PLAYBACK_EN
			if(gPlayContrl->RecBackPlayFlag == 0)
#endif
			{
				// 文件夹播放，使用相对路径
				gPlayContrl->FileOpenSuccessFlag = FileOpenByNum(&gPlayContrl->PlayerFile, NULL, (uint16_t)(gPlayContrl->CurFileIndex + gPlayContrl->FolderHandle.StartFileNum));
				if(gPlayContrl->FileOpenSuccessFlag)
				{
					gPlayContrl->CurFolderIndex = GetValidFolderNum(gPlayContrl->PlayerFile.FsContext, gPlayContrl->PlayerFile.FolderNum);
					gPlayContrl->CurFileIndex   = gPlayContrl->PlayerFile.FileNumInFolder;
					FileClose(&gPlayContrl->PlayerFile);
				}
			}
			if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
			{
				APP_DBG("Folder open fail!\n");
				return FALSE;
			}
		}
	}
#endif

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//;;BTAudioPlayStart();
	}
	else
	{
        if(gSys.CurModuleID == MODULE_ID_PLAYER_SD)
        {
            SoundRemind(SOUND_CARD_PLAYBACK);  
        }
        else if(gSys.CurModuleID == MODULE_ID_PLAYER_USB)
        {
            SoundRemind(SOUND_UDISC_PLAYBACK);  
        }

        if(GetQuickResponseFlag())
        {
        	return FALSE;
        }
        
		AudioPlayerSongOpen();
	}

	return TRUE;
}

// 去初始化
bool AudioPlayerDeinitialize(void)
{
	APP_DBG("AudioPlayerDeinitialize\n");
	if(gPlayContrl == NULL)
	{
		return FALSE;
	}
	// 设置断点信息
	if(gSys.CurModuleID != MODULE_ID_BLUETOOTH
#ifdef FUNC_MATCH_PLAYER_BP
	&& gPlayContrl->IsBPMached
#endif
	  )
	{
#ifdef FUNC_REC_PLAYBACK_EN
		if(gPlayContrl->RecBackPlayFlag)
		{
			AudioPlayerSetBreakPoint();
		}
		else
#endif
		{
			if(!gPlayContrl->FileOpenSuccessFlag)
			{
				// 停止播放时，退出模式，也保存断点信息，但是播放时间重新设置为0s
#ifdef FUNC_MATCH_PLAYER_BP
				PLAYER_BP_RET* BPRet = GetMatchPlayerBPRet();
				if(gPlayContrl->CurFileIndex != BPRet->FileId && IsPlayerEnvironmentOk())
#else
				if(IsPlayerEnvironmentOk())
#endif
				{
					AudioPlayerOpenSongFile();
				}
				gPlayContrl->CurPlayTime  = 0;
				AudioPlayerSetBreakPoint();
				AudioPlayerCloseSongFile();
			}
			else
			{
				AudioPlayerSetBreakPoint();
			}
		}
	}
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//BTAudioPlayStop();
		AudioPlayerSetBreakPoint();
	}
	else
	{
		AudioPlayerStop();
	}
        
	// 关闭歌词
#ifdef FUNC_LRC_EN
	if(gPlayContrl->IsLrcRunning)
	{
		CloseLrcFile();
		gPlayContrl->IsLrcRunning = FALSE;
	}
#endif

	// DeinitTimer(&gPlayContrl->NumKeyTimer);

	// 释放资源
	APP_MMM_FREE(gPlayContrl);
	gPlayContrl = NULL;

	return TRUE;
}

bool AudioPlayerSongOpen(void)
{
	APP_DBG("PlayCtrl:AudioPlayerSongOpen\n");
	gPlayContrl->RepeatAB.RepeatFlag = REPEAT_CLOSED;
	gPlayContrl->IsPlayerBeStop = FALSE;
	
	AudioPlayerCloseSongFile();
	if(AudioPlayerOpenSongFile())
	{
		// 本地播放
		gPlayContrl->error_times = 0;
		// 立即更新一次播放文件信息
		//if(!DecoderTaskPlayStart(&gPlayContrl->PlayerFile, IO_TYPE_FILE, gPlayContrl->CurPlayTime)
		//|| !DecoderTaskIsInitializeOk())//失败
		{
			APP_DBG("decoder init fail\n");
			SetPlayState(PLAYER_STATE_IDLE);
			AudioPlayerCloseSongFile();
#ifdef FUNC_REC_PLAYBACK_EN
			if(gPlayContrl->RecBackPlayFlag)
			{
				MsgSend(MSG_REC_FILE_DEL);// 录音文件打开失败，直接删除
				return TRUE;
			}
#endif
			if(gPlayContrl->IsPlayerPreSong)
			{
				//AudioPlayerPreSong();
				MsgSend(MSG_PRE_SONG);// 本地播放文件打开失败，自动切换到下一首
			}
			else
			{
				//AudioPlayerNextSong();
				MsgSend(MSG_NEXT_SONG);// 本地播放文件打开失败，自动切换到下一首
			}

		}
		//else
		{
			//gPlayContrl->CurSongInfo = audio_decoder_get_song_info();
			//DisplaySongInfoLog();
			// 歌词查找
#ifdef FUNC_LRC_EN
			if(!gPlayContrl->IsLrcRunning)
			{
				PlayerSetSongInfoText();
			}
#endif
			if(GetPlayState() != PLAYER_STATE_PAUSE)
			{
				SetPlayState(PLAYER_STATE_PLAYING);
			}
		}
		UpdatePlayingTime();
		AudioPlayerSetBreakPoint();
		return TRUE;
	}

	if(!IsPlayerEnvironmentOk())
	{
		gSys.NextModuleID = GetNextModeId(MODULE_ID_UNKNOWN);
		MsgSend(MSG_COMMON_CLOSE);
		return FALSE;
	}

	if((gPlayContrl->error_times++) > 3) // 磁盘仍有效，但是连续打开文件失败，则重新初始化
	{
		SwitchPlayerDisk((gSys.CurModuleID == MODULE_ID_PLAYER_SD ? SD_TYPE : USB_TYPE));
		return TRUE;
	}
	MsgSend(MSG_NEXT_SONG);// 本地播放文件打开失败，自动切换到下一首

	return TRUE;
}


//数字键处理
bool AudioPlayerNumMsg(uint16_t Msg)
{
	uint16_t FileCnt = gPlayContrl->FolderHandle.RecFileCnt;
	APP_DBG("PlayCtrl:AudioPlayerNumMsg\n");

	if(gPlayContrl->InputNumber < FileCnt)
	{
		if(!gPlayContrl->NumKeyTimer.IsRunning)
		{
			InitTimer(&gPlayContrl->NumKeyTimer, 2000, NumKeyTimerCB);      
		}
		else
		{
			ResetTimer(&gPlayContrl->NumKeyTimer);
		}
		
		StartTimer(&gPlayContrl->NumKeyTimer);

		gPlayContrl->InputNumber = (gPlayContrl->InputNumber * 10) + (Msg - MSG_NUM_0);
		if(gPlayContrl->InputNumber > FileCnt)
		{
			gPlayContrl->InputNumber = FileCnt;
		}
		SoundRemind(SOUND_NUM0 + (Msg - MSG_NUM_0)); 
		if(gSys.SoundRemindOn)	
		{
			// 停止播放
			AudioPlayerStop();
		}	
	}
	
	return TRUE;
}

void AudioPlayerChangeMode(void)
{
	APP_DBG("PlayCtrl:AudioPlayerChangeMode\n");
}


void AudioPlayerPower(void)
{
	APP_DBG("PlayCtrl:AudioPlayerPower\n");
	gSys.NextModuleID = MODULE_ID_STANDBY;
}


void AudioPlayerControlPlay(void)
{
	APP_DBG("PlayCtrl:AudioPlayerControlPlay\n");
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(!IsAvrcpConnected())
		{
			return;
		}
		//BTPlayCtrlPlay();
	}
	else	// USB, SD mode
	{
		//DecoderTaskResume();
	}
	
	SetPlayState(PLAYER_STATE_PLAYING);
}


void AudioPlayerControlPause(void)
{
	APP_DBG("PlayCtrl:AudioPlayerControlPause\n");
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(!IsAvrcpConnected())
		{
			return;
		}
		//BTPlayCtrlPause();
		WaitMs(500);
	}
	else	// USB, SD mode
	{
		//DecoderTaskPause();
	}

	SetPlayState(PLAYER_STATE_PAUSE);
}


void AudioPlayerPlayPause(void)
{
	APP_DBG("PlayCtrl:AudioPlayerPlayPause\n");
	if(GetPlayState() == PLAYER_STATE_PLAYING)
	{
		AudioPlayerControlPause();
		SoundRemind(SOUND_PAUSE);
	}
	else
	{
		SoundRemind(SOUND_PLAY);
		AudioPlayerControlPlay();
	}
}


void AudioPlayerStop(void)
{
	APP_DBG("PlayCtrl:AudioPlayerStop\n");	
	gPlayContrl->FFAndFBCnt = 0;
	gPlayContrl->CurSongTitlePtr = NULL;
	gPlayContrl->RepeatAB.RepeatFlag = REPEAT_CLOSED;
	gPlayContrl->IsPlayerBeStop = TRUE;
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(!IsAvrcpConnected())
		{
			return;
		}
		//BTPlayCtrlStop();
		WaitMs(500);
	}
	else
	{
		//DecoderTaskStop();
		
			// 关闭歌词
#ifdef FUNC_LRC_EN
		//if(gPlayContrl->LrcFlag)
		{
			CloseLrcFile();
			memset(&gPlayContrl->LrcRow, 0, sizeof(gPlayContrl->LrcRow));
		}

		if(gPlayContrl->IsLrcRunning)
		{
			gPlayContrl->IsLrcRunning = FALSE;
		}
#endif
		AudioPlayerCloseSongFile();
	}

	// 播放时间清零
	gPlayContrl->CurPlayTime = 0;
	SetPlayState(PLAYER_STATE_IDLE);
	MsgClear(MSG_NEXT);
	MsgClear(MSG_NEXT_SONG);
	MsgClear(MSG_PRE);
	MsgClear(MSG_PRE_SONG);
}


bool AudioPlayerNextSong(void)
{
	uint16_t FileCnt = gPlayContrl->TotalFileNumInDisk;
	
	if(gPlayContrl->PlayFolderFlag)
	{
		FileCnt = gPlayContrl->FolderHandle.IncFileCnt;
	}
	APP_DBG("PlayCtrl:AudioPlayerNextSong\n");
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(!IsAvrcpConnected())
		{
			return TRUE;
		}
		//BTPlayCtrlNextSong();
		SoundRemind(SOUND_NEXT_TRACK);
	}
	else	// USB, SD
	{
		SetPlayState(PLAYER_STATE_SWITCH_NEXT);
		
		switch(gPlayContrl->CurPlayMode)
		{
			case PLAY_MODE_RANDOM:
			case PLAY_MODE_RANDOM_FOLDER:
			//根据时钟或某个时刻变化的值生成随机数
				gPlayContrl->CurFileIndex = (uint16_t)GetRandomNum(((uint16_t)OSSysTickGet()), FileCnt);
				break;
			
			case PLAY_MODE_REPEAT_ONE:
				break;

			case PLAY_MODE_REPEAT_FOLDER:
			case PLAY_MODE_REPEAT_ALL:
			case PLAY_MODE_PREVIEW_PLAY:
				gPlayContrl->CurFileIndex++;
#ifndef FUNC_PLAY_RECORD_FOLDER_EN
				if(gPlayContrl->CurFileIndex >= gPlayContrl->RecFileStartId 
				&& gPlayContrl->CurFileIndex <= gPlayContrl->RecFileEndId)
				{
					gPlayContrl->CurFileIndex = gPlayContrl->RecFileEndId + 1;
				}
#endif
				if(gPlayContrl->CurFileIndex > FileCnt)
				{
					gPlayContrl->CurFileIndex = 1;
				}
				break;

			default:
				break;
		}
		gPlayContrl->IsPlayerPreSong = FALSE;
		AudioPlayerStop();
		SoundRemind(SOUND_NEXT_TRACK);
		if(!AudioPlayerSongOpen())
		{
			return FALSE;
		}
	}
	return TRUE;
}


bool AudioPlayerPreSong(void)
{
	uint16_t FileCnt = gPlayContrl->TotalFileNumInDisk;

	if(gPlayContrl->PlayFolderFlag)
	{
		FileCnt = gPlayContrl->FolderHandle.IncFileCnt;
	}
				
	APP_DBG("PlayCtrl:AudioPlayerPreSong\n");

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(!IsAvrcpConnected())
		{
			return TRUE;
		}
		//BTPlayCtrlPreSong();
		SoundRemind(SOUND_PREV_TRACK);
	}
	else// USB, SD
	{
		SetPlayState(PLAYER_STATE_SWITCH_PRE);
		switch(gPlayContrl->CurPlayMode)
		{
			case PLAY_MODE_RANDOM:
			case PLAY_MODE_RANDOM_FOLDER:
				// 根据时钟或某个时刻变化的值生成随机数
				gPlayContrl->CurFileIndex = (uint16_t)GetRandomNum(((uint16_t)OSSysTickGet()), FileCnt);
				break;
			
			case PLAY_MODE_REPEAT_ONE:
				break;
			
			case PLAY_MODE_REPEAT_FOLDER:
			case PLAY_MODE_REPEAT_ALL:
			case PLAY_MODE_PREVIEW_PLAY:
				gPlayContrl->CurFileIndex--;
				if(gPlayContrl->CurFileIndex < 1)
				{
					gPlayContrl->CurFileIndex = (uint16_t)FileCnt;
				}			
#ifndef FUNC_PLAY_RECORD_FOLDER_EN
				if(gPlayContrl->CurFileIndex >= gPlayContrl->RecFileStartId
				&& gPlayContrl->CurFileIndex <= gPlayContrl->RecFileEndId)
				{
					gPlayContrl->CurFileIndex = gPlayContrl->RecFileStartId - 1;
				}
#endif
				break;

			default:
				break;
		}
		gPlayContrl->IsPlayerPreSong = TRUE;
		AudioPlayerStop();
		SoundRemind(SOUND_PREV_TRACK);	
		if(!AudioPlayerSongOpen())
		{
			return FALSE;
		}
	}

	return TRUE;
}

#ifdef FUNC_RECORD_EN
void AudioPlayerRecorder(void)
{
	APP_DBG("PlayCtrl:AudioPlayerRecorder\n");
#ifdef FUNC_REC_PLAYBACK_EN
	if(gSys.CurModuleID != MODULE_ID_REC_BACK_PLAY)
#endif
	{
		gSys.ModuleIDBackUp = gSys.CurModuleID;
	}
	gSys.NextModuleID = MODULE_ID_RECORDER;
	gSys.RecordState = RECORD_STATE_REC;
	gSys.RecordSource = RECORD_SOURCE_LINEIN;
}
#endif


#ifdef FUNC_REC_PLAYBACK_EN
bool AudioPlayerRecBackPlay(void)
{
	APP_DBG("PlayCtrl:AudioPlayerRecBackPlay\n");
	if(gSys.CurModuleID != MODULE_ID_REC_BACK_PLAY)
	{
		EnterRecBackPlay(gSys.CurModuleID, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, FALSE, 0, 0);
	}
	else
	{
		return FALSE; 
	}
	return TRUE;
}
#endif


bool AudioPlayerFolderMode(void)
{
	APP_DBG("PlayCtrl:AudioPlayerFolderMode\n");
	
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		return TRUE;
	}

#ifdef FUNC_REC_PLAYBACK_EN	
	if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
	{
		return TRUE;
	}
#endif
	
	if(gPlayContrl->CurPlayMode == PLAY_MODE_REPEAT_FOLDER || gPlayContrl->CurPlayMode == PLAY_MODE_RANDOM_FOLDER)
		gPlayContrl->PlayFolderFlag = TRUE;
	else
		gPlayContrl->PlayFolderFlag = FALSE;

	if(!gPlayContrl->PlayFolderFlag)
	{
		gPlayContrl->CurFolderIndex = 1;
		if(gPlayContrl->FileOpenSuccessFlag)
		{
			gPlayContrl->CurFileIndex = gPlayContrl->PlayerFile.FileNumInDisk;
		}
		else
		{
			gPlayContrl->CurFileIndex = 1;
		}

		if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex)
		|| gPlayContrl->FolderHandle.RecFileCnt == 0)
		{
			APP_DBG("Folder open fail!\n");
			return FALSE;
		}
	}
	else
	{
		if(gPlayContrl->FileOpenSuccessFlag)
		{
			gPlayContrl->CurFolderIndex = GetValidFolderNum(gPlayContrl->PlayerFile.FsContext, gPlayContrl->PlayerFile.FolderNum);
			gPlayContrl->CurFileIndex   = gPlayContrl->PlayerFile.FileNumInFolder;
		}
		if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
		{
			APP_DBG("Folder open fail!\n");
			return FALSE;
		}
	}

	return TRUE;
}

bool AudioPlayerFolderNext(void)
{
	APP_DBG("PlayCtrl:AudioPlayerFolderNext\n");

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		return TRUE;
	}

#ifdef FUNC_REC_PLAYBACK_EN	    
	if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
	{
		return TRUE;
	}
#endif    
/*
	if(!gPlayContrl->PlayFolderFlag)
	{
		gPlayContrl->PlayFolderFlag = TRUE;
		if(gPlayContrl->FileOpenSuccessFlag)
		{
			gPlayContrl->CurFolderIndex = GetValidFolderNum(gPlayContrl->PlayerFile.FsContext, gPlayContrl->PlayerFile.FolderNum);
			gPlayContrl->CurFileIndex   = gPlayContrl->PlayerFile.FileNumInFolder;
		}

		if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
		{
			APP_DBG("Folder open fail!\n");
			return FALSE;
		}
	}
*/
	if(gPlayContrl->PlayFolderFlag)
	{
		SoundRemind(SOUND_NEXT_FOLD);  
		AudioPlayerStop();
		do
		{
			gPlayContrl->CurFolderIndex++;
			if(gPlayContrl->CurFolderIndex > gPlayContrl->ValidFolderCnt)
			{
				gPlayContrl->CurFolderIndex = 1;
			}
			
			if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
			{
				APP_DBG("Folder open fail!\n");
				return FALSE;
			}
		}
#ifndef FUNC_PLAY_RECORD_FOLDER_EN
		 while((gPlayContrl->FolderHandle.ShortName[0] =='R') &&(gPlayContrl->FolderHandle.ShortName[1] =='E') 
			&&(gPlayContrl->FolderHandle.ShortName[2] =='C') &&(gPlayContrl->FolderHandle.ShortName[3] =='O')
			&&(gPlayContrl->FolderHandle.ShortName[4] =='R') &&(gPlayContrl->FolderHandle.ShortName[5] =='D'));
#else
		while(0);
#endif
		APP_DBG("Play Folder:%s\n", gPlayContrl->FolderHandle.ShortName);
		gPlayContrl->CurFileIndex = 1;
		if(!AudioPlayerSongOpen())
		{
			return FALSE;
		}
	}
	
	return TRUE;
}


bool AudioPlayerFolderPre(void)
{
	APP_DBG("PlayCtrl:AudioPlayerFolderPre\n");

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		return TRUE;
	}

#ifdef FUNC_REC_PLAYBACK_EN		
	if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
	{
		return TRUE;
	}
#endif
/*
	if(!gPlayContrl->PlayFolderFlag)
	{
		gPlayContrl->PlayFolderFlag = TRUE;
		if(gPlayContrl->FileOpenSuccessFlag)
		{
			gPlayContrl->CurFolderIndex = GetValidFolderNum(gPlayContrl->PlayerFile.FsContext, gPlayContrl->PlayerFile.FolderNum);
			gPlayContrl->CurFileIndex   = gPlayContrl->PlayerFile.FileNumInFolder;
		}

		if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
		{
			APP_DBG("Folder open fail!\n");
			return FALSE;
		}
	}
*/
	if(gPlayContrl->PlayFolderFlag)
	{
		//SoundRemind(SOUND_PREV_FOLD); 
		AudioPlayerStop();
		do
		{
			gPlayContrl->CurFolderIndex--;
			if(gPlayContrl->CurFolderIndex < 1)
			{
				gPlayContrl->CurFolderIndex = gPlayContrl->ValidFolderCnt;
			}	            
			if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
			{
				APP_DBG("Folder open fail!\n");
				return FALSE;
			}		
		}
#ifndef FUNC_PLAY_RECORD_FOLDER_EN
		while((gPlayContrl->FolderHandle.ShortName[0] =='R') &&(gPlayContrl->FolderHandle.ShortName[1] =='E') 
		&&(gPlayContrl->FolderHandle.ShortName[2] =='C') &&(gPlayContrl->FolderHandle.ShortName[3] =='O')
		&&(gPlayContrl->FolderHandle.ShortName[4] =='R') &&(gPlayContrl->FolderHandle.ShortName[5] =='D'));
#else
		while(0);
#endif
		APP_DBG("Play Folder:%s\n", gPlayContrl->FolderHandle.ShortName);
		gPlayContrl->CurFileIndex = 1;
		if(!AudioPlayerSongOpen())
		{
			return FALSE;
		}
	}

	return TRUE;
}

void AudioPlayerFastForward(void)
{
	APP_DBG("PlayCtrl:AudioPlayerFastForward\n");
#ifdef FUNC_FFFB_END_WITHOUT_AUTO_SWITCH
	IsFFFBEndAutoSwitch = FALSE;//快进快退到文件头/尾是否自动切歌
#endif
	
	if(GetPlayState() < PLAYER_STATE_PLAYING)	// 停止状态下，禁止快进、快退
	{
		return;
	}
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(GetPlayState() != PLAYER_STATE_FF && IsAvrcpConnected())
		{
			//BTPlayCtrlFastForward();
			SetPlayState(PLAYER_STATE_FF);
		}
	}
	else
	{
		if(GetPlayState() == PLAYER_STATE_PAUSE)
		{
			//DecoderTaskResume();
		}
		//DecoderTaskFF(FF_FB_COUNT);
		SetPlayState(PLAYER_STATE_PLAYING);
	}
}


void AudioPlayerFastBackward(void)
{
	APP_DBG("PlayCtrl:AudioPlayerFastBackward\n");

#ifdef FUNC_FFFB_END_WITHOUT_AUTO_SWITCH
	IsFFFBEndAutoSwitch = FALSE;//快进快退到文件头/尾是否自动切歌
#endif
	
	if(GetPlayState() < PLAYER_STATE_PLAYING)	// 停止状态下，禁止快进、快退
	{
		return;
	}
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//if(GetPlayState() != PLAYER_STATE_FB && IsAvrcpConnected())
		{
			//BTPlayCtrlFastBackward();
			SetPlayState(PLAYER_STATE_FB);
		}
	}
	else
	{
		if(GetPlayState() == PLAYER_STATE_PAUSE)
		{
			//DecoderTaskResume();
		}
		
		//DecoderTaskFB(FF_FB_COUNT);
		SetPlayState(PLAYER_STATE_PLAYING);
	}
}


void AudioPlayerFFFBEnd(void)
{
	APP_DBG("PlayCtrl:AudioPlayerFFFBEnd\n");
#ifdef FUNC_FFFB_END_WITHOUT_AUTO_SWITCH
	IsFFFBEndAutoSwitch = TRUE;//快进快退到文件头/尾是否自动切歌
#endif
	if(GetPlayState() == PLAYER_STATE_FF || GetPlayState() == PLAYER_STATE_FB)
	{
		if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
		{
			//BTPlayCtrlEndFastBackForward();
		}
	}

	AudioPlayerControlPlay();
	
#ifdef FUNC_LRC_EN
	gPlayContrl->LrcRow.StartTime = 0;
	gPlayContrl->LrcRow.Duration = 0;
#endif
}


void AudioPlayerSwitchMode(void)
{
	uint8_t			old_play_mode = gPlayContrl->CurPlayMode;

	APP_DBG("PlayCtrl:AudioPlayerSwitchMode %d\n", old_play_mode);
	// 根据当前播放模式，切换到下一播放模式
	gPlayContrl->CurPlayMode++;
	gPlayContrl->CurPlayMode %= PLAY_MODE_SUM;
	if(((old_play_mode == PLAY_MODE_REPEAT_FOLDER || old_play_mode == PLAY_MODE_RANDOM_FOLDER) && 
		(gPlayContrl->CurPlayMode != PLAY_MODE_REPEAT_FOLDER && gPlayContrl->CurPlayMode != PLAY_MODE_RANDOM_FOLDER)) 
		||
		((old_play_mode == PLAY_MODE_REPEAT_ALL || old_play_mode == PLAY_MODE_REPEAT_ONE || old_play_mode == PLAY_MODE_RANDOM || old_play_mode == PLAY_MODE_PREVIEW_PLAY) &&
		(gPlayContrl->CurPlayMode != PLAY_MODE_REPEAT_ALL && gPlayContrl->CurPlayMode != PLAY_MODE_REPEAT_ONE && gPlayContrl->CurPlayMode != PLAY_MODE_RANDOM && gPlayContrl->CurPlayMode != PLAY_MODE_PREVIEW_PLAY)))
	{
		AudioPlayerFolderMode();
	}
	APP_DBG("AudioPlayerSwitchMode:%d\n", gPlayContrl->CurPlayMode);
}


void AudioPlayerRepeatAB(void)
{
	APP_DBG("PlayCtrl:AudioPlayerRepeatAB\n");
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH 
	|| PLAYER_STATE_PLAYING != GetPlayState() 
	|| gPlayContrl->CurPlayMode == PLAY_MODE_PREVIEW_PLAY)
	{
		// 非播放状态下，不响应Mode键长按事件
		return;
	}
	switch(gPlayContrl->RepeatAB.RepeatFlag)
	{
		case REPEAT_CLOSED: // 设置起点
			gPlayContrl->RepeatAB.StartTime  = gPlayContrl->CurPlayTime;
			gPlayContrl->RepeatAB.RepeatFlag = REPEAT_A_SETED;
			APP_DBG("Set RepeatAB StartTime = %d ms\n", gPlayContrl->RepeatAB.StartTime);
			break;
		case REPEAT_A_SETED: // 设置终点，并开始复读
			if(gPlayContrl->CurPlayTime <= gPlayContrl->RepeatAB.StartTime)
			{
				// 避免异常情况发生
				gPlayContrl->RepeatAB.RepeatFlag = REPEAT_CLOSED;
				APP_DBG("RepeatAB fail, start time <= end time\n");
				break;
			}

			gPlayContrl->RepeatAB.Times      = (gPlayContrl->CurPlayTime - gPlayContrl->RepeatAB.StartTime);
			gPlayContrl->RepeatAB.LoopCount  = 3;
			gPlayContrl->RepeatAB.RepeatFlag = REPEAT_OPENED;
			APP_DBG("Set RepeatAB Duration = %d ms\n", gPlayContrl->RepeatAB.Times);
			//DecoderTaskFB(gPlayContrl->RepeatAB.Times);
			break;
		default: // 取消复读
			gPlayContrl->RepeatAB.RepeatFlag = REPEAT_CLOSED;
			APP_DBG("Cancel RepeatAB\n");
			break;
	}
}


bool AudioPlayerEvent(uint16_t Msg)
{		
#ifdef FUNC_BROWSER_EN
	//文件浏览功能，只在SD卡、U盘播放模式下有效
	if((gSys.CurModuleID == MODULE_ID_PLAYER_SD) || (gSys.CurModuleID == MODULE_ID_PLAYER_USB))
	{
		FOLDER* Folder;
		uint16_t FileNumInFolder;
		uint16_t FileNumInDisk;

		if(!gPlayContrl->BrowseFlag)
		{
			//非文件浏览状态下
			if(Msg == MSG_BROWSE)
			{
				APP_DBG("Start file browse!\n");
				gPlayContrl->BrowseFlag = TRUE;
				FileBrowseInit();
				return TRUE;
			}
		}
		else
		{
			//文件浏览状态下
			if(Msg == MSG_BROWSE)		//停止浏览，返回播放状态 
			{				
				APP_DBG("End file browse!\n");
				gPlayContrl->BrowseFlag = FALSE;
				return TRUE;				
			}
			if((Msg == MSG_PLAY_PAUSE)	//进入下一级目录浏览，或播放选中的歌曲
			|| (Msg == MSG_STOP)		//返回上一级目录浏览
			|| (Msg == MSG_NEXT)		//选择下一个文件夹或文件
			|| (Msg == MSG_PRE))		//选择上一个文件夹或文件
			{
				if(FileBrowseOp(Msg))
				{
					//选中一首歌曲准备播放				
					APP_DBG("Play selected song!\n");
					
					FileBrowseGetResult(&Folder, &FileNumInFolder, &FileNumInDisk);
					APP_DBG("      FolderName: %-.8s\n", Folder->ShortName);
					APP_DBG("       FolderNum: %d\n", Folder->FolderNum);
					APP_DBG("  ValidFolderNum: %d\n", Folder->ValidFolderNum);
					APP_DBG(" FileNumInFolder: %d\n", FileNumInFolder);
					APP_DBG("   FileNumInDisk: %d\n", FileNumInDisk);
					
					gPlayContrl->BrowseFlag = FALSE;
					AudioPlayerStop();
					gPlayContrl->CurFileIndex = FileNumInDisk;
					
					gPlayContrl->CurFolderIndex = Folder->ValidFolderNum;
					memcpy(&gPlayContrl->FolderHandle, Folder, sizeof(FOLDER));
					gPlayContrl->CurFolderIndex = Folder->ValidFolderNum;
					if(gPlayContrl->PlayFolderFlag)
					{
						gPlayContrl->CurFileIndex = FileNumInFolder;
					}
					else
					{
						gPlayContrl->CurFileIndex = FileNumInDisk;
					}
					return AudioPlayerSongOpen();
				}
				return TRUE;
			}
		}	 
	}
#endif
	
	switch(Msg)
	{			
		case MSG_MODE:
			AudioPlayerChangeMode();
			return FALSE;
		
		case MSG_POWER:
			AudioPlayerPower();
			return FALSE;
		
		case MSG_PLAY_PAUSE:
			AudioPlayerPlayPause();
			return TRUE;
			
		case MSG_STOP:
			AudioPlayerStop();
			SoundRemind(SOUND_STOP);  
			return TRUE;
			
		case MSG_NEXT:
		case MSG_NEXT_SONG:
			return AudioPlayerNextSong();
		
		case MSG_PRE:
		case MSG_PRE_SONG:
			return AudioPlayerPreSong();
									
#ifdef FUNC_RECORD_EN
		case MSG_REC:
			AudioPlayerRecorder();
			return FALSE;			
#endif

#ifdef FUNC_REC_PLAYBACK_EN
		case MSG_REC_PLAYBACK:
			return AudioPlayerRecBackPlay();
#endif
		case MSG_FOLDER_MODE:
			return AudioPlayerFolderMode();
		
		case MSG_FOLDER_NEXT:
			return AudioPlayerFolderNext();
		
		case MSG_FOLDER_PRE:
			return AudioPlayerFolderPre();
		
		case MSG_FF_START:
			MsgClear(MSG_FF_START);
			AudioPlayerFastForward();
			return TRUE;
		
		case MSG_FB_START:
			MsgClear(MSG_FB_START);
			AudioPlayerFastBackward();
			return TRUE;

		case MSG_FF_FB_END:
			AudioPlayerFFFBEnd();
			return TRUE;
	
		case MSG_REPEAT:
			AudioPlayerSwitchMode();
			return TRUE;
				
		case MSG_REPEAT_AB:
			AudioPlayerRepeatAB();
			return TRUE;
				
		case MSG_NUM_0:
		case MSG_NUM_1:
		case MSG_NUM_2:
		case MSG_NUM_3:
		case MSG_NUM_4:
		case MSG_NUM_5:
		case MSG_NUM_6:
		case MSG_NUM_7:
		case MSG_NUM_8:
		case MSG_NUM_9:
			return AudioPlayerNumMsg(Msg);

		default:
			CommonMsgProccess(Msg);
			break;

	}
	return TRUE;
}


//本播放程序不区分是文件播放还是流数据的播放
//return 1 for OK, 0 for error found
bool PlayerControl(void)
{
	uint16_t Msg  = 0;
    
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
    
	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		APP_DBG("enter bt player\n");
		SoundRemind(SOUND_BT_MODE);  
	}
	else
	{
		APP_DBG("enter player\n");
	}
	if(!IsPlayerEnvironmentOk())// 检测是否可以执行播放
	{
		APP_DBG("device not ready! exit player\n");
		return FALSE;
	}
    if(gSys.MuteFlag)
	{
    	gSys.MuteFlag = 0;
		AudioPlayerMute(gSys.MuteFlag);
	}
	if(!AudioPlayerInitialize(1, 1, TRUE))
	{
		APP_DBG("init fail! exit player\n");
		AudioPlayerDeinitialize();
		if(!IsPlayerEnvironmentOk())
		{
			gSys.NextModuleID = GetNextModeIdByProi(gSys.CurModuleID);
		}
		return FALSE;
	}

#ifdef FUNC_TREB_BASS_EN
	if(gSys.EqStatus == 0)
	{
		TrebBassSet(gSys.TrebVal, gSys.BassVal);
	}
#endif
#ifdef FUNC_BREAKPOINT_EN
	AudioSysInfoSetBreakPoint();
#endif

	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);	

#ifdef FUNC_BROWSER_EN
	gPlayContrl->BrowseFlag = FALSE;
#endif

	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	// 消息循环体
	while(Msg != MSG_COMMON_CLOSE)
	{
		// 检查播放状态是否改变，如卡的拔除动作，
		PlayerTimerCB();
		CheckTimer(&gPlayContrl->NumKeyTimer);
		
		Msg = MsgRecv(10);// 消息接收，无消息阻塞20ms，有消息立即返回
		
		if((GetPlayState() == PLAYER_STATE_PLAYING) && IsPlayerEnvironmentOk())
		{
			//gPlayContrl->CurPlayTime = DecoderTaskGetPlayTime();//更新播放时间
		}

		if(!AudioPlayerEvent(Msg))
		{
			// 停止decoder task的编解码工作
			if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
			{
				// 停止播放, 设置断点信息
				if(((GetPlayState() == PLAYER_STATE_PLAYING) || (GetPlayState() == PLAYER_STATE_PAUSE)) 
				&& gPlayContrl->IsBPMached)
				{
					AudioPlayerSetBreakPoint();
					gPlayContrl->IsBPMached = 0;
				}
				AudioPlayerStop();
			}
			break;
		}
		
		// 歌曲播放完毕切换到上一首或者下一首
		if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
		{
			//if((GetPlayState() == PLAYER_STATE_PLAYING)
			//&& (DecoderTaskIsSongEnd() || DecoderTaskIsSongBegin())
			//&& IsPlayerEnvironmentOk())
			{
				APP_DBG("End of Song\n");

#ifdef FUNC_FFFB_END_WITHOUT_AUTO_SWITCH
				if(IsFFFBEndAutoSwitch == FALSE)//快进快退到文件头/尾是否自动切歌
				{
					continue;
				}
#endif
				if(!IsPlayerEnvironmentOk())
				{
					gSys.NextModuleID = GetNextModeIdByProi(gSys.CurModuleID);
					MsgSend(MSG_COMMON_CLOSE);
				}
				//if(DecoderTaskIsSongBegin()
#ifdef FUNC_REC_PLAYBACK_EN
				//||gPlayContrl->RecBackPlayFlag
#endif//
				//)
				{
#ifdef FUNC_FFFB_END_WITHOUT_AUTO_SWITCH
					AudioPlayerStop();
					AudioPlayerSongOpen();
#else
//					AudioPlayerPreSong();
					MsgSend(MSG_PRE_SONG);
#endif
				}
				//else if(gPlayContrl->IsPlayerBeStop) //被强制stop则仍然播放当前歌曲
				{
					AudioPlayerSongOpen();
				}
				//else
				{
					//AudioPlayerNextSong();
					MsgSend(MSG_NEXT_SONG);
				}	
			}
		}
	}

	APP_DBG("exit player\n");

	// save data if any
	// clean the resource if any
	AudioPlayerDeinitialize();
	return TRUE;
}


void SetPlayState(uint8_t state)
{
	if(gPlayContrl != NULL)
	{
		if(gPlayContrl->State != state)
		{
			APP_DBG("PlayCtrl:SetPlayState %d\n", state);
			gPlayContrl->State = state;
		}
	}
}

uint8_t GetPlayState(void)
{
	if(gPlayContrl != NULL)
	{
		return gPlayContrl->State;
	}
	else
	{
		return PLAYER_STATE_IDLE;
	}
}

#else

AUDIO_PLAYER_CONTROL*  gPlayContrl = NULL;
bool IsPlayerEnvironmentOk(void)
{
	return FALSE;
}

void AudioPlayerPlayPause(void)
{

}

void AudioPlayerStop(void)
{
	
}

uint8_t GetPlayState(void)
{	
	return PLAYER_STATE_IDLE;
}
	
#endif

