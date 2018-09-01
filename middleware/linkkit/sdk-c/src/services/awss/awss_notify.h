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
#ifndef AWSS_NOTIFY_H
#define AWSS_NOTIFY_H

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum {
    AWSS_NOTIFY_DEV_INFO = 0,
    AWSS_NOTIFY_DEV_TOKEN,
    AWSS_NOTIFY_DEV_RAND,
    AWSS_NOTIFY_SUC,
    AWSS_NOTIFY_MAX,
};

int online_mcast_get_device_info(void *ctx, void *resource, void *remote, void *request);
int online_ucast_get_device_info(void *ctx, void *resource, void *remote, void *request);

int awss_notify_dev_info(int type, int count);
int awss_devinfo_notify();
int awss_suc_notify();
int awss_connectap_notify();
int awss_devinfo_notify_stop();
int awss_suc_notify_stop();
int awss_connectap_notify_stop();

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
