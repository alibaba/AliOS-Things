/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#if defined (MQTT_COMM_ENABLED)
#include <stdio.h>
#include <string.h>
#include "iot_export.h"
#include "iot_import.h"
#include "iotx_utils.h"
#include "iotx_system.h"
#include "iotx_log.h"
#include "awss_reset.h"
#include "awss_reset_statis.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define AWSS_RESET_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "dev_reset")
#define AWSS_RESET_FREE(ptr)    LITE_free(ptr)

static uint8_t awss_report_reset_suc = 0;
static uint16_t awss_report_reset_id = 0;
static void *report_reset_timer = NULL;

int awss_report_reset_to_cloud();

void awss_report_reset_reply(void *pcontext, void *pclient, void *mesg)
{
    char rst = 0;

    iotx_mqtt_event_msg_pt msg = (iotx_mqtt_event_msg_pt)mesg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            break;
        default:
            return;
    }

    log_debug("[RST]", "%s\r\n", __func__);

    awss_report_reset_suc = 1;
    HAL_Kv_Set(AWSS_KV_RST, &rst, sizeof(rst), 0);

    HAL_Timer_Stop(report_reset_timer);
    HAL_Timer_Delete(report_reset_timer);
    report_reset_timer = NULL;

    AWSS_RST_UPDATE_STATIS(AWSS_RST_STATIS_SUC);
    iotx_event_post(IOTX_RESET);  /* for old version of event */
    do {  /* for new version of event */
        void *cb = NULL;
        cb = (void *)iotx_event_callback(ITE_AWSS_STATUS);
        if (cb == NULL) {
            break;
        }
        ((int (*)(int))cb)(IOTX_RESET);
    } while (0);

    AWSS_RST_DISP_STATIS();

#ifdef DEV_BIND_ENABLED
    extern int awss_start_bind();
    awss_start_bind();
#endif
}

int awss_report_reset_to_cloud()
{
    int ret = -1;
    int final_len = 0;
    char *topic = NULL;
    char *packet = NULL;
    int packet_len = AWSS_RESET_PKT_LEN;
    int topic_len = AWSS_RESET_TOPIC_LEN;

    if (awss_report_reset_suc)
        return 0;

    AWSS_RST_UPDATE_STATIS(AWSS_RST_STATIS_START);

    if (report_reset_timer == NULL) {
        report_reset_timer = HAL_Timer_Create("report_rst", (void (*)(void *))awss_report_reset_to_cloud, NULL);
    }
    HAL_Timer_Stop(report_reset_timer);
    HAL_Timer_Start(report_reset_timer, 3000);

    do {
        char pk[PRODUCT_KEY_LEN + 1] = {0};
        char dn[DEVICE_NAME_LEN + 1] = {0};

        HAL_GetProductKey(pk);
        HAL_GetDeviceName(dn);

        topic = (char *)AWSS_RESET_MALLOC(topic_len + 1);
        if (topic == NULL) {
            goto REPORT_RST_ERR;
        }
        memset(topic, 0, topic_len + 1);

        HAL_Snprintf(topic, topic_len, TOPIC_RESET_REPORT_REPLY, pk, dn);

        ret = IOT_MQTT_Subscribe_Sync(NULL, topic, IOTX_MQTT_QOS0,
                                      (iotx_mqtt_event_handle_func_fpt)awss_report_reset_reply, NULL, 1000);
        if (ret < 0) {
            goto REPORT_RST_ERR;
        }

        memset(topic, 0, topic_len + 1);
        HAL_Snprintf(topic, topic_len, TOPIC_RESET_REPORT, pk, dn);
    } while (0);

    packet = AWSS_RESET_MALLOC(packet_len + 1);
    if (packet == NULL) {
        ret = -1;
        goto REPORT_RST_ERR;
    }
    memset(packet, 0, packet_len + 1);

    do {
        char id_str[AWSS_RESET_MSG_ID_LEN + 1] = {0};
        HAL_Snprintf(id_str, AWSS_RESET_MSG_ID_LEN, "\"%u\"", awss_report_reset_id ++);
        final_len = HAL_Snprintf(packet, packet_len, AWSS_RESET_REQ_FMT, id_str, METHOD_RESET_REPORT, "{}");
    } while (0);

    log_debug("[RST]", "report reset:%s\r\n", packet);

    ret = IOT_MQTT_Publish_Simple(NULL, topic, IOTX_MQTT_QOS0, packet, final_len);
    log_debug("[RST]", "report reset result:%d\r\n", ret);

REPORT_RST_ERR:
    if (packet) {
        AWSS_RESET_FREE(packet);
    }
    if (topic) {
        AWSS_RESET_FREE(topic);
    }
    return ret;
}

int awss_report_reset()
{
    char rst = 0x01;

    awss_report_reset_suc = 0;
   
    HAL_Kv_Set(AWSS_KV_RST, &rst, sizeof(rst), 0);
    return awss_report_reset_to_cloud();
}

int awss_check_reset()
{
    int len = 1;
    char rst = 0;

    HAL_Kv_Get(AWSS_KV_RST, &rst, &len);

    if (rst != 0x01) { /* reset flag is not set */
        log_debug("[RST]", "no rst\r\n");
        return 0;
    }
    log_debug("[RST]", "need report rst\r\n");
    awss_report_reset_suc = 0;

    return 1;
}

int awss_stop_report_reset()
{
    if (report_reset_timer == NULL) {
        return 0;
    }

    HAL_Timer_Stop(report_reset_timer);
    HAL_Timer_Delete(report_reset_timer);
    report_reset_timer = NULL;

    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif