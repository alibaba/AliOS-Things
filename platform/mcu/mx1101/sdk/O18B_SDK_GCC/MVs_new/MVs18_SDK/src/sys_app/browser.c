///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: browser.c
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "fat_file.h"
#include "folder.h"

#ifdef FUNC_BROWSER_EN

#define DISP_SHORT_NAME_LEN	11		//非当前行显示的最大长度
#define DISP_LONG_NAME_LEN	66		//当前行显示的最大长度
#define DISP_ROW_CNT		5		//最多显示多少行

//文件浏览节点信息，可以保存文件或文件夹信息。
typedef struct _BW_NODE
{
	uint8_t Name[DISP_SHORT_NAME_LEN];

	bool 	 IsFile;		//TRUE: file, FALSE: folder
	uint16_t NumInDisk;		//文件或文件夹在全盘上的序号
	uint16_t NumInFolder;	//文件或文件夹在所在目录中的序号
	uint16_t IncFileCnt;	//如果是文件夹，其包含的文件个数（不递归）
	uint16_t IncFolderCnt;	//如果是文件夹，其包含的文件夹个数（不递归）

} BW_NODE;

//浏览路径节点信息
typedef struct _BW_PATH_NODE
{
	FOLDER 	 Folder;
	uint16_t StartRowNum;
	uint8_t	 CurNodeIndex;

} BW_PATH_NODE;


static BW_NODE	BrowserNodes[DISP_ROW_CNT];			//缓存当前浏览页面中需要显示的文件或文件夹的信息
static uint8_t	CurNodeIndex;						//当前行在缓存中的位置
static uint8_t	NodesCnt;							//缓冲区中缓存的个数
static uint16_t	StartRowNum;						//缓冲区中第一个节点的序号
//static uint8_t 	CurLongName[DISP_LONG_NAME_LEN];	//缓存当前行的文件或文件夹的长名称，用于滚屏显示
static FOLDER	CurFolder;							//当前正在浏览的文件夹

//记录从根文件夹到当前文件夹的路径信息，以便能够返回上一级文件夹
static BW_PATH_NODE 	BrowserPath[MAX_STACK_DEPTH];
static uint8_t			CurPathLevel;


static void BW_Display(void)
{
	uint8_t i;
		
	DBG("\n****");
	
	//显示当前文件夹路径
	for(i = 0; i < CurPathLevel; i++)
	{
		DBG("%-.8s\\", &BrowserPath[i].Folder.ShortName[0]);
	}

	DBG("%-.8s(%d, %d)*******\n", 
		&CurFolder.ShortName[0],		
		CurFolder.IncFolderCnt, 
		CurFolder.IncFileCnt);

	for(i = 0; i < NodesCnt; i++)
	{
		if(i == CurNodeIndex)
		{
			DBG(">>");	
		}
		else
		{
			DBG("  ");	
		}

		if(!BrowserNodes[i].IsFile)		
		{	
			//显示文件信息			
			DBG("[] %-.8s (%d, %d)\n", 
				&BrowserNodes[i].Name[0], 
				BrowserNodes[i].IncFolderCnt, 
				BrowserNodes[i].IncFileCnt);			
		}
		else							
		{	
			//显示文件信息
			DBG("** %-.8s.%-.3s\n", &BrowserNodes[i].Name[0], &BrowserNodes[i].Name[8]);	
		}
	}
	DBG("*************************\n\n");	
}


