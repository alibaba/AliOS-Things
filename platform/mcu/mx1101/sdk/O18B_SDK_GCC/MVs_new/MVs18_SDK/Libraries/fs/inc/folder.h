//maintainer lilu
#ifndef __FOLDER_H__
#define __FOLDER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fsinfo.h"
// Folder info structure define.
typedef struct	_FOLDER
{
	uint16_t			FolderNum;							//文件夹号
	uint16_t			ValidFolderNum;						//有效文件夹号

	uint32_t	        ParentFoldStartSecNum;	            //父目录起始扇区号
	uint32_t			DirSecNum;							//FDI所在扇区号
	uint8_t			    DirOffset;							//FDI在所在扇区中的偏移量

	uint32_t			EndDirSecNum;						//文件夹中最后一个文件目录项所在扇区号
	uint8_t			    EndDirOffset;						//文件夹中最后一个文件目录项所在扇区中的偏移量

	uint32_t			StartSecNum;
	uint8_t			    ShortName[9];						//folder short name

	uint16_t			StartFileNum;						//for calculate absolute file number in whole partition.

	uint16_t			IncFileCnt;							//include file sum of the folder, not recursion
	uint16_t			IncFolderCnt;

	uint16_t			RecFileCnt;							//recursion file sum of the current folder
	uint16_t			RecFolderCnt;

	FS_CONTEXT*         FsContext;

} FOLDER;


//按文件夹序号打开文件夹
//打开成功：返回TRUE，目标文件夹信息填充到Folder所指的结构体。
//打开失败：返回FALSE。
//ParentFolder == NULL: 打开整个设备上的第FolderNum个文件夹。
//ParentFolder != NULL: 打开ParentFolder文件夹中的第FolderNum个文件夹。
bool FolderOpenByNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t FolderNum);

//按文件夹有效序号（滤除空文件夹后的序号）打开文件夹。
//函数功能类似于FolderOpenByNum()。
bool FolderOpenByValidNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t ValidFolderNum);

//按文件夹名称打开文件夹。
//打开成功：返回TRUE，目标文件夹信息填充到Folder所指的结构体。
//打开失败：返回FALSE。
//ParentFolder == NULL: 打开根目录中的FolderName文件夹。
//ParentFolder != NULL: 打开ParentFolder文件夹中的FolderName文件夹。
//FolderName[]长度不大于8字节，根目录名称为"\\"
bool FolderOpenByName(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName);

//
//通过长文件夹打开文件
//需要栈空间较大 FAT_NAME_MAX *2
//
bool FolderOpenByLfName(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName, uint8_t Len);

//获取指定文件夹的长文件名。
//长文件名最长为66个字节，所以LongFileName[]数组至少要有66个字节，否则将会导致不可预料的错误。
//获取到长文件名，返回TRUE。
//无长文件名，返回FALSE。
bool FolderGetLongName(FOLDER* Folder, uint8_t* LongFileName, uint8_t GetMaxLength);	//LongFileName[]: 66 Bytes

//新建文件夹。
//在ParentFolder文件夹中新建一个名称为FolderName[]的文件夹。
//ParentFolder父文件夹指针不能为NULL。
//返回TRUE：创建成功，新创建的文件夹信息填充到Folder所指结构体中。
//返回FALSE：创建失败。
bool FolderCreate(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName);

//按长文件夹名称在指定文件夹中新建一个文件夹。
//目标文件夹信息填充到Folder所指的结构体。
//FolderLongName[64]为长文件夹名,Unicode 16编码，长度为128Byte 空闲位置为0xFF
//FolderShortName[11]为短文件夹名,Utf-8编码，长度为11Byte 空闲位置为空格"0x20"，最后三位必为0x20
//ParentFolder 不能为空
//Len 为长文件夹名实际长度，单位为uint16_t
bool FolderCreateByLongName(FOLDER* Folder, FOLDER* ParentFolder, uint16_t* FolderLongName, uint8_t* FolderShortName, uint8_t Len);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
