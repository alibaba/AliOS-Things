/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxStream.h
 * Description : Stream
 * History :
 *
 */

/*
**********************************************************************
* File Name  : CdxStream.h
* Author       : bzchen@allwinnertech.com
* Version      : 1.0
* Data          : 2013.08.29
* Description: This file define data structure and interface of the stream module.
**********************************************************************/

#ifndef CDX_STREAM_H
#define CDX_STREAM_H
#include <CdxTypes.h>
#include <cdx_log.h>
#include <CdxBinary.h>
#include <CdxAtomic.h>
#include <CdxKeyedVector.h>
#include <CdxEnumCommon.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CdxStreamProbeDataS CdxStreamProbeDataT;
typedef struct CdxStreamCreatorS CdxStreamCreatorT;

#define STREAM_SEEK_SET 0           //* offset from the file start.
#define STREAM_SEEK_CUR 1           //* offset from current file position.
#define STREAM_SEEK_END 2           //* offset from the file end.

#define CDX_STREAM_FLAG_SEEK    0x01U
#define CDX_STREAM_FLAG_STT     0x02U   /*seek to time*/
#define CDX_STREAM_FLAG_NET     0x04U   /*net work stream*/
#define CDX_STREAM_FLAG_DTMB    0x08U   /*dtmb live stream */

typedef struct CdxDataSourceS CdxDataSourceT;
typedef struct CdxStreamS CdxStreamT;
typedef CdxStreamT CustomerStream;

enum DSExtraDataTypeE
{
    EXTRA_DATA_UNKNOWN,
    EXTRA_DATA_HTTP_HEADER,
    EXTRA_DATA_APPOINTED_TS,
    EXTRA_DATA_RTP,
    EXTRA_DATA_AES,
    EXTRA_DATA_BDMV,
    EXTRA_DATA_RTSP,
    EXTRA_DATA_HTTP, //download spend time etc.
    EXTRA_DATA_HLS,
};

struct CdxDataSourceS
{
    void* pHTTPServer;
    cdx_char *uri;  /* format : "scheme://..." */
    void *extraData; /* extra data for some stream, ex: http header for http stream */
    enum DSExtraDataTypeE extraDataType;
    cdx_int64 offset; /* for id3 parser */
    int probeSize;
};

#if 1
/* These two struct is deprecated, you should use CdxKeyedVectorT interface */
typedef struct CdxHttpHeaderField
{
    const char *key;
    const char *val;
}CdxHttpHeaderFieldT;

typedef struct CdxHttpHeaderFields
{
    int num;
    CdxHttpHeaderFieldT *pHttpHeader;
}CdxHttpHeaderFieldsT;
#endif

typedef struct ExtraDataContainerS
{
    void *extraData;
    enum DSExtraDataTypeE extraDataType;
}ExtraDataContainerT;

/*------------------------BDMV stream header---------------------------*/
struct IoOperationS
{
    int (*openDir)(void * /*cbhdr*/, const char * /*name*/, void ** /*dir handler*/);
    int (*readDir)(void * /*cbhdr*/, void * /*dir handler*/, char * /*dname*/, int /*dnameLen*/);
    int (*closeDir)(void * /*cbhdr*/, void * /*dir handler*/);

    int (*openFile)(void * /*cbhdr*/, const char * /*pathname*/, int /*flags*/);
    int (*accessFile)(void * /*cbhdr*/, const char * /*pathname*/, int /*mode*/);
};

struct BdmvExtraDataS
{
    struct IoOperationS *ioCb;
    void *cbHdr;
};
/*------------------------RTP end-------------------------------------*/

/*------------------------RTP stream header----------------------------*/
typedef struct RtpStreamItfS RtpStreamItfT;
struct RtpStreamItfOpsS
{
    int (*filter)(RtpStreamItfT *, CdxBufferT * /*in*/, CdxListT ** /*out*/);
    void (*destroy)(RtpStreamItfT *);
};

struct RtpStreamItfS
{
    struct RtpStreamItfOpsS *ops;
};

struct RtpStreamExtraDataS
{
    RtpStreamItfT *itf;
    cdx_uint32 timeScale;
    cdx_bool multicast;
};
/*------------------------RTP end-------------------------------------*/

/*------------------------AES stream header----------------------------*/
enum PaddingType
{
    CDX_PKCS7,
};

typedef struct AesStreamExtraDataS
{
    cdx_uint8 key[16];
    cdx_uint8 iv[16];
    enum PaddingType paddingType;
    void *extraData; /* extra data for some stream, ex: http header for http stream */
    enum DSExtraDataTypeE extraDataType;
}AesStreamExtraDataT;
/*------------------------AES end------------------------------------*/

