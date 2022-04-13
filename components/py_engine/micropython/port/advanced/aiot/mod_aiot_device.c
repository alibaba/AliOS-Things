/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "amp_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "amp_kv.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dm_api.h"
#include "aiot_state_api.h"
// #include "be_inl.h"
// #include "ota_agent.h"
#include "module_aiot.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#define MOD_STR "AIOT"

#define OTA_MODE_NAME "system"

static char g_iot_close_flag = 0;
static char g_iot_conn_flag = 0;
static char g_iot_clean_flag = 0;
static void *g_iot_close_sem = NULL;


// ota_service_t g_appota_service;

typedef enum {
    ON_CONNECT    = 1,
    ON_DISCONNECT     = 3,
    ON_SERVICE = 5,
    ON_PROPS     = 7,
    ON_CLOSE   = 2,
    ON_ERROR    = 4,
} lk_callback_func_t;

// extern const char *amp_jsapp_version_get(void);

static int user_py_ota_triger_cb(void* pctx, char *ver, char *url)
{
    //int ret = OTA_TRANSPORT_PAR_FAIL;
    // ota_service_t *ota_svc = (ota_service_t *)pctx;
    // char *current_ver = NULL;
    // current_ver = amp_jsapp_version_get();
    // if((ota_svc != NULL) && (ver != NULL) && (current_ver != NULL)) {
    //     ret = 0;
    //     if(strncmp(ver, current_ver, strlen(ver)) <= 0) {
    //         ret = OTA_TRANSPORT_VER_FAIL;
    //         amp_error(MOD_STR, "user ota version too old!");
    //     }
    //     else {
    //         if(ota_svc->feedback_func.on_user_event_cb != NULL) {
    //             ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
    //         }
    //         else {
    //             ret = OTA_TRANSPORT_PAR_FAIL;
    //             amp_error(MOD_STR, "user ota trigger cb is NULL!");
    //         }
    //     }
    // }
    return 0;
}

static int user_interval_upgrade_cb(void* pctx, char *ver, char *url)
{
    //int ret = OTA_TRANSPORT_PAR_FAIL;
    // ota_service_t *ota_svc = (ota_service_t *)pctx;
    // char current_amp_ver[64];
    // memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
    // amp_app_version_get(current_amp_ver);
    // if((pctx != NULL) && (ver != NULL) && (url != NULL)) {
    //     ret = 0;
    //     if(strncmp(ver, current_amp_ver, strlen(ver)) <= 0) {
    //         ret = OTA_TRANSPORT_VER_FAIL;
    //         amp_error(MOD_STR, "system ota version too old!");
    //     }
    //     else {
    //         if(ota_svc->feedback_func.on_user_event_cb != NULL) {
    //             ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
    //         }
    //         else {
    //             ret = OTA_TRANSPORT_PAR_FAIL;
    //             amp_error(MOD_STR, "user ota trigger cb is NULL!");
    //         }

    //     }
    // }
    return 0;
}

/* 用户数据接收处理回调函数 */
static void aiot_app_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    printf("demo_dm_recv_handler, type = %d\r\n", recv->type);
    iot_device_handle_t *iot_device_handle = (iot_device_handle_t *)userdata;
    // duk_context *ctx = be_get_context();
    int js_cb_ref = 0;

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

            // js_cb_ref = iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONPROPS_REF];
            // be_push_ref(ctx, js_cb_ref);
            // duk_push_object(ctx);
            // {
            //     duk_push_int(ctx, (unsigned long)recv->data.property_set.msg_id);
            //     duk_put_prop_string(ctx, -2, "msg_id");
            //     duk_push_int(ctx, recv->data.property_set.params_len);
            //     duk_put_prop_string(ctx, -2, "params_len");
            //     duk_push_lstring(ctx, recv->data.property_set.params, recv->data.property_set.params_len);
            //     duk_put_prop_string(ctx, -2, "params");
            // }

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
            amp_debug("msg_id = %ld, service_id = %s, params = %.*s",
                   (unsigned long)recv->data.async_service_invoke.msg_id,
                   recv->data.async_service_invoke.service_id,
                   recv->data.async_service_invoke.params_len,
                   recv->data.async_service_invoke.params);

            js_cb_ref = iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONSERVICE_REF];
            // be_push_ref(ctx, js_cb_ref);
            // duk_push_object(ctx);
            // {
            //     duk_push_int(ctx, (unsigned long)recv->data.async_service_invoke.msg_id);
            //     duk_put_prop_string(ctx, -2, "msg_id");
            //     duk_push_int(ctx, recv->data.async_service_invoke.service_id);
            //     duk_put_prop_string(ctx, -2, "service_id");
            //     duk_push_string(ctx, recv->data.async_service_invoke.params_len);
            //     duk_put_prop_string(ctx, -2, "params_len");
            //     duk_push_lstring(ctx, recv->data.async_service_invoke.params, recv->data.async_service_invoke.params_len);
            //     duk_put_prop_string(ctx, -2, "params");
            // }    

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

