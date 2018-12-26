/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : player.h
 * Description : player
 * History :
 *
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "cdx_log.h"
//#include "cdx_config.h"

#if VIDEO_SUPPORT
#include "vdecoder.h"
#include "layerControl.h"
#include "deinterlace.h"
#endif
#include "adecoder.h"
#include "soundControl.h"
#if SUBTITLE_SUPPORT
#include "sdecoder.h"
#include "subtitleControl.h"
#endif


#include <CdxEnumCommon.h>

enum EPLAYERNOTIFY {
    PLAYBACK_NOTIFY_EOS = PLAYBACK_NOTIFY_VALID_RANGE_MIN,

    //* param == NULL;
    PLAYBACK_NOTIFY_FIRST_PICTURE,

    //* width = ((int*)param)[0]; height = ((int*)param)[1];
    PLAYBACK_NOTIFY_VIDEO_SIZE,
    //* leftOffset = ((int*)param)[0; topOffset = ((int*)param)[1];
    //* cropWidth = ((int*)param)[2]; cropHeight = ((int*)param)[3];
    PLAYBACK_NOTIFY_VIDEO_CROP,

    //* subtitle_id   = ((unsigned int*)param)[0];
    //* pSubtitleItem = (SubtitleItem*)((unsigned int*)param)[1];
    PLAYBACK_NOTIFY_SUBTITLE_ITEM_AVAILABLE,
    //* subtitle_id = (unsigned int)param;
    PLAYBACK_NOTIFY_SUBTITLE_ITEM_EXPIRED,

    //* video stream not supported, video decoder crash.
    PLAYBACK_NOTIFY_VIDEO_UNSUPPORTED,
    //* audio stream not supported, audio decoder crash.
    PLAYBACK_NOTIFY_AUDIO_UNSUPPORTED,
    //* subtitle stream not supported, subtitle decoder crash.
    PLAYBACK_NOTIFY_SUBTITLE_UNSUPPORTED,
    PLAYBACK_NOTIFY_AUDIORAWPLAY,

    PLAYBACK_NOTIFY_SET_SECURE_BUFFER_COUNT,
    PLAYBACK_NOTIFY_SET_SECURE_BUFFERS,

    PLAYBACK_NOTIFY_AUDIO_INFO,

    PLAYBACK_NOTIFY_VIDEO_RENDER_FRAME,

    PLAYBACK_NOTIFY_MAX,
};
CHECK_PLAYBACK_NOTIFY_MAX_VALID(PLAYBACK_NOTIFY_MAX)

enum EPLAYERSTATUS
{
    PLAYER_STATUS_STOPPED = 0,
    PLAYER_STATUS_STARTED,
    PLAYER_STATUS_PAUSED
};

enum EMEDIATYPE
{
    MEDIA_TYPE_VIDEO = 0,
    MEDIA_TYPE_AUDIO,
    MEDIA_TYPE_SUBTITLE,
    MEDIA_TYPE_METADATA
};

enum EPICTURE3DMODE
{
    PICTURE_3D_MODE_NONE = 0,
    PICTURE_3D_MODE_TWO_SEPERATED_PICTURE,
    PICTURE_3D_MODE_SIDE_BY_SIDE,
    PICTURE_3D_MODE_TOP_TO_BOTTOM,
    PICTURE_3D_MODE_LINE_INTERLEAVE,
    PICTURE_3D_MODE_COLUME_INTERLEAVE
};

enum EDISPLAY3DMODE
{
    DISPLAY_3D_MODE_2D = 0,
    DISPLAY_3D_MODE_3D,
    DISPLAY_3D_MODE_HALF_PICTURE
};

enum EDISPLAYRATIO
{
    DISPLAY_RATIO_FULL_SCREEN,
    DISPLAY_RATIO_LETTERBOX,
    //* add new mode.
};

typedef struct MEDIASTREAMDATAINFO
{
    char*   pData;
    int     nLength;
    int64_t nPts;
    int64_t nPcr;
    int     bIsFirstPart;
    int     bIsLastPart;
    int64_t nDuration;      //* in unit of us.
    int     nStreamChangeFlag;
    int     nStreamChangeNum;
    void   *pStreamInfo;
}MediaStreamDataInfo;

typedef int (*PlayerCallback)(void* pUserData, int eMessageId, void* param);

typedef void* Player;


