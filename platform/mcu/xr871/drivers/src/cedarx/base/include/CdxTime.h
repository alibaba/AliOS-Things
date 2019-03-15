/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxTime.h
 * Description : Time
 * History :
 *
 */

#ifndef CDX_TIME_H
#define CDX_TIME_H

#include <CdxTypes.h>

/* Wall Clock: it's not monotonic and may have discontinuities */
CDX_API cdx_int64 CdxGetNowUs(void);

/* Monotonic clock */
CDX_API cdx_int64 CdxMonoTimeUs(void);

#endif
