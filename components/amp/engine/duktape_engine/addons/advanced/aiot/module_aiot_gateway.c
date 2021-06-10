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
#include "aiot_subdev_api.h"
#include "aiot_state_api.h"
#include "be_inl.h"
#include "ota_agent.h"
#include "module_aiot.h"

#define MOD_STR "AIOT"

#define OTA_MODE_NAME "system"

typedef struct {
    iot_gateway_handle_t *handle;
    char *message;
    char *topic;
    char *payload;
    int js_cb_ref;
    int ret_code;
    int topic_len;
    int payload_len;
    aiot_mqtt_option_t option;
    aiot_subdev_jscallback_type_t cb_type;
} subdev_notify_param_t;

static char g_iot_close_flag = 0;
static char g_iot_conn_flag = 0;
static char g_iot_clean_flag = 0;
static aos_sem_t g_iot_close_sem = NULL;

static ota_store_module_info_t g_module_info[3];
ota_service_t g_appota_service;
static int g_on_message_cb_ref = 0;
extern const char *amp_jsapp_version_get(void);

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
    if ((pctx == NULL) || (ver == NULL) || (module_name == NULL)) {
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
            }
            else {
                ret = OTA_TRANSPORT_PAR_FAIL;
                amp_error(MOD_STR, "user ota trigger cb is NULL!");
            }
        }
    } else {
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

static void aiot_subdev_notify(void *pdata)
{
    subdev_notify_param_t *param = (subdev_notify_param_t *)pdata;
    iot_gateway_handle_t *handle = param->handle;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, param->js_cb_ref);

    switch (param->cb_type) {
    case AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF:
        duk_push_object(ctx);
        duk_push_int(ctx, param->ret_code);
        duk_put_prop_string(ctx, -2, "code");
        duk_push_pointer(ctx, param->handle);
        duk_put_prop_string(ctx, -2, "handle");
        if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        break;
    case AIOT_SUBDEV_JSCALLBACK_LOGIN_REF:
    case AIOT_SUBDEV_JSCALLBACK_LOGOUT_REF:
        duk_push_int(ctx, param->ret_code);
        if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        break;
    case AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF:
    case AIOT_SUBDEV_JSCALLBACK_GET_TOPO_REF:
    case AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF:
        duk_push_int(ctx, param->ret_code);
        duk_push_lstring(ctx, param->message, strlen(param->message));
        if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        aos_free(param->message);
        break;
    case AIOT_SUBDEV_JSCALLBACK_ON_MQTT_MESSAGE_REF:
        duk_push_object(ctx);
        duk_push_int(ctx, param->ret_code);
        duk_put_prop_string(ctx, -2, "code");
        if (param->option == AIOT_MQTTOPT_RECV_HANDLER) {
            duk_push_lstring(ctx, param->topic, param->topic_len);
            duk_put_prop_string(ctx, -2, "topic");
            duk_push_lstring(ctx, param->payload, param->payload_len);
            duk_put_prop_string(ctx, -2, "payload");
        }
        if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
            amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
        }
        aos_free(param->topic);
        aos_free(param->payload);
        break;
    default:
        be_unref(ctx, param->js_cb_ref);
        aos_free(param);
        return;
    }

    duk_pop(ctx);

    if (param->cb_type != AIOT_SUBDEV_JSCALLBACK_ON_MQTT_MESSAGE_REF) {
        be_unref(ctx, param->js_cb_ref);
    }

    duk_gc(ctx, 0);
    aos_free(param);
}

static void aiot_subdev_packet_dump(const aiot_subdev_recv_t *packet)
{
    printf("%s: packet->type %d\r\n", __func__, packet->type);
    switch (packet->type) {
        case AIOT_SUBDEVRECV_TOPO_ADD_REPLY:
        case AIOT_SUBDEVRECV_TOPO_DELETE_REPLY:
        case AIOT_SUBDEVRECV_TOPO_GET_REPLY:
        case AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY:
        case AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY:
        case AIOT_SUBDEVRECV_SUB_REGISTER_REPLY:
        case AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY: {
            printf("msgid        : %d\r\n", packet->data.generic_reply.msg_id);
            printf("code         : %d\r\n", packet->data.generic_reply.code);
            printf("product key  : %s\r\n", packet->data.generic_reply.product_key);
            printf("device name  : %s\r\n", packet->data.generic_reply.device_name);
            printf("message      : %s\r\n", (packet->data.generic_reply.message == NULL)?("NULL"):(packet->data.generic_reply.message));
            printf("data         : %s\r\n", packet->data.generic_reply.data);
        }
        break;
        case AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY: {
            printf("msgid        : %d\r\n", packet->data.generic_notify.msg_id);
            printf("product key  : %s\r\n", packet->data.generic_notify.product_key);
            printf("device name  : %s\r\n", packet->data.generic_notify.device_name);
            printf("params       : %s\r\n", packet->data.generic_notify.params);
        }
        break;
        default: {

        }
    }
    printf("%s exit\r\n", __func__);
}

