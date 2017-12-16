//spp proc interface implementation example file, one may implement it by following the defined protocol.
//spp_app_proc.c
//zhouyi,2012.10.30
//maintainer: Halley

#include "type.h"
#include "app_config.h"
#include "folder.h"
#include "Fat_file.h"
#include "msgq.h"
#include "app_msg.h"
#include "adc.h"
#include "Pwm.h"
#include "eq.h"
#include "eq_params.h"
#include "radio_control.h"
#include "dac.h"
#include "wakeup.h"
#include "task_decoder.h"
#include "player_control.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "sys_app.h"
#include "debug.h"
#include "power_management.h"

extern uint32_t GetNextModeId(uint32_t CurModeId);
bool IsPlayerEnvironmentOk(void);

#if 1

#include "spp_app_proc.h"


uint32_t UserProcSppData(void);

//static uint8_t FolderCacheItemNum = 0; //current available cache item number.

#define FOLDER_SEARCH_ERROR     0x7F
#define MAX_FOLDER_CACHE_LEVEL  8

#define FOLDER_CACHE FOLDER


//uint8_t FileGetLongName(FAT_FILE* File, uint8_t* LongFileName, uint8_t GetMaxLength)
//{
////    mvprintf("FileGetLongName....\n");
//    return 1;
//}
//
//uint8_t FolderGetLongName(FOLDER* Folder, uint8_t* LongFileName, uint8_t GetMaxLength)
//{
////    mvprintf("FolderGetLongName....\n");
//    return 1;
//}
//uint8_t FileOpenByNum(FAT_FILE* File, FOLDER* Folder, uint16_t FileNum)
//{
////    mvprintf("FileOpenByNum(%d/%d)....\n", Folder ? Folder->FolderNum : -1, FileNum);
//    return 1;
//}
//uint8_t FolderOpenByNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t FolderNum)
//{
//    if(!ParentFolder)Folder->Level = 0;
//    else Folder->Level = ParentFolder->Level + 1;
//
//    if(ParentFolder)FolderNum = 10 * (ParentFolder->Level+1) + FolderNum;//test
//
//    Folder->FolderNum = FolderNum;
//
////    mvprintf("FolderOpenByNum(%d/%d)....\n", ParentFolder ? ParentFolder->FolderNum: 0, FolderNum);
//    return 1;
//}

///$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//------------HEART SYNC-----------------
#define MASK_CMD_SYNC            0x0000

//------------MODE-----------------------
#define MASK_CMD_MODE            0x0100
#define CMD_MODE_NEXT_MODE       0x0101
#define CMD_MODE_SELECT_MODE     0x0102
#define CMD_MODE_GET_MODE        0x0103
#define CMD_MODE_GET_DEVICE_LINK 0x0104

//------------FS-------------------------
#define MASK_CMD_FS              0x0200
#define CMD_FS_GET_FOLDER_NUM    0x0201
#define CMD_FS_GET_FILE_NUM      0x0202
#define CMD_FS_GET_FOLDER_INFO   0x0203
#define CMD_FS_GET_FILE_INFO     0x0204

//------------Play------------------------
#define MASK_CMD_PLAY            0x0300
#define CMD_PLAY_PLAY            0x0301
#define CMD_PLAY_PAUSE           0x0302
#define CMD_PLAY_STOP            0x0303
#define CMD_PLAY_NEXT            0x0304
#define CMD_PLAY_PRE             0x0305
#define CMD_PLAY_PLAYPAUSE       0x0306
#define CMD_PLAY_FF              0x0307
#define CMD_PLAY_FB              0x0308
#define CMD_PLAY_STOP_FF_FB      0x0309
#define CMD_PLAY_SEEK_PLAY_TIME  0x030A
#define CMD_PLAY_NEXT_FOLDER     0x030B
#define CMD_PLAY_PRE_FOLDER      0x030C
#define CMD_PLAY_SELECT_SONG     0x030D
#define CMD_PLAY_GET_STATUS      0x0320
#define CMD_PLAY_GET_SONG_INFO   0x0321
#define CMD_PLAY_SET_REP_MODE    0x0322
#define CMD_PLAY_GET_REP_MODE    0x0323
#define CMD_PLAY_ENABLE_FOLDER   0x0324
#define CMD_PLAY_DISABLE_FOLDER  0x0325
#define CMD_PLAY_GET_TITLE       0x0330
#define CMD_PLAY_GET_ARTIST      0x0331
#define CMD_PLAY_GET_ALBUM       0x0332
#define CMD_PLAY_GET_COMMENT     0x0333
#define CMD_PLAY_GET_YEAR        0x0334

//-------------------Audio---------------------
#define MASK_CMD_AUDIO           0x0400
#define CMD_AUDIO_VOL_ADD        0x0401
#define CMD_AUDIO_VOL_SUB        0x0402
#define CMD_AUDIO_SET_VOL        0x0403
#define CMD_AUDIO_GET_VOL        0x0404
#define CMD_AUDIO_SET_EQ         0x0405
#define CMD_AUDIO_GET_EQ         0x0406
#define CMD_AUDIO_MUTE           0x0407
#define CMD_AUDIO_UNMUTE         0x0408
#define CMD_AUDIO_SET_LINEIN_CHANNEL     0x0409
#define CMD_AUDIO_SET_LINEIN_GAIN        0x040A
#define CMD_AUDIO_ENABLE_MIC     0x040B
#define CMD_AUDIO_DISABLE_MIC    0x040C

#define CMD_AUDIO_GET_USER_EQ_ITEM_NUM   0x0421
#define CMD_AUDIO_SET_USER_EQ_PARAMETER  0x0422

//------------------MASK_CMD_RTC---------------------------
#define MASK_CMD_RTC             0x0500

//-----------------MASK_CMD_GPIO---------------------------
#define MASK_CMD_GPIO            0x0600

//------------------MASK_CMD_ADC---------------------------
#define MASK_CMD_ADC             0x0700

//------------------MASK_CMD_PWM---------------------------
#define MASK_CMD_PWM             0x0800

//------------------MASK_CMD_WAKEUP------------------------
#define MASK_CMD_WAKEUP          0x0900

//------------------MASK_CMD_DATA--------------------------
#define MASK_CMD_DATA            0x1000
//start file transfering
#define CMD_DATA_FILENAME        0x1001 //full path name data size is 250 means next package gives rest of name
#define CMD_DATA_FILENAME_CON    0x1002 //full path name continued if data is full
#define CMD_DATA_FILE_DATA_LEN   0x1003 //file size, if 0 means not know, this package can be omitted.
//file data
#define CMD_DATA_FILE_DATA       0x1004 //max payload size if (250-1) Bytes, the first byte is the package id(from 0-255)!!!
//file end
#define CMD_DATA_FILE_END        0x1005 //end of file transfer
//user define data format,also O18 can give LRC/Power/Others info to host by send this cmd.
#define CMD_DATA_USER_DATA       0x1010 //user defined data, may not used for the recent time.


//----------------------------------------------
#include <string.h>
#include "type.h"
#include "gpio.h"
#include "TimeOut.h"

#define MAX_CMD_DATA_LEN	32						//命令数据域最大长度
#define RX_QUEUE_LENGTH 	(MAX_CMD_DATA_LEN + 10)		//串口中断接收缓冲区长度

uint8_t gSlaveCmd[MAX_CMD_DATA_LEN + 3];					//长度域、命令码域、数据域

#define SLAVE_RESP_BUF_SIZE 251
static uint8_t* SlaveRespBuf = NULL;
uint8_t SuspendTick = 0;

#define gSlaveCmdData		(&gSlaveCmd[3])
#define gSlaveCmdCodeH		(gSlaveCmd[1])
#define gSlaveCmdCodeL		(gSlaveCmd[2])

#define DEBUG_SPP_SLAVE_INFO
enum {
	SELECT_MODE_CMD_STATE_DONE = 0,
	SELECT_MODE_CMD_STATE_PENDING
};

uint8_t	gSelectModeCmdState = SELECT_MODE_CMD_STATE_DONE;

//----------------------------------------------
void SlaveSendResp(uint16_t Cmd, uint8_t* Buf, uint8_t Len)
{
	uint8_t CheckSum;
	uint8_t RespHead[5];
	uint8_t LenTemp;
	CheckSum = Len + (BYTE1(Cmd) | 0x80) + BYTE0(Cmd);
	LenTemp = Len;
	while(LenTemp)
	{
		CheckSum += Buf[LenTemp - 1];
		LenTemp--;
	}
	CheckSum = 0 - CheckSum;
	RespHead[0] = 0x55;
	RespHead[1] = 0xAA;
	RespHead[2] = Len;
	RespHead[3] = (BYTE1(Cmd) | 0x80);
	RespHead[4] = (BYTE0(Cmd));
#ifdef DEBUG_SPP_SLAVE_INFO
	DBG("0x%02x %02x %02x %02x %02x:", RespHead[0], RespHead[1], RespHead[2], RespHead[3], RespHead[4]);
#endif
	SppSendData(RespHead, 5);
	if(Len)
	{
#ifdef DEBUG_SPP_SLAVE_INFO
		{
			uint16_t i;
			for(i = 0; i < Len; i++)
			{
				DBG("%02x ", Buf[i]);
			}
		}
#endif
		SppSendData(Buf, Len);
	}
#ifdef DEBUG_SPP_SLAVE_INFO
	DBG("%02x(checksun)\n\n", CheckSum);
#endif
	SppSendData(&CheckSum, 1);
}