//     if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
//         amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
//     }
//     duk_pop(ctx);
//     duk_gc(ctx, 0);
 }

static void aiot_device_handle_notify(void *pdata)
{
    iot_device_handle_t *iot_device_handle = (iot_device_handle_t *)pdata;

    // duk_context *ctx = be_get_context();
    // be_push_ref(ctx, iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_CREATE_DEV_REF]);

    // duk_push_object(ctx);
    // {
    //     duk_push_int(ctx, iot_device_handle->res);
    //     duk_put_prop_string(ctx, -2, "code");
    //     duk_push_pointer(ctx, iot_device_handle);
    //     duk_put_prop_string(ctx, -2, "handle");
    // }

    // if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
    //     amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    // }
    // duk_pop(ctx);

    // duk_gc(ctx, 0);
}

static void *aiot_device_connect(void *pdata)
{
    int res = -1;
    char current_amp_ver[64];
    //ota_service_t *ota_svc = &g_appota_service;
    iot_device_handle_t *iot_device_handle = (iot_device_handle_t *)pdata;
    
    uint16_t keepaliveSec = 0;
    
    int js_cb_ref = iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_CREATE_DEV_REF];
    keepaliveSec = iot_device_handle->keepaliveSec;

    res = aiot_mqtt_client_start(&iot_device_handle->mqtt_handle, keepaliveSec, js_cb_ref);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "mqtt client create failed");
        amp_free(iot_device_handle);
        return NULL;
    }

    g_iot_conn_flag = 1;
    iot_device_handle->res = res;

    /* device model service */
    iot_device_handle->dm_handle = aiot_dm_init();
    if (iot_device_handle->dm_handle == NULL) {
        amp_debug(MOD_STR, "aiot_dm_init failed");
        amp_free(iot_device_handle);
        return NULL;
    }

    /* 配置MQTT实例句柄 */
    aiot_dm_setopt(iot_device_handle->dm_handle, AIOT_DMOPT_MQTT_HANDLE, iot_device_handle->mqtt_handle);
    /* 配置消息接收处理回调函数 */
    aiot_dm_setopt(iot_device_handle->dm_handle, AIOT_DMOPT_RECV_HANDLER, (void *)aiot_app_dm_recv_handler);
    /* 配置回调函数参数 */
    aiot_dm_setopt(iot_device_handle->dm_handle, AIOT_DMOPT_USERDATA, iot_device_handle);

    /* return aiot_device_handle */
    amp_task_schedule_call(aiot_device_handle_notify, iot_device_handle);

    /* app ota service */
    // ota_register_cb(ota_svc, OTA_CB_ID_UPGRADE, (void*)user_jsapp_ota_triger_cb);
    // ota_register_cb(ota_svc, OTA_CB_ID_MODULE_UPGRADE, (void*)user_interval_upgrade_cb);
    //ota_svc->module_ota = 0;
    //amp_debug(MOD_STR, "[%s %d]module ota = %d",__func__,__LINE__,ota_svc->module_ota);

    // memset(ota_svc->module_name, 0x00, sizeof(ota_svc->module_name));
    // strcpy(ota_svc->module_name, OTA_MODE_NAME);

    // ota_svc->mqtt_client = iot_device_handle->mqtt_handle;
    // res = ota_service_init(ota_svc);
    // if(res >= STATE_SUCCESS) {
    //     amp_debug(MOD_STR, "user ota init ok!");
    // }
    // else {
    //     amp_error(MOD_STR, "user ota init failed!");
    // }

    //report app js version
    // res = ota_transport_inform(ota_svc, NULL, amp_jsapp_version_get());
    // if(res < STATE_SUCCESS) {
    //     amp_error(MOD_STR, "user ota report ver failed!");
    // }

    // memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
    // amp_app_version_get(current_amp_ver);
    // //report amp app version
    // res = ota_transport_inform(ota_svc, OTA_MODE_NAME, current_amp_ver);
    // if(res < STATE_SUCCESS) {
    //     amp_error(MOD_STR, "system ota report ver failed!");
    // }

    while(!g_iot_close_flag){
        amp_msleep(1000);
    }

    // duk_context *ctx = be_get_context();
    // be_unref(ctx, iot_device_handle->js_cb_ref);

    aiot_mqtt_client_stop(&iot_device_handle->mqtt_handle);
    //ota_service_deinit(ota_svc);

    amp_free(iot_device_handle);
    g_iot_conn_flag = 0;
    amp_semaphore_post(g_iot_close_sem);
    amp_thread_delete(NULL);
    return NULL;
}

