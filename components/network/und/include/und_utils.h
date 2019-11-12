/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_UTILS_H
#define UND_UTILS_H

#include "und_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UND_INIT_SANITY_CHECK(param, uninit_val, err) \
    do { \
        if (!!((param) == (uninit_val))) { \
            und_err("%s is not init", #param); \
            return (err); \
        } \
    } while(0)

#define UND_PARAM_RANGE_SANITY_CHECK(param, max, min, err) \
    do { \
        if ((param) > (max) || (param) < (min)) { \
            und_err("Invalid param, out of range"); \
            return (err); \
        } \
    } while(0)

#define UND_PTR_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            und_err("Invalid param, %s = %p", #ptr, ptr); \
            return (err); \
        } \
    } while(0)

#define UND_STR_PTR_SANITY_CHECK(ptr, err) \
    do { \
        UND_PTR_SANITY_CHECK(ptr, err); \
        if (0 == aos_strlen((ptr))) { \
            und_err("Invalid param, %s = '%s'", #ptr, (ptr)); \
            return (err); \
        } \
    } while(0)

#define UND_STR_PTR_MAXLEN_SANITY_CHECK(ptr, maxlen, err) \
    do { \
        UND_STR_PTR_SANITY_CHECK(ptr, err); \
        if (maxlen < aos_strlen((ptr))) { \
            und_err("Invalid param, %s = '%s'", #ptr, (ptr)); \
            return (err); \
        } \
    } while(0)


#ifdef __cplusplus
}
#endif

#endif  /* end of UND_UTILS_H */
