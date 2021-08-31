/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "py_defines.h"
#include "app_message.h"
#include "aos_network.h"
#include "aos/kv.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
//#include "aiot_das_api.h"
#include "aiot_ntp_api.h"
#include "aiot_dm_api.h"
#include "aiot_devinfo_api.h"
#include "app_mgr.h"
#include "cJSON.h"

#define MOD_STR "APP_MESSAGE"

int pyamp_upgrading = 0;
int pyamp_device_token_verify_rejected = 0;
char pyamp_file_url[300] = {0};

/* ntp timestamp */
int64_t pyamp_g_ntp_time = 0;
int64_t pyamp_g_up_time = 0;

int32_t pyamp_update_file(const char *payload)
{
    int32_t ret = -1;
    cJSON *root = NULL, *params = NULL, *url = NULL;

    root = cJSON_Parse(payload);
    if (root == NULL || !cJSON_IsObject(root))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        return ret;
    }
    params = cJSON_GetObjectItem(root, "params");
    if (params == NULL || !cJSON_IsObject(params))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        cJSON_Delete(root);
        return ret;
    }
    do
    {
        url = cJSON_GetObjectItem(params, "url");
        if (url == NULL || !cJSON_IsString(url))
        {
            amp_debug(MOD_STR, "JSON Parse Error");
            cJSON_Delete(root);
            return ret;
        }

        strcpy(pyamp_file_url, url->valuestring);
        // amp_debug(MOD_STR, "get url: %s", url->valuestring);
        if (pyamp_upgrading)
        {
            amp_debug(MOD_STR, "pyamp_upgrading.........");
            return ret;
        }
        pyamp_apppack_upgrade(pyamp_file_url);
    } while (0);

    cJSON_Delete(root);

    return STATE_SUCCESS;
}

int32_t pyamptoken_verify(const char *payload)
{
    int32_t ret = -1;
    char token_flag[AMP_DEVICE_TOKEN_LENGTH] = {0};
    int token_len = AMP_DEVICE_TOKEN_LENGTH;
    cJSON *root = NULL, *params = NULL, *token = NULL;

    root = cJSON_Parse(payload);
    if (root == NULL || !cJSON_IsObject(root))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        return ret;
    }
    params = cJSON_GetObjectItem(root, "params");
    if (params == NULL || !cJSON_IsObject(params))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        cJSON_Delete(root);
        return ret;
    }

    token = cJSON_GetObjectItem(params, "token");
    if (token == NULL || !cJSON_IsString(token))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        cJSON_Delete(root);
        return ret;
    }
    // strcpy(pyamp_file_url, token->valuestring);
    amp_debug(MOD_STR, "get token flag is: %s", token->valuestring);
    aos_kv_set(AMP_DEVICE_TOKEN_VERIFY_FLAG, token->valuestring, strlen(token->valuestring), 1);
    if (strcmp(token->valuestring, "rejected") == 0)
    {
        amp_debug(MOD_STR, "token verify failed, destroy pclient");
        pyamp_device_token_verify_rejected = 1;
    }

    cJSON_Delete(root);

    return STATE_SUCCESS;
}

void pyamp_putfile_message_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    switch (packet->type)
    {
    case AIOT_MQTTRECV_PUB:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "pub, qos: %d, len: %d, topic: %s  \r\n", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
        amp_debug(MOD_STR, "pub, len: %d, payload: %s  \r\n", packet->data.pub.payload_len, packet->data.pub.payload);
        pyamp_update_file(packet->data.pub.payload);
        break;
    default:
        break;
    }
}

void pyamp_verify_message_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    switch (packet->type)
    {
    case AIOT_MQTTRECV_PUB:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "pub, qos: %d, len: %d, topic: %.*s", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
        amp_debug(MOD_STR, "pub, len: %d, payload: %.*s", packet->data.pub.payload_len, packet->data.pub.payload);
        pyamptoken_verify(packet->data.pub.payload);
        break;
    default:
        break;
    }
}

