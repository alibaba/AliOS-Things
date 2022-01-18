/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aiot_mqtt_api.h"
#include "aiot_state_api.h"
#include "aiot_subdev_api.h"
#include "aiot_sysdep_api.h"
#ifdef AOS_COMP_KV
#include "aos/kv.h"
#endif
#include "module_aiot.h"
#include "py/mperrno.h"
#include "py_defines.h"

#define MOD_STR       "AIOT_GATEWAY"

#define OTA_MODE_NAME "system"

// static ota_service_t pyamp_g_aiot_device_appota_service;
// static ota_store_module_info_t module_info[3];

const mp_obj_type_t linksdk_gateway_type;

typedef enum {
    ON_CONNECT = 0,
    ON_DISCONNECT = 1,
    ON_CLOSE = 2,
    ON_SUBCRIBE = 3,
    ON_LOGIN = 4,
    ON_LOGOUT = 5,
    ON_TOPOADD = 6,
    ON_TOPOREMOVE = 7,
    ON_TOPOGET = 8,
    ON_SUBREGISTER = 9,
    ON_CB_MAX = 10,
} lk_cb_func_t;

typedef enum {
    GATEWAY_TOPO_ADD = 0,
    GATEWAY_TOPO_DEL = 1,
    GATEWAY_SUBDEV_LOGIN = 2,
    GATEWAY_SUBDEV_LOGOUT = 3,
    GATEWAY_SUBDEV_REGISTER = 4,
} gateway_funcode_t;

typedef struct {
    iot_gateway_handle_t *iot_gateway_handle;
    lk_cb_func_t cb_type;
    int ret_code;
    char *topic;
    int topic_len;
    char *payload;
    int payload_len;
    char *params;
    int params_len;
    uint64_t msg_id;
    char *service_id;
    aiot_mqtt_option_t option;
    aiot_mqtt_event_type_t event_type;
    aiot_mqtt_recv_type_t recv_type;
} subdev_notify_param_t;

typedef struct {
    mp_obj_base_t base;
    mp_uint_t port;
    iot_gateway_handle_t *iot_gateway_handle;
} linksdk_gateway_obj_t;

static char *__amp_strdup(const char *src)
{
    char *dst;
    size_t len = 0;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src);

    dst = aos_malloc(len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

static void aiot_subdev_packet_dump(const aiot_subdev_recv_t *packet)
{
    amp_debug(MOD_STR, "%s: packet->type %d\r\n", __func__, packet->type);
    switch (packet->type) {
    case AIOT_SUBDEVRECV_TOPO_ADD_REPLY:
    case AIOT_SUBDEVRECV_TOPO_DELETE_REPLY:
    case AIOT_SUBDEVRECV_TOPO_GET_REPLY:
    case AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY:
    case AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY:
    case AIOT_SUBDEVRECV_SUB_REGISTER_REPLY:
    case AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY:
        {
            amp_debug(MOD_STR, "msgid        : %d\r\n", packet->data.generic_reply.msg_id);
            amp_debug(MOD_STR, "code         : %d\r\n", packet->data.generic_reply.code);
            amp_debug(MOD_STR, "product key  : %s\r\n", packet->data.generic_reply.product_key);
            amp_debug(MOD_STR, "device name  : %s\r\n", packet->data.generic_reply.device_name);
            amp_debug(MOD_STR, "message      : %s\r\n",
                      (packet->data.generic_reply.message == NULL) ? ("NULL") : (packet->data.generic_reply.message));
            amp_debug(MOD_STR, "data         : %s\r\n", packet->data.generic_reply.data);
        }
        break;
    case AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY:
        {
            amp_debug(MOD_STR, "msgid        : %d\r\n", packet->data.generic_notify.msg_id);
            amp_debug(MOD_STR, "product key  : %s\r\n", packet->data.generic_notify.product_key);
            amp_debug(MOD_STR, "device name  : %s\r\n", packet->data.generic_notify.device_name);
            amp_debug(MOD_STR, "params       : %s\r\n", packet->data.generic_notify.params);
        }
        break;
    default:
        break;
    }
    amp_debug(MOD_STR, "%s exit\r\n", __func__);
}

static void subcribe_cb(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)userdata;

    switch (packet->type) {
    case AIOT_MQTTRECV_PUB:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "pub, qos: %d, len: %d, topic: %s\r\n", packet->data.pub.qos, packet->data.pub.topic_len,
                  packet->data.pub.topic);
        amp_debug(MOD_STR, "pub, len: %d, payload: %s\r\n", packet->data.pub.payload_len, packet->data.pub.payload);
        mp_obj_t dict = MP_OBJ_NULL;
        dict = mp_obj_new_dict(2);
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("topic", 5),
                          mp_obj_new_str(packet->data.pub.topic, packet->data.pub.topic_len));
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("payload", 7),
                          mp_obj_new_str(packet->data.pub.payload, packet->data.pub.payload_len));
        mp_obj_t callback = iot_gateway_handle->callback[ON_SUBCRIBE];
        if (callback != MP_OBJ_NULL && mp_obj_is_fun(callback)) {
            callback_to_python(callback, dict);
        } else {
            amp_warn(MOD_STR,
                     "iot_gateway_handle->cb_on_subcribe is "
                     "not function\r\n");
        }
        break;
    default:
        break;
    }
}

