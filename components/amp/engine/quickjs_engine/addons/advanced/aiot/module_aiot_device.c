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
#include "quickjs.h"
#include "ota_agent.h"
#include "module_aiot.h"
#include "app_upgrade.h"
#include "quickjs_addon_common.h"

#define MOD_STR "AIOT"

#define OTA_MODE_NAME "system"

static char g_iot_close_flag = 0;
static char g_iot_conn_flag = 0;
static char g_iot_clean_flag = 0;
static aos_sem_t g_iot_close_sem = NULL;
JSClassID js_aiot_device_class_id;

ota_service_t g_aiot_device_appota_service;
static ota_store_module_info_t module_info[3];

// extern const char *amp_jsapp_version_get(void);

typedef struct {
    iot_device_handle_t *iot_device_handle;
    char *topic;
    char *payload;
    char *service_id;
    char *params;
    JSValue js_cb_ref;
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

// static int user_jsapp_ota_triger_cb(void* pctx, char *ver, char *module_name)
// {
//     int ret = OTA_TRANSPORT_PAR_FAIL;
//     ota_service_t *ota_svc = (ota_service_t *)pctx;

//     amp_error(MOD_STR, "user_jsapp_ota_triger_cb IS CALLED");
//     if ((ota_svc == NULL) || (ver == NULL) || (module_name == NULL)) {
//         return ret;
//     }
//     if (strncmp(module_name, "default", strlen(module_name)) == 0) {
//         char *current_ver = NULL;
//         current_ver = amp_jsapp_version_get();
//         if (current_ver == NULL) {
//             return ret;
//         }
//         ret = 0;
//         if (strncmp(ver, current_ver, strlen(ver)) <= 0) {
//             ret = OTA_TRANSPORT_VER_FAIL;
//             amp_error(MOD_STR, "user ota version too old!");
//         } else {
//             if (ota_svc->feedback_func.on_user_event_cb != NULL) {
//                 ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
//             } else {
//                 ret = OTA_TRANSPORT_PAR_FAIL;
//                 amp_error(MOD_STR, "user ota trigger cb is NULL!");
//             }
//         }
//     } else {
//         int ret = OTA_TRANSPORT_PAR_FAIL;
//         char current_amp_ver[64];
//         memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
//         amp_app_version_get(current_amp_ver);
//         ret = 0;
//         if (strncmp(ver, current_amp_ver, strlen(ver)) <= 0) {
//             ret = OTA_TRANSPORT_VER_FAIL;
//             amp_error(MOD_STR, "system ota version too old!");
//         } else {
//             if (ota_svc->feedback_func.on_user_event_cb != NULL) {
//                 ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
//             } else {
//                 ret = OTA_TRANSPORT_PAR_FAIL;
//                 amp_error(MOD_STR, "user ota trigger cb is NULL!");
//             }
//         }
//     }
//     return ret;
// }

static void aiot_device_notify(void *pdata)
{
    amp_error(MOD_STR, "aiot_device_notify IS CALLED");
    device_notify_param_t *param = (device_notify_param_t *)pdata;

    JSContext *ctx = js_get_context();
    JSValue obj = JS_NewObject(ctx);

    if (param->dm_recv) {
        switch (param->dm_recv_type) {
        case AIOT_DMRECV_PROPERTY_SET:
            amp_error(MOD_STR, "AIOT_DMRECV_PROPERTY_SET CASE");
            JS_SetPropertyStr(ctx, obj, "msg_id", JS_NewInt32(ctx, param->msg_id));
            JS_SetPropertyStr(ctx, obj, "params_len", JS_NewInt32(ctx, param->params_len));
            JS_SetPropertyStr(ctx, obj, "params", JS_NewStringLen(ctx, param->params,param->params_len));
            aos_free(param->params);
            break;
        case AIOT_DMRECV_ASYNC_SERVICE_INVOKE:
            amp_error(MOD_STR, "AIOT_DMRECV_ASYNC_SERVICE_INVOKE CASE");
            JS_SetPropertyStr(ctx, obj, "msg_id", JS_NewInt32(ctx, param->msg_id));
            JS_SetPropertyStr(ctx, obj, "service_id", JS_NewString(ctx, param->service_id));
            JS_SetPropertyStr(ctx, obj, "params_len", JS_NewInt32(ctx, param->params_len));
            JS_SetPropertyStr(ctx, obj, "params", JS_NewStringLen(ctx, param->params,param->params_len));
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
            JS_SetPropertyStr(ctx, obj, "code", JS_NewInt32(ctx, param->ret_code));
            JSValue aiot =  JS_NewObjectClass(ctx, js_aiot_device_class_id);
            aos_printf("iot linsdk handle %p\n", param->iot_device_handle);
            JS_SetOpaque(aiot, param->iot_device_handle);
            JS_SetPropertyStr(ctx, obj, "handle", aiot);
            break;
        default:
            aos_free(param);
            return;
        }
    } else {
        switch (param->recv_type) {
        case AIOT_MQTTRECV_PUB:
            JS_SetPropertyStr(ctx, obj, "code", JS_NewInt32(ctx, param->ret_code));
            JS_SetPropertyStr(ctx, obj, "handle", JS_NewInt32(ctx, param->iot_device_handle));
            JS_SetPropertyStr(ctx, obj, "topic", JS_NewStringLen(ctx,param->topic, param->topic_len));
            JS_SetPropertyStr(ctx, obj, "payload", JS_NewStringLen(ctx, param->payload,param->payload_len));
            aos_free(param->topic);
            aos_free(param->payload);
            break;
        default:
            aos_free(param);
            return;
        }
    }
    JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 1, &obj);
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, obj);
    JS_FreeValue(ctx, param->js_cb_ref);

    aos_free(param);
}

