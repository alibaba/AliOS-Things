/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_DBG_H_
#define _TEE_DBG_H_

#include "tee_types.h"
#include "tee_os_dep.h"

enum
{
    ERR = 0, /* error */
    WRN = 1, /* warning */
    INF = 2, /* information */
};

#define tee_dbg_print(_l, _f, _a...)    \
    do {                                \
        printk((const char *)_f, ##_a); \
    } while (0)

#define TEE_ASSERT(_x)                                                    \
    do {                                                                  \
        if (!(_x)) {                                                      \
            tee_dbg_print(ERR, (const int8_t *)"ASSERT FAILURE:\n");      \
            tee_dbg_print(ERR, (const int8_t *)"%s (%d): %s\n", __FILE__, \
                          __LINE__, __FUNCTION__);                        \
            while (1) /* loop */                                          \
                ;                                                         \
        }                                                                 \
    } while (0)

#endif /* _TEE_DBG_H_ */