//***********************************************************//
//* Define IO status.
//***********************************************************//
enum CdxIOStateE
{
    CDX_IO_STATE_OK = 0,  //* nothing wrong of the data accession.
    CDX_IO_STATE_INVALID,
    CDX_IO_STATE_ERROR,   //* unknown error, can't access the media file.
    CDX_IO_STATE_EOS,      /*end of stream*/
    CDX_IO_STATE_CLEAR  // for mms playlist
};

/*for stream->control*/
enum CdxStreamCommandE
{
    STREAM_CMD_GET_DURATION = 0x101,
        /* Get the duration of the media file.
            *param is a pointer to a int64_t variable, duration = *(int64_t*)param.
            *return 0 if OK, return -1 if not supported by the stream handler.
            */

    STREAM_CMD_READ_NOBLOCK = 0x102,
        /* for sock recv in seek operation, return as soon as possible.
             * return 0 if OK, return -1 if not support.
             */

    STREAM_CMD_GET_SOCKRECVBUFLEN = 0x103,
        /* Get the socket recv buf len, *(cdx_int32*)param.
             * return 0 if OK, return -1 if not support.
             */
        /* To Add More commands here.*/

        /* struct StreamCacheStateS */
    STREAM_CMD_GET_CACHESTATE = 0x104,

    STREAM_CMD_SET_FORCESTOP = 0x105,
        /* Force stop the stream running job.  */

    STREAM_CMD_CLR_FORCESTOP = 0x106,
        /* Resume the stream running job.  */

    STREAM_CMD_RESET_STREAM = 0x107,
        /*for mms playlist*/

    STREAM_CMD_EXT_IO_OPERATION = 0x108,
        /*for bdmv io operation: opendir, readdir, closedir, open, access*/

    STREAM_CMD_SET_CALLBACK = 0x109,
        /*for setting callback*/
    STREAM_CMD_SET_ISHLS= 0x110,
        /*for setting hls parser flag*/

    STREAM_CMD_GET_IP = 0x110,
        /* For get tcp ip*/
    STREAM_CMD_REQUEST_WRITE_INFO = 0x120,
    STREAM_CMD_UPDATE_WRITE_INFO  = 0x121,
    STREAM_CMD_SET_EOF            = 0x122,
    STREAM_CMD_SET_RELATIVE_START_POS,
    STREAM_CMD_SET_RELATIVE_FILE_SIZE,
};

/*stream event*/
enum CdxStreamEventE
{
    STREAM_EVT_DOWNLOAD_START = STREAM_EVENT_VALID_RANGE_MIN,
    STREAM_EVT_DOWNLOAD_END,
    STREAM_EVT_DOWNLOAD_ERROR,
    STREAM_EVT_NET_DISCONNECT,
    STREAM_EVT_DOWNLOAD_RESPONSE_HEADER,
    STREAM_EVT_DOWNLOAD_START_TIME,
    STREAM_EVT_DOWNLOAD_FIRST_TIME,
    STREAM_EVT_DOWNLOAD_END_TIME,
    STREAM_EVT_DOWNLOAD_GET_TCP_IP,
    STREAM_EVT_DOWNLOAD_DOWNLOAD_ERROR,
    STREAM_EVT_CMCC_LOG_RECORD,

    STREAM_EVT_MAX,
};
CHECK_STREAM_EVENT_MAX_VALID(STREAM_EVT_MAX)

/* STREAM_CMD_EXT_IO_OPERATION param */
enum ExtIoctlCmdE
{
    EXT_IO_OPERATION_OPENDIR = 0,
    EXT_IO_OPERATION_READDIR,
    EXT_IO_OPERATION_CLOSEDIR,

    EXT_IO_OPERATION_OPENFILE,
    EXT_IO_OPERATION_READFILE,
    EXT_IO_OPERATION_CLOSEFILE,

    EXT_IO_OPERATION_ACCESS,
};

struct ExtIoctlParamS
{
    int cmd;

    void *inHdr; /* (int)'dirId', (int)'fd', 'DIR *' */
    char *inPath;

    char *inBuf;
    int inBufLen;

    void *outHdr; /* (int)'dirid', (int)'fd', 'DIR *' */
    int outRet;
};

typedef enum CdxIOStateE CdxIOStateT;
typedef enum CdxStreamCommandE CdxStreamCommandT;

struct StreamCacheStateS
{
    cdx_int32 nCacheCapacity;
    cdx_int32 nCacheSize;
    cdx_int32 nBandwidthKbps;
    cdx_int32 nPercentage; /* ((100 * download_offset)/totle_size)*/
};

