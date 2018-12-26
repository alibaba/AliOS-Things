/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:partition.c
//  maintainer: lilu
/////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "fs_config.h"
#include "folder.h"
#include "fat_file.h"
#include "fsinfo.h"
#include "diskio.h"
#include "presearch.h"
#include "host_stor.h"

#define SIGNATURE_OFFSET    	0x01fe            		// will contain 0x55AA
#define PARTITION_OFFSET    	0x01be            		// will contain partition table

#define COMMON_SIGNATURE    	0xAA55
#ifdef FS_EXFAT_EN
extern bool		gIsExFat;
extern uint8_t*		BitmapBuffer;
extern uint8_t*		gFolderNoFatChain;
uint8_t		ExFatBuffer[768];
#endif

//define partition structure
#pragma pack(1)
typedef struct _PARTITION         // Partition Entries definition
{
	uint8_t 	Active;
	uint8_t 	StartHead;
	uint8_t 	StartSector;
	uint8_t 	StartCylinder;
	uint8_t 	PartitionType;
	uint8_t 	EndHead;
	uint8_t 	EndSector;
	uint8_t 	EndCylinder;
	uint32_t 	FirstSectorNum;
	uint32_t 	SectorCount;

} PARTITION;
#pragma pack()


//define BPB structe
#pragma pack(1)
typedef struct
{
	uint8_t 	BS_jmpBoot[3];		// 0-2
	uint8_t		BS_OEMName[8];		// 3-10
	uint16_t	BPB_BytsPerSec;		// 11-12
	uint8_t		BPB_SecPerClus;		// 13
	uint16_t	BPB_RsvdSecCnt;		// 14-15
	uint8_t		BPB_NumFATs;		// 16
	uint16_t	BPB_RootEntCnt;		// 17-18
	uint16_t	BPB_TotSec16;		// 19-20
	uint8_t		BPB_Media;			// 21
	uint16_t	BPB_FATSz16;		// 22-23
	uint16_t	BPB_SecPerTrk;		// 24-25
	uint16_t	BPB_NumHeads;		// 26-27
	uint32_t	BPB_HiddSec;		// 28-31
	uint32_t	BPB_TotSec32;		// 32-35
	uint32_t	BPB_FATSz32;		// 36-39
	uint16_t	BPB_ExtFlags;		// 40-41
	uint16_t	BPB_FSVer;			// 42-43
	uint32_t	BPB_RootClus;		// 44-47
	uint16_t	BPB_FSInfo;			// 48-49
	uint16_t	BPB_BkBootSec;		// 50-51

} BOOT_SECTOR;

typedef struct
{
	uint8_t		JupmBoot[3];		// 0-2
	uint8_t		FileSystemName[8];	// 3-10
	uint8_t		MustBeZero[53];		// 11-63
	uint64_t 	PartitionOffset;	// 64-71 partition first sector  
	uint64_t 	VolumeLength;		// 72-79 partition sectors count  
	uint32_t 	FatOffset;			// 80-83 FAT first sector  
	uint32_t 	FatLength;			// 84-87 FAT sectors count  
	uint32_t 	ClusterHeapOffset;	// 88-91 first cluster sector  
	uint32_t 	ClusterCount;		// 92-95 total clusters count  
	uint32_t 	RootDirCluster;		// 96-99 first cluster of the root dir  
	uint32_t 	VolumeSerial;		// 100-103 volume serial number
	uint8_t 	FsVersionMinor;		// 104 FS version 
	uint8_t 	FsVersionMajor;		// 105
	uint16_t 	VolumeFlag;			// 106-107 volume state flags  
	uint8_t 	BytesPerSector;		// 108 sector size as (1 << n)  
	uint8_t 	SectorsPerCluster;	// 109 sectors per cluster as (1 << n)  
	uint8_t 	NumOfFATS;			// 110 always 1  
	uint8_t 	DriverSelect;		// 111 always 0x80  
	uint8_t 	PercentInUse;		// 112 percentage of allocated space  
} EXFAT_BOOT_SECTOR;
#pragma pack()

