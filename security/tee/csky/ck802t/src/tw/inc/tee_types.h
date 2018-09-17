/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_TYPES_H_
#define _TEE_TYPES_H_

#ifndef __STDINT__

typedef char          int8_t;
typedef unsigned char uint8_t;

typedef short          int16_t;
typedef unsigned short uint16_t;

typedef int          int32_t;
typedef unsigned int uint32_t;

typedef long long          int64_t;
typedef unsigned long long uint64_t;

typedef signed long int ssize_t;
typedef unsigned long   size_t;

typedef long          long_t;
typedef unsigned long ulong_t;

#endif /* __STDINT__ */

typedef unsigned char bool;

typedef int32_t tee_stat_t;

#ifndef false
#define false (0)
#endif

#ifndef true
#define true (1)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

/* EXPORT_SYMBOL is used to export the available symbols to TAs */
#define EXPORT_SYMBOL(_symbol_)

#endif /* _TEE_TYPES_H_ */
