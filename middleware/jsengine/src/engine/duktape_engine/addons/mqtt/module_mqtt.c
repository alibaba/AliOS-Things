/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 *  TODO：MQTT.getDeviceSecert & MQTT.sign
 */

#include <stdarg.h>

#include "jse_common.h"
#include "be_inl.h"

#include "linkkit/dev_sign_api.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/mqtt_api.h"
#include "linkkit/wrappers/wrappers.h"

#if 0
#define MSG_LEN_MAX 2048
#define IOT_MQTT_TOPIC_NAME_SIZE (128 + 1)

typedef struct {
    struct be_list_head list;
    int js_cb_ref;
    char topic[IOT_MQTT_TOPIC_NAME_SIZE];
} topic_process_t;
#endif

static void *pclient = NULL;

struct mqtt_sub_cb_param {
    char *payload;
    int payload_len;
    char *topic;
    int topic_len;
    int js_cb_ref;
};

static void mqtt_sub_topic_notify(void *data)
{
    struct mqtt_sub_cb_param *p = (struct mqtt_sub_cb_param *)data;
    duk_context *ctx            = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_lstring(ctx, p->payload, p->payload_len);
    duk_json_decode(ctx, -1);
    duk_push_lstring(ctx, p->topic, p->topic_len);
    duk_pcall(ctx, 2);
    duk_pop(ctx);
    jse_free(p->payload);
    jse_free(p->topic);
    jse_free(p);
}

static void mqtt_sub_callback(void *pcontext, void *pclient,
                              iotx_mqtt_event_msg_pt msg)
{
    struct mqtt_sub_cb_param *p        = NULL;
    iotx_mqtt_topic_info_t *topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            jse_debug(
                "receive subscribe message, topic[%d]: %s, payload[%d]: %s\n",
                topic_info->topic_len, topic_info->ptopic,
                topic_info->payload_len, topic_info->payload);

            /* save payload content */
            if ((p = (struct mqtt_sub_cb_param *)jse_malloc(
                     sizeof(struct mqtt_sub_cb_param))) == NULL) {
                jse_warn("allocate memory failed\n");
                return;
            }
            memset(p, 0, sizeof(struct mqtt_sub_cb_param));

            if ((p->payload = (char *)jse_malloc(topic_info->payload_len)) ==
                NULL) {
                jse_warn("allocate payload memory failed\n");
                jse_free(p);
                return;
            }
            memcpy(p->payload, topic_info->payload, topic_info->payload_len);
            p->payload_len = topic_info->payload_len;

            if ((p->topic = (char *)jse_malloc(topic_info->topic_len)) ==
                NULL) {
                jse_warn("allocate payload memory failed\n");
                jse_free(p->payload);
                jse_free(p);
                return;
            }
            memcpy(p->topic, topic_info->ptopic, topic_info->topic_len);
            p->topic_len = topic_info->topic_len;

            p->js_cb_ref = (int)pcontext;
            /* subscribe callback function */
            int ret = jse_task_schedule_call(mqtt_sub_topic_notify, p);
            if (ret < 0) {
                jse_warn("jse_task_schedule_call failed\n");
                jse_free(p->payload);
                jse_free(p->topic);
                jse_free(p);
            }
            break;
        default:
            break;
    }
}

#if 0
#define CM_READ_ONLY
static const char string_SHA_METHOD[] CM_READ_ONLY = "hmacsha1";
static const char string_MD5_METHOD[] CM_READ_ONLY = "hmacmd5";
static const char string_TIMESTAMP[] CM_READ_ONLY  = "2524608000000";

static const char string_AUTH_URL[] CM_READ_ONLY =
    "https://iot-auth.cn-shanghai.aliyuncs.com/auth/register/device";

static const char string_AUTH_CONTENT_TYPE[] CM_READ_ONLY =
    "application/x-www-form-urlencoded";
static const char string_hmac_format[] CM_READ_ONLY =
    "deviceName%s"
    "productKey%s"
    "random%s";
static const char string_auth_req_format[] CM_READ_ONLY =
    "productKey=%s&"
    "deviceName=%s&"
    "signMethod=%s&"
    "sign=%s&"
    "version=default&"
    "clientId=%s&"
    "random=%s&"
    "resources=mqtt";
static const int int_random_length = 15;

