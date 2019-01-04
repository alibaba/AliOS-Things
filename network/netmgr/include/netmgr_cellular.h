/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_WIFI_H
#define NETMGR_WIFI_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif

int netmgr_cellular_init(void);
void netmgr_cellular_deinit(void);
int netmgr_cellular_start(bool autoconfig);

bool netmgr_get_ip_state(void);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_WIFI_H */