#ifdef __cplusplus
extern "C" {
#endif

Player* PlayerCreate(void);

void PlayerDestroy(Player* pl);

int PlayerSetCallback(Player* pl, PlayerCallback callback, void* pUserData);


//*******************************  START  **********************************//
//** Play Control APIs.
//**

int PlayerStart(Player* pl);

int PlayerStop(Player* pl);      //* media stream information is still kept by the player.

int PlayerPause(Player* pl);

//* for seek operation, mute be called under paused status.
int PlayerReset(Player* pl, int64_t nSeekTimeUs);

//* must be called under stopped status, all stream information cleared.
int PlayerClear(Player* pl);

#if VIDEO_SUPPORT
//* set player playback in fast mode.
//* in fast mode, video is showed directly without any synchronization, and
//* audio is should not send in.
int PlayerFast(Player* pl, int bDecodeKeyframeOnly);

//* return from fast mode, the player is set to started status when return.
int PlayerStopFast(Player* pl);
#endif

enum EPLAYERSTATUS PlayerGetStatus(Player* pl);

int64_t PlayerGetPosition(Player* pl);    //* current time positon in us.

int64_t PlayerGetPositionCMCC(Player* pl);    //* current time positon in us.

int64_t PlayerGetPts(Player* pl);       //* current presentation time stamp.

int PlayerSetDiscardAudio(Player* pl, int f);  // for IPTV, discard audi in fast mode

//********************************  END  ***********************************//

//*******************************  START  **********************************//
//** Streaming Control APIs.
//**

int PlayerRequestStreamBuffer(Player*       pl,
                              int             nRequireSize,
                              void**          ppBuf,
                              int*            pBufSize,
                              void**          ppRingBuf,
                              int*            pRingBufSize,
                              enum EMEDIATYPE eMediaType,
                              int             nStreamIndex);

int PlayerSubmitStreamData(Player*            pl,
                           MediaStreamDataInfo* pDataInfo,
                           enum EMEDIATYPE      eMediaType,
                           int                  nStreamIndex);

int PlayerSetEos(Player* pl);

int PlayerSetFirstPts(Player* pl, int64_t nFirstPts);

#if VIDEO_SUPPORT
//* estimated by pts and stream data size.
int PlayerGetVideoBitrate(Player* pl);

//* estimated by pts.
int PlayerGetVideoFrameRate(Player* pl);

//* estimated by pts.
int PlayerGetVideoFrameDuration(Player* pl);

//* how much video stream data in stream buffere.
int PlayerGetVideoStreamDataSize(Player* pl);

//* the size of video stream buffer
int PlayerGetVideoStreamBufferSize(Player* pl);

//* how many stream frame in buffer.
int PlayerGetVideoStreamFrameNum(Player* pl);

//* how many picture has been decoded and waiting to show.
int PlayerGetValidPictureNum(Player* pl);
#endif

//* estimated by pts and stream data size.
int PlayerGetAudioBitrate(Player* pl);

//* get audio sample rate, channel count and how many bits per sample of pcm data.
int PlayerGetAudioParam(Player* pl, int* pSampleRate, int* pChannelCount, int* pBitsPerSample);

//* how much audio stream data in stream buffer.
int PlayerGetAudioStreamDataSize(Player* pl);

//* how many stream frame in buffer.
int PlayerGetAudioStreamFrameNum(Player* pl);

//* how much audio pcm data has been decoded to the pcm buffer.
int PlayerGetAudioPcmDataSize(Player* pl);

//* how much audio pcm data cached in audio device.
int PlayerGetAudioCacheTimeInSoundDevice(Player* pl);

//********************************  END  ***********************************//

//*******************************  START  **********************************//
//** Video APIs.
//**
#if VIDEO_SUPPORT
int PlayerSetVideoStreamInfo(Player* pl, VideoStreamInfo* pStreamInfo);

int PlayerCanSupportVideoStream(Player* pl, VideoStreamInfo* pStreamInfo);

int PlayerHasVideo(Player* pl);

int PlayerConfigVideoScaleDownRatio(Player* pl, int nHorizonRatio, int nVerticalRatio);

int PlayerConfigVideoRotateDegree(Player* pl, int nDegree);

int PlayerConfigVideoDeinterlace(Player* pl, int bOpenDeinterlace);
int PlayerConfigDispErrorFrame(Player* pl, int bDispErrorFrame);
int PlayerConfigDropDelayFrame(Player* pl, int bDropDelayFrame);
int PlayerConfigTvStreamFlag(Player* pl, int bFlag);
#endif
//********************************  END  ***********************************//

//*******************************  START  **********************************//
//** Audio APIs.
//**

int PlayerSetAudioStreamInfo(Player* pl, AudioStreamInfo* pStreamInfo,
        int nStreamNum, int nDefaultStream);

int PlayerAddAudioStream(Player* pl, AudioStreamInfo* pStreamInfo);

int PlayerCanSupportAudioStream(Player* pl, AudioStreamInfo* pStreamInfo);

int PlayerGetAudioStreamCnt(Player* pl);

int PlayerGetAudioStreamInfo(Player* pl, int* pStreamNum, AudioStreamInfo** ppStreamInfo);

int PlayerHasAudio(Player* pl);

int PlayerSwitchAudio(Player* pl, int nStreamIndex);

//hkw switch audio track for IPTV
int PlayerStopAudio(Player* pl);

int PlayerStartAudio(Player* pl);

//********************************  END  ***********************************//

//*******************************  START  **********************************//
//** Subtitle APIs.
//**
#if SUBTITLE_SUPPORT
int PlayerSetSubtitleStreamInfo(Player* pl, SubtitleStreamInfo* pStreamInfo,
        int nStreamNum, int nDefaultStream);

int PlayerAddSubtitleStream(Player* pl, SubtitleStreamInfo* pStreamInfo);

int PlayerCanSupportSubtitleStream(Player* pl, SubtitleStreamInfo* pStreamInfo);

int PlayerGetSubtitleStreamCnt(Player* pl);

/* stream info array is allocated inside, user need to free the memory by
 * calling free(*ppStreamInfo);
 */
int PlayerGetSubtitleStreamInfo(Player* pl, int* pStreamNum, SubtitleStreamInfo** ppStreamInfo);

/* stream info array is allocated inside, user need to free the memory by
 * calling free(*ppStreamInfo);
 */
int PlayerProbeSubtitleStreamInfo(const char*          strFileName,
                                  int*                 pStreamNum,
                                  SubtitleStreamInfo** ppStreamInfo);

/* stream info array is allocated inside, user need to free the memory by
 * calling free(*ppStreamInfo);
 */
int PlayerProbeSubtitleStreamInfoFd(int fd, int offset, int len, int* pStreamNum,
        SubtitleStreamInfo** ppStreamInfo);

int PlayerSwitchSubtitle(Player* pl, int nStreamIndex);

//* adjust subtitle show time, in
int PlayerSetSubtitleShowTimeAdjustment(Player* pl, int nTimeMs);

//* get the adjustment of subtitle show time, in unit of ms.
int PlayerGetSubtitleShowTimeAdjustment(Player* pl);

//* set subRender
int PlayerSetSubCtrl(Player* pl, SubCtrl* pSubCtrl);
#endif
//********************************  END  ***********************************//

//*******************************  START  **********************************//
//** Display Control APIs.
//**
#if VIDEO_SUPPORT
int PlayerSetWindow(Player* pl, LayerCtrl* pLc);

//* this API is not used in new display
int PlayerSet3DMode(Player* pl, enum EPICTURE3DMODE ePicture3DMode,
        enum EDISPLAY3DMODE eDisplay3DMode);

int PlayerGet3DMode(Player* pl, enum EPICTURE3DMODE* ePicture3DMode,
        enum EDISPLAY3DMODE* eDisplay3DMode);

//* some times you need to know the picture size from the decoder under player.
int PlayerGetPictureSize(Player* pl, int* pWidth, int* pHeight);

//* Get the video crop window.
int PlayerGetPictureCrop(Player* pl, int* pLeftOff, int* pTopOff,
        int* pCropWidth, int* pCropHeight);

//* Hide video layer.
int PlayerVideoHide(Player* pl);

//* show video layer.
int PlayerVideoShow(Player* pl);

//* set whether keep last picture when player stopped.
//* the last picture is hold in default.
int PlayerSetHoldLastPicture(Player* pl, int bHold);

//* set video window.
int PlayerSetVideoRegion(Player* pl, int x, int y, int nWidth, int nHeight);

//* set picture display ratio, full stream, letter box or other modes.
int PlayerSetDisplayRatio(Player* pl, enum EDISPLAYRATIO eDisplayRatio);

//* set deinterlace
int PlayerSetDeinterlace(Player* pl, Deinterlace* pDi);
#endif
//********************************  END  ***********************************//

//*******************************  START  **********************************//
//** Audio Output Control APIs.
//**
int PlayerSetAudioSink(Player* pl, SoundCtrl* pAudioSink);

//* get audio balance, return 1 means left channel, 2 means right channel, 3 means stereo.
int PlayerGetAudioBalance(Player* pl);

//* set audio balance, 1 means left channel, 2 means right channel, 3 means stereo.
int PlayerSetAudioBalance(Player* pl, int nAudioBalance);

//* mute the audio, bMute = 1 means mute the audio, 0 means resume from muted.
int PlayerSetAudioMute(Player* pl, int bMute);

/* get the audio mute setting, 1 means audio muted, 0 means not muted, -1 means
 * there is no audio stream.
 */
int PlayerGetAudioMuteFlag(Player* pl);

int PlayerSetAudioForceWriteToDeviceFlag(Player* pl, int bForceFlag);

int PlayerConfigExtraScaleInfo(Player* pl, int nWidthTh, int nHeightTh,
        int nHorizontalScaleRatio, int nVerticalScaleRatio);

int PlayerSetPlayBackSettings(Player* pl,const struct XAudioPlaybackRate* rate);

int PlayerGetPlayBackSettings(Player* pl,XAudioPlaybackRate* rate);

//********************************  END  ***********************************//

#ifdef __cplusplus
}
#endif

#endif
