#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "interface/cm_abstract.h"
#include "interface/log_abstract.h"
#include "dm_cm_impl.h"
#include "dm_logger.h"
#include "dm_cm_msg_info.h"

#include "dm_import.h"
#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_cm.h"

#define CM_IMPL_EXTENTED_ROOM_FOR_STRING_MALLOC 1
static int dm_cm_impl_deinit(void* _self);

static const char string_cm_abstrct_impl_class_name[] __DM_READ_ONLY__ =  "dm_cm_impl_cls";
static const char string_down_raw[] __DM_READ_ONLY__ = "down_raw";
static const char string_down_raw_reply[] __DM_READ_ONLY__ = "down_raw_reply";
static const char string_up_raw[] __DM_READ_ONLY__ = "up_raw";
static const char string_up_raw_reply[] __DM_READ_ONLY__ = "up_raw_reply";
static const char string__reply[] __DM_READ_ONLY__ = "_reply";

static void* dm_cm_impl_ctor(void* _self, va_list* params)
{
    dm_cm_impl_t* self = _self;

    self->cm_inited = 0;

    return self;
}

static void* dm_cm_impl_dtor(void* _self)
{
    dm_cm_impl_t* self = _self;

    if (self->cm_inited) dm_cm_impl_deinit(self);

    return self;
}

static int dm_cm_impl_init(void* _self, const char* _product_key, const char* _device_name, const char* _device_secret,
                            const char* _device_id, iotx_cm_event_handle_fp_t event_cb, void* pcontext, dm_cloud_domain_type_t domain_type)
{
    dm_cm_impl_t* self = _self;
    iotx_cm_init_param_t init_param;
    int ret = SUCCESS_RETURN;

    if (!_product_key || !_device_name || !_device_secret || !_device_id || !event_cb || !pcontext) return -1;

    if (self->cm_inited) return ret;

    init_param.event_func = event_cb;
    init_param.user_data = pcontext;

    init_param.domain_type = (iotx_cm_cloud_domain_types_t)domain_type;

    ret = IOT_CM_Init(&init_param, NULL);

    dm_log_debug("ret = IOT_CM_Init() = %d\n", ret);

    if (FAIL_RETURN == ret) {
        dm_printf("init fail\n");
    } else {
        self->cm_inited = 1;
    }

    return ret;
}

static int dm_cm_impl_deinit(void* _self)
{
    dm_cm_impl_t* self = _self;

    self->cm_inited = 0;

    return IOT_CM_Deinit(NULL);;
}

static int dm_cm_impl_regist(void* _self, char* uri, iotx_cm_register_fp_t register_cb, void* context)
{
    iotx_cm_register_param_t register_param;
    int ret;

    if(!uri || !register_cb || !context) {
        dm_log_err("invalid param!");
        return FAIL_RETURN;
    }

    dm_printf("%s: uri: %s\n", __FUNCTION__, uri);

    /* Subscribe the specific topic */
    register_param.URI = uri;

    if (strstr(uri, string_down_raw) || strstr(uri, string_down_raw_reply) || strstr(uri, string_up_raw) || strstr(uri, string_up_raw_reply)) {
        register_param.message_type = IOTX_CM_MESSAGE_RAW;
    } else if (strstr(uri, string__reply)) {
        register_param.message_type = IOTX_CM_MESSAGE_RESPONSE;
    } else {
        register_param.message_type = IOTX_CM_MESSAGE_REQUEST;
    }

    register_param.register_func = register_cb;
    register_param.user_data = context;
    ret = IOT_CM_Register(&register_param, NULL);

    dm_log_debug("ret = IOT_CM_Register() = %d\n", ret);

    if (FAIL_RETURN == ret) {
        dm_printf("register fail\n");
    }

    return ret;
}

static int dm_cm_impl_unregist(void* _self, char* uri)
{
    dm_cm_impl_t* self = _self;
    iotx_cm_unregister_param_t unregister_param;
    int ret = -1;

    if (!uri || !self->cm_inited) return -1;

    unregister_param.URI = uri;

    ret = IOT_CM_Unregister(&unregister_param, NULL);

    dm_log_debug("ret = IOT_CM_Unregister() = %d\n", ret);

    if (FAIL_RETURN == ret) {
        dm_printf("unregister fail\n");
    }

    return ret;
}