static void aiot_subdev_recv_handler(void *handle, const aiot_subdev_recv_t *packet, void *user_data)
{
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)user_data;
    subdev_notify_param_t *param;

    param = aos_malloc(sizeof(subdev_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc gateway notify param fail");
        return;
    }
    param->handle = iot_gateway_handle;

    aiot_subdev_packet_dump(packet);

    switch (packet->type) {
        case AIOT_SUBDEVRECV_TOPO_ADD_REPLY:
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF;
            param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
            param->message = __amp_strdup(packet->data.generic_reply.data);
            param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF];
            break;
        case AIOT_SUBDEVRECV_TOPO_DELETE_REPLY:
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF;
            param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
            param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF];
            break;
        case AIOT_SUBDEVRECV_TOPO_GET_REPLY:
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_GET_TOPO_REF;
            param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
            param->message = __amp_strdup(packet->data.generic_reply.data);
            param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_GET_TOPO_REF];
            break;
        case AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY:
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_LOGIN_REF;
            param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
            param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_LOGIN_REF];
            break;
        case AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY:
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_LOGOUT_REF;
            param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
            param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_LOGOUT_REF];
            break;
        case AIOT_SUBDEVRECV_SUB_REGISTER_REPLY:
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF;
            param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
            param->message = __amp_strdup(packet->data.generic_reply.data);
            param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF];
            break;
        case AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY:
            printf("msgid        : %d\n", packet->data.generic_reply.msg_id);
            printf("code         : %d\n", packet->data.generic_reply.code);
            printf("product key  : %s\n", packet->data.generic_reply.product_key);
            printf("device name  : %s\n", packet->data.generic_reply.device_name);
            printf("message      : %s\n", (packet->data.generic_reply.message == NULL)?("NULL"):(packet->data.generic_reply.message));
            printf("data         : %s\n", packet->data.generic_reply.data);
            aos_free(param);
            return;
        case AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY:
            printf("msgid        : %d\n", packet->data.generic_notify.msg_id);
            printf("product key  : %s\n", packet->data.generic_notify.product_key);
            printf("device name  : %s\n", packet->data.generic_notify.device_name);
            printf("params       : %s\n", packet->data.generic_notify.params);
            aos_free(param);
            return;
        default: {
            amp_error(MOD_STR, "%s: unknown type %d", __func__, packet->type);
            aos_free(param);
            return;
        }
    }

    amp_task_schedule_call(aiot_subdev_notify, param);
}

