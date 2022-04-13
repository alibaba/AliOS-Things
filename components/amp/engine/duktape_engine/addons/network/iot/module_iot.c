/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "dev_model_api.h"
#include "be_inl.h"

#include "infra_compat.h"

#define MOD_STR "AIOT"
#define EXAMPLE_MASTER_DEVID            (0)
#define EXAMPLE_YIELD_TIMEOUT_MS        (200)

#define IOTX_PRODUCT_KEY_LEN            (20)
#define IOTX_DEVICE_NAME_LEN            (32)
#define IOTX_DEVICE_SECRET_LEN          (64)
#define IOTX_DEVICE_ID_LEN              (64)
#define IOTX_PRODUCT_SECRET_LEN         (64)

extern int update_flag;

typedef struct {
    int ret;
    int js_cb_ref;
} iot_connect_notify_param_t;

typedef struct iot_module {
    int dev_id;
    int js_cb_ref;
} iot_module_t;

typedef struct {
    iotx_sign_mqtt_t sign_mqtt;
    int ret;
    int js_cb_ref;
} iot_sign_notify_param_t;

typedef struct {
    iotx_linkkit_dev_meta_info_t meta;
    iotx_mqtt_region_types_t region;
    int js_cb_ref;
} iot_sign_t;

static iot_module_t g_iot_init_module;
static iot_module_t g_iot_onProps_module;
static iot_module_t g_iot_service_module;
static iot_module_t g_iot_connect_module;
static iot_module_t g_iot_disconnect_module;
static unsigned char iot_close = 0;

static void aiot_sign_notify(void *pdata)
{
    iot_sign_notify_param_t *p = (iot_sign_notify_param_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->ret);

    duk_push_object(ctx);
    if(p->ret >= 0) {
        /* sign success */
        duk_push_string(ctx, p->sign_mqtt.clientid);
        duk_put_prop_string(ctx, -2, "clientid");
        duk_push_string(ctx, p->sign_mqtt.hostname);
        duk_put_prop_string(ctx, -2, "host");
        duk_push_int(ctx, p->sign_mqtt.port);
        duk_put_prop_string(ctx, -2, "port");
        duk_push_string(ctx, p->sign_mqtt.username);
        duk_put_prop_string(ctx, -2, "username");
        duk_push_string(ctx, p->sign_mqtt.password);
        duk_put_prop_string(ctx, -2, "password");
    }
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, p->js_cb_ref);
    amp_free(p);

    duk_gc(ctx, 0);
}


