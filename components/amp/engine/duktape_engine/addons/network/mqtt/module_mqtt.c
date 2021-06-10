/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "be_inl.h"
#include "module_mqtt.h"
#include "aiot_state_api.h"

#define MOD_STR "MQTT"
#define MQTT_TASK_YIELD_TIMEOUT 200
static char g_mqtt_close_flag = 0;
static aos_sem_t g_mqtt_close_sem = NULL;

static void mqtt_handle_notify(void *pdata)
{
    int res;
    amp_mqtt_handle_t *amp_mqtt_handle = (amp_mqtt_handle_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF]);
    duk_push_int(ctx, amp_mqtt_handle->res);
    duk_push_pointer(ctx, amp_mqtt_handle);
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    /* free when mqtt connect failed */
    if(amp_mqtt_handle->res < 0){
        be_unref(ctx, amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF]);
        aos_free(amp_mqtt_handle);
    }

    duk_gc(ctx, 0);
}

static void mqtt_connect_task(void *pdata)
{
    int ret;
    amp_mqtt_handle_t *amp_mqtt_handle = NULL;
    amp_mqtt_params_t *amp_mqtt_params = (amp_mqtt_params_t *)pdata;

    amp_mqtt_handle = aos_malloc(sizeof(amp_mqtt_handle_t));
    if (amp_mqtt_handle == NULL) {
        amp_debug(MOD_STR, "amp mqtt handle malloc failed");
        aos_free(amp_mqtt_params);
        return;
    }

    amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF] = amp_mqtt_params->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF];

    ret = mqtt_client_start(&amp_mqtt_handle->mqtt_handle, amp_mqtt_params);
    if (ret < 0) {
        amp_debug(MOD_STR, "mqtt client init failed");
        aos_free(amp_mqtt_params);
        aos_free(amp_mqtt_handle);
        return;
    }

    amp_mqtt_handle->res = ret;

    /* return aiot_device_handle */
    amp_task_schedule_call(mqtt_handle_notify, amp_mqtt_handle);

    while(!g_mqtt_close_flag) {
        aos_msleep(1000);
    }

    aos_free(amp_mqtt_params);
    aos_free(amp_mqtt_handle);
    aos_sem_signal(&g_mqtt_close_sem);
    aos_task_exit(0);
}

static duk_ret_t native_mqtt_start(duk_context *ctx)
{
    int ret;
    amp_mqtt_params_t *mqtt_params = NULL;
    aos_task_t mqtt_task;

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be object and function\n");
        ret = -1;
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "host");
    duk_get_prop_string(ctx, 0, "port");
    duk_get_prop_string(ctx, 0, "client_id");
    duk_get_prop_string(ctx, 0, "username");
    duk_get_prop_string(ctx, 0, "password");
    duk_get_prop_string(ctx, 0, "keepaliveSec");

    if (!duk_is_string(ctx, -6) || !duk_is_number(ctx, -5) ||
        !duk_is_string(ctx, -4) || !duk_is_string(ctx, -3) ||
        !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1))
    {
        amp_warn(MOD_STR,
            "Parameter 1 must be an object like {host: string, "
            "port: uint, client_id: string, username: string, "
            "password: string, keepalive_interval: uint}\n");
        ret = -2;
        duk_pop_n(ctx, 6);
        goto out;
    }

    mqtt_params = (amp_mqtt_params_t *)aos_malloc(sizeof(amp_mqtt_params_t));
    if (!mqtt_params) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    mqtt_params->host = duk_get_string(ctx, -6);
    mqtt_params->port = duk_get_number(ctx, -5);
    mqtt_params->clientid = duk_get_string(ctx, -4);
    mqtt_params->username = duk_get_string(ctx, -3);
    mqtt_params->password = duk_get_string(ctx, -2);
    mqtt_params->keepaliveSec = duk_get_number(ctx, -1);

    amp_debug(MOD_STR, "host: %s, port: %d\n", mqtt_params->host, mqtt_params->port);
    amp_debug(MOD_STR, "client_id: %s, username: %s, password: %s\n", mqtt_params->clientid, mqtt_params->username, mqtt_params->password);

    duk_dup(ctx, 1);
    // init_params->js_cb_ref = be_ref(ctx);
    mqtt_params->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF] = be_ref(ctx);

    /* create task to IOT_MQTT_Yield() */
    ret = aos_task_new_ext(&mqtt_task, "amp mqtt task", mqtt_connect_task, mqtt_params, 1024 * 4, MQTT_TSK_PRIORITY);
    if (ret < 0)
    {
        amp_warn(MOD_STR, "jse_osal_create_task failed\n");
        be_unref(ctx, mqtt_params->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF]);
        aos_free(mqtt_params);
        ret = -4;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/* subscribe */
