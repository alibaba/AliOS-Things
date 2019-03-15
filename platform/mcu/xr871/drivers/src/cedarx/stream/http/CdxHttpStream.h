/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxHttpStream.h
 * Description : Part of http stream.
 * History :
 *
 */

#ifndef CDX_HTTP_STREAM_H
#define CDX_HTTP_STREAM_H

#include <CdxStream.h>
#include <CdxAtomic.h>
#include <CdxUrl.h>
#include <AwPool.h>
#include <zlib.h>
#include <assert.h>

#define __SAVE_BITSTREAMS (0)

//#define closesocket close

#define __CONFIG_ZLIB (0)

#define CONFIG_ALI_YUNOS (0)

struct HttpHeaderField
{
    const char *key;
    const char *val;
};

typedef struct AW_HTTP_FIELD_TYPE
{
    char *fieldName;
    struct AW_HTTP_FIELD_TYPE *next;
}CdxHttpFieldT;

typedef struct AW_HTTP_HEADER
{
     char *protocol;
     char *method;
     char *uri;
     unsigned int statusCode;
     char *reasonPhrase;
     unsigned int httpMinorVersion;
     // Field variables
     CdxHttpFieldT *firstField;
     CdxHttpFieldT *lastField;
     unsigned int fieldNb;
     char *fieldSearch;
     CdxHttpFieldT *fieldSearchPos;
     // Body variables
     char *body;
     size_t bodySize;
     char *buffer;
     size_t bufferSize;
     unsigned int isParsed;
     //
     char *cookies;
     int posHdrSep;
 } CdxHttpHeaderT;

typedef struct BandwidthEntry
{
    cdx_int64 mDelayUs;
    cdx_int32 mNumBytes;
}BandwidthEntryT;

enum HttpStreamStateE
{
    HTTP_STREAM_IDLE       = 0x00L,
    HTTP_STREAM_CONNECTING = 0x01L,
    HTTP_STREAM_READING    = 0x02L,
    HTTP_STREAM_SEEKING    = 0x03L,
};