#define CANCEL_COMMON_SIGNATURE_JUDGMENT 
// Load partition information from storage device.
bool PartitionLoad(FS_CONTEXT* FsContext, uint8_t DriverId)
{
	uint32_t		RootDirSectors;
	uint32_t		BootSecNum = 0;
	uint32_t		FatSize;
	uint32_t		TotalSectors;
	uint8_t			IndexPtb;
	uint8_t			SectorSizeMul;
	uint32_t		temp2;
	uint8_t			ExtBootEntry = 0;
	uint16_t		BPBRsvSectCnt;
	uint8_t			LogicDriverId = 1;
	uint8_t			LoopFlag = 0;

#ifdef FUNC_USB_EN
	if(FsContext->gFsInfo.DevID == DEV_ID_USB)
	{
		SectorSizeMul = (HostStorGetBlockSize() / SECTOR_SIZE);
	}
	else
#endif
	{
		SectorSizeMul = 1;
	}
//	FS_DBG("SectorSizeMul: %d\n", (uint16_t)SectorSizeMul);
#ifdef FS_EXFAT_EN
	BitmapBuffer = ExFatBuffer;
	gFolderNoFatChain = &ExFatBuffer[512];
#endif
	while(1)
	{
		LoopFlag = 0;
		//read MBR
		//if(!ReadBlock(BootSecNum, (uint8_t*)(FsContext->FAT_BUFFER), 1))
		if(RES_OK != disk_read(FsContext->gFsInfo.DevID, (uint8_t*)(FsContext->FAT_BUFFER), BootSecNum, 1))
		{
			FS_DBG("ReadBlock() error!\n");
			return FALSE;
		}
		FsContext->gFsInfo.CurrFATSecNum = BootSecNum;
	#ifndef CANCEL_COMMON_SIGNATURE_JUDGMENT
		if(*(uint16_t*)((FsContext->FAT_BUFFER) + SIGNATURE_OFFSET) != COMMON_SIGNATURE)		//if 0x55aa
		{
			return FALSE;
		}
	#endif
#ifdef FS_EXFAT_EN
		if(0 == memcmp("EXFAT", &(FsContext->FAT_BUFFER[3]), 5))
		{
			gIsExFat = TRUE;
			break;
		}
		else
		{
			gIsExFat = FALSE;
		}
#endif
		for(IndexPtb = 0; IndexPtb < 4; IndexPtb++)
		{
			PARTITION* pPart = &((PARTITION*)((FsContext->FAT_BUFFER) + PARTITION_OFFSET))[IndexPtb];

			if((pPart->PartitionType == 0x05)			//Extended partition
					&& ((ExtBootEntry == 0) || (IndexPtb >= ExtBootEntry)))
			{
				BootSecNum += Le32ToCpu(pPart->FirstSectorNum);
				ExtBootEntry = IndexPtb + 1;
	//			FS_DBG("Extended partition bootSecNum:%ld\n", (uint32_t)BootSecNum);
				LoopFlag = 1;
				break;
			}

			//check the partition type and get the boot sector num
			if(pPart->PartitionType == 0x04
					|| pPart->PartitionType == 0x06
					|| pPart->PartitionType == 0x0B
					|| pPart->PartitionType == 0x0C
					|| pPart->PartitionType == 0x01
					|| pPart->PartitionType == 0x0E)
			{
				if(DriverId <= 1 || LogicDriverId == DriverId)//LogicDriverId ???
				{
					BootSecNum += Le32ToCpu(pPart->FirstSectorNum);
					break;
				}
				LogicDriverId++;
			}
#ifdef FS_EXFAT_EN
			if(pPart->PartitionType == 0x07)
			{
				if(DriverId <= 1 || LogicDriverId == DriverId)
				{
					BootSecNum += Le32ToCpu(pPart->FirstSectorNum);
					gIsExFat = TRUE;
					break;
				}
				LogicDriverId++;
			}
#endif
		}
		if(LoopFlag)
		{
			continue;
		}

		if((IndexPtb == 4) && ((ExtBootEntry > 0) && (ExtBootEntry < 4)))
		{
			//if extended parttion failed,try again.
			BootSecNum = 0;
			continue;//增加对第一分区为无效扩展分区设备的支持
		}
		break;
	}

	if((DriverId > 1) && (LogicDriverId != DriverId))
	{
		return FALSE;
	}

	//read DBR
	if(RES_OK != disk_read(FsContext->gFsInfo.DevID, (uint8_t*)(FsContext->FAT_BUFFER), (BootSecNum * SectorSizeMul), 1))
	{
		return FALSE;
	}
	
	FsContext->gFsInfo.CurrFATSecNum = (BootSecNum * SectorSizeMul);
#ifndef CANCEL_COMMON_SIGNATURE_JUDGMENT
	if(*(uint16_t*)((FsContext->FAT_BUFFER)  + SIGNATURE_OFFSET) != COMMON_SIGNATURE)		//if 0x55aa
	{
		return FALSE;
	}
#endif
#ifdef FS_EXFAT_EN
	if(gIsExFat)//------------------------------------------------------------ExFat Start
	{
		FsContext->gFsInfo.FAT32 = TRUE;
		FsContext->gFsInfo.IsCpatFS = TRUE;
		
	#define ExFatBoot ((EXFAT_BOOT_SECTOR*)((FsContext->FAT_BUFFER)))
		FsContext->gFsInfo.ClusterSize = 1 << (ExFatBoot->SectorsPerCluster);
		FsContext->gFsInfo.FatStart = ExFatBoot->FatOffset + BootSecNum;
		FsContext->gFsInfo.DataStart = ExFatBoot->ClusterHeapOffset + BootSecNum;
		FsContext->gFsInfo.MaxCluster = ExFatBoot->ClusterCount;
		FsContext->gFsInfo.RootStart = FsContext->gFsInfo.DataStart + (ExFatBoot->RootDirCluster - 2) * FsContext->gFsInfo.ClusterSize;
		
		FsContext->gFsInfo.ClusterSize *= SectorSizeMul;
		FsContext->gFsInfo.FatStart *= SectorSizeMul;
		FsContext->gFsInfo.DataStart *= SectorSizeMul;
		FsContext->gFsInfo.RootStart *= SectorSizeMul;
		
		FsContext->gFsInfo.MaxSector = ExFatBoot->VolumeLength;
		return TRUE;
	}							//------------------------------------------------------------ExFat End
#endif
	
#define pzero_sector ((BOOT_SECTOR*)((FsContext->FAT_BUFFER)))

	//updata device information
	FsContext->gFsInfo.ClusterSize = pzero_sector->BPB_SecPerClus;

	//get first sector of FAT
	BPBRsvSectCnt = Le16ToCpu(pzero_sector->BPB_RsvdSecCnt);
	FsContext->gFsInfo.FatStart = BPBRsvSectCnt + BootSecNum;

	//get sectors of boot dir
	if(pzero_sector->BPB_RootEntCnt != 0)
	{
		RootDirSectors = (Le16ToCpu(pzero_sector->BPB_RootEntCnt) * 32 + SECTOR_SIZE - 1) / SECTOR_SIZE;
	}
	else
	{
		RootDirSectors = 0;
	}

	//get FAT size
	if(pzero_sector->BPB_FATSz16 != 0)
	{
		FatSize = Le16ToCpu(pzero_sector->BPB_FATSz16);
	}
	else
	{
		FatSize = Le32ToCpu(pzero_sector->BPB_FATSz32);
	}

	//get total cluster
	if(pzero_sector->BPB_TotSec16 != 0)
	{
		TotalSectors = Le16ToCpu(pzero_sector->BPB_TotSec16);
	}
	else
	{
		TotalSectors = Le32ToCpu(pzero_sector->BPB_TotSec32);
	}

	temp2 = (TotalSectors - (BPBRsvSectCnt + FatSize * pzero_sector->BPB_NumFATs + RootDirSectors)) / FsContext->gFsInfo.ClusterSize;
//	FS_DBG("temp2:%ld\n", (uint32_t)temp2);

	if(temp2 < 4085)
	{
		FS_DBG("FAT12\n");
		FsContext->gFsInfo.IsCpatFS = FALSE;				//FAT12
		return TRUE;
	}
	else if(temp2 < 65525)
	{
		FS_DBG("FAT16\n");
		FsContext->gFsInfo.FAT32 = 0;						//FAT16
		FsContext->gFsInfo.IsCpatFS = TRUE;
	}
	else
	{
		//FAT32设备BPB_RootEntCnt/BPB_TotSec16/BPB_FATSz16数值必须为零
		if(memcmp((uint8_t*)((FsContext->FAT_BUFFER) + 0x36), "FAT16\0\0\0", 8) == 0)
		{
			//客户有一个U盘(USB启动盘)，根据计算得出的簇数目应该为FAT32，实际为FAT16系统，为支持这一类设备，需要做此处理
			FS_DBG("FAT16\n");
			FsContext->gFsInfo.FAT32 = 0;
		}
		else
		{
			FS_DBG("FAT32\n");
			FsContext->gFsInfo.FAT32 = 1;						//FAT32
		}
		FsContext->gFsInfo.IsCpatFS = TRUE;
	}

	FsContext->gFsInfo.DataStart = FsContext->gFsInfo.FatStart + (FatSize * pzero_sector->BPB_NumFATs) + RootDirSectors;		//the first sector of data
	// get max cluster number in this drive.
	//FsContext->gFsInfo.MaxCluster = ((TotalSectors - FsContext->gFsInfo.DataStart) / FsContext->gFsInfo.ClusterSize) + 2;
	FsContext->gFsInfo.MaxCluster = temp2 + 2;

	//get the data start and boot dir start
	if(FsContext->gFsInfo.FAT32)
	{
		FsContext->gFsInfo.RootStart = FsContext->gFsInfo.DataStart + (Le32ToCpu(pzero_sector->BPB_RootClus) - 2) * FsContext->gFsInfo.ClusterSize;
	}
	else
	{
		FsContext->gFsInfo.RootStart = FsContext->gFsInfo.FatStart + FatSize * pzero_sector->BPB_NumFATs;						//FAT16 the first sector of root dir
	}

	FsContext->gFsInfo.ClusterSize *= SectorSizeMul;
//	RootDirSectors *= SectorSizeMul;
	FsContext->gFsInfo.FatStart *= SectorSizeMul;
	FsContext->gFsInfo.DataStart *= SectorSizeMul;
	FsContext->gFsInfo.MaxSector = (BootSecNum + TotalSectors) * SectorSizeMul;
	FsContext->gFsInfo.RootStart *= SectorSizeMul;


//	FS_DBG("*************UDisk virtual params******************\n");
//	FS_DBG("ClusterSize:%ld\n", (uint32_t)FsContext->gFsInfo.ClusterSize);
//	FS_DBG("RootDirSectors:%ld\n", (uint32_t)RootDirSectors);
//	FS_DBG("FatStart:%ld\n", (uint32_t)FsContext->gFsInfo.FatStart);
//	FS_DBG("RootStart:%ld\n", (uint32_t)FsContext->gFsInfo.RootStart);
//	FS_DBG("DataStart:%ld\n", (uint32_t)FsContext->gFsInfo.DataStart);
//	FS_DBG("MaxCluster:%ld\n", (uint32_t)FsContext->gFsInfo.MaxCluster);
//	FS_DBG("*****************************************************\n\n");

	return TRUE;
}