void SlaveSysModeProcess(uint16_t cmd)
{
	uint8_t temp = 0;
//	TIMER Timer;
	switch(cmd)
	{
		case SLAVE_NEXT_MODE:
			SuspendTick = 0;
			//TryGotoNextMode(gSys.SystemMode);
			gSys.NextModuleID = GetNextModeId(gSys.CurModuleID);
			MsgSend(MSG_MODE);
			SetModeSwitchState(MODE_SWITCH_STATE_START);
			SlaveRespBuf[0] = (uint8_t)gSys.NextModuleID;
			SlaveSendResp(SLAVE_NEXT_MODE, SlaveRespBuf, 1);
			gSelectModeCmdState = SELECT_MODE_CMD_STATE_PENDING;
			break;

		case SLAVE_SELECT_MODE:
			SuspendTick = 0;
			gSys.NextModuleID = (uint32_t)gSlaveCmdData[0];
			DBG("NextModuleID %LX \n", gSys.NextModuleID);
			if(IsModeEnvOk(gSys.NextModuleID) == FALSE)
			{
				gSys.NextModuleID = GetNextModeId(gSys.NextModuleID);
			}
			MsgSend(MSG_MODE);
			SetModeSwitchState(MODE_SWITCH_STATE_START);
			SlaveRespBuf[0] = (uint8_t)gSys.NextModuleID;
//			SlaveSendResp(SLAVE_SELECT_MODE, SlaveRespBuf, 1);
			SlaveSendResp(SLAVE_SELECT_MODE_PENDING, SlaveRespBuf, 1);
			gSelectModeCmdState = SELECT_MODE_CMD_STATE_PENDING;
			break;

		case SLAVE_GET_MODE:
			SuspendTick = 0;
			SlaveRespBuf[0] = (uint8_t)gSys.CurModuleID;
			SlaveSendResp(SLAVE_GET_MODE, SlaveRespBuf, 1);
			break;

		case SLAVE_GET_DEVICE_LINK:
			SuspendTick = 0;
			temp = IsUDiskLink() ? 1 : 0;
			temp |= IsCardLink() ? 2 : 0;
			temp |= IsLineInLink() ? 4 : 0;
			temp |= IsPcLink() ? 8 : 0;
			SlaveRespBuf[0] = temp;
			SlaveSendResp(SLAVE_GET_DEVICE_LINK, SlaveRespBuf, 1);
			break;
	}
}
void SlaveFileSystemProcess(uint16_t cmd)
{
	uint8_t temp = 0;
	FOLDER  folder;
	FOLDER  ParentFolder;
	FAT_FILE    file;
	int16_t 	sTempWord;
	int16_t 	sTempWord2;
	uint16_t* 	P;

	if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
	{
		return;
	}

	switch(cmd)
	{
#if 1//def FUNC_FOLDER_EN
		case SLAVE_GET_VALID_FOLDER_SUM:
			SuspendTick = 0;
			SlaveRespBuf[0] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum);
			SlaveRespBuf[1] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum);
			SlaveRespBuf[2] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum);
			SlaveRespBuf[3] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum);
			SlaveSendResp(SLAVE_GET_VALID_FOLDER_SUM, SlaveRespBuf, 4);
			break;
#endif
		case SLAVE_GET_FILE_SUM:
			SuspendTick = 0;
			SlaveRespBuf[0] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.FileSum);
			SlaveRespBuf[1] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.FileSum);
			SlaveSendResp(SLAVE_GET_FILE_SUM, SlaveRespBuf, 2);
			break;
		case SLAVE_GET_FOLDER_INFO:
			BYTE1(sTempWord) = gSlaveCmdData[0];
			BYTE0(sTempWord) = gSlaveCmdData[1];
			BYTE1(sTempWord2) = gSlaveCmdData[2];
			BYTE0(sTempWord2) = gSlaveCmdData[3];
			if(sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum)
			{
				break;
			}
			//打开相应文件夹
			if(sTempWord == 0)
			{
				if(!FolderOpenByNum(&folder, NULL, sTempWord2))
				{
					break;
				}
			}
			else
			{
				if(!FolderOpenByNum(&ParentFolder, NULL, sTempWord))
				{
					break;
				}
				if(!FolderOpenByNum(&folder, &ParentFolder, sTempWord2))
				{
					break;
				}
			}
			SuspendTick = 0;
			SlaveRespBuf[0] = (uint8_t)(folder.FolderNum >> 8);
			SlaveRespBuf[1] = (uint8_t)folder.FolderNum;
			SlaveRespBuf[2] = (uint8_t)(folder.ValidFolderNum >> 8);
			SlaveRespBuf[3] = (uint8_t)folder.ValidFolderNum;
			SlaveRespBuf[4] = (uint8_t)(folder.StartFileNum >> 8);
			SlaveRespBuf[5] = (uint8_t)folder.StartFileNum;
			SlaveRespBuf[6] = (uint8_t)(folder.IncFileCnt >> 8);
			SlaveRespBuf[7] = (uint8_t)folder.IncFileCnt;
			SlaveRespBuf[8] = (uint8_t)(folder.IncFolderCnt >> 8);
			SlaveRespBuf[9] = (uint8_t)folder.IncFolderCnt;
			SlaveRespBuf[10] = (uint8_t)(folder.RecFileCnt >> 8);
			SlaveRespBuf[11] = (uint8_t)folder.RecFileCnt;
			SlaveRespBuf[12] = (uint8_t)(folder.RecFolderCnt >> 8);
			SlaveRespBuf[13] = (uint8_t)folder.RecFolderCnt;
			memcpy(&SlaveRespBuf[14], &folder.ShortName[0], 8);
			if(FolderGetLongName(&folder, &SlaveRespBuf[23], 66))
			{
				for(P = (uint16_t*)(&SlaveRespBuf[23]); *P != 0; P++)//计算长文件名长度
				{
					temp++;
				}
				SlaveRespBuf[22] = temp * 2;
			}
			else
			{
				SlaveRespBuf[22] = 0;
			}
			SlaveSendResp(SLAVE_GET_FOLDER_INFO, SlaveRespBuf, 23 + SlaveRespBuf[22]);
			break;
		case SLAVE_GET_FILE_INFO:
			//sTempWord = *(uint16_t*)(&gSlaveCmdData[2]);
			BYTE1(sTempWord) = gSlaveCmdData[0];
			BYTE0(sTempWord) = gSlaveCmdData[1];
			BYTE1(sTempWord2) = gSlaveCmdData[2];
			BYTE0(sTempWord2) = gSlaveCmdData[3];
			if(sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum)
			{
				break;
			}
			//打开相应文件夹
			if(sTempWord != 0)
			{
				if(!FolderOpenByNum(&folder, NULL, sTempWord))
				{
					break;
				}

				if(!FileOpenByNum(&file, &folder, sTempWord2))
				{
					break;
				}
			}
			else
			{
				if(!FileOpenByNum(&file, NULL, sTempWord2))
				{
					break;
				}
			}
			SuspendTick = 0;
			SlaveRespBuf[0] = (uint8_t)(file.FileNumInDisk >> 8);
			SlaveRespBuf[1] = (uint8_t)file.FileNumInDisk;
			SlaveRespBuf[2] = (uint8_t)(file.FileNumInFolder >> 8);
			SlaveRespBuf[3] = (uint8_t)file.FileNumInFolder;
			SlaveRespBuf[4] = (uint8_t)(file.FolderNum >> 8);
			SlaveRespBuf[5] = (uint8_t)file.FolderNum;
			memcpy(&SlaveRespBuf[6], &file.ShortName[0], 11);
			if(FileGetLongName(&file, &SlaveRespBuf[18], 66))
			{
				for(P = (uint16_t*)(&SlaveRespBuf[18]); *P != 0; P++)//计算长文件名长度
				{
					temp++;
				}
				SlaveRespBuf[17] = temp * 2;
			}
			else
			{
				SlaveRespBuf[17] = 0;
			}
			SlaveSendResp(SLAVE_GET_FILE_INFO, SlaveRespBuf, 18 + SlaveRespBuf[17]);
			FileClose(&file);
			break;
		default:
			break;
	}
}

