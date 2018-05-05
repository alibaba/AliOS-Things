/**
 ****************************************************************************************
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/**
 ****************************************************************************************
 * @defgroup PLATFORM_DRIVERS PLATFORM_DRIVERS
 * @ingroup MACSW
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup ARCH_AT91SAM9261 ARCH_AT91SAM9261
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// required to define GLOBAL_INT_** macros as inline assembly
#include "boot.h"
#include "ll.h"
#include "compiler.h"
#include "rwnx_config.h"
#include "generic.h"

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE   4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// ARM is little endian
#define CPU_LE                          1

#define ASSERT_REC(cond)
#define ASSERT_REC_VAL(cond, ret)
#define ASSERT_REC_NO_RET(cond)

#define ASSERT_ERR(cond)                  ASSERT(cond)
#define ASSERT_ERR2(cond, param0, param1)
#define ASSERT_WARN(cond)

/// @}
/// @}
#endif // _ARCH_H_
