///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_decoder.h
///////////////////////////////////////////////////////////////////////////////
/*
zhouyi,2012.07.11, initial version
maintainer: Aissen
*/
#ifndef __TASK_DECODER_H__
#define __TASK_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "audio_decoder.h"
#include "folder.h"

//开始播放
//FileHandle: 文件指针
//FileType:   IO_TYPE_FILE--播放U盘/SD卡中的文件，IO_TYPE_MEMORY--播放提示音
//StartTime:  播放的开始时间 
bool DecoderTaskPlayStart(void* FileHandle, uint8_t FileType, uint32_t StartTime);
void DecoderTaskStop(void);				//停止播放
void DecoderTaskPause(void);			//暂停
void DecoderTaskResume(void);			//由暂停恢复播放
void DecoderTaskFF(uint32_t StepTime);	//快进
void DecoderTaskFB(uint32_t StepTime);	//快退
void DecoderTaskSeek(uint32_t Time);	//指定时间播放
bool DecoderTaskIsSongEnd(void);		//判断一首歌是否播放结束
bool DecoderTaskIsSongBegin(void);		//判断一首歌是否快退到歌曲开头
bool DecoderTaskIsInitializeOk(void);	//判断一首歌初始化是否成功
uint32_t DecoderTaskGetPlayTime(void);	//获取当前播放时间

//阻塞播放提示音，播放完恢复原来的状态
bool DecoderTaskSyncPlay(void* FileHandle, uint8_t FileType);
extern void DecoderTaskEntrance(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__TASK_DECODER_H__*/
