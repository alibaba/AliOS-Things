/*
 * Copyright (c) 2008-2017 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : Id3Base.h
 * Description : Id3_v1 and Id3_v2 parser head file
 * History :
 *
 */

#ifndef ID3_H
#define ID3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "StringContainer.h"
#include "CdxMetaData.h"

#define IS_VERSION_2(v) ((v >= ID3_V2_2) && (v <= ID3_V2_4))
#define IS_VERSION_1(v) ((v >= ID3_V1) && (v <= ID3_V1_1))

#define V1_TAG_SIZE 128

typedef struct _ID3 ID3;
typedef struct _Iterator Iterator;
typedef struct _id3_header id3_header;

struct _Iterator {
    ID3  *mParent;//get from parent, parent to free
    char *mID;
    size_t mOffset;
    const cdx_uint8 *mFrameData;//get from parent, parent to free
    size_t mFrameSize;
    int    encoding;

    kbool      (*done)(void* hself);
    kerrno     (*getID)(void* hself, StringCtn *id);
    kerrno     (*getString)(void* hself, StringCtn *s, StringCtn *ss/*NULL*/);
    kerrno     (*getStringInternal)(void* hself, StringCtn *s, kbool secondhalf);
    const cdx_uint8* (*getData)(void* hself, size_t *length);
    kerrno     (*next)(void* hself);
    kerrno     (*findFrame)(void* hself);
    size_t     (*getHeaderLength)(void* hself);
};

enum _Version {
    ID3_UNKNOWN,
    ID3_V1,
    ID3_V1_1,
    ID3_V2_2,
    ID3_V2_3,
    ID3_V2_4,
};

typedef enum _Version Version;

struct _id3_header {
    char id[3];
    cdx_uint8 version_major;
    cdx_uint8 version_minor;
    cdx_uint8 flags;
    cdx_uint8 enc_size[4];
};

struct _ID3 {
    kbool    mIsValid;
    cdx_uint8 *mData;
    size_t   mSize;
    size_t   mFirstFrameOffset;
    Version  mVersion;

    // size of the ID3 tag including header before any unsynchronization.
    // only valid for IDV2+
    size_t   mRawSize;

    kbool  (*isValid)(void* hself);
    Version (*version)(void* hself);
    void * (*getAlbumArt)(void* hself, size_t *length, StringCtn *mime);
    size_t (*rawSize)(void* hself);
    kbool  (*doParseV1)(void* hself);
    kbool  (*doParseV2)(void* hself);
    kerrno (*doRemoveUnsynchronization)(void* hself);
    kbool  (*doRemoveUnsynchronizationV2_4)(void* hself, kbool iTunesHack);

    CdxStreamT*   stream;
    cdx_uint8*    extraBuf;
    size_t        extraBufRange;
    size_t        extraBufOffset;
    size_t        extraBufValidSz;
    size_t  (*read)(void* hself, void* buf, size_t sz);
    size_t  (*readAt)(void* hself, int64_t offset, void* buf, size_t sz);
    kbool   (*getsize)(void* hself, int64_t* sz);
    int64_t (*tell)(void* hself);
};

Iterator* GenerateIterator(ID3* parent, const char *id);
kbool EraseIterator(void* arg);
ID3*  GenerateId3(CdxStreamT* in, cdx_uint8* init_buf, size_t buf_sz,
                    kbool ignoreV1);
kbool EraseId3(void* arg);

void  Id3BaseGetMetaData(CdxMediaInfoT *mediaInfo, ID3* id3Base);

void Id3BaseExtraAlbumPic(CdxMediaInfoT *mediaInfo, ID3* id3Base);

#endif
