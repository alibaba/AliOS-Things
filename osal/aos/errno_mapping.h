/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ERRNO_MAPPING_H
#define ERRNO_MAPPING_H

#include <errno.h>
#include <aos/aos.h>

#define ERRNO_MAPPING(ret)                           \
    do{                                              \
        return ret;                                  \
    }while(0)

#endif /* ERRNO_MAPPING_H */
