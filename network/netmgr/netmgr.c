/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "netmgr.h"

int netmgr_init(void)
{
#ifdef NET_WITH_WIFI
    return netmgr_wifi_init();
#else
#endif
}

void netmgr_deinit(void)
{
#ifdef NET_WITH_WIFI
    netmgr_wifi_deinit();
#else
#endif
}

int netmgr_start(bool autoconfig)
{
#ifdef NET_WITH_WIFI
    return netmgr_wifi_start(autoconfig);
#else
#endif
}
