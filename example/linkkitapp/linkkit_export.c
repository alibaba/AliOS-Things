#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "linkkit_export.h"
#include "class_interface.h"

#ifdef CM_SUPPORT_MULTI_THREAD
#include "iot_import.h"
#endif /* CM_SUPPORT_MULTI_THREAD */

#define EVENT_PROPERTY_POST_IDENTIFIER         "post"
#define LINKKIT_EXPORT_PRINTF printf

/* used for packing up all parameters from dm callback. */
typedef struct {
    void* thing_id;
    char  property_service_version_identifier[64];
    int   id;
    int   code;
    char* response_message;
    void* raw_data;
    int   raw_data_length;
    dm_callback_type_t callback_type;
#ifdef SUBDEV_ENABLE
    void* sub_thing_id;
    char* message;
    dm_subdev_callback_type_t subcallback_type; //dm_type_subsidiary_device
#endif
    dm_thing_type_t thing_type;
} dm_msg_t;

static linkkit_ops_t* g_linkkit_ops = NULL;
static lite_queue_t* g_message_queue = NULL;
static void* user_ctx = NULL;

#ifdef SUBDEV_ENABLE
static linkkit_subdev_ops_t* g_linkkit_subdev_ops = NULL;
static void* user_subdev_ctx = NULL;
#endif

static void* dm_object  = NULL;
#ifdef SERVICE_OTA_ENABLED
static void* fota_object = NULL;
#endif /* SERVICE_OTA_ENABLED */

#ifdef CM_SUPPORT_MULTI_THREAD
    void* thread_process_dispatch;
    void* sem_dispatch;
    int   thread_process_disatch_exit = 0;
#endif /* CM_SUPPORT_MULTI_THREAD */

/* callback function for dm. pack up all parameters and send to queue. this callback should be processed as fast as possible. */
static void dm_callback(dm_callback_type_t callback_type,
                        void* thing_id, const char* property_service_identifier,
                        int id, void* raw_data, int raw_data_length, int code, char* response_message,
                        void *param_data, int param_data_length)
{
    linkkit_ops_t* linkkit_ops = g_linkkit_ops;
    void* context = user_ctx;
    dm_msg_t* dm_msg = NULL;
    int response_message_length = 0;

    if (!linkkit_ops || ! context) return;

    if (callback_type == dm_callback_type_raw_data_arrived) {
        if (linkkit_ops->raw_data_arrived) {
            linkkit_ops->raw_data_arrived(thing_id, raw_data, raw_data_length, context);
        }
        return;
    }

    if(callback_type == dm_callback_type_linkkit_data_arrived) {
        if(linkkit_ops->linkit_data_arrived) {
            linkkit_ops->linkit_data_arrived(thing_id, param_data, param_data_length, context);
        }
        return;
    }

    dm_msg = calloc(1, sizeof(dm_msg_t));
    if (!dm_msg) return;

    dm_msg->thing_type = dm_type_master_device;
    dm_msg->callback_type = callback_type;
    dm_msg->thing_id = thing_id;

    if (property_service_identifier) {
        strcpy(dm_msg->property_service_version_identifier, property_service_identifier);
    }

    dm_msg->id = id;
    dm_msg->code = code;
    dm_msg->raw_data = raw_data;
    dm_msg->raw_data_length = raw_data_length;

    if (response_message) {
        response_message_length = strlen(response_message);
        if (response_message_length) {
            dm_msg->response_message = calloc(1, response_message_length + 1);
            strcpy(dm_msg->response_message, response_message);
        }
    }

    lite_queue_insert(g_message_queue, dm_msg);
#ifdef CM_SUPPORT_MULTI_THREAD
    HAL_SemaphorePost(sem_dispatch);
#endif /*  */

    LINKKIT_EXPORT_PRINTF("\n---------------\nsubmit an item to queue @%p.\n---------------\n", dm_msg);
}

