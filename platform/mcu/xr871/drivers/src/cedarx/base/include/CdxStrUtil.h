/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxStrUtil.h
 * Description : StrUtil
 * History :
 *
 */

#ifndef CDX_STR_UTIL_H
#define CDX_STR_UTIL_H

#include <CdxTypes.h>
#include <CdxList.h>
#include <AwPool.h>

struct CdxStrItemS
{
    CdxListNodeT node;
    cdx_char *val;
};

#ifdef __cplusplus
extern "C"
{
#endif

void CdxStrTrimTail(cdx_char *str);

void CdxStrTrimHead(cdx_char *str);

void CdxStrTrim(cdx_char *str);

void CdxStrTolower(cdx_char *str);

cdx_char *CdxStrAttributeOfKey(AwPoolT *pool, const cdx_char *str, cdx_char *key, cdx_char sepa);

cdx_uint32 CdxStrSplit(AwPoolT *pool, cdx_char *str, char sepa, CdxListT *itemList);

#ifdef __cplusplus
}
#endif

#endif
