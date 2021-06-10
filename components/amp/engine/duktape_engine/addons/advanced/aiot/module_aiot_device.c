/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aos/kv.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dm_api.h"
#include "aiot_state_api.h"
#include "be_inl.h"
#include "ota_agent.h"
#include "module_aiot.h"

#define MOD_STR "AIOT"

#define OTA_MODE_NAME "system"

static char g_iot_close_flag = 0;
static char g_iot_conn_flag = 0;
static char g_iot_clean_flag = 0;
static aos_sem_t g_iot_close_sem = NULL;

ota_service_t g_aiot_device_appota_service;
static ota_store_module_info_t module_info[3];

extern const char *amp_jsapp_version_get(void);

typedef struct {
    iot_device_handle_t *iot_device_handle;
    char *topic;
    char *payload;
    char *service_id;
    char *params;
    int js_cb_ref;
    int ret_code;
    int topic_len;
    int payload_len;
    int params_len;
    int dm_recv;
    uint64_t msg_id;
    aiot_mqtt_option_t option;
    aiot_mqtt_event_type_t event_type;
    aiot_mqtt_recv_type_t recv_type;
    aiot_dm_recv_type_t dm_recv_type;
    aiot_dev_jscallback_type_t cb_type;
} device_notify_param_t;

static char *__amp_strdup(char *src)
{
    char   *dst;
    size_t  len = 0;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src);

    dst = aos_malloc(len+1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

static int user_jsapp_ota_triger_cb(void* pctx, char *ver, char *module_name)
{
    int ret = OTA_TRANSPORT_PAR_FAIL;
    ota_service_t *ota_svc = (ota_service_t *)pctx;
    if ((ota_svc == NULL) || (ver == NULL) || (module_name == NULL)) {
        return ret;
    }
    if (strncmp(module_name, "default", strlen(module_name)) == 0) {
        char *current_ver = NULL;
        current_ver = amp_jsapp_version_get();
        if (current_ver == NULL) {
            return ret;
        }
        ret = 0;
        if (strncmp(ver, current_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "user ota version too old!");
        } else {
            if (ota_svc->feedback_func.on_user_event_cb != NULL) {
                ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
            } else {
                ret = OTA_TRANSPORT_PAR_FAIL;
                amp_error(MOD_STR, "user ota trigger cb is NULL!");
            }
        }
    } else {
        int ret = OTA_TRANSPORT_PAR_FAIL;
        char current_amp_ver[64];
        memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
        amp_app_version_get(current_amp_ver);
        ret = 0;
        if (strncmp(ver, current_amp_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "system ota version too old!");
        } else {
            if (ota_svc->feedback_func.on_user_event_cb != NULL) {
                ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
            } else {
                ret = OTA_TRANSPORT_PAR_FAIL;
                amp_error(MOD_STR, "user ota trigger cb is NULL!");
            }
        }
    }
    return ret;
}

static void aiot_device_notify(void *pdata)
{
    device_notify_param_t *param = (device_notify_param_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, param->js_cb_ref);

    if (param->dm_recv) {
        switch (param->dm_recv_type) {
        case AIOT_DMRECV_PROPERTY_SET:
            duk_push_object(ctx);
            duk_push_int(ctx, (unsigned long)param->msg_id);
            duk_put_prop_string(ctx, -2, "msg_id");
            duk_push_int(ctx, param->params_len);
            duk_put_prop_string(ctx, -2, "params_len");
            duk_push_lstring(ctx, param->params, param->params_len);
            duk_put_prop_string(ctx, -2, "params");
            aos_free(param->params);
            break;
        case AIOT_DMRECV_ASYNC_SERVICE_INVOKE:
            duk_push_object(ctx);
            duk_push_int(ctx, (unsigned long)param->msg_id);
            duk_put_prop_string(ctx, -2, "msg_id");
            duk_push_string(ctx, param->service_id);
            duk_put_prop_string(ctx, -2, "service_id");
            duk_push_int(ctx, param->params_len);
            duk_put_prop_string(ctx, -2, "params_len");
            duk_push_lstring(ctx, param->params, param->params_len);
            duk_put_prop_string(ctx, -2, "params");
            aos_free(param->service_id);
            aos_free(param->params);
            break;
        default:
            aos_free(param);
            return;
        }
    } else if (param->option == AIOT_MQTTOPT_EVENT_HANDLER) {
        switch (param->event_type) {
        case AIOT_MQTTEVT_CONNECT:
        case AIOT_MQTTEVT_RECONNECT:
        case AIOT_MQTTEVT_DISCONNECT:
            duk_push_object(ctx);
            duk_push_int(ctx, param->ret_code);
            duk_put_prop_string(ctx, -2, "code");
            duk_push_pointer(ctx, param->iot_device_handle);
            duk_put_prop_string(ctx, -2, "handle");
            amp_error(MOD_STR, "%s: handle 0x%x",
            __func__, (unsigned int)param->iot_device_handle);
            amp_error(MOD_STR, "%s: mqtthandle 0x%x",
            __func__, (unsigned int)param->iot_device_handle->mqtt_handle);
            break;
        default:
            aos_free(param);
            return;
        }
    } else {
        switch (param->recv_type) {
        case AIOT_MQTTRECV_PUB:
            duk_push_object(ctx);
            duk_push_int(ctx, param->ret_code);
            duk_put_prop_string(ctx, -2, "code");
            duk_push_pointer(ctx, param->iot_device_handle);
            duk_put_prop_string(ctx, -2, "handle");
            duk_push_lstring(ctx, param->topic, param->topic_len);
            duk_put_prop_string(ctx, -2, "topic");
            duk_push_lstring(ctx, param->payload, param->payload_len);
            duk_put_prop_string(ctx, -2, "payload");
            aos_free(param->topic);
            aos_free(param->payload);
            break;
        default:
            aos_free(param);
            return;
        }
    }

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);

    aos_free(param);
}

