/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota/ota_service.h"
#include "ota_hal_os.h"
#include "ota_hal_plat.h"
#include "ota_verify.h"

static int ota_gen_info_msg(char *buf, int len, int id, const char *ver)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"version\":\"%s\"}}", id, ver);
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
static int ota_gen_report_msg(char *buf, int len, int id, int progress, const char *msg)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", id, progress, msg ? msg : NULL);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

/*
* Generate topic name according to @ota_topic_type, @product_key, @device_name
* and then copy to @buf.
* 0, successful; -1, failed
*/
static int ota_mqtt_gen_topic_name(char *buf, int len, const char *topic, char *pk, char *dn)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "/ota/device/%s/%s/%s", topic, pk, dn);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int ota_mqtt_publish(const char *topic, const char *msg, char *pk, char *dn)
{
    int ret = 0;
    char name[OTA_MQTT_TOPIC_LEN] = {0};
    if (topic == NULL || msg == NULL || pk == NULL || dn == NULL) {
        return -1;
    }
    ret = ota_mqtt_gen_topic_name(name, OTA_MQTT_TOPIC_LEN, topic, pk, dn);
    if (ret < 0) {
        return -1;
    }
    OTA_LOG_I("Public name:%s msg:%s", name, msg);
    ret = ota_hal_mqtt_publish(name, 1, (void *)msg, strlen(msg) + 1);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

static void ota_mqtt_sub_cb(void *pcontext, void *pclient, void* msg)
{
    char *payload = NULL;
    if (msg == NULL) {
        return;
    }
    ota_mqtt_msg_t *mqtt_msg = (ota_mqtt_msg_t*)msg;
    switch (mqtt_msg->event) {
        case OTA_MQTT_EVENT_PUB_RECEIVED:
            payload = (char *)mqtt_msg->topic->payload;
            break;
        default:
            return;
    }
    if(payload == NULL) {
        OTA_LOG_E("payload is null");
        return;
    }
    ota_service_t* ctx = (ota_service_t*)pcontext;
    OTA_LOG_I("mqtt cb evt:%d %s", mqtt_msg->event, payload);
    if ((!ctx)||!(ctx->upgrade_cb)) {
        return;
    }
    ctx->upgrade_cb(ctx, payload);
}

static int ota_trans_inform(void* pctx)
{
    int  ret = 0;
    char msg[OTA_MSG_INFORM_LEN] = {0};
    ota_service_t* ctx = pctx;
    if (!ctx) {
        return -1;
    }
    ret = ota_gen_info_msg(msg, OTA_MSG_INFORM_LEN, 0, ctx->sys_ver);
    if (ret != 0) {
        return -1;
    }
    ret = ota_mqtt_publish("inform", msg, ctx->pk, ctx->dn);
    if (0 != ret) {
        return OTA_TRANSPORT_FAIL;
    }
    return ret;
}

static int ota_trans_upgrade(void* pctx)
{
    int   ret = 0;
    char  name[OTA_MQTT_TOPIC_LEN] = {0};
    ota_service_t* ctx = pctx;
    if (!ctx) {
        return -1;
    }
    ret = ota_mqtt_gen_topic_name(name, OTA_MQTT_TOPIC_LEN, "upgrade", ctx->pk, ctx->dn);
    if (ret < 0) {
        return -1;
    }
    OTA_LOG_I("upgrade:%s", name);
    ret = ota_hal_mqtt_subscribe(name, ota_mqtt_sub_cb, pctx);
    if (ret < 0) {
        return OTA_TRANSPORT_FAIL;
    }
    return ret;
}

static int ota_trans_status(int progress, void* pctx)
{
    int  ret = -1;
    char msg[OTA_MSG_REPORT_LEN] = {0};
    char err[OTA_MAX_VER_LEN] = {0};
    ota_service_t* ctx = pctx;
    if (!ctx) {
        return -1;
    }
    int  status = ctx->upg_status;
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
    ret = ota_gen_report_msg(msg, OTA_MSG_REPORT_LEN, 0, progress, err);
    if (0 != ret) {
        return -1;
    }
    ret = ota_mqtt_publish("progress", msg, ctx->pk, ctx->dn);
    if (0 != ret) {
        return OTA_TRANSPORT_FAIL;
    }
    return ret;
}

static int ota_trans_init(void)
{
    return ota_hal_mqtt_init();
}

static int ota_trans_deinit(void)
{
    return ota_hal_mqtt_deinit();
}

static ota_transport_t trans_mqtt = {
    .init             = ota_trans_init,
    .inform           = ota_trans_inform,
    .upgrade          = ota_trans_upgrade,
    .status           = ota_trans_status,
    .deinit           = ota_trans_deinit,
};

ota_transport_t *ota_get_transport(void)
{
    return &trans_mqtt;
}
