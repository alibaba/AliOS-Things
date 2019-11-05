/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_MAIN_H__
#define __AWSS_MAIN_H__

#include "awss_log.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define DEFAULT_SSID                   zc_default_ssid
#define DEFAULT_PASSWD                 zc_default_passwd

extern const char *zc_default_ssid;
extern const char *zc_default_passwd;

int __awss_start(void);
int __awss_stop(void);

void awss_update_config_press();
uint32_t awss_get_press_timeout_ms();
void awss_set_channel_scan_interval_ms(uint32_t timeout);
uint32_t awss_get_channel_scan_interval_ms();
void awss_set_press_timeout_ms(unsigned int timeout_ms);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