/* 用户数据接收处理回调函数 */
static void aiot_app_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    iot_device_handle_t *iot_device_handle = (iot_device_handle_t *)userdata;
    duk_context *ctx = be_get_context();

    device_notify_param_t *param;

    param = aos_malloc(sizeof(device_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc device_notify_param_t fail");
        return;
    }
    memset(param, 0, sizeof(device_notify_param_t));
    param->dm_recv = 1;
    param->dm_recv_type = recv->type;
    param->iot_device_handle = iot_device_handle;
    switch (recv->type) {

        /* 属性上报, 事件上报, 获取期望属性值或者删除期望属性值的应答 */
        case AIOT_DMRECV_GENERIC_REPLY: {
            // printf("msg_id = %d, code = %d, data = %.*s, message = %.*s\r\n",
            //        recv->data.generic_reply.msg_id,
            //        recv->data.generic_reply.code,
            //        recv->data.generic_reply.data_len,
            //        recv->data.generic_reply.data,
            //        recv->data.generic_reply.message_len,
            //        recv->data.generic_reply.message);
        }
        break;

        /* 属性设置 */
        case AIOT_DMRECV_PROPERTY_SET: {
            amp_debug(MOD_STR, "msg_id = %ld, params = %.*s",
                   (unsigned long)recv->data.property_set.msg_id,
                   recv->data.property_set.params_len,
                   recv->data.property_set.params);

            param->js_cb_ref = iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONPROPS_REF];
            param->msg_id = recv->data.property_set.msg_id;
            param->params_len = recv->data.property_set.params_len;
            param->params = __amp_strdup(recv->data.property_set.params);
            param->cb_type = AIOT_DEV_JSCALLBACK_ONPROPS_REF;

            /* TODO: 以下代码演示如何对来自云平台的属性设置指令进行应答, 用户可取消注释查看演示效果 */
            /*
            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_PROPERTY_SET_REPLY;
                msg.data.property_set_reply.msg_id = recv->data.property_set.msg_id;
                msg.data.property_set_reply.code = 200;
                msg.data.property_set_reply.data = "{}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }
            */
        }
        break;

        /* 异步服务调用 */
        case AIOT_DMRECV_ASYNC_SERVICE_INVOKE: {
            amp_debug(MOD_STR, "msg_id = %ld, service_id = %s, params = %.*s",
                   (unsigned long)recv->data.async_service_invoke.msg_id,
                   recv->data.async_service_invoke.service_id,
                   recv->data.async_service_invoke.params_len,
                   recv->data.async_service_invoke.params);

            param->js_cb_ref = iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONSERVICE_REF];
            param->msg_id = recv->data.async_service_invoke.msg_id;
            param->params_len = recv->data.async_service_invoke.params_len;
            param->service_id = __amp_strdup(recv->data.async_service_invoke.service_id);
            param->params = __amp_strdup(recv->data.async_service_invoke.params);
            param->cb_type = AIOT_DEV_JSCALLBACK_ONSERVICE_REF;

            /* TODO: 以下代码演示如何对来自云平台的异步服务调用进行应答, 用户可取消注释查看演示效果
             *
             * 注意: 如果用户在回调函数外进行应答, 需要自行保存msg_id, 因为回调函数入参在退出回调函数后将被SDK销毁, 不可以再访问到
             */

            /*
            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_ASYNC_SERVICE_REPLY;
                msg.data.async_service_reply.msg_id = recv->data.async_service_invoke.msg_id;
                msg.data.async_service_reply.code = 200;
                msg.data.async_service_reply.service_id = "ToggleLightSwitch";
                msg.data.async_service_reply.data = "{\"dataA\": 20}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }
            */
        }
        break;

        /* 同步服务调用 */
        case AIOT_DMRECV_SYNC_SERVICE_INVOKE: {
            amp_debug(MOD_STR, "msg_id = %ld, rrpc_id = %s, service_id = %s, params = %.*s",
                   (unsigned long)recv->data.sync_service_invoke.msg_id,
                   recv->data.sync_service_invoke.rrpc_id,
                   recv->data.sync_service_invoke.service_id,
                   recv->data.sync_service_invoke.params_len,
                   recv->data.sync_service_invoke.params);

            /* TODO: 以下代码演示如何对来自云平台的同步服务调用进行应答, 用户可取消注释查看演示效果
             *
             * 注意: 如果用户在回调函数外进行应答, 需要自行保存msg_id和rrpc_id字符串, 因为回调函数入参在退出回调函数后将被SDK销毁, 不可以再访问到
             */

            /*
            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_SYNC_SERVICE_REPLY;
                msg.data.sync_service_reply.rrpc_id = recv->data.sync_service_invoke.rrpc_id;
                msg.data.sync_service_reply.msg_id = recv->data.sync_service_invoke.msg_id;
                msg.data.sync_service_reply.code = 200;
                msg.data.sync_service_reply.service_id = "SetLightSwitchTimer";
                msg.data.sync_service_reply.data = "{}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }
            */
        }
        break;

        /* 下行二进制数据 */
        case AIOT_DMRECV_RAW_DATA: {
            amp_debug(MOD_STR, "raw data len = %d", recv->data.raw_data.data_len);
            /* TODO: 以下代码演示如何发送二进制格式数据, 若使用需要有相应的数据透传脚本部署在云端 */
            /*
            {
                aiot_dm_msg_t msg;
                uint8_t raw_data[] = {0x01, 0x02};

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_RAW_DATA;
                msg.data.raw_data.data = raw_data;
                msg.data.raw_data.data_len = sizeof(raw_data);
                aiot_dm_send(dm_handle, &msg);
            }
            */
        }
        break;

        /* 二进制格式的同步服务调用, 比单纯的二进制数据消息多了个rrpc_id */
        case AIOT_DMRECV_RAW_SYNC_SERVICE_INVOKE: {
            amp_debug(MOD_STR, "raw sync service rrpc_id = %s, data_len = %d",
                   recv->data.raw_service_invoke.rrpc_id,
                   recv->data.raw_service_invoke.data_len);
        }
        break;

        default:
            break;
    }

    amp_task_schedule_call(aiot_device_notify, param);
}


