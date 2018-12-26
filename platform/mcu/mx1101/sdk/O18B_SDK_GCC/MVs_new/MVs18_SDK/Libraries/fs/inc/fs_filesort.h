/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:filesort.h
//maintainer lilu
/////////////////////////////////////////////////////////////////////////

#ifndef __FS_FILESORT_H__
#define __FS_FILESORT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fs_config.h"

#if FS_FILE_SORT_EN == 1
//
//预搜索过程中找到一个新的歌曲文件，将其文件名拷贝到缓冲区
//由PreSearch()函数调用
void FileSortSearchNewFile(FS_CONTEXT* FsContext);

//预搜索结束，完成文件名排序
//然后将排序后的列表压缩后存放在FileSortTable[]数组中
//由PreSearch()函数调用
void FileSortSearchEnd(FS_CONTEXT* FsContext);

//该函数在FileOpenByNum()函数之前调用
//计算排序后的第FileSortNum个文件所对应的原始全盘文件号
//忽略原始文件号不在[StartFileNum + 1, EndFileNum]范围内的文件
//1.计算全盘上排序后的第m个文件的原始全盘文件号：
//		GetFileNumInDisk(FsContext, m, 0, 0xFFFF)，得到全盘排序后第m个文件对应的原始全盘文件号
//2.计算文件夹内排序后的第m个文件的原始全盘文件号：
//		GetFileNumInDisk(FsContext, m, Folder->StartFileNum, Folder->StartFileNum + Folder->IncFileCnt)
uint16_t GetFileNumInDisk(FS_CONTEXT* FsContext, uint16_t FileSortNum, uint16_t StartFileNum, uint16_t EndFileNum);

#endif //FS_FILE_SORT_EN

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__FS_FILESORT_H__