struct CdxStreamProbeDataS
{
    cdx_char *buf;
    cdx_uint32 len;

    /* maybe we should define it as char private[0]? */
    cdx_char *uri[0];
};

typedef int (*ParserCallback)(void* pUserData, int eMessageId, void* param);
struct CallBack
{
    ParserCallback callback;
    void *pUserData;
};

typedef struct ContorlTaskS ContorlTask;
struct ContorlTaskS
{
    cdx_int32 cmd;
    void *param;
    ContorlTask *next;
};
struct CdxStreamCreatorS
{
    CdxStreamT *(*create)(CdxDataSourceT *);
};

struct CdxStreamOpsS
{
    cdx_int32 (*connect)(CdxStreamT * /*stream*/);

    CdxStreamProbeDataT *(*getProbeData)(CdxStreamT * /*stream*/);

    cdx_int32 (*stream_read)(CdxStreamT * /*stream*/, void * /*buf*/, cdx_uint32 /*len*/);

    cdx_int32 (*stream_close)(CdxStreamT * /*stream*/);

    cdx_int32 (*getIOState)(CdxStreamT * /*stream*/);

    cdx_uint32 (*attribute)(CdxStreamT * /*stream*/);

    cdx_int32 (*control)(CdxStreamT * /*stream*/, cdx_int32 /*cmd*/, void * /*param*/);

    /*以上接口必须实现*/

    cdx_int32 (*stream_write)(CdxStreamT *, void * /*buf*/, cdx_uint32 /*len*/);

    cdx_int32 (*getMetaData)(CdxStreamT *, const cdx_char * /*key*/, void ** /*pVal*/);

    cdx_int32 (*seek)(CdxStreamT * /*stream*/, cdx_int64 /*offset*/, cdx_int32 /*whence*/);

    cdx_int32 (*seekToTime)(CdxStreamT * /*stream*/, cdx_int64 /*time us*/);

    cdx_bool (*eos)(CdxStreamT * /*stream*/);

    cdx_int64 (*tell)(CdxStreamT * /*stream*/);

    cdx_int64 (*size)(CdxStreamT * /*stream*/);

    //cdx_int32 (*forceStop)(CdxStreamT * /*stream*/);

    /*cut  Size/CachedSize/SetBufferSize/GetBufferSize */
};

struct CdxStreamS
{
    struct CdxStreamOpsS *ops;
};

int AwStreamRegister(CdxStreamCreatorT *creator, cdx_char *type);

CdxStreamT *CdxStreamCreate(CdxDataSourceT *source);

/****************************************************************************
 *           Danger! Danger! Danger!
 *
 * CdxStreamOpen() includes two parts: create() and connect().
 * create(): It is (or should) nonblock, and unlikely to fail.
 * connect(): It can block, and has a great chance to fail.
 *
 * After create() return, you can use the stream to force stop connect(). This
 * is why we separate CdxStreamOpen() into two parts.
 *
 * So, you must call CdxStreamClose() even when CdxStreamOpen() failed. Or you
 * will suffer from memory and file descriptor leak. Yes, it's pain in the ass!
 *
 ***************************************************************************/
int CdxStreamOpen(CdxDataSourceT *source, pthread_mutex_t *mutex, cdx_bool *exit,
        CdxStreamT **stream, ContorlTask *streamTasks);

static inline cdx_int32 CdxStreamConnect(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->connect);
    return stream->ops->connect(stream);
}

static inline CdxStreamProbeDataT *CdxStreamGetProbeData(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->getProbeData);
    return stream->ops->getProbeData(stream);
}

static inline cdx_int32 CdxStreamRead(CdxStreamT *stream, void *buf, cdx_int32 len)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->stream_read);
    return stream->ops->stream_read(stream, buf, len);
}

static inline cdx_int32 CdxStreamWrite(CdxStreamT *stream, void *buf, cdx_int32 len)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->stream_write ? stream->ops->stream_write(stream, buf, len) : -1;
}

static inline cdx_int32 CdxStreamClose(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->stream_close);
    return stream->ops->stream_close(stream);
}

static inline cdx_int32 CdxStreamGetIoState(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->getIOState);
    return stream->ops->getIOState(stream);
}

static inline cdx_uint32 CdxStreamAttribute(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->attribute);
    return stream->ops->attribute(stream);
}

static inline cdx_int32 CdxStreamControl(CdxStreamT *stream,
                                        cdx_int32 cmd, void *param)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    CDX_CHECK(stream->ops->control);
    return stream->ops->control(stream, cmd, param);
}