static void aiot_mqtt_message_cb(iot_mqtt_message_t *message, void *userdata)
{
    iot_mqtt_userdata_t *udata = (iot_mqtt_userdata_t *)userdata;
    iot_device_handle_t *iot_device_handle;
    device_notify_param_t *param;

    if (!message || !udata)
        return;

    param = aos_malloc(sizeof(device_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return;
    }
    memset(param, 0, sizeof(device_notify_param_t));
    param->iot_device_handle = (iot_device_handle_t *)udata->handle;
    param->option = message->option;
    param->js_cb_ref = param->iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_CREATE_DEV_REF];
    param->cb_type = AIOT_DEV_JSCALLBACK_CREATE_DEV_REF;

    if (message->option == AIOT_MQTTOPT_EVENT_HANDLER) {
        switch (message->event.type) {
        case AIOT_MQTTEVT_CONNECT:
        case AIOT_MQTTEVT_RECONNECT:
        case AIOT_MQTTEVT_DISCONNECT:
            param->ret_code = message->event.code;
            param->event_type = message->event.type;
            break;
        default:
            aos_free(param);
            return;
        }
    } else if (message->option == AIOT_MQTTOPT_RECV_HANDLER) {
        switch (message->recv.type) {
        case AIOT_MQTTRECV_PUB:
            param->ret_code = message->recv.code;
            param->topic_len = message->recv.topic_len;
            param->payload_len = message->recv.payload_len;
            param->topic = __amp_strdup(message->recv.topic);
            param->payload = __amp_strdup(message->recv.payload);
            param->recv_type = message->recv.type;
            break;
        default:
            aos_free(param);
            return;
        }
    } else {
        aos_free(param);
        return;
    }

    amp_task_schedule_call(aiot_device_notify, param);
}


