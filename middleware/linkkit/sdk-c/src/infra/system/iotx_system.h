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

int     iotx_guider_authenticate(void);
void    iotx_guider_set_domain_type(int domain_type);
char   *iotx_guider_get_domain(void);
void    iotx_guider_auth_set(int authed);
int     iotx_guider_auth_get(void);

int     iotx_device_info_init(void);
int     iotx_device_info_set(
            const char *product_key,
            const char *device_name,
            const char *device_secret);

iotx_device_info_pt iotx_device_info_get(void);
iotx_conn_info_pt iotx_conn_info_get(void);


#define MIDREPORT_PAYLOAD_LEN       (62 + PID_STRLEN_MAX + MID_STRLEN_MAX + 32 +1)
#define MIDREPORT_REQID_LEN         (PRODUCT_KEY_LEN + DEVICE_NAME_LEN + 6)

#define AOS_VERSON_MSG_LEN          (256)
#define LINKKIT_VERSION_MSG_LEN     (192)
#define FIRMWARE_VERSION_MSG_LEN    (64)

int     iotx_midreport_reqid(char *requestId, char *product_key, char *device_name);
int     iotx_midreport_payload(char *msg, char *requestId, char *mid, char *pid);
int     iotx_midreport_topic(char *topic_name, char *topic_head, char *product_key, char *device_name);

/* AOS version report API */
int     iotx_gen_aos_report_topic(char *topic_name, char *product_key, char *device_name);
int     iotx_gen_aos_report_payload(char *msg, int requestId, char *versionData);

const char *iotx_ca_get(void);

#if defined(__cplusplus)
}
#endif
#endif
