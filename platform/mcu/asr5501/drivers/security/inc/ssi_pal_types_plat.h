/****************************************************************************
* This confidential and proprietary software may be used only as authorized *
* by a licensing agreement from ARM Israel.                                 *
* Copyright (C) 2015 ARM Limited or its affiliates. All rights reserved.    *
* The entire notice above must be reproduced on all authorized copies and   *
* copies may only be made to the extent permitted by a licensing agreement  *
* from ARM Israel.                                                          *
*****************************************************************************/

/*! @file
@brief This file contains basic type definitions that are platform dependent.
*/
#ifndef SSI_PAL_TYPES_PLAT_H
#define SSI_PAL_TYPES_PLAT_H
/* Host specific types for standard (ISO-C99) compilant platforms */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uintptr_t        SaSiVirtAddr_t;
typedef uint32_t            SaSiBool_t;
typedef uint32_t            SaSiStatus;

#define SaSiError_t         SaSiStatus
#define SASI_INFINITE         0xFFFFFFFF

#define CEXPORT_C
#define CIMPORT_C

#endif /*SSI_PAL_TYPES_PLAT_H*/