/* 用户数据接收处理回调函数 */
static void aiot_app_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    iot_device_handle_t *iot_device_handle = (iot_device_handle_t *)userdata;

    device_notify_param_t *param;

    amp_error(MOD_STR, "aiot_app_dm_recv_handler IS CALLED");

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

    amp_error(MOD_STR, "aiot_mqtt_message_cb IS CALLED");

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
    char current_amp_ver[64];
    ota_service_t *ota_svc = &g_aiot_device_appota_service;
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
static JSValue native_aiot_create_device(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = -1;
    void *mqtt_handle = NULL;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    u_int32_t keepaliveSec = 0;
    JSValue js_cb_ref;
    aos_task_t iot_device_task;
    iot_device_handle_t *iot_device_handle = NULL;
    ota_service_t *ota_svc = &g_aiot_device_appota_service;

    /* check paramters */
    JSValue options = argv[0];
    JSValue cb = argv[1];
    if (!JS_IsObject(options) || !JS_IsFunction(ctx, cb)) {
        amp_warn(MOD_STR, "parameter must be object and function\n");
        res = -1;
        goto out;
    }

    if (g_iot_clean_flag) {
        amp_warn(MOD_STR, "module source clean, ignore");
        goto out;
    }

    /* get device certificate */

    JSValue j_productKey = JS_GetPropertyStr(ctx, argv[0], "productKey");
    JSValue j_deviceName = JS_GetPropertyStr(ctx, argv[0], "deviceName");
    JSValue j_deviceSecret = JS_GetPropertyStr(ctx, argv[0], "deviceSecret");
    JSValue j_keepaliveSec = JS_GetPropertyStr(ctx, argv[0], "keepaliveSec");

    if(!JS_IsString(j_productKey) || !JS_IsString(j_deviceName) ||
        !JS_IsString(j_deviceSecret) || !JS_IsNumber(j_keepaliveSec)){
        amp_warn(MOD_STR, "Parameter invalid");
        res = -2;
        goto out;
    }

    productKey    = JS_ToCString(ctx, j_productKey);
    deviceName    = JS_ToCString(ctx, j_deviceName);
    deviceSecret  = JS_ToCString(ctx,j_deviceSecret);
    JS_ToUint32(ctx, &keepaliveSec, j_keepaliveSec);
    amp_info(MOD_STR, "productKey=%s deviceName=%s deviceSecret=%s keepaliveSec=%d\n",productKey,deviceName,deviceSecret,keepaliveSec);

    memset(ota_svc->pk, 0, sizeof(ota_svc->pk));
    memset(ota_svc->dn, 0, sizeof(ota_svc->dn));
    memset(ota_svc->ds, 0, sizeof(ota_svc->ds));
    memcpy(ota_svc->pk, productKey, strlen(productKey));
    memcpy(ota_svc->dn, deviceName, strlen(deviceName));
    memcpy(ota_svc->ds, deviceSecret, strlen(deviceSecret));

    aos_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICESECRET, deviceSecret, IOTX_DEVICE_SECRET_LEN, 1);

    js_cb_ref = JS_DupValue(ctx, cb);

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
        JS_FreeValue(ctx, js_cb_ref);
        aos_free(iot_device_handle);
        goto out;
    }