void SlavePlayCtrlProcess(uint16_t Cmd)
{
//	FOLDER  folder;
	uint8_t temp = 0;
	int16_t 	sTempWord;
	int16_t 	sTempWord2;
	uint16_t*  P;
	uint32_t	sTempDWord;

	switch(Cmd)
	{
		case SLAVE_PLAY:
		case SLAVE_PAUSE:
			SuspendTick = 0;
			MsgSend(MSG_PLAY_PAUSE);
			SlaveSendResp(Cmd, NULL, 0);
			break;
		case SLAVE_STOP:
			SuspendTick = 0;
			MsgSend(MSG_STOP);
			SlaveSendResp(SLAVE_STOP, NULL, 0);
			break;
		case SLAVE_NEXT:
			SuspendTick = 0;
			MsgSend(MSG_NEXT);
			SlaveSendResp(SLAVE_NEXT, NULL, 0);
			break;

		case SLAVE_PRE:
			SuspendTick = 0;
			MsgSend(MSG_PRE);
			SlaveSendResp(SLAVE_PRE, NULL, 0);
			break;

		case SLAVE_FF:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			SuspendTick = 0;
			//MsgSend(SYS_KEY_ID_FAST_FORWARD);
			MsgSend(MSG_FF_START);
			SlaveSendResp(SLAVE_FF, NULL, 0);
			break;

		case SLAVE_FB:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			SuspendTick = 0;
			//MsgSend(SYS_KEY_ID_FAST_BACKWARD);
			MsgSend(MSG_FB_START);
			SlaveSendResp(SLAVE_FB, NULL, 0);
			break;
		case SLAVE_STOP_FF_FB:
			/*
				if(!isUSBorSD())
					break;
				SuspendTick = 0;
				MessageSend(MSG_FIFO_KEY, MSG_FF_FB_END);
				SlaveSendResp(SLAVE_STOP_FF_FB, SlaveRespBuf, 0);
				*/
			break;
		case SLAVE_SEEK_PLAY_TIME:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			SuspendTick = 0;
			BYTE0(sTempDWord) = gSlaveCmdData[3];
			BYTE1(sTempDWord) = gSlaveCmdData[2];
			BYTE2(sTempDWord) = gSlaveCmdData[1];
			BYTE3(sTempDWord) = gSlaveCmdData[0];
			if(sTempDWord <= gPlayContrl->CurSongInfo->duration)
			{
				gPlayContrl->CurPlayTime = sTempDWord;
				if(GetPlayState() == PLAYER_STATE_PLAYING)
				{
					DecoderTaskSeek(gPlayContrl->CurPlayTime);
				}
			}
			SlaveSendResp(SLAVE_SEEK_PLAY_TIME, SlaveRespBuf, 0);
			break;
		case SLAVE_ENABLE_FOLDER: // 文件夹播放模式开关
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			SuspendTick = 0;
			gPlayContrl->PlayFolderFlag = TRUE;
			if(gPlayContrl->FileOpenSuccessFlag)
			{
				gPlayContrl->CurFolderIndex = gPlayContrl->PlayerFile.FolderNum;
				gPlayContrl->CurFileIndex   = gPlayContrl->PlayerFile.FileNumInFolder;
			}
			SlaveSendResp(SLAVE_ENABLE_FOLDER, NULL, 0);
			break;
		case SLAVE_DISABLE_FOLDER: // 文件夹播放模式开关
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			SuspendTick = 0;
			gPlayContrl->PlayFolderFlag = FALSE;
			gPlayContrl->CurFolderIndex = 1;
			if(gPlayContrl->FileOpenSuccessFlag)
			{
				gPlayContrl->CurFileIndex = gPlayContrl->PlayerFile.FileNumInDisk;
			}
			else
			{
				gPlayContrl->CurFileIndex = 1;
			}
			SlaveSendResp(SLAVE_DISABLE_FOLDER, NULL, 0);
			break;
		case SLAVE_NEXT_FOLDER:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			if(!gPlayContrl->PlayFolderFlag)
			{
				break;
			}
			AudioPlayerStop();
			gPlayContrl->CurFolderIndex++;
			gPlayContrl->CurFolderIndex %= gPlayContrl->ValidFolderCnt;

			if(!FolderOpenByNum(&gPlayContrl->FolderHandle,
			                    NULL,
			                    gPlayContrl->CurFolderIndex
			                   ))
			{
				break;
			}
			gPlayContrl->CurFileIndex = 1;

            AudioPlayerPlayPause();

			SuspendTick = 0;
			SlaveSendResp(SLAVE_NEXT_FOLDER, SlaveRespBuf, 0);
			break;
		case SLAVE_PRE_FOLDER:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			if(!gPlayContrl->PlayFolderFlag)
			{
				break;
			}
			SuspendTick = 0;
			AudioPlayerStop();
			if(gPlayContrl->CurFolderIndex > 1)
			{
				gPlayContrl->CurFolderIndex--;
			}
			else
			{
				gPlayContrl->CurFolderIndex = gPlayContrl->ValidFolderCnt;
			}
			if(!FolderOpenByNum(&gPlayContrl->FolderHandle, NULL, gPlayContrl->CurFolderIndex))
			{
				break;
			}
			gPlayContrl->CurFileIndex = 1;

            AudioPlayerPlayPause();

			SlaveSendResp(SLAVE_PRE_FOLDER, SlaveRespBuf, 0);
			break;
		case SLAVE_PLAY_PAUSE:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			SuspendTick = 0;
			MsgSend(MSG_PLAY_PAUSE);
			SlaveSendResp(SLAVE_PLAY_PAUSE, NULL, 0);
			break;
		case SLAVE_SELECT_SONG:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}

			BYTE1(sTempWord) = gSlaveCmdData[0];
			BYTE0(sTempWord) = gSlaveCmdData[1];
			BYTE1(sTempWord2) = gSlaveCmdData[2];
			BYTE0(sTempWord2) = gSlaveCmdData[3];
//			if((sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum)||(sTempWord == 0))
//				break;
//            if(!FolderOpenByValidNum(&gPlayContrl->FolderHandle, NULL, *(uint16_t*)gSlaveCmdData))
			if((sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum) || (sTempWord == 0))
			{
				break;
			}
			if(!FolderOpenByNum(&gPlayContrl->FolderHandle, NULL, sTempWord))

			{
				break;
			}
			if((sTempWord2 <= 0) || (sTempWord2 > gPlayContrl->FolderHandle.RecFileCnt))
			{
				break;
			}
#ifdef FUNC_IR_KEYPAD_EN
			StopTimer(&gPlayContrl->NumKeyTimer);
#endif
			// 停止播放
			AudioPlayerStop();
			gPlayContrl->CurFileIndex = sTempWord2;
			gPlayContrl->CurFolderIndex = sTempWord;

            AudioPlayerPlayPause();

			SuspendTick = 0;
			SlaveSendResp(SLAVE_SELECT_SONG, SlaveRespBuf, 1);
			break;

		case SLAVE_GET_PLAY_STATUS:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}

			SuspendTick = 0;
			SlaveRespBuf[0] = gSys.CurModuleID;
			SlaveRespBuf[1] = gPlayContrl->State;
			SlaveRespBuf[2] = BYTE1(gPlayContrl->CurFolderIndex);
			SlaveRespBuf[3] = BYTE0(gPlayContrl->CurFolderIndex);
			SlaveRespBuf[4] = BYTE1(gPlayContrl->FolderHandle.ValidFolderNum);
			SlaveRespBuf[5] = BYTE0(gPlayContrl->FolderHandle.ValidFolderNum);
			SlaveRespBuf[6] = BYTE1(gPlayContrl->CurFileIndex);
			SlaveRespBuf[7] = BYTE0(gPlayContrl->CurFileIndex);
			SlaveRespBuf[8] = BYTE1(gPlayContrl->PlayerFile.FileNumInFolder);
			SlaveRespBuf[9] = BYTE0(gPlayContrl->PlayerFile.FileNumInFolder);
			SlaveRespBuf[10] = BYTE3(gPlayContrl->CurPlayTime);
			SlaveRespBuf[11] = BYTE2(gPlayContrl->CurPlayTime);
			SlaveRespBuf[12] = BYTE1(gPlayContrl->CurPlayTime);
			SlaveRespBuf[13] = BYTE0(gPlayContrl->CurPlayTime);
			SlaveSendResp(SLAVE_GET_PLAY_STATUS, SlaveRespBuf, 14);
			break;
		case SLAVE_GET_SONG_INFO:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}

			SuspendTick = 0;
			SlaveRespBuf[0] = gPlayContrl->CurSongInfo->stream_type;
			SlaveRespBuf[1] = (uint8_t)gPlayContrl->CurSongInfo->num_channels;
			SlaveRespBuf[2] = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 24);
			SlaveRespBuf[3] = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 16);
			SlaveRespBuf[4]  = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 8);
			SlaveRespBuf[5] = (uint8_t)gPlayContrl->CurSongInfo->sampling_rate;

			SlaveRespBuf[6] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 24);
			SlaveRespBuf[7] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 16);
			SlaveRespBuf[8] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 8);
			SlaveRespBuf[9] = (uint8_t)gPlayContrl->CurSongInfo->bitrate;
			SlaveRespBuf[10] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 24);
			SlaveRespBuf[11] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 16);
			SlaveRespBuf[12] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 8);
			SlaveRespBuf[13] = (uint8_t)gPlayContrl->CurSongInfo->duration;
//			SlaveRespBuf[14] = (uint8_t)(gSongInfo.FileHeaderLen>>24);
//			SlaveRespBuf[15] = (uint8_t)(gSongInfo.FileHeaderLen>>16);
//			SlaveRespBuf[16] = (uint8_t)(gSongInfo.FileHeaderLen>>8);
//			SlaveRespBuf[17] = (uint8_t)gSongInfo.FileHeaderLen;
			SlaveRespBuf[14] = (uint8_t)gPlayContrl->CurSongInfo->vbr_flag;
			//SlaveRespBuf[15] = 0;//audio_decoder get_mpeg_version;//gSongInfo.Mp3MpegVersion;
			//SlaveRespBuf[16] = audio_decoder->song_info->stream_type;//1;//gSongInfo.Mp3Layer;
			//SlaveSendResp(SLAVE_GET_SONG_INFO, SlaveRespBuf, 17);
			SlaveSendResp(SLAVE_GET_SONG_INFO, SlaveRespBuf, 15); //17
			break;
		case SLAVE_SET_REPEAT_MODE:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			SuspendTick = 0;
			if(gSlaveCmdData[0] < PLAY_MODE_SUM)
			{
				gPlayContrl->CurPlayMode = gSlaveCmdData[0];
			}

			SlaveSendResp(SLAVE_SET_REPEAT_MODE, SlaveRespBuf, 0);
			break;
		case SLAVE_GET_REPEAT_MODE:
			SuspendTick = 0;
			SlaveRespBuf[0] =  gPlayContrl->CurPlayMode;
			SlaveSendResp(SLAVE_GET_REPEAT_MODE, SlaveRespBuf, 1);
			break;
