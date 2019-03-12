/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_EXPORT_COMPAT_H__
#define __IOT_EXPORT_COMPAT_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* for forward compatibility */

/* V2.3.0 */
#define IOTX_CLOUD_DOMAIN_SH        IOTX_CLOUD_REGION_SHANGHAI
#define IOTX_CLOUD_DOMAIN_SG        IOTX_CLOUD_REGION_SINGAPORE
#define IOTX_CLOUD_DOMAIN_JP        IOTX_CLOUD_REGION_JAPAN
#define IOTX_CLOUD_DOMAIN_US        IOTX_CLOUD_REGION_USA_WEST
#define IOTX_CLOUD_DOMAIN_GER       IOTX_CLOUD_REGION_GERMANY
#define IOTX_IOCTL_SET_DOMAIN       IOTX_IOCTL_SET_REGION
#define IOTX_IOCTL_GET_DOMAIN       IOTX_IOCTL_GET_REGION

#define IOT_OpenLog(arg)
#define IOT_CloseLog()              IOT_SetLogLevel(IOT_LOG_NONE)
#define IOT_LOG_EMERG               IOT_LOG_NONE

#define IOT_Linkkit_Post            IOT_Linkkit_Report

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_EXPORT_COMPAT_H__ */