#ifdef SUBDEV_ENABLE
static void dm_subdev_callback(dm_subdev_callback_type_t callback_type,
                               void* sub_thing_id, int id, int code, char* message, void* raw_data, int raw_data_length,
                               void* param, int param_length)
{
    linkkit_subdev_ops_t* linkkit_subdev_ops = g_linkkit_subdev_ops;
    dm_msg_t* msg = NULL;
    void* context = user_subdev_ctx;

    if(!linkkit_subdev_ops ||  !context) return;

    if (callback_type == dm_subdev_callback_type_raw_data_arrived) {
        if (linkkit_subdev_ops->raw_data_arrived) {
            linkkit_subdev_ops->raw_data_arrived(sub_thing_id, raw_data, raw_data_length, context);
        }
        return;
    }
    if(callback_type == dm_subdev_callback_type_linkkit_data_arrived) {
        if(linkkit_subdev_ops->linkit_data_arrived) {
            linkkit_subdev_ops->linkit_data_arrived(sub_thing_id, param, param_length, context);
        }
        return;
    }

    if(callback_type == dm_subdev_callback_type_topo_add_fail) {
        if(linkkit_subdev_ops->topo_add_fail) {
            linkkit_subdev_ops->topo_add_fail(sub_thing_id, code, message, context);
        }
    }

    if(callback_type == dm_subdev_callback_type_login_fail) {
        if(linkkit_subdev_ops->login_fail) {
            linkkit_subdev_ops->login_fail(sub_thing_id, code, message, context);
        }
    }

    msg = calloc(1, sizeof(dm_msg_t));
    if (!msg) return;
    msg->thing_type = dm_type_subsidiary_device;
    msg->subcallback_type = callback_type;
    msg->sub_thing_id = sub_thing_id;
    msg->id = id;
    msg->code = code;

/*    msg->message = message; */
/*    msg->raw_data = raw_data; */
/*    msg->raw_data_length = raw_data_length; */

    lite_queue_insert(g_message_queue, msg);

//    LINKKIT_EXPORT_PRINTF("\n---------------\nsubmit an item to queue @%p.\n---------------\n", msg);
}
#endif

static void handle_masterdev_request(dm_msg_t* dm_msg, void* ctx)
{
    linkkit_ops_t* linkkit_ops = g_linkkit_ops;
    void* context = ctx;
#ifdef RRPC_ENABLED
    int rrpc = 0;
#endif /* RRPC_ENABLED */
    if (!linkkit_ops || !context) return;

    switch (dm_msg->callback_type) {
    case dm_callback_type_cloud_connected:
#if LOCAL_CONN_ENABLE
    case dm_callback_type_local_connected:
        if (linkkit_ops->on_connect) linkkit_ops->on_connect(context, dm_callback_type_cloud_connected == dm_msg->callback_type ? 1 : 0);
#else
        if (linkkit_ops->on_connect) linkkit_ops->on_connect(context);
#endif
        break;
    case dm_callback_type_cloud_disconnected:
#if LOCAL_CONN_ENABLE
    case dm_callback_type_local_disconnected:
        if (linkkit_ops->on_connect) linkkit_ops->on_connect(context, dm_callback_type_cloud_disconnected == dm_msg->callback_type ? 1 : 0);
#else
        if (linkkit_ops->on_disconnect) linkkit_ops->on_disconnect(context);
#endif
        break;
    case dm_callback_type_property_value_set:
        if (linkkit_ops->thing_prop_changed) {
            linkkit_ops->thing_prop_changed(dm_msg->thing_id,
                                            dm_msg->property_service_version_identifier,
                                            context);
        }
        break;
    case dm_callback_type_new_thing_created:
        if (linkkit_ops->thing_create) linkkit_ops->thing_create(dm_msg->thing_id, context);
        break;
#ifdef RRPC_ENABLED
    case dm_callback_type_rrpc_requested:
        rrpc = 1;
#endif /* RRPC_ENABLED */
    case dm_callback_type_service_requested:
        if (linkkit_ops->thing_call_service) {
            linkkit_ops->thing_call_service(dm_msg->thing_id,
                                            dm_msg->property_service_version_identifier,
                                            dm_msg->id,
#ifdef RRPC_ENABLED
                                            rrpc,
#endif /* RRPC_ENABLED */
                                            context);
        }
        break;
    case dm_callback_type_raw_data_arrived:
        /* will be handled in dm_callback() */
        break;
    case dm_callback_type_thing_enabled:
        if (linkkit_ops->thing_enable) linkkit_ops->thing_enable(dm_msg->thing_id, context);
        break;
    case dm_callback_type_thing_disabled:
        if (linkkit_ops->thing_disable) linkkit_ops->thing_disable(dm_msg->thing_id, context);
        break;
    case dm_callback_type_response_arrived:
        if (linkkit_ops->response_arrived) linkkit_ops->response_arrived(dm_msg->thing_id, dm_msg->id, dm_msg->code, dm_msg->response_message, context);
        break;
#ifdef LOCAL_CONN_ENABLE
    case dm_callback_type_local_post:
        linkkit_post_property(dm_msg->thing_id, NULL);
        break;
#endif
    default:
        break;
    }
}