#define CM_AUTH_TIMEOUT (10 * 1000)

extern uint32_t HAL_Random(uint32_t region);
static char *genRandomString(int length)
{
    int flag, i;
    char *str;
    if ((str = (char *)jse_malloc(length)) == NULL) {
        return NULL;
    }

    for (i = 0; i < length - 1; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                str[i] = 'A' + HAL_Random(26);
                break;
            case 1:
                str[i] = 'a' + HAL_Random(26);
                break;
            case 2:
                str[i] = '0' + HAL_Random(10);
                break;
            default:
                str[i] = 'x';
                break;
        }
    }
    str[length - 1] = '\0';
    return str;
}

int _http_response(char *payload, const int payload_len,
                   const char *request_string, const char *url,
                   const int port_num, const char *pkey)
{
#define HTTP_POST_MAX_LEN (1024)
#define HTTP_RESP_MAX_LEN (1024)

    int ret            = SUCCESS_RETURN;
    char *requ_payload = NULL;
    char *resp_payload = NULL;
    httpclient_t httpc;
    httpclient_data_t httpc_data;

    memset(&httpc, 0, sizeof(httpclient_t));
    memset(&httpc_data, 0, sizeof(httpclient_data_t));

    /* todo */
    httpc.header =
        "Accept: text/xml,text/javascript,text/html,application/json\r\n";

    requ_payload = (char *)jse_malloc(HTTP_POST_MAX_LEN);
    if (NULL == requ_payload) {
        return ERROR_MALLOC;
    }
    memset(requ_payload, 0, HTTP_POST_MAX_LEN);

    snprintf(requ_payload, HTTP_POST_MAX_LEN, "%s", request_string);

    resp_payload = (char *)jse_malloc(HTTP_RESP_MAX_LEN);
    if (!resp_payload) {
        ret = FAIL_RETURN;
        goto RETURN;
    }

    memset(resp_payload, 0, HTTP_RESP_MAX_LEN);

    httpc_data.post_content_type = (char *)string_AUTH_CONTENT_TYPE;
    httpc_data.post_buf          = requ_payload;
    httpc_data.post_buf_len      = strlen(requ_payload);
    httpc_data.response_buf      = resp_payload;
    httpc_data.response_buf_len  = HTTP_RESP_MAX_LEN;

    ret = httpclient_common(&httpc, url, port_num, pkey, HTTPCLIENT_POST,
                            CM_AUTH_TIMEOUT, &httpc_data);
    if (ret != 0) {
        ret = FAIL_RETURN;
        goto RETURN;
    }

    memcpy(payload, httpc_data.response_buf, payload_len);

RETURN:
    if (requ_payload) {
        jse_free(requ_payload);
        requ_payload = NULL;
    }
    if (resp_payload) {
        jse_free(resp_payload);
        resp_payload = NULL;
    }

    return ret;
}

static char *_set_auth_req_str(const char *product_key, const char *device_name,
                               const char *client_id, const char *sign,
                               const char *ts)
{
#define AUTH_STRING_MAXLEN (1024)

    char *req = NULL;

    req = jse_malloc(AUTH_STRING_MAXLEN);
    memset(req, 0, AUTH_STRING_MAXLEN);

    snprintf(req, AUTH_STRING_MAXLEN, string_auth_req_format, product_key,
             device_name, string_SHA_METHOD, sign, client_id, ts);
    return req;
}

static char *_get_device_secret(const char *product_key,
                                const char *device_name, const char *client_id,
                                const char *guider_addr,
                                const char *request_string)
{
    char payload[512]  = {0};
    char *deviceSecret = NULL;

    payload[511] = '\0';
    strncpy(payload, request_string, sizeof(payload) - 1);

    jse_debug("[%s][%d] request_string=%s \n", __FUNCTION__, __LINE__,
           request_string);
    jse_debug("[%s][%d] guider_addr=%s \n", __FUNCTION__, __LINE__, guider_addr);

    _http_response(payload, sizeof(payload), request_string, guider_addr, 443,
                   iotx_ca_get());

    jse_debug("payload = %s \n", payload);

    cJSON *root = NULL;
    cJSON *item;

    root = cJSON_Parse(payload);
    if (root == NULL) {
        goto do_exit;
    }

    item = cJSON_GetObjectItem(root, "code");
    if (cJSON_IsNumber(item)) {
        if (item->valuedouble != 200) {
            goto do_exit;
        }

    } else if (cJSON_IsString(item)) {
        if (strcmp(item->valuestring, "200") != 0) {
            goto do_exit;
        }
    } else {
        goto do_exit;
    }

    item = cJSON_GetObjectItem(root, "data");
    if (item == NULL) goto do_exit;

    cJSON *deviceSecretItem = cJSON_GetObjectItem(item, "deviceSecret");
    if (cJSON_IsString(deviceSecretItem)) {
        /* get deviceSecret */
        deviceSecret = strdup(deviceSecretItem->valuestring);
        jse_debug("get deviceSecret %s\n", deviceSecret);
    }

do_exit:
    cJSON_Delete(root);
    return deviceSecret;
}

