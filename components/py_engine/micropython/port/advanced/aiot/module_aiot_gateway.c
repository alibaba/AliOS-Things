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
#include "aiot_subdev_api.h"
#include "aiot_state_api.h"
#include "be_inl.h"
#include "ota_agent.h"
#include "module_aiot.h"

#define MOD_STR "AIOT"

#define OTA_MODE_NAME "system"

static char g_iot_close_flag = 0;
static char g_iot_conn_flag = 0;
static char g_iot_clean_flag = 0;
static void *g_iot_close_sem = NULL;

ota_service_t g_appota_service;

extern const char *amp_jsapp_version_get(void);

static int user_jsapp_ota_triger_cb(void* pctx, char *ver, char *url)
{
    int ret = OTA_TRANSPORT_PAR_FAIL;
    ota_service_t *ota_svc = (ota_service_t *)pctx;
    char *current_ver = NULL;
    current_ver = amp_jsapp_version_get();
    if((ota_svc != NULL) && (ver != NULL) && (current_ver != NULL)) {
        ret = 0;
        if(strncmp(ver, current_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "user ota version too old!");
        }
        else {
            if(ota_svc->feedback_func.on_user_event_cb != NULL) {
                ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
            }
            else {
                ret = OTA_TRANSPORT_PAR_FAIL;
                amp_error(MOD_STR, "user ota trigger cb is NULL!");
            }
        }
    }
    return ret;
}

static int user_interval_upgrade_cb(void* pctx, char *ver, char *url)
{
    int ret = OTA_TRANSPORT_PAR_FAIL;
    ota_service_t *ota_svc = (ota_service_t *)pctx;
    char current_amp_ver[64];
    memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
    amp_app_version_get(current_amp_ver);
    if((pctx != NULL) && (ver != NULL) && (url != NULL)) {
        ret = 0;
        if(strncmp(ver, current_amp_ver, strlen(ver)) <= 0) {
            ret = OTA_TRANSPORT_VER_FAIL;
            amp_error(MOD_STR, "system ota version too old!");
        }
        else {
            if(ota_svc->feedback_func.on_user_event_cb != NULL) {
                ota_svc->feedback_func.on_user_event_cb(OTA_EVENT_UPGRADE_TRIGGER, 0, ota_svc->feedback_func.param);
            }
            else {
                ret = OTA_TRANSPORT_PAR_FAIL;
                amp_error(MOD_STR, "user ota trigger cb is NULL!");
            }

        }
    }
    return ret;
}

void aiot_app_subdev_recv_handler(void *handle, const aiot_subdev_recv_t *packet, void *user_data)
{
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)user_data;
    int js_cb_ref = 0;

    switch (packet->type) {
        case AIOT_SUBDEVRECV_TOPO_ADD_REPLY:
            js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF];

        case AIOT_SUBDEVRECV_TOPO_DELETE_REPLY:
            js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF];

        case AIOT_SUBDEVRECV_TOPO_GET_REPLY:
            js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF];

        case AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY:
            js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF];

        case AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY:
            js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF];

        case AIOT_SUBDEVRECV_SUB_REGISTER_REPLY:
            js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF];

        case AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY:
        {
            printf("msgid        : %d\n", packet->data.generic_reply.msg_id);
            printf("code         : %d\n", packet->data.generic_reply.code);
            printf("product key  : %s\n", packet->data.generic_reply.product_key);
            printf("device name  : %s\n", packet->data.generic_reply.device_name);
            printf("message      : %s\n", (packet->data.generic_reply.message == NULL)?("NULL"):(packet->data.generic_reply.message));
            printf("data         : %s\n", packet->data.generic_reply.data);
        }
        break;
        case AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY: {
            printf("msgid        : %d\n", packet->data.generic_notify.msg_id);
            printf("product key  : %s\n", packet->data.generic_notify.product_key);
            printf("device name  : %s\n", packet->data.generic_notify.device_name);
            printf("params       : %s\n", packet->data.generic_notify.params);
        }
        break;
        default: {

        }
    }

    // amp_task_schedule_call(aiot_gateway_message_notify, p);
}

