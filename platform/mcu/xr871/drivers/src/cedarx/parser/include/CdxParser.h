/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxParser.h
 * Description : Parser
 * History :
 *
 */

#ifndef CDX_PARSER_H
#define CDX_PARSER_H

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <CdxTypes.h>
#include <cdx_log.h>
#include <CdxStream.h>
//#include <vdecoder.h>
#include <adecoder/adecoder.h>
//#include <sdecoder.h>
#include <CdxEnumCommon.h>

#ifdef __ANDROID__
#include <cutils/properties.h>
#endif

enum CdxParserTypeE
{
    CDX_PARSER_UNKNOW = -1,
    CDX_PARSER_MOV,
    CDX_PARSER_MKV,
    CDX_PARSER_ASF,
    CDX_PARSER_TS,
    CDX_PARSER_AVI,
    CDX_PARSER_FLV,
    CDX_PARSER_PMP,

    CDX_PARSER_HLS,
    CDX_PARSER_DASH,
    CDX_PARSER_MMS,
    CDX_PARSER_BD,
    CDX_PARSER_OGG,
    CDX_PARSER_M3U9,
    CDX_PARSER_RMVB,
    CDX_PARSER_PLAYLIST,
    CDX_PARSER_APE,
    CDX_PARSER_FLAC,
    CDX_PARSER_AMR,
    CDX_PARSER_ATRAC,
    CDX_PARSER_MP3,
    CDX_PARSER_DTS,
    CDX_PARSER_AC3,
    CDX_PARSER_AAC,
    CDX_PARSER_WAV,
    CDX_PARSER_REMUX, /* rtsp, etc... */
    CDX_PARSER_WVM,
    CDX_PARSER_MPG,
    CDX_PARSER_MMSHTTP,
    CDX_PARSER_AWTS,
    CDX_PARSER_SSTR,
    CDX_PARSER_CAF,
    CDX_PARSER_G729,
    CDX_PARSER_DSD,
    CDX_PARSER_AIFF,
    CDX_PARSER_ID3V2,
    CDX_PARSER_ENV,
    CDX_PARSER_SSTR_PLAYREADY,
    CDX_PARSER_AWRAWSTREAM,
    CDX_PARSER_AWSPECIALSTREAM,
};

typedef struct CdxPacketS CdxPacketT;
typedef struct CdxMediaInfoS CdxMediaInfoT;
typedef enum CdxParserTypeE CdxParserTypeT;
typedef struct CdxParserCreatorS CdxParserCreatorT;
typedef struct CdxParserS CdxParserT;
//typedef struct ParserMetaDataS ParserMetaDataT;

#define MINOR_STREAM    0x0001 /*0 major stream, 1 minor stream*/
#define FIRST_PART      0x0002
#define LAST_PART       0x0004
#define KEY_FRAME       0x0008

struct CdxPacketS
{
    void *buf;
    void *ringBuf;
    cdx_int32 buflen;
    cdx_int32 ringBufLen;
    int64_t pts;
    cdx_int64 duration;
    cdx_int32 type;
    cdx_int32 length;
    cdx_uint32 flags; /* MINOR_STREAM, FIRST_PART, LAST_PART, etc... */
    cdx_int32 streamIndex;
    cdx_int64 pcr;
    cdx_int32 infoVersion;
    void *info;//VideoInfo/AudioInfo/SubtitleInfo
};

//#define PARSER_METADATA_ORGINAL_URI     "parser.orginalUri"
//#define PARSER_METADATA_REDIRECT_URI    "parser.redirectUri"
//#define PARSER_METADATA_ACCESSIBLE_URI  "parser.accessibleUri"

struct ParserCacheStateS
{
    cdx_int32 nCacheCapacity;
    cdx_int32 nCacheSize;
    cdx_int32 nBandwidthKbps;
    cdx_int32 nPercentage; /* current_caching_positioin / duration */
};

struct StreamSeekPos
{
    cdx_int64 pos;
    cdx_int64 time;
    cdx_int64 startTime;
};