#if 1//def FUNC_TAG_EN	
		case SLAVE_GET_TITLE:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}

			SuspendTick = 0;
			SlaveRespBuf[0] = 0;//gTagInfo.TagType;
			SlaveRespBuf[1] = gPlayContrl->CurSongInfo->char_set;
			for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->title[0]); *P != 0; P++)//计算Artist长度
			{
				temp++;
			}
			temp = temp * 2;
			if(temp > 30)
			{
				temp = 30;
			}
			SlaveRespBuf[2] = temp;
			memcpy(&SlaveRespBuf[3], &gPlayContrl->CurSongInfo->title[0], temp);
			SlaveSendResp(SLAVE_GET_TITLE, SlaveRespBuf, temp + 3);
			break;
		case SLAVE_GET_ARTIST:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}

			SuspendTick = 0;
			SlaveRespBuf[0] = 0;//gTagInfo.TagType;
			SlaveRespBuf[1] = gPlayContrl->CurSongInfo->char_set;
			for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->artist[0]); *P != 0; P++)//计算Artist长度
			{
				temp++;
			}
			temp = temp * 2;
			if(temp > 30)
			{
				temp = 30;
			}
			SlaveRespBuf[2] = temp;
			memcpy(&SlaveRespBuf[3], &gPlayContrl->CurSongInfo->artist[0], temp);
			SlaveSendResp(SLAVE_GET_ARTIST, SlaveRespBuf, temp + 3);
			break;
		case SLAVE_GET_ALBUM:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}

			SuspendTick = 0;
			SlaveRespBuf[0] = 0;//gTagInfo.TagType;
			SlaveRespBuf[1] = gPlayContrl->CurSongInfo->char_set;
			for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->album[0]); *P != 0; P++)//计算Artist长度
			{
				temp++;
			}
			temp = temp * 2;
			if(temp > 30)
			{
				temp = 30;
			}
			SlaveRespBuf[2] = temp;
			memcpy(&SlaveRespBuf[3], &gPlayContrl->CurSongInfo->album[0], temp);
			SlaveSendResp(SLAVE_GET_ALBUM, SlaveRespBuf, temp + 3);
			break;
		case SLAVE_GET_COMMENT:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			SuspendTick = 0;
			SlaveRespBuf[0] = 0;//gTagInfo.TagType;
			SlaveRespBuf[1] = gPlayContrl->CurSongInfo->char_set;
			for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->comment[0]); *P != 0; P++)//计算Artist长度
			{
				temp++;
			}
			temp = temp * 2;
			if(temp > 30)
			{
				temp = 30;
			}
			SlaveRespBuf[2] = temp;
			memcpy(&SlaveRespBuf[3], &gPlayContrl->CurSongInfo->comment[0], temp);
			SlaveSendResp(SLAVE_GET_COMMENT, SlaveRespBuf, temp + 3);
			break;
		case SLAVE_GET_YEAR:
			if(!IsPlayerEnvironmentOk())
			{
				break;
			}
			if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
			{
				break;
			}
			SuspendTick = 0;
			SlaveRespBuf[0] = 0;//gTagInfo.TagType;
			SlaveRespBuf[1] = gPlayContrl->CurSongInfo->char_set;
			for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->year[0]); *P != 0; P++)//计算Artist长度
			{
				temp++;
			}
			temp = temp * 2;
			if(temp > 30)
			{
				temp = 30;
			}
			SlaveRespBuf[2] = temp;
			memcpy(&SlaveRespBuf[3], &gPlayContrl->CurSongInfo->year[0], temp);
			SlaveSendResp(SLAVE_GET_YEAR, SlaveRespBuf, temp + 3);
			break;
#endif
	}
}

void SlaveAudioProcess(uint16_t Cmd)
{
	if(!gPlayContrl)
	{
		return;
	}
	switch(Cmd)
	{
		case SLAVE_VOLADD:
			SuspendTick = 0;
			MsgSend(MSG_VOL_UP);
			SlaveSendResp(SLAVE_VOLADD, SlaveRespBuf, 0);
			break;

		case SLAVE_VOLSUB:
			SuspendTick = 0;
			MsgSend(MSG_VOL_DW);
			SlaveSendResp(SLAVE_VOLSUB, SlaveRespBuf, 0);
			break;
		case SLAVE_SET_VOL:
			SuspendTick = 0;
			if(gSlaveCmdData[0] <= MAX_VOLUME)
			{
				gSys.Volume = gSlaveCmdData[0];
			}
#ifdef FUNC_LINEIN_EN
			if(gSys.CurModuleID == MODULE_ID_LINEIN)	//line in
			{
				SetSysVol();
			}
#endif
#ifdef FUNC_RADIO_EN
			else if(gSys.CurModuleID == MODULE_ID_RADIO)
			{
				//RadioSetVolume(gSys.Volume);
			}
			else
#endif
			{
				SetSysVol();
			}
			SlaveSendResp(SLAVE_SET_VOL, SlaveRespBuf, 0);
			break;
		case SLAVE_GET_VOL:
			SuspendTick = 0;
			SlaveRespBuf[0] = gSys.Volume;
			SlaveSendResp(SLAVE_GET_VOL, SlaveRespBuf, 1);
			break;
		case SLAVE_SET_EQ:
			if(gSlaveCmdData[0] < EQ_STYLE_SUM)
			{
				SuspendTick = 0;
                if(gSys.Eq == EQ_STYLE_OFF && gSlaveCmdData[0] != EQ_STYLE_OFF)
                {
                    EqEnable();
                }
                else if(gSys.Eq != EQ_STYLE_OFF && gSlaveCmdData[0] == EQ_STYLE_OFF)
                {
                    EqDisable();
                }
				gSys.Eq = gSlaveCmdData[0];
				if(gPlayContrl->CurSongInfo != NULL)
				{
					uint32_t SampleRateTmp = DacAdcSampleRateGet();
					if(SampleRateTmp == 0)
					{
						SampleRateTmp = 44100;
					}

					EqStyleSelect(SampleRateTmp, gSys.Eq);
				}
				SlaveSendResp(SLAVE_SET_EQ, SlaveRespBuf, 0);
			}
			break;
		case SLAVE_GET_EQ:
			SuspendTick = 0;
			SlaveRespBuf[0] = gSys.Eq;
			SlaveSendResp(SLAVE_GET_EQ, SlaveRespBuf, 1);
			break;
		case SLAVE_MUTE:
			SuspendTick = 0;
			if(!gSys.MuteFlag)
			{
				gSys.MuteFlag = TRUE;
				DacSoftMuteSet(TRUE, TRUE);
			}
			SlaveSendResp(SLAVE_MUTE, SlaveRespBuf, 0);
			break;
		case SLAVE_UNMUTE:
			SuspendTick = 0;
			if(gSys.MuteFlag)
			{
				gSys.MuteFlag = FALSE;
#ifdef FUNC_LINEIN_EN
				if(gSys.CurModuleID == MODULE_ID_LINEIN)	//line in
				{
					SetSysVol();
				}
				else
#endif
#ifdef FUNC_RADIO_EN
					if(gSys.CurModuleID == MODULE_ID_RADIO)
					{
						//RadioSetVolume(gSys.Volume);
					}
					else
#endif
					{
						SetSysVol();
					}
				DacSoftMuteSet(FALSE, FALSE);
			}
			SlaveSendResp(SLAVE_UNMUTE, SlaveRespBuf, 0);
			break;
		case SLAVE_SET_LINEIN_GAIN:		//增益最大64级，0 ~63
			SuspendTick = 0;
			if(gSlaveCmdData[0] <= 63)
			{
				//SetLineInVolume(gSlaveCmdData[0]);
			}
			SlaveSendResp(SLAVE_SET_LINEIN_GAIN, SlaveRespBuf, 0);
			break;

//		case SLAVE_ENABLE_MIC:		//MIC 增益M, 0 = IC_GAIN_2DB, 1 = IC_GAIN_12DB, 2 = IC_GAIN_22DB, 3 = IC_GAIN_32DB
//			SuspendTick = 0;
//			if(gSlaveCmdData[0] <= 3)
//				InDacLineInGain(gSlaveCmdData[0]);
//			SlaveSendResp(SLAVE_ENABLE_MIC, SlaveRespBuf, 0);
//			break;
//		case SLAVE_DISABLE_MIC:
//			SuspendTick = 0;
//			MicDisable();
//			SlaveSendResp(SLAVE_DISABLE_MIC, SlaveRespBuf, 0);
//			break;

		default:
			break;
	}

}
void SlaveRtcProcess(uint16_t Cmd)
{

	uint8_t uRtcAlarmNum;
	uint8_t uAlarmMode;
	uint8_t uAlarmData;
	RTC_DATE_TIME uRtcTime;
	RTC_DATE_TIME uAlarmTime;
	RTC_LUNAR_DATE uLunarDate;

	uint8_t temp = 0;

	switch(Cmd)
	{
		case SLAVE_SET_TIME:
			SuspendTick = 0;
			BYTE0(uRtcTime.Year) = gSlaveCmdData[1];
			BYTE1(uRtcTime.Year) = gSlaveCmdData[0];
			uRtcTime.Mon = gSlaveCmdData[2];
			uRtcTime.Date = gSlaveCmdData[3];
			uRtcTime.Hour = gSlaveCmdData[4];
			uRtcTime.Min = gSlaveCmdData[5];
			uRtcTime.Sec = gSlaveCmdData[6];
			RtcSetCurrTime(&uRtcTime);
			SlaveSendResp(SLAVE_SET_TIME, SlaveRespBuf, 0);
			break;
		case SLAVE_GET_TIME:
			SuspendTick = 0;
			RtcGetCurrTime(&uRtcTime);
			SlaveRespBuf[0] = BYTE1(uRtcTime.Year);
			SlaveRespBuf[1] = BYTE0(uRtcTime.Year);
			SlaveRespBuf[2] = uRtcTime.Mon;
			SlaveRespBuf[3] = uRtcTime.Date;
			SlaveRespBuf[4] = uRtcTime.WDay;
			SlaveRespBuf[5] = uRtcTime.Hour;
			SlaveRespBuf[6] = uRtcTime.Min;
			SlaveRespBuf[7] = uRtcTime.Sec;
			SlaveSendResp(SLAVE_GET_TIME, SlaveRespBuf, 8);
			break;
#if 1//def FUNC_ALARM_EN
		case SLAVE_SET_ALARM_TIME:

			if((gSlaveCmdData[0] <= 8) && (gSlaveCmdData[0] != 0))	//RtcAlarm must be available
			{
				SuspendTick = 0;
				//OpenAlarm(gSlaveCmdData[0]);
				uRtcAlarmNum = gSlaveCmdData[0];
				uAlarmMode  = gSlaveCmdData[1];
				uAlarmData  = gSlaveCmdData[2];
				BYTE0(uAlarmTime.Year) = gSlaveCmdData[4];
				BYTE1(uAlarmTime.Year) = gSlaveCmdData[3];
				uAlarmTime.Mon = gSlaveCmdData[5];
				uAlarmTime.Date = gSlaveCmdData[6];
				uAlarmTime.WDay = gSlaveCmdData[7];
				uAlarmTime.Hour = gSlaveCmdData[8];
				uAlarmTime.Min = gSlaveCmdData[9];
				uAlarmTime.Sec = gSlaveCmdData[10];
				RtcSetAlarmTime(uRtcAlarmNum, uAlarmMode, uAlarmData, &uAlarmTime);
				SlaveSendResp(SLAVE_SET_ALARM_TIME, SlaveRespBuf, 0);
			}
			break;
		case SLAVE_GET_ALARM_TIME:
			temp = RtcGetAlarmTime(gSlaveCmdData[0], &uAlarmMode, &uAlarmData, &uAlarmTime);
			if((temp != 0) && (temp != 0xff))
			{
				SuspendTick = 0;
				uAlarmMode = temp;
				SlaveRespBuf[0] = uAlarmMode;
				SlaveRespBuf[1] = BYTE1(uAlarmTime.Year);
				SlaveRespBuf[2] = BYTE0(uAlarmTime.Year);
				SlaveRespBuf[3] = uAlarmTime.Mon;
				SlaveRespBuf[4] = uAlarmTime.Date;
				SlaveRespBuf[5] = uAlarmTime.WDay;
				SlaveRespBuf[6] = uAlarmTime.Hour;
				SlaveRespBuf[7] = uAlarmTime.Min;
				SlaveRespBuf[8] = uAlarmTime.Sec;
				SlaveSendResp(SLAVE_GET_ALARM_TIME, SlaveRespBuf, 9);
			}
			break;
		case SLAVE_ENABLE_ALARM:
			SuspendTick = 0;
			if((gSlaveCmdData[0] <= 8) && (gSlaveCmdData[0] != 0))
			{
				//OpenAlarm(gSlaveCmdData[0]);
				RtcAlarmSetStatus(gSlaveCmdData[0], ALARM_STATUS_OPENED);
				SlaveSendResp(SLAVE_ENABLE_ALARM, SlaveRespBuf, 0);
			}
			break;
		case SLAVE_DISABLE_ALARM:
			SuspendTick = 0;
			if((gSlaveCmdData[0] <= 8) && (gSlaveCmdData[0] != 0))
			{
				//CloseAlarm(gSlaveCmdData[0]);
				RtcAlarmSetStatus(gSlaveCmdData[0], ALARM_STATUS_CLOSED);
				SlaveSendResp(SLAVE_DISABLE_ALARM, SlaveRespBuf, 0);
			}
			break;
		case SLAVE_GET_ALARM_STATUS:
			SuspendTick = 0;
			SlaveRespBuf[0] = RtcGetAlarmStatus(gSlaveCmdData[0]);
			SlaveSendResp(SLAVE_GET_ALARM_STATUS, SlaveRespBuf, 1);
			break;

		case SLAVE_GET_ALARM_FLAG:
			SuspendTick = 0;
			SlaveRespBuf[0] = RtcCheckAlarmFlag();
			SlaveSendResp(SLAVE_GET_ALARM_FLAG, SlaveRespBuf, 1);
			break;

#endif
		case SLAVE_GET_MONTH_DAYS:
//			SuspendTick = 0;
//			SlaveRespBuf[0] = GetMonthDays(*(uint16_t*)gSlaveCmdData, gSlaveCmdData[2]);
//			SlaveSendResp(SLAVE_GET_MONTH_DAYS, SlaveRespBuf, 1);
			break;

#if 1//def FUNC_LUNAR_EN
		case SLAVE_SOLAR_TO_LUNAR:
			SuspendTick = 0;
			//uRtcTime.Year = *((uint16_t*)gSlaveCmdData);
			BYTE0(uRtcTime.Year) = gSlaveCmdData[1];
			BYTE1(uRtcTime.Year) = gSlaveCmdData[0];
			uRtcTime.Mon = gSlaveCmdData[2];
			uRtcTime.Date = gSlaveCmdData[3];
			SolarToLunar(&uRtcTime, &uLunarDate);
			SlaveRespBuf[0] = BYTE1(uLunarDate.Year);
			SlaveRespBuf[1] = BYTE0(uLunarDate.Year);
			SlaveRespBuf[2] = uLunarDate.Month;
			SlaveRespBuf[3] = uLunarDate.Date;
			SlaveRespBuf[4] = uLunarDate.MonthDays;
			SlaveRespBuf[5] = (uint8_t)uLunarDate.IsLeapMonth;
			SlaveSendResp(SLAVE_SOLAR_TO_LUNAR, SlaveRespBuf, 6);
			break;
		case SLAVE_GET_HEAVENLY_STEM:
			SuspendTick = 0;
			SlaveRespBuf[0] = GetHeavenlyStem(*(uint16_t*)gSlaveCmdData);
			SlaveSendResp(SLAVE_GET_HEAVENLY_STEM, SlaveRespBuf, 1);
			break;
		case SLAVE_GET_EARTHLY_BRANCH:
			SuspendTick = 0;
			SlaveRespBuf[0] = GetEarthlyBranch(*(uint16_t*)gSlaveCmdData);
			SlaveSendResp(SLAVE_GET_EARTHLY_BRANCH, SlaveRespBuf, 1);
			break;
#endif
		default:
			break;
	}
}

