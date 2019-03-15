/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:fs_config.h
//maintainer lilu
/////////////////////////////////////////////////////////////////////////
#ifndef __FS_CONFIG_H__
#define __FS_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include	"type.h"

///////////////////////////////////////以下为只读，不可配置部分
#define FAT_USE_LFN		1       //read only

#define FAT_NAME_MAX	128	    //read only

#define MAX_FS_CONTEXT_COUNT 1  //支持同时mount的分区数  read only
///////////////////////////////////////只读，不可配置部分


///////////////////////////////////////以下为可配置部分

#define FS_FILE_SORT_EN    0          //是否支持排序 
#define FS_MAX_FILE_SORT_NUM	1024  //支持的排序文件个数，最大个数 2048   

///////////////////////////////////////可配置部分

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FS_CONFIG_H_