static int _calc_hmac_signature(const char *product_key,
                                const char *product_secret,
                                const char *device_name, char *hmac_sigbuf,
                                const int hmac_buflen, const char *random)
{
    char signature[64];
    char hmac_source[512];
    int ret = FAIL_RETURN;

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));

    ret = snprintf(hmac_source, sizeof(hmac_source), string_hmac_format,
                   device_name, product_key, random);

    utils_hmac_sha1(hmac_source, strlen(hmac_source), signature, product_secret,
                    strlen(product_secret));

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return ret;
}

char *iot_get_deviceSecret(const char *product_key, const char *product_secret,
                           const char *device_name, const char *client_id)
{
    char *req_str        = NULL;
    char guider_sign[48] = {0};
    char *s_random       = NULL;
    char *deviceSecret   = NULL;

    s_random = strdup("012345678901234");
    /* s_random = genRandomString(int_random_length); */
    /* TODO:   string_TIMESTAMP -> random */
    _calc_hmac_signature(product_key, product_secret, device_name, guider_sign,
                         sizeof(guider_sign), s_random);

    jse_debug("guider_sign = \n");
    for (int i = 0; i < 40; i++) jse_debug("0x%x ", guider_sign[i]);
    jse_debug("\n");

    req_str = _set_auth_req_str(product_key, device_name, client_id,
                                guider_sign, s_random);

    deviceSecret = _get_device_secret(product_key, device_name, client_id,
                                      string_AUTH_URL, req_str);
    if (req_str) jse_free(req_str);
    if (s_random) jse_free(s_random);
    return deviceSecret;
}

typedef struct {
    char productKey[16];
    char deviceName[32];
    char productSecret[48];
    char deviceSecret[48];
    int js_cb_ref;
} IOT_DEVICESECRET_s;

static void *mqtt_task_handle = NULL;

void mqtt_notify_jse(void *arg)
{
    IOT_DEVICESECRET_s *iotDeviceSecret = (IOT_DEVICESECRET_s *)arg;
    duk_context *ctx                    = be_get_context();
    be_push_ref(ctx, iotDeviceSecret->js_cb_ref);
    if (iotDeviceSecret->deviceSecret[0]) {
        duk_push_null(ctx);
        duk_push_string(ctx, iotDeviceSecret->deviceSecret);
    } else {
        duk_push_string(ctx, "something error");
        duk_push_null(ctx);
    }
    duk_pcall(ctx, 2);
    duk_pop(ctx);
    be_unref(ctx, iotDeviceSecret->js_cb_ref);
    jse_free(iotDeviceSecret);
}

static void mqtt_get_secret_task(void *arg)
{
    /*
     * ProductKey: a1jbUxTP2VU
     * ProductSecret: VtuTmAMhR8QXfMa5
     */

    /* auth */
    IOT_DEVICESECRET_s *iotDeviceSecret = (IOT_DEVICESECRET_s *)arg;
    char *deviceSecret                  = NULL;
    /* only test ...
     * deviceSecret = iot_get_deviceSecret("a1jbUxTP2VU", "VtuTmAMhR8QXfMa5",
     *                               "123456", "a1jbUxTP2VU.123456");
     */
    char client_id[128];
    snprintf(client_id, sizeof(client_id), "%s.%s", iotDeviceSecret->productKey,
             iotDeviceSecret->deviceName);
    deviceSecret = iot_get_deviceSecret(iotDeviceSecret->productKey,
                                        iotDeviceSecret->productSecret,
                                        iotDeviceSecret->deviceName, client_id);
    if (deviceSecret)
        strcpy(iotDeviceSecret->deviceSecret, deviceSecret);
    else
        iotDeviceSecret->deviceSecret[0] = 0;

    if (jse_task_schedule_call(mqtt_notify_jse, iotDeviceSecret)) {
        jse_warn("jse_task_schedule_call failed\n");
        duk_context *ctx = be_get_context();
        be_unref(ctx, iotDeviceSecret->js_cb_ref);
        jse_free(deviceSecret);
    }

    jse_osal_delete_task(mqtt_task_handle);
}