/*************************************************************************************
 * Function:    native_aiot_create_device
 * Description: python native addon for UDP.createSocket();
 * Called by:   python api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static mp_obj_t native_aiot_create_device(size_t n_args, const mp_obj_t *args)
{
    int res = -1;
    void *mqtt_handle = NULL;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    int keepaliveSec = 0;
    int js_cb_ref = 0;
    void *iot_device_task;
    int iot_device_task_stack_used;
    iot_device_handle_t *iot_device_handle = NULL;
    amp_os_thread_param_t iot_device_task_params = {0};
    // ota_service_t *ota_svc = &g_appota_service;


    if (n_args < 5)
    {
        printf("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    //iot_host_name = mp_obj_str_get_str(args[0]);
    productKey = mp_obj_str_get_str(args[1]);
    deviceName = mp_obj_str_get_str(args[2]);
    deviceSecret = mp_obj_str_get_str(args[3]);
    productSecret = mp_obj_str_get_str(args[4]);
    keepaliveSec = 30 ;

    //todo:grady 可选参数 keepaliveSec 待实现
    //duk_get_prop_string(ctx, 0, "keepaliveSec");



//     productKey    = duk_get_string(ctx, -4);
//     deviceName    = duk_get_string(ctx, -3);
//     deviceSecret  = duk_get_string(ctx, -2);
//     keepaliveSec  = duk_get_number(ctx, -1);

//     memset(ota_svc->pk, 0, sizeof(ota_svc->pk));
//     memset(ota_svc->dn, 0, sizeof(ota_svc->dn));
//     memset(ota_svc->ds, 0, sizeof(ota_svc->ds));
//     memcpy(ota_svc->pk, productKey, strlen(productKey));
//     memcpy(ota_svc->dn, deviceName, strlen(deviceName));
//     memcpy(ota_svc->ds, deviceSecret, strlen(deviceSecret));

    amp_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
    amp_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
    amp_kv_set(AMP_CUSTOMER_DEVICESECRET, deviceSecret, IOTX_DEVICE_SECRET_LEN, 1);

//     duk_dup(ctx, 1);
//     js_cb_ref = be_ref(ctx);

    iot_device_handle =(iot_device_handle_t *)amp_malloc(sizeof(iot_device_handle_t));
    if (!iot_device_handle) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_CREATE_DEV_REF] = js_cb_ref;
    iot_device_handle->keepaliveSec = keepaliveSec;

    iot_device_task_params.name = "amp aiot device task";
    iot_device_task_params.priority = amp_get_default_task_priority();
    iot_device_task_params.stack_size = 1024 * 10;

    res = amp_thread_create(&iot_device_task, aiot_device_connect, iot_device_handle, &iot_device_task_params, &iot_device_task_stack_used);
    if (res != STATE_SUCCESS) {
        amp_warn(MOD_STR, "iot create task failed");
        amp_free(iot_device_handle);
        goto out;
    }

out:
    return mp_const_none;
}


STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_create_device, 5, native_aiot_create_device);

/* dynmic register */
static mp_obj_t native_aiot_dynreg(void)
{
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

//     amp_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
//     amp_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
//     amp_kv_set(AMP_CUSTOMER_PRODUCTSECRET, productSecret, IOTX_PRODUCT_SECRET_LEN, 1);

//     duk_dup(ctx, 1);
//     js_cb_ref = be_ref(ctx);

//     res = aiot_dynreg_http(js_cb_ref);
//     if (res < STATE_SUCCESS) {
//         amp_debug(MOD_STR, "device dynmic register failed");
//     }

// out:
//     duk_push_int(ctx, res);
    return mp_obj_new_int(1);
}

