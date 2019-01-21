/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#define TFormat_printf(...)  \
    do {                                                     \
        printf("\e[0;32m%s@line%d:\t", __FUNCTION__, __LINE__);  \
        printf(__VA_ARGS__);                                 \
        printf("\e[0m");                                   \
    } while (0)

#endif