#ifdef SUBDEV_ENABLE
static void handle_subdev_request(dm_msg_t* msg, void* ctx)
{
    linkkit_subdev_ops_t* linkkit_subdev_ops = g_linkkit_subdev_ops;
    void* context = ctx;

    if (!linkkit_subdev_ops || !context) return;

    switch (msg->subcallback_type) {
    case dm_subdev_callback_type_topo_add_success:
        if(linkkit_subdev_ops->topo_add_success)  linkkit_subdev_ops->topo_add_success(msg->sub_thing_id, ctx);
        break;
    case dm_subdev_callback_type_topo_add_fail:
        if(linkkit_subdev_ops->topo_add_fail) linkkit_subdev_ops->topo_add_fail(msg->sub_thing_id, msg->code, msg->message, ctx);
        break;
    case dm_subdev_callback_type_login_success:
        if(linkkit_subdev_ops->login_success) linkkit_subdev_ops->login_success(msg->sub_thing_id, ctx);
        break;
    case dm_subdev_callback_type_login_fail:
        if(linkkit_subdev_ops->login_fail) linkkit_subdev_ops->login_fail(msg->sub_thing_id, msg->code, msg->message, ctx);
        break;
    case dm_subdev_callback_type_new_thing_created:
        if(linkkit_subdev_ops->thing_create) linkkit_subdev_ops->thing_create(msg->sub_thing_id, ctx);
        break;
    case dm_subdev_callback_type_thing_disabled:
        if(linkkit_subdev_ops->thing_disable) linkkit_subdev_ops->thing_disable(msg->sub_thing_id, ctx);
        break;
    case dm_subdev_callback_type_thing_enabled:
        if(linkkit_subdev_ops->thing_enable) linkkit_subdev_ops->thing_enable(msg->sub_thing_id, ctx);
        break;
    case dm_subdev_callback_type_raw_data_arrived:
        /* will be handled in dm_sub_callback() */
        break;
    case dm_subdev_callback_type_linkkit_data_arrived:
        /* will be handled in dm_sub_callback() */
        break;
    default:
        break;
    }
}
#endif
#ifdef CM_SUPPORT_MULTI_THREAD
void* linkkit_dispatch(void* params)
#else
void* linkkit_dispatch()
#endif
{
    if (!g_message_queue || !user_ctx) {
        LINKKIT_EXPORT_PRINTF("linkkit unitialized");
        return NULL;
    }

    while (1) {
#ifdef CM_SUPPORT_MULTI_THREAD
        HAL_SemaphoreWait(sem_dispatch, PLATFORM_WAIT_INFINITE);
        if (thread_process_disatch_exit) return 0;
#endif
        dm_msg_t* msg = lite_queue_delete(g_message_queue);
        if (!msg) break;
        if(msg->thing_type == dm_type_master_device) {
            handle_masterdev_request(msg, user_ctx);
        }else {
#ifdef SUBDEV_ENABLE
            if(user_subdev_ctx) {
                handle_subdev_request(msg, user_subdev_ctx);
            }
#endif
        }
        free(msg);
    }

    return NULL;
}

/* async type */
static int linkkit_start_routine(handle_dm_callback_fp_t callback_fp, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_cloud_domain_type_t domain_type)
{
    int ret = -1;
#ifdef CM_SUPPORT_MULTI_THREAD
    int stack_used;
#endif /* CM_SUPPORT_MULTI_THREAD */
    if (dm_object) return ret;

    if (callback_fp == NULL) return ret;

    dm_object = new_object(DM_IMPL_CLASS, "dm obj", get_tsl_from_cloud, log_level, callback_fp, domain_type);

    ret = dm_object ? 0 : -1;
#ifdef CM_SUPPORT_MULTI_THREAD
    HAL_ThreadCreate(&thread_process_dispatch, linkkit_dispatch, NULL, NULL, &stack_used);
    HAL_ThreadDetach(thread_process_dispatch);
#endif /* CM_SUPPORT_MULTI_THREAD */
    return ret;
}

int linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_ops_t* ops, linkkit_cloud_domain_type_t domain_type, void* user_context)
{
    int ret = -1;

    if (!ops || !user_context || max_buffered_msg <= 0) return ret;

#ifdef CM_SUPPORT_MULTI_THREAD
    g_message_queue = lite_queue_create(max_buffered_msg, 1);
    sem_dispatch = HAL_SemaphoreCreate();
    if (!sem_dispatch) return -1;
#else
    g_message_queue = lite_queue_create(max_buffered_msg, 0);
#endif /* CM_SUPPORT_MULTI_THREAD */

    if (!g_message_queue) return ret;

    g_linkkit_ops = ops;
    user_ctx = user_context;

    ret = linkkit_start_routine(dm_callback, get_tsl_from_cloud, log_level, domain_type);

    return ret;
}