static void aiot_subdev_notify(void *pdata)
{
    subdev_notify_param_t *param = (subdev_notify_param_t *)pdata;

    mp_obj_t dict = MP_OBJ_NULL;
    mp_obj_t callback = MP_OBJ_NULL;

    switch (param->cb_type) {
    case ON_LOGIN:
    case ON_LOGOUT:
    case ON_TOPOADD:
    case ON_TOPOREMOVE:
    case ON_SUBREGISTER:
        callback = param->iot_gateway_handle->callback[param->cb_type];
        if (callback != MP_OBJ_NULL && mp_obj_is_fun(callback))
            callback_to_python(callback, mp_obj_new_int(param->ret_code));
        else
            amp_error(MOD_STR, "callback function is not rigister\r\n");
        break;

    case ON_TOPOGET:
        dict = mp_obj_new_dict(2);
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("code", 4), mp_obj_new_int(param->ret_code));
        mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("message", 7),
                          mp_obj_new_str(param->params, strlen(param->params)));
        callback = param->iot_gateway_handle->callback[param->cb_type];
        if (callback != MP_OBJ_NULL && mp_obj_is_fun(callback))
            callback_to_python(callback, dict);
        else
            amp_error(MOD_STR, "callback function is not rigister.\r\n");
        aos_free(param->params);
        break;

    case ON_CONNECT:
    case ON_CLOSE:
    case ON_DISCONNECT:
        dict = mp_obj_new_dict(2);
        mp_obj_dict_store(dict, mp_obj_new_str("code", 4), mp_obj_new_int(param->ret_code));
        mp_obj_dict_store(dict, mp_obj_new_str("handle", 6), MP_OBJ_FROM_PTR(param->iot_gateway_handle));
        callback = param->iot_gateway_handle->callback[param->cb_type];
        if (callback != MP_OBJ_NULL && mp_obj_is_fun(callback)) {
            callback_to_python(callback, dict);
        } else {
            amp_error(MOD_STR, "callback function is not rigister\r\n");
        }
        break;

    default:
        break;
    }

    aos_free(param);
}

