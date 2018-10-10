/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "awss_cmp.h"
#include "awss_notify.h"
#include "awss_reset.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int awss_report_cloud()
{
    awss_cmp_online_init();
#ifdef DEVICE_MODEL_ENABLED
    awss_check_reset();
#endif
    awss_report_token();
#ifdef ALCS_ENABLED
    awss_cmp_local_init();
#endif
    awss_dev_bind_notify_stop();
    awss_dev_bind_notify();
#ifdef WIFI_PROVISION_ENABLED
#ifndef AWSS_DISABLE_REGISTRAR
    extern void awss_registrar_init(void);
    awss_registrar_init();
#endif
#endif
    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
