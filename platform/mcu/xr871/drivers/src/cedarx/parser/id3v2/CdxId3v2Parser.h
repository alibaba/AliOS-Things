/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : CdxId3Parser.h
* Description :
* History :
*   Author  : Khan <chengkan@allwinnertech.com>
*   Date    : 2014/12/08
*/

#ifndef CDX_ID3_PARSER_H
#define CDX_ID3_PARSER_H

#define    INFLEN    1024*8//less than 8k
#define ID3TAGNUM 8
#define SKIPLEN 1024
#define ID3v2_HEADER_SIZE       10

#include "Id3Base.h"

typedef struct __language_coding
{
    char language[4];
    cdx_int32  coding;
} language_coding_t;

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

typedef enum CDX_A_AUDIO_FONTTYPE
{
    A_AUDIO_FONTTYPE_ISOIEC8859_1  = 0,       //ISO/IEC 8859-1
    A_AUDIO_FONTTYPE_UTF_16LE,//
    A_AUDIO_FONTTYPE_UTF_16BE,
    A_AUDIO_FONTTYPE_UTF_8,//
    A_AUDIO_FONTTYPE_ISOIEC8859_2,//
    A_AUDIO_FONTTYPE_ISOIEC8859_3,//
    A_AUDIO_FONTTYPE_ISOIEC8859_4,//
    A_AUDIO_FONTTYPE_ISOIEC8859_5,//
    A_AUDIO_FONTTYPE_ISOIEC8859_6,
    A_AUDIO_FONTTYPE_ISOIEC8859_7,
    A_AUDIO_FONTTYPE_ISOIEC8859_8,
    A_AUDIO_FONTTYPE_ISOIEC8859_9,
    A_AUDIO_FONTTYPE_ISOIEC8859_10,
    A_AUDIO_FONTTYPE_ISOIEC8859_11,
    A_AUDIO_FONTTYPE_ISOIEC8859_12,
    A_AUDIO_FONTTYPE_ISOIEC8859_13,
    A_AUDIO_FONTTYPE_ISOIEC8859_14,
    A_AUDIO_FONTTYPE_ISOIEC8859_15,
    A_AUDIO_FONTTYPE_ISOIEC8859_16,
    A_AUDIO_FONTTYPE_WINDOWS_1250,
    A_AUDIO_FONTTYPE_WINDOWS_1251,//
    A_AUDIO_FONTTYPE_WINDOWS_1252,
    A_AUDIO_FONTTYPE_WINDOWS_1253,
    A_AUDIO_FONTTYPE_WINDOWS_1254,
    A_AUDIO_FONTTYPE_WINDOWS_1255,
    A_AUDIO_FONTTYPE_WINDOWS_1256,
    A_AUDIO_FONTTYPE_WINDOWS_1257,
    A_AUDIO_FONTTYPE_WINDOWS_1258,
    A_AUDIO_FONTTYPE_KOI8_R,
    A_AUDIO_FONTTYPE_KOI8_U,
    A_AUDIO_FONTTYPE_GB2312,
    A_AUDIO_FONTTYPE_GBK,
    A_AUDIO_FONTTYPE_BIG5,
    A_AUDIO_FONTTYPE_ = -1
}cdx_audio_fonttype_e;

struct Id3Pic
{
    cdx_uint8* addr;
    cdx_int32 lenth;
    struct Id3Pic*   father;
};

typedef struct Id3v2ParserImplS
{
    //audio common
    CdxParserT base;
    CdxStreamT *stream;
    cdx_int32 mErrno; //Parser Status
    cdx_uint32 flags; //cmd
    cdx_int32  forceStop;
    //id3 base
    CdxParserT *child;/*为子parser，下一级的媒体文件parser，如mp3 parser*/
    cdx_int32 shareStreamWithChild;
    cdx_char *  keyinfo;
//    cdx_char    newurl[128];
    cdx_int64   fdoffset;
    cdx_int64   file_size;
    CdxDataSourceT cdxDataSource;
    cdx_int32  Id3v2len;
    cdx_int64  file_offset;
    CdxStreamT *childStream;
    pthread_mutex_t lock;

#if 0
     /*ID3*/
    cdx_int32                   mGenre_sz;             // 流派
    char                        *mGenre;               // pop soft...
    cdx_audio_fonttype_e        mGenreCharEncode;

    cdx_int32                   mtitle_sz;             // 歌曲名
    char                        *mtitle;
    cdx_audio_fonttype_e        mtitleCharEncode;

    cdx_int32                   mauthor_sz;            // 演唱者
    char                        *mauthor;
    cdx_audio_fonttype_e        mauthorCharEncode;

    cdx_int32                   mAlbum_sz;             // 专集
    char                        *mAlbum;
    cdx_audio_fonttype_e        mAlbumCharEncode;

    cdx_int32                   mYear_sz;              // 出品年代
    char                        *mYear;
    cdx_audio_fonttype_e        mYearCharEncode;

    cdx_int32                   mAPic_sz;             // attached picture
    cdx_id3_image_info_t        *mAPic;
    cdx_audio_fonttype_e        mAPicCharEncode;

    cdx_int32                   mInforBufLeftLength;
    cdx_uint8                   *mInforBuf;
    cdx_uint8                    mInfor[INFLEN];
    struct Id3Pic               *pAlbumArt;
    cdx_int32                   pAlbumArtid;
#endif

    ID3*                        id3v2;
}Id3v2ParserImplS;

#endif
