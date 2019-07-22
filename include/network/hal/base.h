/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_BASE_H
#define HAL_BASE_H

#include "aos/list.h"

/*
 * HAL Module define
 */
typedef struct {
    dlist_t     list;
    int         magic;
    const char *name;
    void       *priv_dev; /* Driver may want to describe it */
} hal_module_base_t;

#endif /* HAL_BASE_H */

