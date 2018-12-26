/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : audioDecComponent.h
 * Description : audio decoder component
 * History :
 *
 */

#ifndef AUDIO_DECODE_COMPONENT
#define AUDIO_DECODE_COMPONENT

#include "player_i.h"
#include "avtimer.h"
#include "adecoder.h"

typedef struct AUDIOSTREAMDATAINFO
{
    char*   pData;
    int     nLength;
    int64_t nPts;
    int64_t nPcr;
    int     bIsFirstPart;
    int     bIsLastPart;
}AudioStreamDataInfo;

typedef struct AudioDecComp AudioDecComp;

AudioDecComp* AudioDecCompCreate(void);

int AudioDecCompDestroy(AudioDecComp* a);

int AudioDecCompStart(AudioDecComp* a);

int AudioDecCompStop(AudioDecComp* a);

int AudioDecCompPause(AudioDecComp* a);

enum EPLAYERSTATUS AudioDecCompGetStatus(AudioDecComp* a);

int AudioDecCompReset(AudioDecComp* a, int64_t nSeekTime);

int AudioDecCompSetEOS(AudioDecComp* a);

int AudioDecCompSetCallback(AudioDecComp* a, PlayerCallback callback, void* pUserData);

int AudioDecCompSetAudioStreamInfo(AudioDecComp*    a,
                                   AudioStreamInfo* pStreamInfo,
                                   int              nStreamCount,
                                   int              nDefaultStreamIndex);

int AudioDecCompAddAudioStream(AudioDecComp* a, AudioStreamInfo* pStreamInfo);

int AudioDecCompGetAudioStreamCnt(AudioDecComp* a);

int AudioDecCompCurrentStreamIndex(AudioDecComp* a);

int AudioDecCompGetAudioStreamInfo(AudioDecComp* a, int* pStreamNum,
        AudioStreamInfo** ppStreamInfo);

int AudioDecCompGetAudioSampleRate(AudioDecComp* a, unsigned int* pSampleRate,
        unsigned int* pChannelNum, unsigned int* pBitRate);

int AudioDecCompSetTimer(AudioDecComp* a, AvTimer* timer);

int AudioDecCompRequestStreamBuffer(AudioDecComp* a,
                                    int           nRequireSize,
                                    char**        ppBuf,
                                    int*          pBufSize,
                                    char**        ppRingBuf,
                                    int*          pRingBufSize,
                                    int           nStreamIndex);

int AudioDecCompSubmitStreamData(AudioDecComp*        a,
                                 AudioStreamDataInfo* pDataInfo,
                                 int                  nStreamIndex);

int AudioDecCompStreamBufferSize(AudioDecComp* a, int nStreamIndex);

int AudioDecCompStreamDataSize(AudioDecComp* a, int nStreamIndex);

int AudioDecCompStreamFrameNum(AudioDecComp* a, int nStreamIndex);

int AudioDecCompRequestPcmData(AudioDecComp*   a,
                               unsigned char** ppData,
                               unsigned int*   pSize,
                               int64_t*        pPts,
                               CdxPlaybkCfg*   cfg);

int AudioDecCompReleasePcmData(AudioDecComp* a, int nReleaseSize);

int AudioDecCompPcmDataSize(AudioDecComp* a, int nStreamIndex);

int AudioDecCompSwitchStream(AudioDecComp* a, int nStreamIndex);

void AudioDecRawSendCmdToHalClbk(void *pself,void *param);

#endif
