/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#ifndef ALI_CRYPTO_OSA_H
#define ALI_CRYPTO_OSA_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ALI_CRYPTO_PLAT_AOS
#define osa_malloc(_size)       aos_malloc(_size)
#define osa_free(_ptr)          aos_free(_ptr)
#define osa_calloc(_n, _size)   aos_calloc(_n, _size)
#else
#define osa_malloc(_size)       malloc(_size)
#define osa_free(_ptr)          free(_ptr)
#define osa_calloc(_n, _size)   calloc(_n, _size)
#endif


#define osa_memcpy(_dst, _src, _size) memcpy(_dst, _src, _size)
#define osa_memset(_src, _val, _size) memset(_src, _val, _size)
#define osa_memcmp(_dst, _src, _size) memcmp(_dst, _src, _size)
#define osa_strlen(_str)              strlen(_str)

#ifdef ALI_CRYPTO_PLATFORM_C

#else
#define osa_printf                    printf
#endif


#ifdef __cplusplus
}
#endif

#endif /* ALI_CRYPTO_OSA_H*/