void SlaveGpioProcess(uint16_t Cmd)
{
	uint32_t	Temp;
	switch(Cmd)
	{
		case SLAVE_SET_GPIO_REG:
			SuspendTick = 0;
			BYTE0(Temp) = gSlaveCmd[7];
			BYTE1(Temp) = gSlaveCmd[6];
			BYTE2(Temp) = gSlaveCmd[5];
			BYTE3(Temp) = gSlaveCmd[4];
			GpioSetReg(gSlaveCmdData[0], Temp);
			SlaveSendResp(SLAVE_SET_GPIO_REG, SlaveRespBuf, 0);
			break;
		case SLAVE_GET_GPIO_REG:
			SuspendTick = 0;
			Temp = GpioGetReg(gSlaveCmdData[0]);
			SlaveRespBuf[3] = BYTE0(Temp);
			SlaveRespBuf[2] = BYTE1(Temp);
			SlaveRespBuf[1] = BYTE2(Temp);
			SlaveRespBuf[0] = BYTE3(Temp);
			SlaveSendResp(SLAVE_GET_GPIO_REG, SlaveRespBuf, 4);
			break;
		case SLAVE_SET_GPIO_REG_BIT:
			SuspendTick = 0;
			BYTE0(Temp) = gSlaveCmd[7];
			BYTE1(Temp) = gSlaveCmd[6];
			BYTE2(Temp) = gSlaveCmd[5];
			BYTE3(Temp) = gSlaveCmd[4];
			GpioSetRegBits(gSlaveCmdData[0], Temp);
			SlaveSendResp(SLAVE_SET_GPIO_REG_BIT, SlaveRespBuf, 0);
			break;
		case SLAVE_CLR_GPIO_REG_BIT:
			SuspendTick = 0;
			BYTE0(Temp) = gSlaveCmd[7];
			BYTE1(Temp) = gSlaveCmd[6];
			BYTE2(Temp) = gSlaveCmd[5];
			BYTE3(Temp) = gSlaveCmd[4];
			GpioClrRegBits(gSlaveCmdData[0], Temp);
			SlaveSendResp(SLAVE_CLR_GPIO_REG_BIT, SlaveRespBuf, 0);
			break;
		default:
			break;
	}
}

