/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAAS_ACTIVATION_H
#define HAAS_ACTIVATION_H

#define ACTIVATION_AOS_VERSION   "AOS-R-3.3.0"
#define ACTIVATION_PARTNER       "AliOS Things Team"
#define ACTIVATION_APP_NET       "MQTT/HTTPS"
#define ACTIVATION_PROJECT       "HAAS Inside"
#define ACTIVATION_FRAMEWORK     "HaaS Python"
#define ACTIVATION_NETWORK       "Ethernet"
#define ACTIVATION_DEVICE_TYPE   "solo"
#define ACTIVATION_CLOUD_TYPE    "aliyun"
#define ACTIVATION_BOARD         "01Studio pyBoard Pro"
#define ACTIVATION_MCU           "STM32F407"

#define ACTIVATION_ERR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define ACTIVATION_INFO(fmt, ...)
#define ACTIVATION_DEBUG(fmt, ...)

#ifdef AOS_COMP_KV
static int32_t g_activation_report = 0;
#endif

#define ACTIVATION_REPORT_FORMAT \
    "POST /report HTTP/1.1\r\nHost:os-activation.iot.aliyun.com\r\n\
User-Agent: AliOS-Things\r\nContent-Length:%d\r\nAccept: */*\r\nContent-Type:application/json\r\n\
Connection: Keep-Alive\r\n\r\n%s\r\n"

#define ACTIVATION_REPORT_PAYLOAD_FORMAT       "{\"activationStr\":\"%s\"}"

#define ACTIVATION_RESPONSE_RESULT_START       "{\"success\":"
#define ACTIVATION_RESPONSE_RESULT_END         ","

#define ACTIVATION_RESPONSE_MESSAGE_START      ",\"message\":\""
#define ACTIVATION_RESPONSE_MESSAGE_END        "\"}"

/* The message length is limited by Aliyun */
#define ACTIVATION_MSG_MAX_LEN                 (255)

#define REPORT_SERVER_ADDR                     "os-activation.iot.aliyun.com"
#define REPORT_SERVER_PORT                     (80)

#define ACTIVATION_AOS_VERSION_PREFIX          "AOS-R-"
#define ACTIVATION_AOS_VERSION_POSTFIX_MAX_LEN (8)

#define MAX_WAIT_MS                            (12 * 1000)
#define DELAY_PERIOD_MS                        (100)

/* tigger activation report */
int activation_report(void);

#endif