static int32_t topic_subscribe(void *mqtt_handle, const char *fmt, void *callback)
{
    int res = STATE_SUCCESS;
    int topic_len = 0;
    char *topic = NULL;
    char amp_internal_productkey[IOTX_PRODUCT_KEY_LEN] = {0};
    char amp_internal_devicename[IOTX_DEVICE_NAME_LEN] = {0};
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;

    aos_kv_get(AMP_INTERNAL_PRODUCTKEY, amp_internal_productkey, &productkey_len);
    aos_kv_get(AMP_INTERNAL_DEVICENAME, amp_internal_devicename, &devicename_len);

    topic_len = strlen(fmt) + strlen(amp_internal_productkey) + strlen(amp_internal_devicename) + 1;
    topic = aos_malloc(topic_len);
    if (topic == NULL)
    {
        amp_debug(MOD_STR, "memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    aos_snprintf(topic, topic_len, fmt, amp_internal_productkey, amp_internal_devicename);

    res = aiot_mqtt_sub(mqtt_handle, topic, callback, 0, NULL);
    if (res < STATE_SUCCESS)
    {
        amp_debug(MOD_STR, "subscribe failed");
        aos_free(topic);
        return -1;
    }

    aos_free(topic);
    return 0;
}

static int32_t topic_publish(void *handle, const char *fmt, char *payload)
{
    int res = STATE_SUCCESS;
    char *topic = NULL;
    int topic_len = 0;
    char amp_internal_productkey[IOTX_PRODUCT_KEY_LEN] = {0};
    char amp_internal_devicename[IOTX_DEVICE_NAME_LEN] = {0};
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;

    aos_kv_get(AMP_INTERNAL_PRODUCTKEY, amp_internal_productkey, &productkey_len);
    aos_kv_get(AMP_INTERNAL_DEVICENAME, amp_internal_devicename, &devicename_len);

    topic_len = strlen(fmt) + strlen(amp_internal_productkey) + strlen(amp_internal_devicename) + 1;
    topic = aos_malloc(topic_len);
    if (topic == NULL)
    {
        amp_debug(MOD_STR, "memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    aos_snprintf(topic, topic_len, fmt, amp_internal_productkey, amp_internal_devicename);

    res = aiot_mqtt_pub(handle, topic, payload, strlen(payload), 0);
    if (res < STATE_SUCCESS)
    {
        amp_debug(MOD_STR, "publish failed, res = %d", res);
        aos_free(topic);
        return -1;
    }

    aos_free(topic);
    return 0;
}

int32_t pyamp_internal_service_subscribe(void *mqtt_handle)
{
    int res = STATE_SUCCESS;

    const char *update_fmt = "/sys/%s/%s/thing/service/putfile";
    const char *verify_fmt = "/sys/%s/%s/thing/service/verify";

    res = topic_subscribe(mqtt_handle, update_fmt, pyamp_putfile_message_handler);
    if (res < STATE_SUCCESS)
    {
        amp_debug(MOD_STR, "subscribe update topic failed");
        return -1;
    }

    res = topic_subscribe(mqtt_handle, verify_fmt, pyamp_verify_message_handler);
    if (res < STATE_SUCCESS)
    {
        amp_debug(MOD_STR, "subscribe update topic failed");
        return -1;
    }

    return res;
}

#define PROP_POST_FORMAT_TOKEN "{\"params\":{\"token\":\"%s\"}}"
int32_t pyamp_internal_service_publish(void *mqtt_handle)
{
    int res = 0;
    char token_content[AMP_DEVICE_TOKEN_LENGTH] = {0};
    int token_len = AMP_DEVICE_TOKEN_LENGTH;
    const char *verify_fmt = "/sys/%s/%s/thing/event/property/post";
    char *payload = NULL;

    payload = aos_malloc(128);
    if (payload == NULL)
    {
        amp_debug(MOD_STR, "memory not enough");
        return -1;
    }
    memset(payload, 0, 128);
    aos_kv_get(AMP_DEVICE_TOKEN, token_content, &token_len);
    aos_snprintf(payload, 128, PROP_POST_FORMAT_TOKEN, token_content);

    res = topic_publish(mqtt_handle, verify_fmt, payload);
    if (res < STATE_SUCCESS)
    {
        amp_debug(MOD_STR, "publish failed");
        aos_free(payload);
        return -1;
    }

    aos_free(payload);
    return res;
}

#if 0
/* das service */
int32_t amp_das_service(void *mqtt_handle)
{
    int32_t res = STATE_SUCCESS;
    void *das_handle = NULL;

    das_handle = aiot_das_init();
    if (das_handle == NULL) {
        amp_debug(MOD_STR, "das service init failed");
        return -1;
    }

    res = aiot_das_setopt(das_handle, AIOT_DASOPT_MQTT_HANDLE, mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "das set option failed");
        aiot_das_deinit(&das_handle);
        return -1;
    }

    res = aiot_das_stepping(das_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "das stepping failed");
        aiot_das_deinit(&das_handle);
        return -1;
    }

    return res;
}
#endif

static void aiot_ntp_recv_handler(void *handle, const aiot_ntp_recv_t *packet, void *userdata)
{
    switch (packet->type)
    {
    case AIOT_NTPRECV_LOCAL_TIME:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "year: %d, month: %d, day: %d, hour: %d, min: %d, sec: %d, msec: %d, timestamp: %d",
                packet->data.local_time.year,
                packet->data.local_time.mon,
                packet->data.local_time.day,
                packet->data.local_time.hour,
                packet->data.local_time.min,
                packet->data.local_time.sec,
                packet->data.local_time.msec,
                packet->data.local_time.timestamp
        );

        pyamp_g_ntp_time = packet->data.local_time.timestamp;
        pyamp_g_up_time = aos_now_ms();

        break;
    default:
        break;
    }
}

static void aiot_ntp_event_handler(void *handle, const aiot_ntp_event_t *event, void *userdata)
{
    switch (event->type)
    {
    case AIOT_NTPEVT_INVALID_RESPONSE:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "ntp receive data invalid");
        break;
    case AIOT_NTPEVT_INVALID_TIME_FORMAT:
        amp_debug(MOD_STR, "ntp receive data error");
        break;
    default:
        break;
    }
}

