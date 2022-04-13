//
// jquick_network.h
//
// Created by liujuncheng@alibaba-inc.com on 2020/11/13.
//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#ifndef __APPX_JQUICK_WIFI_H__
#define __APPX_JQUICK_WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

int jquick_network_is_connect();

long jquick_network_download_url_to_file(const char* url, const char* filePath);

#ifdef __cplusplus
}
#endif

#endif  // __APPX_JQUICK_WIFI_H__
