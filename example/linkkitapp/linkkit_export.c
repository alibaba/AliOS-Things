#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include "linked_list.h"
#include "lite_queue.h"
#include "linkkit_export.h"
#include "class_interface.h"

#ifdef CM_SUPPORT_MULTI_THREAD
#include "iot_import.h"
#endif /* CM_SUPPORT_MULTI_THREAD */

#include "iot_import.h"

#define EVENT_PROPERTY_POST_IDENTIFIER         "post"
#define LINKKIT_EXPORT_PRINTF                   printf

/* used for packing up all parameters from dm callback. */
typedef struct _dm_msg {
    void*                     thing_id;
    char                      property_service_version_identifier[64];
    int                       id;
    int                       code;
    char*                     response_message;
    void*                     raw_data;
    int                       raw_data_length;
    dm_callback_type_t        callback_type;
#ifdef SUBDEV_ENABLE
    void*                     sub_thing_id;
    dm_subdev_callback_type_t subcallback_type; //dm_type_subsidiary_device
    char*                     subdev_pk;
    char*                     subdev_dn;
    char*                     subdev_ds;
#endif
    dm_thing_type_t           thing_type;
} dm_msg_t;

static linkkit_ops_t* g_linkkit_ops = NULL;
static lite_queue_t* g_message_queue = NULL;
static void* user_ctx = NULL;

#ifdef SUBDEV_ENABLE
static linkkit_subdev_ops_t* g_linkkit_subdev_ops = NULL;
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

#define LINKKIT_MAX_POST_CB_NUMBER 20
typedef struct _post_cb
{
    int                    id_expected;
    void*                  cb;
    unsigned long long     time;
} post_cb_t;
void* g_list_post_cb = NULL;

static int deep_strcpy(char** _dst, const void* _src)
{
    char** dst = _dst;
    const char* src = _src;
    int size = strlen(src) + 1;

    *dst = calloc(1, size);

    if (*dst == NULL) return -1;

    strcpy(*dst, src);

    return 0;
}

/* callback function for dm. pack up all parameters and send to queue. this callback should be processed as fast as possible. */
static void dm_callback(dm_callback_type_t callback_type,
                        void* thing_id, const char* property_service_identifier,
                        int id, void* raw_data, int raw_data_length, int code, char* response_message,
                        void *param_data, int param_data_length
#ifdef SUBDEV_ENABLE
                        , char* sub_pk, char* sub_dn, char* sub_ds
#endif
                        )
{
    linkkit_ops_t* linkkit_ops = g_linkkit_ops;
    void* context = user_ctx;
    dm_msg_t* dm_msg = NULL;

    if (!linkkit_ops || ! context) return;

    if (callback_type == dm_callback_type_raw_data_arrived) {
        if (linkkit_ops->raw_data_arrived) {
            linkkit_ops->raw_data_arrived(thing_id, raw_data, raw_data_length, context);
        }
        return;
    }

    if (callback_type == dm_callback_type_linkkit_data_arrived) {
        if (linkkit_ops->linkit_data_arrived) {
            linkkit_ops->linkit_data_arrived(thing_id, param_data, param_data_length, context);
        }
        return;
    }
#ifdef SUBDEV_ENABLE
    if (callback_type == dm_callback_type_subthing_registered && (!sub_dn || !sub_pk || !sub_ds)) {
        return;
    }
#endif
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
        deep_strcpy(&dm_msg->response_message, response_message);
    }

#ifdef SUBDEV_ENABLE
    if (callback_type == dm_callback_type_subthing_registered) {
        deep_strcpy(&dm_msg->subdev_pk, sub_pk);
        deep_strcpy(&dm_msg->subdev_dn, sub_dn);
        deep_strcpy(&dm_msg->subdev_ds, sub_ds);
    }
#endif

    if (lite_queue_insert(g_message_queue, dm_msg) == -1) {
        if (dm_msg->response_message != NULL)
            free(dm_msg->response_message);
        free(dm_msg);
    }
#ifdef CM_SUPPORT_MULTI_THREAD
    HAL_SemaphorePost(sem_dispatch);
#endif /*  */
}

