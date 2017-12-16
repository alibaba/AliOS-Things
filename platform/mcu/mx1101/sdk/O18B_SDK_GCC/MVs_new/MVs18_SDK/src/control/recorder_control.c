/**
 *******************************************************************************
 * @file       record_control.c
 * @author     Alven
 * @maintainer Lilu
 * @version    V2.0.0
 * @brief      record ctrl
 * @maintainer lilu
 * @changeLog
 * 2013-03-10  Created by Alven
 * 2014-06-04  Delete the old playback code by Lilu
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "type.h"
#include "clk.h"
#include "dac.h"
#include "presearch.h"
#include "app_config.h"
#include "fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "recorder_control.h"
#include "sound_remind.h"
#include "mp3enc_api.h"
#include "dir.h"
#include "sd_card.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "sys_app.h"
#include "task_decoder.h"

#include "audio_encoder.h"
#include "adpcm_encoder.h"
#include "pcm_sync.h"
#ifdef FUNC_RECORD_EN

//extern void PlayerTimerCB(void);
//extern bool IsPlayerEnvironmentOk(void);
extern uint32_t GetNextModeIdByProi(uint32_t CurModeId);

extern void ParseFat(FS_CONTEXT* FsContext);
extern uint8_t gBTRecorder;//蓝牙通话录音控制变量

#define REFLESH_DURATION 5000 // ms
#ifdef FUNC_REC_FILE_REFRESH_EN
SW_TIMER FileRefreshTimer = {0};
#endif
#ifdef BT_HF_RECORD_FUNC
//ADPCM
uint8_t* WriteMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
uint8_t* ReadMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
uint32_t EncodedBufDataLen = 0;
bool AdpcmEncoderInit(uint32_t SampleRate, uint32_t BitRate, uint8_t ChannelNum);
#endif

uint8_t FileWriteErrNum = 0;

#define ENCODER_SAMPLES_SIZE       (1152)/**<unit:samples*/
RECORD_CONTROL*  gRecordContrl = NULL;
int16_t MixBuf[ENCODER_SAMPLES_SIZE*RECORD_CHANNEL] = {0};
uint16_t 	TempIndex = 0;//剩余点数
bool 	 	IsRecoderFlag = FALSE;
uint8_t		RecordState;

uint32_t Mp3EncoderInit(uint16_t SampleRate, uint16_t BitRate, uint8_t Ch);

MP3EncoderContext  Mp3EncCon __ATTRIBUTE__(AT(ENC_MEM_ADDR));
uint8_t Mp3OutBuf[1024] __ATTRIBUTE__(AT((VMEM_ADDR + 17 * 1024)));

uint8_t*	MpWriteBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;
uint16_t	Mp3DataLen = 0;
uint8_t*	MpReadBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;

//#define     REG_ASDM_ANA_CTRL_1            (*(volatile unsigned long *) 0x4001A0C4)
//#define     MASK_ASDM_LP_PGA_L             (0x8)
//#define     MASK_ASDM_LP_PGA_R             (0x800)
//void CodecAdcPgaLpSet(bool PgaLpL, bool PgaLpR)
//{
//	if(PgaLpL)
//	{
//		REG_ASDM_ANA_CTRL_1 |= MASK_ASDM_LP_PGA_L;
//	}
//	else
//	{
//		REG_ASDM_ANA_CTRL_1 &= ~MASK_ASDM_LP_PGA_L;
//	}
//	if(PgaLpR)
//	{
//		REG_ASDM_ANA_CTRL_1 |= MASK_ASDM_LP_PGA_R;
//	}
//	else
//	{
//		REG_ASDM_ANA_CTRL_1 &= ~MASK_ASDM_LP_PGA_R;
//	}
//}

DECLARE_SEMAPHORE(RecProcSema, 1);
void LockRecordPlayHandle(void)
{
	OSDOWN(RecProcSema);
}

void UnLockRecordPlayHandle(void)
{
	OSUP(RecProcSema);
}

DECLARE_SEMAPHORE(RecSema, 1);
void LockRecProcess(void)
{
	OSDOWN(RecSema);
}

void UnLockRecProcess(void)
{
	OSUP(RecSema);
}
/**
 * @brief  free allocated mem
 * @param  NONE
 * @return NONE
 */
void RecoderMemFree(void)
{
	LockRecordPlayHandle();
	//free allocated mem
	if(gRecordContrl == NULL)
	{
		UnLockRecordPlayHandle();
		return; 
	}

	if(gRecordContrl->RecordFile.FsContext != NULL)
	{
		FileClose(&gRecordContrl->RecordFile);
	}

	APP_MMM_FREE(gRecordContrl);
	gRecordContrl = NULL;
	UnLockRecordPlayHandle();
}

/**
 * @brief  allocate record mem
 * @param  NONE
 * @return TRUE OR FALSE
 */
bool RecoderMemAllocate(void)
{
	//申请MEM
	gRecordContrl = APP_MMM_MALLOC(sizeof(RECORD_CONTROL), MMM_ALLOC_NORMAL);
	if(NULL == gRecordContrl)
	{
		return FALSE;
	}
	memset(gRecordContrl, 0, sizeof(RECORD_CONTROL));
	gRecordContrl->RecordDeviceID = USB_DISK;
	return TRUE;
}

/**
 * @brief  get record file name
 * @param  SongType 0:WAV 1:MP3
 * @return NONE
 */
void RecordNewFileNameGet(uint8_t SongType)
{
	gRecordContrl->RecordFileName[0] = 'R';
	gRecordContrl->RecordFileName[1] = 'E';
	gRecordContrl->RecordFileName[2] = 'C';
	gRecordContrl->RecordFileName[3] = gRecordContrl->RecordFileNum / 100 + '0';
	gRecordContrl->RecordFileName[4] = (gRecordContrl->RecordFileNum / 10) % 10 + '0';
	gRecordContrl->RecordFileName[5] = gRecordContrl->RecordFileNum % 10 + '0';
	gRecordContrl->RecordFileName[6] = ' ';
	gRecordContrl->RecordFileName[7] = ' ';

	switch(SongType)
	{
		case WAV_FORMAT:
			gRecordContrl->RecordFileName[8] = 'W';
			gRecordContrl->RecordFileName[9] = 'A';
			gRecordContrl->RecordFileName[10] = 'V';
			break;

		case MP3_FORMAT:
			gRecordContrl->RecordFileName[8] = 'M';
			gRecordContrl->RecordFileName[9] = 'P';
			gRecordContrl->RecordFileName[10] = '3';
			break;

		default:
			break;
	}
}