#ifdef SERVICE_OTA_ENABLED
int linkkit_ota_init(handle_service_fota_callback_fp_t callback_fp)
{
    fota_t** ota;
    int ret = -1;

    if (fota_object) return 0;

    if (callback_fp == NULL) return -1;

    fota_object = new_object(SERVICE_FOTA_CLASS, "fota obj");

    ota = fota_object;

    if (ota && *ota && (*ota)->install_callback_function) {
        (*ota)->install_callback_function(ota, callback_fp);
        ret = 0;
    }

    return ret;
}
#endif /* SERVICE_OTA_ENABLED */
int linkkit_end()
{
#ifdef CM_SUPPORT_MULTI_THREAD
    thread_process_disatch_exit = 1;
    HAL_SemaphorePost(sem_dispatch);
    HAL_SleepMs(10);
    HAL_SemaphoreDestroy(sem_dispatch);
#endif /* CM_SUPPORT_MULTI_THREAD */

    if (g_message_queue) lite_queue_destroy(g_message_queue);
#ifdef SERVICE_OTA_ENABLED
    if (fota_object) delete_object(fota_object);
#endif /* SERVICE_OTA_ENABLED */
    if (dm_object) delete_object(dm_object);

    g_message_queue = NULL;
    dm_object = NULL;
#ifdef SERVICE_OTA_ENABLED
    fota_object = NULL;
#endif /* SERVICE_OTA_ENABLED */
    return 0;
}

void* linkkit_set_tsl(const char* tsl, int tsl_len)
{
    void* thing = NULL;
    const dm_t** dm = dm_object;

    if (dm && *dm && tsl && tsl_len > 0 && *tsl == '{' && *(tsl + tsl_len - 1) == '}') {
        thing = (*dm)->generate_new_thing(dm, tsl, tsl_len);
    }

    return thing;
}

int linkkit_set_value(linkkit_method_set_t method_set, const void* thing_id, const char* identifier, const void* value, const char* value_str)
{
    dm_t** dm = dm_object;
    int ret = -1;

    if (dm == NULL || *dm == NULL || thing_id == NULL || identifier == NULL || method_set >= linkkit_method_set_number || (value == NULL && value_str == NULL)) return -1;

    if (method_set == linkkit_method_set_property_value) {
        ret = (*dm)->set_property_value(dm, thing_id, identifier, value, value_str);
    } else if (method_set == linkkit_method_set_event_output_value) {
        ret = (*dm)->set_event_output_value(dm, thing_id, identifier, value, value_str);
    } else if (method_set == linkkit_method_set_service_output_value) {
        ret = (*dm)->set_service_output_value(dm, thing_id, identifier, value, value_str);
    }

    return ret;
}

int linkkit_get_value(linkkit_method_get_t method_get, const void* thing_id, const char* identifier, void* value, char** value_str)
{
    dm_t** dm = dm_object;
    int ret = -1;

    if (dm == NULL || *dm == NULL || thing_id == NULL || identifier == NULL || method_get >= linkkit_method_get_number || (value == NULL && value_str == NULL)) return -1;

    if (method_get == linkkit_method_get_property_value) {
        ret = (*dm)->get_property_value(dm, thing_id, identifier, value, value_str);
    } else if (method_get == linkkit_method_get_event_output_value) {
        ret = (*dm)->get_event_output_value(dm, thing_id, identifier, value, value_str);
    } else if (method_get == linkkit_method_get_service_output_value) {
        ret = (*dm)->get_service_output_value(dm, thing_id, identifier, value, value_str);
    } else if (method_get == linkkit_method_get_service_input_value) {
        ret = (*dm)->get_service_input_value(dm, thing_id, identifier, value, value_str);
    }

    return ret;
}
#ifdef RRPC_ENABLED
int linkkit_answer_service(const void* thing_id, const char* service_identifier, int response_id, int code, int rrpc)
#else
int linkkit_answer_service(const void* thing_id, const char* service_identifier, int response_id, int code)
#endif /* RRPC_ENABLED */
{
    dm_t** dm = dm_object;

    if (dm == NULL || *dm == NULL || thing_id == NULL || service_identifier == NULL) return -1;

#ifdef RRPC_ENABLED
    return (*dm)->answer_service(dm, thing_id, service_identifier, response_id, code, rrpc);
#else
    return (*dm)->answer_service(dm, thing_id, service_identifier, response_id, code);
#endif /* RRPC_ENABLED */
}

