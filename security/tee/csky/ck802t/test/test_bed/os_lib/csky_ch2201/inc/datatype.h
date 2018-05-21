/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef __DATATYPE_H__
#define __DATATYPE_H__

#include "tee_types.h"

#define CK_INT32    int32_t
#define CK_UINT32   uint32_t
#define CK_UINT16   uint16_t
#define CK_UINT8    uint8_t
#define CK_INT8     int8_t
#define CK_REG      uint32_t
#define BOOL        bool

#define IN
#define OUT
#define INOUT

#define FAILURE -1
#define SUCCESS 0

#ifndef FALSE
#define FALSE               (0)
#endif
#ifndef TRUE
#define TRUE                (1)
#endif

typedef  unsigned int IRQn_Type;

#ifndef __INLINE
#define __INLINE inline
#endif

#endif