static void *aiot_sign(void *pdata)
{
    int ret = -1;
    iot_sign_t *iot_sign_params = (iot_sign_t *)pdata;
    iot_sign_notify_param_t *p = NULL;

    p = (iot_sign_notify_param_t *)amp_calloc(1, sizeof(iot_sign_notify_param_t));
    if(p == NULL) {
        amp_error(MOD_STR, "malloc failed");
        HAL_ThreadDelete(NULL);
        return NULL;
    }

     p->js_cb_ref = iot_sign_params->js_cb_ref;

    /* Start Sign */
    amp_debug(MOD_STR, "Sign region: %d", iot_sign_params->region);
    if (IOT_Sign_MQTT(iot_sign_params->region, &iot_sign_params->meta, &p->sign_mqtt) < 0) {
        amp_error(MOD_STR, "IOT_Sign_MQTT failed");
        p->ret = -1;
        goto sign_fail;
    }
    p->ret = 0;

sign_fail:
    amp_task_schedule_call(aiot_sign_notify, p);
    HAL_ThreadDelete(NULL);
    return NULL;
}
static duk_ret_t native_aiot_sign(duk_context *ctx)
{
    int ret = -1;
    int dev_id = 0;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    const char *domain = NULL;
    void *iot_task;
    int iot_task_stack_used;
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    iot_sign_t *iot_sign_params = NULL;
    amp_os_thread_param_t iot_task_params = {0};
    iotx_linkkit_dev_meta_info_t *master_meta_info;
    iotx_sign_mqtt_t sign_mqtt;

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be object and function");
        ret = -1;
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "deviceSecret");

    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) ||
        !duk_is_string(ctx, -1))
    {
        amp_warn(MOD_STR, 
            "Parameter 1 must be an object like {productKey: string, "
            "deviceName: string, deviceSecret: string");
        ret = -2;
        goto out;
    }

    productKey    = duk_get_string(ctx, -3);
    if (productKey == NULL) {
        amp_warn(MOD_STR, "productKey is null");
        goto out;
    }

    deviceName    = duk_get_string(ctx, -2);
    if (deviceName == NULL) {
        amp_warn(MOD_STR, "deviceName is null");
        goto out;
    }

    deviceSecret  = duk_get_string(ctx, -1);
    if (deviceSecret == NULL) {
        amp_warn(MOD_STR, "deviceSecret is null");
        goto out;
    }

    duk_pop_3(ctx);

    /* options.region (option) */
    duk_get_prop_string(ctx, 0, "region");
    if (duk_is_string(ctx, -1)) {
        domain = duk_get_string(ctx, -1);
        if(strcmp(domain, "cn-shanghai") == 0) {
            domain_type = IOTX_CLOUD_REGION_SHANGHAI; /* Shanghai */
        }
        else if(strcmp(domain, "ap-southeast-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_SINGAPORE; /* Singapore */
        }
        else if(strcmp(domain, "ap-northeast-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_JAPAN; /* Japan */
        }
        else if(strcmp(domain, "us-west-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_USA_WEST; /* America */
        }
        else if(strcmp(domain, "eu-central-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_GERMANY; /* Germany */
        }
        else {
            domain_type = IOTX_CLOUD_REGION_SHANGHAI;
        }
    }
    duk_pop(ctx);

    iot_sign_params = (iot_sign_t *)amp_calloc(1, sizeof(iot_sign_t));
    if(iot_sign_params == NULL) {
        amp_error(MOD_STR, "malloc failed");
        goto out;
    }

    memcpy(iot_sign_params->meta.product_key, productKey, strlen(productKey));
    memcpy(iot_sign_params->meta.device_name, deviceName, strlen(deviceName));
    memcpy(iot_sign_params->meta.device_secret, deviceSecret, strlen(deviceSecret));
    iot_sign_params->region = domain_type;

    amp_debug(MOD_STR, "set domain type: %d", domain_type);

    duk_dup(ctx, 1);
    iot_sign_params->js_cb_ref = be_ref(ctx);

    iot_task_params.name = "amp aiot sign";
    iot_task_params.priority = ADDON_TSK_PRIORRITY;
    iot_task_params.stack_size = 1024 * 2;

    ret = HAL_ThreadCreate(&iot_task, aiot_sign, iot_sign_params, &iot_task_params, &iot_task_stack_used);
    if (ret != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void device_connected_event_handler(void *pdata)
{
    iot_connect_notify_param_t *p = (iot_connect_notify_param_t *)pdata;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->ret);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    if(p->ret >= 0) {
        /* free js callback if connect success */
        be_unref(ctx, p->js_cb_ref);
    }

    duk_gc(ctx, 0);
}

static void *aiot_connect(void *pdata)
{
    int ret = -1;
    iot_module_t *iot_module = (iot_module_t *)pdata;
    int dev_id = iot_module->dev_id;
    int js_cb_ref = iot_module->js_cb_ref;
    iot_connect_notify_param_t p;

    /* Start Connect Aliyun Server */
    do
    {
        ret = IOT_Linkkit_Connect(dev_id);
        if (ret >= 0)
        {
            p.js_cb_ref = js_cb_ref;
            p.ret = ret;
            amp_task_schedule_call(device_connected_event_handler, &p);
            break;
        }
        p.js_cb_ref = js_cb_ref;
        p.ret = ret;
        amp_task_schedule_call(device_connected_event_handler, &p);
        amp_debug(MOD_STR, "IOT_Linkkit_Connect failed! retry after %d ms", 5000);
        HAL_SleepMs(5000);
    } while (1);

    amp_debug(MOD_STR, "IOT_Linkkit_Connect ok");
    iot_close = 0;
    while(!update_flag && !iot_close){
        IOT_Linkkit_Yield(EXAMPLE_YIELD_TIMEOUT_MS);
    }
    duk_context *ctx = be_get_context();
    be_unref(ctx, p.js_cb_ref);
    
    IOT_Linkkit_Close(dev_id);

    HAL_ThreadDelete(NULL);
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
static duk_ret_t native_aiot_create_device(duk_context *ctx)
{
    int ret = -1;
    int dev_id = 0;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    const char *domain = NULL;
    void *iot_task;
    int iot_task_stack_used;
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    static iot_module_t iot_connect_module;
    amp_os_thread_param_t iot_task_params = {0};
    iotx_linkkit_dev_meta_info_t master_meta_info;

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1))
    {
        amp_warn(MOD_STR, "parameter must be object and function");
        ret = -1;
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "productKey");
    duk_get_prop_string(ctx, 0, "deviceName");
    duk_get_prop_string(ctx, 0, "deviceSecret");

    if (!duk_is_string(ctx, -3) || !duk_is_string(ctx, -2) ||
        !duk_is_string(ctx, -1))
    {
        amp_warn(MOD_STR, 
            "Parameter 1 must be an object like {productKey: string, "
            "deviceName: string, deviceSecret: string");
        ret = -2;
        goto out;
    }

    productKey    = duk_get_string(ctx, -3);
    if (productKey == NULL) {
        amp_warn(MOD_STR, "productKey is null");
        goto out;
    }

    deviceName    = duk_get_string(ctx, -2);
    if (deviceName == NULL) {
        amp_warn(MOD_STR, "deviceName is null");
        goto out;
    }

    deviceSecret  = duk_get_string(ctx, -1);
    if (deviceSecret == NULL) {
        amp_warn(MOD_STR, "deviceSecret is null");
        goto out;
    }

    duk_pop_3(ctx);

    /* options.region (option) */
    duk_get_prop_string(ctx, 0, "region");
    if (duk_is_string(ctx, -1)) {
        domain = duk_get_string(ctx, -1);
        if(strcmp(domain, "cn-shanghai") == 0) {
            domain_type = IOTX_CLOUD_REGION_SHANGHAI; /* Shanghai */
        }
        else if(strcmp(domain, "ap-southeast-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_SINGAPORE; /* Singapore */
        }
        else if(strcmp(domain, "ap-northeast-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_JAPAN; /* Japan */
        }
        else if(strcmp(domain, "us-west-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_USA_WEST; /* America */
        }
        else if(strcmp(domain, "eu-central-1") == 0) {
            domain_type = IOTX_CLOUD_REGION_GERMANY; /* Germany */
        }
        else {
            domain_type = IOTX_CLOUD_REGION_SHANGHAI;
        }
    }
    duk_pop(ctx);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, productKey, strlen(productKey));
    memcpy(master_meta_info.device_name, deviceName, strlen(deviceName));
    memcpy(master_meta_info.device_secret, deviceSecret, strlen(deviceSecret));

    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
    amp_debug(MOD_STR, "set domain type: %d", domain_type);
    // IOT_RegisterCallback(ITE_CONNECT_SUCC, device_connected_event_handler);
    
    dev_id = IOT_Linkkit_Open(0, &master_meta_info);
    if (dev_id < 0){
        amp_error(MOD_STR, "IOT_Linkkit_Open Failed");
        return dev_id;
    }

    duk_dup(ctx, 1);
    iot_connect_module.dev_id = dev_id;
    iot_connect_module.js_cb_ref = be_ref(ctx);

    iot_task_params.name = "amp aiot task";
    iot_task_params.priority = ADDON_TSK_PRIORRITY;
    iot_task_params.stack_size = 1024 * 10;

    ret = HAL_ThreadCreate(&iot_task, aiot_connect, &iot_connect_module, &iot_task_params, &iot_task_stack_used);
    if (ret != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        goto out;
    }

    duk_push_int(ctx, dev_id);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}


/*************************************************************************************
 * Function:    native_aiot_postProps
 * Description: js native addon for
 *UDP.send(sock_id,option,buffer_array,function(ret){}) Called by:   js api
 * Input:       sock_id: interger
 *            options: is a object include options.ip and options.port
 *            buffer_array: is a array which include message to send
 *            function(ret): is the callback function which has a ret input
 *param Output:      return send msg length when send success return error
 *number when send fail
 **************************************************************************************/
static duk_ret_t native_aiot_postProps(duk_context *ctx)
{
    int ret = -1;
    int dev_id;
    const char *payload;

    if (!duk_is_number(ctx, 0) || !duk_is_string(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, string)");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "dev_id [%d] is invalid", dev_id);
        goto out;
    }

    payload = duk_get_string(ctx, 1);

    if (payload == NULL) {
        amp_warn(MOD_STR, "payload is null");
        goto out;
    }

    ret = IOT_Linkkit_Report(dev_id, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, strlen(payload));
    if (ret < 0) {
        amp_warn(MOD_STR, "post properties failed!");
        goto out;
    }

    duk_push_int(ctx, ret);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_aiot_postProps
 * Description: js native addon for
 *UDP.send(sock_id,option,buffer_array,function(ret){}) Called by:   js api
 * Input:       sock_id: interger
 *            options: is a object include options.ip and options.port
 *            buffer_array: is a array which include message to send
 *            function(ret): is the callback function which has a ret input
 *param Output:      return send msg length when send success return error
 *number when send fail
 **************************************************************************************/
static int property_set_event_handler(const int devid, const char *request, const int request_len)
{
    iot_module_t *iot_module = &g_iot_onProps_module;
    amp_debug(MOD_STR, "Property Set Received, Request: %s", request);

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, iot_module->js_cb_ref);
    duk_push_int(ctx, request_len);
    duk_push_string(ctx, request);
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
    return 0;
}

static int user_props_recv_from_cloud()
{
    IOT_RegisterCallback(ITE_PROPERTY_SET, property_set_event_handler);
    return 0;
}

static duk_ret_t native_aiot_onProps(duk_context *ctx)
{
    int ret = -1;
    int dev_id;
    int payload_len;
    char *payload;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, function)");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "dev_id [%d] is invalid", dev_id);
        goto out;
    }

    duk_dup(ctx, 1);
    g_iot_onProps_module.js_cb_ref = be_ref(ctx);
    ret = user_props_recv_from_cloud();
    if (ret < 0) {
        amp_error(MOD_STR, "uart_add_recv fail!");
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_aiot_postProps
 * Description: js native addon for
 *UDP.send(sock_id,option,buffer_array,function(ret){}) Called by:   js api
 * Input:       sock_id: interger
 *            options: is a object include options.ip and options.port
 *            buffer_array: is a array which include message to send
 *            function(ret): is the callback function which has a ret input
 *param Output:      return send msg length when send success return error
 *number when send fail
 **************************************************************************************/
static int service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    iot_module_t *iot_module = &g_iot_service_module;
    amp_debug(MOD_STR, "Service Request Received, Service ID: %s, Payload: %s", serviceid, request);


    duk_context *ctx = be_get_context();
    be_push_ref(ctx, iot_module->js_cb_ref);
    duk_push_lstring(ctx, serviceid, serviceid_len);
    duk_push_lstring(ctx, request, request_len);
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
    return 0;
}

static int user_service_recv_from_cloud()
{
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, service_request_event_handler);
    return 0;
}

static duk_ret_t native_aiot_onService(duk_context *ctx)
{
    int ret = -1;
    int dev_id;
    int payload_len;
    char *payload;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, function)");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "dev_id [%d] is invalid", dev_id);
        goto out;
    }

    duk_dup(ctx, 1);
    g_iot_service_module.js_cb_ref = be_ref(ctx);
    ret = user_service_recv_from_cloud();
    if (ret < 0) {
        amp_error(MOD_STR, "uart_add_recv fail!");
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_aiot_postEvent
 * Description: js native addon for
 *            UDP.recv(sock_id,function(length, buffer_array, src_ip,
 *src_port){}) Called by:   js api Input:       sock_id: interger
 *            function(length, buffer_array, src_ip, src_port): the callback
 *function length: the recv msg length buffer_array: the recv msg buffer src_ip:
 *the peer ip string src_port: the peer port which is a interger
 *
 * Output:      return 0 when UDP.recv call ok
 *            return error number UDP.recv call fail
 **************************************************************************************/
static duk_ret_t native_aiot_postEvent(duk_context *ctx)
{
    int ret = -1;
    int dev_id;
    char *event_id;
    char *event_payload;

    if (!duk_is_number(ctx, 0) || !duk_is_object(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, object)");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "dev_id [%d] is invalid", dev_id);
        goto out;
    }

    duk_get_prop_string(ctx, 1, "id");
    if (!duk_is_string(ctx, -1)) {
        amp_warn(MOD_STR, "event id not specify");
        duk_pop(ctx);
        goto out;
    }
    event_id = (char *)duk_get_string(ctx, -1);

    duk_get_prop_string(ctx, 1, "params");
    if (!duk_is_string(ctx, -1)) {
        amp_warn(MOD_STR, "event params not specify");
        duk_pop(ctx);
        goto out;
    }
    event_payload = (char *)duk_get_string(ctx, -1);

    ret = IOT_Linkkit_TriggerEvent(dev_id, event_id, strlen(event_id), event_payload, strlen(event_payload));
    if (ret < 0) {
        amp_warn(MOD_STR, "post event failed!");
        goto out;
    }

    duk_push_int(ctx, ret);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}


static int connect_event_handler(void)
{
    iot_module_t *iot_module = &g_iot_connect_module;
    amp_debug(MOD_STR, "Cloud Connected");

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, iot_module->js_cb_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
    return 0;
}

static duk_ret_t native_aiot_onConnect(duk_context *ctx)
{
    int ret = -1;
    int dev_id;
    int payload_len;
    char *payload;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, function)");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "dev_id [%d] is invalid", dev_id);
        goto out;
    }

    duk_dup(ctx, 1);
    g_iot_connect_module.js_cb_ref = be_ref(ctx);
    IOT_RegisterCallback(ITE_CONNECT_SUCC, connect_event_handler);
out:
    duk_push_int(ctx, ret);
    return 1;
}


static int disconnect_event_handler(void)
{
    iot_module_t *iot_module = &g_iot_disconnect_module;
    amp_debug(MOD_STR, "Cloud Disconnected");

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, iot_module->js_cb_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
    return 0;
}

static duk_ret_t native_aiot_onDisconnect(duk_context *ctx)
{
    int ret = -1;
    int dev_id;
    int payload_len;
    char *payload;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, function)");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "dev_id [%d] is invalid", dev_id);
        goto out;
    }

    duk_dup(ctx, 1);
    g_iot_disconnect_module.js_cb_ref = be_ref(ctx);
    IOT_RegisterCallback(ITE_DISCONNECTED, disconnect_event_handler);