static void aiot_subdev_recv_handler(void *handle, const aiot_subdev_recv_t *packet, void *user_data)
{
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)user_data;
    subdev_notify_param_t *param;

    param = aos_malloc(sizeof(subdev_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc gateway notify param fail.\r\n");
        return;
    }

    memset(param, 0, sizeof(subdev_notify_param_t));
    aiot_subdev_packet_dump(packet);

    param->iot_gateway_handle = iot_gateway_handle;

    switch (packet->type) {
    case AIOT_SUBDEVRECV_TOPO_ADD_REPLY:
        param->cb_type = ON_TOPOADD;
        param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
        break;
    case AIOT_SUBDEVRECV_TOPO_DELETE_REPLY:
        param->cb_type = ON_TOPOREMOVE;
        param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
        break;
    case AIOT_SUBDEVRECV_TOPO_GET_REPLY:
        param->cb_type = ON_TOPOGET;
        param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
        param->params = __amp_strdup(packet->data.generic_reply.data);
        break;
    case AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY:
        param->cb_type = ON_LOGIN;
        param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
        break;
    case AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY:
        param->cb_type = ON_LOGOUT;
        param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
        break;
    case AIOT_SUBDEVRECV_SUB_REGISTER_REPLY:
        param->cb_type = ON_SUBREGISTER;
        param->ret_code = packet->data.generic_reply.code == 200 ? 0 : packet->data.generic_reply.code;
        break;
    case AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY:
        amp_debug(MOD_STR, "msgid        : %d\n", packet->data.generic_reply.msg_id);
        amp_debug(MOD_STR, "code         : %d\n", packet->data.generic_reply.code);
        amp_debug(MOD_STR, "product key  : %s\n", packet->data.generic_reply.product_key);
        amp_debug(MOD_STR, "device name  : %s\n", packet->data.generic_reply.device_name);
        amp_debug(MOD_STR, "message      : %s\n",
                  (packet->data.generic_reply.message == NULL) ? ("NULL") : (packet->data.generic_reply.message));
        amp_debug(MOD_STR, "data         : %s\n", packet->data.generic_reply.data);
        aos_free(param);
        return;
    case AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY:
        amp_debug(MOD_STR, "msgid        : %d\n", packet->data.generic_notify.msg_id);
        amp_debug(MOD_STR, "product key  : %s\n", packet->data.generic_notify.product_key);
        amp_debug(MOD_STR, "device name  : %s\n", packet->data.generic_notify.device_name);
        amp_debug(MOD_STR, "params       : %s\n", packet->data.generic_notify.params);
        aos_free(param);
        return;
    default:
        {
            amp_error(MOD_STR, "%s: unknown type %d\n", __func__, packet->type);
            aos_free(param);
            return;
        }
    }

    py_task_schedule_call(aiot_subdev_notify, param);
}

