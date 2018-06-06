/**
 ****************************************************************************************
 *
 * @file co_status.h
 *
 * @brief Status definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _CO_STATUS_H_
#define _CO_STATUS_H_

/**
 ****************************************************************************************
 * @defgroup CO_STATUS CO_STATUS
 * @ingroup COMMON
 * @brief Status definitions.
 *
 * The STATUS module contains the status definitions used in the stack.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// Status/error codes used in the MAC software.
enum
{
    /// Successful
    CO_OK,
    /// Unuccessful
    CO_FAIL,
    /// Empty
    CO_EMPTY,
    /// Full
    CO_FULL,
    /// Bad parameters supplied
    CO_BAD_PARAM,
    /// Element not found
    CO_NOT_FOUND,
    /// No more element available
    CO_NO_MORE_ELT_AVAILABLE,
    /// No element currently in use
    CO_NO_ELT_IN_USE,
    /// Busy
    CO_BUSY,
    /// Operation already in progress
    CO_OP_IN_PROGRESS,
};

/// @}

#endif // _CO_STATUS_H_