static duk_ret_t native_get_device_secret(duk_context *ctx)
{
    int err;

    /* check parameter */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        jse_warn("parameter must be object and function\n");
        err = -1;
        goto out;
    }

    /* busy */
    if (mqtt_task_handle) {
        jse_warn("Resource busy\n");
        err = -2;
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "productSecret");
    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) ||
        !duk_is_string(ctx, -1)) {
        jse_warn("Parameter 1 must be an object like {productKey: string, "
            "deviceName: string, productSecret: string}\n");
        err = -3;
        goto pop_out;
    }

    /* create process task and set callback */
    IOT_DEVICESECRET_s *iotDeviceSecret =
        (IOT_DEVICESECRET_s *)jse_calloc(1, sizeof(*iotDeviceSecret));
    if (!iotDeviceSecret) {
        jse_warn("allocate memory failed\n");
        err = -4;
        goto pop_out;
    }
    const char *productKey    = duk_get_string(ctx, -3);
    const char *deviceName    = duk_get_string(ctx, -2);
    const char *productSecret = duk_get_string(ctx, -1);
    jse_debug("productKey: %s, deviceName: %s\n", productKey, deviceName);
    strncpy(iotDeviceSecret->productKey, productKey,
            sizeof(iotDeviceSecret->productKey) - 1);
    strncpy(iotDeviceSecret->deviceName, deviceName,
            sizeof(iotDeviceSecret->deviceName) - 1);
    strncpy(iotDeviceSecret->productSecret, productSecret,
            sizeof(iotDeviceSecret->productSecret) - 1);
    jse_osal_create_task("mqtt network task", mqtt_get_secret_task,
                        iotDeviceSecret, 1024 * 7, MQTTHTTP_TSK_PRIORITY,
                        &mqtt_task_handle);
    if (!mqtt_task_handle) {
        jse_warn("jse_osal_create_task failed\n");
        jse_free(iotDeviceSecret);
        err = -5;
    } else {
        duk_dup(ctx, 1);
        iotDeviceSecret->js_cb_ref = be_ref(ctx);
        err                        = 0;
    }

pop_out:
    duk_pop_n(ctx, 3);

out:
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_device_info(duk_context *ctx)
{
    char *productKey   = NULL;
    char *deviceName   = NULL;
    char *deviceSecret = NULL;

    duk_push_object(ctx);
    board_getDeviceInfo(&productKey, &deviceName, &deviceSecret);
    if (productKey) {
        duk_push_string(ctx, productKey);
        duk_put_prop_string(ctx, -2, "productKey");
    }
    if (deviceName) {
        duk_push_string(ctx, deviceName);
        duk_put_prop_string(ctx, -2, "deviceName");
    }
    if (deviceSecret) {
        duk_push_string(ctx, deviceSecret);
        duk_put_prop_string(ctx, -2, "deviceSecret");
    }
    jse_free(productKey);
    jse_free(deviceName);
    jse_free(deviceSecret);
    return 1;
}

