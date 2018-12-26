/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:fsinfo.h
//maintainer lilu
/////////////////////////////////////////////////////////////////////////

#ifndef __FSINFO_H__
#define __FSINFO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <string.h>
#include "fs_config.h"

#define		SECTOR_SIZE			512		//bytes per sector
/////////////////////////滤除空文件夹//////////////////////////////////////////////////////
#define	MAX_FOLDER_NUM_SAVE 	2048			//save foler 1-2048 empty flag

// Define device ID in system.
typedef	enum _DEV_ID
{
    DEV_ID_NONE,
    DEV_ID_USB,
    DEV_ID_SD,
    DEV_ID_SPIFS,
    DEV_ID_ATA
} DEV_ID;

//获取当前默认工作的设备类型
extern uint8_t FsGetDriveType(void);

// File system structure
typedef struct _FS_INFO
{
	DEV_ID 	DevID;				//device id of this drive
	bool 	FAT32;				//is FAT32 file system
	bool	IsCpatFS;			//is compatalbe file system

	bool	IsFATChange;
	//bool	IsDIRChange;

	int32_t	CurrFATSecNum;		//current FAT sector number
	//int32_t	CurrDirSecNum;		//current directory sector number

//	uint8_t  	ClusterSize;		//sector per cluster  ExFat Max Sector Size:32MB
	uint32_t  	ClusterSize;		//sector per cluster
	uint32_t    FatStart;			//the sector number of FAT start
	uint32_t  	RootStart;			//This is set to the cluster number of the first cluster of the root director.,
	uint32_t  	DataStart;			//the first sector of data
	uint32_t  	MaxCluster;			//max cluster number in this drive
	uint32_t  	MaxSector;			//max sector number in this drive

	uint32_t  	FreeClusNum;

	uint16_t	FileSum;			//整个设备上的歌曲文件总数
	uint16_t	FileSumInFolder;	//文件系统内部使用
	uint16_t	FolderSum;			//整个设备上的文件夹总数
	uint16_t	FolderNumInFolder;	//文件系统内部使用

	uint16_t	ValidFolderSum;		//整个设备上的非空文件夹总数

	uint16_t	RootFileSum;		//根文件夹下的文件个数

} FS_INFO;


#define	MAX_STACK_DEPTH 	9
typedef struct _LEAD_PATH
{
	uint32_t		SecNum[MAX_STACK_DEPTH];		//本节点描述项所在扇区号
	uint8_t		Offset[MAX_STACK_DEPTH];		//本节点描述项所在扇区号内部的偏移量
	uint8_t		NodesCnt;

	uint32_t		FdiSecNum;						//末端节点所处扇区号，扇区内偏移量始终为0

	uint16_t 		PreFileNumInDisk;				//本引导节点之前的文件总数，不包括本节点
	uint16_t 		PreFileNumInFolder;				//本引导节点之前同一文件夹中的文件总数，不包括本节点

	uint16_t 		ParentFolderNum;				//本引导节点所在的文件夹号
	uint16_t 		PreFolderNumInFolder;			//在本引导节点所在文件夹内，本引导节点之前同一级的文件夹个数
	uint16_t		PreFolderNumInDisk;				//按文件夹遍历顺序时，本节点之前全盘上所有文件夹个数
	uint16_t		StartFileNumNextFolder;			//本节点所在文件夹以及之前所有文件夹中的所有文件个数，查找下一个文件夹时使用

	uint32_t		FolderDirStart;

} LEAD_PATH;

#define MAX_LEAD_PATH_CNT	10		//最多记录10条引导路径，每条路径上最多有7个引导节点，不同路径上的节点可能是重复的	



#define			MEMORY_PAGE_CNT		1024

typedef struct _FDI_STATCK
{
	uint32_t		SecNum[MAX_STACK_DEPTH];			//本节点描述项所在扇区号
	uint8_t		Offset[MAX_STACK_DEPTH];			//本节点描述项所在扇区号内部的偏移量
	uint16_t		FolderNum[MAX_STACK_DEPTH];			//本节点同一文件夹中，在本节点之前的文件夹数

	uint8_t		Depth;								//本条路径上的节点个数

} FDI_STATCK;


//define File Description Block
#pragma pack(1)
typedef struct _FDB
{
	uint8_t	FileName[8];
	uint8_t	ExtName[3];
	uint8_t	Attribute;
	uint8_t 	NTRes;
	uint8_t	CrtTimeTenth;
	uint16_t	CreateTime;
	uint16_t	CreateDate;
	uint16_t	AccessDate;
	uint16_t	StartHigh;			//this entry's first cluster number. High
	uint16_t	ModifyTime;
	uint16_t	ModifyDate;
	uint16_t	StartLow;			//this entry's first cluster number. Low
	uint32_t	Size;				//32-bit uint32_t holding this file’s size in bytes

} FDB;
#pragma pack()

#pragma pack(1)
// Define long name dirctory entry sturct.
typedef struct _LONG_DIR_ENTRY
{
	uint8_t		Ord;
	uint8_t		Name1[10];
	uint8_t		Attribute;
	uint8_t		Type;
	uint8_t		CheckSum;
	uint8_t		Name2[12];
	uint8_t		Resv[2];
	uint8_t		Name3[4];

} LONG_DIR_ENTRY;
#pragma pack()

