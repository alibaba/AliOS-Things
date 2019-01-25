/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#ifndef ALI_CRYPTO_OSA_H
#define ALI_CRYPTO_OSA_H
#include "config.h"
#include "ls_osa.h"

#ifdef __cplusplus
extern "C" {
#endif

#define osa_malloc(_size)                ls_osa_malloc(_size)
#define osa_free(_ptr)                   ls_osa_free(_ptr)
#define osa_calloc(_n, _size)            ls_osa_calloc(_n, _size)

#define osa_memcpy(_dst, _src, _size)    memcpy(_dst, _src, _size)
#define osa_memset(_src, _val, _size)    memset(_src, _val, _size)
#define osa_memcmp(_dst, _src, _size)    memcmp(_dst, _src, _size)
#define osa_strlen(_str)                 strlen(_str)

#define osa_printf                       ls_osa_print
#define osa_snprintf                     ls_osa_snprintf

#ifdef __cplusplus
}
#endif

#endif /* ALI_CRYPTO_OSA_H*/
