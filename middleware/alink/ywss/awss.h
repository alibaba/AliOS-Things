/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AWSS_H_
#define _AWSS_H_

extern char *zc_default_ssid;
extern char *zc_default_passwd;

#ifdef CONFIG_YWSS
#define DEFAULT_SSID    zc_default_ssid
#define DEFAULT_PASSWD  zc_default_passwd
#else
#define DEFAULT_SSID    ""
#define DEFAULT_PASSWD  ""
#endif

#define WLAN_CONNECTION_TIMEOUT_MS      (30 * 1000)

#endif
