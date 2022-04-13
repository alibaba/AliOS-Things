/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal_trans.h"
#include "ota/ota_agent.h"

static void otacoap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    char *p_payload = NULL;
    unsigned char resp_code = 0;
    ota_service_t *ctx = (ota_service_t *)arg;
    ota_coap_get_code(p_response, &resp_code);
    ota_coap_get_payload(p_response, (const char**)&p_payload, &len);
    OTA_LOG_I("coap res code = %d, len=%d, msg=%s", resp_code,len, p_payload ? p_payload : NULL);
    if (p_payload != NULL) {
        ota_sevice_parse_msg(ctx, payload);
    }
}

/**
 * ota_coap_publish message to Cloud
 *
 * @param[in] name      topic name
 * @param[in] msg       message content
 * @param[in] pk        product key
 * @param[in] dn        device name
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
static int ota_coap_publish(char *name, char *msg, char *pk, char* dn)
{
    int ret = 0;
    char topic[OTA_MSG_LEN] = {0};
    ota_coap_message_t message;
    message.p_payload     = (unsigned char *)msg;
    message.payload_len   = (unsigned short)strlen(msg);
    message.resp_callback = otacoap_response_handler;
    message.msg_type      = COAP_MESSAGE_CON;
    message.content_type  = COAP_CONTENT_TYPE_JSON;
    ret = ota_snprintf(topic, OTA_MSG_LEN, "/topic/ota/device/%s/%s/%s", name, pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    OTA_LOG_I("coap pub url:%s msg:%s", ctx->url, msg);
    if (0 !=(ret = ota_coap_send(topic, &message))) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_transport_inform  OTA inform version to cloud.
 *
 * @param[in] ota_service_t *ctx ctx  ota service context
 * @param[in]          char *ver      version string
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
int ota_transport_inform(ota_service_t *ctx, char* ver)
{
    int  ret              = 0;
    char msg[OTA_MSG_LEN] = {0};
    ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":%d,\"params\":{\"version\":\"%s\"}}", 0, ver);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_coap_publish("request", msg, ctx->pk, ctx->dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_transport_upgrade  OTA subcribe message from Cloud.
 *
 * @param[in] ota_service_t *ctx ctx  ota service context
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
int ota_transport_upgrade(ota_service_t *ctx)
{
    int  ret                = OTA_TRANSPORT_INT_FAIL;
    return ret;
}

/**
 * ota_transport_status  OTA report status to Cloud
 *
 * @param[in] ota_service_t *ctx ctx  ota service context
 * @param[in] status    [1-100] percent, [<0] error no.
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
int ota_transport_status(ota_service_t *ctx, int status)
{
    int  ret                = OTA_TRANSPORT_INT_FAIL;
    char msg[OTA_MSG_LEN]   = {0};
    char *err_str           = "";

    if(ctx == NULL) {
        return ret;
    }
    if (status < 0) {
        switch (status) {
            case OTA_INIT_FAIL:
                err_str = "OTA init failed";
                break;
            case OTA_TRANSPORT_INT_FAIL:
                err_str = "OTA transport init failed";
                break;
            case OTA_TRANSPORT_VER_FAIL:
                err_str = "OTA transport verion is too old";
                break;
            case OTA_TRANSPORT_PAR_FAIL:
                err_str = "OTA transport parse failed";
                break;
            case OTA_DOWNLOAD_INIT_FAIL:
                err_str = "OTA download init failed";
                break;
            case OTA_DOWNLOAD_HEAD_FAIL:
                err_str = "OTA download header failed";
                break;
            case OTA_DOWNLOAD_CON_FAIL:
                err_str = "OTA download connect failed";
                break;
            case OTA_DOWNLOAD_REQ_FAIL:
                err_str = "OTA download request failed";
                break;
            case OTA_DOWNLOAD_RECV_FAIL:
                err_str = "OTA download receive failed";
                break;
            case OTA_VERIFY_MD5_FAIL:
                err_str = "OTA verfiy MD5 failed";
                break;
            case OTA_VERIFY_SHA2_FAIL:
                err_str = "OTA verfiy SHA256 failed";
                break;
            case OTA_VERIFY_RSA_FAIL:
                err_str = "OTA verfiy RSA failed";
                break;
            case OTA_VERIFY_IMAGE_FAIL:
                err_str = "OTA verfiy image failed";
                break;
            case OTA_UPGRADE_WRITE_FAIL:
                err_str = "OTA upgrade write failed";
                break;
            case OTA_UPGRADE_FW_SIZE_FAIL:
                err_str = "OTA upgrade FW too big";
                break;
            case OTA_CUSTOM_CALLBAK_FAIL:
                err_str = "OTA register callback failed";
                break;
            case OTA_MCU_INIT_FAIL:
                err_str = "OTA MCU init failed";
                break;
            case OTA_MCU_VERSION_FAIL:
                err_str = "OTA MCU init failed";
                break;
            case OTA_MCU_NOT_READY:
                err_str = "OTA MCU not ready";
                break;
            case OTA_MCU_REBOOT_FAIL:
                err_str = "OTA MCU fail to reboot";
                break;
            case OTA_MCU_HEADER_FAIL:
                err_str = "OTA MCU header error";
                break;
            case OTA_MCU_UPGRADE_FAIL:
                err_str = "OTA MCU upgrade fail";
                break;
            case OTA_INVALID_PARAMETER:
                err_str = "OTA invalid parameter";
                break;
            default:
                err_str = "OTA undefined failed";
                break;
        }
    }
    ret = ota_snprintf(msg, OTA_MSG_LEN -1, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", 1, status, err_str);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_coap_publish("progress", msg, ctx->pk, ctx->dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_transport_init  OTA transport init
 *
 * @param[in] ota_service_t *ctx ctx  ota service context
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_init(void)
{
    int ret = 0;
    ret = ota_coap_init();
    if(ret < 0){
        ret = OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_transport_init  OTA transport deinit
 *
 * @param[in] ota_service_t *ctx ctx  ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_deinit(void)
{   
    int ret = 0;
    ret = ota_coap_deinit();
    if(ret < 0){
        ret = OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

