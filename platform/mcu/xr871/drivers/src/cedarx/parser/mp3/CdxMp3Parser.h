/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : CdxMp3Parser.h
* Description :
* History :
*   Author  : Khan <chengkan@allwinnertech.com>
*   Date    : 2014/08/08
*/

#ifndef _CDX_MP3_PARSER_H_
#define _CDX_MP3_PARSER_H_

#include "Id3Base.h"

#define ENABLE_INFO_DEBUG       1
#define ENABLE_FILE_DEBUG       0
#define AV_TIME_BASE            1000000

#define MP3_PROBE_SIZE          10 * 1024
#define MP3_PACKET_SIZE         1024
#define AVPROBE_SCORE_MAX       100

#define ID3v2_HEADER_SIZE       10
#define MPA_STEREO              0
#define MPA_JSTEREO             1
#define MPA_DUAL                2
#define MPA_MONO                3

#define    INFLEN    1024*8
#define ID3TAGNUM 8
#define SKIPLEN 1024

typedef enum mp3_sta_ {
    MP3_STA_IDLE,
    MP3_STA_SEEKING,
    MP3_STA_PREFETCHING,
    MP3_STA_READING,
    MP3_STA_INITING,
}mp3_sta;

#define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

const uint16_t CdxMpaFreqTab[3] = { 44100, 48000, 32000 };

const uint16_t CdxMpaBitrateTab[2][3][15] = {
    { {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
      {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },
      {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 } },
    { {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160}
    }
};

#define MPADECODEHEADER \
    int mFrameSize; \
    int errorProtection; \
    int layer; \
    int mSampleRate; \
    int mSampleRateIndex; /* between 0 and 8 */ \
    int mBitRate; \
    int nChannels; \
    int mode; \
    int modeExt; \
    int lsf;

typedef struct MPADecodeHeader {
    MPADECODEHEADER
} MPADecodeHeader;
#if 0
static int CdxId3v2Match(const cdx_uint8 *buf)
{
    return  ('I' == buf[0]) &&
            ('D' == buf[1]) &&
            ('3' == buf[2]) &&
            (0xff != buf[3]) &&
            (0xff != buf[4]) &&
            (0 == (buf[6] & 0x80)) &&
            (0 == (buf[7] & 0x80)) &&
            (0 == (buf[8] & 0x80)) &&
            (0 == (buf[9] & 0x80));
}

static int CdxId3v2TagLen(const uint8_t * buf)
{
    int len = ((buf[9]  & 0x7f) +
               ((buf[6] & 0x7f) << 21) +
               ((buf[7] & 0x7f) << 14) +
               (buf[8] & 0x7f) << 7)   +
               ID3v2_HEADER_SIZE;

    if (buf[5] & 0x10)
        len += ID3v2_HEADER_SIZE;
    return len;
}
#endif

typedef struct __language_coding
{
    char language[4];
    cdx_int32  coding;
}anguage_coding_s;

typedef enum {
    IMG_FORMAT_BMP =0,
    IMG_FORMAT_JPG,
    IMG_FORMAT_GIF,
    IMG_FORMAT_PNG,
    IMG_FORMAT_UNSUPPORT = -1
}img_format_t;

typedef struct CDX_ID3_IMAGE_INFO
{
    int               length;         //image length
    int               FileLocation;   //image location
    img_format_t      img_format;
    int               pic_type;       //picture type;
    int               img_high;
    int               img_width;
    int               otherdata;

}cdx_id3_image_info_t;

typedef struct XINGSeeker {
    int64_t mFirstFramePos;
    int64_t mDurationUs;
    int32_t mSizeBytes;
    int32_t mEncoderDelay;
    int32_t mEncoderPadding;

    // TOC entries in XING header. Skip the first one since it's always 0.
    unsigned char mTOC[99];
    cdx_bool mTOCValid;
}XINGSeeker;

typedef struct VBRISeeker {
    int64_t     mBasePos;
    int64_t     mDurationUs;
    cdx_uint32  mSegmentsize;
    cdx_uint32  mSegments[1024];
}VBRISeeker;

typedef struct MP3ParserImpl{
    // Cdx Struct
    CdxParserT      base;
    CdxStreamT      *stream;

    int             mStatus;
    int             mErrno;    // errno
    int             exitFlag;

    pthread_cond_t  cond;

    XINGSeeker      *mXINGSeeker;
    VBRISeeker      *mVBRISeeker;

    cdx_int64       mFileSize;
    cdx_uint64      mFirstFramePos;
    cdx_uint32      mFixedHeader;
    int64_t         mCurrentPos;
    cdx_uint16      mChannels;
    cdx_uint32      mSampleRate;
    cdx_int32       mBitRate;
    cdx_int32       mavgBitRate;
    cdx_uint32      mFrameSize;
    cdx_int32       readPacketSize;
    /*Duration*/
    cdx_uint64      mDuration;

    cdx_uint32      mSeeking;
    cdx_int64       mSeekingTime;
    int             teeFd;

    ID3*            id3v1;
} MP3ParserImpl;

#endif