static bool BW_LoadNodes(uint8_t NodeIndex, uint16_t Num, uint8_t n)
{
	FOLDER Folder;
	FAT_FILE File;
	
	DBG("LoadNode(%d, %d, %d)\n", (uint16_t)NodeIndex, (uint16_t)Num, (uint16_t)n);
	DBG("(%d, %d)\n", (uint16_t)CurFolder.IncFolderCnt, (uint16_t)CurFolder.IncFolderCnt);		
	while(n--)
	{
		if(NodeIndex >= DISP_ROW_CNT)
		{
			return FALSE;
		}

		if(Num <= CurFolder.IncFolderCnt)
		{
		 	//加载文件夹信息
			if(!FolderOpenByNum(&Folder, &CurFolder, Num))
			{
				DBG("Open folder error 001!\n");
				return FALSE;
			}
			memcpy(&BrowserNodes[NodeIndex].Name[0], &Folder.ShortName[0], 8);
			BrowserNodes[NodeIndex].IsFile = FALSE;
			BrowserNodes[NodeIndex].NumInFolder = Num;
			BrowserNodes[NodeIndex].IncFileCnt = Folder.IncFileCnt;
			BrowserNodes[NodeIndex].IncFolderCnt = Folder.IncFolderCnt;
		}
		else if(Num <= CurFolder.IncFolderCnt + CurFolder.IncFileCnt)	
		{
			//加载文件信息
			if(!FileOpenByNum(&File, &CurFolder, Num - CurFolder.IncFolderCnt))
			{
			 	DBG("Open file error!\n");
				return FALSE;
			}
			memcpy(&BrowserNodes[NodeIndex].Name[0], &File.ShortName[0], 11);
			BrowserNodes[NodeIndex].IsFile = TRUE;
			BrowserNodes[NodeIndex].NumInFolder = Num - CurFolder.IncFolderCnt;
			BrowserNodes[NodeIndex].NumInDisk = File.FileNumInDisk;
			BrowserNodes[NodeIndex].IncFileCnt = 0;		//该域只对文件夹有效
			BrowserNodes[NodeIndex].IncFolderCnt = 0;	//该域只对文件夹有效
			FileClose(&File);
		}
		else
		{
		 	return FALSE;
		}

		Num++;
		NodeIndex++;
	}

	return TRUE;
}


//显示当前文件夹中子文件夹和文件（不递归）
//如果StartNum == 0: 在现有基础上加载信息，Num为目标行号
//如果StartNum != 0: 从StartNum项开始加载，Num为目标行号
static bool BW_LoadInfo(uint16_t StartNum, uint16_t Num)
{	
	uint8_t i;

	if(Num == 0)
	{
		Num = 1;
	}
	else if(Num > CurFolder.IncFolderCnt + CurFolder.IncFileCnt)
	{
		Num = CurFolder.IncFolderCnt + CurFolder.IncFileCnt;
	}

	if(!StartNum)
	{
		//缓冲区中已有缓存项: StartRowNum ~ StartRowNum+LoadCnt
		if(Num < StartRowNum - 1)
		{
			//全部重新加载，从Num开始加载DISP_ROW_CNT行, CurNodeIndex = 0
			CurNodeIndex = 0;
			StartRowNum = Num;
			BW_LoadNodes(0, Num, DISP_ROW_CNT);
		}
		else if(Num == StartRowNum - 1)
		{
			//全体后移一项，只加载Num一行，到缓冲区第一项, CurNodeIndex = 0
			for(i = DISP_ROW_CNT - 1; i > 0; i--)
			{
				memcpy(&BrowserNodes[i], &BrowserNodes[i - 1], sizeof(BW_NODE));
			}
			CurNodeIndex = 0;
			StartRowNum = Num;
			BW_LoadNodes(0, Num, 1);
		}
		else if(Num > StartRowNum + DISP_ROW_CNT)
		{
			//全部重新加载，从(Num - DISP_ROW_CNT + 1)开始加载DISP_ROW_CNT行, CurNodeIndex = (DISP_ROW_CNT - 1)
			CurNodeIndex = DISP_ROW_CNT - 1;
			StartRowNum = Num - DISP_ROW_CNT + 1;
			BW_LoadNodes(0, StartRowNum, DISP_ROW_CNT);
		}
		else if(Num == StartRowNum + DISP_ROW_CNT)
		{
			//全体前移一项，只加载Num一行，到缓冲区最后一项, CurNodeIndex = (DISP_ROW_CNT - 1)
			memcpy(&BrowserNodes[0], &BrowserNodes[1], sizeof(BW_NODE)*(DISP_ROW_CNT - 1));
			CurNodeIndex = DISP_ROW_CNT - 1;
			StartRowNum = Num - DISP_ROW_CNT + 1;
			BW_LoadNodes(DISP_ROW_CNT - 1, Num, 1);
		}
		else
		{
			//不需要重新加载, CurNodeIndex = (Num - StartRowNum)
			CurNodeIndex = Num - StartRowNum;
		}
	}
	else
	{
		//初始化全部重加载
		//从StartNum行开始加载若干项，CurNodeIndex = (Num - StartNum)
		CurNodeIndex = Num - StartNum;
		StartRowNum = StartNum;
		
		NodesCnt = CurFolder.IncFolderCnt + CurFolder.IncFileCnt;
		if(NodesCnt > DISP_ROW_CNT)
		{
			NodesCnt = DISP_ROW_CNT;
		}

		BW_LoadNodes(0, StartNum, NodesCnt);
	}

	BW_Display();
	return TRUE;
}