/**
 * @brief  search record file in record folder
 * @param  NONE
 * @return NONE
 */
void RecordFileSearch(FOLDER* Folder)
{
	uint8_t	EntryType;
	uint8_t Temp1;
	uint8_t Temp2;
	uint8_t Temp3;
	uint16_t FileNum;
	uint8_t i;

	gRecordContrl->RecordFileSum = 0;

	DirSetStartEntry(Folder->FsContext, Folder->StartSecNum, 0, TRUE);

	while(1)
	{
		EntryType = DirGetNextEntry(Folder->FsContext);
		if(EntryType == ENTRY_FILE)
		{
			//DBG("找到歌曲：%-.8s%-3s\n", &gCurrentEntry->FileName[0], &gCurrentEntry->FileName[8]);
			if( (Folder->FsContext->gCurrentEntry->FileName[0] != 'R') || 
				(Folder->FsContext->gCurrentEntry->FileName[1] != 'E') || 
				(Folder->FsContext->gCurrentEntry->FileName[2] != 'C') ||
				(Folder->FsContext->gCurrentEntry->FileName[6] != ' ') ||
				(Folder->FsContext->gCurrentEntry->FileName[7] != ' ') ||
				((Folder->FsContext->gCurrentEntry->ExtName[0] != 'M') && (Folder->FsContext->gCurrentEntry->ExtName[0] != 'W')) ||
				((Folder->FsContext->gCurrentEntry->ExtName[1] != 'P') && (Folder->FsContext->gCurrentEntry->ExtName[1] != 'A')) ||
				((Folder->FsContext->gCurrentEntry->ExtName[2] != '3') && (Folder->FsContext->gCurrentEntry->ExtName[2] != 'V')))
			{
				continue;
			}

			//将录音文件名转变为数字形式
			Temp1 = Folder->FsContext->gCurrentEntry->FileName[3] - '0';
			Temp2 = Folder->FsContext->gCurrentEntry->FileName[4] - '0';
			Temp3 = Folder->FsContext->gCurrentEntry->FileName[5] - '0';
			if((Temp1 > 9) || (Temp2 > 9) || (Temp3 > 9))
			{
				continue;	//数字非法
			}
			FileNum = Temp1 * 100 + Temp2 * 10 + Temp3;

			//将该录音文件名以数字形式保存到RecFileMap[]数组
			if((FileNum > 0) && (FileNum <= MAX_REC_FILE_NUM))
			{
				//更新内存中的缓存信息
				//先搜到的文件较早创建，后搜到的文件较晚创建，较晚创建的保存在表的开头
				for(i = gRecordContrl->RecordFileSum; i > 0; i--)
				{
					gRecordContrl->RecordFileMap[i] = gRecordContrl->RecordFileMap[i - 1];
				}
				gRecordContrl->RecordFileMap[0] = FileNum;
				gRecordContrl->RecordFileSum++;
			}
		}
		else if(EntryType == ENTRY_END)
		{
			return;		//文件搜索结束
		}
	}
}

/**
 * @brief  create record file
 * @param  SongType 0:WAV 1:MP3
 * @return err code
 */
#if 0
int32_t RecordCreateNewFile(uint8_t SongType)
{
	uint16_t i;
	// Open root directory
	if(!FolderOpenByNum(&gRecordContrl->ParentFolder, NULL, 1))
	{
		DBG("open root error!\n");
		return OPEN_FOLDER_ERR;
	}
	//open record folder
	if(!FolderOpenByName(&gRecordContrl->RecordFolder, &gRecordContrl->ParentFolder, "record"))
	{
		if(!FolderCreate(&gRecordContrl->RecordFolder, &gRecordContrl->ParentFolder, "record"))
		{
			DBG("FolderCreate() error!\n");
			return CREATE_FOLDER_ERR;
		}
		DBG("FolderCreate() OK!\n");
	}
	else
	{
		//search
		RecordFileSearch(&gRecordContrl->RecordFolder);
	}

	//get file new name
	for(gRecordContrl->RecordFileNum = 1; gRecordContrl->RecordFileNum <= MAX_REC_FILE_NUM; gRecordContrl->RecordFileNum++)
	{
		for(i = 0; i < gRecordContrl->RecordFileSum; i++)
		{
			if(gRecordContrl->RecordFileMap[i] == gRecordContrl->RecordFileNum)
			{
				break;
			}
		}

		if(i == gRecordContrl->RecordFileSum)
		{
			RecordNewFileNameGet(SongType);
			break;
		}
	}

	if(!FileOpenByName(&gRecordContrl->RecordFile, &gRecordContrl->RecordFolder, &gRecordContrl->RecordFileName[0]))
	{
		//create file
		if(!FileCreate(&gRecordContrl->RecordFile, &gRecordContrl->RecordFolder, &gRecordContrl->RecordFileName[0]))
		{
			DBG("FileCreate() error!\n");
			return CREATE_FILE_ERR;
		}
	}
	switch(SongType)
	{
		case WAV_FORMAT:
			if(gBTRecorder)
			{
				if(!AdpcmEncoderInit(8000, 0, 1))
				{
					DBG("WAV ENCODE INIT ERR!\n");
					return WAV_ENCODER_INIT_ERR;
				}
			}
			else
			{
				if(!AdpcmEncoderInit(44100, RECORD_BITRATE, RECORD_CHANNEL))
				{
					DBG("WAV ENCODE INIT ERR!\n");
					return WAV_ENCODER_INIT_ERR;
				}
			}
			break;
		case MP3_FORMAT:
			if(!Mp3EncoderInit(44100, RECORD_BITRATE, RECORD_CHANNEL))
			{
				DBG("MP3 ENCODE INIT ERR!\n");
				return MP3_ENCODER_INIT_ERR;
			}
			gRecordContrl->EncoderSampleSize = 1152;
			break;

		default:
			break;
	}

	//update record file map
	for(i = gRecordContrl->RecordFileSum; i > 0; i--)
	{
		gRecordContrl->RecordFileMap[i] = gRecordContrl->RecordFileMap[i - 1];
	}
	gRecordContrl->RecordFileMap[0] = gRecordContrl->RecordFileNum;
	gRecordContrl->RecordFileSum++;

	return RECORD_NONE_ERR;
}
#endif
/**
 * @brief  get file handle
 * @param  NONE
 * @return NONE
 */
