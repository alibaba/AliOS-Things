/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_WIFI_HAL_H
#define UR_WIFI_HAL_H

enum {
    WIFI_DISCOVERY_TIMEOUT           = 75,     /* ms */
    WIFI_AUTH_REQUEST_TIMEOUT        = 3000,   /* ms */
    WIFI_AUTH_RELAY_TIMEOUT          = 3000,   /* ms */
    WIFI_AUTH_RESPONSE_TIMEOUT       = 3000,   /* ms */
    WIFI_LINK_QUALITY_MOBILE_TIMEOUT = 1000,   /* ms */
    WIFI_LINK_QUALITY_TIMEOUT        = 30000,  /* ms */
#ifndef CONFIG_AOS_DDA
#ifdef CONFIG_AOS_MESH_BCAST
    WIFI_ADVERTISEMENT_TIMEOUT       = 5000,   /* ms, 5 seconds */
#else
    WIFI_ADVERTISEMENT_TIMEOUT       = 20000,  /* ms, 20 seconds */
#endif
    WIFI_NEIGHBOR_ALIVE_TIMEOUT      = 240000, /* ms, 4 mins */
#else
    WIFI_ADVERTISEMENT_TIMEOUT       = 4000,   /* ms, 4 seconds */
    WIFI_NEIGHBOR_ALIVE_TIMEOUT      = 24000,  /* ms, 24 seconds */
#endif
    WIFI_NOTIFICATION_TIMEOUT        = 60000,  /* ms, 1 mins */
    WIFI_ADDR_CACHE_ALIVE_TIMEOUT    = 3,
    WIFI_LEADER_DELAY_TIMEOUT        = 750,    /* ms */
};

#endif  /* UR_WIFI_HAL_H */