static void aiot_mqtt_message_cb(iot_mqtt_message_t *message, void *userdata)
{
    iot_mqtt_userdata_t *udata = (iot_mqtt_userdata_t *)userdata;
    subdev_notify_param_t *param;

    if (!message || !udata)
        return;

    param = aos_malloc(sizeof(subdev_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc gateway notify param fail");
        return;
    }
    param->handle = (iot_gateway_handle_t *)udata->handle;
    param->option = message->option;

    if (message->option == AIOT_MQTTOPT_RECV_HANDLER) {
        switch (message->recv.type) {
        case AIOT_MQTTRECV_PUB:
            if (g_on_message_cb_ref == 0) {
                aos_free(param);
                return;
            }
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_ON_MQTT_MESSAGE_REF;
            param->js_cb_ref = g_on_message_cb_ref;
            param->ret_code = message->recv.code;
            param->topic_len = message->recv.topic_len;
            param->payload_len = message->recv.payload_len;
            param->topic = __amp_strdup(message->recv.topic);
            param->payload = __amp_strdup(message->recv.payload);
            break;
        default:
            aos_free(param);
            return;
        }
    } else if (message->option == AIOT_MQTTOPT_EVENT_HANDLER) {
        switch (message->event.type) {
        case AIOT_MQTTEVT_CONNECT:
            aos_free(param);
            return;
        case AIOT_MQTTEVT_RECONNECT:
        case AIOT_MQTTEVT_DISCONNECT:
            if (g_on_message_cb_ref == 0) {
                aos_free(param);
                return;
            }
            param->cb_type = AIOT_SUBDEV_JSCALLBACK_ON_MQTT_MESSAGE_REF;
            param->js_cb_ref = g_on_message_cb_ref;
            param->ret_code = message->event.code;
            break;
        default:
            aos_free(param);
            return;
        }
    }

    amp_task_schedule_call(aiot_subdev_notify, param);
}

#define SUBDEV_FILE_PATH   AMP_FS_ROOT_DIR"pack.bin"
#define OS_APP_PATH        AMP_FS_ROOT_DIR"os_app.bin"
#define OS_KERNEL_PATH     AMP_FS_ROOT_DIR"os_kernel.bin"
static void aiot_gateway_connect(void *pdata)
{
    int res = -1;
    char current_amp_ver[64];
    ota_service_t *ota_svc = &g_appota_service;
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)pdata;
    subdev_notify_param_t *param;
    iot_mqtt_userdata_t *userdata;

    uint16_t keepaliveSec = 0;

    keepaliveSec = iot_gateway_handle->keepaliveSec;

    param = aos_malloc(sizeof(subdev_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc gateway notify param fail");
        return;
    }
    param->cb_type = AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF;
    param->js_cb_ref = iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF];
    param->handle = iot_gateway_handle;

    userdata = aos_malloc(sizeof(iot_mqtt_userdata_t));
    if (!userdata) {
        amp_error(MOD_STR, "alloc mqtt userdata fail");
        aos_free(param);
        return;
    }
    userdata->callback = aiot_mqtt_message_cb;
    userdata->handle = iot_gateway_handle;

    res = aiot_mqtt_client_start(&iot_gateway_handle->mqtt_handle, keepaliveSec, userdata);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "mqtt client create failed");
        aos_free(userdata);
        aos_free(param);
        aos_free(iot_gateway_handle);
        return;
    }

    g_iot_conn_flag = 1;
    param->ret_code = res;

    /* gateway model service */
    iot_gateway_handle->subdev_handle = aiot_subdev_init();
    if (iot_gateway_handle->subdev_handle == NULL) {
        amp_debug(MOD_STR, "aiot_subdev_init failed");
        aos_free(param);
        aos_free(iot_gateway_handle);
        return;
    }
    /* set mqtt handle */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle , AIOT_SUBDEVOPT_MQTT_HANDLE, iot_gateway_handle->mqtt_handle);
    /* set subdev handler */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle , AIOT_SUBDEVOPT_RECV_HANDLER, aiot_subdev_recv_handler);
    /* 配置回调函数参数 */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle, AIOT_SUBDEVOPT_USERDATA, (void *)iot_gateway_handle);

    /* return aiot_device_handle */
    amp_task_schedule_call(aiot_subdev_notify, param);

    /* app device active info report */
    res = amp_app_devinfo_report(iot_gateway_handle->mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "device active info report failed");
    }

    /* app ota service */
    ota_service_param_reset(ota_svc);
    memset(g_module_info, 0x00, sizeof(g_module_info));
    ota_register_module_store(ota_svc, g_module_info, 3);

    ota_register_trigger_msg_cb(ota_svc, (void *)user_jsapp_ota_triger_cb, NULL);
    amp_debug(MOD_STR, "[%s %d]module",__func__,__LINE__);

    ota_set_module_information(ota_svc, OTA_MODE_NAME, OS_APP_PATH, OTA_UPGRADE_ALL);
    ota_set_module_information(ota_svc, "default", SUBDEV_FILE_PATH, OTA_UPGRADE_ALL);
    ota_svc->mqtt_client = iot_gateway_handle->mqtt_handle;
    res = ota_service_init(ota_svc);
    if (res >= 0) {
        amp_debug(MOD_STR, "user ota init ok!");
    }
    else {
        amp_error(MOD_STR, "user ota init failed!");
    }

    //report app js version
    res = ota_report_module_version(ota_svc, "default", amp_jsapp_version_get());
    if(res < 0) {
        amp_error(MOD_STR, "user ota report ver failed!");
    }

    memset(current_amp_ver, 0x00, sizeof(current_amp_ver));
    amp_app_version_get(current_amp_ver);
    //report amp app version
    res = ota_report_module_version(ota_svc, OTA_MODE_NAME, current_amp_ver);
    if (res < 0) {
        amp_error(MOD_STR, "system ota report ver failed!");
    }

    while (!g_iot_close_flag) {
        aos_msleep(1000);
    }

    duk_context *ctx = be_get_context();

    aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);

    aos_free(userdata);
    aos_free(iot_gateway_handle);
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
static duk_ret_t native_aiot_create_gateway(duk_context *ctx)
{
    int res = -1;
    void *mqtt_handle = NULL;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    int keepaliveSec = 0;
    aos_task_t iot_device_task;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    ota_service_t *ota_svc = &g_appota_service;

    /* check paramters */
    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
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
        !duk_is_string(ctx, -2) || !duk_is_number(ctx, -1)) {
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

    iot_gateway_handle =(iot_gateway_handle_t *)aos_malloc(sizeof(iot_gateway_handle_t));
    if (!iot_gateway_handle) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF] = be_ref(ctx);
    iot_gateway_handle->keepaliveSec = keepaliveSec;

    res = aos_task_new_ext(&iot_device_task, "amp aiot device task", aiot_gateway_connect, iot_gateway_handle, 1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != 0) {
        amp_warn(MOD_STR, "iot create task failed");
        aos_free(iot_gateway_handle);
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
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer), array)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);

    subdev_length = duk_get_length(ctx, 1);
    amp_debug(MOD_STR, "sub_dev num is: %d", subdev_length);

    subdev = aos_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }
    memset(subdev, 0, subdev_length * sizeof(aiot_subdev_dev_t));

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");
        duk_get_prop_string(ctx, -3, "deviceSecret");

        subdev[i].product_key = __amp_strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = __amp_strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = __amp_strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF] = be_ref(ctx);

    res = aiot_subdev_send_topo_add(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "send subdev topo add failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    }

out:
    if (subdev) {
        for (i = 0; i < subdev_length; i++) {
            if (subdev[i].product_key) {
                aos_free(subdev[i].product_key);
            }
            if (subdev[i].device_name) {
                aos_free(subdev[i].device_name);
            }
            if (subdev[i].device_secret) {
                aos_free(subdev[i].device_secret);
            }
        }
        aos_free(subdev);
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
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_GET_TOPO_REF] = be_ref(ctx);

    res = aiot_subdev_send_topo_get(iot_gateway_handle->subdev_handle);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "send subdev topo get failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
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
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = aos_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
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

        subdev[i].product_key = __amp_strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = __amp_strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = __amp_strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF] = be_ref(ctx);

    res = aiot_subdev_send_topo_delete(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "send subdev topo remove failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    }

out:
    if (subdev) {
        for (i = 0; i < subdev_length; i++) {
            if (subdev[i].product_key) {
                aos_free(subdev[i].product_key);
            }
            if (subdev[i].device_name) {
                aos_free(subdev[i].device_name);
            }
            if (subdev[i].device_secret) {
                aos_free(subdev[i].device_secret);
            }
        }
        aos_free(subdev);
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
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = aos_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub dev malloc failed");
        goto out;
    }

    for (i = 0; i < subdev_length; i++) {
        duk_get_prop_index(ctx, 1, i);
        /* get pk */
        duk_get_prop_string(ctx, -1, "productKey");
        duk_get_prop_string(ctx, -2, "deviceName");

        subdev[i].product_key = __amp_strdup(duk_get_string(ctx, -2));
        subdev[i].device_name = __amp_strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF] = be_ref(ctx);

    res = aiot_subdev_send_sub_register(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "send subdev register failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    }

out:
    if (subdev) {
        for (i = 0; i < subdev_length; i++) {
            if (subdev[i].product_key) {
                aos_free(subdev[i].product_key);
            }
            if (subdev[i].device_name) {
                aos_free(subdev[i].device_name);
            }
        }
        aos_free(subdev);
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
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer), array)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = aos_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
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

        subdev[i].product_key = __amp_strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = __amp_strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = __amp_strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_LOGIN_REF] = be_ref(ctx);

    res = aiot_subdev_send_batch_login(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "send subdev batch login failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    }

out:
    if (subdev) {
        for (i = 0; i < subdev_length; i++) {
            if (subdev[i].product_key) {
                aos_free(subdev[i].product_key);
            }
            if (subdev[i].device_name) {
                aos_free(subdev[i].device_name);
            }
            if (subdev[i].device_secret) {
                aos_free(subdev[i].device_secret);
            }
        }
        aos_free(subdev);
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
    int i;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer), array)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    subdev_length = duk_get_length(ctx, 1);

    subdev = aos_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
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

        subdev[i].product_key = __amp_strdup(duk_get_string(ctx, -3));
        subdev[i].device_name = __amp_strdup(duk_get_string(ctx, -2));
        subdev[i].device_secret = __amp_strdup(duk_get_string(ctx, -1));

        duk_pop_n(ctx, 3);
    }

    duk_dup(ctx, 2);
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_LOGOUT_REF] = be_ref(ctx);

    res = aiot_subdev_send_batch_logout(iot_gateway_handle->subdev_handle, subdev, subdev_length);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "send subdev batch logout failed, res: -0x%04X", -res);
        // aiot_subdev_deinit(&iot_gateway_handle->subdev_handle);
        // aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    }