void* GetRecordFileHandle(void)
{
	if(gRecordContrl == NULL || gRecordContrl->RecordFile.FsContext == NULL)
	{
		return NULL; 
	}
	return &gRecordContrl->RecordFile;
}

/**
 * @brief  init record device
 * @param  DeviceId: 0:USB 1:SD
 * @return TRUE OR FALSE
 */
bool RecordDevInit(uint8_t DeviceId, uint8_t EncoderType)
{
	FSDeInit(DEV_ID_SD);
	FSDeInit(DEV_ID_USB);
    
	if(DeviceId == DEV_ID_SD)
	{
		LockSdClk();
		SdControllerInit();
		UnLockSdClk();
	}

	if((DeviceId == DEV_ID_USB && !HostStorInit())
	        || (DeviceId == DEV_ID_SD && SdCardInit() != NONE_ERR))
	{
		APP_DBG("device init error!\n");
		return FALSE;
	}

	if(!FSInit(DeviceId))
	{
		APP_DBG("FSInit() error!\n");
		return FALSE;
	}	
	
	//if(RecordCreateNewFile(EncoderType) < 0)
	{
		DBG("create record file err\n");
		return FALSE;
	}
	DBG("create record file OK\n");
	if(!FolderOpenByName(&gRecordContrl->RecordFolder, &gRecordContrl->ParentFolder, "record"))//add by lilu  创建文件后刷新文件夹Handle
	{
		DBG("Reflash Folder Error!\n");
	}
	return TRUE;
}

/**
 * @brief  init recorder
 * @param  NONE
 * @return TRUE OR FALSE
 */
bool RecorderInit(uint8_t EncoderType)
{
	APP_DBG("RecorderInit.\n");
	//申请MEM
	if(!RecoderMemAllocate())
	{
		APP_DBG("Record mode allocated mem fail\n");
		return FALSE;
	}
	gRecordContrl->EncoderType = EncoderType;
#ifdef FUNC_USB_EN
	if(IsUDiskLink())
	{
		if(!RecordDevInit(USB_DISK, EncoderType))
		{
			APP_DBG("init usb fail\n");
#ifdef FUNC_CARD_EN
			if(IsCardLink())
			{
				if(!RecordDevInit(SD_CARD, EncoderType))
				{
					APP_DBG("init sd card fail\n");
					RecoderMemFree();
					return FALSE;
				}
				gRecordContrl->RecordDeviceID = SD_CARD;
				return TRUE;
			}
#endif
			RecoderMemFree();
			return FALSE;
		}
		gRecordContrl->RecordDeviceID = USB_DISK;
		return TRUE;
	}
#endif
#ifdef FUNC_CARD_EN
	if(IsCardLink())
	{
		if(!RecordDevInit(SD_CARD, EncoderType))
		{
			APP_DBG("init sd card fail\n");
			RecoderMemFree();
			return FALSE;
		}
		gRecordContrl->RecordDeviceID = SD_CARD;
		return TRUE;
	}	
#endif
	RecoderMemFree();
	return FALSE;
}

/**
 * @brief  save file in recording
 * @param  NONE
 * @return err code
 */
int32_t RecoderFileReflesh(void)
{
	uint16_t FileId = gRecordContrl->RecordFile.FileNumInFolder;
    
	FileClose(&gRecordContrl->RecordFile);

	if(!FileOpenByNum(&gRecordContrl->RecordFile, &gRecordContrl->RecordFolder, FileId))
	{
		DBG("File open error!\n");
		return OPEN_FILE_ERR;
	}
    
	if(FileSeek(&gRecordContrl->RecordFile, 0, SEEK_FILE_END))
	{
		DBG("File seek error!\n");
		return FILE_SEEK_ERR;
	}

	return RECORD_NONE_ERR;
}
bool CurrRecordDeviceIsRemoved(void);
/**
 * @brief  record process
 * @param  RecordBuf the data will write into record file
 * @param  DataLength
 * @return err code
 */
int32_t RecoderProc(uint8_t* RecordBuf, uint16_t DataLength)
{
	LockRecordPlayHandle();
	if(gRecordContrl == NULL || gRecordContrl->RecordFile.FsContext == NULL)
	{
		UnLockRecordPlayHandle();
		return WRITE_FILE_ERR; 
	}

	if((FileWrite(RecordBuf, 1, DataLength, &gRecordContrl->RecordFile) == 0) && DataLength != 0)
	{
		//写文件出错  要得到具体错误可以使用如下api判断是否 磁盘已满、设备拔除
		WaitMs(100);
		if(CurrRecordDeviceIsRemoved())
		{
			DBG("设备拔除!\n");
			MsgSend(MSG_MODE);
			SetModeSwitchState(MODE_SWITCH_STATE_START);
		}
		else
		{
			ParseFat(gRecordContrl->RecordFile.FsContext);
			if(gRecordContrl->RecordFile.FsContext->gFsInfo.FreeClusNum < 10)
			{
				DBG("剩余空间不足！\n");
				MsgSend(MSG_MODE);
				SetModeSwitchState(MODE_SWITCH_STATE_START);
			}
			else
			{
				DBG("写盘出现未知错误！\n");
				FileWriteErrNum++;
				if(FileWriteErrNum == 10)
				{
					MsgSend(MSG_MODE);
					SetModeSwitchState(MODE_SWITCH_STATE_START);
				}
			}
		}
		//FileClose(&gRecordContrl->RecordFile);
		UnLockRecordPlayHandle();
		return WRITE_FILE_ERR;
	}
	else
	{
		FileWriteErrNum = 0;
	}
	//FileSave(&gRecordContrl->RecordFile);
	gRecordContrl->RecordFileSize += DataLength;

	UnLockRecordPlayHandle();
	return RECORD_NONE_ERR;
}

void RecordPause(uint8_t SongType)
{
	switch(SongType)
	{
#ifdef BT_HF_RECORD_FUNC
		case WAV_FORMAT:
			{
				uint8_t	*		EncodedBuf;
				uint32_t		EncodedBufLen;
				
				//if(audio_encoder_update_stream_header(&EncodedBuf, &EncodedBufLen))
				{
					if(EncodedBuf != NULL && EncodedBufLen > 0)
					{
						APP_DBG("RecordPause : Write Header\n");
						FileSeek(&gRecordContrl->RecordFile, 0, 0);
						FileWrite(EncodedBuf, 1, EncodedBufLen, &gRecordContrl->RecordFile);
					}
				}
			}			
			break;
#endif
		case MP3_FORMAT:
			break;

		default:
			break;
	}
}
void RecordResume(uint8_t SongType)
{
	switch(SongType)
	{
#ifdef BT_HF_RECORD_FUNC
		case WAV_FORMAT:
			{
				APP_DBG("RecordResume : seek end of file\n");
				FileSeek(&gRecordContrl->RecordFile, 0, 2);
			}			
			break;
#endif
		case MP3_FORMAT:
			break;

		default:
			break;
	}
}