static void aiot_gateway_handle_notify(void *pdata)
{
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF]);

    duk_push_object(ctx);
    {
        duk_push_int(ctx, iot_gateway_handle->res);
        duk_put_prop_string(ctx, -2, "code");
        duk_push_pointer(ctx, iot_gateway_handle);
        duk_put_prop_string(ctx, -2, "handle");
    }

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
}

static void *aiot_gateway_connect(void *pdata)
{
    int res = -1;
    char current_amp_ver[64];
    ota_service_t *ota_svc = &g_appota_service;
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)pdata;
    
    int js_cb_ref;
    uint16_t keepaliveSec = 0;
    
    js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF];
    keepaliveSec = iot_gateway_handle->keepaliveSec;

    res = aiot_mqtt_client_start(&iot_gateway_handle->mqtt_handle, keepaliveSec, js_cb_ref);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "mqtt client create failed");
        amp_free(iot_gateway_handle);
        return NULL;
    }

    g_iot_conn_flag = 1;
    iot_gateway_handle->res = res;

    /* device model service */
    iot_gateway_handle->subdev_handle = aiot_subdev_init();
    if (iot_gateway_handle->subdev_handle == NULL) {
        amp_debug(MOD_STR, "aiot_subdev_init failed");
        amp_free(iot_gateway_handle);
        return NULL;
    }

    /* set mqtt handle */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle , AIOT_SUBDEVOPT_MQTT_HANDLE, iot_gateway_handle->mqtt_handle);
    /* set subdev handler */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle , AIOT_SUBDEVOPT_RECV_HANDLER, aiot_app_subdev_recv_handler);
    /* 配置回调函数参数 */
    aiot_dm_setopt(iot_gateway_handle->subdev_handle, AIOT_SUBDEVOPT_USERDATA, (void *)iot_gateway_handle);

    /* return aiot_device_handle */
    amp_task_schedule_call(aiot_gateway_handle_notify, iot_gateway_handle);

    /* app ota service */
    ota_register_cb(ota_svc, OTA_CB_ID_UPGRADE, (void*)user_jsapp_ota_triger_cb);
    ota_register_cb(ota_svc, OTA_CB_ID_MODULE_UPGRADE, (void*)user_interval_upgrade_cb);
    ota_svc->module_ota = 0;
    amp_debug(MOD_STR, "[%s %d]module ota = %d",__func__,__LINE__,ota_svc->module_ota);

    memset(ota_svc->module_name, 0x00, sizeof(ota_svc->module_name));
    strcpy(ota_svc->module_name, OTA_MODE_NAME);

    ota_svc->mqtt_client = iot_gateway_handle->mqtt_handle;
    res = ota_service_init(ota_svc);
    if(res >= 0) {
        amp_debug(MOD_STR, "user ota init ok!");
    }
    else {
        amp_error(MOD_STR, "user ota init failed!");
    }

    //report app js version
    res = ota_transport_inform(ota_svc, NULL, amp_jsapp_version_get());
    if(res < 0) {
        amp_error(MOD_STR, "user ota report ver failed!");
    }

    memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
    amp_app_version_get(current_amp_ver);
    //report amp app version
    res = ota_transport_inform(ota_svc, OTA_MODE_NAME, current_amp_ver);
    if(res < 0) {
        amp_error(MOD_STR, "system ota report ver failed!");
    }

    while(!g_iot_close_flag){
        amp_msleep(1000);
    }

    duk_context *ctx = be_get_context();
    be_unref(ctx, iot_gateway_handle->js_cb_ref);

    aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    ota_service_deinit(ota_svc);

    amp_free(iot_gateway_handle);
    g_iot_conn_flag = 0;
    amp_semaphore_post(g_iot_close_sem);
    amp_thread_delete(NULL);
    return NULL;
}