static void aiot_mqtt_message_cb(iot_mqtt_message_t *message, void *userdata)
{
    iot_mqtt_userdata_t *udata = (iot_mqtt_userdata_t *)userdata;
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)udata->handle;
    subdev_notify_param_t *param;

    if (!message || !udata)
        return;

    amp_debug(MOD_STR, "aiot_mqtt_message_cb IS CALLED\r\n");

    param = aos_malloc(sizeof(subdev_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc gateway notify param fail\r\n");
        return;
    }
    memset(param, 0, sizeof(subdev_notify_param_t));

    param->iot_gateway_handle = iot_gateway_handle;

    param->option = message->option;
    amp_debug(MOD_STR, "message->option is %d\r\n", message->option);
    if (message->option == AIOT_MQTTOPT_EVENT_HANDLER) {
        amp_debug(MOD_STR, "message->event.type is %d\r\n", message->event.type);

        switch (message->event.type) {
        case AIOT_MQTTEVT_CONNECT:
            aos_sem_signal(&param->iot_gateway_handle->g_iot_connect_sem);
            aos_free(param);
            return;
        case AIOT_MQTTEVT_DISCONNECT:
            param->cb_type = ON_DISCONNECT;
            param->ret_code = message->event.code;
            param->event_type = message->event.type;
            break;
        default:
            aos_free(param);
            return;
        }
    } else if (message->option == AIOT_MQTTOPT_RECV_HANDLER) {
        amp_debug(MOD_STR, "message->recv.type is %d\r\n", message->recv.type);
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

    py_task_schedule_call(aiot_subdev_notify, param);
}

static void aiot_gateway_connect(void *pdata)
{
    int res = -1;
    char current_amp_ver[64];
    // ota_service_t *ota_svc = &pyamp_g_aiot_device_appota_service;
    iot_gateway_handle_t *iot_gateway_handle = (iot_gateway_handle_t *)pdata;
    iot_mqtt_userdata_t *userdata;
    subdev_notify_param_t *param;

    amp_debug(MOD_STR, "start mqtt connect task.\r\n");

    uint16_t keepaliveSec = 0;

    keepaliveSec = iot_gateway_handle->keepaliveSec;

    userdata = aos_malloc(sizeof(iot_mqtt_userdata_t));
    if (!userdata) {
        amp_error(MOD_STR, "alloc mqtt userdata fail.\r\n");
        return;
    }
    userdata->callback = aiot_mqtt_message_cb;
    userdata->handle = iot_gateway_handle;

    res = pyamp_aiot_mqtt_client_start(&iot_gateway_handle->mqtt_handle, keepaliveSec, userdata);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "mqtt client create failed\r\n");
        aos_free(userdata);
        aos_free(iot_gateway_handle);
        return;
    }

    iot_gateway_handle->g_iot_conn_flag = 1;

    /* device model service */
    iot_gateway_handle->subdev_handle = aiot_subdev_init();
    if (iot_gateway_handle->subdev_handle == NULL) {
        amp_debug(MOD_STR, "aiot_subdev_init failed\r\n");
        aos_free(userdata);
        aos_free(iot_gateway_handle);
        return;
    }

    /* set mqtt handle */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle, AIOT_SUBDEVOPT_MQTT_HANDLE, iot_gateway_handle->mqtt_handle);
    /* set subdev handler */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle, AIOT_SUBDEVOPT_RECV_HANDLER, aiot_subdev_recv_handler);
    /* 配置回调函数参数 */
    aiot_subdev_setopt(iot_gateway_handle->subdev_handle, AIOT_SUBDEVOPT_USERDATA, (void *)iot_gateway_handle);

    param = aos_malloc(sizeof(subdev_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc gateway notify param fail.\r\n");
        return;
    }

    aos_sem_wait(&iot_gateway_handle->g_iot_connect_sem, AOS_WAIT_FOREVER);
    /* Make sure gateway related functions can be performed in the CONNECT callback */
    param->ret_code = 0;
    param->iot_gateway_handle = iot_gateway_handle;
    param->cb_type = ON_CONNECT;
    param->option = AIOT_MQTTOPT_EVENT_HANDLER;
    param->event_type = AIOT_MQTTEVT_CONNECT;
    py_task_schedule_call(aiot_subdev_notify, param);

    /* app gateway active info report */
    res = pyamp_amp_app_devinfo_report(iot_gateway_handle->mqtt_handle);
    if (res < STATE_SUCCESS)
        amp_debug(MOD_STR, "gateway active info report failed\r\n");
    while (!iot_gateway_handle->g_iot_close_flag)
        aos_msleep(1000);

    pyamp_aiot_mqtt_client_stop(&iot_gateway_handle->mqtt_handle);

    aos_free(userdata);
    iot_gateway_handle->g_iot_conn_flag = 0;
    aos_sem_signal(&iot_gateway_handle->g_iot_close_sem);
    aos_task_exit(0);
    return;
}

