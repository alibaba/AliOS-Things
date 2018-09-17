/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _AWSS_MAIN_H_
#define _AWSS_MAIN_H_

#include "log.h"

#define AWSS_VER                       "{\"smartconfig\":\"2.0\",\"zconfig\":\"2.0\",\"router\":\"2.0\",\"ap\":\"2.0\"}"

#define DEFAULT_SSID                   zc_default_ssid
#define DEFAULT_PASSWD                 zc_default_passwd
#define ADHA_SSID                      zc_adha_ssid
#define ADHA_PASSWD                    zc_adha_passwd

#define WLAN_CONNECTION_TIMEOUT_MS     (30 * 1000)
#define DEV_INFO_LEN_MAX               (512)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AWSS_DEBUG 1
#if (AWSS_DEBUG == 1)
#define awss_debug(fmt, args...) log_debug(fmt, ##args)
#else
#define awss_debug(fmt, args...)
#endif

extern const char *zc_default_ssid;
extern const char *zc_default_passwd;
extern const char *zc_adha_ssid;
extern const char *zc_adha_passwd;

int __awss_start(void);
int __awss_stop(void);

int awss_cancel_aha_monitor(void);
int awss_event_post(int event);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
