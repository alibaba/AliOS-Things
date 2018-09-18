/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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