/* ntp service */
int32_t pyamp_ntp_service(void *mqtt_handle)
{
    int32_t res = STATE_SUCCESS;
    int32_t time_zone = 8;
    void *ntp_handle = NULL;

    static int ntp_enable = 0;

    if (ntp_enable) {
        return res;
    }

    ntp_handle = aiot_ntp_init();
    if (ntp_handle == NULL) {
        amp_debug(MOD_STR, "ntp service init failed");
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_MQTT_HANDLE, (void *)mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "ntp set mqtt handle failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_TIME_ZONE, (void *)&time_zone);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "ntp set time zone failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_RECV_HANDLER, (void *)aiot_ntp_recv_handler);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "ntp set receive handler failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_EVENT_HANDLER, (void *)aiot_ntp_event_handler);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "das set event handler failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_send_request(ntp_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "das stepping failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    ntp_enable = 1;
    return res;
}

/* 定位事件上报 */
static int32_t location_event_post(void *dm_handle, char *event_id, char *params)
{
    aiot_dm_msg_t msg;

    memset(&msg, 0, sizeof(aiot_dm_msg_t));
    msg.type = AIOT_DMMSG_EVENT_POST;
    msg.data.event_post.event_id = event_id;
    msg.data.event_post.params = params;

    return aiot_dm_send(dm_handle, &msg);
}

int32_t pyamp_location_service(void *mqtt_handle)
{
    int32_t res = STATE_SUCCESS;
    void *dm_handle = NULL;
    aos_locator_info_t locator_info;
    char *bts = NULL;
    char *lbs_info = NULL;

    res = aos_get_locator_info(&locator_info);
    if (res != STATE_SUCCESS) {
        amp_debug(MOD_STR, "get network info failed");
        return -1;
    }

    bts = (char *)aos_malloc(64);
    if (bts == NULL) {
        amp_debug(MOD_STR, "network info malloc failed");
        return -1;
    }

    /* bts info format */
    res = snprintf(bts,
                    64,
                    "%s,%s,%d,%d,%d",
                    locator_info.mcc,
                    locator_info.mnc,
                    locator_info.lac,
                    locator_info.cellid,
                    locator_info.signal
                    );
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "lbs info generate failed");
        aos_free(bts);
        return -1;
    }

    amp_debug(MOD_STR, "lbs info is: %s", bts);

    lbs_info = (char *)aos_malloc(256);
    if (lbs_info == NULL) {
        amp_debug(MOD_STR, "network info malloc failed");
        return -1;
    }

    /* lbs info format */
    res = snprintf(lbs_info,
                    64,
                    LBS_INFO_FMT,
                    bts,
                    bts
                    );
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "lbs info generate failed");
        aos_free(lbs_info);
        return -1;
    }

    /* 创建DATA-MODEL实例 */
    dm_handle = aiot_dm_init();
    if (dm_handle == NULL) {
        amp_debug(MOD_STR, "dm handle init failed");
        return -1;
    }

    /* 配置MQTT实例句柄 */
    aiot_dm_setopt(dm_handle, AIOT_DMOPT_MQTT_HANDLE, mqtt_handle);

    char *lbs_event_id = "LocationInfo";

    location_event_post(dm_handle, lbs_event_id, "{\"ErrorCode\": 0}");

    aos_free(bts);
    aos_free(lbs_info);

    return res;
}