typedef struct CdxHttpStreamImpl
{
    CdxStreamT base;
    CdxStreamT *tcpStream;

    CdxStreamProbeDataT probeData;
    cdx_int32 ioState;                          //for interface use
    int seekAble;                               //seekable flag.
    CdxUrlT* url;                               //scheme,port,etc
    cdx_int32 exitFlag;                         //when close, exit
    cdx_int32 forceStopFlag;                    //forceStop, not exit GetNetworkData
    cdx_int64 totalSize;                        //content-length
    cdx_int64 bufPos;                           //stream data is buffered to this file pos.
    cdx_int64 readPos;                          //stream data is read to this pos.
    cdx_int32 eosFlag;                          //for internal use.
                                                //all stream data is read from network
    void *data;                                 //point to response(2K)
    cdx_char *sourceUri;                        //the source uri

    char *httpDataBuffer;                       //databuf, store one group data.
                                                //1) 4KB, store response entity+probeData;
                                                //2) 1024*n Bytes: normal data.
    int       httpDataSize;                     //total data size of httpDataBuffer

    char *httpDataBufferChunked;                // store parsed data from chunked data;
    int  httpDataSizeChunked;                   // valid data size of httpDataBufferChunked
    int  httpDataBufferPosChunked;              // read pos of httpDataBufferChunked,
                                                // 0~httpDataSizeChunked-1
    int  restChunkSize;                         // when chunked, during CdxStreamOpen,
                                                // httpDataBuffer may not store the whole chunk,
                                                // this variable store rest size of the chunk.
    int dataCRLF;                               // data\r\n, next chunk need to get rid of \r\n.
    cdx_char tmpChunkedLen[10];                 // store temp chunked len while force stop.
                                                // eg: abcd\r\n, ab
    int tmpChunkedSize;                         // size of temp chunked len while force stop.
                                                // eg: ab is 2 bytes
    int lenCRLF;                                // len\r\n, next chunk need to get rid of \r\n.
    int chunkedLen;                             // while force stop, store current chunked len.
                                                // len\r\n

    const cdx_char *ua;                         // UA
    int nHttpHeaderSize;                        //header number to be added
    struct CdxHttpHeaderField *pHttpHeader;     //http header
    ExtraDataContainerT hfsContainer;
    int chunkedFlag;                            //set when "Transfer-Encoding: chunked"
    cdx_atomic_t ref;                           //reference count, for free resource while
                                                //still blocking.
    pthread_t threadId;

    pthread_t getNetworkDataThreadId;
    int seekFlag;
    int pauseReadDataFlag;
    pthread_mutex_t bufferMutex;
    pthread_cond_t bufferCond;
    pthread_mutex_t seekMutex;
    pthread_cond_t seekCond;
    pthread_mutex_t pauseReadDataMutex;
    pthread_cond_t pauseReadDataCond;

    pthread_mutex_t lock;                      //for break blocking operation
    pthread_cond_t cond;

    cdx_char *buffer;                           //httpStream's buffer
    cdx_int64 maxBufSize;                       //max buffer size
    cdx_int64 validDataSize;
    cdx_char *bufReadPtr;                       //read pointer
    cdx_char *bufWritePtr;                      //write pointer
    cdx_char *bufEndPtr;                        //point to the end of buffer

    cdx_int64 mTotalTransferTimeUs;             //for bandwidth measurement.
    cdx_int32 mTotalTransferBytes;
    cdx_int32 mBandwidthTimes;
    BandwidthEntryT bandWidthHistory[100];
    cdx_int32 index;

    //cdx_char *protectAreaPtr;                 //protect area, not cover, for seek back.
    cdx_int64 protectAreaPos;                   //protectArea begin file pos.
                                                //readPos-protectAreaPos: 0~maxProtectAreaSize
    cdx_int64 protectAreaSize;                  //Size of protectArea
    cdx_int32 maxProtectAreaSize;
    cdx_int32 bufFullFlag;                      //buffer full flag.
    cdx_int32 getNetworkDataFlag;               //in GetNetworkData loop flag.

    enum HttpStreamStateE state;
    AwPoolT *pool;
    cdx_int32 bStreamReadEos;                   //CdxStreamRead eos.

    //cdx_int32 nResponseDataSize;              //when reconnect, store the size of data
                                                //in response entity.
#if __SAVE_BITSTREAMS
    FILE *fp_http;
    cdx_int32 streamIndx;
    cdx_char location[256];
#endif

    ParserCallback callback;
    void *pUserData;
    cdx_int64 downloadStart;
    cdx_int64 downloadEnd;
    cdx_int64 downloadTimeMs;

#if CONFIG_ALI_YUNOS
    cdx_int64 downloadFirstTime;
    cdx_int64 downloadLastTime;
    int mYunOSstatusCode;
    char tcpIp[128];
#endif

    int isHls;
    cdx_int64 baseOffset; //* for id3 parser

#if __CONFIG_ZLIB
    int compressed;
    z_stream inflateStream;
    cdx_uint8 *inflateBuffer;
#endif

    int keepAlive;
    int isAuth;
    cdx_bool isDTMB;
    cdx_bool enoughData;
}CdxHttpStreamImplT;

typedef struct CdxHttpSendBuffer
{
    void *size;
    void *buf;
}CdxHttpSendBufferT;

extern CdxHttpHeaderT *CdxHttpNewHeader(void);
extern char *CdxHttpGetField(CdxHttpHeaderT *httpHdr, const char *fieldName);
extern char* CdxHttpBuildRequest(CdxHttpHeaderT *httpHdr);
extern CdxUrlT *CdxUrlRedirect(CdxUrlT **url, char *redir);
extern const char *GetUA(int n, CdxHttpHeaderFieldT *pHttpHeader);
extern CdxUrlT* CdxUrlNew(char* url);
extern void CdxHttpSetUri(CdxHttpHeaderT *httpHdr, const char *uri);
extern void CdxHttpSetField(CdxHttpHeaderT *httpHdr, const char *fieldName);
extern void CdxHttpFree(CdxHttpHeaderT *httpHdr);
extern int CdxHttpResponseAppend(CdxHttpHeaderT *httpHdr, char *response, int length);
extern int CdxHttpIsHeaderEntire(CdxHttpHeaderT *httpHdr);
extern int CdxHttpResponseParse(CdxHttpHeaderT *httpHdr);
extern int CdxHttpAuthenticate(CdxHttpHeaderT *httpHdr, CdxUrlT *url, int *authRetry);
extern cdx_int32 ReadChunkedSize(CdxStreamT *stream, cdx_char tmpLen[], cdx_int32* size);
extern cdx_int32 ReadChunkedData(CdxStreamT *stream, void *ptr, cdx_int32 size);
extern char *RmSpace(char *str);
extern int CdxHttpAddBasicAuthentication(CdxHttpHeaderT *httpHdr, const char *username,
    const char *password);

#endif
