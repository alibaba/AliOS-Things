/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMessage.h
 * Description : Message
 * History :
 *
 */

#ifndef CDX_MESSAGE_H
#define CDX_MESSAGE_H
#include <CdxTypes.h>
#include <cdx_log.h>
#include <CdxMeta.h>

typedef struct CdxMessageS CdxMessageT;
typedef struct CdxHandlerS CdxHandlerT;
typedef struct CdxDeliverS CdxDeliverT;

struct CdxDeliverOpsS
{
    cdx_int32 (*postUS)(CdxDeliverT *, CdxMessageT *, cdx_uint64/*timeUs*/);
};

struct CdxDeliverS
{
    struct CdxDeliverOpsS *ops;
};

CdxDeliverT *CdxDeliverCreate();

void CdxDeliverDestroy(CdxDeliverT *deliver);

void CdxDeliverReset(void);

static inline cdx_int32 CdxDeliverPostUs(CdxDeliverT *deliver, CdxMessageT *msg,
                                        cdx_uint64 timeUs)
{
    CDX_CHECK(deliver);
    CDX_CHECK(deliver->ops);
    CDX_CHECK(deliver->ops->postUS);
    return deliver->ops->postUS(deliver, msg, timeUs);
}

/**
 *目前msg的场景都是串行的，后续有串行的需要再
 *考虑加锁
 */

/* sizeof(name) < 32 */
struct CdxMessageOpsS
{
    CdxMessageT *(*incRef)(CdxMessageT *);

    void (*decRef)(CdxMessageT *);

    CdxMetaT *(*getMeta)(CdxMessageT *);

    cdx_int32 (*what)(CdxMessageT *);

    cdx_err (*postUs)(CdxMessageT *, cdx_uint64); /* asynchronous */

    CdxMessageT *(*dup)(CdxMessageT *);

    void (*deliver)(CdxMessageT *);
};

struct CdxMessageS
{
    struct CdxMessageOpsS *ops;
};

static inline CdxMessageT *CdxMessageIncRef(CdxMessageT *msg)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->incRef);
    return msg->ops->incRef(msg);
}

static inline void CdxMessageDecRef(CdxMessageT *msg)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->decRef);
    return msg->ops->decRef(msg);
}

static inline CdxMetaT *CdxMessageGetMeta(CdxMessageT *msg)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->getMeta);
    return msg->ops->getMeta(msg);
}

static inline void CdxMessageDeliver(CdxMessageT *msg)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->deliver);
    return msg->ops->deliver(msg);
}

#define CdxMessageFastNotify(pool, what, hdr)                             \
    do                                                          \
    {                                                           \
        CdxMessageT *_notifyMsg = CdxMessageCreate(pool, what, hdr);  \
        CDX_FORCE_CHECK(_notifyMsg);                            \
        CdxMessagePost(_notifyMsg);                             \
    }while (0)

#define CdxMessageSetInt32(msg, name, val) \
    CdxMetaSetInt32(CdxMessageGetMeta(msg), name, val)

#define CdxMessageFindInt32(msg, name, pVal) \
    CdxMetaFindInt32(CdxMessageGetMeta(msg), name, pVal)

#define CdxMessageSetInt64(msg, name, val) \
    CdxMetaSetInt64(CdxMessageGetMeta(msg), name, val)

#define CdxMessageFindInt64(msg, name, pVal) \
    CdxMetaFindInt64(CdxMessageGetMeta(msg), name, pVal)

#define CdxMessageSetString(msg, name, val) \
    CdxMetaSetString(CdxMessageGetMeta(msg), name, val)

#define CdxMessageFindString(msg, name, pVal) \
    CdxMetaFindString(CdxMessageGetMeta(msg), name, pVal)

#define CdxMessageSetObject(msg, name, val) \
    CdxMetaSetObject(CdxMessageGetMeta(msg), name, val)

#define CdxMessageFindObject(msg, name, pVal) \
    CdxMetaFindObject(CdxMessageGetMeta(msg), name, (void **)(pVal))

#define CdxMessageRemoveObject(msg, name) \
    CdxMetaRemoveObject(CdxMessageGetMeta(msg), name)

#define CdxMessageClearMeta(msg) \
    CdxMetaClear(CdxMessageGetMeta(msg))

static inline cdx_int32 CdxMessageWhat(CdxMessageT *msg)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->what);
    return msg->ops->what(msg);
}

static inline cdx_err CdxMessagePostUs(CdxMessageT *msg, cdx_uint64 timeOut)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->postUs);
    return msg->ops->postUs(msg, timeOut);
}

static inline CdxMessageT *CdxMessageDup(CdxMessageT *msg)
{
    CDX_CHECK(msg);
    CDX_CHECK(msg->ops);
    CDX_CHECK(msg->ops->dup);
    return msg->ops->dup(msg);
}

#define CdxMessagePost(msg) \
    CdxMessagePostUs(msg, 0)

#ifdef __cplusplus
extern "C" {
#endif
#define CdxMessageCreate(pool, what, handler) \
    __CdxMessageCreate(pool, what, handler, __FILE__, __LINE__)

CdxMessageT *__CdxMessageCreate(AwPoolT *pool, cdx_int32 what, CdxHandlerT *handler,
                            char *file, int line);

void CdxMessageDestroy(CdxMessageT  *msg);

/*-----------------------------msg handler-----------------------------*/
typedef struct CdxHandlerItfS CdxHandlerItfT;

struct CdxHandlerItfOpsS
{
    void (*msgRecv)(CdxHandlerItfT *, CdxMessageT *);
};

struct CdxHandlerItfS
{
    struct CdxHandlerItfOpsS *ops;
};

struct CdxHandlerOpsS
{
    cdx_int32 (*postUs)(CdxHandlerT *, CdxMessageT *, cdx_uint64);

    void (*msgRecv)(CdxHandlerT *, CdxMessageT *);
};

struct CdxHandlerS
{
    struct CdxHandlerOpsS *ops;
};

static inline cdx_int32 CdxHandlerPostUs(CdxHandlerT *hdr, CdxMessageT *msg,
                                        cdx_uint64 timeUs)
{
    CDX_CHECK(hdr);
    CDX_CHECK(hdr->ops);
    return hdr->ops->postUs(hdr, msg, timeUs);
}

static inline void CdxHandlerMsgRecv(CdxHandlerT *hdr, CdxMessageT *msg)
{
    CDX_CHECK(hdr);
    CDX_CHECK(hdr->ops);
    return hdr->ops->msgRecv(hdr, msg);
}

CdxHandlerT *CdxHandlerCreate(AwPoolT *pool, CdxHandlerItfT *itf, CdxDeliverT *deliver);

void CdxHandlerDestroy(CdxHandlerT *hdr);

/*--------------------------msg handler end----------------------------*/

#ifdef __cplusplus
}
#endif

#endif  // A_MESSAGE_H_