#ifdef SUBDEV_ENABLE
static void dm_subdev_callback(dm_subdev_callback_type_t callback_type, void* sub_thing_id, int id, int code, char* response_message,
                               char* subdev_pk_from_register, char* subdev_dn_from_register, char* subdev_ds_from_register)
{
    dm_msg_t* dm_msg = NULL;
    void* context = user_ctx;

    if (!context) return;

    dm_msg = calloc(1, sizeof(dm_msg_t));

    if (!dm_msg) return;

    dm_msg->thing_type = dm_type_subsidiary_device;
    dm_msg->subcallback_type = callback_type;
    dm_msg->sub_thing_id = sub_thing_id;
    dm_msg->id = id;
    dm_msg->code = code;

    if (response_message) {
        deep_strcpy(&dm_msg->response_message, response_message);
    }

    if (callback_type == dm_subdev_callback_type_register_success || callback_type == dm_subdev_callback_type_register_fail) {
        if (subdev_pk_from_register) deep_strcpy(&dm_msg->subdev_pk, subdev_pk_from_register);
        if (subdev_dn_from_register) deep_strcpy(&dm_msg->subdev_dn, subdev_dn_from_register);
        if (subdev_ds_from_register) deep_strcpy(&dm_msg->subdev_ds, subdev_ds_from_register);
    }

    if (lite_queue_insert(g_message_queue, dm_msg) == -1) {
        if (dm_msg->response_message != NULL)
            free(dm_msg->response_message);
        free(dm_msg);
    }
}
#endif

static void find_post_cb_by_id(void* _post_cb, va_list* params)
{
    post_cb_t* post_cb = _post_cb;
    post_cb_t** post_cb_ret;
    int id_expected;

    id_expected = va_arg(*params, int);
    post_cb_ret = va_arg(*params, post_cb_t**);

    if (post_cb->id_expected == id_expected) {
        *post_cb_ret = post_cb;
    }
}

static void handle_masterdev_request(dm_msg_t* dm_msg, void* ctx)
{
    linkkit_ops_t* linkkit_ops = g_linkkit_ops;
    void* context = ctx;
#ifdef RRPC_ENABLED
    int rrpc = 0;
#endif /* RRPC_ENABLED */

#ifdef SUBDEV_ENABLE
    int sub_thing = 0;
#endif

    linked_list_t* list = NULL;
    post_cb_t* post_cb = NULL;
    handle_post_cb_fp_t handle_post_cb_fp;

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
        if (linkkit_ops->on_disconnect) linkkit_ops->on_disconnect(context, dm_callback_type_cloud_disconnected == dm_msg->callback_type ? 1 : 0);
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
#ifdef SUBDEV_ENABLE
    case dm_callback_type_subthing_registered:
        if (linkkit_ops->sub_thing_registered) linkkit_ops->sub_thing_registered(dm_msg->thing_id, dm_msg->subdev_pk, dm_msg->subdev_dn, dm_msg->subdev_ds, context);
        break;
    case dm_callback_type_subthing_destroyed:
        if (linkkit_ops->sub_thing_destroy) linkkit_ops->sub_thing_destroy(dm_msg->thing_id, context);
        break;
    case dm_callback_type_new_subthing_created:
        sub_thing = 1;
#endif
    case dm_callback_type_new_thing_created:
#ifdef SUBDEV_ENABLE
        if (linkkit_ops->thing_create) linkkit_ops->thing_create(dm_msg->thing_id, context, sub_thing);
#else
        if (linkkit_ops->thing_create) linkkit_ops->thing_create(dm_msg->thing_id, context);
#endif
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
    case dm_callback_type_linkkit_data_arrived:
        /* handled in dm_callback() */
        break;
    case dm_callback_type_thing_enabled:
        if (linkkit_ops->thing_enable) linkkit_ops->thing_enable(dm_msg->thing_id, context);
        break;
    case dm_callback_type_thing_disabled:
        if (linkkit_ops->thing_disable) linkkit_ops->thing_disable(dm_msg->thing_id, context);
        break;
    case dm_callback_type_response_arrived:
        list = g_list_post_cb;
        if (list) {
            linked_list_iterator(list, find_post_cb_by_id, dm_msg->id, &post_cb);
            if (post_cb && post_cb->cb) {
                handle_post_cb_fp = post_cb->cb;
                handle_post_cb_fp(dm_msg->thing_id, dm_msg->id, dm_msg->code, dm_msg->response_message, context);

                linked_list_remove(list, post_cb);
                free(post_cb);
            }
        }
        break;
#ifdef LOCAL_CONN_ENABLE
    case dm_callback_type_local_post:
        linkkit_post_property(dm_msg->thing_id, NULL, NULL);
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
    int success = 0;

    if (!linkkit_subdev_ops || !context) return;

    switch (msg->subcallback_type) {
    case dm_subdev_callback_type_topo_add_success:
        success = 1;
    case dm_subdev_callback_type_topo_add_fail:
        if (linkkit_subdev_ops->topo_add_notify) linkkit_subdev_ops->topo_add_notify(msg->sub_thing_id, msg->code, msg->response_message, success, ctx);
        break;
    case dm_subdev_callback_type_topo_delete_success:
        success = 1;
    case dm_subdev_callback_type_topo_delete_fail:
        if (linkkit_subdev_ops->topo_delete_notify) linkkit_subdev_ops->topo_delete_notify(msg->sub_thing_id, msg->code, msg->response_message, success, ctx);
        break;
    case dm_subdev_callback_type_login_success:
        success = 1;
    case dm_subdev_callback_type_login_fail:
        if (linkkit_subdev_ops->login_notify) linkkit_subdev_ops->login_notify(msg->sub_thing_id, msg->code, msg->response_message, success, ctx);
        break;
    case dm_subdev_callback_type_logout_success:
        success = 1;
    case dm_subdev_callback_type_logout_fail:
        if (linkkit_subdev_ops->logout_notify) linkkit_subdev_ops->logout_notify(msg->sub_thing_id, msg->code, msg->response_message, success, ctx);
        break;
    case dm_subdev_callback_type_register_success:
        success = 1;
    case dm_subdev_callback_type_register_fail:
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

        if (msg->thing_type == dm_type_master_device) {
            handle_masterdev_request(msg, user_ctx);
#ifdef SUBDEV_ENABLE
        } else if (msg->thing_type == dm_type_subsidiary_device){
            handle_subdev_request(msg, user_ctx);
#endif
        }

        if (msg->response_message) free(msg->response_message);
#ifdef SUBDEV_ENABLE
        if (msg->subdev_pk) free(msg->subdev_pk);
        if (msg->subdev_dn) free(msg->subdev_dn);
        if (msg->subdev_ds) free(msg->subdev_ds);
#endif
        free(msg);
    }

    return NULL;
}