out:
    duk_push_int(ctx, ret);
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
    int ret     = -1;
    int dev_id = 0;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number");
        goto out;
    }

    dev_id = duk_get_int(ctx, 0);
    if (dev_id < 0) {
        amp_warn(MOD_STR, "devid id[%d] is invalid", dev_id);
        goto out;
    }

    // closing iot in aiot_connect()
    iot_close = 1;
    // ret = IOT_Linkkit_Close(dev_id);
    // if (ret != 0) {
    //     amp_warn(MOD_STR, "iot client close failed: %d", ret);
    //     goto out;
    // }

    duk_push_int(ctx, 0);
    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_iot_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_aiot_create_device, 2);
    duk_put_prop_string(ctx, -2, "device");

    duk_push_c_function(ctx, native_aiot_sign, 2);
    duk_put_prop_string(ctx, -2, "sign");

    duk_push_c_function(ctx, native_aiot_postProps, 2);
    duk_put_prop_string(ctx, -2, "postProps");

    duk_push_c_function(ctx, native_aiot_onProps, 2);
    duk_put_prop_string(ctx, -2, "onProps");

    duk_push_c_function(ctx, native_aiot_postEvent, 2);
    duk_put_prop_string(ctx, -2, "postEvent");

    duk_push_c_function(ctx, native_aiot_onService, 2);
    duk_put_prop_string(ctx, -2, "onService");

    duk_push_c_function(ctx, native_aiot_onConnect, 2);
    duk_put_prop_string(ctx, -2, "onConnect");

    duk_push_c_function(ctx, native_aiot_onDisconnect, 2);
    duk_put_prop_string(ctx, -2, "onDisconnect");

    duk_push_c_function(ctx, native_aiot_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_prop_string(ctx, -2, "IOT");
}
