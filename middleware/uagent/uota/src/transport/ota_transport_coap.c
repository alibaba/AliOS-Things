/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>

#include "ota_log.h"
#include "ota_service.h"
#include "ota_verify.h"
#include "ota_rsa_verify.h"
#include "ota_hal_os.h"
#include "ota_base64.h"

static ota_cloud_cb_t ota_update;
// Generate topic name according to @ota_topic_type, @product_key, @device_name
// and then copy to @buf.
// 0, successful; -1, failed
static int otacoap_GenTopicName(char *buf, int len, char *topic)
{
    int ret = 0;
    if (buf == NULL || topic == NULL) {
        OTA_LOG_E("coap gen topic is null");
        return -1;
    }
    ret = ota_snprintf(buf, len, "/topic/ota/device/%s/%s/%s", topic, ota_get_service()->pk, ota_get_service()->dn);
    if (ret < 0) {
        OTA_LOG_E("coap topic failed");
        return -1;
    }
    return 0;
}

// Generate firmware information according to @id, @version
// and then copy to @buf.
// 0, successful; -1, failed
static int otalib_GenReqMsg(char *buf, int len, int id, const char *ver)
{
    int ret = 0;
    ret     = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"mode\":\"coap\",\"version\":\"%s\"}}", id, ver? ver: NULL);
    if (ret < 0) {
        OTA_LOG_E("coap msg failed");
        return -1;
    }
    return 0;
}

// Generate report information according to @id, @msg
// and then copy to @buf.
// 0, successful; -1, failed
static int otalib_GenReportMsg(char *buf, int len, int id, int progress, const char *msg)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", id,progress, msg?msg:NULL);
    if (ret < 0) {
        OTA_LOG_E("coap report failed");
        return -1;
    }
    return 0;
}

static void otacoap_response_handler(void *arg, void *p_response)
{
    int              len       = 0;
    char             *p_payload = NULL;
    unsigned char    resp_code = 0;
    ota_coap_get_code(p_response, &resp_code);
    ota_coap_get_payload(p_response, (const char **)&p_payload, &len);
    OTA_LOG_D("coap res code = %d, len=%d, msg=%s", resp_code,len, p_payload ? p_payload : NULL);
    if ((NULL != ota_get_service()->h_ch) && (NULL != p_payload)) {
        ota_update(p_payload);
    }
}

// report progress of OTA
static int otacoap_Publish(char *topic, char *msg)
{
    int              ret = 0;
    char             uri[OTA_MQTT_TOPIC_LEN + 1] = {0};
    ota_coap_message_t       message;
    message.p_payload        = (unsigned char *)msg;
    message.payload_len      = (unsigned short)strlen(msg);
    message.resp_callback    = otacoap_response_handler;
    message.msg_type         = COAP_MESSAGE_CON;
    message.content_type     = COAP_CONTENT_TYPE_JSON;
    ret = otacoap_GenTopicName(uri, OTA_COAP_URI_MAX_LEN, topic);
    if (ret < 0) {
        OTA_LOG_E("coap topic failed");
        return -1;
    }
    OTA_LOG_I("coap pub url:%s msg:%s\n", uri, msg);
    if (0 !=(ret = ota_coap_send(ota_get_service()->h_ch, (char *)uri, &message))) {
        OTA_LOG_E("coap send failed:%d", ret);
        return -1;
    }
    return 0;
}

static int ota_trans_inform(const char* ver, char* pk, char* dn)
{
    int  ret = 0;
    char msg[OTA_MSG_INFORM_LEN] = {0};
    if ((NULL == ota_get_service()->h_ch)) {
        OTA_LOG_E("coap invalid parameter");
        return -1;
    }
    ret = otalib_GenReqMsg(msg, OTA_MSG_INFORM_LEN, 0, ver);
    if (ret != 0) {
        OTA_LOG_E("coap msg failed");
        return -1;
    }
    OTA_LOG_I("coap pub msg:%s\n", msg);
    ret = otacoap_Publish("request", msg);
    if (0 != ret) {
        OTA_LOG_E("coap req failed");
        return -1;
    }
    return 0;
}

int ota_trans_init(void)
{
    int ret = 0;
    ret = ota_coap_init();
    if (ret < 0) {
        OTA_LOG_E("coap init failed");
        return -1;
    }
    return 0;
}

int ota_trans_response(const char *response)
{
    return 0;
}

static int ota_trans_upgrade(ota_cloud_cb_t msg_cb, char* pk, char* dn)
{
    ota_update = msg_cb;
    return 0;
}

static int ota_trans_status(int progress, char* pk, char* dn)
{
    int  ret = -1;
    int status = ota_get_status();
    char msg[OTA_MSG_REPORT_LEN] = {0};
    char err[32];
    memset(err, 0x00, sizeof(err));
    if (status < 0) {
        progress = status;
        switch (status) {
            case OTA_INIT_FAIL:
                sprintf(err, "%s", "ota init failed");
                break;
            case OTA_INIT_VER_FAIL:
                sprintf(err, "%s", "ota version not match");
                break;
            case OTA_DOWNLOAD_FAIL:
                sprintf(err, "%s", "ota download failed");
                break;
            case OTA_DOWNLOAD_URL_FAIL:
                sprintf(err, "%s", "ota download url failed");
                break;
            case OTA_DOWNLOAD_IP_FAIL:
                sprintf(err, "%s", "ota download ip failed");
                break;
            case OTA_DOWNLOAD_CON_FAIL:
                sprintf(err, "%s", "ota download connect failed");
                break;
            case OTA_DOWNLOAD_READ_FAIL:
                sprintf(err, "%s", "ota download read failed");
                break;
            case OTA_DOWNLOAD_WRITE_FAIL:
                sprintf(err, "%s", "ota download write failed");
                break;
            case OTA_VERIFY_FAIL:
                sprintf(err, "%s", "ota verify failed");
                break;
            case OTA_UPGRADE_FAIL:
                sprintf(err, "%s", "ota upgrade failed");
                break;
            case OTA_REBOOT_FAIL:
                sprintf(err, "%s", "ota reboot failed");
                break;
            case OTA_VERIFY_RSA_FAIL:
                sprintf(err, "%s", "ota verify rsa failed");
                break;
            case OTA_VERIFY_HASH_FAIL:
                sprintf(err, "%s", "ota verify hash failed");
                break;
            case OTA_UPGRADE_DIFF_FAIL:
                sprintf(err, "%s", "ota diff failed");
                break;
            default:
                sprintf(err, "%s", "ota undefined failed");
                break;
        }
    }
    ret = otalib_GenReportMsg(msg, OTA_MSG_REPORT_LEN, 0, progress, err);
    if (0 != ret) {
        OTA_LOG_E("coap report failed");
        return -1;
    }

    ret = otacoap_Publish("progress", msg);
    if (0 != ret) {
        OTA_LOG_E("coap progress failed");
        return -1;
    }

    return ret;
}

static int ota_trans_deinit(void)
{
    return ota_coap_deinit();
}

static ota_transport_t trans_coap = {
    .init                  = ota_trans_init,
    .inform                = ota_trans_inform,
    .response              = ota_trans_response,
    .upgrade               = ota_trans_upgrade,
    .status                = ota_trans_status,
    .deinit                = ota_trans_deinit,
};

ota_transport_t *ota_get_transport_coap(void)
{
    return &trans_coap;
}
