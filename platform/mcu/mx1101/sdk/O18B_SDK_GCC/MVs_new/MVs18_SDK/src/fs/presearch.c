/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:presearch.c
//  maintainer: lilu
/////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "dev_detect_driver.h"
#include "fs_config.h"
#include "folder.h"
#include "fat_file.h"
#include "fsinfo.h"
#include "dir.h"
#include "fsmount_buffermanager.h"
#include "fs_mount.h"
#include "fs_sync.h"
#include "fs_filesort.h"
#include "partition.h"
#include "breakpoint.h"
#ifndef INDEPEND_FREERTOS
#include "os.h"
#endif

#ifdef FUNC_SPI_UPDATE_EN
uint8_t UpgradeFileFound = 0xFF;
#endif// FUNC_SPI_UPDATE_EN

static bool fs_inited_flag = 0;
#ifndef INDEPEND_FREERTOS
SEMAPHORE	Sync;	// Identifier of sync object
#endif
//extern TerminateFunc terminate_cur_fs_proc;
#ifdef FS_EXFAT_EN
extern bool		gIsExFat;
void InitFolderNoFatChainFlag(FS_CONTEXT* FsContext);
void SetEntryNoFatChain(uint8_t Flag);
void ClrFolderNoFatChainFlag(FS_CONTEXT* FsContext, uint16_t FolderNum);
#endif

uint8_t FsSyncInit(void* FsContext)
{
#ifndef INDEPEND_FREERTOS
	if(FsContext)
	{
		SEMA_INIT(Sync, 1)
		return 1;
	}
#endif
	return 0;
}

void FsLock(void* FsContext)
{
#ifndef INDEPEND_FREERTOS
	if(FsContext)
	{
		OSDOWN(Sync);
	}
#endif
}

void FsUnLock(void* FsContext)
{
#ifndef INDEPEND_FREERTOS
	if(FsContext)
	{
		OSUP(Sync);
	}
#endif
}
////////////////////////////////////////////////////////////////
//
//寻找有特定扩展名的文件
//
void FindSpecialFileCallBack(FS_CONTEXT* FsContext)
{
#ifdef FUNC_SPI_UPDATE_EN   //注：升级文件相关，勿改
	//检查是否为SPI FLASH升级文件：*.MVA
	//只检查根目录
	if((FsContext->gFsInfo.FolderSum == 1) && (UpgradeFileFound != 1)) //只搜索根目录是否有升级文件，注释这句则全盘搜索
	{
		if(memcmp(FsContext->gCurrentEntry->ExtName, "MVA", 3) == 0)
		{
			FAT_FILE MVA_File;
			uint8_t ShortNameLen;
			uint8_t FileName[12];
			uint8_t *ShortName = FsContext->gCurrentEntry->FileName;
			uint8_t MVAbuf[4] = {0};
			uint8_t MVA[4] = { 0x4d, 0x56, 0x4f, 0x12};   //M V O 0X12
			
			for(ShortNameLen = 0; ShortNameLen < 8; ShortNameLen++)    //查找短文件名
			{
				if(ShortName[ShortNameLen] == 0x20)
				{
					break;
				}
				
			}
			memset(FileName, 0x00, 12);
			memcpy(FileName, ShortName, ShortNameLen);
			memcpy(&FileName[ShortNameLen], ".MVA", sizeof(".MVA"));
			FsUnLock(FsContext);
			FileOpen( &MVA_File, FileName, FA_READ);
			FileRead( MVAbuf, 1, 4, &MVA_File);
			FileClose(&MVA_File);
			
			if( (MVAbuf[0] == MVA[0]) && (MVAbuf[1] == MVA[1]) && (MVAbuf[2] == MVA[2]) && (MVAbuf[3] == MVA[3]))
			{
#ifdef  FUNC_UPDATE_CONTROL				
				DBG("Please enter key to select upgrade or NOT in 5 seconds,system will not upgrade if time overstep:\n");
				TimeOutSet(&MVAtime,5000);
				while(!IsTimeOut(&MVAtime))
				{
					switch(AdcKeyScan())
					{
						case MSG_PLAY_PAUSE:
#endif		
						
							UpgradeFileFound = 1;
						
#ifdef FUNC_UPDATE_CONTROL						
							break;
						
						case MSG_PRE:
							return;
						
						default:
							break;
					}
				}
#endif				
			}
		}
	}

#endif

//以下为demo,可改。搜索到每个文件都会Call一次
#if 0 //demo
	// 如果想获取特点文件Handle 则需定义全局变量如：FAT_FILE SpecialFile；
	if(FsContext->gFsInfo.FolderSum == 1) //只搜索根目录是否有特定扩展名的文件，注释这句则全盘搜索
	{
		if(memcmp(FsContext->gCurrentEntry->ExtName, "XYZ", 3) == 0)
		{
			FileGetInfo(&SpecialFile); //找到特定文件，获取到了SpecialFile信息。
		}
	}
#endif

}
////////////////////////////////////////////////////////////////


//判断文件的类型，由文件系统底层调用
uint8_t FileGetType(FS_CONTEXT* FsContext)
{
	uint8_t ExtFileName[3];

	ExtFileName[0] = FsContext->gCurrentEntry->ExtName[0];
	ExtFileName[1] = FsContext->gCurrentEntry->ExtName[1];
	ExtFileName[2] = FsContext->gCurrentEntry->ExtName[2];


	if((ExtFileName[0] == 'M') && (ExtFileName[1] == 'P') && (ExtFileName[2] == '3' || ExtFileName[2] == '2'))
	{
		return FILE_TYPE_MP3;
	}
	else if((ExtFileName[0] == 'W') && (ExtFileName[1] == 'M') && (ExtFileName[2] == 'A'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'A') && (ExtFileName[1] == 'S') && (ExtFileName[2] == 'F'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'W') && (ExtFileName[1] == 'M') && (ExtFileName[2] == 'V'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'A') && (ExtFileName[1] == 'S') && (ExtFileName[2] == 'X'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'W') && (ExtFileName[1] == 'A') && (ExtFileName[2] == 'V'))
	{
		return FILE_TYPE_WAV;
	}
	else if((ExtFileName[0] == 'S') && (ExtFileName[1] == 'B') && (ExtFileName[2] == 'C'))
	{
		return FILE_TYPE_SBC;
	}
	else if((ExtFileName[0] == 'F') && (ExtFileName[1] == 'L') && (ExtFileName[2] == 'A'))
	{
		return FILE_TYPE_FLAC;
	}
	else if(((ExtFileName[0] == 'A') && (ExtFileName[1] == 'A') && ExtFileName[2] == 'C') 
		|| ((ExtFileName[0] == 'M') && (ExtFileName[1] == 'P') && ExtFileName[2] == '4') 
		|| ((ExtFileName[0] == 'M') && (ExtFileName[1] == '4') && ExtFileName[2] == 'A'))
	{
		return FILE_TYPE_AAC;
	}
	else if((ExtFileName[0] == 'A') && (ExtFileName[1] == 'I') && (ExtFileName[2] == 'F'))
    {
        return FILE_TYPE_AIF;
    }
	else
	{
		return FILE_TYPE_UNKNOWN;
	}
}

// 获取FS扫描后与播放模式断点信息相匹配的文件、文件夹ID号
#ifdef FUNC_MATCH_PLAYER_BP

static PLAYER_BP_RET sPlayerBPRet = {-1, 1, 1};

// 获取播放断点匹配信息，应该在FS初始化成功后调用
PLAYER_BP_RET* GetMatchPlayerBPRet(void)
{
	return &sPlayerBPRet;
}

