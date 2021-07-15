/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_DEV_AP_H__
#define __AWSS_DEV_AP_H__

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#include "exports/iot_export_awss.h"

// int awss_dev_ap_stop(void);
// int awss_start(pair_success_callback pair_cb);
int wifimgr_process_dev_ap_switchap_request(void *ctx, void *resource, void *remote, void *request);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
