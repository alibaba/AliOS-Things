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
    /* system info */
    /* type, eg: rtos, linux */
    const char *os;
    /* type, eg: solo, sub, gw */
    const char *type;
    /* partner name, eg: tmall_genie */
    const char *partner;
    /* app protocol, eg: http, lwm2m */
    const char *app_net;
    /* project name, eg: tmall_genie*/
    const char *project;
    /* cloud name, eg: aliyun or others */
    const char *cloud;
} hal_module_base_t;

#endif /* HAL_BASE_H */