int linkkit_invoke_raw_service(const void* thing_id, int is_up_raw, void* raw_data, int raw_data_length)
{
    dm_t** dm = dm_object;
    int ret;

    if (dm == NULL || *dm == NULL || thing_id == NULL || raw_data == NULL || raw_data_length == 0) return -1;

    if (is_up_raw) {
        ret = (*dm)->invoke_raw_service(dm, thing_id, raw_data, raw_data_length);
    } else {
        ret = (*dm)->answer_raw_service(dm, thing_id, raw_data, raw_data_length);
    }

    return ret;
}
#ifdef SERVICE_OTA_ENABLED
int linkkit_invoke_ota_service(void* data_buf, int data_buf_length)
{
    fota_t** ota = fota_object;
    int ret;

    if (ota == NULL || *ota == NULL || (*ota)->perform_ota_service == NULL || data_buf == NULL || data_buf_length <= 0) return -1;

    ret = (*ota)->perform_ota_service(ota, data_buf, data_buf_length);

    return ret;
}
#endif /* SERVICE_OTA_ENABLED */

#ifdef EXTENDED_INFO_ENABLED
int linkkit_trigger_extended_info_operate(const void* thing_id, const char* params, linkkit_extended_info_operate_t linkkit_extended_info_operation)
{
    dm_t** dm = dm_object;

    if (linkkit_extended_info_operation == linkkit_extended_info_operate_update) {
        if (dm == NULL || *dm == NULL || (*dm)->trigger_extended_info_update == NULL || thing_id == NULL) return -1;

        return (*dm)->trigger_extended_info_update(dm, thing_id, params);
    } else if (linkkit_extended_info_operation == linkkit_extended_info_operate_delete) {
        if (dm == NULL || *dm == NULL || (*dm)->trigger_extended_info_delete == NULL || thing_id == NULL) return -1;

        return (*dm)->trigger_extended_info_delete(dm, thing_id, params);
    }

    return -1;
}
#endif /* EXTENDED_INFO_ENABLED */

int linkkit_trigger_event(const void* thing_id, const char* event_identifier)
{
    dm_t** dm = dm_object;

    if (dm == NULL || *dm == NULL || (*dm)->trigger_event == NULL || thing_id == NULL || event_identifier == NULL || strcmp(event_identifier, EVENT_PROPERTY_POST_IDENTIFIER) == 0) return -1;

    return (*dm)->trigger_event(dm, thing_id, event_identifier, NULL);
}

int linkkit_post_property(const void* thing_id, const char* property_identifier)
{
    dm_t** dm = dm_object;

    if (dm == NULL || *dm == NULL || (*dm)->trigger_event == NULL || thing_id == NULL) return -1;

    return (*dm)->trigger_event(dm, thing_id, EVENT_PROPERTY_POST_IDENTIFIER, property_identifier);
}
#ifdef SUBDEV_ENABLE
int linkkit_subdev_init(linkkit_subdev_ops_t* subdev_ops, void* user_subdev_context)
{
    int ret = -1;
    dm_t** dm = dm_object;

    if (!subdev_ops || !user_subdev_context) return ret;

    g_linkkit_subdev_ops = subdev_ops;
    user_subdev_ctx = user_subdev_context;
    if(dm && *dm) {
        ret = (*dm)->add_subdev_callback_function(dm, dm_subdev_callback);
    }
    return ret;
}

void* linkkit_add_subdev(const void* gw_thing_id, const char* product_key, const char* device_name, const char* device_secret, const char* tsl, int tsl_len)
{
    void* thing = NULL;
    const dm_t** dm = dm_object;

    if (dm && *dm && tsl && tsl_len > 0 && *tsl == '{' && *(tsl + tsl_len - 1) == '}' && product_key && device_name && device_secret) {
        thing = (*dm)->generate_new_subthing(dm, product_key, device_name, device_secret, tsl, tsl_len);
    }

    return thing;
}
#endif

#ifndef CM_SUPPORT_MULTI_THREAD
int linkkit_yield(int timeout_ms)
{
    dm_t** dm = dm_object;

    if (dm && *dm && (*dm)->yield) {
        return (*dm)->yield(dm, timeout_ms);
    }

    return -1;
}
#endif /* CM_SUPPORT_MULTI_THREAD */