static void aiot_device_connect(void *pdata)
{
    int res = -1;
    //char current_amp_ver[64];
    //ota_service_t *ota_svc = &g_aiot_device_appota_service;
    iot_device_handle_t *iot_device_handle = (iot_device_handle_t *)pdata;
    iot_mqtt_userdata_t *userdata;

    uint16_t keepaliveSec = 0;

    keepaliveSec = iot_device_handle->keepaliveSec;

    userdata = aos_malloc(sizeof(iot_mqtt_userdata_t));
    if (!userdata) {
        amp_error(MOD_STR, "alloc mqtt userdata fail");
        return;
    }
    userdata->callback = aiot_mqtt_message_cb;
    userdata->handle = iot_device_handle;

    res = aiot_mqtt_client_start(&iot_device_handle->mqtt_handle, keepaliveSec, userdata);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "mqtt client create failed");
        aos_free(userdata);
        aos_free(iot_device_handle);
        return;
    }

    g_iot_conn_flag = 1;
    iot_device_handle->res = res;

    /* device model service */
    iot_device_handle->dm_handle = aiot_dm_init();
    if (iot_device_handle->dm_handle == NULL) {
        amp_debug(MOD_STR, "aiot_dm_init failed");
        aos_free(userdata);
        aos_free(iot_device_handle);
        return;
    }

    /* 配置MQTT实例句柄 */
    aiot_dm_setopt(iot_device_handle->dm_handle, AIOT_DMOPT_MQTT_HANDLE, iot_device_handle->mqtt_handle);
    /* 配置消息接收处理回调函数 */
    aiot_dm_setopt(iot_device_handle->dm_handle, AIOT_DMOPT_RECV_HANDLER, (void *)aiot_app_dm_recv_handler);
    /* 配置回调函数参数 */
    aiot_dm_setopt(iot_device_handle->dm_handle, AIOT_DMOPT_USERDATA, iot_device_handle);

    /* app device active info report */
    res = amp_app_devinfo_report(iot_device_handle->mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "device active info report failed");
    }
    while (!g_iot_close_flag){
        aos_msleep(1000);
    }

    duk_context *ctx = be_get_context();

    aiot_mqtt_client_stop(&iot_device_handle->mqtt_handle);

    aos_free(userdata);
    aos_free(iot_device_handle);
    g_iot_conn_flag = 0;
    aos_sem_signal(&g_iot_close_sem);
    aos_task_exit(0);
    return;
}