/* device activition info report */
int32_t pyamp_devinfo_report_service(void *mqtt_handle)
{
    int32_t res = STATE_SUCCESS;
    void *devinfo_handle = NULL;
    aiot_devinfo_msg_t *devinfo = NULL;
    char *msg = NULL;
    int32_t msg_len = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN] = {0};
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;

    devinfo_handle = aiot_devinfo_init();
    if (devinfo_handle == NULL) {
        amp_debug(MOD_STR, "ntp service init failed");
        return -1;
    }

    res = aiot_devinfo_setopt(devinfo_handle, AIOT_DEVINFOOPT_MQTT_HANDLE, (void *)mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "devinfo set mqtt handle failed");
        aiot_devinfo_deinit(&devinfo_handle);
        return -1;
    }

    aos_kv_get(AMP_INTERNAL_PRODUCTKEY, product_key, &productkey_len);
    aos_kv_get(AMP_INTERNAL_DEVICENAME, device_name, &devicename_len);

    msg_len = strlen(DEVICE_INFO_UPDATE_FMT) + 32;
    msg = (char *)aos_malloc(msg_len);
    if (msg == NULL) {
        amp_debug(MOD_STR, "malloc msg err");
        return -1;
    }
    memset(msg, 0, msg_len);

    /* devinfo update message */
    res = snprintf(msg,
                       msg_len,
                       DEVICE_INFO_UPDATE_FMT,
                       APPLICATION,
                       MODULE_NAME
                      );
    if (res <= 0) {
        amp_debug(MOD_STR, "topic msg generate err");
        aos_free(msg);
        return -1;
    }

    devinfo = aos_malloc(sizeof(aiot_devinfo_msg_t));
    if (devinfo == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        return -1;
    }
    memset(devinfo, 0, sizeof(aiot_devinfo_msg_t));

    devinfo->product_key = aos_malloc(IOTX_PRODUCT_KEY_LEN);
    if (devinfo->product_key == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        aos_free(devinfo);
        return -1;
    }
    memset(devinfo->product_key, 0, IOTX_PRODUCT_KEY_LEN);

    devinfo->device_name = aos_malloc(IOTX_DEVICE_NAME_LEN);
    if (devinfo->device_name == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        aos_free(devinfo);
        return -1;
    }
    memset(devinfo->device_name, 0, IOTX_DEVICE_NAME_LEN);

    devinfo->data.update.params = aos_malloc(msg_len);
    if (devinfo == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        aos_free(devinfo);
        return -1;
    }
    memset(devinfo->data.update.params, 0, msg_len);

    devinfo->type = AIOT_DEVINFO_MSG_UPDATE;
    memcpy(devinfo->product_key, product_key, strlen(product_key));
    memcpy(devinfo->device_name, device_name, strlen(device_name));
    memcpy(devinfo->data.update.params, msg, msg_len);

    res = aiot_devinfo_send(devinfo_handle, devinfo);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "das stepping failed");
        aos_free(msg);
        aos_free(devinfo->product_key);
        aos_free(devinfo->device_name);
        aos_free(devinfo->data.update.params);
        aos_free(devinfo);
        aiot_devinfo_deinit(&devinfo_handle);
        return -1;
    }

    aos_free(msg);
    aos_free(devinfo->product_key);
    aos_free(devinfo->device_name);
    aos_free(devinfo->data.update.params);
    aos_free(devinfo);

    return res;
}