out:
    if(productKey != NULL){
       JS_FreeCString(ctx, productKey);
    }
    if(deviceName != NULL){
       JS_FreeCString(ctx, deviceName);
    }
    if(deviceSecret != NULL){
       JS_FreeCString(ctx, deviceSecret);
    }
    return JS_NewInt32(ctx, res);
}

// /* dynmic register */
// static duk_ret_t native_aiot_dynreg(duk_context *ctx)
// {
//     int res = -1;
//     int js_cb_ref = 0;

//     if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
//         amp_warn(MOD_STR, "parameter must be (object, function)");
//         goto out;
//     }

//     duk_get_prop_string(ctx, 0, "productKey");
//     duk_get_prop_string(ctx, 0, "deviceName");
//     duk_get_prop_string(ctx, 0, "productSecret");

//     char *productKey = (char *)duk_get_string(ctx, -3);
//     char *deviceName = (char *)duk_get_string(ctx, -2);
//     char *productSecret = (char *)duk_get_string(ctx, -1);

//     aos_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
//     aos_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
//     aos_kv_set(AMP_CUSTOMER_PRODUCTSECRET, productSecret, IOTX_PRODUCT_SECRET_LEN, 1);

//     duk_dup(ctx, 1);
//     js_cb_ref = be_ref(ctx);

//     res = aiot_dynreg_http(js_cb_ref);
//     if (res < STATE_SUCCESS) {
//         amp_debug(MOD_STR, "device dynmic register failed");
//     }

// out:
//     duk_push_int(ctx, res);
//     return 1;
// }

/* post event */
static JSValue native_aiot_postEvent(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *event_id = NULL;
    char *event_payload = NULL;
    amp_warn(MOD_STR, "native_aiot_postEvent is called");

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    JSValue id = JS_GetPropertyStr(ctx, argv[0], "id");
     if (JS_IsString(id)) {
        event_id = JS_ToCString(ctx, id);
    }
    if (event_id != NULL) {
        JS_FreeCString(ctx, event_id);
    }
    JS_FreeValue(ctx, id);

    JSValue params = JS_GetPropertyStr(ctx, argv[0], "params");
    JSValue jjson = JS_UNDEFINED;
    if (!JS_IsUndefined(params)) {
        if (JS_VALUE_GET_TAG(params) != JS_TAG_STRING) {
            jjson = JS_JSONStringify(ctx, params, JS_NULL, JS_NULL);
            event_payload = JS_ToCString(ctx, jjson);
        } else {
            event_payload = JS_ToCString(ctx, params);
        }
    }
    if (event_payload != NULL) {
        JS_FreeCString(ctx, event_payload);
    }
    if (!JS_IsUndefined(jjson)) {
        JS_FreeValue(ctx, jjson);
    }
    JS_FreeValue(ctx, params);
    amp_warn(MOD_STR, "native_aiot_postEvent event_id=%s event_payload=%s\n",event_id,event_payload);

    /* callback */
    JSValue cb = argv[1];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_POST_EVENT_REF] = cb;

    res = aiot_app_send_event_post(iot_device_handle->dm_handle, event_id, event_payload);
    if (res < STATE_SUCCESS) {
        amp_warn(MOD_STR, "post event failed!");
    }