/*************************************************************************************
 * Function:    native_aiot_create_gateway
 * Description: js native addon for UDP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static mp_obj_t aiot_create_gateway(mp_obj_t self_in, mp_obj_t data)
{
    int res = -1;
    void *mqtt_handle = NULL;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    u_int32_t keepaliveSec = 0;
    aos_task_t iot_gateway_task;
    iot_gateway_handle_t *iot_gateway_handle = NULL;
    // ota_service_t *ota_svc = &pyamp_g_aiot_device_appota_service;

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_obj_new_int(res);
    }

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "%s  param1 type error,param type must be dict\r\n", __func__);
        return mp_obj_new_int(-1);
    }

    mp_obj_t index = mp_obj_new_str_via_qstr("productKey", 10);
    productKey = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    index = mp_obj_new_str_via_qstr("deviceName", 10);
    deviceName = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    index = mp_obj_new_str_via_qstr("deviceSecret", 12);
    deviceSecret = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    index = mp_obj_new_str_via_qstr("keepaliveSec", 12);
    keepaliveSec = mp_obj_get_int(mp_obj_dict_get(data, index));

    amp_info(MOD_STR, "productKey=%s deviceName=%s deviceSecret=%s keepaliveSec=%d\r\n", productKey, deviceName,
             deviceSecret, keepaliveSec);
    /*
        memset(ota_svc->pk, 0, sizeof(ota_svc->pk));
        memset(ota_svc->dn, 0, sizeof(ota_svc->dn));
        memset(ota_svc->ds, 0, sizeof(ota_svc->ds));
        memcpy(ota_svc->pk, productKey, strlen(productKey));
        memcpy(ota_svc->dn, deviceName, strlen(deviceName));
        memcpy(ota_svc->ds, deviceSecret, strlen(deviceSecret));
    */
    aos_kv_set(AMP_CUSTOMER_PRODUCTKEY, productKey, IOTX_PRODUCT_KEY_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICENAME, deviceName, IOTX_DEVICE_NAME_LEN, 1);
    aos_kv_set(AMP_CUSTOMER_DEVICESECRET, deviceSecret, IOTX_DEVICE_SECRET_LEN, 1);

    self->iot_gateway_handle->keepaliveSec = keepaliveSec;

    res = aos_task_new_ext(&iot_gateway_task, "amp aiot gateway task", aiot_gateway_connect, self->iot_gateway_handle,
                           1024 * 10, AOS_DEFAULT_APP_PRI);
    if (res != STATE_SUCCESS) {
        amp_warn(MOD_STR, "iot create task failed\n");
        aos_free(iot_gateway_handle);
        goto out;
    }

out:

    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_create_gateway, aiot_create_gateway);

/* aiot_gateway_close */
static mp_obj_t aiot_gateway_close(mp_obj_t self_in)
{
    int res = -1;

    linksdk_gateway_obj_t *self = self_in;

    if (self_in == NULL)
        return mp_obj_new_int(res);

    iot_gateway_handle_t *iot_gateway_handle = self->iot_gateway_handle;

    if (iot_gateway_handle->g_iot_conn_flag) {
        iot_gateway_handle->g_iot_close_flag = 1;
        aos_sem_wait(&iot_gateway_handle->g_iot_close_sem, 8000);
        iot_gateway_handle->g_iot_close_flag = 0;
        aos_msleep(10);
    }

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_aiot_gateway_close, aiot_gateway_close);

/* publish */
static mp_obj_t aiot_publish(mp_obj_t self_in, mp_obj_t data)
{
    int res = -1;
    char *topic;
    char *payload;
    uint16_t payload_len = 0;
    uint8_t qos = 0;

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_obj_new_int(res);
    }

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "%s  data type error,param type must be dict\r\n", __func__);
        return mp_obj_new_int(-1);
    }

    mp_obj_t index = mp_obj_new_str_via_qstr("topic", 5);
    topic = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    index = mp_obj_new_str_via_qstr("payload", 7);
    payload = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    payload_len = strlen(payload);

    index = mp_obj_new_str_via_qstr("qos", 3);
    qos = mp_obj_get_int(mp_obj_dict_get(data, index));

    amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d\r\n", topic, payload, qos);

    res = aiot_mqtt_pub(self->iot_gateway_handle->mqtt_handle, topic, payload, payload_len, qos);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt publish failed\r\n");
        goto out;
    }

out:
    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_publish, aiot_publish);

/* unsubscribe */
static mp_obj_t aiot_unsubscribe(mp_obj_t self_in, mp_obj_t data)
{
    int res = -1;
    char *topic;
    uint32_t qos = 0;

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_obj_new_int(res);
    }

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "%s  data type error,param type must be dict\r\n", __func__);
        return mp_obj_new_int(-1);
    }

    mp_obj_t index = mp_obj_new_str_via_qstr("topic", 5);
    topic = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    amp_debug(MOD_STR, "unsubscribe topic: %s\r\n", topic);

    res = aiot_mqtt_unsub(self->iot_gateway_handle->mqtt_handle, topic);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt unsubscribe failed");
        goto out;
    }