//static void DisplayPath(void)
//{
//	uint8_t i;
//
//	DBG(("*******Path: %d*********\n", (WORD)CurPathLevel));
//	for(i = 0; i < CurPathLevel; i++)
//	{
//		DBG(("%-.8s  %d %d\n", 
//			&BrowserPath[i].Folder.ShortName[0],
//			(WORD)BrowserPath[i].StartRowNum,
//			(WORD)BrowserPath[i].CurNodeIndex));
//	}
//}


//进入当前文件夹的某个子文件夹
//路径中不包含当前文件夹信息
static bool BW_EnterSubFolder(uint16_t SubFolderNum)
{
	//检查是否到最大深度，根文件夹深度为0
	if(CurPathLevel >= MAX_STACK_DEPTH - 1)
	{
		return FALSE;
	}
	
	//将当前文件夹保存到路径
	memcpy(&BrowserPath[CurPathLevel].Folder, &CurFolder, sizeof(FOLDER));
	BrowserPath[CurPathLevel].StartRowNum = StartRowNum;
	BrowserPath[CurPathLevel].CurNodeIndex = CurNodeIndex;
	CurPathLevel++;
//	DisplayPath();

	//打开子文件夹
	if(!FolderOpenByNum(&CurFolder, &CurFolder, SubFolderNum))
	{
	 	DBG("Open folder error 002!\n");
		return FALSE;
	}

	DBG("Open folder OK!\n");

	//显示当前文件夹中子文件夹和文件（不递归）
	BW_LoadInfo(1, 1);

	return TRUE;
}


//返回当前文件夹的上级文件夹
static bool BW_EnterParentFolder(void)
{
	//将当前文件夹从路径中删除
	if(CurPathLevel == 0)
	{
		DBG("已是根目录！\n"); 
		return FALSE;
	}

//	DisplayPath();
	CurPathLevel--;	
	memcpy(&CurFolder, &BrowserPath[CurPathLevel].Folder, sizeof(FOLDER));
	BW_LoadInfo(BrowserPath[CurPathLevel].StartRowNum, BrowserPath[CurPathLevel].StartRowNum + BrowserPath[CurPathLevel].CurNodeIndex);  
	return TRUE;
}


//Browser initial.
bool FileBrowseInit(void)
{
	DBG("FileBrowseInit()\n");
	
	CurPathLevel = 0;

	//打开根目录
	if(!FolderOpenByNum(&CurFolder, NULL, 1))
	{
	 	DBG("Open root error!\n");
		return FALSE;
	}
	DBG("Open root OK!\n");

	DBG("%-.8s (%d folder, %d files)  %ld  %d\n", 
		CurFolder.ShortName, 
		CurFolder.IncFolderCnt, 
		CurFolder.IncFileCnt, 
		CurFolder.StartSecNum, 
		CurFolder.StartFileNum);

	//显示当前文件夹中子文件夹和文件（不递归）
	BW_LoadInfo(1, 1);	
	return TRUE;
}


bool FileBrowseOp(uint16_t Msg)
{
	DBG("FileBrowseOp()\n");
	switch(Msg)
	{
		case MSG_PRE:		//浏览上一个文件夹
			BW_LoadInfo(0, StartRowNum + CurNodeIndex - 1);
			break;

		case MSG_NEXT:		//浏览下一个文件夹	
			BW_LoadInfo(0, StartRowNum + CurNodeIndex + 1);
			break;

		case MSG_PLAY_PAUSE:	//进入子文件夹，或播放歌曲
			if(BrowserNodes[CurNodeIndex].IsFile)
			{
				return TRUE;
			}
			else
			{
				DBG("进入子文件夹\n");
				BW_EnterSubFolder(BrowserNodes[CurNodeIndex].NumInFolder);
			}
			break;

		case MSG_STOP:	//返回上一级文件夹
			BW_EnterParentFolder();
			break;

		default:
			break;
	}
	return FALSE;
}

bool FileBrowseGetResult(FOLDER** Folder, uint16_t* FileNumInFolder, uint16_t* FileNumInDisk)
{
	DBG("FileBrowseGetResult()\n");
	if(Folder != NULL)
	{
		*Folder = &CurFolder;
	}
	
	if(FileNumInFolder != NULL)
	{
		*FileNumInFolder = BrowserNodes[CurNodeIndex].NumInFolder;
	}
	
	if(FileNumInDisk != NULL)
	{
		*FileNumInDisk = BrowserNodes[CurNodeIndex].NumInDisk;
	}
	return TRUE;
}

#endif

