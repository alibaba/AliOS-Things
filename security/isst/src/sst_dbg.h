/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_DBG_H_
#define _SST_DBG_H_

#include "stdint.h"
#include "stdio.h"
#include "sst_osa.h"

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)

#define SST_TAG "SST"
#define SST_ERR(_f, _a ...) sst_printf("SST %s %d: "_f, __FUNCTION__, __LINE__, ##_a)

#define SST_VER(_f, _a ...) sst_printf("SST %s %d: "_f, __FUNCTION__, __LINE__, ##_a)

#ifdef CONFIG_SST_DBG
#define SST_INF(_f, _a ...) sst_printf("SST %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#define SST_INF(_f, _a ...)
#endif

#endif /* _SST_DBG_H_ */

