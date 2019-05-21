/**
 ****************************************************************************************
 *
 * @file ke_config.h
 *
 * @brief This file contains all the constant that can be changed in order to
 * tailor the kernel.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _KE_CONFIG_H_
#define _KE_CONFIG_H_

/**
 ****************************************************************************************
 * @defgroup KERNEL KERNEL
 * @ingroup MACSW
 * @brief Kernel modules.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwnx_config.h"

/*
 * CONSTANT DEFINITIONS
 ****************************************************************************************
 */


/** @name Kernel configuration
 * @{
 * Defines if Kernel heap management is compiled or not
 */

#define KE_MEM_NX       1
#define KE_MEM_LINUX    0
#define KE_MEM_LIBC     0

#define KE_PROFILING    0
/// @}
/// @}

#endif // _KE_CONFIG_H_
