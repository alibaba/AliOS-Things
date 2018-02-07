/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : demuxComponent.h
* Description : stream control and video stream demux
* History :
*   Author  : AL3
*   Date    : 2015/05/05
*   Comment : first version
*
*/

#ifndef DEMUX_COMPONENT_H
#define DEMUX_COMPONENT_H

#if 0
#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <utils/RefBase.h>
#endif

#include "player.h"      //* player library in "android/hardware/aw/"
#include "mediaInfo.h"
#include <CdxEnumCommon.h>

typedef void* DemuxComp;

#define SOURCE_TYPE_URL           0x1
#define SOURCE_TYPE_FD            0x2
#define SOURCE_TYPE_ISTREAMSOURCE 0x3

enum EDEMUXNOTIFY  //* player internal notify.
{
    DEMUX_NOTIFY_PREPARED       = DEMUX_NOTIFY_VALID_RANGE_MIN,
    DEMUX_NOTIFY_EOS,
    DEMUX_NOTIFY_IOERROR,
    DEMUX_NOTIFY_RESET_PLAYER,
    DEMUX_NOTIFY_SEEK_FINISH,
    DEMUX_NOTIFY_CACHE_STAT,
    DEMUX_NOTIFY_BUFFER_START,
    DEMUX_NOTIFY_BUFFER_END,
    DEMUX_NOTIFY_PAUSE_PLAYER,
    DEMUX_NOTIFY_RESUME_PLAYER,

    DEMUX_IOREQ_ACCESS,
    DEMUX_IOREQ_OPEN,
    DEMUX_IOREQ_OPENDIR,
    DEMUX_IOREQ_READDIR,
    DEMUX_IOREQ_CLOSEDIR,
    DEMUX_VIDEO_STREAM_CHANGE,
    DEMUX_AUDIO_STREAM_CHANGE,

    //for cmcc
    DEMUX_NOTIFY_HLS_DOWNLOAD_START,
    DEMUX_NOTIFY_HLS_DOWNLOAD_ERROR,
    DEMUX_NOTIFY_HLS_DOWNLOAD_END,

    //* CMCC 2.1.7.12-m3 log
    DEMUX_NOTIFY_LOG_RECORDER,

    // for timeShift end
    DEMUX_NOTIFY_TIMESHIFT_DURATION,
    DEMUX_NOTIFY_METADATA,
    DEMUX_NOTIFY_MAX,
};
CHECK_DEMUX_NOTIFY_MAX_VALID(DEMUX_NOTIFY_MAX)

enum EDEMUXERROR
{
    DEMUX_ERROR_NONE        = 0,
    DEMUX_ERROR_UNKNOWN     = -1,
    DEMUX_ERROR_IO          = -2,
    DEMUX_ERROR_USER_CANCEL = -3,
};

enum ECACHEPOLICY
{
    CACHE_POLICY_ADAPTIVE             = 0,
    CACHE_POLICY_SMOOTH               = 2,
};

typedef int (*DemuxCallback)(void* pUserData, int eMessageId, void* param);

DemuxComp* DemuxCompCreate(void);

void DemuxCompDestroy(DemuxComp* d);

void DemuxCompClear(DemuxComp* d);  //* clear the data source, like just created.

int DemuxCompSetUrlSource(DemuxComp* d, void* pHTTPServer,
                        const char* pUrl, const CdxKeyedVectorT* pHeaders);

int DemuxCompSetFdSource(DemuxComp* d, int fd, int64_t nOffset, int64_t nLength);

int DemuxCompSetStreamSource(DemuxComp* d, const char* pUri);

int DemuxCompSetPlayer(DemuxComp* d, Player* player);

int DemuxCompSetHdcpOps(DemuxComp* d, struct HdcpOpsS* pHdcp);

int DemuxCompSetCallback(DemuxComp* d, DemuxCallback callback, void* pUserData);

int DemuxCompPrepareAsync(DemuxComp* d);

int DemuxCompCancelPrepare(DemuxComp* d);   //* should call back DEMUX_PREPARE_FINISH message.

#if VIDEO_SUPPORT
int DemuxProbeH265RefPictureNumber(char* pDataBuf, int nDataLen);
#endif

MediaInfo* DemuxCompGetMediaInfo(DemuxComp* d);

int DemuxCompStart(DemuxComp* d);

int DemuxCompStop(DemuxComp* d);    //* close the data source, must call prepare again to restart.

int DemuxCompPause(DemuxComp* d);   //* no pause status in demux component, return OK immediately.

int DemuxCompGetStatus(DemuxComp* d);

int DemuxCompSeekTo(DemuxComp* d, int nSeekTimeMs);

int DemuxCompCancelSeek(DemuxComp* d);  //* should not call back DEMUX_SEEK_FINISH message.

int DemuxCompNotifyFirstFrameShowed(DemuxComp* d);   //* notify video first frame showed.

#if ONLINE_SUPPORT
int DemuxCompSetCacheStatReportInterval(DemuxComp* d, int ms);

int DemuxCompSetCachePolicy(DemuxComp*          d,
                            enum ECACHEPOLICY   eCachePolicy,
                            int                 nStartPlaySize,
                            int                 nStartPlayTimeMs,
                            int                 nMaxBufferSize);
#endif

#if SECURE_BUFFER_SUPPORT
int DemuxCompSetSecureBufferCount(DemuxComp* d, void* param);

int DemuxCompSetSecureBuffers(DemuxComp* d,void* param);
#endif

int DemuxCompSeekToResetUrl(DemuxComp* d, int nSeekTimeMs);

int DemuxCompGetLiveMode(DemuxComp* d);

int DemuxCompSetLiveMode(DemuxComp* d, int liveMode);

#if ONLINE_SUPPORT
int DemuxCompGetCacheSize(DemuxComp* d);
#endif

int DemuxCompSetHdcpOps(DemuxComp* d, struct HdcpOpsS* pHdcpOps);

#endif

