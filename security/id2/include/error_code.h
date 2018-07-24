/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        IROT_SUCCESS              = 0,  ///< The operation was successful.
        IROT_ERROR_GENERIC        = -1, ///< Non-specific casuse.
        IROT_ERROR_BAD_PARAMETERS = -2, ///< Input parameters were invlid.
        IROT_ERROR_SHORT_BUFFER =
          -3, ///< The supplied buffer is too short for the output.
        IROT_ERROR_EXCESS_DATA =
          -4, ///< Too much data for the requested operation was passed.
        IROT_ERROR_OUT_OF_MEMORY = -5, ///< System out of memory resources.
        IROT_ERROR_COMMUNICATION = -7, ///< Communication error
        IROT_ERROR_NOT_SUPPORTED =
          -8, ///< The request operation is valid but is not supported in this
              ///< implementation.
        IROT_ERROR_NOT_IMPLEMENTED =
          -9, ///< The requested operation should exist but is not yet
              ///< implementation.
        IROT_ERROR_TIMEOUT        = -10, ///< Communication Timeout
        IROT_ERROR_ITEM_NOT_FOUND = -11, ///< Id2 is not exist
    } irot_result_t;

#ifdef __cplusplus
}
#endif

#endif
