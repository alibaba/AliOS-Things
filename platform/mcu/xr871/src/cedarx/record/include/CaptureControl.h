#ifndef CAPTURE_CONTROL_H
#define CAPTURE_CONTROL_H
#include "cdx_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int                      nChannels;
    int                      nSamplerate;
    int                      nBitpersample;
} CdxCapbkCfg;

typedef struct CaptureCtrl CaptureCtrl;

struct CaptureControlOpsS
{
    void (*destroy)(CaptureCtrl* c);
    void (*setFormat)(CaptureCtrl* c, CdxCapbkCfg* cfg);
    int (*start)(CaptureCtrl* c);
    int (*stop)(CaptureCtrl* c);
    int (*read)(CaptureCtrl* c, void* pData, int nDataSize);
};

struct CaptureCtrl
{
    struct CaptureControlOpsS* ops;
};

CaptureCtrl* CaptureDeviceCreate();

static inline void CaptureDeviceDestroy(CaptureCtrl* c)
{
    CDX_CHECK(c);
    CDX_CHECK(c->ops);
    CDX_CHECK(c->ops->destroy);
    return c->ops->destroy(c);
}

static inline void CaptureDeviceSetFormat(CaptureCtrl* c, CdxCapbkCfg* cfg)
{
    CDX_CHECK(c);
    CDX_CHECK(c->ops);
    CDX_CHECK(c->ops->setFormat);
    return c->ops->setFormat(c, cfg);
}

static inline int CaptureDeviceStart(CaptureCtrl* c)
{
    CDX_CHECK(c);
    CDX_CHECK(c->ops);
    CDX_CHECK(c->ops->start);
    return c->ops->start(c);
}

static inline int CaptureDeviceStop(CaptureCtrl* c)
{
    CDX_CHECK(c);
    CDX_CHECK(c->ops);
    CDX_CHECK(c->ops->stop);
    return c->ops->stop(c);
}

static inline int CaptureDeviceRead(CaptureCtrl* c, void* pData, int nDataSize)
{
    CDX_CHECK(c);
    CDX_CHECK(c->ops);
    CDX_CHECK(c->ops->read);
    return c->ops->read(c, pData, nDataSize);
}

#ifdef __cplusplus
}
#endif

#endif
