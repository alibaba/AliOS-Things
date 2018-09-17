/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __SDK_IMPL_INTERNAL__
#define __SDK_IMPL_INTERNAL__
#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_import.h"
#include "iot_export.h"

//#include "lite-log.h"
//#include "lite-utils.h"
//#include "guider.h"

#define POINTER_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            log_err("Invalid argument, %s = %p", #ptr, ptr); \
            return (err); \
        } \
    } while(0)

#define STRING_PTR_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            log_err("Invalid argument, %s = %p", #ptr, (ptr)); \
            return (err); \
        } \
        if (0 == strlen((ptr))) { \
            log_err("Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return (err); \
        } \
    } while(0)

#if defined(__cplusplus)
}
#endif
#endif  /* __SDK_IMPL_INTERNAL__ */
