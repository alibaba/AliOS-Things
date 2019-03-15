/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxWriter.h
 * Description : Allwinner Muxer Writer Definition
 * History :
 *
 */

#ifndef __CDX_WRITER_H__
#define __CDX_WRITER_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cdx_log.h>

typedef struct CdxWriterS CdxWriterT;
struct CdxWriterOps{
    int (*read)(CdxWriterT *thiz, void *buf, int size);
    int (*write)(CdxWriterT *thiz, void *buf, int size);
    long (*seek)(CdxWriterT *thiz, long moffset, int mwhere);
    long (*tell)(CdxWriterT *thiz);
    int  (*close)(CdxWriterT *thiz);
};

struct CdxWriterS
{
    struct CdxWriterOps* ops;
    char*  uri;
};

static inline int CdxWriterRead(CdxWriterT *w, void *buf, int size)
{
    CDX_CHECK(w);
    CDX_CHECK(w->ops);
    CDX_CHECK(w->ops->read);
    return w->ops->read(w, buf, size);
}

static inline int CdxWriterWrite(CdxWriterT *w, void *buf, int size)
{
    CDX_CHECK(w);
    CDX_CHECK(w->ops);
    CDX_CHECK(w->ops->write);
    return w->ops->write(w, buf, size);
}

static inline int CdxWriterSeek(CdxWriterT *w, long moffset, int mwhere)
{
    CDX_CHECK(w);
    CDX_CHECK(w->ops);
    CDX_CHECK(w->ops->seek);
    return w->ops->seek(w, moffset, mwhere);
}

static inline int CdxWriterTell(CdxWriterT *w)
{
    CDX_CHECK(w);
    CDX_CHECK(w->ops);
    CDX_CHECK(w->ops->tell);
    return w->ops->tell(w);
}

static inline int CdxWriterClose(CdxWriterT *w)
{
    CDX_CHECK(w);
    CDX_CHECK(w->ops);
    CDX_CHECK(w->ops->close);
    return w->ops->close(w);
}


//CdxWriterT *CdxWriterCreat();
void CdxWriterDestroy(CdxWriterT *writer);

#endif
