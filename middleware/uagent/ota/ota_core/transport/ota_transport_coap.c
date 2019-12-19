/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_common.h"

static void otacoap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    char *p_payload = NULL;
    unsigned char resp_code = 0;
    ota_coap_get_code(p_response, &resp_code);
    ota_coap_get_payload(p_response, (const char**)&p_payload, &len);
    OTA_LOG_I("coap res code = %d, len=%d, msg=%s", resp_code,len, p_payload ? p_payload : NULL);
    if (p_payload != NULL) {
        ota_parse_dl_url(payload);
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
    ret = ota_snprintf(topic, OTA_MSG_LEN - 1, "/topic/ota/device/%s/%s/%s", name, pk, dn);
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
 * @param[in] pk        product key
 * @param[in] dn        device name
 * @param[in] ver       version string
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
static int ota_transport_inform(char* pk, char* dn, char* ver)
{
    int  ret = 0;
    char msg[OTA_MSG_LEN] = {0};
    ret = ota_snprintf(msg, OTA_MSG_LEN-1, "{\"id\":%d,\"params\":{\"mode\":\"coap\",\"version\":\"%s\"}}", 1, ver);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_coap_publish("request", msg, pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return 0;
}

/**
 * ota_transport_upgrade  OTA subcribe message from Cloud.
 *
 * @param[in] pk        product key
 * @param[in] dn        device name
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
static int ota_transport_upgrade(char* pk, char* dn)
{
    return 0;
}

/**
 * ota_transport_status  OTA report status to Cloud
 *
 * @param[in] pk        product key
 * @param[in] dn        device name
 * @param[in] status    [1-100] percent, [<0] error no.
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old. 
 */
static int ota_transport_status(char* pk, char* dn, int status)
{
    int  ret                = 0;
    char msg[OTA_MSG_LEN]   = {0};
    char *err_str           = "";

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
            default:
                err_str = "OTA undefined failed";
                break;
        }
    }
    ret = ota_snprintf(msg, OTA_MSG_LEN -1, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", 1, status, err_str);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_coap_publish("progress", msg, pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_transport_init  OTA transport init
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
static int ota_transport_init(void)
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
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
static int ota_transport_deinit(void)
{
    int ret = 0;
    ret = ota_coap_deinit();
    if(ret < 0){
        ret = OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}