enum EPARSERNOTIFY  //* notify.
{
    PARSER_NOTIFY_VIDEO_STREAM_CHANGE = PARSER_NOTIFY_VALID_RANGE_MIN,
    PARSER_NOTIFY_AUDIO_STREAM_CHANGE,
    PARSER_NOTIFY_TIMESHIFT_END_INFO,
    PARSER_NOTIFY_META_DATA,
    PARSER_NOTIFY_MAX,
};
CHECK_PARSER_NOTIFY_MAX_VALID(PARSER_NOTIFY_MAX)

enum CdxParserCommandE
{
    CDX_PSR_CMD_SWITCH_AUDIO,
    CDX_PSR_CMD_SWITCH_SUBTITLE,

    CDX_PSR_CMD_DISABLE_AUDIO,
    CDX_PSR_CMD_DISABLE_VIDEO,
    CDX_PSR_CMD_DISABLE_SUBTITLE,

    CDX_PSR_CMD_SET_DURATION,
    CDX_PSR_CMD_REPLACE_STREAM,
    CDX_PSR_CMD_SET_LASTSEGMENT_FLAG,
    CDX_PSR_CMD_CLR_INFO,

    CDX_PSR_CMD_STREAM_SEEK,
    CDX_PSR_CMD_GET_CACHESTATE,

    CDX_PSR_CMD_SET_FORCESTOP,
    CDX_PSR_CMD_CLR_FORCESTOP,
    CDX_PSR_CMD_UPDATE_PARSER,
    CDX_PSR_CMD_SET_HDCP,
    CDX_PSR_CMD_SET_SECURE_BUFFER_COUNT,
    CDX_PSR_CMD_SET_SECURE_BUFFERS,
    CDX_PSR_CMD_GET_STREAM_EXTRADATA,

#if LIVEMODE_VIDEO
    // get the shiftTimeUrl for cmcc livemode1&2 seekTo
    CDX_PSR_CMD_GET_REDIRECT_URL,
#endif

    // for cmcc get the stream url in LogRecorder
    CDX_PSR_CMD_GET_URL,
    // get the statusCode of http
    CDX_PSR_CMD_GET_STREAM_STATUSCODE,
    // for Ali YUNOS get TS info
    CDX_PSR_CMD_GET_TS_M3U_BANDWIDTH,
    CDX_PSR_CMD_GET_TS_SEQ_NUM,
    CDX_PSR_CMD_GET_TS_LENGTH,
    CDX_PSR_CMD_GET_TS_DURATION,
    CDX_PSR_CMD_SET_YUNOS_UUID,

    // for cmcc timeShift set lastSeqNum
    CDX_PSR_CMD_SET_TIMESHIFT_LAST_SEQNUM,

    // parser and stream use the same setCallback cmd,
    // the code below must be the end of  this structure
    CDX_PSR_CMD_SET_CALLBACK = STREAM_CMD_SET_CALLBACK,
};

enum CdxParserStatusE
{
    PSR_OK,
    PSR_INVALID,
    PSR_OPEN_FAIL,
    PSR_IO_ERR,
    PSR_USER_CANCEL,
    PSR_INVALID_OPERATION,
    PSR_UNKNOWN_ERR,
    PSR_EOS,
    /*other error info*/
};

#define AUDIO_STREAM_LIMIT 1
#define VIDEO_STREAM_LIMIT 1
#define SUBTITLE_STREAM_LIMIT 32

#if VIDEO_SUPPORT
struct VideoInfo
{
    cdx_atomic_t ref;
    cdx_int32 videoNum;
    VideoStreamInfo    video[VIDEO_STREAM_LIMIT];
};
#endif

struct AudioInfo
{
    cdx_atomic_t ref;
    cdx_int32 audioNum;
    AudioStreamInfo    audio[AUDIO_STREAM_LIMIT];
};