static duk_ret_t native_mqtt_subscribe(duk_context *ctx)
{
    int res = -1;
    amp_mqtt_handle_t *amp_mqtt_handle = NULL;
    const char *topic = NULL;
    uint8_t qos = 0;
    int js_cb_ref = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object)");
        goto out;
    }

    amp_mqtt_handle = duk_get_pointer(ctx, 0);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    duk_get_prop_string(ctx, 1, "topic");
    duk_get_prop_string(ctx, 1, "qos");

    if (!duk_is_number(ctx, -1) || !duk_is_string(ctx, -2))
    {
        amp_warn(MOD_STR, "invalid params");
        duk_pop_n(ctx, 2);
        goto out;
    }

    qos = duk_get_number(ctx, -1);
    topic = (char *)duk_get_string(ctx, -2);

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_SCRIBE_TOPIC_REF] = js_cb_ref;

    res = aiot_mqtt_sub(amp_mqtt_handle->mqtt_handle, topic, NULL, qos, NULL);
    if (res < 0) {
        amp_error(MOD_STR, "aiot app mqtt subscribe failed\n");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* unsubscribe */
static duk_ret_t native_mqtt_unsubscribe(duk_context *ctx)
{
    int res = -1;
    amp_mqtt_handle_t *amp_mqtt_handle = NULL;
    const char *topic;
    uint8_t qos = 0;
    int js_cb_ref = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, string, function)");
        goto out;
    }

    amp_mqtt_handle = duk_get_pointer(ctx, 0);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    topic = (char *)duk_get_string(ctx, 1);

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_UNSCRIBE_TOPIC_REF] = js_cb_ref;

    amp_debug(MOD_STR, "unsubscribe topic: %s", topic);

    res = aiot_mqtt_unsub(amp_mqtt_handle->mqtt_handle, topic);
    if (res < 0) {
        amp_error(MOD_STR, "aiot app mqtt unsubscribe failed\n");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* publish */
static duk_ret_t native_mqtt_publish(duk_context *ctx)
{
    int res = -1;
    amp_mqtt_handle_t *amp_mqtt_handle = NULL;
    const char *topic;
    const char *payload;
    uint16_t payload_len = 0;
    uint8_t qos = 0;
    int js_cb_ref = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    amp_mqtt_handle = duk_get_pointer(ctx, 0);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    duk_get_prop_string(ctx, 1, "topic");
    duk_get_prop_string(ctx, 1, "payload");
    duk_get_prop_string(ctx, 1, "qos");

    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1))
    {
        amp_warn(MOD_STR, "invalid params");
        duk_pop_n(ctx, 3);
        goto out;
    }

    qos = duk_get_number(ctx, -1);
    payload = (char *)duk_get_string(ctx, -2);
    topic = (char *)duk_get_string(ctx, -3);
    payload_len = strlen(payload);

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_PUBLISH_REF] = js_cb_ref;

    amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d", topic, payload, qos);

    res = aiot_mqtt_pub(amp_mqtt_handle->mqtt_handle, topic, payload, payload_len, qos);
    if (res < 0) {
        amp_error(MOD_STR, "aiot app mqtt publish failed");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

static duk_ret_t native_mqtt_close(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref = 0;
    amp_mqtt_handle_t *amp_mqtt_handle = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be pointer function");
        goto out;
    }

    amp_mqtt_handle = duk_get_pointer(ctx, 0);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt client is null");
        goto out;
    }

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    amp_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_CLIENT_STOP_REF] = js_cb_ref;

    res = mqtt_client_stop(&amp_mqtt_handle->mqtt_handle);
    if (res < 0) {
        amp_debug(MOD_STR, "mqtt client stop failed");
    }

out:
    /* release mqtt in mqtt_yield_task() */
    g_mqtt_close_flag = 1;
    aos_sem_wait(&g_mqtt_close_sem, MQTT_TASK_YIELD_TIMEOUT + 50);
    g_mqtt_close_flag = 0;
    return 1;
}

static void module_mqtt_source_clean(void)
{
    if (g_mqtt_close_flag) {
        aos_sem_wait(&g_mqtt_close_sem, MQTT_TASK_YIELD_TIMEOUT + 50);
        g_mqtt_close_flag = 0;
    }
}

void module_mqtt_register(void)
{
    duk_context *ctx = be_get_context();

    if (!g_mqtt_close_sem) {
        if (aos_sem_new(&g_mqtt_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create mqtt sem fail");
            return;
        }
    }

    amp_module_free_register(module_mqtt_source_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("start",       native_mqtt_start, 2);
    AMP_ADD_FUNCTION("subscribe",   native_mqtt_subscribe, 3);
    AMP_ADD_FUNCTION("unsubscribe", native_mqtt_unsubscribe, 3);
    AMP_ADD_FUNCTION("publish",     native_mqtt_publish, 5);
    AMP_ADD_FUNCTION("close",       native_mqtt_close, 2);

    duk_put_prop_string(ctx, -2, "MQTT");
}
