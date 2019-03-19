/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "netmgr.h"

int netmgr_init(void)
{
#ifdef NET_WITH_WIFI
    return netmgr_wifi_init();
#elif defined(NET_WITH_CELLULAR)
    return netmgr_cellular_init();
#else
    return 0;
#endif
}

void netmgr_deinit(void)
{
#ifdef NET_WITH_WIFI
    netmgr_wifi_deinit();
#elif defined(NET_WITH_CELLULAR)
    netmgr_cellular_deinit();
#else
    return 0;
#endif
}

int netmgr_start(bool autoconfig)
{
#ifdef NET_WITH_WIFI
    return netmgr_wifi_start(autoconfig);
#elif defined(NET_WITH_CELLULAR)
    return netmgr_cellular_start(autoconfig);
#else
    return 0;
#endif
}