#define PRESEARCH_STATUS_NONE        0x00
#define PRESEARCH_STATUS_SEARCHING   0x01
#define PRESEARCH_STATUS_SEARCHED    0x02
typedef struct _FS_IO_BUFFER
{
	uint8_t Buffer[SECTOR_SIZE];
	uint32_t BufferSecNum;	
	bool	IsBufferChange;//不包含Buffer 和BufferSecNum同时改变
}FS_IO_BUFFER;

// File system context
typedef struct _FS_CONTEXT
{
	// File System memory address.
	uint8_t CurrentBufferFlag;	//指示IO_BUFFER中哪个是最近更新的
	FS_IO_BUFFER IO_BUFFER[2];	//整合原FileIoBuffer、DIR_BUFFER，复用节省内存
	
	uint8_t FAT_BUFFER[SECTOR_SIZE];


	FS_INFO gFsInfo;  // File system structure

	uint8_t	gPreSearchStatus;
	//建立文件系统快速引导信息
	//每个快速引导节点包括的信息：目录项所在扇区号、到根文件夹的路径、所在文件夹编号、前一个歌曲文件编号
	//打开文件和打开文件夹时利用此引导信息，实现快速定位到目标文件或文件夹
	//打开文件夹时，利用此引导信息，快速统计文件夹包含的文件和子文件夹个数
	LEAD_PATH			LeadPaths[MAX_LEAD_PATH_CNT];
	uint16_t				CurrentInterval;
	uint16_t				FdiCnt;
	uint8_t				OutIndex;

	//
	uint16_t	gPreFileNum;
	uint16_t	gPreFileNumInFolder;
	uint16_t	gPreFolderNum;
	uint16_t	gPreFolderNumInFolder;
	bool	gFindFolderFlag;


	uint16_t 	FolderStartFileNum;

	bool	gMemoryPageMapValid;

	//dir
	FDB*			gCurrentEntry;
	uint32_t			gDirSecNum;
	uint8_t 			gEntryOffset;
	uint32_t 			gFolderDirStart;
	FDI_STATCK		FdiStack;
	//bool			gExcludeRecycled = FALSE;

	//fat_write
	uint8_t			gMemoryPageMap[MEMORY_PAGE_CNT / 8];
	uint32_t			gClusStepNum;

	//folder
	uint16_t IncFileSum;
	uint16_t IncFolderSum;

	uint8_t gFolderEmpty[MAX_FOLDER_NUM_SAVE / 8];		//one bit per folder

	uint32_t gCurFreeClusNum;


} FS_CONTEXT;

extern bool FSInit(uint8_t DeviceID);
extern bool FSDeInit(uint8_t DeviceID);

//下面一组函数为文件系统内部使用。
void FSINFO_Init(FS_CONTEXT* FsContext);
void FSINFO_SaveFdi(FS_CONTEXT* FsContext);
void FSINFO_ChangeFolderNum(FS_CONTEXT* FsContext);
void FSINFO_ChangeFileNum(FS_CONTEXT* FsContext);
void FSINFO_InitOpenFolder(FS_CONTEXT* FsContext, uint16_t StartFolderNum, uint16_t FolderNum);
bool FSINFO_FindLastNode(FS_CONTEXT* FsContext,  uint32_t SecNum, uint8_t Offset, uint8_t Depth, bool RecFlag, bool RootFlag);
void FSINFO_InitOpenFile(FS_CONTEXT* FsContext, uint16_t FileNum);


//将某个文件夹标识设置为非空。
void ClrFolderEmptyFlag(FS_CONTEXT* FsContext, uint16_t FolderNum);

//将某个文件夹标识设置为空。
void SetFolderEmptyFlag(FS_CONTEXT* FsContext, uint16_t FolderNum);

//判断某个文件夹是否为空。
bool IsFolderEmpty(FS_CONTEXT* FsContext, uint16_t FolderNum);

//根据原始文件夹号计算有效文件夹号（滤除空文件夹后的序号）。
uint16_t GetValidFolderNum(FS_CONTEXT* FsContext, uint16_t OriginalFolderNum);

//根据有效文件夹号（滤除空文件夹后的序号）计算原始文件夹号。
uint16_t GetOriginalFolderNum(FS_CONTEXT* FsContext, uint16_t ValidFolderNum);

//计算非空文件夹数目
//uint16_t GetValidFolderSum();

// 获取当前已初始化，并正在使用的设备类型
DEV_ID GetCurFsDevType(void);

// 获取FS扫描后与播放模式断点信息相匹配的文件、文件夹ID号
#ifdef FUNC_MATCH_PLAYER_BP
typedef struct __PLAYER_BP_RET_
{
	int8_t BPIndex; // 相匹配的播放断点序号(0~2), -1 -- not found
	uint16_t FileId;   // 播放断点所在文件ID
	uint16_t FolderId; // 播放断点所在文件夹ID
} PLAYER_BP_RET;

typedef struct _PLAYER_BP_INFO_ // 播放模式 断点信息
{
	uint32_t FileAddr;
	uint8_t  CRC8;
} PLAYER_BP_INFO;


// 获取播放断点匹配信息，应该在FS初始化成功后调用
PLAYER_BP_RET* GetMatchPlayerBPRet(void);
void SetMatchPlayerBPRet(int8_t BPIndex, uint16_t FileId, uint16_t FolderId);

#endif

const unsigned char *GetLibVersionFs(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