/**
 * @brief  EXIT record
 * @param  NONE
 * @return NONE
 */
void RecoderExit(uint8_t SongType)
{
	APP_DBG("RecoderExit state = %d, type = %d\n", RecordState, SongType);
	if(RecordState == RECORD_STATE_REC)
	{
		switch(SongType)
		{
			case WAV_FORMAT:
				{
#ifdef BT_HF_RECORD_FUNC
					//if(gBTRecorder)
					{
						uint8_t	*		EncodedBuf;
						uint32_t		EncodedBufLen;
						//gBTRecorder = 0;
						
						//if(audio_encoder_update_stream_header(&EncodedBuf, &EncodedBufLen))
						{
							if(EncodedBuf != NULL && EncodedBufLen > 0)
							{
								APP_DBG("Write Header\n");
								FileSeek(&gRecordContrl->RecordFile, 0, 0);
								FileWrite(EncodedBuf, 1, EncodedBufLen, &gRecordContrl->RecordFile);
							}
						}
					}
					//else
					{
						uint8_t		*ADPCMHeader;
						uint32_t	ADPCMHeaderLen;
						//if(adpcm_encoder_update_stream_header((ADPCMEncoderContext*)(ENC_MEM_ADDR), &ADPCMHeader, &ADPCMHeaderLen))
						{
							FileSeek(&gRecordContrl->RecordFile, 0, 0);
							if(!FileWrite(ADPCMHeader, 1, ADPCMHeaderLen, &gRecordContrl->RecordFile))
							{
								DBG("FileWrite WAV Header Error!\n");
							}
						}
						//else
						{
							APP_DBG("Audio encoder error!\n");
						}
					}
#endif
				}
				break;
			case MP3_FORMAT:
				break;

			default:
				break;
		}
	}
	FileClose(&gRecordContrl->RecordFile);
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);	
	RecoderMemFree();   //释放申请的MEM
	IsRecoderFlag = FALSE;
}

/**
 * @brief  judge current device is removed
 * @param  NONE
 * @return TRUE-remove
 */
bool CurrRecordDeviceIsRemoved(void)
{
#ifdef FUNC_USB_EN
	if(gRecordContrl->RecordDeviceID == USB_DISK)
	{
		return !IsUDiskLink();
	}
#endif
#ifdef FUNC_CARD_EN
	if(gRecordContrl->RecordDeviceID == SD_CARD)
	{
		return !IsCardLink();
	}
#endif
	return FALSE;
}

#ifdef BT_HF_RECORD_FUNC
void InitBtHfRecordBuffer(void)
{
	WriteMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
	ReadMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
	EncodedBufDataLen = 0;
}

//adpcm 编码相关
uint32_t PcmRxGetSamplesAvailable(void)
{
	//return GetAvailablePcmSyncDataSize();
}

uint32_t PcmRxReceiveSamples(int16_t* RecvBuffer, uint32_t RecvSamples)
{
	//return ReadDataFromPcmBuffer((uint16_t*)RecvBuffer, RecvSamples);
}