void SetMatchPlayerBPRet(int8_t BPIndex, uint16_t FileId, uint16_t FolderId)
{
    sPlayerBPRet.BPIndex  = BPIndex;
    sPlayerBPRet.FileId   = FileId;
    sPlayerBPRet.FolderId = FolderId;
}


////////////////////////////////////////////////////////////
//用于恢复播放断点
static uint32_t FsGetPlayerBPInfo(PLAYER_BP_INFO* PlayerBPInfo, uint32_t BPCnt)
{
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	uint32_t i;
	BP_PLAYER_INFO *BpPlayInfo;
	BpPlayInfo = (BP_PLAYER_INFO *)BP_GetInfo(BP_PLAYER_INFO_TYPE);

	for(i = 0; i < PLAYER_DISK_BP_CNT && i < BPCnt; i++)
	{
		PlayerBPInfo[i].FileAddr = BpPlayInfo->PlayDiskInfo[i].FileAddr;
		PlayerBPInfo[i].CRC8     = BpPlayInfo->PlayDiskInfo[i].CRC8;
	}

	return i;
	
#else
	return 0;
#endif
}

#endif
//
//统计文件和文件夹总数
//
bool PreSearch(FS_CONTEXT* FsContext)
{
	ENTRY_TYPE EntryType;

#ifdef FUNC_MATCH_PLAYER_BP
	uint32_t i, BPCnt = 0;
	PLAYER_BP_INFO NvmPlayerData[3];	
	sPlayerBPRet.BPIndex  = -1;
	// sPlayerBPRet.FolderId = 1;
	// sPlayerBPRet.FileId   = 1;

	BPCnt = FsGetPlayerBPInfo(NvmPlayerData, 3);	
#endif

	FS_DBG("PreSearch start!\n");
#ifdef FUNC_SPI_UPDATE_EN
	UpgradeFileFound = 0;
#endif
	DirSetStartEntry(FsContext, FsContext->gFsInfo.RootStart, 0, TRUE);
	FSINFO_Init(FsContext);
#ifdef FS_EXFAT_EN
	if(gIsExFat)
	{
		InitFolderNoFatChainFlag(FsContext);
		SetEntryNoFatChain(0);
	}
#endif

	FsContext->gFolderDirStart = FsContext->gFsInfo.RootStart;

	while(1)
	{
		if(terminate_cur_fs_proc != NULL && terminate_cur_fs_proc())
		{
			return FALSE;
		}
		EntryType = (ENTRY_TYPE)DirGetNextEntry(FsContext);

		//FS_DBG("fcount:%d\n",++fcount);

		if(FsContext->gEntryOffset == 0)
		{
			FSINFO_SaveFdi(FsContext);
		}

		switch(EntryType)
		{
			case ENTRY_FILE: 	//是歌曲

				//FS_DBG("fcount:%d\n",++fcount);
				if(FileGetType(FsContext) != FILE_TYPE_UNKNOWN)
				{
					//FS_DBG("找到歌曲：%-.8s%-.3s\n", &FsContext->gCurrentEntry->FileName[0], &FsContext->gCurrentEntry->FileName[8]);
					if(FsContext->gFsInfo.FileSum >= 65535)
					{
						return TRUE;
					}
					FsContext->gFsInfo.FileSum++;
					FsContext->gFsInfo.FileSumInFolder++;
					//FS_DBG("Find a audio file:%d!\n", fcount);
#if FS_FILE_SORT_EN == 1
					FileSortSearchNewFile(FsContext);
#endif
					//FsContext->gCurrentEntry 当前目录项
#ifdef FUNC_MATCH_PLAYER_BP
					if(BPCnt > 0 && sPlayerBPRet.BPIndex  == -1)
					{
						for(i = (BPCnt - 1); i > 0; i--)
						{
							if(NvmPlayerData[i].FileAddr ==
							        (FsContext->gDirSecNum + FsContext->gEntryOffset))
							{
								uint8_t Crc8Data[15];
								memcpy(Crc8Data, FsContext->gCurrentEntry->FileName, 8);
								memcpy((void*)(Crc8Data + 8), FsContext->gCurrentEntry->ExtName, 3);
								memcpy((void*)(Crc8Data + 11), (void*)(&FsContext->gCurrentEntry->Size), 4);

								if(NvmPlayerData[i].CRC8 != GetCrc8CheckSum(Crc8Data, sizeof(Crc8Data)))
								{
									continue;
								}
								sPlayerBPRet.BPIndex  = i;
								sPlayerBPRet.FileId   = FsContext->gFsInfo.FileSum;
								sPlayerBPRet.FolderId = FsContext->gFsInfo.FolderSum;
								break;
							}
						}
					}
#endif
				}

				FindSpecialFileCallBack(FsContext);

				break;

			case ENTRY_FOLDER:	//是文件夹，下次循环不会进入这个文件夹内
				FsContext->gFsInfo.FolderNumInFolder++;
				//FS_DBG("ENTRY_FOLDER: %d\n", FsContext->gFsInfo.FolderNumInFolder);
				break;

			case ENTRY_END:		//返回上一级文件夹，调整之前记录的部分节点
				FSINFO_ChangeFileNum(FsContext);
				//记录根文件夹中的文件个数
				if(FsContext->gFsInfo.FolderSum == 1)
				{
					FsContext->gFsInfo.RootFileSum = FsContext->gFsInfo.FileSum;
				}

				//回到当前文件夹的开头位置
				//FS_DBG("开始查找下一个文件夹的位置！\n");
				RewindFolderStart(FsContext);

				//从当前位置，在整个文件系统中寻找下一个文件夹
				if(!FindNextFolder(FsContext))
				{
					//FS_DBG("PreSearch end! %d ms \n", (uint16_t)PastTimeGet(&Timer));
#if FS_FILE_SORT_EN == 1
					FileSortSearchEnd(FsContext);
#endif
					return TRUE;
				}
				else	//进入下一个文件夹成功
				{
					//FS_DBG("Into Next Folder%8s\n", &FsContext->gCurrentEntry->FileName[0]);
					if(FsContext->gFsInfo.FolderSum >= 65535)
					{
#if FS_FILE_SORT_EN == 1
						FileSortSearchEnd(FsContext);
#endif
						return TRUE;
					}
					FsContext->gFsInfo.FolderSum++;
					FsContext->gFsInfo.FolderNumInFolder = 0;
					FsContext->gFsInfo.FileSumInFolder = 0;
					if(!DirEnterSonFolder(FsContext))
					{
						//FS_DBG("进入子文件夹失败！\n");
						FsContext->gFsInfo.FolderSum--;
					}
					//FS_DBG("进入子文件夹成功！\n");
#ifdef FS_EXFAT_EN
					if(gIsExFat)
					{
						if((FsContext->gCurrentEntry->Attribute & 0x80) == 0)
						{
							ClrFolderNoFatChainFlag(FsContext, FsContext->gFsInfo.FolderSum);
							SetEntryNoFatChain(0);
						}
						else
						{
							SetEntryNoFatChain(1);
						}
					}
#endif
				}
				break;

			default:
				break;
		}
		switch(FsContext->gFsInfo.DevID)
		{
			case DEV_ID_USB:
				if(!IsUDiskLink())
				{
					return FALSE;
				}
				break;
			case DEV_ID_SD:
				if(!IsCardLink())
				{
					return FALSE;
				}
				break;
			default:
				break;
		}
	}
}

