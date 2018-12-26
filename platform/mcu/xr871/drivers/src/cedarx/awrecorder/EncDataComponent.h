/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : EncDataComponent.h
 * Description : EncDataComponent
 * History :
 *
 */

#ifndef __MUX_COMP_H__
#define __MUX_COMP_H__

#include "CdxMuxer.h"
#include "AudioEncodeComponent.h"
//#include "VideoEncodeComponent.h"

enum MUXERNOTIFY
{
    ENCDATA_NOTIFY_ERROR,
    ENCDATA_NOTIFY_ENCODE_PACKET,
};

typedef void* EncDataComp;
typedef int (*MuxerCallback)(void* pUserData, int eMessageId, void* param);

EncDataComp* EncDataCompCreate(void * app);
int EncDataCompInit(EncDataComp *v,VideoEncodeConfig *videoConfig,
                            AudioEncodeConfig *audioConfig, EncDataCallBackOps *ops);
void EncDataCompDestory(EncDataComp* p);
int EncDataCompStart(EncDataComp *p);
int EncDataCompStop(EncDataComp *p);
int EncDataCompReset(EncDataComp *p);
int EncDataCompSetAudioEncodeComp(EncDataComp *p, AudioEncodeComp* pAudioEncoder);
#if VIDEO_SUPPORT
int EncDataCompSetVideoEncodeComp(EncDataComp *p, VideoEncodeComp* pVideoEncoder);
#endif
int EncDataCompSetCallback(EncDataComp* v, MuxerCallback callback, void* pUserData);

#endif /* __VIDEO_RESIZER_MUXER_H__ */

