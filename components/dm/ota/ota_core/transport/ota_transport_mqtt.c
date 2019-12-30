/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_common.h"

/**
 * ota_mqtt_publish message to Cloud
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
static int ota_mqtt_publish(const char *name, char *msg, char *pk, char *dn)
{
    int ret = 0;
    char topic[OTA_MSG_LEN] = {0};
    if (name == NULL || msg == NULL || pk == NULL || dn == NULL) {
        return OTA_TRANSPORT_INT_FAIL;
    }

    ret = ota_snprintf(topic, OTA_MSG_LEN-1, "/ota/device/%s/%s/%s", name, pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    OTA_LOG_I("Public topic:%s msg:%s", topic, msg);
    ret = ota_hal_mqtt_publish(topic, 1, (void *)msg, strlen(msg) + 1);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_mqtt_sub_cb  upgrade callback
 *
 * @param[in] pctx      ota context
 * @param[in] pclient   mqtt pclient
 * @param[in] msg       mqtt message 
 *
 * @return void
 */
static void ota_mqtt_sub_cb(void *pctx, void *pclient, void *msg)
{
    char *payload = NULL;
    if (msg == NULL) {
        return;
    }
    ota_mqtt_msg_t *mqtt_msg = (ota_mqtt_msg_t *)msg;
    switch (mqtt_msg->event) {
        case OTA_MQTT_EVENT_PUB_RECEIVED:
            payload = (char *)mqtt_msg->topic->payload;
            break;
        default:
            return;
    }
    if(payload != NULL) {
        OTA_LOG_I("mqtt cb evt:%d %s", mqtt_msg->event, payload);
        ota_parse_dl_url(payload);
    }
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
int ota_transport_inform(char* pk, char* dn, char* ver)
{
    int  ret              = 0;
    char msg[OTA_MSG_LEN] = {0};
    ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":%d,\"params\":{\"version\":\"%s\"}}", 0, ver);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_mqtt_publish("inform", msg, pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
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
int ota_transport_upgrade(char* pk, char* dn)
{
    int  ret                = 0;
    char topic[OTA_MSG_LEN] = {0};
    ret = ota_snprintf(topic, OTA_MSG_LEN-1, "/ota/device/%s/%s/%s", "upgrade", pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    OTA_LOG_I("upgrade:%s", topic);
    ret = ota_hal_mqtt_subscribe(topic, ota_mqtt_sub_cb, NULL);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
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
int ota_transport_status(char* pk, char* dn, int status)
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
    ret = ota_mqtt_publish("progress", msg, pk, dn);
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
int ota_transport_init(void)
{
    int ret = 0;
    ret = ota_hal_mqtt_init();
    if(ret < 0){
        return OTA_TRANSPORT_INT_FAIL;
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
int ota_transport_deinit(void)
{
    int ret = 0;
    ret = ota_hal_mqtt_deinit();
    if(ret < 0){
        ret = OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}