out:
    return JS_NewInt32(ctx, res);
}

/* post props */
static JSValue native_aiot_postProps(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *payload_str= NULL;
    amp_debug(MOD_STR, "native_aiot_postProps is called");

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    // JSValue payload = JS_GetPropertyStr(ctx, argv[0], "payload");
    JSValue payload = argv[0];

    // const char* payload_str = NULL;
    JSValue jjson = JS_UNDEFINED;
    if (!JS_IsUndefined(payload)) {
        if (JS_VALUE_GET_TAG(payload) != JS_TAG_STRING) {
            jjson = JS_JSONStringify(ctx, payload, JS_NULL, JS_NULL);
            payload_str = JS_ToCString(ctx, jjson);
        } else {
            payload_str = JS_ToCString(ctx, payload);
        }
    }
    amp_debug(MOD_STR, "native_aiot_postProps payload_str=%s \n",payload_str);
    JS_FreeCString(ctx, payload_str);
    if (!JS_IsUndefined(jjson)) {
        JS_FreeValue(ctx, jjson);
    }

    JSValue cb = argv[1];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_POST_PROPS_REF] = cb;

    res = aiot_app_send_property_post(iot_device_handle->dm_handle, payload_str);
    if (res < STATE_SUCCESS) {
        amp_warn(MOD_STR, "property post failed, res:%d", res);
    }

out:
    return JS_NewInt32(ctx, res);
}

/*  onprops */
static JSValue native_aiot_onProps(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = STATE_SUCCESS;
    iot_device_handle_t *iot_device_handle = NULL;

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    JSValue cb = argv[0];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONPROPS_REF] = cb;

out:
    return JS_NewInt32(ctx, res);
}

/*  onService */
static JSValue native_aiot_onService(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = STATE_SUCCESS;
    iot_device_handle_t *iot_device_handle = NULL;

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    JSValue cb = argv[0];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONSERVICE_REF] = cb;

out:
    return JS_NewInt32(ctx, res);
}

/* publish */
static JSValue native_aiot_publish(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *topic;
    char *payload;
    uint16_t payload_len = 0;
    uint8_t qos = 0;

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    JSValue val = JS_GetPropertyStr(ctx, argv[0], "topic");
    topic = JS_ToCString(ctx,val);
    JS_FreeValue(ctx,val);

    val = JS_GetPropertyStr(ctx, argv[0], "payload");
    payload = JS_ToCString(ctx,val);
    JS_FreeValue(ctx,val);
    payload_len = strlen(payload);

    val = JS_GetPropertyStr(ctx, argv[0], "qos");
    JS_ToInt32(ctx,&qos, val);
    JS_FreeValue(ctx,val);

    JSValue cb = argv[1];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_PUBLISH_REF] = cb;

    amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d", topic, payload, qos);

    res = aiot_mqtt_pub(iot_device_handle->mqtt_handle, topic, payload, payload_len, qos);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt publish failed\n");
        goto out;
    }

out:
    if(topic != NULL){
       JS_FreeCString(ctx, topic);
    }
    if(payload != NULL){
       JS_FreeCString(ctx, payload);
    }
    return JS_NewInt32(ctx, res);
}

/* unsubscribe */
static JSValue native_aiot_unsubscribe(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *topic;
    uint8_t qos = 0;

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    JSValue val = JS_GetPropertyStr(ctx, argv[0], "topic");
    topic = JS_ToCString(ctx, val);
    JS_FreeValue(ctx, val);

    JSValue cb = argv[1];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_UNSUBSCRIBE_REF] = cb;

    amp_debug(MOD_STR, "unsubscribe topic: %s", topic);

    res = aiot_mqtt_unsub(iot_device_handle->mqtt_handle, topic);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt unsubscribe failed");
        goto out;
    }

out:
    if(topic != NULL) {
       JS_FreeCString(ctx, topic);
    }
    return JS_NewInt32(ctx,res);
}