static duk_ret_t native_mqtt_sign(duk_context *ctx)
{
    if (!duk_is_object(ctx, 0)) {
        jse_warn("parameter must be object\n");
        duk_push_null(ctx);
        return 1;
    }

    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "deviceSecret");
    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) ||
        !duk_is_string(ctx, -1)) {
        jse_warn("Parameter 1 must be an object like {productKey: string, deviceName: string, deviceSecret: string}\n");
        duk_pop_n(ctx, 3);
        duk_push_null(ctx);
        return 1;
    }
    const char *productKey     = duk_get_string(ctx, -3);
    const char *deviceName     = duk_get_string(ctx, -2);
    const char *deviceSecret   = duk_get_string(ctx, -1);
    unsigned char content[256] = {0};
    snprintf((char *)content, sizeof(content),
             "clientId%sdeviceName%sproductKey%s", deviceName, deviceName,
             productKey);

    unsigned char k_ipad[64];
    unsigned char k_opad[64];
    unsigned char out[20] = {0};

    strncpy((char *)k_ipad, deviceSecret, sizeof(k_ipad));
    strncpy((char *)k_opad, deviceSecret, sizeof(k_opad));

    int i;
    for (i = 0; i < sizeof(k_ipad); i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    jse_sha1_context sha1_ctx;
    jse_sha1_init(&sha1_ctx);
    jse_sha1_starts(&sha1_ctx);
    jse_sha1_update(&sha1_ctx, k_ipad, sizeof(k_ipad));
    jse_sha1_update(&sha1_ctx, content, strlen((char *)content));
    jse_sha1_finish(&sha1_ctx, out);
    jse_sha1_free(&sha1_ctx);

    jse_sha1_init(&sha1_ctx);
    jse_sha1_starts(&sha1_ctx);
    jse_sha1_update(&sha1_ctx, k_opad, sizeof(k_opad));
    jse_sha1_update(&sha1_ctx, out, sizeof(out));
    jse_sha1_finish(&sha1_ctx, out);
    jse_sha1_free(&sha1_ctx);

    int j;
    char sign[41];
    for (i = 0; i < sizeof(out); ++i) {
        unsigned char byte[2];
        byte[0] = out[i] / 16;
        byte[1] = out[i] % 16;

        for (j = 0; j < 2; ++j) {
            if (byte[j] >= 0 && byte[j] <= 9)
                sign[2 * i + j] = '0' + byte[j];
            else
                sign[2 * i + j] = 'a' + byte[j] - 10;
        }
    }
    sign[40] = 0;

    duk_pop_n(ctx, 3);
    duk_push_string(ctx, sign);
    return 1;
}
#endif

static void mqtt_start_notify(void *arg)
{
    int js_cb_ref    = (int)arg;
    duk_context *ctx = be_get_context();

    be_push_ref(ctx, js_cb_ref);
    duk_push_null(ctx);
    duk_pcall(ctx, 1);
    duk_pop(ctx);
    be_unref(ctx, js_cb_ref);
}