/*************************************************************************************
 * Function:    native_aiot_create_device
 * Description: js native addon for UDP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static duk_ret_t native_aiot_create_gateway(duk_context *ctx)
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
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    amp_os_thread_param_t iot_device_task_params = {0};
    ota_service_t *ota_svc = &g_appota_service;

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

    amp_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
    amp_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
    amp_kv_set(AMP_CUSTOMER_DEVICESECRET, deviceSecret, IOTX_DEVICE_SECRET_LEN, 1);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle =(iot_gateway_handle_t *)amp_malloc(sizeof(iot_gateway_handle_t));
    if (!iot_gateway_handle) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF] = js_cb_ref;
    iot_gateway_handle->keepaliveSec = keepaliveSec;

    iot_device_task_params.name = "amp aiot device task";
    iot_device_task_params.priority = amp_get_default_task_priority();
    iot_device_task_params.stack_size = 1024 * 10;

    res = amp_thread_create(&iot_device_task, aiot_gateway_connect, iot_gateway_handle, &iot_device_task_params, &iot_device_task_stack_used);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        amp_free(iot_gateway_handle);
        goto out;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* addTopo */
static duk_ret_t native_aiot_addTopo(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    aiot_subdev_dev_t *subdev;
    int subdev_length = 0;
    int js_cb_ref = 0;
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer), array)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);

    subdev_length = duk_get_length(ctx, 1);
    amp_debug(MOD_STR, "sub_dev num is: %d", subdev_length);

    subdev = amp_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");
        duk_get_prop_string(ctx, -3, "deviceSecret");

        subdev[i].product_key = strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF] = js_cb_ref;

    res = aiot_subdev_send_topo_add(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_subdev_send_topo_add failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    }

out:
    if (subdev) {
        if (subdev->product_key) {
            amp_free(subdev->product_key);
        }
        if (subdev->device_name) {
            amp_free(subdev->device_name);
        }
        if (subdev->device_secret) {
            amp_free(subdev->device_secret);
        }
        amp_free(subdev);
    }
    duk_push_int(ctx, res);
    return 1;
}

/* getTopo */
static duk_ret_t native_aiot_getTopo(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    int js_cb_ref = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_GET_TOPO_REF] = js_cb_ref;

    res = aiot_subdev_send_topo_get(iot_gateway_handle->subdev_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_subdev_send_topo_get failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
        return -1;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* removeTopo */
static duk_ret_t native_aiot_removeTopo(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    aiot_subdev_dev_t *subdev;
    int subdev_length = 0;
    int js_cb_ref = 0;
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = amp_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");
        duk_get_prop_string(ctx, -3, "deviceSecret");

        subdev[i].product_key = strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF] = js_cb_ref;

    res = aiot_subdev_send_topo_delete(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_subdev_send_topo_delete failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
        return -1;
    }

out:
    if (subdev) {
        if (subdev->product_key) {
            amp_free(subdev->product_key);
        }
        if (subdev->device_name) {
            amp_free(subdev->device_name);
        }
        if (subdev->device_secret) {
            amp_free(subdev->device_secret);
        }
        amp_free(subdev);
    }
    duk_push_int(ctx, res);
    return 1;
}

/* registerSubDevice */
static duk_ret_t native_aiot_registerSubDevice(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    aiot_subdev_dev_t *subdev;
    int subdev_length = 0;
    int js_cb_ref = 0;
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = amp_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");
        duk_get_prop_string(ctx, -3, "deviceSecret");

        subdev[i].product_key = strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF] = js_cb_ref;

    res = aiot_subdev_send_sub_register(iot_gateway_handle->subdev_handle, subdev, sizeof(subdev)/sizeof(aiot_subdev_dev_t));
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_subdev_send_sub_register failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
        return -1;
    }