/* subscribe */
static JSValue native_aiot_subscribe(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res = -1;
    iot_device_handle_t *iot_device_handle = NULL;
    char *topic = NULL;
    uint32_t qos = 0;
    amp_debug(MOD_STR, "native_aiot_subscribe is called");

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    JSValue val = JS_GetPropertyStr(ctx, argv[0], "topic");
    topic = JS_ToCString(ctx, val);
    JS_FreeValue(ctx, val);
    val = JS_GetPropertyStr(ctx, argv[0], "qos");
    JS_ToInt32(ctx,&qos, val);
    JS_FreeValue(ctx, val);

    JSValue cb = argv[1];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_SUBSCRIBE_REF] = cb;

    amp_debug(MOD_STR, "subscribe topic: %s, qos is: %d", topic, qos);

    res = aiot_mqtt_sub(iot_device_handle->mqtt_handle, topic, NULL, (uint8_t)qos, NULL);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt subscribe failed\n");
        goto out;
    }

out:
    if(topic != NULL){
       JS_FreeCString(ctx, topic);
    }
    return JS_NewInt32(ctx,res);
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
static JSValue native_aiot_close(JSContext *ctx, JSValueConst this_val,int argc, JSValueConst *argv)
{
    int res     = -1;
    iot_device_handle_t *iot_device_handle = NULL;

    iot_device_handle = JS_GetOpaque2(ctx, this_val, js_aiot_device_class_id);
    if (!iot_device_handle) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    JSValue cb = argv[1];
    JS_DupValue(ctx, cb);
    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_END_CLIENT_REF] = cb;

    g_iot_close_flag = 1;
    aos_sem_wait(&g_iot_close_sem, 200 + 50);
    res = aiot_mqtt_client_stop(&iot_device_handle->mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot stop failed");
        goto out;
    }

    g_iot_close_flag = 0;
    aos_free(iot_device_handle);
    return JS_NewInt32(ctx,0);

out:
    // duk_push_int(ctx, res);
    return JS_NewInt32(ctx,res);
}

static void module_iot_source_clean(void)
{
    JSContext *ctx = js_get_context();

    if (g_iot_conn_flag) {
        g_iot_close_flag = 1;
        aos_sem_wait(&g_iot_close_sem, 5100);
        g_iot_close_flag = 0;
        aos_msleep(10);
    }

    g_iot_clean_flag = 1;
}

static JSClassDef js_aiot_device_class = {
    "AIOT_DEVICE",
};

static const JSCFunctionListEntry js_aiot_device_funcs[] = {
    JS_CFUNC_DEF("device", 1, native_aiot_create_device ),
    JS_CFUNC_DEF("subscribe", 3,   native_aiot_subscribe),
    JS_CFUNC_DEF("unsubscribe", 3,  native_aiot_unsubscribe),
    JS_CFUNC_DEF("publish",  3,    native_aiot_publish),
    JS_CFUNC_DEF("postProps", 3,   native_aiot_postProps),
    JS_CFUNC_DEF("onProps",  2,    native_aiot_onProps),
    JS_CFUNC_DEF("postEvent", 3,   native_aiot_postEvent),
    JS_CFUNC_DEF("onService", 2,   native_aiot_onService),
    JS_CFUNC_DEF("end",     2,     native_aiot_close),

};

static int js_aiot_device_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_aiot_device_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_aiot_device_class_id, &js_aiot_device_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_aiot_device_funcs, countof(js_aiot_device_funcs));
    JS_SetClassProto(ctx, js_aiot_device_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_aiot_device_funcs, countof(js_aiot_device_funcs));
}

JSModuleDef *js_init_module_aiot_device(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_aiot_device_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_aiot_device_funcs, countof(js_aiot_device_funcs));
    return m;
}

void module_aiot_device_register(void)
{
    amp_debug(MOD_STR, "module_aiot_device_register");
    JSContext *ctx = js_get_context();

    if (!g_iot_close_sem) {
        if (aos_sem_new(&g_iot_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create iot sem fail");
            return;
        }
    }
    g_iot_clean_flag = 0;

    amp_module_free_register(module_iot_source_clean);

    aos_printf("module aiot_device register\n");
    js_init_module_aiot_device(ctx, "AIOT_DEVICE");
}