/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
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