out:
    if (subdev) {
        for (i = 0; i < subdev_length; i++) {
            if (subdev[i].product_key) {
                aos_free(subdev[i].product_key);
            }
            if (subdev[i].device_name) {
                aos_free(subdev[i].device_name);
            }
            if (subdev[i].device_secret) {
                aos_free(subdev[i].device_secret);
            }
        }
        aos_free(subdev);
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
    aos_sem_wait(&g_iot_close_sem, 200 + 50);
    res = aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);
    if (res < 0) {
        amp_debug(MOD_STR, "aiot stop failed");
        goto out;
    }

    g_iot_close_flag = 0;
    duk_push_int(ctx, 0);
    aos_free(iot_gateway_handle);
    return 1;

out:
    duk_push_int(ctx, res);
    return 1;
}

/* publish */
static duk_ret_t native_aiot_publish(duk_context *ctx)
{
    int res = -1;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    char *topic;
    char *payload;
    uint16_t payload_len = 0;
    uint8_t qos = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    if (iot_gateway_handle == NULL) {
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

    //duk_dup(ctx, 2);
    //iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_PUBLISH_REF] = be_ref(ctx);

    //amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d", topic, payload, qos);

    res = aiot_mqtt_pub(iot_gateway_handle->mqtt_handle, topic, payload, payload_len, qos);
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
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    char *topic;
    uint8_t qos = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, string)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    if (iot_gateway_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    topic = (char *)duk_get_string(ctx, 1);

    duk_dup(ctx, 2);
    iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_UNSUBSCRIBE_REF] = be_ref(ctx);

    amp_debug(MOD_STR, "unsubscribe topic: %s", topic);

    res = aiot_mqtt_unsub(iot_gateway_handle->mqtt_handle, topic);
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
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    char *topic = NULL;
    uint8_t qos = 0;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1) || !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (pointer, object, function)");
        goto out;
    }

    iot_gateway_handle = duk_get_pointer(ctx, 0);
    if (iot_gateway_handle == NULL) {
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

    //duk_dup(ctx, 2);
    //iot_gateway_handle->js_cb_ref[AIOT_SUBDEV_JSCALLBACK_SUBSCRIBE_REF] = be_ref(ctx);

    amp_debug(MOD_STR, "subscribe topic: %s, qos is: %d", topic, qos);

    res = aiot_mqtt_sub(iot_gateway_handle->mqtt_handle, topic, NULL, qos, NULL);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt subscribe failed\n");
        goto out;
    }

