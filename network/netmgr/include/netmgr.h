/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_H
#define NETMGR_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdbool.h>

#ifdef NET_WITH_WIFI
#include "netmgr_wifi.h"
#elif defined(NET_WITH_CELLULAR)
#include "netmgr_cellular.h"
#endif

int netmgr_init(void);
void netmgr_deinit(void);
int netmgr_start(bool autoconfig);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_H */
