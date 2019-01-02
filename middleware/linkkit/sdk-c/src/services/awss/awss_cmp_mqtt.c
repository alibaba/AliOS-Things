/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include "os.h"
#include "mqtt_instance.h"
#include "iot_import.h"
#include "iot_export.h"
#include "awss_cmp.h"
#include "awss_notify.h"
#include "ntp.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

static char user_data;
static char online_init = 0;

int awss_cmp_mqtt_register_cb(char *topic, void *cb)
{
    if (topic == NULL) {
        return -1;
    }

    return mqtt_subscribe(topic, (void (*)(char *, int, void *, int, void *))cb, &user_data);
}

int awss_cmp_mqtt_unregister_cb(char *topic)
{
    return mqtt_unsubscribe(topic);
}

int awss_cmp_mqtt_send(char *topic, void *data, int len, int qos)
{
    return mqtt_publish(topic, qos, data, len);  //IOTX_MQTT_QOS1 or IOTX_MQTT_QOS1
}

const struct awss_cmp_couple awss_online_couple[] = {
    {TOPIC_MATCH_REPORT_REPLY, awss_report_token_reply},
    {TOPIC_RESET_REPORT_REPLY, awss_report_reset_reply},
#ifndef AWSS_DISABLE_REGISTRAR
    {TOPIC_ZC_CHECKIN,         awss_enrollee_checkin},
    {TOPIC_ZC_ENROLLEE_REPLY,  awss_report_enrollee_reply},
    {TOPIC_ZC_CIPHER_REPLY,    awss_get_cipher_reply},
#endif
    {TOPIC_SWITCHAP,           awss_online_switchap},
    {TOPIC_NTP_REPLY,          linkkit_ntp_time_reply}
};

int awss_cmp_online_init()
{
    if (online_init) {
        return 0;
    }

    char topic[TOPIC_LEN_MAX] = {0};
    int i;
#ifdef AWSS_INIT_CMP
    {
        char pk[OS_PRODUCT_KEY_LEN + 1] = {0};
        char dn[OS_DEVICE_NAME_LEN + 1] = {0};
        char ds[OS_DEVICE_SECRET_LEN + 1] = {0};

        os_product_get_key(pk);
        os_device_get_name(dn);
        os_device_get_secret(ds);
        mqtt_init_instance(pk, dn, ds, 1024);
    }
#endif
    for (i = 0; i < sizeof(awss_online_couple) / sizeof(awss_online_couple[0]); i ++) {
        memset(topic, 0, sizeof(topic));
        awss_build_topic(awss_online_couple[i].topic, topic, TOPIC_LEN_MAX);
        awss_cmp_mqtt_register_cb(topic, awss_online_couple[i].cb);
    }

    online_init = 1;

    return 0;
}

int awss_cmp_online_deinit()
{
    unsigned char i;
    char topic[TOPIC_LEN_MAX] = {0};

    if (!online_init) {
        return 0;
    }

    awss_connectap_notify_stop();

    for (i = 0; i < sizeof(awss_online_couple) / sizeof(awss_online_couple[0]); i ++) {
        memset(topic, 0, sizeof(topic));
        awss_build_topic(awss_online_couple[i].topic, topic, TOPIC_LEN_MAX);
        awss_cmp_mqtt_unregister_cb(topic);
    }

    online_init = 0;

    return 0;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
