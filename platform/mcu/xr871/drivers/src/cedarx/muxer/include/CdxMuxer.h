/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMuxer.h
 * Description : Allwinner Muxer Definition
 * History :
 *
 */

#ifndef CDX_MUXER_H
#define CDX_MUXER_H

#include <stdlib.h>
#include <stdint.h>
//#include <vencoder.h>
#include <aencoder.h>
#include <cdx_log.h>
#include <CdxWriter.h>
#include <CdxTypes.h>

typedef enum {
    UNKNOWN_CMD = 0,
    SET_CACHE_MEM,
    SET_FS_WRITE_MODE,
    SET_FS_SIMPLE_CACHE_SIZE,
    SET_MP4_TMP_PATH,
    SET_PLAY_TIME_LENGTH,
    SET_IS_SDCARD_DISAPPEAR,
    GET_IS_SDCARD_DISAPPEAR
} MuxCtrlCommads;

typedef enum tag_FSWRITEMODE {
    FSWRITEMODE_CACHETHREAD = 0,
    FSWRITEMODE_SIMPLECACHE,
    FSWRITEMODE_DIRECT,
}FSWRITEMODE;

typedef struct CdxFsCacheMemInfo
{
    cdx_uint8              *mp_cache;
    cdx_uint32            m_cache_size;
}CdxFsCacheMemInfo;

enum CdxMuxerTypeE
{
    CDX_MUXER_UNKNOW = -1,
    CDX_MUXER_MOV,
    CDX_MUXER_TS,
    CDX_MUXER_AVI,
    CDX_MUXER_AAC,
    CDX_MUXER_MP3,
    CDX_MUXER_MOV_EX,
    CDX_MUXER_AMR, //add
};

#define FS_WRITER (0)

typedef struct CdxMuxerCreatorS CdxMuxerCreatorT;
typedef struct CdxMuxerS CdxMuxerT;
typedef enum CdxMuxerTypeE CdxMuxerTypeT;
typedef struct CdxMuxerMediaInfoS CdxMuxerMediaInfoT;
typedef struct MuxerVideoStreamInfoS MuxerVideoStreamInfoT;
typedef struct MuxerAudioStreamInfoS MuxerAudioStreamInfoT;
typedef struct CdxMuxerPacketS CdxMuxerPacketT;

struct CdxMuxerCreatorS
{
    CdxMuxerT *(*create)(CdxWriterT* /* stream */);
};

struct CdxMuxerPacketS
{
    void *buf;
    cdx_int32 buflen;
    cdx_int64 pts;
    cdx_int64 duration;
    cdx_int32 type;
    cdx_int32 length;
    cdx_int32 streamIndex;
};

struct CdxMuxerOpsS
{
    cdx_int32 (*writeExtraData)(CdxMuxerT *, unsigned char* /* extradata */,
                                int /* extradataLen */, int idx);
    cdx_int32 (*writeHeader)(CdxMuxerT *);
    cdx_int32 (*writePacket)(CdxMuxerT *, CdxMuxerPacketT * /* pkt */);
    cdx_int32 (*writeTrailer)(CdxMuxerT *);
    cdx_int32 (*control)(CdxMuxerT *, int /* uCmd */, void * /* pParam*/);
    cdx_int32 (*close)(CdxMuxerT *);
    cdx_int32 (*setMediaInfo)(CdxMuxerT *, CdxMuxerMediaInfoT *);
};

struct CdxMuxerS
{
    enum CdxMuxerTypeE type;
    struct CdxMuxerOpsS *ops;
};

#if VIDEO_SUPPORT
struct MuxerVideoStreamInfoS
{
    VENC_CODEC_TYPE    eCodeType;
    int                nWidth;
    int                nHeight;
    int                nFrameRate;
    int                nCreatTime;
    int                nRotateDegree;
};
#endif

#define ADECODER_MAX_LANG_CHAR_SIZE  (64)
struct MuxerAudioStreamInfoS
{
    AUDIO_ENCODER_TYPE      eCodecFormat;
    int                     nChannelNum;
    int                     nBitsPerSample;
    int                     nSampleCntPerFrame;
    int                     nSampleRate;
    int                     nAvgBitrate;
    int                     nMaxBitRate;
    unsigned char           strLang[ADECODER_MAX_LANG_CHAR_SIZE];
};

#if VIDEO_SUPPORT
#define MAX_VIDEO_NUM 2
#endif
#define MAX_AUDIO_NUM 2
struct CdxMuxerMediaInfoS
{
    int                    audioNum;
#if VIDEO_SUPPORT
    int                    videoNum;
    MuxerVideoStreamInfoT  video;
#endif
    MuxerAudioStreamInfoT  audio;
    int                    geo_available;
    int                    latitudex;
    int                    longitudex;
    FSWRITEMODE            writer_mode;
};

#if FS_WRITER
#define ByteIOContext CdxFsWriter
#else
#define ByteIOContext CdxWriterT
#endif

#ifdef __cplusplus
extern "C"
{
#endif

CdxMuxerT *CdxMuxerCreate(CdxMuxerTypeT type, CdxWriterT *stream);

static inline int CdxMuxerSetMediaInfo(CdxMuxerT *mux, CdxMuxerMediaInfoT *mediaInfo)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->setMediaInfo);
    return mux->ops->setMediaInfo(mux, mediaInfo);
}

static inline int CdxMuxerWriteExtraData(CdxMuxerT *mux, unsigned char* pdata,
                                         int data_len, int index)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->writeHeader);
    return mux->ops->writeExtraData(mux, pdata, data_len, index);
}

static inline int CdxMuxerWriteHeader(CdxMuxerT *mux)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->writeHeader);
    return mux->ops->writeHeader(mux);
}

static inline int CdxMuxerWritePacket(CdxMuxerT *mux, CdxMuxerPacketT *pkt)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->writePacket);
    return mux->ops->writePacket(mux, pkt);
}

static inline int CdxMuxerWriteTrailer(CdxMuxerT *mux)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->writeTrailer);
    return mux->ops->writeTrailer(mux);
}

static inline int CdxMuxerControl(CdxMuxerT *mux, int uCmd, void * pParam)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->control);
    return mux->ops->control(mux, uCmd, pParam);
}

static inline int CdxMuxerClose(CdxMuxerT *mux)
{
    CDX_CHECK(mux);
    CDX_CHECK(mux->ops);
    CDX_CHECK(mux->ops->close);
    return mux->ops->close(mux);
}

#ifdef __cplusplus
}
#endif
#endif