out:

    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_unsubscribe, aiot_unsubscribe);

/* subscribe */
static mp_obj_t aiot_subscribe(mp_obj_t self_in, mp_obj_t data)
{
    int res = -1;
    char *topic = NULL;
    uint32_t qos = 0;
    amp_debug(MOD_STR, "native_aiot_subscribe is called\r\n");

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_obj_new_int(res);
    }

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        amp_error(MOD_STR, "%s data type error,param type must be dict\r\n", __func__);
        return mp_obj_new_int(-1);
    }

    mp_obj_t index = mp_obj_new_str_via_qstr("topic", 5);
    topic = mp_obj_str_get_str(mp_obj_dict_get(data, index));

    index = mp_obj_new_str_via_qstr("qos", 3);
    qos = mp_obj_get_int(mp_obj_dict_get(data, index));

    amp_debug(MOD_STR, "subscribe topic: %s, qos is: %d", topic, qos);

    res = aiot_mqtt_sub(self->iot_gateway_handle->mqtt_handle, topic, subcribe_cb, (uint8_t)qos,
                        self->iot_gateway_handle);

    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt subscribe failed\n");
        goto out;
    } else {
        amp_info(MOD_STR, "subcribe topic: %s  succeed\r\n", topic);
    }

out:
    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_subscribe, aiot_subscribe);

static mp_obj_t aiot_topo_common(mp_obj_t self_in, mp_obj_t data, gateway_funcode_t funcode)
{
    int res = -1;
    int i;
    const char *productKey;
    const char *productSecret;
    const char *deviceName;
    const char *deviceSecret;
    size_t subdev_num;
    mp_obj_t *subdev_items = NULL;
    aiot_subdev_dev_t *subdev;
    int subdev_length = 0;
    mp_obj_dict_t *dict_self;
    mp_map_elem_t *elem;

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_obj_new_int(res);
    }

    mp_obj_list_get(data, &subdev_num, &subdev_items);
    if (subdev_items == 0) {
        return mp_obj_new_int(-1);
    }

    subdev_length = subdev_num;
    subdev = aos_malloc(subdev_length * sizeof(aiot_subdev_dev_t));
    if (subdev == NULL) {
        amp_debug(MOD_STR, "sub device malloc failed\r\n");
        goto out;
    }
    memset(subdev, 0, subdev_length * sizeof(aiot_subdev_dev_t));

    for (i = 0; i < subdev_num; i++) {
        mp_obj_t index = mp_obj_new_str_via_qstr("productKey", 10);
        dict_self = MP_OBJ_TO_PTR(subdev_items[i]);
        elem = mp_map_lookup(&dict_self->map, index, MP_MAP_LOOKUP);
        if (elem != NULL) {
            productKey = mp_obj_str_get_str(mp_obj_dict_get(subdev_items[i], index));
            subdev[i].product_key = __amp_strdup(productKey);
        }

        index = mp_obj_new_str_via_qstr("deviceName", 10);
        dict_self = MP_OBJ_TO_PTR(subdev_items[i]);
        elem = mp_map_lookup(&dict_self->map, index, MP_MAP_LOOKUP);
        if (elem != NULL) {
            deviceName = mp_obj_str_get_str(mp_obj_dict_get(subdev_items[i], index));
            subdev[i].device_name = __amp_strdup(deviceName);
        }

        index = mp_obj_new_str_via_qstr("deviceSecret", 12);
        dict_self = MP_OBJ_TO_PTR(subdev_items[i]);
        elem = mp_map_lookup(&dict_self->map, index, MP_MAP_LOOKUP);
        if (elem != NULL) {
            deviceSecret = mp_obj_str_get_str(mp_obj_dict_get(subdev_items[i], index));
            subdev[i].device_secret = __amp_strdup(deviceSecret);
        }

        index = mp_obj_new_str_via_qstr("productSecret", 13);
        dict_self = MP_OBJ_TO_PTR(subdev_items[i]);
        elem = mp_map_lookup(&dict_self->map, index, MP_MAP_LOOKUP);
        if (elem != NULL) {
            productSecret = mp_obj_str_get_str(mp_obj_dict_get(subdev_items[i], index));
            subdev[i].product_secret = __amp_strdup(productSecret);
        }
    }

    switch (funcode) {
    case GATEWAY_TOPO_ADD:
        res = aiot_subdev_send_topo_add(self->iot_gateway_handle->subdev_handle, subdev, subdev_length);
        break;

    case GATEWAY_TOPO_DEL:
        res = aiot_subdev_send_topo_delete(self->iot_gateway_handle->subdev_handle, subdev, subdev_length);
        break;

    case GATEWAY_SUBDEV_LOGIN:
        res = aiot_subdev_send_batch_login(self->iot_gateway_handle->subdev_handle, subdev, subdev_length);
        break;

    case GATEWAY_SUBDEV_LOGOUT:
        res = aiot_subdev_send_batch_logout(self->iot_gateway_handle->subdev_handle, subdev, subdev_length);
        break;

    case GATEWAY_SUBDEV_REGISTER:
        res = aiot_subdev_send_sub_register(self->iot_gateway_handle->subdev_handle, subdev, subdev_length);
        break;
    }
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt publish failed ret = %d.\r\n", res);
        goto out;
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
            if (subdev[i].product_secret) {
                aos_free(subdev[i].product_secret);
            }
        }
        aos_free(subdev);
    }

    return mp_obj_new_int(res);
}

