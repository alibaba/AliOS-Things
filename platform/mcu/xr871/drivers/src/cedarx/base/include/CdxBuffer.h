/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxBuffer.h
 * Description : Buffer
 * History :
 *
 */

#ifndef CDX_BUFFER_H
#define CDX_BUFFER_H
#include <CdxTypes.h>
#include <cdx_log.h>
#include <CdxMeta.h>
#include <CdxList.h>
#include <string.h>

/*
  *下面函数，返回失败的时候，对应的buf 还是合法，
  *可以继续使用，当然也要正常释放。
  */

struct CdxBufferItemS
{
    CdxBufferT *val;
    CdxListNodeT node;
};

struct CdxBufferOpsS
{
    CdxBufferT *(*incRef)(CdxBufferT *);
    void (*decRef)(CdxBufferT *);
    cdx_uint8 *(*getData)(CdxBufferT *);
    cdx_uint8 *(*getBase)(CdxBufferT *);
    cdx_uint32 (*getSize)(CdxBufferT *);
    void (*append)(CdxBufferT *, const cdx_uint8 * /*data*/, cdx_uint32 /*len*/ );
    void (*insert)(CdxBufferT *, const cdx_uint8 * /*data*/, cdx_uint32 /*len*/ );
    void (*setRange)(CdxBufferT *, cdx_uint32 /*offset*/, cdx_uint32/*len*/);
    void (*seekRange)(CdxBufferT *, cdx_int32 /*left*/, cdx_int32/*right*/);
    CdxMetaT *(*getMeta)(CdxBufferT *);
};

struct CdxBufferS
{
    struct CdxBufferOpsS *ops;
};

static inline CdxBufferT *CdxBufferIncRef(CdxBufferT *buf)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->incRef);
    return buf->ops->incRef(buf);
}

static inline void CdxBufferDecRef(CdxBufferT *buf)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->decRef);
    return buf->ops->decRef(buf);
}

static inline cdx_uint8 *CdxBufferGetData(CdxBufferT *buf)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->getData);
    return buf->ops->getData(buf);
}

static inline cdx_uint8 *CdxBufferGetBase(CdxBufferT *buf)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->getBase);
    return buf->ops->getBase(buf);
}

static inline cdx_uint32 CdxBufferGetSize(CdxBufferT *buf)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->getSize);
    return buf->ops->getSize(buf);
}

static inline void CdxBufferAppend(CdxBufferT *buf, const cdx_uint8 *data,
                                    cdx_uint32 len)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->append);
    buf->ops->append(buf, data, len);
}

static inline void CdxBufferInsert(CdxBufferT *buf,
                                    cdx_uint8 *data, cdx_uint32 len)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->insert);
    buf->ops->insert(buf, data, len);
}

static inline CdxMetaT *CdxBufferGetMeta(CdxBufferT *buf)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->getMeta);
    return buf->ops->getMeta(buf);
}

static inline void CdxBufferSetRange(CdxBufferT *buf,
                                    cdx_uint32 offset, cdx_uint32 len)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->setRange);
    buf->ops->setRange(buf, offset, len);
}

static inline void CdxBufferSeekRange(CdxBufferT *buf,
                                    cdx_int32 left, cdx_int32 right)
{
    CDX_CHECK(buf);
    CDX_CHECK(buf->ops);
    CDX_CHECK(buf->ops->seekRange);
    buf->ops->seekRange(buf, left, right);
}

#define CdxBufferDump(buf) \
    CDX_BUF_DUMP(CdxBufferGetData(buf), CdxBufferGetSize(buf))

#define CdxBufferSetInt32(buf, name, val) \
    CdxMetaSetInt32(CdxBufferGetMeta(buf), name, val)

#define CdxBufferFindInt32(buf, name, pVal) \
    CdxMetaFindInt32(CdxBufferGetMeta(buf), name, pVal)

#define CdxBufferRemoveInt32(buf, name) \
    CdxMetaRemoveInt32(CdxBufferGetMeta(buf), name)

#define CdxBufferSetInt64(buf, name, val) \
    CdxMetaSetInt64(CdxBufferGetMeta(buf), name, val)

#define CdxBufferFindInt64(buf, name, pVal) \
    CdxMetaFindInt64(CdxBufferGetMeta(buf), name, pVal)

typedef CdxBufferT CdxStringT;

#define CdxStringCreate(pool) \
    CdxBufferCreate(pool, 1024, NULL, 0)

#define CdxStringDestroy(str)   \
    CdxBufferDestroy(str)

static inline void CdxStringAppend(CdxStringT *str, cdx_char *val)
{
    CDX_CHECK(val);
    CdxBufferAppend(str, (cdx_uint8 *)(val), strlen(val) + 1);
    CdxBufferSetRange(str, 0, CdxBufferGetSize(str) - 1);
}

#define CdxStringGet(str) \
    (cdx_char *)CdxBufferGetData(str)

#define CdxBufferCreate(pool, capacity, initData, len) \
    __CdxBufferCreate(pool, capacity, initData, len, __FILE__, __LINE__)

#ifdef __cplusplus
extern "C" {
#endif

CdxBufferT *__CdxBufferCreate(AwPoolT *pool, cdx_uint32 capacity, cdx_uint8 *initData,
                            cdx_uint32 len, char *file, int line);

void CdxBufferDestroy(CdxBufferT *buf);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // CDX_BUFFER_H