/*************************************************************************************
 * Function:    native_aiot_create_device
 * Description: js native addon for UDP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static duk_ret_t native_aiot_create_device(duk_context *ctx)
{
    int res = -1;
    void *mqtt_handle = NULL;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    int keepaliveSec = 0;
    int js_cb_ref = 0;
    aos_task_t iot_device_task;
    iot_device_handle_t *iot_device_handle = NULL;
    ota_service_t *ota_svc = &g_aiot_device_appota_service;

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be object and function");
        goto out;
    }

    if (g_iot_clean_flag) {
        amp_warn(MOD_STR, "module source clean, ignore");
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "deviceSecret");
    duk_get_prop_string(ctx, 0, "keepaliveSec");

    if (!duk_is_string(ctx, -4) || !duk_is_string(ctx, -3) ||
        !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1))
    {
        amp_warn(MOD_STR, "Parameter invalid");
        res = -2;
        duk_pop_n(ctx, 4);
        goto out;
    }

    productKey    = duk_get_string(ctx, -4);
    deviceName    = duk_get_string(ctx, -3);
    deviceSecret  = duk_get_string(ctx, -2);
    keepaliveSec  = duk_get_number(ctx, -1);

    memset(ota_svc->pk, 0, sizeof(ota_svc->pk));
    memset(ota_svc->dn, 0, sizeof(ota_svc->dn));
    memset(ota_svc->ds, 0, sizeof(ota_svc->ds));
    memcpy(ota_svc->pk, productKey, strlen(productKey));
    memcpy(ota_svc->dn, deviceName, strlen(deviceName));
    memcpy(ota_svc->ds, deviceSecret, strlen(deviceSecret));

    aos_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICESECRET, deviceSecret, IOTX_DEVICE_SECRET_LEN, 1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    iot_device_handle =(iot_device_handle_t *)aos_malloc(sizeof(iot_device_handle_t));
    if (!iot_device_handle) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_CREATE_DEV_REF] = js_cb_ref;
    iot_device_handle->keepaliveSec = keepaliveSec;

    res = aos_task_new_ext(&iot_device_task, "amp aiot device task", aiot_device_connect, iot_device_handle, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != STATE_SUCCESS) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(iot_device_handle);
        goto out;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* dynmic register */
static duk_ret_t native_aiot_dynreg(duk_context *ctx)
{
    int res = -1;
    int js_cb_ref = 0;

    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (object, function)");
        goto out;
    }

    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "productSecret");

    char *productKey = (char *)duk_get_string(ctx, -3);
    char *deviceName = (char *)duk_get_string(ctx, -2);
    char *productSecret = (char *)duk_get_string(ctx, -1);

    aos_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_PRODUCTSECRET, productSecret, IOTX_PRODUCT_SECRET_LEN, 1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    res = aiot_dynreg_http(js_cb_ref);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "device dynmic register failed");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* post event */
static duk_ret_t native_aiot_postEvent(duk_context *ctx)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *event_id;
    char *event_payload;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (number, object, function)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);

    duk_get_prop_string(ctx, 1, "id");
    duk_get_prop_string(ctx, 1, "params");

    event_id = (char *)duk_get_string(ctx, -2);
    event_payload = (char *)duk_get_string(ctx, -1);

    res = aiot_app_send_event_post(iot_device_handle->dm_handle, event_id, event_payload);
    if (res < STATE_SUCCESS) {
        amp_warn(MOD_STR, "post event failed!");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* post props */
static duk_ret_t native_aiot_postProps(duk_context *ctx)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *params;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, string, function)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);
    params = (char *)duk_get_string(ctx, 1);

    res = aiot_app_send_property_post(iot_device_handle->dm_handle, params);
    if (res < STATE_SUCCESS) {
        amp_warn(MOD_STR, "property post failed");
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/*  onprops */
static duk_ret_t native_aiot_onProps(duk_context *ctx)
{
    int res = STATE_SUCCESS;
    iot_device_handle_t *iot_device_handle = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer, function)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);

    duk_dup(ctx, 1);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONPROPS_REF] = be_ref(ctx);

out:
    duk_push_int(ctx, res);
    return 1;
}

