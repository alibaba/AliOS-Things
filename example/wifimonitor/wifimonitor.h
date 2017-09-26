/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/network.h>

#ifndef _WIFI_MONITOR_H_
#define _WIFI_MONITOR_H_

#define WIFI_JSON_SIGN "\"type\":\"wifimonitor\","

#ifndef bool
#define bool unsigned char
#endif

void wifimonitor_event_handler(input_event_t *event, void *priv_data);
void wifimonitor_start(bool automatic);

#endif // _WIFI_MONITOR_H_