void SlaveAdcProcess(uint16_t Cmd)
{
	uint8_t i = 0;
	uint16_t 	rtn;
	switch(Cmd)
	{
		case SLAVE_ENABLE_ADC_CHANNEL:
			SuspendTick = 0;
			switch(gSlaveCmdData[0])
			{
				case 0:
					SarAdcGpioSel(ADC_CHANNEL_LDOIN);
					break;
				case 1:
					break;
				case 2:
					SarAdcGpioSel(ADC_CHANNEL_B5);
					break;
				case 3:
					SarAdcGpioSel(ADC_CHANNEL_B6);
					break;
				case 4:
					SarAdcGpioSel(ADC_CHANNEL_B7);
					break;
				case 5:
					SarAdcGpioSel(ADC_CHANNEL_B22);
					break;
				case 6:
					SarAdcGpioSel(ADC_CHANNEL_B23);
					break;
				case 7:
					SarAdcGpioSel(ADC_CHANNEL_B24);
					break;
				case 8:
					SarAdcGpioSel(ADC_CHANNEL_B25);
					break;
				case 9:
					SarAdcGpioSel(ADC_CHANNEL_C7);
					break;
				case 10:
					SarAdcGpioSel(ADC_CHANNEL_C8);
					break;
				case 11:
					SarAdcGpioSel(ADC_CHANNEL_C9);
					break;
				default:
					break;
			}
			SlaveSendResp(SLAVE_ENABLE_ADC_CHANNEL, SlaveRespBuf, 0);
			break;
		case SLAVE_DISABLE_ADC_CHANNEL:
			break;
		case SLAVE_GET_ADC_CHANNEL_VALUE:
			i = gSlaveCmdData[0];
			if(((i < 11) && (i >= 2)) || (i == 0))
			{
				SuspendTick = 0;
				rtn = SarAdcChannelGetValue(i);	 //参考电压为3.3v
				SlaveRespBuf[1] = BYTE0(rtn);
				SlaveRespBuf[0] = BYTE1(rtn);
				SlaveSendResp(SLAVE_GET_ADC_CHANNEL_VALUE, SlaveRespBuf, 2);
			}
			break;
		case SLAVE_GET_VIN_VOLTAGE:
			SuspendTick = 0;
			//SarAdcGpioSel(ADC_CHANNEL_LDOIN);
			rtn = SarAdcGetLdoinVoltage();	 ////return votage at LDO_5VIN pin, unit is mV.
			SlaveRespBuf[1] = BYTE0(rtn);
			SlaveRespBuf[0] = BYTE1(rtn);
			SlaveSendResp(SLAVE_GET_VIN_VOLTAGE, SlaveRespBuf, 2);
			break;
		default:
			break;
	}
}

void SlavePwmProcess(uint16_t Cmd)
{
	uint8_t PwmCh = 0;
	int16_t 	sTempWord;
	int16_t 	sTempWord2;

	switch(Cmd)
	{
		case SLAVE_PWM_CONFIG_CHANNEL:				 //refer to pwm.h
			BYTE1(sTempWord) = gSlaveCmdData[1];
			BYTE0(sTempWord) = gSlaveCmdData[2];
			BYTE1(sTempWord2) = gSlaveCmdData[3];
			BYTE0(sTempWord2) = gSlaveCmdData[4];
			if((gSlaveCmdData[0] <= 0x0F)
			        && (sTempWord >= sTempWord2))
			{
				SuspendTick = 0;
				if(gSlaveCmdData[0] > 0x07)
				{
					gSlaveCmdData[0] -= 0x08;
				}
				PwmConfig(gSlaveCmdData[0], (uint32_t)(sTempWord), (uint32_t)(sTempWord2));
				SlaveSendResp(SLAVE_PWM_CONFIG_CHANNEL, SlaveRespBuf, 0);
			}
			break;
		case SLAVE_ENABLE_PWM_CHANNEL:	//注意：C0和D0不能同时用做PWM输出，C1和D1不能同时用做PWM输出
			if(gSlaveCmdData[0] <= 0x0f)
			{
				SuspendTick = 0;
				if(gSlaveCmdData[0] > 0x07)
				{
					gSlaveCmdData[0] -= 0x08;
					PwmCh = PWM_IO_SEL1;
				}
				else
				{
					PwmCh = PWM_IO_SEL0;
				}
				if(gSlaveCmdData[1] > 3)
				{
					gSlaveCmdData[1] = 0;
				}
				PwmEnableChannel(gSlaveCmdData[0], PwmCh, gSlaveCmdData[1]);
				SlaveSendResp(SLAVE_ENABLE_PWM_CHANNEL, SlaveRespBuf, 0);
			}
			break;
		case SLAVE_DISABLE_PWM_CHANNEL:
			if(gSlaveCmdData[0] <= 0x0F)
			{
				SuspendTick = 0;
				if(gSlaveCmdData[0] > 0x07)
				{
					gSlaveCmdData[0] -= 0x08;
				}
				PwmDisableChannel(gSlaveCmdData[0]);
				SlaveSendResp(SLAVE_DISABLE_PWM_CHANNEL, SlaveRespBuf, 0);
			}
			break;
		default:
			break;
	}
}

void SlaveDeepSleepProcess(uint16_t Cmd)
{
	int16_t 	sTempWord;
	int16_t 	sTempWord2;
	switch(Cmd)
	{
		case SLAVE_SET_WAKEUP_MODE:
			BYTE1(sTempWord) = gSlaveCmdData[0];
			BYTE0(sTempWord) = gSlaveCmdData[1];
			BYTE1(sTempWord2) = gSlaveCmdData[2];
			BYTE0(sTempWord2) = gSlaveCmdData[3];
			SuspendTick = 0;
			SlaveSendResp(SLAVE_SET_WAKEUP_MODE, SlaveRespBuf, 0);
			//SetWakeUpMode(*(uint16_t*)gSlaveCmdData);
			SysSetWakeUpSrcInDeepSleep((uint32_t)sTempWord, (uint32_t)sTempWord2, gSlaveCmdData[4]);
//          SysGotoDeepSleepCfg();		//不能在这里调用这个函数
			break;
		case SLAVE_GOTO_DEEP_SLEEP:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_GOTO_DEEP_SLEEP, SlaveRespBuf, 0);

			SysGotoDeepSleepGpioCfg();
//			SysGotoDeepSleepCfg();
			SysGotoDeepSleep();
			break;
		default:
			break;
	}
}

//#define FM_AREA_CHINA  0x00 // 00B—87.5~108MHz (US/Europe, China)
//#define FM_AREA_JAPAN  0x01 //01B—76~90MHz (Japan)
//#define FM_AREA_RUSSIA 0x02 // 10B—65.8~73MHz (Russia)
//#define FM_AREA_OTHER  0x03 // 11B—60~76MHz

//#define CHINA_BASE_FREQ  875 // China/US/Europe频段 基准频率
//#define JAPAN_BASE_FREQ  760 // Japan频段 基准频率
//#define RUSSIA_BASE_FREQ 650 // Russia频段 基准频率
//#define OTHER_BASE_FREQ  600 // 其它频段 基准频率

void SlaveRadioRetCurrentStatus(void)
{
#ifdef FUNC_RADIO_EN
	SlaveRespBuf[0] = sRadioControl->State;
//	SlaveRespBuf[1] = gRadioCtrl.SeekMode;
//	SlaveRespBuf[2] = gRadioCtrl.RadioSeekResult;
	SlaveRespBuf[1] = 1;//RadioGetStereoState();////st
	SlaveRespBuf[2] = (uint8_t)(sRadioControl->Freq >> 8);
	SlaveRespBuf[3] = (uint8_t)sRadioControl->Freq;
	SlaveRespBuf[4] = sRadioControl->CurStaIdx;//当前台号// Start from 1,0表示没有台
	SlaveRespBuf[5] = sRadioControl->ChlCount;	//
//	SlaveRespBuf[8] = gRadioData.StationSum;
	SlaveSendResp(SLAVE_GET_CUR_STATUS, SlaveRespBuf, 6);
#endif
}

void SlaveRadioProcess(uint16_t Cmd)
{
#ifdef FUNC_RADIO_EN
	int16_t 	sTempWord;
//       uint8_t i;
	switch(Cmd)
	{
		case SLAVE_SET_RADIO_PARAM:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_SET_RADIO_PARAM, SlaveRespBuf, 0);
			sRadioControl->CurFreqArea = gSlaveCmdData[0];
			//gRadioCtrl.RadioSeekStep = gSlaveCmdData[1];
			// 清空电台列表
			memset(sRadioControl->Channel, 0, sizeof(sRadioControl->Channel));
			sRadioControl->ChlCount = 0;

			// 重新按新的频段开始搜台
			MsgSend(MSG_FF_START);
			break;
		case SLAVE_SET_RADIO_FREQ:
			BYTE1(sTempWord) = gSlaveCmdData[0];
			BYTE0(sTempWord) = gSlaveCmdData[1];
			SuspendTick = 0;
			SlaveSendResp(SLAVE_SET_RADIO_FREQ, SlaveRespBuf, 0);
			sRadioControl->Freq = sTempWord;
			if((sRadioControl->Freq < 875) || (sRadioControl->Freq > 1080))
			{
				sRadioControl->Freq = 875;
			}
			RadioSetFreq(sRadioControl->Freq);
			//RadioMute(FALSE);
			break;
		case SLAVE_NEXT_STEP:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_NEXT_STEP, SlaveRespBuf, 0);
//			sRadioControl->Freq++;
//			if((sRadioControl->Freq > 1080) || (sRadioControl->Freq < 875))
//			{
//				sRadioControl->Freq = 1080;
//			}
//			RadioSetFreq();
//			MsgSend(MSG_RADIO_CHL_ADD);
			break;
		case SLAVE_PREV_STEP:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_PREV_STEP, SlaveRespBuf, 0);
			//MsgSend(MSG_RADIO_CHL_SUB);
			break;
		case SLAVE_NEXT_STATION:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_NEXT_STATION, SlaveRespBuf, 0);
//			MsgSend(SYS_KEY_ID_NEXT_SONG);
			MsgSend(MSG_NEXT);
			break;
		case SLAVE_PREV_STATION:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_PREV_STATION, SlaveRespBuf, 0);
//			MsgSend(SYS_KEY_ID_PRE_SONG);
//			MsgSend(MSG_RADIO_UP);
			break;
		case SLAVE_AUTO_SCAN:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_AUTO_SCAN, SlaveRespBuf, 0);
			MsgSend(MSG_PLAY_PAUSE);
			break;
		case SLAVE_NEXT_SEEK:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_NEXT_SEEK, SlaveRespBuf, 0);
//			MessageSend(MSG_FIFO_KEY, MSG_FF_START);
			break;
		case SLAVE_PREV_SEEK:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_PREV_SEEK, SlaveRespBuf, 0);