/* addTopo */
static mp_obj_t aiot_addTopo(mp_obj_t self_in, mp_obj_t data)
{
    return aiot_topo_common(self_in, data, GATEWAY_TOPO_ADD);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_add_topo, aiot_addTopo);

/* removeTopo */
static mp_obj_t aiot_removeTopo(mp_obj_t self_in, mp_obj_t data)
{
    return aiot_topo_common(self_in, data, GATEWAY_TOPO_DEL);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_remove_topo, aiot_removeTopo);

/* login */
static mp_obj_t aiot_login(mp_obj_t self_in, mp_obj_t data)
{
    return aiot_topo_common(self_in, data, GATEWAY_SUBDEV_LOGIN);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_login, aiot_login);

/* logout */
static mp_obj_t aiot_logout(mp_obj_t self_in, mp_obj_t data)
{
    return aiot_topo_common(self_in, data, GATEWAY_SUBDEV_LOGOUT);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_logout, aiot_logout);

/* registerSubDevice */
static mp_obj_t aiot_registerSubDevice(mp_obj_t self_in, mp_obj_t data)
{
    return aiot_topo_common(self_in, data, GATEWAY_SUBDEV_REGISTER);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_register_subDevice, aiot_registerSubDevice);

/* getTop */
static mp_obj_t aiot_getTop(mp_obj_t self_in)
{
    int res = -1;

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_obj_new_int(res);
    }

    res = aiot_subdev_send_topo_get(self->iot_gateway_handle->subdev_handle);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "aiot app mqtt publish failed\r\n");
        goto out;
    }

out:
    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_gateway_get_topo, aiot_getTop);

STATIC mp_obj_t aiot_register_cb(mp_obj_t self_in, mp_obj_t id, mp_obj_t func)
{
    int callback_id = mp_obj_get_int(id);

    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        return mp_obj_new_int(-1);
    }

    if (self->iot_gateway_handle->callback == NULL) {
        return mp_obj_new_int(-1);
    }

    self->iot_gateway_handle->callback[callback_id] = func;

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_3(native_gateway_register_cb, aiot_register_cb);

static mp_obj_t get_gateway_handle(mp_obj_t self_in)
{
    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        goto out;
    }
    iot_gateway_handle_t *iot_gateway_handle = self->iot_gateway_handle;
    if (!iot_gateway_handle) {
        amp_warn(MOD_STR, "parameter must be handle\r\n");
        goto out;
    } else {
        return mp_obj_new_int((mp_int_t)iot_gateway_handle);
    }
out:
    return mp_obj_new_int((mp_int_t)0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_gateway_get_gateway_handle, get_gateway_handle);

