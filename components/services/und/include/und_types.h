/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_TYPES_H
#define UND_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UND_PARAM_ERR = -3,
    UND_MEM_ERR = -2,
    UND_ERR = -1,
    UND_SUCCESS = 0,
};

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_TYPES_H */
