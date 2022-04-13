/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 *  TODO：MQTT.getDeviceSecert & MQTT.sign
 */

#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"
#include "mqtt_wrapper.h"
#include "mqtt_api.h"
#include "wrappers_defs.h"

#define MOD_STR "MQTT"
struct mqtt_sub_cb_param
{
    char *payload;
    int payload_len;
    char *topic;
    int topic_len;
    int js_cb_ref;
};

typedef struct {
    void *pclient;
    int err;
    int js_cb_ref;
} mqtt_init_param_t;

static int close_flag = 0;
extern int update_flag;

static void mqtt_sub_topic_notify(void *data)
{
    int i = 0;
    struct mqtt_sub_cb_param *p = (struct mqtt_sub_cb_param *)data;
    duk_context *ctx = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_lstring(ctx, p->topic, p->topic_len);
    int arr_idx = duk_push_array(ctx);
    for (i = 0; i < p->payload_len; i++) {
        duk_push_int(ctx, p->payload[i]);
        duk_put_prop_index(ctx, arr_idx, i);
    }
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    // be_unref(ctx, p->js_cb_ref);
    amp_free(p->payload);
    amp_free(p->topic);
    amp_free(p);

    duk_gc(ctx, 0);
}

static void mqtt_sub_callback(void *pcontext, void *pclient,
                              iotx_mqtt_event_msg_pt msg)
{
    struct mqtt_sub_cb_param *p = NULL;
    iotx_mqtt_topic_info_t *topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    char *topic = NULL;

    switch (msg->event_type)
    {
    case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
        topic = (char *)amp_malloc(topic_info->topic_len + 1);
        memset(topic, 0, topic_info->topic_len + 1);
        memcpy(topic, topic_info->ptopic, topic_info->topic_len);
        amp_debug(MOD_STR, 
            "receive subscribe message, topic[%d]: %s, payload[%d]: %s\n",
            topic_info->topic_len, topic,
            topic_info->payload_len, topic_info->payload);
        amp_free(topic);
        /* save payload content */
        if ((p = (struct mqtt_sub_cb_param *)amp_malloc(
                 sizeof(struct mqtt_sub_cb_param))) == NULL)
        {
            amp_warn(MOD_STR, "allocate memory failed\n");
            return;
        }
        memset(p, 0, sizeof(struct mqtt_sub_cb_param));

        if ((p->payload = (char *)amp_malloc(topic_info->payload_len)) ==
            NULL)
        {
            amp_warn(MOD_STR, "allocate payload memory failed\n");
            amp_free(p);
            return;
        }
        memcpy(p->payload, topic_info->payload, topic_info->payload_len);
        p->payload_len = topic_info->payload_len;

        if ((p->topic = (char *)amp_malloc(topic_info->topic_len + 1)) ==
            NULL)
        {
            amp_warn(MOD_STR, "allocate payload memory failed\n");
            amp_free(p->payload);
            amp_free(p);
            return;
        }
        memcpy(p->topic, topic_info->ptopic, topic_info->topic_len + 1);
        p->topic[topic_info->topic_len] = 0;
        p->topic_len = topic_info->topic_len;
        p->js_cb_ref = (int)pcontext;
        /* subscribe callback function */
        int ret = amp_task_schedule_call(mqtt_sub_topic_notify, p);
        if (ret < 0)
        {
            amp_warn(MOD_STR, "amp_task_schedule_call failed\n");
            amp_free(p->payload);
            amp_free(p->topic);
            amp_free(p);
        }
        break;
    default:
        break;
    }
}

static void mqtt_start_notify(void *arg)
{
    mqtt_init_param_t *init_params = (mqtt_init_param_t *)arg;
    duk_context *ctx = be_get_context();

    be_push_ref(ctx, init_params->js_cb_ref);
    duk_push_int(ctx, init_params->err);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    /* free when mqtt connect failed */
    if(init_params->err == -2){
        amp_free(init_params);
        be_unref(ctx, init_params->js_cb_ref);
    }

    duk_gc(ctx, 0);
}