out:
    duk_push_int(ctx, res);
    return 1;
}

/* on mqtt event */
static duk_ret_t native_aiot_on_mqtt_event(duk_context *ctx)
{
    int res = STATE_SUCCESS;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }

    duk_dup(ctx, 0);
    g_on_message_cb_ref = be_ref(ctx);

out:
    duk_push_int(ctx, res);
    return 1;
}

static void module_iot_source_clean(void)
{
    duk_context *ctx = be_get_context();

    g_on_message_cb_ref = 0;
    if (g_iot_conn_flag) {
        g_iot_close_flag = 1;
        aos_sem_wait(&g_iot_close_sem, 5100);
        g_iot_close_flag = 0;
        aos_msleep(10);
    }

    g_iot_clean_flag = 1;
}

void module_aiot_gateway_register(void)
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

    AMP_ADD_FUNCTION("gateway",           native_aiot_create_gateway, 2);
    AMP_ADD_FUNCTION("addTopo",           native_aiot_addTopo, 3);
    AMP_ADD_FUNCTION("getTopo",           native_aiot_getTopo, 2);
    AMP_ADD_FUNCTION("removeTopo",        native_aiot_removeTopo, 3);
    AMP_ADD_FUNCTION("registerSubDevice", native_aiot_registerSubDevice, 3);
    AMP_ADD_FUNCTION("login",             native_aiot_login, 3);
    AMP_ADD_FUNCTION("logout",            native_aiot_logout, 3);
    AMP_ADD_FUNCTION("subscribe",         native_aiot_subscribe, 3);
    AMP_ADD_FUNCTION("unsubscribe",       native_aiot_unsubscribe, 3);
    AMP_ADD_FUNCTION("publish",           native_aiot_publish, 3);
    AMP_ADD_FUNCTION("onMqttMessage",     native_aiot_on_mqtt_event, 1);

    duk_put_prop_string(ctx, -2, "AIOT_GATEWAY");
}