/*  onService */
static duk_ret_t native_aiot_onService(duk_context *ctx)
{
    int res = STATE_SUCCESS;
    iot_device_handle_t *iot_device_handle = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer, function)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);

    duk_dup(ctx, 1);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONSERVICE_REF] = be_ref(ctx);

out:
    duk_push_int(ctx, res);
    return 1;
}

/* publish */
static duk_ret_t native_aiot_publish(duk_context *ctx)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *topic;
    char *payload;
    uint16_t payload_len = 0;
    uint8_t qos = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);
    if (iot_device_handle == NULL) {
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

    amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d", topic, payload, qos);

    res = aiot_mqtt_pub(iot_device_handle->mqtt_handle, topic, payload, payload_len, qos);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt publish failed\n");
        goto out;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* unsubscribe */
static duk_ret_t native_aiot_unsubscribe(duk_context *ctx)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *topic;
    uint8_t qos = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, string)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);
    if (iot_device_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    topic = (char *)duk_get_string(ctx, 1);

    amp_debug(MOD_STR, "unsubscribe topic: %s", topic);

    res = aiot_mqtt_unsub(iot_device_handle->mqtt_handle, topic);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt unsubscribe failed");
        goto out;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* subscribe */
static duk_ret_t native_aiot_subscribe(duk_context *ctx)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *topic = NULL;
    uint8_t qos = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);
    if (iot_device_handle == NULL) {
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

    amp_debug(MOD_STR, "subscribe topic: %s, qos is: %d", topic, qos);

    res = aiot_mqtt_sub(iot_device_handle->mqtt_handle, topic, NULL, qos, NULL);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt subscribe failed\n");
        goto out;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/*************************************************************************************
 * Function:    native_aiot_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static duk_ret_t native_aiot_close(duk_context *ctx)
{
    int res     = -1;
    iot_device_handle_t *iot_device_handle = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be pointer and function");
        goto out;
    }

    iot_device_handle = duk_get_pointer(ctx, 0);
    if (iot_device_handle == NULL) {
        amp_warn(MOD_STR, "iot_device_hanle is null");
        goto out;
    }

    duk_dup(ctx, 1);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_END_CLIENT_REF] = be_ref(ctx);

    g_iot_close_flag = 1;
    aos_sem_wait(&g_iot_close_sem, 200 + 50);
    res = aiot_mqtt_client_stop(&iot_device_handle->mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot stop failed");
        goto out;
    }

    g_iot_close_flag = 0;
    duk_push_int(ctx, 0);
    aos_free(iot_device_handle);
    return 1;

out:
    duk_push_int(ctx, res);
    return 1;
}

static void module_iot_source_clean(void)
{
    duk_context *ctx = be_get_context();

    if (g_iot_conn_flag) {
        g_iot_close_flag = 1;
        aos_sem_wait(&g_iot_close_sem, 5100);
        g_iot_close_flag = 0;
        aos_msleep(10);
    }

    g_iot_clean_flag = 1;
}

void module_aiot_device_register(void)
{
    duk_context *ctx = be_get_context();

    if (!g_iot_close_sem) {
        if (aos_sem_new(&g_iot_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create iot sem fail");
            return;
        }
    }
    g_iot_clean_flag = 0;

    amp_module_free_register(module_iot_source_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("device",       native_aiot_create_device, 2);
    AMP_ADD_FUNCTION("subscribe",    native_aiot_subscribe, 3);
    AMP_ADD_FUNCTION("unsubscribe",  native_aiot_unsubscribe, 3);
    AMP_ADD_FUNCTION("publish",      native_aiot_publish, 3);
    AMP_ADD_FUNCTION("postProps",    native_aiot_postProps, 3);
    AMP_ADD_FUNCTION("onProps",      native_aiot_onProps, 2);
    AMP_ADD_FUNCTION("postEvent",    native_aiot_postEvent, 3);
    AMP_ADD_FUNCTION("register",     native_aiot_dynreg, 2);
    AMP_ADD_FUNCTION("onService",    native_aiot_onService, 2);
    AMP_ADD_FUNCTION("end",          native_aiot_close, 2);

    duk_put_prop_string(ctx, -2, "AIOT_DEVICE");
}
