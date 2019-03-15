/*
* Copyright (c) 2008-2017 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : CdxMetaData.h
* Description :
* History :
*   Author  : Khan <chengkan@allwinnertech.com>
*   Date    : 2017/3/7
*/
#include "BaseUtils.h"

#ifndef CDXMETADATA_H
#define CDXMETADATA_H

#define META_MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define META_MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

typedef struct _Map Map;

enum _META_TITLE_KINDS
{
    ARTIST = 0,
    ALBUM,
    ALBUM_ARTIST,
    TITLE,
    COMPOSER,
    GENRE,
    YEAR,
    AUTHOR,
    CDTRACKNUMBER,
    DISCNUMBER,
    COMPILATION,
    DATE,
};

enum _key_string
{
    CdxMetaKeyAlbum             = META_MKBETAG('a','l','b','u'),  // cstring
    CdxMetaKeyArtist            = META_MKBETAG('a','r','t','i'),  // cstring
    CdxMetaKeyAlbumArtist       = META_MKBETAG('a','a','r','t'),  // cstring
    CdxMetaKeyComposer          = META_MKBETAG('c','o','m','p'),  // cstring
    CdxMetaKeyGenre             = META_MKBETAG('g','e','n','r'),  // cstring
    CdxMetaKeyTitle             = META_MKBETAG('t','i','t','l'),  // cstring
    CdxMetaKeyYear              = META_MKBETAG('y','e','a','r'),  // cstring
    CdxMetaKeyAlbumArt          = META_MKBETAG('a','l','b','A'),  // compressed image data
    CdxMetaKeyAlbumArtMIME      = META_MKBETAG('a','l','A','M'),  // cstring
    CdxMetaKeyAuthor            = META_MKBETAG('a','u','t','h'),  // cstring
    CdxMetaKeyCDTrackNumber     = META_MKBETAG('c','d','t','r'),  // cstring
    CdxMetaKeyDiscNumber        = META_MKBETAG('d','n','u','m'),  // cstring
    CdxMetaKeyDate              = META_MKBETAG('d','a','t','e'),  // cstring
    CdxMetaKeyWriter            = META_MKBETAG('w','r','i','t'),  // cstring
    CdxMetaKeyCompilation       = META_MKBETAG('c','p','i','l'),  // cstring
    CdxMetaKeyLocation          = META_MKBETAG('l','o','c',' '),  // cstring
    CdxMetaKeyTimeScale         = META_MKBETAG('t','m','s','l'),  // cdx_int32
};

typedef enum _META_TITLE_KINDS META_IDX;
typedef enum _key_string CdxMetaKeyString;

struct _Map{
    META_IDX idx;
    CdxMetaKeyString key;
    const char *tag1;
    const char *tag2;
};

void SetMetaData(CdxMediaInfoT *mediaInfo, META_IDX idx, const char* content);

#endif