#if SUBTITLE_SUPPORT
struct SubtitleInfo
{
    cdx_atomic_t ref;
    cdx_int32 subtitleNum;
    SubtitleStreamInfo subtitle[SUBTITLE_STREAM_LIMIT];
};
#endif

struct CdxProgramS
{
    cdx_int32 id;
    cdx_uint32 flags;
    cdx_uint32 duration; //unit: ms

    cdx_int32 audioNum, audioIndex;
    AudioStreamInfo    audio[AUDIO_STREAM_LIMIT];
    cdx_uint64 audioIndexMask;

#if VIDEO_SUPPORT
    cdx_int32 videoNum, videoIndex;
    VideoStreamInfo    video[VIDEO_STREAM_LIMIT];
    cdx_uint64 videoIndexMask;
#endif

#if SUBTITLE_SUPPORT
    cdx_int32 subtitleNum, subtitleIndex;
    SubtitleStreamInfo subtitle[SUBTITLE_STREAM_LIMIT];
#endif

    cdx_int64 firstPts;
    cdx_int32 metadataNum;
};

#define PROGRAM_LIMIT 1 //no switch program interface now, so limit 1

struct CdxMediaInfoS
{
    cdx_int64 fileSize;
    cdx_uint32 bitrate;
    cdx_bool bSeekable;
    cdx_int32 programNum, programIndex;

    struct CdxProgramS program[PROGRAM_LIMIT];

    void *privData;

/*    cdx_uint8   album[64];
    cdx_int32   albumsz;

    cdx_uint8   author[64];
    cdx_int32   authorsz;

    cdx_uint8   genre[64];
    cdx_int32   genresz;

    cdx_uint8   title[64];
    cdx_int32   titlesz;

    cdx_uint8   year[64];
    cdx_int32   yearsz;

    cdx_uint8   composer[64];
    cdx_int32   composersz;

    cdx_uint8   date[64];
    cdx_int32   datesz;

    cdx_uint8   artist[64];
    cdx_int32   artistsz;

    cdx_uint8   writer[64];
    cdx_int32   writersz;

    cdx_uint8   albumArtist[64];
    cdx_int32   albumArtistsz;

    cdx_uint8   compilation[64];
    cdx_int32   compilationsz;

    cdx_uint8   cdTrackNumber[64];
    cdx_int32   cdTrackNumbersz;*/

    cdx_uint8   location[64];
    cdx_uint8   rotate[4];
    cdx_int32   discNumber;
    cdx_uint8   *pAlbumArtBuf;
    cdx_int32    nAlbumArtBufSize;

    int         id3v2HadParsed;
};

#define MUTIL_AUDIO         0x0001U /*will disable switch audio*/
//#define MUTIL_VIDEO         0x0002U /*will disable switch video*/
#define MUTIL_SUBTITLE      0x0004U /*will disable switch subtitle*/

#define DISABLE_AUDIO       0x0008U
#define DISABLE_VIDEO       0x0010U
#define DISABLE_SUBTITLE    0x0020U
#define NO_NEED_DURATION    0x0040U

#define BD_BASE             0x0080U
#define BD_DEPENDENT        0x0100U
#define BD_TXET             0x0180U

// for DASH
#define SEGMENT_MP4         0x0200U

// for mms playlist
#define NO_NEED_CLOSE_STREAM 0x0400U
// for miracast
#define MIRACST             0x0800U

// for smooth streaming
#define SEGMENT_SMS         0x1000U

// for cmcc timeShift
#define CMCC_TIME_SHIFT     0x2000U

#define NOT_LASTSEGMENT     0x4000U

// for playready sstr
#define SEGMENT_PLAYREADY   0x8000U

#define MutilAudioStream(flags) (!!(flags & MUTIL_AUDIO))

