#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "linkkit_export.h"

#define LINKKIT_EXPORT_PRINTF printf

/* used for packing up all parameters from dm callback. */
typedef struct {
    void* thing_id;
    char  property_service_version_identifier[64];
    int   request_id;
    void* raw_data;
    int   raw_data_length;
    dm_callback_type_t callback_type;
} dm_msg_t;

static linkkit_ops_t* g_ops = NULL;
static lite_queue_t* g_message_queue = NULL;
static void* user_ctx = NULL;
static void* dm_object  = NULL;
#ifdef OTA_ENABLED
static void* fota_object = NULL;
#endif /* OTA_ENABLED */
static void handle_request(dm_msg_t* msg, void* ctx);
/* callback function for dm. pack up all parameters and send to queue. this callback should be processed as fast as possible. */
static void dm_callback(dm_callback_type_t callback_type,
                        void* thing_id, const char* property_service_identifier,
                        int request_id, void* raw_data, int raw_data_length)
{
    linkkit_ops_t* ops = g_ops;
    void* context = user_ctx;
    dm_msg_t* msg = NULL;

    if (!ops || ! context) return;

    if (callback_type == dm_callback_type_raw_data_arrived) {
        if (ops->raw_data_arrived) {
            ops->raw_data_arrived(thing_id, raw_data, raw_data_length, context);
        }
        return;
    }

    msg = calloc(1, sizeof(dm_msg_t));
    if (!msg) return;

    msg->callback_type = callback_type;
    msg->thing_id = thing_id;

    if (property_service_identifier) {
        strcpy(msg->property_service_version_identifier, property_service_identifier);
    }

    msg->request_id = request_id;
    msg->raw_data = raw_data;
    msg->raw_data_length = raw_data_length;

    //lite_queue_insert(g_message_queue, msg);
    handle_request(msg, user_ctx);
    free(msg);

    LINKKIT_EXPORT_PRINTF("\n---------------\nsubmit an item to queue @%p.\n---------------\n", msg);
}

static void handle_request(dm_msg_t* msg, void* ctx)
{
    linkkit_ops_t* ops = g_ops;
    void* context = ctx;

    if (!ops || !context) return;

    switch (msg->callback_type) {
    case dm_callback_type_cloud_connected:
        if (ops->on_connect) ops->on_connect(context);
        break;
    case dm_callback_type_cloud_disconnected:
        if (ops->on_disconnect) ops->on_disconnect(context);
        break;
    case dm_callback_type_property_value_set:
        if (ops->thing_prop_changed) {
            ops->thing_prop_changed(msg->thing_id,
                                    msg->property_service_version_identifier,
                                    context);
        }
        break;
    case dm_callback_type_new_thing_created:
        if (ops->thing_create) ops->thing_create(msg->thing_id, context);
        break;
    case dm_callback_type_service_requested:
        if (ops->thing_call_service) {
            ops->thing_call_service(msg->thing_id,
                                    msg->property_service_version_identifier,
                                    msg->request_id,
                                    context);
        }
        break;
    case dm_callback_type_raw_data_arrived:
        /* will be handled in dm_callback() */
        break;
    case dm_callback_type_thing_enabled:
        if (ops->thing_enable) ops->thing_enable(msg->thing_id, context);
        break;
    case dm_callback_type_thing_disabled:
        if (ops->thing_disable) ops->thing_disable(msg->thing_id, context);
        break;
    default:
        break;
    }
}

int linkkit_dispatch(void)
{
    if (!g_message_queue || !user_ctx) {
        LINKKIT_EXPORT_PRINTF("linkkit unitialized");
        return -1;
    }

    while (1) {
        dm_msg_t* msg = lite_queue_delete(g_message_queue);
        if (!msg) break;
        handle_request(msg, user_ctx);
        free(msg);
    }

    return 0;
}

/* async type */
static int linkkit_start_routine(handle_dm_callback_fp_t callback_fp, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_cloud_domain_type_t domain_type)
{
    int ret = -1;

    if (dm_object) return ret;

    if (callback_fp == NULL) return ret;

    dm_object = new_object(DM_IMPL_CLASS, "dm obj", get_tsl_from_cloud, log_level, callback_fp, domain_type);

    ret = dm_object ? 0 : -1;

    return ret;
}

int linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_ops_t* ops, linkkit_cloud_domain_type_t domain_type, void* user_context)
{
    int ret = -1;

    if (!ops || !user_context || max_buffered_msg <= 0) return ret;

    g_message_queue = lite_queue_create(max_buffered_msg, 0);
    if (!g_message_queue) return ret;

    g_ops = ops;
    user_ctx = user_context;

    ret = linkkit_start_routine(dm_callback, get_tsl_from_cloud, log_level, domain_type);

    return ret;
}
#ifdef OTA_ENABLED
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
#endif /* OTA_ENABLED */
int linkkit_end()
{
    if (g_message_queue) lite_queue_destroy(g_message_queue);
#ifdef OTA_ENABLED
    if (fota_object) delete_object(fota_object);
#endif /* OTA_ENABLED */
    if (dm_object) delete_object(dm_object);

    g_message_queue = NULL;
    dm_object = NULL;
#ifdef OTA_ENABLED
    fota_object = NULL;
#endif /* OTA_ENABLED */
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

int linkkit_answer_service(const void* thing_id, const char* service_identifier, int response_id, int code)
{
    dm_t** dm = dm_object;

    if (dm == NULL || *dm == NULL || thing_id == NULL || service_identifier == NULL) return -1;

    return (*dm)->answer_service(dm, thing_id, service_identifier, response_id, code);
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
#ifdef OTA_ENABLED
int linkkit_invoke_ota_service(void* data_buf, int data_buf_length)
{
    fota_t** ota = fota_object;
    int ret;

    if (ota == NULL || *ota == NULL || (*ota)->perform_ota_service == NULL || data_buf == NULL || data_buf_length <= 0) return -1;

    ret = (*ota)->perform_ota_service(ota, data_buf, data_buf_length);

    return ret;
}
#endif /* OTA_ENABLED */
int linkkit_trigger_event(const void* thing_id, const char* event_identifier, const char* property_identifier)
{
    dm_t** dm = dm_object;

    if (dm == NULL || *dm == NULL || (*dm)->trigger_event == NULL || thing_id == NULL || event_identifier == NULL) return -1;

    return (*dm)->trigger_event(dm, thing_id, event_identifier, property_identifier);
}

#ifndef CMP_SUPPORT_MULTI_THREAD
int linkkit_yield(int timeout_ms)
{
    dm_t** dm = dm_object;

    if (dm && *dm && (*dm)->yield) {
        return (*dm)->yield(dm, timeout_ms);
    }

    return -1;
}
#endif
