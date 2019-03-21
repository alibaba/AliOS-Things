/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota/ota_service.h"
#include "ota_verify.h"
#include "ota_hal_os.h"

/*
* Generate topic name according to @ota_topic_type, @product_key, @device_name
* and then copy to @buf.
* 0, successful; -1, failed
*/
static int otacoap_GenTopicName(char *buf, int len, char *topic, char *pk, char *dn)
{
    int ret = 0;
    if (buf == NULL || topic == NULL) {
        return -1;
    }
    ret = ota_snprintf(buf, len, "/topic/ota/device/%s/%s/%s", topic, pk, dn);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

/*
* Generate firmware information according to @id, @version
* and then copy to @buf.
* 0, successful; -1, failed
*/
static int otalib_GenReqMsg(char *buf, int len, int id, const char *ver)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"mode\":\"coap\",\"version\":\"%s\"}}", id, ver ? ver : NULL);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

/*
* Generate report information according to @id, @msg
* and then copy to @buf.
* 0, successful; -1, failed
*/
static int otalib_GenReportMsg(char *buf, int len, int id, int progress, const char *msg)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", id, progress, msg ? msg : NULL);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static void otacoap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    char *p_payload = NULL;
    unsigned char resp_code = 0;
    ota_coap_get_code(p_response, &resp_code);
    ota_coap_get_payload(p_response, (const char**)&p_payload, &len);
    OTA_LOG_I("coap res code = %d, len=%d, msg=%s", resp_code,len, p_payload ? p_payload : NULL);

    ota_service_t *ctx = (ota_service_t*)arg;
    if (!ctx->upgrade_cb) {
        return;
    }
    if ((NULL != ctx->h_ch) && (NULL != p_payload)) {
        ctx->upgrade_cb(ctx, p_payload);
    }
}

/* report progress of OTA*/
static int otacoap_Publish(char *topic, char *msg, void *pctx)
{
    int ret = 0;
    ota_coap_message_t message;
    ota_service_t *ctx = pctx;
    if ((NULL == ctx) || (NULL == ctx->h_ch)) {
        return -1;
    }
    message.p_payload     = (unsigned char *)msg;
    message.payload_len   = (unsigned short)strlen(msg);
    message.resp_callback = otacoap_response_handler;
    message.msg_type      = COAP_MESSAGE_CON;
    message.content_type  = COAP_CONTENT_TYPE_JSON;
    ret = otacoap_GenTopicName(ctx->url, OTA_COAP_URI_MAX_LEN, topic, ctx->pk, ctx->dn);
    if (ret < 0) {
        return -1;
    }
    OTA_LOG_I("coap pub url:%s msg:%s", ctx->url, msg);
    if (0 !=(ret = ota_coap_send(ctx->h_ch, ctx->url, &message))) {
        return -1;
    }
    return 0;
}

static int ota_trans_inform(void *pctx)
{
    int  ret = 0;
    char msg[OTA_MSG_INFORM_LEN] = {0};
    ota_service_t* ctx = pctx;
    if ((NULL == ctx) || (NULL == ctx->h_ch)) {
        return -1;
    }
    ret = otalib_GenReqMsg(msg, OTA_MSG_INFORM_LEN, 0, ctx->sys_ver);
    if (ret != 0) {
        return -1;
    }
    OTA_LOG_I("coap pub msg:%s", msg);
    ret = otacoap_Publish("request", msg, ctx);
    if (0 != ret) {
        return OTA_TRANSPORT_FAIL;
    }
    return 0;
}

int ota_trans_init(void)
{
    int ret = 0;
    ret = ota_coap_init();
    if (ret < 0) {
        return OTA_TRANSPORT_FAIL;
    }
    return 0;
}

int ota_trans_response(const char *response)
{
    return 0;
}

static int ota_trans_upgrade(void *pctx)
{
    return 0;
}

static int ota_trans_status(int progress, void *pctx)
{
    int  ret                     = -1;
    char msg[OTA_MSG_REPORT_LEN] = {0};
    char err[OTA_MAX_VER_LEN]    = {0};
    ota_service_t *ctx           = pctx;
    if (NULL == ctx) {
        OTA_LOG_E("parameter null.");
        return -1;
    }
    int status = ctx->upg_status;
    memset(err, 0x00, sizeof(err));
    if (status < 0) {
        progress = status;
        switch (status) {
            case OTA_INIT_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota init failed");
                break;
            case OTA_INIT_VER_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota version not match");
                break;
            case OTA_DOWNLOAD_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota download failed");
                break;
            case OTA_DOWNLOAD_URL_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota download url failed");
                break;
            case OTA_DOWNLOAD_IP_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota download ip failed");
                break;
            case OTA_DOWNLOAD_CON_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota download connect failed");
                break;
            case OTA_DOWNLOAD_READ_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota download read failed");
                break;
            case OTA_DOWNLOAD_WRITE_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota download write failed");
                break;
            case OTA_VERIFY_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota verify failed");
                break;
            case OTA_UPGRADE_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota upgrade failed");
                break;
            case OTA_REBOOT_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota reboot failed");
                break;
            case OTA_VERIFY_RSA_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota verify rsa failed");
                break;
            case OTA_VERIFY_HASH_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota verify hash failed");
                break;
            case OTA_UPGRADE_DIFF_FAIL:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota diff failed");
                break;
            default:
                ota_snprintf(err, OTA_MAX_VER_LEN - 1, "%s", "ota undefined failed");
                break;
        }
    }
    ret = otalib_GenReportMsg(msg, OTA_MSG_REPORT_LEN, 0, progress, err);
    if (0 != ret) {
        return -1;
    }
    ret = otacoap_Publish("progress", msg, ctx);
    if (0 != ret) {
        return OTA_TRANSPORT_FAIL;
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
    .upgrade               = ota_trans_upgrade,
    .status                = ota_trans_status,
    .deinit                = ota_trans_deinit,
};

ota_transport_t *ota_get_transport(void)
{
    return &trans_coap;
}