typedef struct HdcpOpsS HdcpOps;
struct HdcpOpsS
{
    int (*init)(void **);
    void (*deinit)(void *);
    cdx_uint32 (*decrypt)(void *handle, const cdx_uint8 privateData[16],
                          const cdx_uint8 *in, cdx_uint8 *out, cdx_uint32 len, int streamType);
};
typedef struct DownloadObjectS DownloadObject;
struct DownloadObjectS
{
    int seqNum;
    char *uri;
    int statusCode; //http statusCode
    cdx_int64 seqSize;    //segment size, byte
    cdx_int64 seqDuration;//segment duration, us
    cdx_int64 spendTime;  //ms
    cdx_int64 rate;       //bps
};

typedef struct TimeShiftEndInfoS TimeShiftEndInfoT;
struct TimeShiftEndInfoS
{
    int timeShiftLastSeqNum;
    cdx_int64 timeShiftDuration;
};

struct CdxParserCreatorS
{
    CdxParserT *(*create)(CdxStreamT *, cdx_uint32 /*flags*/);
    cdx_uint32 (*probe)(CdxStreamProbeDataT *);/*return score(0-100)*/
};

struct CdxParserOpsS
{
    cdx_int32 (*control)(CdxParserT *, cdx_int32 /* cmd */, void * /* param */);

    cdx_int32 (*prefetch)(CdxParserT *, CdxPacketT * /* pkt */);

    cdx_int32 (*read)(CdxParserT *, CdxPacketT * /* pkt */);

    cdx_int32 (*getMediaInfo)(CdxParserT *, CdxMediaInfoT * /* MediaInfo */);

    cdx_int32 (*seekTo)(CdxParserT *, cdx_int64 /* timeUs */);

    cdx_uint32 (*attribute)(CdxParserT *); /*return falgs define as open's falgs*/

    cdx_int32 (*getStatus)(CdxParserT *); /*return enum CdxPrserStatusE*/

    cdx_int32 (*close)(CdxParserT *);

    cdx_int32 (*init)(CdxParserT *);
};

struct CdxParserS
{
    enum CdxParserTypeE type;
    struct CdxParserOpsS *ops;
};

struct ParserUriKeyInfoS
{
    const char *comment;
    const char *scheme[10];
    const char *suffix[10];
    const char *attr[10];
};

#ifdef __cplusplus
extern "C"
{
#endif

int AwParserRegister(CdxParserCreatorT *creator, CdxParserTypeT type,
                    struct ParserUriKeyInfoS *keyInfo);

int CdxParserPrepare(CdxDataSourceT *source, cdx_uint32 flags, pthread_mutex_t *mutex,
                     cdx_bool *exit, CdxParserT **parser, CdxStreamT **stream,
                     ContorlTask *parserTasks, ContorlTask *streamTasks);
CdxParserT *CdxParserCreate(CdxStreamT *, cdx_uint32 /*flags*/);
int CdxParserOpen(CdxStreamT *stream, cdx_uint32 flags, pthread_mutex_t *mutex, cdx_bool *exit,
    CdxParserT **parser, ContorlTask *parserTasks);
#define CdxParserForceStop(parser) \
    (CdxParserControl(parser, CDX_PSR_CMD_SET_FORCESTOP, NULL))

#define CdxParserClrForceStop(parser) \
    (CdxParserControl(parser, CDX_PSR_CMD_CLR_FORCESTOP, NULL))

static inline cdx_int32 CdxParserControl(CdxParserT *parser, cdx_int32 cmd, void *param)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->control);
    return parser->ops->control(parser, cmd, param);
}

static inline cdx_int32 CdxParserPrefetch(CdxParserT *parser, CdxPacketT *pkt)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->prefetch);
    return parser->ops->prefetch(parser, pkt);
}

static inline cdx_int32 CdxParserRead(CdxParserT *parser, CdxPacketT *pkt)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->read);
    return parser->ops->read(parser, pkt);
}

static void PrintMediaInfo(CdxMediaInfoT *mediaInfo)
{
    CDX_LOGD("*********PrintMediaInfo begin*********");
    struct CdxProgramS *program = &mediaInfo->program[0];

    CDX_LOGD("fileSize = %lld, "
            "bSeekable = %d, "
            "duration = %d, "
            "audioNum = %d, "
#if VIDEO_SUPPORT
            "videoNum = %d, "
#endif
#if SUBTITLE_SUPPORT
            "subtitleNum = %d "
#endif
            ,
            mediaInfo->fileSize,
            mediaInfo->bSeekable,
            program->duration,
            program->audioNum
#if VIDEO_SUPPORT
            ,program->videoNum
#endif
#if SUBTITLE_SUPPORT
            ,program->subtitleNum
#endif
            );

    int i;
#if VIDEO_SUPPORT
    for (i = 0; i < VIDEO_STREAM_LIMIT && i < program->videoNum; i++)
    {
        VideoStreamInfo *video = program->video + i;
        CDX_LOGD("***Video[%d]*** "
                "eCodecFormat = 0x%x, "
                "nWidth = %d, "
                "nHeight = %d, "
                "nFrameRate = %d, "
                "nFrameDuration = %d, "
                "bIs3DStream = %d ",
                i,
                video->eCodecFormat,
                video->nWidth,
                video->nHeight,
                video->nFrameRate,
                video->nFrameDuration,
                video->bIs3DStream);

    }
#endif

    for (i = 0; i < AUDIO_STREAM_LIMIT && i < program->audioNum; i++)
    {
        AudioStreamInfo *audio = program->audio + i;
        CDX_UNUSE(audio);
        CDX_LOGD("***Audio[%d]*** "
                "eCodecFormat = 0x%x, "
                "eSubCodecFormat = 0x%x, "
                "nChannelNum = %d, "
                "nBitsPerSample = %d, "
                "nSampleRate = %d ",
                i,
                audio->eCodecFormat,
                audio->eSubCodecFormat,
                audio->nChannelNum,
                audio->nBitsPerSample,
                audio->nSampleRate);

    }

#if SUBTITLE_SUPPORT
    for (i = 0; i < SUBTITLE_STREAM_LIMIT && i < program->subtitleNum; i++)
    {
        SubtitleStreamInfo *subtitle = program->subtitle + i;

        CDX_LOGD("***Subtitle[%d]*** "
                "eCodecFormat = 0x%x, "
                "strLang = (%s) ",
                i,
                subtitle->eCodecFormat,
                subtitle->strLang);
    }
#endif

    CDX_LOGD("*********PrintMediaInfo end*********");
}

#ifdef __ANDROID__
#define ACODEC "ro.codec.audio."
#define VCODEC "ro.codec.video."
static int AudioCodecIsForbidden(int eCodecFormat)
{
    char value[PROP_VALUE_MAX] = {0};
    switch (eCodecFormat)
    {
    case AUDIO_CODEC_FORMAT_AC3:
    case AUDIO_CODEC_FORMAT_DTS:
        property_get(ACODEC "dts", value, "0");
        if (!strcmp(value, "off"))
            return 1;
        break;
    case AUDIO_CODEC_FORMAT_COOK:
        property_get(ACODEC "cook", value, "0");
        if (!strcmp(value, "off"))
            return 1;
        break;
    case AUDIO_CODEC_FORMAT_WMA_STANDARD:
    case AUDIO_CODEC_FORMAT_WMA_LOSS:
    case AUDIO_CODEC_FORMAT_WMA_PRO:
        property_get(ACODEC "wma", value, "0");
        if (!strcmp(value, "off"))
            return 1;
        break;
    default:
        break;
    }

    return 0;
}

#if VIDEO_SUPPORT
static int VideoCodecIsForbidden(int eCodecFormat)
{
    char value[PROP_VALUE_MAX] = {0};
    switch (eCodecFormat)
    {
    case VIDEO_CODEC_FORMAT_DIVX5:
        property_get(VCODEC "divx5", value, "0");
        if (!strcmp(value, "off"))
            return 1;
        break;
    case VIDEO_CODEC_FORMAT_RX:
        property_get(VCODEC "rx", value, "0");
        if (!strcmp(value, "off"))
            return 1;
        break;
    case VIDEO_CODEC_FORMAT_WMV3:
        property_get(VCODEC "vc-1", value, "0");
        if (!strcmp(value, "off"))
            return 1;
        break;
    default:
        break;
    }

    return 0;
}
#endif