/* post event */
static mp_obj_t native_aiot_postEvent(void)
{
//     int res = -1;
//     iot_device_handle_t *iot_device_handle = NULL;
//     char *event_id;
//     char *event_payload;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
//         amp_warn(MOD_STR, "parameter must be (number, object, function)");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);

//     duk_get_prop_string(ctx, 1, "id");
//     duk_get_prop_string(ctx, 1, "params");

//     event_id = (char *)duk_get_string(ctx, -2);
//     event_payload = (char *)duk_get_string(ctx, -1);

//     duk_dup(ctx, 2);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_POST_EVENT_REF] = be_ref(ctx);

//     res = aiot_app_send_event_post(iot_device_handle->dm_handle, event_id, event_payload);
//     if (res < STATE_SUCCESS) {
//         amp_warn(MOD_STR, "post event failed!");
//     }

// out:
//     duk_push_int(ctx, res);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_post_event, 0, native_aiot_postEvent);

/* post props */
static mp_obj_t native_aiot_postProps()
{
//     int res = -1;
//     iot_device_handle_t *iot_device_handle = NULL;
//     char *params;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2)) {
//         amp_warn(MOD_STR, "parameter must be (pointer, string, function)");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);
//     params = (char *)duk_get_string(ctx, 1);

//     duk_dup(ctx, 2);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_POST_PROPS_REF] = be_ref(ctx);

//     res = aiot_app_send_property_post(iot_device_handle->dm_handle, params);
//     if (res < STATE_SUCCESS) {
//         amp_warn(MOD_STR, "property post failed");
//     }

// out:
//     duk_push_int(ctx, res);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_post_property, 0, native_aiot_postProps);


/*  onprops */
static mp_obj_t native_aiot_onProps()
{
//     int res = STATE_SUCCESS;
//     iot_device_handle_t *iot_device_handle = NULL;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
//         amp_warn(MOD_STR, "parameter must be (pointer, function)");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);

//     duk_dup(ctx, 1);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONPROPS_REF] = be_ref(ctx);

// out:
//     duk_push_int(ctx, res);
    return mp_obj_new_int(1);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_on_props,0,native_aiot_onProps);

/*  onService */
static mp_obj_t native_aiot_onService()
{
//     int res = STATE_SUCCESS;
//     iot_device_handle_t *iot_device_handle = NULL;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
//         amp_warn(MOD_STR, "parameter must be (pointer, function)");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);

//     duk_dup(ctx, 1);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_ONSERVICE_REF] = be_ref(ctx);

// out:
//     duk_push_int(ctx, res);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_on_service,0,native_aiot_onService);


/* publish */
static mp_obj_t native_aiot_publish()
{
//     int res = -1;
//     iot_device_handle_t *iot_device_handle = NULL;
//     char *topic;
//     char *payload;
//     uint16_t payload_len = 0;
//     uint8_t qos = 0;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
//         amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);
//     if (iot_device_handle == NULL) {
//         amp_warn(MOD_STR, "mqtt handle is null");
//         goto out;
//     }

//     duk_get_prop_string(ctx, 1, "topic");
//     duk_get_prop_string(ctx, 1, "payload");
//     duk_get_prop_string(ctx, 1, "qos");

//     if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1))
//     {
//         amp_warn(MOD_STR, "invalid params");
//         duk_pop_n(ctx, 3);
//         goto out;
//     }

//     qos = duk_get_number(ctx, -1);
//     payload = (char *)duk_get_string(ctx, -2);
//     topic = (char *)duk_get_string(ctx, -3);
//     payload_len = strlen(payload);

//     duk_dup(ctx, 2);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_PUBLISH_REF] = be_ref(ctx);

//     amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d", topic, payload, qos);

//     res = aiot_mqtt_pub(iot_device_handle->mqtt_handle, topic, payload, payload_len, qos);
//     if (res < STATE_SUCCESS) {
//         amp_error(MOD_STR, "aiot app mqtt publish failed\n");
//         goto out;
//     }

// out:
//     duk_push_int(ctx, res);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_publish, 0, native_aiot_publish);


/* unsubscribe */
static mp_obj_t native_aiot_unsubscribe()
{
    // int res = -1;
    // iot_device_handle_t *iot_device_handle = NULL;
    // char *topic;
    // uint8_t qos = 0;

    // if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2)) {
    //     amp_warn(MOD_STR, "parameter must be (pointer, string)");
    //     goto out;
    // }

    // iot_device_handle = duk_get_pointer(ctx, 0);
    // if (iot_device_handle == NULL) {
    //     amp_warn(MOD_STR, "mqtt handle is null");
    //     goto out;
    // }

//     topic = (char *)duk_get_string(ctx, 1);

//     duk_dup(ctx, 2);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_UNSUBSCRIBE_REF] = be_ref(ctx);

//     amp_debug(MOD_STR, "unsubscribe topic: %s", topic);

//     res = aiot_mqtt_unsub(iot_device_handle->mqtt_handle, topic);
//     if (res < STATE_SUCCESS) {
//         amp_error(MOD_STR, "aiot app mqtt unsubscribe failed");
//         goto out;
//     }

// out:
//     duk_push_int(ctx, res);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_unsubscribe, 0, native_aiot_unsubscribe);


/* subscribe */
static mp_obj_t native_aiot_subscribe()
{
//     int res = -1;
//     iot_device_handle_t *iot_device_handle = NULL;
//     char *topic = NULL;
//     uint8_t qos = 0;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
//         amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);
//     if (iot_device_handle == NULL) {
//         amp_warn(MOD_STR, "mqtt handle is null");
//         goto out;
//     }

//     duk_get_prop_string(ctx, 1, "topic");
//     duk_get_prop_string(ctx, 1, "qos");

//     if (!duk_is_number(ctx, -1) || !duk_is_string(ctx, -2))
//     {
//         amp_warn(MOD_STR, "invalid params");
//         duk_pop_n(ctx, 2);
//         goto out;
//     }

//     qos = duk_get_number(ctx, -1);
//     topic = (char *)duk_get_string(ctx, -2);

//     duk_dup(ctx, 2);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_SUBSCRIBE_REF] = be_ref(ctx);

//     amp_debug(MOD_STR, "subscribe topic: %s, qos is: %d", topic, qos);

//     res = aiot_mqtt_sub(iot_device_handle->mqtt_handle, topic, NULL, qos, NULL);
//     if (res < STATE_SUCCESS) {
//         amp_error(MOD_STR, "aiot app mqtt subscribe failed\n");
//         goto out;
//     }

// out:
//     duk_push_int(ctx, res);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_subscribe, 0, native_aiot_subscribe);


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
static mp_obj_t native_aiot_close()
{
//     int res     = -1;
//     iot_device_handle_t *iot_device_handle = NULL;

//     if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
//         amp_warn(MOD_STR, "parameter must be pointer and function");
//         goto out;
//     }

//     iot_device_handle = duk_get_pointer(ctx, 0);
//     if (iot_device_handle == NULL) {
//         amp_warn(MOD_STR, "iot_device_hanle is null");
//         goto out;
//     }

//     duk_dup(ctx, 1);
//     iot_device_handle->js_cb_ref[AIOT_DEV_JSCALLBACK_END_CLIENT_REF] = be_ref(ctx);

//     g_iot_close_flag = 1;
//     amp_semaphore_wait(g_iot_close_sem, 200 + 50);
//     res = aiot_mqtt_client_stop(&iot_device_handle->mqtt_handle);
//     if (res < STATE_SUCCESS) {
//         amp_debug(MOD_STR, "aiot stop failed");
//         goto out;
//     }

//     g_iot_close_flag = 0;
//     duk_push_int(ctx, 0);
//     amp_free(iot_device_handle);
//     return 1;

// out:
//     duk_push_int(ctx, res);
    return mp_obj_new_int(1);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(device_obj_close, 0, native_aiot_close);


static void module_iot_source_clean(void)
{
    // duk_context *ctx = be_get_context();

    // if (g_iot_conn_flag) {
    //     g_iot_close_flag = 1;
    //     amp_semaphore_wait(g_iot_close_sem, 5100);
    //     g_iot_close_flag = 0;
    //     amp_msleep(10);
    // }

    // g_iot_clean_flag = 1;
}

// void module_aiot_device_register(void)
// {
//     // duk_context *ctx = be_get_context();

//     // if (!g_iot_close_sem) {
//     //     g_iot_close_sem = amp_semaphore_create();
//     //     if (!g_iot_close_sem) {
//     //         amp_error(MOD_STR, "create iot sem fail");
//     //         return;
//     //     }
//     // }
//     // g_iot_clean_flag = 0;

//     // amp_module_free_register(module_iot_source_clean);

//     // duk_push_object(ctx);

//     AMP_ADD_FUNCTION("device",       native_aiot_create_device, 2);
//     AMP_ADD_FUNCTION("subscribe",    native_aiot_subscribe, 3);
//     AMP_ADD_FUNCTION("unsubscribe",  native_aiot_unsubscribe, 3);
//     AMP_ADD_FUNCTION("publish",      native_aiot_publish, 3);
//     AMP_ADD_FUNCTION("postProps",    native_aiot_postProps, 3);
//     AMP_ADD_FUNCTION("onProps",      native_aiot_onProps, 2);
//     AMP_ADD_FUNCTION("postEvent",    native_aiot_postEvent, 3);
//     AMP_ADD_FUNCTION("register",     native_aiot_dynreg, 2);
//     AMP_ADD_FUNCTION("onService",    native_aiot_onService, 2);
//     AMP_ADD_FUNCTION("end",          native_aiot_close, 2);

//     // duk_put_prop_string(ctx, -2, "AIOT_DEVICE");
// }


STATIC const mp_rom_map_elem_t device_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_device)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_create_device), MP_ROM_PTR(&device_obj_create_device)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_subscribe), MP_ROM_PTR(&device_obj_subscribe)},    
    {MP_OBJ_NEW_QSTR(MP_QSTR_unsubscribe), MP_ROM_PTR(&device_obj_unsubscribe)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_publish), MP_ROM_PTR(&device_obj_publish)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_register), MP_ROM_PTR(&device_obj_publish)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_register_call_back), MP_ROM_PTR(&device_obj_subscribe)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_post_property), MP_ROM_PTR(&device_obj_post_property)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_post_event), MP_ROM_PTR(&device_obj_post_event)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_on_props), MP_ROM_PTR(&device_obj_on_props)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_on_service), MP_ROM_PTR(&device_obj_on_service)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_close), MP_ROM_PTR(&device_obj_close)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_CONNECT), MP_ROM_INT(ON_CONNECT)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_DISCONNECT), MP_ROM_INT(ON_DISCONNECT)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_SERVICE), MP_ROM_INT(ON_SERVICE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_PROPS), MP_ROM_INT(ON_PROPS)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_CLOSE), MP_ROM_INT(ON_CLOSE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ON_ERROR), MP_ROM_INT(ON_ERROR)},
};

STATIC MP_DEFINE_CONST_DICT(device_module_globals, device_module_globals_table);

const mp_obj_module_t device_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&device_module_globals,
};