static void mqtt_event_handle(void *pcontext, void *pclient,
                              iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id                = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            jse_debug("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            jse_debug("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            jse_debug("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            jse_debug("subscribe success, packet-id=%u",
                      (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            jse_debug("subscribe wait ack timeout, packet-id=%u",
                      (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            jse_debug("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            jse_debug("unsubscribe success, packet-id=%u",
                      (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            jse_debug("unsubscribe timeout, packet-id=%u",
                      (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            jse_debug("unsubscribe nack, packet-id=%u",
                      (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            jse_debug("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            jse_debug("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            jse_debug("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            jse_debug(
                "topic message arrived but without any related handle: "
                "topic=%.*s, topic_msg=%.*s",
                topic_info->topic_len, topic_info->ptopic,
                topic_info->payload_len, topic_info->payload);
            break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            jse_debug("buffer overflow, %s", msg->msg);
            break;

        default:
            jse_debug("Should NOT arrive here.");
            break;
    }
}

static void mqtt_yield_task(void *arg)
{
    int last_mqtt_state = 0;

    while (1) {
        int mqtt_state = IOT_MQTT_CheckStateNormal(pclient);
        if (last_mqtt_state == 0 && mqtt_state == 1) {
            /* notify mqtt connected */
            jse_task_schedule_call(mqtt_start_notify, arg);
            last_mqtt_state = 1;
        }
        IOT_MQTT_Yield(pclient, 200);
    }
}

static duk_ret_t native_mqtt_start(duk_context *ctx)
{
    int err;
    iotx_mqtt_param_t mqtt_params;

    /* mqtt client already running? */
    if (pclient) {
        jse_warn("mqtt client already start\n");
        err = -1;
        goto out;
    }

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        jse_warn("parameter must be object and function\n");
        err = -1;
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "deviceSecret");
    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) ||
        !duk_is_string(ctx, -1)) {
        jse_warn(
            "Parameter 1 must be an object like {productKey: string, "
            "deviceName: string, deviceSecret: string}\n");
        err = -2;
        goto pop_out;
    }
    const char *productKey   = duk_get_string(ctx, -3);
    const char *deviceName   = duk_get_string(ctx, -2);
    const char *deviceSecret = duk_get_string(ctx, -1);
    jse_debug("productKey: %s, deviceName: %s\n", productKey, deviceName);

    HAL_SetProductKey(productKey);
    HAL_SetDeviceName(deviceName);
    HAL_SetDeviceSecret(deviceSecret);

    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.handle_event.h_fp = mqtt_event_handle;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        jse_warn("MQTT construct failed");
        err = -3;
        return -1;
    }

    duk_dup(ctx, 1);
    int js_cb_ref = be_ref(ctx);

    /* create task to IOT_MQTT_Yield() */
    err = jse_osal_create_task("mqtt yield task", mqtt_yield_task,
                              (void *)js_cb_ref, 2048, MQTT_TSK_PRIORITY, NULL);
    if (err) {
        jse_warn("jse_osal_create_task failed\n");
        IOT_MQTT_Destroy(&pclient);
        be_unref(ctx, js_cb_ref);
        err = -4;
        goto pop_out;
    }

    err = 0;
pop_out:
    duk_pop_n(ctx, 3);
out:
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_mqtt_subscribe(duk_context *ctx)
{
    int ret = -1;

    /* check parameter */
    if (!duk_is_string(ctx, 0) || !duk_is_function(ctx, 1)) {
        jse_warn("parameter must be string and function\n");
        goto out;
    }

    const char *topic = duk_get_string(ctx, 0);

    duk_dup(ctx, 1);
    int js_cb_ref = be_ref(ctx);
    jse_debug("subscribe topic: %s, js_cb_ref: %d\n", topic, js_cb_ref);
    ret = IOT_MQTT_Subscribe(pclient, (const char *)topic, IOTX_MQTT_QOS0,
                             mqtt_sub_callback, (int)js_cb_ref);
    if (ret < 0) {
        jse_warn("mqtt_subscribe failed, ret: %d\n", ret);
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_mqtt_unsubscribe(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }

    const char *topic = duk_get_string(ctx, 0);

    ret = IOT_MQTT_Unsubscribe(pclient, (const char *)topic);
    if (ret < 0) {
        jse_warn("mqtt_unsubscribe failed, ret: %d\n", ret);
    }
    jse_debug("unsubscribe topic: %s, ret: %d\n", topic, ret);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_mqtt_publish(duk_context *ctx)
{
    int ret = -1;
    if (!duk_is_string(ctx, 0) || !duk_is_string(ctx, 1)) {
        jse_warn("parameter must be string and string\n");
        goto out;
    }
    const char *topic = duk_get_string(ctx, 0);
    int msg_len;
    const char *msg = duk_get_lstring(ctx, 1, &msg_len);
    jse_debug("topic: %s, msg: %s, msg_len: %d\n", topic, msg, msg_len);
    ret = IOT_MQTT_Publish_Simple(pclient, (const char *)topic, IOTX_MQTT_QOS1,
                                  (void *)msg, msg_len);
    if (ret < 0) {
        jse_warn("mqtt_publish failed, ret: %d\n", ret);
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_mqtt_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_string(ctx, "0.0.2");
    duk_put_prop_string(ctx, -2, "VERSION");

    /*
    duk_push_c_function(ctx, native_device_info, 0);
    duk_put_prop_string(ctx, -2, "device");

    duk_push_c_function(ctx, native_get_device_secret, 2);
    duk_put_prop_string(ctx, -2, "getDeviceSecret");

    duk_push_c_function(ctx, native_mqtt_sign, 1);
    duk_put_prop_string(ctx, -2, "sign"); */

    duk_push_c_function(ctx, native_mqtt_start, 2);
    duk_put_prop_string(ctx, -2, "start");

    duk_push_c_function(ctx, native_mqtt_subscribe, 2);
    duk_put_prop_string(ctx, -2, "subscribe");

    duk_push_c_function(ctx, native_mqtt_unsubscribe, 1);
    duk_put_prop_string(ctx, -2, "unsubscribe");

    duk_push_c_function(ctx, native_mqtt_publish, 2);
    duk_put_prop_string(ctx, -2, "publish");

    duk_put_global_string(ctx, "MQTT");
}
