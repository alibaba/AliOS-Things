/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


#ifndef _PRIV_STDINT_H_
#define _PRIV_STDINT_H_
#ifdef __cplusplus
extern "C" {
#endif


/* For newlib and minilibc utint32_t are not same */
#undef _UINT32_T_DECLARED
#define _UINT32_T_DECLARED
typedef unsigned int uint32_t;

#undef _UINT64_T_DECLARED
#define _UINT64_T_DECLARED
typedef unsigned long long  uint64_t;


#undef _INT32_T_DECLARED
#define _INT32_T_DECLARED
typedef signed int int32_t;

#undef _INT64_T_DECLARED
#define _INT64_T_DECLARED
typedef signed long long int64_t;


#include_next <sys/_stdint.h>

#ifdef __cplusplus
}
#endif

#endif
