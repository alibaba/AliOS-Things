/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMeta.h
 * Description : Meta
 * History :
 *
 */

#ifndef CDX_META_H
#define CDX_META_H
#include <CdxTypes.h>
#include <cdx_log.h>
#include <AwPool.h>

#define CDX_META_ITEM_NAMESIZE 32

//typedef void (*pfObjDtor)(void *);
typedef struct CdxMetaS CdxMetaT;

/* sizeof(name) < 32 */
struct CdxMetaOpsS
{
    CdxMetaT *(*incRef)(CdxMetaT *);
    void (*decRef)(CdxMetaT *);

    cdx_err (*setInt32)(CdxMetaT *, cdx_char * /*name*/, cdx_int32);
    cdx_err (*findInt32)(CdxMetaT *, cdx_char * /*name*/, cdx_int32 *);
    cdx_err (*removeInt32)(CdxMetaT *, cdx_char * /*name*/);

    cdx_err (*setInt64)(CdxMetaT *, cdx_char * /*name*/, cdx_int64);
    cdx_err (*findInt64)(CdxMetaT *, cdx_char * /*name*/, cdx_int64 *);
    cdx_err (*removeInt64)(CdxMetaT *, cdx_char * /*name*/);

    cdx_err (*setString)(CdxMetaT *, cdx_char * /*name*/, cdx_char *);
    cdx_err (*findString)(CdxMetaT *, cdx_char * /*name*/, cdx_char **);
    cdx_err (*appendString)(CdxMetaT *, cdx_char * /*name*/, cdx_char *);
    cdx_err (*removeString)(CdxMetaT *, cdx_char * /*name*/);

    cdx_err (*setData)(CdxMetaT *, cdx_char * /*name*/, cdx_uint8 *, cdx_uint32);
    cdx_err (*findData)(CdxMetaT *, cdx_char * /*name*/, cdx_uint8 **, cdx_uint32*);
    cdx_err (*removeData)(CdxMetaT *, cdx_char * /*name*/);

    cdx_err (*setObject)(CdxMetaT *, cdx_char * /*name*/, void *);
    cdx_err (*findObject)(CdxMetaT *, cdx_char * /*name*/, void **);
    cdx_err (*removeObject)(CdxMetaT *, cdx_char * /*name*/);

    CdxMetaT *(*dup)(CdxMetaT *);
    void (*clear)(CdxMetaT *);
};

struct CdxMetaS
{
    struct CdxMetaOpsS *ops;
};

static inline CdxMetaT *CdxMetaIncRef(CdxMetaT  *meta)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->incRef);
    return meta->ops->incRef(meta);
}

static inline void CdxMetaDecRef(CdxMetaT  *meta)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->decRef);
    return meta->ops->decRef(meta);
}

static inline cdx_err CdxMetaSetInt32(CdxMetaT  *meta,
                                cdx_char *name, cdx_int32 val)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->setInt32);
    return meta->ops->setInt32(meta, name, val);
}

static inline cdx_err CdxMetaFindInt32(CdxMetaT  *meta,
                                cdx_char *name, cdx_int32 *pVal)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->findInt32);
    return meta->ops->findInt32(meta, name, pVal);
}

static inline cdx_err CdxMetaRemoveInt32(CdxMetaT  *meta, cdx_char *name)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->removeInt32);
    return meta->ops->removeInt32(meta, name);
}

static inline cdx_err CdxMetaSetInt64(CdxMetaT  *meta,
                                cdx_char *name, cdx_int64 val)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->setInt64);
    return meta->ops->setInt64(meta, name, val);
}

static inline cdx_err CdxMetaFindInt64(CdxMetaT  *meta,
                                cdx_char *name, cdx_int64 *pVal)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->findInt64);
    return meta->ops->findInt64(meta, name, pVal);
}

static inline cdx_err CdxMetaSetString(CdxMetaT  *meta,
                                cdx_char *name, cdx_char *val)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->setString);
    return meta->ops->setString(meta, name, val);
}

/**
 *通过入参获得所需要的string
 *用户需要自行调用CdxFree()释放空间
 */
static inline cdx_err CdxMetaFindString(CdxMetaT  *meta,
                                cdx_char *name, cdx_char **pVal)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->findString);
    return meta->ops->findString(meta, name, pVal);
}

static inline cdx_err CdxMetaAppendString(CdxMetaT  *meta,
                                cdx_char *name, cdx_char *val)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->appendString);
    return meta->ops->appendString(meta, name, val);
}

static inline cdx_err CdxMetaSetData(CdxMetaT  *meta, cdx_char *name,
                                    cdx_uint8 *val, cdx_uint32 size)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->setData);
    return meta->ops->setData(meta, name, val, size);
}

/**
 *通过入参获得所需要的string
 *用户需要自行调用CdxFree()释放空间
 */
static inline cdx_err CdxMetaFindData(CdxMetaT  *meta, cdx_char *name,
                                    cdx_uint8 **pVal, cdx_uint32 *pSize)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->findData);
    return meta->ops->findData(meta, name, pVal, pSize);
}

static inline cdx_err CdxMetaSetObject(CdxMetaT *meta, cdx_char *name, void *val)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->setObject);
    return meta->ops->setObject(meta, name, val);
}

/**
 *通过入参获得需要的对象的句柄
 *meta生命周期内,对象是有效，
 *需要长期保存使用的话请自行做引用计数+1
 *或者保存副本
 */
static inline cdx_err CdxMetaFindObject(CdxMetaT  *meta,
                                cdx_char *name, void **pVal)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->findObject);
    return meta->ops->findObject(meta, name, pVal);
}

static inline cdx_err CdxMetaRemoveObject(CdxMetaT *meta, cdx_char *name)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->removeObject);
    return meta->ops->removeObject(meta, name);
}

static inline CdxMetaT *CdxMetaDup(CdxMetaT *meta)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->dup);
    return meta->ops->dup(meta);
}

static inline void CdxMetaClear(CdxMetaT *meta)
{
    CDX_CHECK(meta);
    CDX_CHECK(meta->ops);
    CDX_CHECK(meta->ops->clear);
    meta->ops->clear(meta);
    return ;
}

#ifdef  __cplusplus
extern "C"
{
#endif

#define CdxMetaCreate(pool) \
    __CdxMetaCreate(pool, __FILE__, __LINE__)

CdxMetaT *__CdxMetaCreate(AwPoolT *pool, char *file, int line);

void CdxMetaDestroy(CdxMetaT *meta);

#ifdef __cplusplus
}
#endif

#endif  // A_MESSAGE_H_