static void mqtt_event_handle(void *pcontext, void *pclient,
                              iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type)
    {
    case IOTX_MQTT_EVENT_UNDEF:
        amp_debug(MOD_STR, "undefined event occur.");
        break;

    case IOTX_MQTT_EVENT_DISCONNECT:
        amp_debug(MOD_STR, "MQTT disconnect.");
        break;

    case IOTX_MQTT_EVENT_RECONNECT:
        amp_debug(MOD_STR, "MQTT reconnect.");
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
        amp_debug(MOD_STR, "subscribe success, packet-id=%u",
                  (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
        amp_debug(MOD_STR, "subscribe wait ack timeout, packet-id=%u",
                  (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
        amp_debug(MOD_STR, "subscribe nack, packet-id=%u", (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
        amp_debug(MOD_STR, "unsubscribe success, packet-id=%u",
                  (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
        amp_debug(MOD_STR, "unsubscribe timeout, packet-id=%u",
                  (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
        amp_debug(MOD_STR, "unsubscribe nack, packet-id=%u",
                  (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
        amp_debug(MOD_STR, "publish success, packet-id=%u", (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
        amp_debug(MOD_STR, "publish timeout, packet-id=%u", (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_PUBLISH_NACK:
        amp_debug(MOD_STR, "publish nack, packet-id=%u", (unsigned int)packet_id);
        break;

    case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
        amp_debug(MOD_STR, 
            "topic message arrived but without any related handle: "
            "topic=%.*s, topic_msg=%.*s",
            topic_info->topic_len, topic_info->ptopic,
            topic_info->payload_len, topic_info->payload);
        break;

    case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
        amp_debug(MOD_STR, "buffer overflow, %s", msg->msg);
        break;

    default:
        amp_debug(MOD_STR, "Should NOT arrive here.");
        break;
    }
}

static void mqtt_yield_task(void *arg)
{
    int err;
    int last_mqtt_state = 0;
    static void *pclient = NULL;
    mqtt_init_param_t *init_params = (mqtt_init_param_t *)arg;

    pclient = init_params->pclient;
    int js_cb_ref = init_params->js_cb_ref;

    if (wrapper_mqtt_connect(pclient) != 0) {
        amp_error(MOD_STR, "wrapper_mqtt_connect failed");
        wrapper_mqtt_release(&pclient);
        /* notify mqtt connect failed */
        init_params->err = -2;
        amp_task_schedule_call(mqtt_start_notify, init_params);
        HAL_ThreadDelete(NULL);
        return;
    }

    close_flag = 0;

    while (1)
    {
        int mqtt_state = wrapper_mqtt_check_state(pclient);
        if (last_mqtt_state == 0 && mqtt_state == 1)
        {
            /* notify mqtt connected */
            init_params->err = 0;
            amp_task_schedule_call(mqtt_start_notify, init_params);
            last_mqtt_state = 1;
        }
        if (last_mqtt_state == 1 && mqtt_state == 0)
        {
            /* notify mqtt disconnect */
            init_params->err = -1;
            amp_task_schedule_call(mqtt_start_notify, init_params);
            last_mqtt_state = 0;
        }
        wrapper_mqtt_yield(pclient, 200);
        if (update_flag || close_flag){
            break;
        }
    }
    wrapper_mqtt_release(&pclient);
    amp_free(init_params);
    HAL_ThreadDelete(NULL);
}

static duk_ret_t native_mqtt_start(duk_context *ctx)
{
    int err;
    static void *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    void *mqtt_task;
    int mqtt_task_stack_used;
    amp_os_thread_param_t mqtt_task_params = {0};

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be object and function\n");
        err = -1;
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "host");
    duk_get_prop_string(ctx, 0, "port");
    duk_get_prop_string(ctx, 0, "client_id");
    duk_get_prop_string(ctx, 0, "username");
    duk_get_prop_string(ctx, 0, "password");
    duk_get_prop_string(ctx, 0, "keepalive_interval");

    if (!duk_is_string(ctx, -6) || !duk_is_number(ctx, -5) ||
        !duk_is_string(ctx, -4) || !duk_is_string(ctx, -3) ||
        !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1))
    {
        amp_warn(MOD_STR, 
            "Parameter 1 must be an object like {host: string, "
            "port: uint, client_id: string, username: string, "
            "password: string, keepalive_interval: uint}\n");
        err = -2;
        duk_pop_n(ctx, 6);
        goto out;
    }

    const char *host = duk_get_string(ctx, -6);
    const uint16_t port = duk_get_number(ctx, -5);
    const char *client_id = duk_get_string(ctx, -4);
    const char *username = duk_get_string(ctx, -3);
    const char *password = duk_get_string(ctx, -2);
    const uint8_t keepalive_interval = duk_get_number(ctx, -1);

    amp_debug(MOD_STR, "host: %s, port: %d\n", host, port);
    amp_debug(MOD_STR, "client_id: %s, username: %s, password: %s\n", client_id, username, password);

    memset(&mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));
    mqtt_params.host = host;
    mqtt_params.port = port;
    mqtt_params.client_id = client_id;
    mqtt_params.username = username;
    mqtt_params.password = password;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;

    mqtt_params.pub_key = NULL;
    mqtt_params.request_timeout_ms    = CONFIG_MQTT_REQUEST_TIMEOUT;
    mqtt_params.clean_session         = 0;
    mqtt_params.keepalive_interval_ms = keepalive_interval * 1000;
    mqtt_params.read_buf_size         = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.write_buf_size        = CONFIG_MQTT_MESSAGE_MAXLEN;

    pclient = wrapper_mqtt_init(&mqtt_params);
    if (NULL == pclient)
    {
        amp_warn(MOD_STR, "MQTT init failed");
        err = -3;
        goto out;
    }

    mqtt_init_param_t *init_params =
        (mqtt_init_param_t *)amp_malloc(sizeof(*init_params));
    if (!init_params) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    duk_dup(ctx, 1);
    init_params->js_cb_ref = be_ref(ctx);
    init_params->pclient = pclient;

    /* create task to IOT_MQTT_Yield() */
    mqtt_task_params.name = "amp mqtt task";
    mqtt_task_params.priority = MQTT_TSK_PRIORITY;
    mqtt_task_params.stack_size = 1024 * 2;

    err = HAL_ThreadCreate(&mqtt_task, mqtt_yield_task, init_params, &mqtt_task_params, &mqtt_task_stack_used);
    if (err)
    {
        amp_warn(MOD_STR, "jse_osal_create_task failed\n");
        wrapper_mqtt_release(&pclient);
        be_unref(ctx, init_params->js_cb_ref);
        amp_free(init_params);
        err = -4;
        goto out;
    }

    err = 0;
    duk_push_pointer(ctx, pclient);
    return 1;
out:
    duk_push_pointer(ctx, NULL);
    return 1;
}

static duk_ret_t native_mqtt_subscribe(duk_context *ctx)
{
    int ret = -1;

    /* check parameter */
    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_number(ctx, 2) || !duk_is_function(ctx, 3))
    {
        amp_warn(MOD_STR, "parameter must be pointer string and function\n");
        goto out;
    }

    void *pclient = duk_get_pointer(ctx, 0);
    const char *topic = duk_get_string(ctx, 1);
    int qos = duk_get_int(ctx, 2);

    duk_dup(ctx, 3);
    int js_cb_ref = be_ref(ctx);
    amp_debug(MOD_STR, "subscribe topic: %s, qos: %d, js_cb_ref: %d\n", topic, qos, js_cb_ref);
    ret = wrapper_mqtt_subscribe(pclient, (const char *)topic, qos, mqtt_sub_callback, (void *)js_cb_ref);
    if (ret < 0)
    {
        amp_warn(MOD_STR, "mqtt_subscribe failed, ret: %d\n", ret);
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_mqtt_unsubscribe(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2))
    {
        amp_warn(MOD_STR, "parameter is pointer string function \n");
        goto out;
    }

    void *pclient = duk_get_pointer(ctx, 0);
    const char *topic = duk_get_string(ctx, 1);

    duk_dup(ctx, 2);
    int js_cb_ref = be_ref(ctx);

    ret = wrapper_mqtt_unsubscribe(pclient, (const char *)topic);
    if (ret < 0)
    {
        amp_warn(MOD_STR, "mqtt_unsubscribe failed, ret: %d\n", ret);
        goto out;
    }
    amp_debug(MOD_STR, "unsubscribe topic: %s, ret: %d\n", topic, ret);

    duk_push_int(ctx, ret);

    be_push_ref(ctx, js_cb_ref);
    duk_push_lstring(ctx, topic, strlen(topic));
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, js_cb_ref);
    duk_gc(ctx, 0);
    return 1;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_mqtt_publish(duk_context *ctx)
{
    int ret = -1;
    iotx_mqtt_topic_info_t mqtt_topic_info;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_string(ctx, 2) || !duk_is_number(ctx, 3) ||
        !duk_is_function(ctx, 4))
    {
        amp_warn(MOD_STR, "parameter must be pointer string string function\n");
        goto out;
    }

    void *pclient = duk_get_pointer(ctx, 0);
    const char *topic = duk_get_string(ctx, 1);
    int msg_len;
    const char *msg = duk_get_lstring(ctx, 2, &msg_len);
    int qos = duk_get_number(ctx, 3);
    duk_dup(ctx, 4);
    int js_cb_ref = be_ref(ctx);
    amp_debug(MOD_STR, "topic: %s, msg: %s, msg_len: %d\n", topic, msg, msg_len);

    memset(&mqtt_topic_info, 0x0, sizeof(iotx_mqtt_topic_info_t));
    mqtt_topic_info.payload = msg;
    mqtt_topic_info.payload_len = msg_len;
    mqtt_topic_info.retain = 0;
    mqtt_topic_info.dup = 0;
    mqtt_topic_info.qos = qos;

    ret = wrapper_mqtt_publish(pclient, (const char *)topic, &mqtt_topic_info);
    if (ret < 0)
    {
        amp_warn(MOD_STR, "mqtt_publish failed, ret: %d\n", ret);
    }

    duk_push_int(ctx, ret);

    be_push_ref(ctx, js_cb_ref);
    duk_push_int(ctx, ret);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, js_cb_ref);
    duk_gc(ctx, 0);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_mqtt_close(duk_context *ctx)
{
    int ret = -1;
    int js_cb_ref;
    iotx_mqtt_topic_info_t mqtt_topic_info;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be pointer function");
        goto out;
    }

    void *pclient = duk_get_pointer(ctx, 0);
    if (pclient == NULL) {
        amp_warn(MOD_STR, "mqtt client is null");
        goto out;
    }
    ret = 0;

out:
    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    /* release mqtt in mqtt_yield_task() */
    close_flag = 1;
    be_push_ref(ctx, js_cb_ref);
    duk_push_int(ctx, ret);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, js_cb_ref);
    duk_gc(ctx, 0);
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

    duk_push_c_function(ctx, native_mqtt_subscribe, 4);
    duk_put_prop_string(ctx, -2, "subscribe");

    duk_push_c_function(ctx, native_mqtt_unsubscribe, 3);
    duk_put_prop_string(ctx, -2, "unsubscribe");

    duk_push_c_function(ctx, native_mqtt_publish, 5);
    duk_put_prop_string(ctx, -2, "publish");

    duk_push_c_function(ctx, native_mqtt_close, 2);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_prop_string(ctx, -2, "MQTT");
}
