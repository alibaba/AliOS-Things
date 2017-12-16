///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: browser.h
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////

#ifndef __BROWSER_H__
#define __BROWSER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//文件浏览初始化
bool FileBrowseInit(void);

//文件浏览事件处理
bool FileBrowseOp(uint16_t Msg);

//文件浏览获取选择的结果	
bool FileBrowseGetResult(FOLDER** Folder, uint16_t* FileNumInFolder, uint16_t* FileNumInDisk);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif




