#ifndef __NDS32_MATH_TYPES_H__
#define __NDS32_MATH_TYPES_H__
/***************************************************************************
 * nds32_math_types.h                                                      *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "nds32_intrinsic.h"

typedef int8_t q7_t;
typedef int16_t q15_t;
typedef int32_t q31_t;
typedef int64_t q63_t;
typedef float float32_t;
typedef double float64_t;

#ifdef  __cplusplus
}
#endif
#endif // NDS32_MATH_TYPES_H
