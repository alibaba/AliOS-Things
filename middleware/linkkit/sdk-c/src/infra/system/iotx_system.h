/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_AUTH_H_
#define _IOTX_AUTH_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include "iot_import.h"
#include "iot_export.h"
#include "utils_sysinfo.h"
#include "report.h"

/* region type define */
typedef enum _REGION_TYPE {
    GUIDER_REGION_SHANGHAI,
    GUIDER_REGION_SINGAPORE,
    GUIDER_REGION_JAPAN,
    GUIDER_REGION_AMERICA,
    GUIDER_REGION_GERMANY,
    GUIDER_REGION_CUSTOM,
    GUIDER_REGION_MAX
} region_type_t;

/* domain type define */
typedef enum _DOMAIN_TYPE {
    GUIDER_DOMAIN_MQTT,
    GUIDER_DOMAIN_HTTP,
    GUIDER_DOMAIN_MAX
} domain_type_t;

int     iotx_guider_authenticate(void);
int     iotx_guider_set_region(int region_type);
int     iotx_guider_get_region(void);
int     iotx_guider_set_custom_domain(int domain_type, const char *domain);
void    iotx_guider_auth_set(int authed);
int     iotx_guider_auth_get(void);
int     iotx_facility_json_print(const char *str, int level, ...);

const char     *iotx_guider_get_domain(int domain_type);

int     iotx_device_info_init(void);
int     iotx_device_info_set(
            const char *product_key,
            const char *device_name,
            const char *device_secret);

iotx_device_info_pt iotx_device_info_get(void);
iotx_conn_info_pt iotx_conn_info_get(void);


const char *iotx_ca_get(void);

#if defined(__cplusplus)
}
#endif
#endif