//
//文件系统初始化，分析分区表，预搜索统计文件系统有关信息.
//
bool FSInit(uint8_t DeviceID)
{

	//get fs memory
	FS_CONTEXT* Fs_Context;

	Fs_Context = gFSContextBufferManager.pfnAllocFSContextBuffer(&gFSContextBufferManager.FSContextBuffer, DeviceID);

	if(Fs_Context == NULL)
	{
		return FALSE;
	}
		
	ENTER_FAT(Fs_Context);

	if(!CheckAllDiskLinkFlag())
	{
		FS_DBG("disk not ready!\n");
		LEAVE_FAT(Fs_Context, FALSE);
	}

	if(Fs_Context->gPreSearchStatus != PRESEARCH_STATUS_NONE)
	{
		FS_DBG("Mount Already\n");
		LEAVE_FAT(Fs_Context, TRUE);
	}

	Fs_Context->gPreSearchStatus = PRESEARCH_STATUS_SEARCHING;

	if(Fs_Context == 0 || FsGetAvailableLogicId() < 0)
	{
		LEAVE_FAT(Fs_Context, FALSE);
	}

	//mount fs
	FsMount(FsGetAvailableLogicId(), Fs_Context);

	FsGetCurContext()->gFsInfo.DevID = (DEV_ID)DeviceID;
	FsGetCurContext()->gFsInfo.CurrFATSecNum = -1;
	//FsGetCurContext()->gFsInfo.CurrDirSecNum = -1;
	FsGetCurContext()->gCurFreeClusNum = 2;


	if(!PartitionLoad(FsGetCurContext(), 0))
	{
		FS_DBG("PartitionLoad() error!\n");
		//return FALSE;
		FsMount(FsGetLogicIdByDeviceType(DeviceID), 0);
		gFSContextBufferManager.pfnFreeFSContextBuffer(&gFSContextBufferManager.FSContextBuffer, DeviceID);
		Fs_Context->gPreSearchStatus = PRESEARCH_STATUS_NONE;
		LEAVE_FAT(Fs_Context, FALSE);
	}
	FS_DBG("PartitionLoad() OK!\n");
	
	{
//		int begin,end;
		//begin = OSSysTickGet();
		//FS_DBG("PreSearch() Begin:%d\n", begin);

		if(!PreSearch(FsGetCurContext()))
		{
			FS_DBG("PreSearch() error!\n");
			//return FALSE;
			LEAVE_FAT(Fs_Context, FALSE);
		}
		//end = OSSysTickGet();
		//FS_DBG("PreSearch() End:%d\n", end - begin);
	}

	Fs_Context->gPreSearchStatus = PRESEARCH_STATUS_SEARCHED;

	FS_DBG("PreSearch() OK\n");


	FS_DBG("Song sum in disk: %d\n", FsGetCurContext()->gFsInfo.FileSum);
	FS_DBG("Folder sum in disk: %d\n", FsGetCurContext()->gFsInfo.FolderSum);
	FS_DBG("Valid folder sum in disk: %d\n", FsGetCurContext()->gFsInfo.ValidFolderSum);
	FS_DBG("***********************************************\n");
	FS_DBG("\n");

	//return TRUE;
	LEAVE_FAT(Fs_Context, TRUE);

//	fs_inited_flag = TRUE;
}

//
//文件系统去初始化
//
bool FSDeInit(uint8_t DeviceID)
{
	bool res = FALSE;

	int8_t LogicId = FsGetLogicIdByDeviceType(DeviceID);

	FS_CONTEXT* Fs_Context = FsGetContextByDeviceType(DeviceID);

#ifdef FUNC_MATCH_PLAYER_BP
//    sPlayerBPRet.BPIndex  = -1;
#endif

	ENTER_FAT(Fs_Context);
	if(LogicId >= 0)
	{
		FsMount(LogicId, 0);

		res = gFSContextBufferManager.pfnFreeFSContextBuffer(&gFSContextBufferManager.FSContextBuffer, DeviceID);
	}
	//return res;
	LEAVE_FAT(Fs_Context, res);

//	fs_inited_flag = FALSE;
}

bool IsFsInited(void)
{
	return fs_inited_flag;
}