//			MessageSend(MSG_FIFO_KEY, MSG_FB_START);
			break;
		case SLAVE_SAVE_STATION:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_SAVE_STATION, SlaveRespBuf, 0);
			//MsgSend(MSG_RADIO_SAVE_CHL);
			break;
		case SLAVE_SET_MONO_MODE:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_SET_MONO_MODE, SlaveRespBuf, 0);
//			if(gSlaveCmdData[0] <= 1)
//			{
//				RadioMonoModeSet(gSlaveCmdData[0]); // 1 -- Force receiver in mono mode;	0 -- Not forced,stereo and mono auto selected
//			}
			break;
		case SLAVE_GET_CUR_STATUS:
			SuspendTick = 0;
			SlaveRadioRetCurrentStatus();
			break;
		case SLAVE_GET_STORED_STATION:
			SuspendTick = 0;
			DBG("CHL list %x \n", sRadioControl->ChlCount);
			memcpy(SlaveRespBuf, (uint8_t*)sRadioControl->Channel, (sRadioControl->ChlCount * 2));
			SlaveSendResp(SLAVE_GET_STORED_STATION, SlaveRespBuf, (sRadioControl->ChlCount * 2));
			break;
	}
#endif
}

#define VB_CUT_FREQ  120
void SlaveSpecialFunction(uint16_t Cmd)
{
	static uint8_t VbFlag = 0;

	if(0 == VbFlag)
	{
		Cmd = SLAVE_ENABLE_VIRTUAL_BASS;
	}
	else
	{
		Cmd = SLAVE_DISABLE_VIRTUAL_BASS;
	}

	VbFlag = !VbFlag;

	switch(Cmd)
	{
		case SLAVE_ENABLE_VIRTUAL_BASS:

			SuspendTick = 0;
			SlaveSendResp(SLAVE_ENABLE_VIRTUAL_BASS, SlaveRespBuf, 0);
			SlaveRespBuf[0] = 0xa5;
			SlaveRespBuf[1] = 0x5a;
			SlaveRespBuf[2] = 0x05;
			SlaveRespBuf[3] = 0xe3;
			SlaveRespBuf[4] = 0x01;
			SlaveRespBuf[5] = 0xe9;
			SlaveRespBuf[6] = 0xaa;
//            SppSendData(SlaveRespBuf, 7);
			DBG("SLAVE_ENABLE_VIRTUAL_BASS\n");
			break;
		case SLAVE_DISABLE_VIRTUAL_BASS:
			SuspendTick = 0;
			SlaveSendResp(SLAVE_DISABLE_VIRTUAL_BASS, SlaveRespBuf, 0);
			SlaveRespBuf[0] = 0xa5;
			SlaveRespBuf[1] = 0x5a;
			SlaveRespBuf[2] = 0x05;
			SlaveRespBuf[3] = 0xe3;
			SlaveRespBuf[4] = 0x00;
			SlaveRespBuf[5] = 0xe8;
			SlaveRespBuf[6] = 0xaa;
//            SppSendData(SlaveRespBuf, 7);
			DBG("SLAVE_DISABLE_VIRTUAL_BASS\n");
		default:
			break;
	}
}

bool SlaveRcvCmd(void)
{
	uint8_t Temp;
	uint8_t CheckSum;
	uint8_t i;
	TIMER Timer;
	static int8_t SlaveRxIndex = -2;
	uint16_t Size = SppGetRecBufDataSize();  //get received buff data size


	TimeOutSet(&Timer, 500);
	while(Size > 0)
	{
		if(IsTimeOut(&Timer))
		{
			//DBG("timeout\n");
			return FALSE;
		}
		if(SppReadData(&Temp, 1) == 0)
		{
			return FALSE;
		}
		Size = SppGetRecBufDataSize();
		//SppSendData(&Temp, 1);//test recv
		if(SlaveRxIndex == -2)
		{
			if(Temp == 0x55)
			{
				SlaveRxIndex = -1;
			}
		}
		else if(SlaveRxIndex == -1)
		{
			if(Temp == 0xaa)
			{
				SlaveRxIndex = 0;
			}
		}
		else
		{
			//SlaveHead = 0;
			gSlaveCmd[SlaveRxIndex++] = Temp;		//Cmd + Data[] + CS
			//如果长度域超过最大长度，则重新接收
			if((gSlaveCmd[0] > MAX_CMD_DATA_LEN))
			{
				SlaveRxIndex = -2;
				continue;
			}
			if(SlaveRxIndex >= gSlaveCmd[0] + 4)	//CS接收完
			{
				CheckSum = 0;
				for(i = 0; i < SlaveRxIndex; i++)
				{
					CheckSum += gSlaveCmd[i];
				}
				SlaveRxIndex = -2;
				//return TRUE;  //如需要CheckSum则注释此行
				if(CheckSum == 0)
				{
					return TRUE;
				}
				else
				{

					return FALSE;
				}
			}
		}
	}
	return FALSE;
}
//
//Deal with the recived cmd
//
//Deal with the recived cmd

void SlaveStateCtrl(void)
{
	uint8_t ErrorAck[1] = {0xFF};
	uint16_t CmdCode;
	if(!SlaveRcvCmd())
	{
		return;
	}

	if(SlaveRespBuf == NULL)
	{
		SlaveRespBuf = APP_MMM_MALLOC(SLAVE_RESP_BUF_SIZE, MMM_ALLOC_NORMAL);
		ASSERT(SlaveRespBuf != NULL);
		if(SlaveRespBuf == NULL)
		{
			return;
		}
	}

	memset(SlaveRespBuf, 0, SLAVE_RESP_BUF_SIZE);
	CmdCode = (gSlaveCmd[1] << 8) + gSlaveCmd[2];
	SuspendTick = 1;
	DBG("cmd code %x \n", CmdCode);
	switch(gSlaveCmd[1])
	{
		case 0x01:
			DBG("mode process \n");
			SlaveSysModeProcess(CmdCode);
			break;
		case 0x02:
			DBG("SlaveFileSystemProcess\n");
			SlaveFileSystemProcess(CmdCode);
			break;
		case 0x03:
			DBG("SlavePlayCtrlProcess\n");
			SlavePlayCtrlProcess(CmdCode);
			break;
		case 0x04:
			DBG("SlaveAudioProcess\n");
			SlaveAudioProcess(CmdCode);
			break;
#ifdef FUNC_RTC_EN
		case 0x05:
			DBG("SlaveRtcProcess\n");
			SlaveRtcProcess(CmdCode);
			break;
#endif
		case 0x06:
			DBG("SlaveGpioProcess\n");
			SlaveGpioProcess(CmdCode);
			break;
		case 0x07:
			DBG("SlaveAdcProcess\n");
			SlaveAdcProcess(CmdCode);
			break;
		case 0x08:
			DBG("SlavePwmProcess\n");
			SlavePwmProcess(CmdCode);
			break;
		case 0x09:
			DBG("SlaveDeepSleepProcess\n");
			SlaveDeepSleepProcess(CmdCode);
			break;
		case 0x0A:
			DBG("SlaveRadioProcess\n");
			SlaveRadioProcess(CmdCode);
			break;
		case 0x70:
			DBG("SlaveSpecialFunction\n");
			SlaveSpecialFunction(CmdCode);
			break;
		default:
			break;
	}

	if(SuspendTick & (CmdCode != 0))
	{
		SuspendTick = 0;
		SlaveSendResp(CmdCode, ErrorAck, 1);
	}

//    APP_MMM_FREE(SlaveRespBuf);
//    SlaveRespBuf = NULL;
}


//----------------------------------------------

uint8_t GenerateResData(uint16_t Cmd, uint8_t* OutBuf, uint8_t* InBuf, uint8_t Size)
{
	uint8_t i, CS = 0;

	OutBuf[0] = 0x55;//Sync
	OutBuf[1] = Size;

	OutBuf[2] = ((Cmd >> 8) & 0xFF) | 0x80; //response mask
	OutBuf[3] = Cmd & 0xFF;

	memcpy(OutBuf + 4, InBuf, Size);

	for(i = 1; i < 4 + Size; i++)
	{
		CS += OutBuf[i];
	}

	OutBuf[4 + Size] = 0 - CS;

	return 5 + Size;
}

////////////////////////RESPONSE MODE/////////////////////////////
void ResCmdMode(uint16_t Cmd, uint8_t* Buf, uint8_t PacketDataSize)
{
	uint8_t Size = 1; //default value

	switch(Cmd)
	{
		case CMD_MODE_NEXT_MODE:
			Buf[100] = 1;//NextModeId; //tmp test
			//send the msg here if any

			break;

		case CMD_MODE_SELECT_MODE:
			//Buf[0] is the mode
			Buf[100] = 0;//ok; //tmp test
			//send the msg here if any

			break;

		case CMD_MODE_GET_MODE:
			Buf[100] = 0;//mode test

			break;

		case CMD_MODE_GET_DEVICE_LINK:
			Buf[100] = 0;//link status value, test

			break;

		case MASK_CMD_SYNC://heat sync proc
			Size = 0;
			break;

		default:
			SppSendData("*UNKNOWN#", 9);
			return;
	}

	Size = GenerateResData(Cmd, Buf, Buf + 100/*result using work byte*/, Size); //pay attention to the response data!!!
	SppSendData(Buf, Size);
}