//#define STREAM_METADATA_ORGINAL_URI     "stream.orginalUri"
#define STREAM_METADATA_REDIRECT_URI    "stream.redirectUri"
#define STREAM_METADATA_ACCESSIBLE_URI  "stream.accessibleUri"

static inline cdx_int32 CdxStreamGetMetaData(CdxStreamT *stream,
                                        const cdx_char *key, void **pVal)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);
    return stream->ops->getMetaData ?
            stream->ops->getMetaData(stream, key, pVal) :
            -1;
}

static inline cdx_int32 CdxStreamSeek(CdxStreamT *stream, cdx_int64 offset,
                                    cdx_int32 whence)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->seek ? stream->ops->seek(stream, offset, whence) : -1;
}

static inline cdx_int32 CdxStreamSeekToTime(CdxStreamT *stream, cdx_int64 timeUs)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->seekToTime ? stream->ops->seekToTime(stream, timeUs) : -1;
}

static inline cdx_bool CdxStreamEos(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->eos ? stream->ops->eos(stream) : CDX_FALSE;
}

static inline cdx_int64 CdxStreamTell(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->tell ? stream->ops->tell(stream) : -1;
}
/*
CDX_INTERFACE cdx_int32 CdxStreamForceStop(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->forceStop ? stream->ops->forceStop(stream) : -1;
}
*/

static inline cdx_int64 CdxStreamSize(CdxStreamT *stream)
{
    CDX_CHECK(stream);
    CDX_CHECK(stream->ops);

    return stream->ops->size ? stream->ops->size(stream) : (-1LL);
}

#define CdxStreamSeekAble(stream) \
    (!!(CdxStreamAttribute(stream) & CDX_STREAM_FLAG_SEEK))

#define CdxStreamIsNetStream(stream) \
    (!!(CdxStreamAttribute(stream) & CDX_STREAM_FLAG_NET))

#define CdxStreamForceStop(stream) \
    (CdxStreamControl(stream, STREAM_CMD_SET_FORCESTOP, NULL))

#define CdxStreamClrForceStop(stream) \
    (CdxStreamControl(stream, STREAM_CMD_CLR_FORCESTOP, NULL))

#define CdxStreamIsDtmbStream(stream) \
    (!!(CdxStreamAttribute(stream) & CDX_STREAM_FLAG_DTMB))

static inline cdx_int32 CdxStreamSkip(CdxStreamT *stream, cdx_uint32 len)
{
    if (len == 0)
    {
        return 0;
    }
    if (CdxStreamSeekAble(stream))
    {
        return CdxStreamSeek(stream, (cdx_int64)len, STREAM_SEEK_CUR);
    }
    else
    {
        cdx_int32 ret = 0;
        int i = 0, skipSize = 0, unitSize = 0;
        cdx_char dummyBuf[512];
        if (len > 512 * 10)
        {
            CDX_LOGE("too large(%u) to dummy for a unseekable stream", len);
            return -1;
        }

        skipSize = len;
        for (i = 0; i < 10; i++)
        {
            unitSize = (skipSize > 512) ? 512 : skipSize;
            ret = CdxStreamRead(stream, dummyBuf, unitSize);
            if (ret != unitSize)
            {
                return -1;
            }
            skipSize -= ret;
            if (skipSize == 0)
            {
                break;
            }
        }
        return CDX_SUCCESS;
    }
}

#define CdxStreamGetU8(stream) \
    ({cdx_uint8 data; CdxStreamRead(stream, &data, 1); GetU8(&data);})

#define CdxStreamGetLE16(stream) \
    ({cdx_uint8 data[2]; CdxStreamRead(stream, data, 2); GetLE16(data);})

#define CdxStreamGetLE32(stream) \
        ({cdx_uint8 data[4]; CdxStreamRead(stream, data, 4); GetLE32(data);})

#define CdxStreamGetLE64(stream) \
        ({cdx_uint8 data[8]; CdxStreamRead(stream, data, 8); GetLE64(data);})

#define CdxStreamGetBE16(stream) \
    ({cdx_uint8 data[2]; CdxStreamRead(stream, data, 2); GetBE16(data);})

#define CdxStreamGetBE32(stream) \
    ({cdx_uint8 data[4]; CdxStreamRead(stream, data, 4); GetBE32(data);})

#define CdxStreamGetBE64(stream) \
        ({cdx_uint8 data[8]; CdxStreamRead(stream, data, 8); GetBE64(data);})

typedef struct REQUEST_BUFFER_INFO
{
    unsigned char* bufPtr;
    unsigned int   bufLen;
}StreamBufferInfo;

#ifdef __cplusplus
}
#endif

#endif