static int FilterForbiddenStream(CdxMediaInfoT *mediaInfo)
{
    struct CdxProgramS *program = &mediaInfo->program[0];
    int i;

    int videoNum = program->videoNum;
    program->videoIndexMask = 0;
    for (i = 0; i < videoNum && i < VIDEO_STREAM_LIMIT; i++)
    {
        VideoStreamInfo *video = program->video + i;
        if (VideoCodecIsForbidden(video->eCodecFormat))
        {
            logw("video codec %d is forbidden", video->eCodecFormat);
            program->videoIndexMask |= 1 << i;
        }
    }

    int audioNum = program->audioNum;
    program->audioIndexMask = 0;
    for (i = 0;  i < audioNum && i < AUDIO_STREAM_LIMIT; i++)
    {
        AudioStreamInfo *audio = program->audio + i;
        if (AudioCodecIsForbidden(audio->eCodecFormat))
        {
            logw("audio codec %d is forbidden", audio->eCodecFormat);
            /* Todo: forbidden the evil one, not anyone */
            program->audioIndexMask |= 1 << i;
        }
    }

    return 0;
}
#endif

/**
 * Get the information of the media from the parser.
 *
 * @param parser A parser which is prepared.
 * @param mediaInfo A struct hold the information of the media.
 *
 * @return 0 for success.
 *
 * @warning Don't allocate memory for member variables like pCodecSpecificData
 *          of mediaInfo in the parser's implementation of this interface.
 *          pCodecSpecificData won't be freed after the function return, so the
 *          memory will lose if you do that. Just set pCodecSpecificData point
 *          to the area which will be freed by the parser, for example, when
 *          the parser is closed.
 */
static inline cdx_int32 CdxParserGetMediaInfo(CdxParserT *parser, CdxMediaInfoT *mediaInfo)
{
    int ret;
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->getMediaInfo);
    ret = parser->ops->getMediaInfo(parser, mediaInfo);
    PrintMediaInfo(mediaInfo);
#ifdef __ANDROID__
    FilterForbiddenStream(mediaInfo);
#endif
    return ret;
}

static inline cdx_int32 CdxParserSeekTo(CdxParserT *parser, cdx_int64 timeUs)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->seekTo);
    return parser->ops->seekTo(parser, timeUs);
}

static inline cdx_uint32 CdxParserAttribute(CdxParserT *parser)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->attribute);
    return parser->ops->attribute(parser);
}

static inline cdx_int32 CdxParserGetStatus(CdxParserT *parser)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->getStatus);
    return parser->ops->getStatus(parser);
}

static inline cdx_int32 CdxParserClose(CdxParserT *parser)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->close);
    return parser->ops->close(parser);
}

static inline cdx_int32 CdxParserInit(CdxParserT *parser)
{
    CDX_CHECK(parser);
    CDX_CHECK(parser->ops);
    CDX_CHECK(parser->ops->init);
    return parser->ops->init(parser);
}
enum {
    PROBE_SPECIFIC_DATA_ERROR =-3,
    PROBE_SPECIFIC_DATA_NONE  =-2,
    PROBE_SPECIFIC_DATA_UNCOMPELETE = -1,
    PROBE_SPECIFIC_DATA_SUCCESS  = 1,
};

#if VIDEO_SUPPORT
cdx_int32 ProbeVideoSpecificData(VideoStreamInfo* pVideoInfo, cdx_uint8* pDataBuf,
    cdx_uint32 dataLen, cdx_uint32 eVideoCodecFormat, enum CdxParserTypeE type);
cdx_int32 probeH265RefPictureNumber(cdx_uint8* pDataBuf, cdx_uint32 nDataLen);
#endif

#ifdef __cplusplus
}
#endif

#endif