////////////////////////RESPONSE AUDIO/////////////////////////////
void ResCmdAudio(uint16_t Cmd, uint8_t* Buf, uint8_t PacketDataSize)
{
	uint8_t Size = 0;

	switch(Cmd)
	{
		case CMD_AUDIO_VOL_ADD:
			MsgSend(MSG_VOL_UP);
			break;

		case CMD_AUDIO_VOL_SUB:
			MsgSend(MSG_VOL_DW);
			break;

		case CMD_AUDIO_MUTE:
		case CMD_AUDIO_UNMUTE:
			MsgSend(MSG_MUTE);
			break;

		case CMD_AUDIO_SET_VOL:
		case CMD_AUDIO_SET_EQ:
		case CMD_AUDIO_SET_LINEIN_CHANNEL:
		case CMD_AUDIO_SET_LINEIN_GAIN:
		case CMD_AUDIO_ENABLE_MIC:
		case CMD_AUDIO_DISABLE_MIC:
			//Size = 0;
			//goto Response_data;
			break;

		case CMD_AUDIO_GET_USER_EQ_ITEM_NUM://get user eq number
			Buf[100] = 2;//two user defined eq cfg available
			Size = 1;
			//goto Response_data;
			break;

		case CMD_AUDIO_SET_USER_EQ_PARAMETER:
			//Size = 0;
			//goto Response_data;
			break;

		default:
			SppSendData("*UNKNOWN#", 9);
			return;
	}

//Response_data:
	Size = GenerateResData(Cmd, Buf, Buf + 100, Size); //pay attention to the response data!!!
	SppSendData(Buf, Size);

}

/////////////////////////////////////////////////////////////////
/////////////Work buf arrangement////////////////////////////////
///////////Pleas double check with all other usage!!!////////////
// offset    |     usage
//  0  - 1023: response and data I/O buffer
//1024 - 1535: folder cache
//1536 - 2047: long file name cache
//2048 - 2048+700: FAT cache
/////////////////////////////////////////////////////////////////
uint8_t* GetLongFileNameBuf(void)
{
	//need 512 Bytes
	return SppGetWorkBuf() + 1536/*1.5KB*/;
}

FOLDER_CACHE* GetFldCache(void)
{
	//need 512Bytes,max 10 cache lvel supported
	return (FOLDER_CACHE*)(SppGetWorkBuf() + 1024);//pls double check the work buffer's capability
}

FAT_FILE* GetFileCache(void)
{
	//need about 600Bytes
	return (FAT_FILE*)(SppGetWorkBuf() + 2048);//pls double check the work buffer's capability
}

uint8_t GetFoldCacheIdx(uint16_t FolderId)
{
//    uint8_t i;
	return 0;//Justin test
//    FOLDER_CACHE *FldCache = GetFldCache();
//
//    //search in the cache
//    for(i = 0; i < FolderCacheItemNum; i++)
//    {
//        //detect the folder number
//        if(FldCache[i].FolderNum == FolderId)return i;
//    }
//
//    //not found, then clear the cache, and find the absolute folder info.
//    if(FolderOpenByNum(FldCache, 0, FolderId))
//    {
//         //clear all and set only the newest one is OK.
//         FolderCacheItemNum = 1;
//         return 0;
//    }
//
//    return FOLDER_SEARCH_ERROR;//not found, may error found!!!
}

//returns cache floder idx,0x7F means error found
uint8_t GetFolderInfoIdx(uint16_t ParentFldId, uint16_t FOLDIdx)
{
//    uint8_t IdxTmp;
//    FOLDER_CACHE *FldCache = GetFldCache();
	return 0;//Justin test
//    if(ParentFldId)
//    {
//        //find the cache Idx
//        IdxTmp = GetFoldCacheIdx(ParentFldId);

//        if(FOLDER_SEARCH_ERROR != IdxTmp)//found
//        {
//            if(IdxTmp > MAX_FOLDER_CACHE_LEVEL-2)//too much cache items level
//            {
//                return FOLDER_SEARCH_ERROR;
//            }

//            if(FolderOpenByNum(FldCache+IdxTmp + 1, FldCache+IdxTmp, FOLDIdx)) //OK
//            {
//                FolderCacheItemNum = IdxTmp + 2;
//                //IdxTmp+1 is the found folder info
//                return IdxTmp + 1;
//            }

//            //error
//            //end case
//        }
//    }
//    else//absolute Folder Idx
//    {
//        if(FolderOpenByNum(FldCache, 0, FOLDIdx)) //OK
//        {
//            FolderCacheItemNum = 1;
//            return 0;
//        }
//    }

//    //error
//    return FOLDER_SEARCH_ERROR;
}

//OK returns 1 and send the response, error returns 0
uint8_t GetFileInfo(uint8_t* Buf, uint16_t ParentFldId, uint16_t FileIdx)
{
//    uint8_t IdxTmp;
//    FOLDER *ParentFolder = 0;
//    FOLDER_CACHE *FldCache = GetFldCache();
//    FAT_FILE* File = GetFileCache();
//    uint8_t* LongFileName = GetLongFileNameBuf();
	return 0;//Justin test
//    if(ParentFldId)
//    {
//        //find the cache Idx
//        IdxTmp = GetFoldCacheIdx(ParentFldId);

//        if(FOLDER_SEARCH_ERROR == IdxTmp)//error
//        {
//            return FOLDER_SEARCH_ERROR;
//        }
//        ParentFolder =  FldCache + IdxTmp;
//    }

//    if(!FileOpenByNum(File, ParentFolder, FileIdx))//error
//    {
//        return 0;
//    }

//    if(FileGetLongName(File, LongFileName, 255))
//    {
//    }
//    else//error
//    {
//        return 0;
//    }

//    //add response here
//    //
//    //tbd...
//    //get file name
//
//    //send response
//    //tbd...

//    return 1;
}

////////////////////////RESPONSE PLAY/////////////////////////////
void ResCmdFs(uint16_t Cmd, uint8_t* Buf/*data and work buf*/, uint8_t PacketDataSize)
{
//    uint8_t Size = 0; //default value
	uint8_t Tmp;
	uint16_t ParentFoldId, FoldID;

	switch(Cmd)
	{
		case CMD_FS_GET_FOLDER_INFO:
			ParentFoldId = Buf[0];
			ParentFoldId <<= 8;
			ParentFoldId |= Buf[1];
			FoldID = Buf[2];
			FoldID <<= 8;
			FoldID |= Buf[3];
			Tmp = GetFolderInfoIdx(ParentFoldId, FoldID);
			if(FOLDER_SEARCH_ERROR != Tmp)//OK
			{
				//add response
				//tbd....
				SppSendData("*FolderOK#", 10);
			}
			else //error found
			{
				SppSendData("*FolderErr#", 11);
			}

			break;

		case CMD_FS_GET_FILE_INFO:
			if(!GetFileInfo(Buf, 1, 1))
			{
				//error found
				SppSendData("*FileErr#", 9);
			}

			break;


		case CMD_FS_GET_FOLDER_NUM:
			//break;

		case CMD_FS_GET_FILE_NUM:
			//break;

		default:
			SppSendData("*UNKNOWN#", 9);
			return;
	}
}

////////////////////////RESPONSE PLAY/////////////////////////////
void ResCmdPlay(uint16_t Cmd, uint8_t* Buf, uint8_t PacketDataSize)
{
	uint8_t Size = 0; //default value

	switch(Cmd)
	{
		case CMD_PLAY_PLAYPAUSE:
			MsgSend(MSG_PLAY_PAUSE);
			break;

		case CMD_PLAY_STOP:
			MsgSend(MSG_STOP);
			break;

		case CMD_PLAY_NEXT:
			MsgSend(MSG_NEXT);
			break;

		case CMD_PLAY_PRE:
			MsgSend(MSG_PRE);
			break;
		
		case CMD_PLAY_SEEK_PLAY_TIME:
			//MsgSend(MSG_PLAYER_PRE_SONG);
			break;

		case CMD_PLAY_FF:
			MsgSend(MSG_FF_START);
			break;

		case CMD_PLAY_FB:
			MsgSend(MSG_FB_START);
			break;

		case CMD_PLAY_PLAY:
		case CMD_PLAY_PAUSE:
		case CMD_PLAY_STOP_FF_FB:

			//Size = 0;
			break;

		case CMD_PLAY_NEXT_FOLDER:
		case CMD_PLAY_PRE_FOLDER:
			//Size = 0;
			break;

		default:
			SppSendData("*UNKNOWN#", 9);
			return;
	}

// Response_data:
	Size = GenerateResData(Cmd, Buf, Buf + 100, Size); //pay attention to the response data!!!
	SppSendData(Buf, Size);
}

////////////////////////RESPONSE DATA/////////////////////////////
void ResCmdData(uint16_t Cmd, uint8_t* Buf, uint8_t PacketDataSize)
{
	uint8_t Size = 0; //default value

	switch(Cmd)
	{
		case CMD_DATA_FILENAME:
			break;
		case CMD_DATA_FILENAME_CON:
			break;

		case CMD_DATA_FILE_DATA_LEN:
			break;

		case CMD_DATA_FILE_END:
			break;

		case CMD_DATA_FILE_DATA:
			break;

		case CMD_DATA_USER_DATA:
		default:
			SppSendData("*UNKNOWN#", 9);
			return;
	}

// Response_data:
	Size = GenerateResData(Cmd, Buf, Buf + 100, Size); //pay attention to the response data!!!
	SppSendData(Buf, Size);
}

//User APIs
//if fetched data from received buffer, then MUST return non-zero.
uint32_t UserProcSppData(void)
{
	uint16_t Size = SppGetRecBufDataSize();  //get received buff data size

	if(gSelectModeCmdState == SELECT_MODE_CMD_STATE_PENDING && GetModeSwitchState() == MODE_SWITCH_STATE_DONE)
	{
		gSelectModeCmdState = SELECT_MODE_CMD_STATE_DONE;
		SlaveSendResp(SLAVE_SELECT_MODE, &(gSys.CurModuleID), 1);
		return 0;
	}

	if(0 == Size)
	{
		return 0;
	}
	SlaveStateCtrl();
	return 1;

}
#endif