bool EncodedDataToBuffer(uint8_t* EncodedData, uint32_t EncodedDataLen)
{
	uint8_t*	tempWriteMemPtr;
	uint32_t	tempLen;

	if(WriteMemBufPtr >= ReadMemBufPtr)
	{
		if((WriteMemBufPtr + EncodedDataLen) < (uint8_t*)(ADPCM_OUT_SAMPLES_BUF + ADPCM_OUT_SAMPLES_BUF_SIZE))
		{
			memcpy(WriteMemBufPtr, EncodedData, EncodedDataLen);
			WriteMemBufPtr += EncodedDataLen;
			LockRecProcess();
			EncodedBufDataLen += EncodedDataLen;
			UnLockRecProcess();
			return TRUE;
		}
		else
		{
			tempWriteMemPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF + (EncodedDataLen - ((uint8_t*)(ADPCM_OUT_SAMPLES_BUF + ADPCM_OUT_SAMPLES_BUF_SIZE) - WriteMemBufPtr));
			if(tempWriteMemPtr < ReadMemBufPtr)
			{
				tempLen = ((uint8_t*)(ADPCM_OUT_SAMPLES_BUF + ADPCM_OUT_SAMPLES_BUF_SIZE) - WriteMemBufPtr);
				memcpy(WriteMemBufPtr, EncodedData, tempLen);
				WriteMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
				
				memcpy(WriteMemBufPtr, EncodedData + tempLen, EncodedDataLen - tempLen);
				WriteMemBufPtr += EncodedDataLen - tempLen;
				LockRecProcess();
				EncodedBufDataLen += EncodedDataLen;
				UnLockRecProcess();
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	else
	{
		if((WriteMemBufPtr + EncodedDataLen) < ReadMemBufPtr)
		{
			memcpy(WriteMemBufPtr, EncodedData, EncodedDataLen);
			WriteMemBufPtr += EncodedDataLen;
			LockRecProcess();
			EncodedBufDataLen += EncodedDataLen;
			UnLockRecProcess();
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

void EncodedDataBufferToFile(uint32_t Len)
{
	uint32_t		WriteLen = 0;
	uint32_t		LengthToEnd = 0;
	uint32_t		RemainLen = 0;

	LengthToEnd = ADPCM_OUT_SAMPLES_BUF_SIZE - (uint32_t)(ReadMemBufPtr - ADPCM_OUT_SAMPLES_BUF);
	
	if(EncodedBufDataLen >= Len)
	{
		WriteLen = LengthToEnd < Len ? LengthToEnd : Len;
		RemainLen = Len - WriteLen;

		RecoderProc(ReadMemBufPtr, WriteLen);

		ReadMemBufPtr += WriteLen;
		if(ReadMemBufPtr >= (uint8_t*)(ADPCM_OUT_SAMPLES_BUF + ADPCM_OUT_SAMPLES_BUF_SIZE))
		{
			ReadMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
		}

		if(RemainLen != 0)
		{
			RecoderProc(ReadMemBufPtr, RemainLen);

			ReadMemBufPtr += RemainLen;
			if(ReadMemBufPtr >= (uint8_t*)(ADPCM_OUT_SAMPLES_BUF + ADPCM_OUT_SAMPLES_BUF_SIZE))
			{
				ReadMemBufPtr = (uint8_t*)ADPCM_OUT_SAMPLES_BUF;
			}
		}

		LockRecProcess();
		EncodedBufDataLen -= Len;
		UnLockRecProcess();

	}
}
#endif

//MP3编码相关Begin
static uint32_t sRecSamples    = 0;
static uint32_t sRecordingTime = 0; // unit ms
static uint32_t sRecordingTimePre = 0;
void ShowRecordingTime(void)
{
	if((sRecordingTime / 1000 - sRecordingTimePre / 1000 >= 1))
	{
		APP_DBG("%s Recording(%ds)\n",gRecordContrl->RecordFileName,
	        sRecordingTime / 1000);
		sRecordingTimePre = sRecordingTime;
	}
}

bool AdpcmEncoderInit(uint32_t SampleRate, uint32_t BitRate, uint8_t ChannelNum)
{
	uint8_t	*		EncodedBuf;
	uint32_t		EncodedBufLen;

	LockRecProcess();
	MpWriteBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;
	Mp3DataLen = 0;
	MpReadBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;
	UnLockRecProcess();

	sRecordingTime = 0;
	sRecSamples    = 0;
#ifdef BT_HF_RECORD_FUNC
	//if(gBTRecorder)
	{
		//if(audio_encoder_initialize((uint8_t*)ADPCM_ENC_MEM_ADDR,
		//                            PcmRxGetSamplesAvailable,
		//                            PcmRxReceiveSamples,
		//                            ADPCM_ENCODER,
		//                            ChannelNum,
		//                            SampleRate,
		//                            BitRate, 0)!= (int32_t)RT_SUCCESS)
		{
			APP_DBG("Audio encoder error!\n");
			return FALSE;
		}
		
		if(audio_encoder_update_stream_header(&EncodedBuf, &EncodedBufLen))
		{
			if(EncodedBuf != NULL && EncodedBufLen > 0)
			{
				FileWrite(EncodedBuf, 1, EncodedBufLen, &gRecordContrl->RecordFile);
			}
		}
	}
	//else
#endif
	{
		uint8_t		*ADPCMHeader;
		uint32_t	ADPCMHeaderLen;
		if(!adpcm_encoder_initialize((ADPCMEncoderContext*)(ENC_MEM_ADDR), 
										ChannelNum, SampleRate, 
										(uint8_t*)(VMEM_ADDR + 17 * 1024)))
		{
			APP_DBG("Audio encoder error!\n");
			return FALSE;
		}
		gRecordContrl->EncoderSampleSize = ((ADPCMEncoderContext*)(ENC_MEM_ADDR))->samples_per_block;
		//if(adpcm_encoder_update_stream_header((ADPCMEncoderContext*)(ENC_MEM_ADDR), &ADPCMHeader, &ADPCMHeaderLen))
		{
			if(!FileWrite(ADPCMHeader, 1, ADPCMHeaderLen, (&gRecordContrl->RecordFile)))
			{
				DBG("FileWrite WAV Header Error!\n");
			}
		}
		//else
		{
			APP_DBG("Audio encoder error!\n");
			return FALSE;
		}
	}
	return TRUE;
}


/**
 * @brief  初始化MP3 encoder
 * @param  SampleRate  32000、44100、48000
 * @param  BitRate 32、48、56、64、80、96、112、128、160、192、224、256、320、384
 * @param  Ch 1:mono 2 stereo
 * @return 1:TRUE 0:FALSE
 */
uint32_t Mp3EncoderInit(uint16_t SampleRate, uint16_t BitRate, uint8_t Ch)
{
	LockRecProcess();
	MpWriteBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;
	Mp3DataLen = 0;
	MpReadBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;
	UnLockRecProcess();

	sRecordingTime = 0;
	sRecSamples    = 0;

	//if(audio_encoder_initialize((uint8_t*)ENC_MEM_ADDR, GetPcmSampleCount, CopyPcmSampleData, MP2_ENCODER, Ch, SampleRate, BitRate, 0))
	//if(mp3_encoder_initialize(&Mp3EncCon, Ch, SampleRate, BitRate, Mp3OutBuf))
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief  每次压缩的长度固定为1152个采样点
 * @param  SampleRate  32000、44100、48000
 * @return 返回当前帧长度
 */
uint16_t Mp3EncoderFrame(uint8_t* OutBuf, int16_t* InBuf, uint32_t Samples)
{
	uint8_t*   TempBuf;
	uint32_t   FrameLen;
	uint32_t   RemainLen;
	uint8_t		EncodeFlag;

	if(gRecordContrl->EncoderType == WAV_FORMAT)
	{
		sRecSamples += gRecordContrl->EncoderSampleSize;
		if(sRecSamples >= (44100 / 4))
		{
			sRecordingTime += (uint32_t)((sRecSamples * 1000) / 44100);
			sRecSamples = 0;
		}
		//EncodeFlag = adpcm_encoder_encode((ADPCMEncoderContext*)(ENC_MEM_ADDR), InBuf, &TempBuf, &FrameLen);
	}
	else
	{
		sRecSamples += 1152;
		if(sRecSamples >= (44100 / 10))
		{
			sRecordingTime += (uint32_t)((sRecSamples * 1000) / 44100);
			sRecSamples = 0;
		}
		//EncodeFlag = mp3_encoder_encode(&Mp3EncCon, InBuf, &TempBuf, &FrameLen);
	}
	if(EncodeFlag)
	{
		if(FrameLen == 0)
		{
			return 0;
		}
		RemainLen = (uint8_t*)(MP3_OUT_SAMPLES_BUF + MP3_OUT_SAMPLES_BUF_SIZE) - OutBuf;
		if(FrameLen <= RemainLen)
		{
			memcpy(OutBuf, TempBuf, FrameLen);
			OutBuf += FrameLen;
		}
		else
		{
			memcpy(OutBuf, TempBuf, RemainLen);
			OutBuf = (uint8_t*)MP3_OUT_SAMPLES_BUF;
			memcpy(OutBuf, TempBuf + RemainLen, FrameLen - RemainLen);
			OutBuf += FrameLen - RemainLen;
		}
		MpWriteBufPtr = OutBuf;
		return FrameLen;
	}
	else
	{
		DBG("encoder fail!\n");
		return 0;
	}
}

/**
 * @brief  manage mp3 output buf
 * @param  InBuf input buffer
 * @param  Samples samples size
 * @return NONE
 */
void Mp3OutBufManagement(int16_t* InBuf, uint32_t Samples)
{
	uint16_t        Mp3FrameLen;
	while(1)
	{
		if(Mp3DataLen <= MP3_OUT_SAMPLES_BUF_SIZE)
		{
			Mp3FrameLen = Mp3EncoderFrame(MpWriteBufPtr, InBuf, Samples);
			//Samples = 0;
			if(Mp3FrameLen)
			{
				LockRecProcess();
				Mp3DataLen += Mp3FrameLen;
				UnLockRecProcess();
				break;
			}
		}
		else
		{
			break;
		}
	}
}

/**
 * @brief  返回编码实际长度
 * @param  NONE
 * @return Mp3DataLen
 */
uint32_t Mp3GetDataLen(void)
{
	return Mp3DataLen;
}

/**
 * @brief  write mp3 output buf data into file
 * @param  Len The recommended length:512 1024 ...
 * @return NONE
 */
void Mp3OutBufWriteFile(uint32_t Len)
{
	if(Mp3DataLen >= Len)
	{
		if(MP3_OUT_SAMPLES_BUF_SIZE - (uint32_t)(MpReadBufPtr - MP3_OUT_SAMPLES_BUF) < Len)
		{
			Len = MP3_OUT_SAMPLES_BUF_SIZE - (uint32_t)(MpReadBufPtr - MP3_OUT_SAMPLES_BUF);
		}

		RecoderProc(MpReadBufPtr, Len);

		LockRecProcess();
		Mp3DataLen -= Len;
		UnLockRecProcess();
		MpReadBufPtr += Len;
		if(MpReadBufPtr >= (uint8_t*)(MP3_OUT_SAMPLES_BUF + MP3_OUT_SAMPLES_BUF_SIZE))
		{
			MpReadBufPtr = (uint8_t*)MP3_OUT_SAMPLES_BUF;
		}
	}
}
//MP3编码相关End

bool IsRecorderEnvironmentOk(void)
{
	return (IsUDiskLink() || IsCardLink());
}

bool RecordStatusCtrl(uint8_t RecordStatus)
{
	switch(RecordStatus)
	{
		case RECORD_STATE_INIT:

			//init audio path
			if(gSys.RecordSource == RECORD_SOURCE_LINEIN)
			{
				AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_LINEIN : AUDIO_CH_LINEIN);
			}
			else if(gSys.RecordSource == RECORD_SOURCE_FM)
			{
				AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_FM : AUDIO_CH_FM);
			}
			//CodecAdcPgaLpSet(FALSE, FALSE);
			//init audio path
			if(!RecorderInit(MP3_FORMAT))
			{
				return FALSE;
			}
			break;

		case RECORD_STATE_BROWSER:
			break;

		case RECORD_STATE_REC:
			if(Mp3GetDataLen() >= 4096)
			{
				Mp3OutBufWriteFile(4096);//写盘
			}
 			break;
					
		case RECORD_STATE_REC_PAUSE:  //不写盘
			break;

		default:
			break;
	}

	return TRUE;
}

#ifdef FUNC_REC_FILE_REFRESH_EN 
void FileRefreshTimerCB(void* Param)
{
	if((Mp3DataLen < ENC_MEM_SIZE/2) && RecordState == RECORD_STATE_REC)
	{
		DBG("Save Fat\\Dir\n");
		FileSave(&gRecordContrl->RecordFile);
		StartTimer(&FileRefreshTimer);
	}
}
#endif
bool RecorderControl(void)
{
	uint16_t Msg = 0;
    
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
	if(!IsRecorderEnvironmentOk())
	{
		APP_DBG("record device is not ready!\n");
		gSys.NextModuleID = gSys.ModuleIDBackUp;
		return FALSE;
	}
#ifdef FUNC_REC_FILE_REFRESH_EN
	InitTimer(&FileRefreshTimer, REFLESH_DURATION, FileRefreshTimerCB);
	StartTimer(&FileRefreshTimer);
#endif
	APP_DBG("enter recoder\n");
	//初始化录音设备
	RecordState = RECORD_STATE_INIT;   
	SoundRemind(SOUND_START_REC);  

	if(RecordStatusCtrl(RecordState))
	{
		if(gSys.RecordState == RECORD_STATE_REC)
		{
			RecordState = RECORD_STATE_REC;
			IsRecoderFlag = TRUE;
			TempIndex = 0;
		}

		SetModeSwitchState(MODE_SWITCH_STATE_DONE);
		
		while(Msg != MSG_COMMON_CLOSE)
		{
			Msg = MsgRecv(2);
#ifdef FUNC_REC_FILE_REFRESH_EN
			CheckTimer(&FileRefreshTimer);
#endif
			RecordStatusCtrl(RecordState);
			ShowRecordingTime();
//			if(CurrRecordDeviceIsRemoved())
//			{
//				APP_DBG("record device is removed!\n");
//				RecoderExit(MP3_FORMAT);
//				gSys.NextModuleID = gSys.ModuleIDBackUp;
//				break;
//			}

			switch(Msg)
			{
				case MSG_PLAY_PAUSE: // 录音暂停
					if(RecordState == RECORD_STATE_REC)
					{
						RecordState = RECORD_STATE_REC_PAUSE;
						IsRecoderFlag = FALSE;
						APP_DBG("%s Record Pause.\n", gRecordContrl->RecordFileName);
					}
					else if(RecordState == RECORD_STATE_REC_PAUSE)
					{
						RecordState = RECORD_STATE_REC;
						IsRecoderFlag = TRUE;
						TempIndex = 0;
						APP_DBG("%s Record Start.\n", gRecordContrl->RecordFileName);
					}
					break;
				case MSG_POWER:
					gSys.NextModuleID = MODULE_ID_STANDBY;
					Msg = MSG_COMMON_CLOSE;
					break;
				
				case MSG_MODE:    //录音退出
				case MSG_REC:
//					RecoderExit(MP3_FORMAT);
					gSys.NextModuleID = gSys.ModuleIDBackUp;
				
//					IsRecoderFlag = FALSE;
					Msg = MSG_COMMON_CLOSE;
					break;
#ifdef FUNC_REC_PLAYBACK_EN
				case MSG_REC_PLAYBACK:	//录音回放
					if(IsUDiskLink() || IsCardLink())
					{
						AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);
						EnterRecBackPlay(gSys.ModuleIDBackUp, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0, 0);
					}
					break;
#endif
				default:
					break;
			}
		}//while(Msg != MSG_COMMON_CLOSE)

		if(Msg == MSG_COMMON_CLOSE)
		{
			RecoderExit(MP3_FORMAT);
		}
	}
	else
	{
		APP_DBG("record init err\n");
		gSys.NextModuleID = gSys.ModuleIDBackUp;
	}

	APP_DBG("leave recorder...\n");
	return TRUE;//OK
}
//接收adc通路数据用于录音
void MixerForRecoder(int16_t* PcmBuf1, int16_t* PcmBuf2, uint16_t SampleCnt, uint32_t PcmDataMode)
{
	uint16_t i;
	uint16_t Cnt = 0;
	uint16_t Temp;//临时变量用于缓存本次第一次MIX buf len
	
	if(IsRecoderFlag)
	{			
		if(PcmDataMode == 0x04)
		{
			while(1)
			{
				Temp = gRecordContrl->EncoderSampleSize - TempIndex/RECORD_CHANNEL;
				if((SampleCnt - Cnt) > Temp)
				{
					if(RECORD_CHANNEL == 1)		// 单声道
					{
						for(i = 0; i < Temp; i++)
						{
							//MixBuf[TempIndex++] = __ssat((PcmBuf1[i + Cnt] + PcmBuf2[i + Cnt]), 16);	//需保证不溢出
                            //MixBuf[TempIndex++] = (int16_t)(((int32_t)PcmBuf1[i + Cnt]  + (int32_t)PcmBuf2[i + Cnt]) >> 1);
						}
					}
					else
					{
						for(i = 0; i < Temp; i++)
						{
							MixBuf[TempIndex++] = __ssat((PcmBuf1[i + Cnt]+ PcmBuf2[i + Cnt]), 16);
                            //MixBuf[TempIndex++] = (int16_t)(((int32_t)PcmBuf1[i + Cnt]  + (int32_t)PcmBuf2[i + Cnt]) >> 1);
							MixBuf[TempIndex++] = MixBuf[TempIndex - 1];
						}
					}
					Mp3OutBufManagement(MixBuf, gRecordContrl->EncoderSampleSize);
					Cnt += Temp;
					TempIndex = 0;
				}
				else
				{
					if(RECORD_CHANNEL == 1)		// 单声道
					{
						for(i = Cnt; i < SampleCnt; i++)
						{
							//MixBuf[TempIndex++] = __ssat((PcmBuf1[i] + PcmBuf2[i]), 16);	//需保证不溢出
                            //MixBuf[TempIndex++] = (int16_t)(((int32_t)PcmBuf1[i + Cnt]  + (int32_t)PcmBuf2[i + Cnt]) >> 1);
						}
					}
					else
					{
						for(i = Cnt; i < SampleCnt; i++)
						{
							MixBuf[TempIndex++] = __ssat((PcmBuf1[i] + PcmBuf2[i]), 16);
                            //MixBuf[TempIndex++] = (int16_t)(((int32_t)PcmBuf1[i + Cnt]  + (int32_t)PcmBuf2[i + Cnt]) >> 1);
							MixBuf[TempIndex++] = MixBuf[TempIndex - 1];
						}
					}
					return;
				}
			}
		}
		else if(PcmDataMode == 0x03)
		{
			while(1)
			{
				Temp = gRecordContrl->EncoderSampleSize - TempIndex/RECORD_CHANNEL;
				if((SampleCnt - Cnt) > Temp)
				{
					if(RECORD_CHANNEL == 1)			// 单声道
					{
						for(i = 0; i < Temp; i++)
						{
							//MixBuf[TempIndex++] = __ssat((PcmBuf1[2 * i + Cnt] + PcmBuf2[2 * i + Cnt]), 16);	//需保证不溢出
                            //MixBuf[TempIndex++] = (int16_t)(((int32_t)PcmBuf1[i + Cnt]  + (int32_t)PcmBuf2[i + Cnt]) >> 1);
						}
					}
					else
					{
						for(i = 0; i < Temp; i++)
						{
							MixBuf[TempIndex++] = PcmBuf1[2 * i + Cnt];
							MixBuf[TempIndex++] = PcmBuf2[2 * i + Cnt];
						}
					}
					Mp3OutBufManagement(MixBuf, gRecordContrl->EncoderSampleSize);
					Cnt += Temp;
					TempIndex = 0;
				}
				else
				{
					if(RECORD_CHANNEL == 1)			// 单声道
					{
						for(i = Cnt; i < SampleCnt; i++)
						{
							//MixBuf[TempIndex++] = __ssat((PcmBuf1[2 * i] + PcmBuf2[2 * i]), 16);	//需保证不溢出
                            //MixBuf[TempIndex++] = (int16_t)(((int32_t)PcmBuf1[i + Cnt]  + (int32_t)PcmBuf2[i + Cnt]) >> 1);
						}
					}
					else
					{
						for(i = Cnt; i < SampleCnt; i++)
						{
							MixBuf[TempIndex++] = PcmBuf1[2 * i];
							MixBuf[TempIndex++] = PcmBuf2[2 * i];
						}
					}
					return;
				}
			}
		}
	}
}

#endif

#ifdef FUNC_REC_PLAYBACK_EN
typedef struct __REC_BACK_PLAY_
{
	uint32_t BackModuleId;
	uint16_t FolderId;
	uint16_t FileId;
	bool     IsWorking;
	DEV_ID   DevId;
}REC_BACK_PLAY;
void LeaveRecPlayBack(void);

static REC_BACK_PLAY sRecBackPlayCtrl __attribute__((section("data"))) = {0};

void InitRecBackPlayHandle(void)
{
	memset(&sRecBackPlayCtrl, 0, sizeof(sRecBackPlayCtrl));
}

bool EnterRecBackPlay(uint32_t ParentModuleId, DEV_ID DevId, bool IsNeedInitFs, uint16_t FolderId, uint16_t FileId)
{
	if(sRecBackPlayCtrl.IsWorking)
	{
		return FALSE;
	}

	sRecBackPlayCtrl.DevId        = DevId;
	sRecBackPlayCtrl.FolderId     = FolderId;
	sRecBackPlayCtrl.FileId       = FileId;
	sRecBackPlayCtrl.BackModuleId = ParentModuleId;//gSys.CurModuleID;
	gSys.NextModuleID            = MODULE_ID_REC_BACK_PLAY;
	MsgSend(MSG_COMMON_CLOSE);
	
	return TRUE;
}

bool RecBackPlayControl(void)
{
	uint16_t Msg     = 0;
    
	APP_DBG("Enter RecBackPlayControl...\n");
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
	if(sRecBackPlayCtrl.BackModuleId == MODULE_ID_UNKNOWN)
	{
		APP_DBG("invalid parent mode!\n");
		LeaveRecPlayBack();
		return FALSE;
	}
	SoundRemind(SOUND_REC_PLAYBACK);     
	if(sRecBackPlayCtrl.DevId == DEV_ID_NONE || 
	    (sRecBackPlayCtrl.DevId == DEV_ID_SD && !IsCardLink()) || 
	    (sRecBackPlayCtrl.DevId == DEV_ID_USB && !IsUDiskLink()))
	{
		APP_DBG("device not ready!\n");
		LeaveRecPlayBack();
		return FALSE;
	}
    
	if(!IsFsInited())
	{
		FSDeInit(DEV_ID_SD);
		FSDeInit(DEV_ID_USB);

		LockSdClk();
		if(sRecBackPlayCtrl.DevId == DEV_ID_SD)
		{
			SdControllerInit();
		}
		UnLockSdClk();

		if((sRecBackPlayCtrl.DevId == DEV_ID_USB && !HostStorInit()) || 
		    (sRecBackPlayCtrl.DevId == DEV_ID_SD && SdCardInit() != NONE_ERR))
		{
			APP_DBG("device init error!\n");
			LeaveRecPlayBack();
			return FALSE;
		}

		if(!FSInit(sRecBackPlayCtrl.DevId))
		{
			APP_DBG("FSInit() error!\n");
			LeaveRecPlayBack();
			return FALSE;
		}        
	}
    
	if(sRecBackPlayCtrl.FolderId == 0)
	{
		FOLDER RootFolder;
		FOLDER FolderHandle;

		if(!FolderOpenByNum(&RootFolder, NULL, 1))
		{
			APP_DBG("open root error!\n");
			LeaveRecPlayBack();
			return FALSE;
		}

		if(!FolderOpenByName(&FolderHandle, &RootFolder, "record"))
		{
			APP_DBG("not found record folder!\n");
			LeaveRecPlayBack();
			return FALSE;
		}
		sRecBackPlayCtrl.FolderId = FolderHandle.ValidFolderNum;
		
		if(sRecBackPlayCtrl.FileId == 0)
		{
			sRecBackPlayCtrl.FileId = FolderHandle.RecFileCnt;
		}
		
	}
    
	if(sRecBackPlayCtrl.FileId == 0)
	{
		FOLDER FolderHandle;
		if(!FolderOpenByNum(&FolderHandle, NULL, sRecBackPlayCtrl.FolderId))
		{
			LeaveRecPlayBack();
			return FALSE;
		}	
		sRecBackPlayCtrl.FileId = FolderHandle.RecFileCnt;
	}
 
	if(!AudioPlayerInitialize(sRecBackPlayCtrl.FileId, sRecBackPlayCtrl.FolderId, FALSE))
	{
		LeaveRecPlayBack();
		return FALSE;
	}

    if(gSys.MuteFlag)
	{
    	gSys.MuteFlag = 0;
		AudioPlayerMute(gSys.MuteFlag);
	}
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		Msg = MsgRecv(10);

		PlayerTimerCB();
		//gPlayContrl->CurPlayTime = DecoderTaskGetPlayTime();//更新播放时间
		
		//if((GetPlayState() == PLAYER_STATE_PLAYING) && (DecoderTaskIsSongEnd() || DecoderTaskIsSongBegin()))
		{
			APP_DBG("End of Song\n");
		
			if(!IsPlayerEnvironmentOk())
			{
				gSys.NextModuleID = GetNextModeIdByProi(gSys.CurModuleID);
				MsgSend(MSG_COMMON_CLOSE);
			}
		
		//	if(gPlayContrl->RecBackPlayFlag || DecoderTaskIsSongBegin())
			{
//				AudioPlayerPreSong();
				MsgSend(MSG_PRE_SONG);
			}
			//else
			{
//				AudioPlayerNextSong();
				MsgSend(MSG_NEXT_SONG);
			}	
		}

		if(!AudioPlayerEvent(Msg))
		{
			AudioPlayerStop();
			break;
		}
		
		switch(Msg)
		{
			case MSG_REC_FILE_DEL: // 删除录音文件
#ifdef FUNC_RECORD_FILE_DELETE_EN
				{
					FOLDER   FolderHandle;
					FAT_FILE FileHandle;
					SoundRemind(SOUND_DELETE_REC);  
					if(gPlayContrl->CurFileIndex > 1)
					{
						sRecBackPlayCtrl.FileId = gPlayContrl->CurFileIndex - 1;
					}
					else
					{
						sRecBackPlayCtrl.FileId = gPlayContrl->FolderHandle.RecFileCnt - 1;
					}
                
					AudioPlayerStop();
					if(FolderOpenByValidNum(&FolderHandle, NULL, sRecBackPlayCtrl.FolderId) && 
					    FileOpenByNum(&FileHandle, &FolderHandle, gPlayContrl->CurFileIndex))
					{
						DBG("%s Has Been Deleted!!!\n",FileHandle.ShortName);
						FileDelete(&FileHandle);
						FileClose(&FileHandle);
						FolderOpenByName(&gPlayContrl->FolderHandle, NULL, "record");
//						AudioPlayerPreSong();
						MsgSend(MSG_PRE);
					}
				}
#endif
				break;

			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;
				
			case MSG_SD_PLUGOUT:
			case MSG_USB_PLUGOUT:
				LeaveRecPlayBack();
				return TRUE;
		}
	}

	LeaveRecPlayBack();
	return TRUE;
}

void LeaveRecPlayBack(void)
{
	AudioPlayerDeinitialize();

	APP_DBG("Leave RecBackPlayControl...\n");
	if(gSys.NextModuleID != MODULE_ID_RECORDER && gSys.NextModuleID != MODULE_ID_STANDBY)
	{
		gSys.NextModuleID = sRecBackPlayCtrl.BackModuleId;
	}
	memset(&sRecBackPlayCtrl, 0, sizeof(sRecBackPlayCtrl));
}
#endif