out:
    if (subdev) {
        if (subdev->product_key) {
            amp_free(subdev->product_key);
        }
        if (subdev->device_name) {
            amp_free(subdev->device_name);
        }
        if (subdev->device_secret) {
            amp_free(subdev->device_secret);
        }
        amp_free(subdev);
    }
    duk_push_int(ctx, res);
    return 1;
}

/* login */
static duk_ret_t native_aiot_login(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    aiot_subdev_dev_t *subdev;
    int subdev_length = 0;
    int js_cb_ref = 0;
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer), array)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = amp_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");
        duk_get_prop_string(ctx, -3, "deviceSecret");

        subdev[i].product_key = strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_LOGIN_REF] = js_cb_ref;

    res = aiot_subdev_send_batch_login(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_subdev_send_batch_login failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
        return -1;
    }

out:
    if (subdev) {
        if (subdev->product_key) {
            amp_free(subdev->product_key);
        }
        if (subdev->device_name) {
            amp_free(subdev->device_name);
        }
        if (subdev->device_secret) {
            amp_free(subdev->device_secret);
        }
        amp_free(subdev);
    }
    duk_push_int(ctx, res);
    return 1;
}

/* logout */
static duk_ret_t native_aiot_logout(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    aiot_subdev_dev_t *subdev;
    int subdev_length = 0;
    int js_cb_ref = 0;
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer), array)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = amp_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");
        duk_get_prop_string(ctx, -3, "deviceSecret");

        subdev[i].product_key = strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    js_cb_ref = be_ref(ctx);

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_LOGOUT_REF] = js_cb_ref;

    res = aiot_subdev_send_batch_logout(iot_gateway_handle->subdev_handle, subdev, sizeof(subdev)/sizeof(aiot_subdev_dev_t));
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "aiot_subdev_send_batch_login failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
        return -1;
    }

out:
    if (subdev) {
        if (subdev->product_key) {
            amp_free(subdev->product_key);
        }
        if (subdev->device_name) {
            amp_free(subdev->device_name);
        }
        if (subdev->device_secret) {
            amp_free(subdev->device_secret);
        }
        amp_free(subdev);
    }
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
static duk_ret_t native_aiot_gateway_close(duk_context *ctx)
{
    int res     = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be pointer");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    if (iot_gateway_handle == NULL) {
        amp_warn(MOD_STR, "iot_gateway_handle is null");
        goto out;
    }

    g_iot_close_flag = 1;
    amp_semaphore_wait(g_iot_close_sem, 200 + 50);
    res = aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    if (res < 0) {
        amp_debug(MOD_STR, "aiot stop failed");
        goto out;
    }

    g_iot_close_flag = 0;
    duk_push_int(ctx, 0);
    amp_free(iot_gateway_handle);
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
        amp_semaphore_wait(g_iot_close_sem, 5100);
        g_iot_close_flag = 0;
        amp_msleep(10);
    }

    g_iot_clean_flag = 1;
}

void module_aiot_gateway_register(void)
{
    duk_context *ctx = be_get_context();

    if (!g_iot_close_sem) {
        g_iot_close_sem = amp_semaphore_create();
        if (!g_iot_close_sem) {
            amp_error(MOD_STR, "create iot sem fail");
            return;
        }
    }
    g_iot_clean_flag = 0;

    amp_module_free_register(module_iot_source_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("gateway",           native_aiot_create_gateway, 2);
    AMP_ADD_FUNCTION("addTopo",           native_aiot_addTopo, 3);
    AMP_ADD_FUNCTION("getTopo",           native_aiot_getTopo, 2);
    AMP_ADD_FUNCTION("removeTopo",        native_aiot_removeTopo, 3);
    AMP_ADD_FUNCTION("registerSubDevice", native_aiot_registerSubDevice, 3);
    AMP_ADD_FUNCTION("login",             native_aiot_login, 3);
    AMP_ADD_FUNCTION("logout",            native_aiot_logout, 3);

    duk_put_prop_string(ctx, -2, "AIOT_GATEWAY");
}