/* async type */
static int linkkit_start_routine(handle_dm_callback_fp_t callback_fp, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_cloud_domain_type_t domain_type)
{
    int ret = -1;
#ifdef SUBDEV_ENABLE
    const dm_t** dm;
#endif
#ifdef CM_SUPPORT_MULTI_THREAD
    int stack_used;
#endif /* CM_SUPPORT_MULTI_THREAD */
    if (dm_object) return ret;

    if (callback_fp == NULL) return ret;

    dm_object = new_object(DM_IMPL_CLASS, "dm obj", get_tsl_from_cloud, log_level, callback_fp, domain_type);

    ret = dm_object ? 0 : -1;

#ifdef SUBDEV_ENABLE
    dm = dm_object;
    if (dm && *dm) {
        ret = (*dm)->add_subdev_callback_function(dm, dm_subdev_callback);
    }
#endif

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

#ifdef SUBDEV_ENABLE
    g_linkkit_subdev_ops = calloc(1, sizeof(linkkit_subdev_ops_t));
    if (!g_linkkit_subdev_ops) return -1;
#endif
    g_list_post_cb = linked_list_create("post cb", 0);
    if (!g_list_post_cb) return -1;

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

static void free_post_cb(void* _post_cb, va_list* params)
{
    void* post_cb = _post_cb;

    if (post_cb) free(post_cb);
}

int linkkit_end()
{

    linked_list_t* list;

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

    list = g_list_post_cb;
    if (list) {
        linked_list_iterator(list, free_post_cb);
        linked_list_destroy(list);
    }
    g_list_post_cb = NULL;
#ifdef SUVDEV_SUPPORT
    free(g_linkkit_subdev_ops);
    g_linkkit_subdev_ops = NULL;
#endif

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
    char identifier_buf[128] = {0};

    if (dm == NULL || *dm == NULL || thing_id == NULL || identifier == NULL || method_set >= linkkit_method_set_number || (value == NULL && value_str == NULL)) return -1;

    if (method_set == linkkit_method_set_property_value) {
        ret = (*dm)->set_property_value(dm, thing_id, identifier, value, value_str);
    } else if (method_set == linkkit_method_set_event_output_value) {
        ret = (*dm)->set_event_output_value(dm, thing_id, identifier, value, value_str);
    } else if (method_set == linkkit_method_set_service_output_value) {
        if (strstr(identifier, ".output") == NULL) {
            snprintf(identifier_buf, sizeof(identifier_buf), "%s%s", identifier, ".output");
            ret = (*dm)->set_service_output_value(dm, thing_id, identifier_buf, value, value_str);
        } else {
            ret = (*dm)->set_service_output_value(dm, thing_id, identifier, value, value_str);
        }
    }

    return ret;
}

int linkkit_get_value(linkkit_method_get_t method_get, const void* thing_id, const char* identifier, void* value, char** value_str)
{
    dm_t** dm = dm_object;
    int ret = -1;
    char identifier_buf[128] = {0};

    if (dm == NULL || *dm == NULL || thing_id == NULL || identifier == NULL || method_get >= linkkit_method_get_number || (value == NULL && value_str == NULL)) return -1;

    if (method_get == linkkit_method_get_property_value) {
        ret = (*dm)->get_property_value(dm, thing_id, identifier, value, value_str);
    } else if (method_get == linkkit_method_get_event_output_value) {
        ret = (*dm)->get_event_output_value(dm, thing_id, identifier, value, value_str);
    } else if (method_get == linkkit_method_get_service_output_value) {
        if (strstr(identifier, ".output") == NULL) {
            snprintf(identifier_buf, sizeof(identifier_buf), "%s%s", identifier, ".output");
            ret = (*dm)->get_service_output_value(dm, thing_id, identifier_buf, value, value_str);
        } else {
            ret = (*dm)->get_service_output_value(dm, thing_id, identifier, value, value_str);
        }
    } else if (method_get == linkkit_method_get_service_input_value) {
        if (strstr(identifier, ".input") == NULL) {
            snprintf(identifier_buf, sizeof(identifier_buf), "%s%s", identifier, ".input");
            ret = (*dm)->get_service_input_value(dm, thing_id, identifier_buf, value, value_str);
        } else {
            ret = (*dm)->get_service_input_value(dm, thing_id, identifier, value, value_str);
        }
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

static void find_oldest_post_cb(void* _post_cb, va_list* params)
{
    post_cb_t* post_cb = _post_cb;
    post_cb_t** post_cb_oldest;
    unsigned long long current_time_ms;
    unsigned long long time_duration_ms;
    unsigned long long time_duration_ms_oldest;

    post_cb_oldest = va_arg(*params, post_cb_t**);
    current_time_ms = va_arg(*params, int);

    if (current_time_ms == post_cb->time) {
        time_duration_ms = 0;
    } else {
        time_duration_ms = current_time_ms > post_cb->time ? current_time_ms - post_cb->time : current_time_ms + __LONG_LONG_MAX__ * 2ULL + 1ULL - post_cb->time;
    }

    if (*post_cb_oldest == NULL) {
        *post_cb_oldest = post_cb;
    }

    time_duration_ms_oldest = current_time_ms > (*post_cb_oldest)->time ? current_time_ms - (*post_cb_oldest)->time : current_time_ms + __LONG_LONG_MAX__ * 2ULL + 1ULL - (*post_cb_oldest)->time;

    if (time_duration_ms > time_duration_ms_oldest) {
        *post_cb_oldest = post_cb;
    }
}

static void insert_post_cb(int id_send, linked_list_t* list, void* cb)
{
    int list_size;
    post_cb_t* post_cb;
    post_cb_t* post_cb_oldest = NULL;
    unsigned long long current_time_ms;
    post_cb = calloc(1, sizeof(post_cb_t));
    if (!post_cb) return;

    current_time_ms = HAL_UptimeMs();
    post_cb->id_expected = id_send;
    post_cb->cb = cb;
    post_cb->time = current_time_ms;

    linked_list_insert(list, post_cb);

    list_size = linked_list_get_size(list);
    if (list_size > LINKKIT_MAX_POST_CB_NUMBER) {
        /* find oldest cb and remove */
        linked_list_iterator(list, find_oldest_post_cb, &post_cb_oldest, current_time_ms);
        if (post_cb_oldest) {
            linked_list_remove(list, post_cb_oldest);
            free(post_cb_oldest);
        }
    }
}

int linkkit_trigger_event(const void* thing_id, const char* event_identifier, handle_post_cb_fp_t cb)
{
    dm_t** dm = dm_object;
    int ret = 0;

    int id_send = 0;
    linked_list_t* list = g_list_post_cb;

    if (dm == NULL || *dm == NULL || (*dm)->trigger_event == NULL || thing_id == NULL || event_identifier == NULL || strcmp(event_identifier, EVENT_PROPERTY_POST_IDENTIFIER) == 0) return -1;

    ret = (*dm)->trigger_event(dm, thing_id, event_identifier, NULL);

    if (ret > 0) {
        id_send = ret;

        if (list && cb) insert_post_cb(id_send, list, cb);
    }

    return ret;
}

int linkkit_post_property(const void* thing_id, const char* property_identifier, handle_post_cb_fp_t cb)
{
    dm_t** dm = dm_object;
    int ret = 0;
    int id_send = 0;
    linked_list_t* list = g_list_post_cb;

    if (dm == NULL || *dm == NULL || (*dm)->trigger_event == NULL || thing_id == NULL) return -1;

    ret = (*dm)->trigger_event(dm, thing_id, EVENT_PROPERTY_POST_IDENTIFIER, property_identifier);

    if (ret > 0) {
        id_send = ret;

        if (list && cb) insert_post_cb(id_send, list, cb);
    }

    return ret;
}

#ifdef SUBDEV_ENABLE

void* linkkit_subdev_create(const char* product_key, const char* device_name, const char* tsl, int tsl_len)
{
    const dm_t** dm = dm_object;

    if (dm && *dm && product_key && device_name) {
        return (*dm)->generate_new_subthing(dm, product_key, device_name, tsl, tsl_len);
    }

    return NULL;
}

int linkkit_subdev_destroy(const void* sub_thing_id)
{
    const dm_t** dm = dm_object;
    int ret = -1;

    if (dm && *dm && sub_thing_id) {
        ret = (*dm)->remove_subthing(dm, sub_thing_id);
    }

    return ret;
}

int linkkit_bind_subdev(const void* subdev_product_key, const char* subdev_device_name, const char* subdev_device_secret, handle_subdev_cb_fp_t cb)
{
    const dm_t** dm = dm_object;
    linkkit_subdev_ops_t* linkkit_subdev_ops = g_linkkit_subdev_ops;
    int ret = -1;

    if (dm && *dm && subdev_product_key && subdev_device_name) {
        ret = (*dm)->bind_sub_thing(dm, subdev_product_key, subdev_device_name, subdev_device_secret);
    }

    if (cb && linkkit_subdev_ops) {
        linkkit_subdev_ops->topo_add_notify = cb;
    }

    return ret;
}

int linkkit_unbind_subdev(const void* subdev_product_key, const void* subdev_device_name, handle_subdev_cb_fp_t cb)
{
    const dm_t** dm = dm_object;
    linkkit_subdev_ops_t* linkkit_subdev_ops = g_linkkit_subdev_ops;

    int ret = -1;

    if (dm && *dm && subdev_product_key && subdev_device_name) {
        ret = (*dm)->unbind_sub_thing(dm, subdev_product_key, subdev_device_name);
    }

    if (cb && linkkit_subdev_ops) {
        linkkit_subdev_ops->topo_delete_notify = cb;
    }

    return ret;
}


int linkkit_subdev_login(const void* sub_thing_id, const char* subdev_device_secret, handle_subdev_cb_fp_t cb)
{
    const dm_t** dm = dm_object;
    linkkit_subdev_ops_t* linkkit_subdev_ops = g_linkkit_subdev_ops;
    int ret = -1;

    if (dm && *dm && sub_thing_id && subdev_device_secret) {
        ret = (*dm)->login_sub_thing(dm, sub_thing_id, subdev_device_secret);
    }

    if (cb && linkkit_subdev_ops) {
        linkkit_subdev_ops->login_notify = cb;
    }

    return ret;
}


int linkkit_subdev_logout(const void* sub_thing_id, handle_subdev_cb_fp_t cb)
{
    const dm_t** dm = dm_object;
    linkkit_subdev_ops_t* linkkit_subdev_ops = g_linkkit_subdev_ops;
    int ret = -1;

    if (dm && *dm && sub_thing_id) {
        ret = (*dm)->logout_sub_thing(dm, sub_thing_id);
    }

    if (cb && linkkit_subdev_ops) {
        linkkit_subdev_ops->logout_notify = cb;
    }

    return ret;
}

int linkkit_topo_delete_subdev(const void* sub_thing_id)
{
    const dm_t** dm = dm_object;
    int ret = -1;

    if (dm && *dm && sub_thing_id) {
        ret = (*dm)->topo_delete_subthing(dm, sub_thing_id);
    }

    return ret;
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