/* dynmic register */
static mp_obj_t hapy_get_ntp_time(mp_obj_t self_in, mp_obj_t cb)
{
    int res = -1;
    linksdk_gateway_obj_t *self = self_in;
    if (self_in == NULL) {
        goto out;
    }
    iot_gateway_handle_t *iot_gateway_handle = self->iot_gateway_handle;
    if (!iot_gateway_handle) {
        amp_warn(MOD_STR, "parameter must be handle\r\n");
        goto out;
    }
    res = hapy_aiot_amp_ntp_service(iot_gateway_handle->mqtt_handle, cb);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "gateway dynmic register failed\r\n");
    }
out:
    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_gateway_get_ntp_time, hapy_get_ntp_time);

STATIC const mp_rom_map_elem_t linkkit_gateway_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&native_gateway_register_cb) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&native_gateway_create_gateway) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getGatewayHandle), MP_ROM_PTR(&native_gateway_get_gateway_handle) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_subscribe), MP_ROM_PTR(&native_gateway_subscribe) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unsubscribe), MP_ROM_PTR(&native_gateway_unsubscribe) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_publish), MP_ROM_PTR(&native_gateway_publish) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getNtpTime), MP_ROM_PTR(&native_gateway_get_ntp_time) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_addTopo), MP_ROM_PTR(&native_gateway_add_topo) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getTopo), MP_ROM_PTR(&native_gateway_get_topo) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_removeTopo), MP_ROM_PTR(&native_gateway_remove_topo) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_login), MP_ROM_PTR(&native_gateway_login) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_logout), MP_ROM_PTR(&native_gateway_logout) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_registerSubDevice), MP_ROM_PTR(&native_gateway_register_subDevice) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_end), MP_ROM_PTR(&native_aiot_gateway_close) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_CONNECT), MP_ROM_INT(ON_CONNECT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_DISCONNECT), MP_ROM_INT(ON_DISCONNECT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_CLOSE), MP_ROM_INT(ON_CLOSE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_SUBCRIBE), MP_ROM_INT(ON_SUBCRIBE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_LOGIN), MP_ROM_INT(ON_LOGIN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_LOGOUT), MP_ROM_INT(ON_LOGOUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_TOPOADD), MP_ROM_INT(ON_TOPOADD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_TOPOGET), MP_ROM_INT(ON_TOPOGET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_TOPOREMOVE), MP_ROM_INT(ON_TOPOREMOVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ON_SUBREGISTER), MP_ROM_INT(ON_SUBREGISTER) },
};

STATIC MP_DEFINE_CONST_DICT(linksdk_gateway_locals_dict, linkkit_gateway_locals_dict_table);

STATIC void linksdk_gateway_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    linksdk_gateway_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "gateway");
}

STATIC mp_obj_t linksdk_gateway_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    linksdk_gateway_obj_t *gateway_obj = m_new_obj(linksdk_gateway_obj_t);
    if (!gateway_obj) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }
    memset(gateway_obj, 0, sizeof(linksdk_gateway_obj_t));

    iot_gateway_handle_t *iot_gateway_handle = m_new_obj(iot_gateway_handle_t);
    if (!iot_gateway_handle) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }
    memset(iot_gateway_handle, 0, sizeof(iot_gateway_handle_t));

    iot_gateway_handle->callback = m_new(mp_obj_t, ON_CB_MAX);

    aos_sem_new(&iot_gateway_handle->g_iot_connect_sem, 0);
    aos_sem_new(&iot_gateway_handle->g_iot_close_sem, 0);

    gateway_obj->iot_gateway_handle = iot_gateway_handle;
    gateway_obj->base.type = &linksdk_gateway_type;

    return MP_OBJ_FROM_PTR(gateway_obj);
}

const mp_obj_type_t linksdk_gateway_type = {
    { &mp_type_type },
    .name = MP_QSTR_Gateway,
    .print = linksdk_gateway_print,
    .make_new = linksdk_gateway_make_new,
    .locals_dict = (mp_obj_t)&linksdk_gateway_locals_dict,
};
