/*!
 * @file sal_arch.h
 *
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SAL_ARCH_H
#define SAL_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_sal API
 *  AT parser, which is used to ease the operation of AT modules.
 *
 *  @{
 */

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

/**
 * Define generic types used in sal.
 */
#if !SAL_NO_STDINT_H
#include <stdint.h>
typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;
#endif

/** @} */ /* end of group aos_sal */

#ifdef __cplusplus
}
#endif

#endif