static int dm_cm_impl_send(void* _self, message_info_t** msg)
{
    message_info_t** message_info = msg;
    iotx_cm_message_info_t cm_message_info = {0};
    iotx_cm_send_peer_t send_peer;
    char* device_name;
    char* product_key;
    int ret;
    int message_type;

    if (!msg) return -1;

    cm_message_info.id = (*message_info)->get_id(message_info);
    cm_message_info.code = (*message_info)->get_code(message_info);
    message_type = (*message_info)->get_message_type(message_info);
    cm_message_info.message_type = message_type == DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST ? IOTX_CM_MESSAGE_REQUEST : (message_type == DM_CM_MSG_INFO_MESSAGE_TYPE_RESPONSE ? IOTX_CM_MESSAGE_RESPONSE : IOTX_CM_MESSAGE_RAW);
    cm_message_info.URI = (*message_info)->get_uri(message_info);
    cm_message_info.method = (*message_info)->get_method(message_info);
    cm_message_info.parameter = cm_message_info.message_type == IOTX_CM_MESSAGE_RAW ? (*message_info)->get_raw_data(message_info) : (*message_info)->get_params_data(message_info);
    cm_message_info.parameter_length = cm_message_info.message_type == IOTX_CM_MESSAGE_RAW ? (*message_info)->get_raw_data_length(message_info) : strlen(cm_message_info.parameter);
#ifdef LOCAL_CONN_ENABLE
    cm_message_info.conn_ctx = (*message_info)->get_conn_ctx(message_info);
#endif
    product_key = (*message_info)->get_product_key(message_info);
    device_name = (*message_info)->get_device_name(message_info);

    memset(&send_peer, 0, sizeof(iotx_cm_send_peer_t));

    if (product_key && device_name) {
        strncpy(send_peer.device_name, device_name, sizeof(send_peer.device_name));
        strncpy(send_peer.product_key, product_key, sizeof(send_peer.product_key));
    }

    ret = IOT_CM_Send(&send_peer, &cm_message_info, NULL);

    dm_log_debug("ret = IOT_CM_Send() = %d\n", ret);

    (*message_info)->clear(message_info);

    return ret;
}
#ifdef LOCAL_CONN_ENABLE
int dm_cm_impl_add_service(void* _self, char* uri, iotx_cm_register_fp_t register_cb, void* context, int auth)
{
    iotx_cm_add_service_param_t add_service_param;
    int ret;

    if(!uri || !register_cb || !context) {
        dm_log_err("invalid param!");
        return FAIL_RETURN;
    }

    dm_printf("%s: uri: %s\n", __FUNCTION__, uri);

    /* Subscribe the specific topic */
    add_service_param.URI = uri;
    add_service_param.message_type = IOTX_CM_MESSAGE_REQUEST;
    add_service_param.register_func = register_cb;
    add_service_param.user_data = context;
    add_service_param.auth_type = auth ? IOTX_CM_MESSAGE_NEED_AUTH : IOTX_CM_MESSAGE_NO_AUTH;

    ret = IOT_CM_Add_Service(&add_service_param, NULL);

    dm_log_debug("ret = IOT_CM_Add_Service() = %d\n", ret);

    if (FAIL_RETURN == ret) {
        dm_printf("add service fail\n");
    }

    return ret;
}
#endif

#ifndef CM_SUPPORT_MULTI_THREAD
static int dm_cm_impl_yield(void* _self, int timeout_ms)
{
    return IOT_CM_Yield(timeout_ms, NULL);
}
#endif /* CM_SUPPORT_MULTI_THREAD */

static const cm_abstract_t _dm_cm_impl_class = {
    sizeof(dm_cm_impl_t),
    string_cm_abstrct_impl_class_name,
    dm_cm_impl_ctor,
    dm_cm_impl_dtor,
    dm_cm_impl_init,
    dm_cm_impl_deinit,
    dm_cm_impl_regist,
    dm_cm_impl_unregist,
    dm_cm_impl_send,
#ifdef LOCAL_CONN_ENABLE
    dm_cm_impl_add_service,
#endif
#ifndef CM_SUPPORT_MULTI_THREAD
    dm_cm_impl_yield,
#endif /* CM_SUPPORT_MULTI_THREAD */
};

const void* get_dm_cm_impl_class()
{
    return &_dm_cm_impl_class;
}
