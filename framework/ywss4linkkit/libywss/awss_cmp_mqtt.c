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

#include <stdio.h>
#include "os.h"
#include "mqtt_instance.h"
#include "awss_cmp.h"
#include "awss_notify.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

static char user_data;
static char online_init = 0;

int awss_cmp_mqtt_register_cb(char *topic, void* cb)
{
    if (topic == NULL)
        return -1;

    return mqtt_subscribe(topic, (void (*)(char *, int, void *, int, void *))cb, &user_data);
}

int awss_cmp_mqtt_unregister_cb(char *topic)
{
    return mqtt_unsubscribe(topic);
}

int awss_cmp_mqtt_send(char *topic, void *data, int len)
{
    return mqtt_publish(topic, 1, data, len);  // IOTX_MQTT_QOS1
}

const struct awss_cmp_couple awss_online_couple[] = {
    {TOPIC_MATCH_REPORT_REPLY, awss_report_token_reply},
    {TOPIC_RESET_REPORT_REPLY, awss_report_reset_reply},
#ifndef AWSS_DISABLE_REGISTRAR
    {TOPIC_ZC_CHECKIN,         awss_enrollee_checkin},
    {TOPIC_ZC_ENROLLEE_REPLY,  awss_report_enrollee_reply},
    {TOPIC_ZC_CIPHER_REPLY,    awss_get_cipher_reply},
#endif
    {TOPIC_SWITCHAP,           awss_online_switchap}
};

int awss_cmp_online_init()
{
    if (online_init)
        return 0;

    char topic[TOPIC_LEN_MAX] = {0};
    int i;
#ifdef AWSS_INIT_CMP
    {
        char pk[PRODUCT_KEY_LEN + 1] = {0};
        char dn[PRODUCT_NAME_LEN + 1] = {0};
        char ds[OS_DEVICE_SECRET_LEN + 1] ={0};

        os_product_get_key(pk);
        os_product_get_name(dn);
        os_get_device_secret(ds);
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

    if (!online_init)
        return 0;

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
