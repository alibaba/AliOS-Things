#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "class_interface.h"
#include "interface/thing_manager_abstract.h"
#include "interface/message_info_abstract.h"
#include "interface/cm_abstract.h"
#include "interface/thing_abstract.h"
#include "interface/thing_manager_abstract.h"
#include "interface/log_abstract.h"
#include "interface/list_abstract.h"

#include "dm_thing_manager.h"
#include "dm_logger.h"
#include "dm_thing.h"
#include "dm_slist.h"
#include "dm_import.h"
#include "dm_cm_msg_info.h"
#include "dm_cm_impl.h"

#ifdef SUBDEV_ENABLE
#include "utils_hmac.h"
#endif

#include "cJSON.h"

#ifdef SUBDEV_ENABLE
typedef struct {
    void* thing_id;
    int   request_id;
    char  product_key[PRODUCT_KEY_MAXLEN];
    char  device_name[DEVICE_NAME_MAXLEN];
    char  device_secret[DEVICE_SECRET_MAXLEN];
}request_info_t;
#endif
static void *g_dm_thing_manager_callback_mutex = NULL;
static void *g_dm_thing_manager_usercall_mutex = NULL;
static const char string_dm_thing_manager_class_name[] __DM_READ_ONLY__ = "dm_thg_mng_cls";
static const char string_thing_service_property_set[] __DM_READ_ONLY__ = "thing.service.property.set";
static const char string_thing_service_property_get[] __DM_READ_ONLY__ = "thing.service.property.get";
static const char string_thing_event_property_post[] __DM_READ_ONLY__ = "thing.event.property.post";
static const char string_method_name_thing_enable[] __DM_READ_ONLY__ = METHOD_NAME_THING_ENABLE;
static const char string_method_name_thing_disable[] __DM_READ_ONLY__ = METHOD_NAME_THING_DISABLE;
static const char string_method_name_thing_delete[] __DM_READ_ONLY__ = METHOD_NAME_THING_DELETE;
static const char string_method_name_thing_tsl_post_reply[] __DM_READ_ONLY__ = METHOD_NAME_THING_TSL_POST_REPLY;
static const char string_method_name_thing_tsl_get[] __DM_READ_ONLY__ = METHOD_NAME_THING_TSL_GET;
static const char string_method_name_thing_tsl_get_reply[] __DM_READ_ONLY__ = METHOD_NAME_THING_TSL_GET_REPLY;
static const char string_method_name_down_raw[] __DM_READ_ONLY__ = METHOD_NAME_DOWN_RAW;
static const char string_method_name_down_raw_reply[] __DM_READ_ONLY__ = METHOD_NAME_DOWN_RAW_PEPLY;
static const char string_method_name_up_raw[] __DM_READ_ONLY__ = METHOD_NAME_UP_RAW;
static const char string_method_name_up_raw_reply[] __DM_READ_ONLY__ = METHOD_NAME_UP_RAW_REPLY;
static const char string_method_name_property_set[] __DM_READ_ONLY__ = METHOD_NAME_PROPERTY_SET;
static const char string_method_name_property_get[] __DM_READ_ONLY__ = METHOD_NAME_PROPERTY_GET;
#ifdef SUBDEV_ENABLE
static const char string_method_name_topo_add[] __DM_READ_ONLY__ = METHOD_NAME_TOPO_ADD;
static const char string_method_name_topo_add_reply[] __DM_READ_ONLY__ = METHOD_NAME_TOPO_ADD_REPLY;
static const char string_method_name_topo_del[] __DM_READ_ONLY__ = METHOD_NAME_TOPO_DEL;
static const char string_method_name_topo_del_reply[] __DM_READ_ONLY__ = METHOD_NAME_TOPO_DEL_REPLY;
static const char string_method_name_topo_get[] __DM_READ_ONLY__ = METHOD_NAME_TOPO_GET;
static const char string_method_name_topo_get_reply[] __DM_READ_ONLY__ = METHOD_NAME_TOPO_GET_REPLY;
static const char string_method_name_sub_register[] __DM_READ_ONLY__ = METHOD_NAME_SUB_REGISTER;
static const char string_method_name_sub_register_reply[] __DM_READ_ONLY__ = METHOD_NAME_SUB_REGISTER_REPLY;
static const char string_method_name_sub_unregister[] __DM_READ_ONLY__ = METHOD_NAME_SUB_UNREGISTER;
static const char string_method_name_sub_unregister_reply[] __DM_READ_ONLY__ = METHOD_NAME_SUB_UNREGISTER_REPLY;
static const char string_method_name_sub_login[] __DM_READ_ONLY__ = METHOD_NAME_SUB_LOGIN;
static const char string_method_name_sub_login_reply[] __DM_READ_ONLY__ = METHOD_NAME_SUB_LOGIN_REPLY;
static const char string_method_name_sub_logout[] __DM_READ_ONLY__ = METHOD_NAME_SUB_LOGOUT;
static const char string_method_name_sub_logout_reply[] __DM_READ_ONLY__ = METHOD_NAME_SUB_LOGOUT_REPLY;
static const char string_method_name_sub_gw_permit[] __DM_READ_ONLY__ = METHOD_NAME_SUB_GW_PERMIT;
static const char string_method_name_sub_gw_permit_reply[] __DM_READ_ONLY__ = METHOD_NAME_SUB_GW_PERMIT_REPLY;
#endif
#ifdef EXTENDED_INFO_ENABLED
static const char string_method_name_deviceinfo_update[] __DM_READ_ONLY__ = METHOD_NAME_DEVICEINFO_UPDATE;
static const char string_method_name_deviceinfo_update_reply[] __DM_READ_ONLY__ = METHOD_NAME_DEVICEINFO_UPDATE_REPLY;
static const char string_method_name_deviceinfo_delete[] __DM_READ_ONLY__ = METHOD_NAME_DEVICEINFO_DELETE;
static const char string_method_name_deviceinfo_delete_reply[] __DM_READ_ONLY__ = METHOD_NAME_DEVICEINFO_DELETE_REPLY;
#endif /* EXTENDED_INFO_ENABLED */
#ifdef RRPC_ENABLED
static const char string_method_name_rrpc_request_plus[] __DM_READ_ONLY__ = METHOD_NAME_RRPC_REQUEST_PLUS;
static const char string_method_name_rrpc_request[] __DM_READ_ONLY__ = METHOD_NAME_RRPC_REQUEST;
#endif /* RRPC_ENABLED */
#ifdef LOCAL_CONN_ENABLE
static const char string_method_name_lan_prefix_get[] __DM_READ_ONLY__ = METHOD_NAME_LAN_PREFIX_GET;
static const char string_method_name_lan_prefix_get_reply[] __DM_READ_ONLY__ = METHOD_NAME_LAN_PREFIX_GET_REPLY;
static const char string_method_name_lan_prefix_update[] __DM_READ_ONLY__ = METHOD_NAME_LAN_PREFIX_UPDATE;
static const char string_method_name_lan_prefix_update_reply[] __DM_READ_ONLY__ = METHOD_NAME_LAN_PREFIX_UPDATE_REPLY;
#endif /* LOCAL_CONN_ENABLE */
static const char string_cm_event_handler_prompt_start[] __DM_READ_ONLY__ = "\ncm_event_handler:\n###\n";
static const char string_cm_event_handler_prompt_end[] __DM_READ_ONLY__ = "\n###\n";
static const char string_cm_event_type_cloud_connected[] __DM_READ_ONLY__ = "cloud connected";
static const char string_cm_event_type_cloud_disconnect[] __DM_READ_ONLY__ = "cloud disconnect";
static const char string_cm_event_type_cloud_reconnect[] __DM_READ_ONLY__ = "cloud reconnect";
#ifdef LOCAL_CONN_ENABLE
static const char string_cm_event_type_local_connected[] __DM_READ_ONLY__ = "local connected";
static const char string_cm_event_type_local_disconnect[] __DM_READ_ONLY__ = "local disconnect";
static const char string_cm_event_type_local_reconnect[] __DM_READ_ONLY__ = "local reconnect";
static const char string_dev_core_service_dev[] __DM_READ_ONLY__ = "/dev/core/service/dev";
static const char string_method_name_property_post[] __DM_READ_ONLY__ = METHOD_NAME_PROPERTY_POST;
static const char string_method_name_property_post_reply[] __DM_READ_ONLY__ = METHOD_NAME_PROPERTY_POST_REPLY;
#endif
static const char string_cm_event_type_found_device[] __DM_READ_ONLY__ = "found device";
static const char string_cm_event_type_remove_device[] __DM_READ_ONLY__ = "remove device";
static const char string_cm_event_type_register_result[] __DM_READ_ONLY__ = "register result";
static const char string_cm_event_type_unregister_result[] __DM_READ_ONLY__ = "unregister result";
static const char string_cm_event_type_send_result[] __DM_READ_ONLY__ = "send result";
static const char string_cm_event_type_new_data_received[] __DM_READ_ONLY__ = "new data received";

static const char string_local_thing_list[] __DM_READ_ONLY__ = "local thing";
static const char string_local_thing_name_list[] __DM_READ_ONLY__ = "local thing name";
static const char string_sub_thing_list[] __DM_READ_ONLY__ = "sub thing";
static const char string_sub_thing_name_list[] __DM_READ_ONLY__ = "sub thing name";
static const char string_callback_list[] __DM_READ_ONLY__ = "callback list";

#ifdef SUBDEV_ENABLE
static const char string_subdev_callback_list[] __DM_READ_ONLY__ = "subdev callback list";
static const char string_subdev_request_list[] __DM_READ_ONLY__ = "subdev request list";
#endif
static const char string_service_property_get_identifier_list[] __DM_READ_ONLY__ = "service property get id list";

static const char string_success[] __DM_READ_ONLY__ = "success";
static const char string_fail[] __DM_READ_ONLY__ = "fail";
static const char string_request[] __DM_READ_ONLY__ = "request";
static const char string_response[] __DM_READ_ONLY__ = "response";
static const char string_event[] __DM_READ_ONLY__ = "event";
static const char string_service[] __DM_READ_ONLY__ = "service";

static const char string_tsl_file_pattern[] __DM_READ_ONLY__ = "%s.%s.txt";

static void free_list_string(void* _thing_name, va_list* params);
static void free_list_thing(void* _thing_name, va_list* params);
static void local_thing_list_iterator(const void* _self, handle_fp_t handle_fp, ...);
static void invoke_callback_func(void* _callback_fp, va_list* params);
static void callback_list_iterator(const void* _self, handle_fp_t handle_fp);
static void generate_subscribe_uri(void* _dm_thing_manager, void* _thing, int cloud, int subdev, char *pk, char *dn);
static void send_request_to_uri(void* _dm_thing_manager, const char* _uri);
static void clear_and_set_message_info(message_info_t** _message_info, dm_thing_manager_t* _dm_thing_manager);
static void get_product_key_device_name(char* _product_key, char* _device_name, void* _thing, void* _dm_thing_manager);
static void* dm_thing_manager_generate_new_local_thing(void* _self, const char* tsl, int tsl_len);
static int dm_thing_manager_set_thing_property_value(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
static int dm_thing_manager_install_product_key_device_name(void* _self, const void* thing_id, char* product_key, char* device_name);
static int dm_thing_manager_trigger_event(void* _self, const void* thing_id, const void* event_identifier, const char* property_identifier);

#ifdef LOCAL_CONN_ENABLE
static void dm_cm_local_conn_register_handler(iotx_cm_send_peer_t* _source, iotx_cm_message_info_t* _msg, void* user_data);
#endif

#ifdef SUBDEV_ENABLE
static int send_subdev_login_message(void* _self, const char* subdev_pk, const char* subdev_dn, const char* subdev_ds, int _clean_session, char *hmac_method);
static int send_subdev_topo_add_message(void* _self, const char* subdev_pk, const char* subdev_dn, const char* subdev_ds, char* hmac_method);
static void invoke_subdev_callback_func(void* _subdev_callback_fp, va_list* params);
static void subdev_callback_list_iterator(const void* _self, handle_fp_t handle_fp);
static request_info_t* find_topo_add_request_info_by_id(void* _dm_thing_manager, int request_id);
static request_info_t* find_topo_delete_request_info_by_id(void* _dm_thing_manager, int request_id);
static void free_sub_request_node(void* _dm_thing_manager, request_info_t* node);
static request_info_t* find_subdev_login_request_info_by_id(void* _dm_thing_manager, int request_id);
static request_info_t* find_subdev_logout_request_info_by_id(void* _dm_thing_manager, int request_id);
static request_info_t* find_subdev_register_request_info_by_id(void* _dm_thing_manager, int request_id);
static void sub_thing_list_iterator(const void* _self, handle_fp_t handle_fp, ...);
static void find_node_by_thing_object(void* _thing, va_list* params);
static void* query_thing_id_by_uri(void* _self, char* uri);
static int check_update_subdev_permit_time(dm_thing_manager_t* _dm_thing_manager);
extern void dm_lltoa(long long n, char* str, int radix);
void* dm_thing_manager_query_thing_id(void* _self, const char* pk, const char* dn, int* is_subthing);
#endif

#ifdef RRPC_ENABLED
static int dm_thing_manager_answer_service(void* _self, const void* thing_id, const void* identifier, int response_id, int code, int rrpc);
#else
static int dm_thing_manager_answer_service(void* _self, const void* thing_id, const void* identifier, int response_id, int code);
#endif /* RRPC_ENABLED */

static void dm_cm_register_handler(iotx_cm_send_peer_t* _source, iotx_cm_message_info_t* _msg, void* user_data);

static void list_insert(void* _list, void* _data)
{
    list_t** list = (list_t**)_list;

    (*list)->insert(list, _data);
}

static void invoke_callback_list(void* _dm_thing_manager, dm_callback_type_t dm_callback_type)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    list_t** list;

    assert(dm_callback_type < dm_callback_type_number);

	HAL_MutexUnlock(g_dm_thing_manager_usercall_mutex);
    /* invoke callback funtions. */
    list = dm_thing_manager->_callback_list;
    if (list && !(*list)->empty(list)) {
        dm_thing_manager->_callback_type = dm_callback_type;
        callback_list_iterator(dm_thing_manager, invoke_callback_func);
    }
	HAL_MutexLock(g_dm_thing_manager_usercall_mutex);
}

#ifdef SUBDEV_ENABLE
static void invoke_subdev_callback_list(void* _dm_thing_manager, dm_subdev_callback_type_t dm_subdev_callback_type)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    list_t** list;

    assert(dm_subdev_callback_type < dm_subdev_callback_type_number);

    /* invoke callback funtions. */
    list = dm_thing_manager->_subdev_callback_list;
    if (list && !(*list)->empty(list)) {
        dm_thing_manager->_subcallback_type = dm_subdev_callback_type;
        subdev_callback_list_iterator(dm_thing_manager, invoke_subdev_callback_func);
    }
}
#endif

static void local_thing_generate_subscribe_uri(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;
    int cloud;

    dm_thing_manager = va_arg(*params, void*);
    cloud = va_arg(*params, int);

    assert(dm_thing_manager && thing && *thing);

    /* subscribe subjects. */
    generate_subscribe_uri(dm_thing_manager, thing, cloud, 0, NULL, NULL);
}

#ifdef _PLATFORM_IS_LINUX_
char* read_file(const char* filename)
{
    FILE* file = NULL;
    long length = 0;
    char* content = NULL;
    size_t read_chars = 0;

    /* open in read binary mode */
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        goto cleanup;
    }

    /* get the length */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        goto cleanup;
    }
    length = ftell(file);
    if (length < 0)
    {
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        goto cleanup;
    }

    /* allocate content buffer */
    content = (char*)dm_lite_calloc(1, (size_t)length + sizeof(""));
    if (content == NULL)
    {
        goto cleanup;
    }

    /* read the file into memory */
    read_chars = fread(content, sizeof(char), (size_t)length, file);
    if ((long)read_chars != length)
    {
        dm_lite_free(content);
        content = NULL;
        goto cleanup;
    }
    content[read_chars] = '\0';


cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return content;
}
#endif

static void change_local_thing_login(void* _local_thing, va_list* params)
{
    dm_thing_t* dm_thing = _local_thing;
    int login;

    login = va_arg(*params, int);

    if (dm_thing) dm_thing->_login = login;
}

static void dm_cm_event_handler(void* pcontext, iotx_cm_event_msg_t* msg, void* user_data)
{
    iotx_cm_event_result_t* cm_event_result;
    dm_thing_manager_t* dm_thing_manager = user_data;
    const char* event_str = NULL;
    list_t** list;
#ifdef _PLATFORM_IS_LINUX_
    char file_name[DEVICE_ID_MAXLEN + 6] = {0};
    char* file_content = NULL;
    void* thing;
#endif
	HAL_MutexLock(g_dm_thing_manager_callback_mutex);
	HAL_MutexLock(g_dm_thing_manager_usercall_mutex);
    dm_printf("%s", string_cm_event_handler_prompt_start);
    if (IOTX_CM_EVENT_REGISTER_RESULT == msg->event_id) {
        iotx_cm_event_result_t* cm_event_result = (iotx_cm_event_result_t*)msg->msg;

        dm_log_info("register result:\tresult: %d(%s)\tURI: %s\t",
                    cm_event_result->result, cm_event_result->result == 0 ? string_success : string_fail, cm_event_result->URI);

        event_str = string_cm_event_type_register_result;

    } else if (IOTX_CM_EVENT_UNREGISTER_RESULT == msg->event_id) {
        cm_event_result = (iotx_cm_event_result_t*)msg->msg;

        dm_log_info("un-register result:\tresult: %s,\tURI: %s,\t", cm_event_result->result, cm_event_result->URI);

        event_str = string_cm_event_type_unregister_result;
    } else if (IOTX_CM_EVENT_CLOUD_DISCONNECT == msg->event_id) {
        if (dm_thing_manager->_cloud_connected) {
            dm_thing_manager->_cloud_connected = 0;
            invoke_callback_list(dm_thing_manager, dm_callback_type_cloud_disconnected);

            local_thing_list_iterator(dm_thing_manager, change_local_thing_login, 0);
        }
        event_str = string_cm_event_type_cloud_disconnect;
    } else if (IOTX_CM_EVENT_CLOUD_RECONNECT == msg->event_id) {
        if (dm_thing_manager->_cloud_connected == 0) {
            dm_thing_manager->_cloud_connected = 1;
            invoke_callback_list(dm_thing_manager, dm_callback_type_cloud_connected);

            local_thing_list_iterator(dm_thing_manager, change_local_thing_login, 1);
        }
        event_str = string_cm_event_type_cloud_reconnect;
    }  else if (IOTX_CM_EVENT_CLOUD_CONNECTED == msg->event_id) {
        if (dm_thing_manager->_cloud_connected == 0) {
            dm_thing_manager->_cloud_connected = 1;

            local_thing_list_iterator(dm_thing_manager, change_local_thing_login, 1);

            if (dm_thing_manager->_destructing == 1) return;
            /* subscribe uris not related to tsl. */
            generate_subscribe_uri(dm_thing_manager, NULL, 1, 0, dm_thing_manager->_product_key, dm_thing_manager->_device_name);
#ifdef _PLATFORM_IS_LINUX_
            dm_snprintf(file_name, sizeof(file_name), string_tsl_file_pattern, dm_thing_manager->_product_key, dm_thing_manager->_device_name);
            file_content = read_file(file_name);
            if (file_content) {
                thing = dm_thing_manager_generate_new_local_thing(dm_thing_manager, file_content, strlen(file_content));
                if (NULL == thing) {
                    dm_log_err("generate new thing failed");
					HAL_MutexUnlock(g_dm_thing_manager_usercall_mutex);
					HAL_MutexUnlock(g_dm_thing_manager_callback_mutex);
                    return;
                }
                dm_lite_free(file_content);
            } else
#endif /* _PLATFORM_IS_LINUX_ */
            if (dm_thing_manager->_get_tsl_from_cloud) {
                /* get tsl template. */
                send_request_to_uri(dm_thing_manager, string_method_name_thing_tsl_get);
            }

            list = (list_t**)dm_thing_manager->_local_thing_list;

            list_iterator(list, local_thing_generate_subscribe_uri, dm_thing_manager, 1);

            invoke_callback_list(dm_thing_manager, dm_callback_type_cloud_connected);
        }
        event_str = string_cm_event_type_cloud_connected;
    } else if (IOTX_CM_EVENT_FOUND_DEVICE == msg->event_id) {
        /* currently do not process this. */
        event_str = string_cm_event_type_found_device;
    } else if (IOTX_CM_EVENT_REMOVE_DEVICE == msg->event_id) {
        /* currently do not process this. */
        event_str = string_cm_event_type_remove_device;
    } else if (IOTX_CM_EVENT_SEND_RESULT == msg->event_id) {
        event_str = string_cm_event_type_send_result;
    } else if (IOTX_CM_EVENT_NEW_DATA_RECEIVED == msg->event_id) {
        iotx_cm_new_data_t* new_data = (iotx_cm_new_data_t*)msg->msg;
#ifdef LOCAL_CONN_ENABLE
        if (strstr(new_data->message_info->URI, string_dev_core_service_dev)) {
            dm_cm_local_conn_register_handler(new_data->peer, new_data->message_info, dm_thing_manager);
        } else {
            dm_cm_register_handler(new_data->peer, new_data->message_info, dm_thing_manager);
        }
#else
        dm_cm_register_handler(new_data->peer, new_data->message_info, dm_thing_manager);
#endif

        event_str = string_cm_event_type_new_data_received;
#ifdef LOCAL_CONN_ENABLE
    }  else if (IOTX_CM_EVENT_LOCAL_CONNECTED == msg->event_id) {
        if (dm_thing_manager->_local_connected == 0) {
            dm_thing_manager->_local_connected = 1;

            if (dm_thing_manager->_destructing == 1) return;

            generate_subscribe_uri(dm_thing_manager, NULL, 0, 0, dm_thing_manager->_product_key, dm_thing_manager->_device_name);

            list = (list_t**)dm_thing_manager->_local_thing_list;

            list_iterator(list, local_thing_generate_subscribe_uri, dm_thing_manager, 0);

            invoke_callback_list(dm_thing_manager, dm_callback_type_local_connected);
        }

        event_str = string_cm_event_type_local_connected;
    } else if (IOTX_CM_EVENT_LOCAL_DISCONNECT == msg->event_id) {
        dm_thing_manager->_local_connected = 0;

        invoke_callback_list(dm_thing_manager, dm_callback_type_local_disconnected);
        event_str = string_cm_event_type_local_disconnect;
    } else if (IOTX_CM_EVENT_LOCAL_RECONNECT == msg->event_id) {
        dm_thing_manager->_local_connected = 1;

        invoke_callback_list(dm_thing_manager, dm_callback_type_local_connected);
        event_str = string_cm_event_type_local_reconnect;
#endif
    } else {
        dm_printf("unknown event %d\n", msg->event_id);
    }

    dm_printf("\nevent %d(%s)%s", msg->event_id, event_str, string_cm_event_handler_prompt_end);
	HAL_MutexUnlock(g_dm_thing_manager_usercall_mutex);
	HAL_MutexUnlock(g_dm_thing_manager_callback_mutex);
}

static void print_iotx_cm_message_info(const void* _source, const void* _msg)
{
    const iotx_cm_send_peer_t* iotx_cm_send_peer = _source;
    const iotx_cm_message_info_t* iotx_cm_message_info = _msg;

    dm_printf("***\nsource\t%s:%s\n", iotx_cm_send_peer->product_key, iotx_cm_send_peer->device_name);

    dm_printf("type\t%s\n", iotx_cm_message_info->message_type == IOTX_CM_MESSAGE_RAW ?
                  "RAW" : (iotx_cm_message_info->message_type == IOTX_CM_MESSAGE_REQUEST ? string_request : string_response));
    dm_printf("URI\t%s\n", iotx_cm_message_info->URI);

    dm_printf("code\t%d\n", iotx_cm_message_info->code);
    dm_printf("id\t%d\n", iotx_cm_message_info->id);

    if (iotx_cm_message_info->method) dm_printf("method\t%s\n", iotx_cm_message_info->method);

    dm_printf("param:\n%s\n", (char*)iotx_cm_message_info->parameter);
    dm_printf("param len:\t%dbytes\n", iotx_cm_message_info->parameter_length);
    if (iotx_cm_message_info->message) dm_printf("message:\t%s\n", iotx_cm_message_info->message);
    dm_printf("***\n");
}

static int check_set_lite_property_for_struct(cJSON* cjson_obj, lite_property_t *lite_property)
{
    int i;
    lite_property_t* sub_property;
    cJSON *subobject;
    for (i = 0; i < lite_property->data_type.data_type_specs_number; i++) {
        sub_property = (lite_property_t*)lite_property->data_type.specs + i;
        subobject = cJSON_GetObjectItem(cjson_obj, sub_property->identifier);
        if (!subobject) return -1;
        if (data_type_type_struct == sub_property->data_type.type) {
            if (check_set_lite_property_for_struct(subobject, sub_property)) {
                return -1;
            }
        }
    }
    return 0;
}

static void find_and_set_array_item(dm_thing_manager_t* dm_thing_manager,thing_t** thing, cJSON* cjson_arr_obj, int item_index, lite_property_t *lite_property,data_type_type_t type)
{
    int int_val;
    float float_val;
    double double_val;
    char temp_buf[64] = {0};
    char identifier[64] = {0};
    char* string_val = NULL;
    cJSON *arr_json_item;
    if (item_index > lite_property->data_type.value.data_type_array_t.size) {
        dm_printf("input json array item:%d > lite json array size:%d ", item_index, lite_property->data_type.value.data_type_array_t.size);
        return;
    }

    arr_json_item = cJSON_GetArrayItem(cjson_arr_obj,item_index);
    if (!arr_json_item) {
        dm_log_err("get array[%d] failed", item_index);
        goto do_exit;
    }
    switch(lite_property->data_type.value.data_type_array_t.item_type) {
        case data_type_type_int: {
            int_val = arr_json_item->valueint;
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", int_val);
        }break;
        case data_type_type_double: {
                double_val = arr_json_item->valuedouble;
                dm_snprintf(temp_buf, sizeof(temp_buf), "%.16lf", double_val);
        }break;
        case data_type_type_float: {
            float_val = arr_json_item->valuedouble;
            dm_snprintf(temp_buf, sizeof(temp_buf), "%.7f", float_val);
        }break;
        case data_type_type_text: {
            string_val = dm_lite_calloc(1, strlen(arr_json_item->valuestring) + 1);
            assert(string_val);
            if (string_val == NULL) {
                dm_printf("NO memory...");
                return;
            }
            strcpy(string_val, arr_json_item->valuestring);
        }break;
        default:{
            dm_log_err("don't support %d type", lite_property->data_type.value.data_type_array_t.item_type);
            goto do_exit;
        }
    }
    dm_thing_manager->_property_identifier_value_set = NULL;
    if (string_val) {
        dm_thing_manager->_property_identifier_value_set = string_val;
    } else if (strlen(temp_buf)) {
        dm_thing_manager->_property_identifier_value_set = dm_lite_calloc(1, strlen(temp_buf) + 1);
        assert(dm_thing_manager->_property_identifier_value_set);
        strcpy(dm_thing_manager->_property_identifier_value_set, temp_buf);
    }

    if (dm_thing_manager->_property_identifier_set && dm_thing_manager->_property_identifier_value_set) {
        dm_snprintf(identifier, sizeof(identifier), "%s[%d]",(char*)dm_thing_manager->_property_identifier_set, item_index);
        dm_thing_manager_set_thing_property_value(dm_thing_manager, thing, identifier,
                                                  NULL, dm_thing_manager->_property_identifier_value_set);

        dm_lite_free(dm_thing_manager->_property_identifier_value_set);
        dm_thing_manager->_property_identifier_value_set = NULL;
    }
do_exit:
    return;
}


static void find_and_set_lite_property_struct_for_service_property_set(void* _item, ...);
static void find_and_set_lite_property_for_service_property_set(void* _item, int _index, va_list* params)
{
    property_t* property = _item;
    lite_property_t* lite_property;
    lite_property_t* lite_property_struct;
    dm_thing_manager_t* dm_thing_manager;
    thing_t** thing;
    cJSON* property_set_param_obj;
    cJSON* temp_cjson_obj;
    char temp_buf[64] = {0};
    int int_val;
    float float_val;
    double double_val;
    char* string_val = NULL;
    char* identifier_prefix = NULL;
    char identifier_buff[128] = {0};
    int index;
    int arrsize = 0;

    dm_thing_manager = va_arg(*params, dm_thing_manager_t*);
    property_set_param_obj = va_arg(*params, cJSON*);
    thing = va_arg(*params, thing_t**);
    identifier_prefix = va_arg(*params, char*);

    _index = _index;

    assert(property && dm_thing_manager && property_set_param_obj && thing && *thing);

    lite_property = (lite_property_t*)property;

    temp_cjson_obj = cJSON_GetObjectItem(property_set_param_obj, lite_property->identifier);
    if (temp_cjson_obj) {
        if (identifier_prefix) {
            dm_snprintf(identifier_buff, sizeof(identifier_buff), "%s%c%s", identifier_prefix, DEFAULT_TSL_DELIMITER, lite_property->identifier);
            dm_thing_manager->_property_identifier_set = identifier_buff;
        } else {
            dm_thing_manager->_property_identifier_set = lite_property->identifier;
        }

        if (lite_property->data_type.type == data_type_type_text) {
#ifndef CJSON_STRING_ZEROCOPY
            string_val = dm_lite_calloc(1, strlen(temp_cjson_obj->valuestring) + 1);
            assert(string_val);
            if (string_val == NULL) {
                dm_printf("NO memory...");
                return;
            }
            strcpy(string_val, temp_cjson_obj->valuestring);
#else
            string_val = dm_lite_calloc(1, temp_cjson_obj->valuestring_length + 1);
            assert(string_val);
            if (string_val == NULL) {
                dm_printf("NO memory...");
                return;
            }
            strncpy(string_val, temp_cjson_obj->valuestring, temp_cjson_obj->valuestring_length);
#endif
        } else if (lite_property->data_type.type == data_type_type_float) {
            float_val = temp_cjson_obj->valuedouble;
            dm_snprintf(temp_buf, sizeof(temp_buf), "%.7f", float_val);
        } else if (lite_property->data_type.type == data_type_type_double) {
            double_val = temp_cjson_obj->valuedouble;
            dm_snprintf(temp_buf, sizeof(temp_buf), "%.16lf", double_val);
        } else if (lite_property->data_type.type == data_type_type_date) {
            if (temp_cjson_obj->type == cJSON_String) {
                dm_snprintf(temp_buf, sizeof(temp_buf), "%s", temp_cjson_obj->valuestring);
            } else if (temp_cjson_obj->type == cJSON_Number) {
                dm_snprintf(temp_buf, sizeof(temp_buf), "%lf", temp_cjson_obj->valuedouble);
            }
        } else if (lite_property->data_type.type == data_type_type_enum ||
                   lite_property->data_type.type == data_type_type_bool ||
                   lite_property->data_type.type == data_type_type_int) {

            int_val = temp_cjson_obj->valueint;
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", int_val);
        } else if (lite_property->data_type.type == data_type_type_struct) {
            assert(cJSON_IsObject(temp_cjson_obj));
            if (!cJSON_IsObject(temp_cjson_obj)) return;
            if (check_set_lite_property_for_struct(temp_cjson_obj, lite_property)) {
                dm_printf("invalid json");
                return;
            }

            for (index = 0; index < lite_property->data_type.data_type_specs_number; ++index) {
                lite_property_struct = (lite_property_t*)lite_property->data_type.specs + index;
                find_and_set_lite_property_struct_for_service_property_set(lite_property_struct, dm_thing_manager,
                                                                           temp_cjson_obj, thing, lite_property->identifier);
            }
            dm_thing_manager->_identifier = lite_property->identifier;
            invoke_callback_list(dm_thing_manager, dm_callback_type_property_value_set);
        } else if (lite_property->data_type.type == data_type_type_array) {
            if (!cJSON_IsArray(temp_cjson_obj)) {
                dm_log_err("json type is not array");
                return;
            }
            arrsize = cJSON_GetArraySize(temp_cjson_obj);
            if (arrsize > lite_property->data_type.value.data_type_array_t.size) {
                dm_printf("input json array item:%d > lite json array item:%d ", arrsize, lite_property->data_type.value.data_type_array_t.size);
                return;
            }
            for (index = 0; index < arrsize; index++){
                find_and_set_array_item(dm_thing_manager,thing, temp_cjson_obj, index, lite_property, lite_property->data_type.type);
            }
            dm_thing_manager->_identifier = lite_property->identifier;
            invoke_callback_list(dm_thing_manager, dm_callback_type_property_value_set);
            return;
        }

        if (string_val) {
            dm_thing_manager->_property_identifier_value_set = string_val;
        } else if (strlen(temp_buf)) {
            dm_thing_manager->_property_identifier_value_set = dm_lite_calloc(1, strlen(temp_buf) + 1);
            assert(dm_thing_manager->_property_identifier_value_set);
            strcpy(dm_thing_manager->_property_identifier_value_set, temp_buf);
        }
    }

    if (dm_thing_manager->_property_identifier_set && dm_thing_manager->_property_identifier_value_set) {
        dm_thing_manager_set_thing_property_value(dm_thing_manager, thing, dm_thing_manager->_property_identifier_set,
                                                  NULL, dm_thing_manager->_property_identifier_value_set);

        dm_lite_free(dm_thing_manager->_property_identifier_value_set);
        dm_thing_manager->_property_identifier_value_set = NULL;
    }
}

static void find_and_set_lite_property_struct_for_service_property_set(void* _item, ...)
{
    va_list params;

    va_start(params, _item);

    find_and_set_lite_property_for_service_property_set(_item, 0, &params);

    va_end(params);
}

static void find_thing_via_product_key_and_device_name(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;
    iotx_cm_send_peer_t* iotx_cm_send_peer;

    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_thing_manager = va_arg(*params, void*);
    iotx_cm_send_peer = va_arg(*params, iotx_cm_send_peer_t*);

    assert(dm_thing_manager && thing && *thing);

    dm_thing_manager_install_product_key_device_name(dm_thing_manager, thing, product_key, device_name);

    if (strcmp(product_key, iotx_cm_send_peer->product_key) == 0 && strcmp(device_name, iotx_cm_send_peer->device_name) == 0) {
        dm_thing_manager->_thing_id = thing;
    }
}

static int set_service_array_input(dm_thing_manager_t* _dm_thing_manager, thing_t** thing, cJSON* cjson_arr_obj, int item_index,lite_property_t *lite_property, data_type_type_t type, char *_identifier)
{
    int ret = -1;
    int int_val;
    float float_val;
    double double_val;
    char temp_buf[64] = {0};
    char identifier[64] = {0};
    cJSON *arr_json_item;

    if(!_dm_thing_manager || !thing || !cjson_arr_obj || item_index < 0 || !lite_property || !_identifier)   {
        dm_log_err("invalid params", item_index);
        goto do_exit;
    }
    if (item_index > lite_property->data_type.value.data_type_array_t.size) {
        dm_printf("input json array item:%d > lite json array size:%d ", item_index, lite_property->data_type.value.data_type_array_t.size);
        goto do_exit;
    }


    arr_json_item = cJSON_GetArrayItem(cjson_arr_obj,item_index);
    if (!arr_json_item) {
        dm_log_err("get array[%d] failed", item_index);
        goto do_exit;
    }
    switch(lite_property->data_type.value.data_type_array_t.item_type) {
    case data_type_type_int: {
        int_val = arr_json_item->valueint;
        dm_snprintf(temp_buf, sizeof(temp_buf), "%d", int_val);
    }break;
    case data_type_type_double: {
        double_val = arr_json_item->valuedouble;
        dm_snprintf(temp_buf, sizeof(temp_buf), "%.16lf", double_val);
    }break;
    case data_type_type_float: {
        float_val = arr_json_item->valuedouble;
        dm_snprintf(temp_buf, sizeof(temp_buf), "%.7f", float_val);
    }break;
    case data_type_type_text: {
        dm_snprintf(temp_buf, sizeof(temp_buf), "%s", arr_json_item->valuestring);
    }break;
    default:{
        dm_log_err("don't support %d type", lite_property->data_type.value.data_type_array_t.item_type);
        goto do_exit;
    }
    }

    if (strlen(temp_buf) > 0) {
        dm_snprintf(identifier, sizeof(identifier), "%s[%d]", _identifier, item_index);
        (*thing)->set_service_input_output_data_value_by_identifier(thing, identifier, NULL, temp_buf);
        ret = 0;
    }
do_exit:
    return ret;
}

static int parse_and_set_service_input(dm_thing_manager_t *_dm_thing_manager,thing_t **thing, service_t *service, char* parameter)
{
    cJSON* obj = NULL;
    cJSON* item;
    int i;
    char identifier[128] = {0};
    char* string_val;
    int arr_size = 0;
    int arr_index = 0;
    input_data_t* service_input_data;
    lite_property_t* property;

    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    obj = cJSON_Parse(parameter);
    if (!obj) return - 1;

    for (i = 0; i < service->service_input_data_num; i++) {
        service_input_data = &service->service_input_data[i]; /* inputData */

        property = &service_input_data->lite_property;

        item = cJSON_GetObjectItem(obj, property->identifier);
        if (!item) continue;

        snprintf(identifier, sizeof(identifier), "%s.%s", service->identifier, property->identifier);

        switch (property->data_type.type) {
        case data_type_type_text:
        {
#ifndef CJSON_STRING_ZEROCOPY
            string_val = dm_lite_calloc(1, strlen(item->valuestring) + 1);

            if (string_val == NULL) {
                if (obj) cJSON_Delete(obj);
                return -1;
            }
            strcpy(string_val, item->valuestring);
#else
            string_val = dm_lite_calloc(1, item->valuestring_length + 1);

            if (string_val == NULL) {
                cJSON_Delete(obj);
                return -1;
            }
            strncpy(string_val, item->valuestring, item->valuestring_length);
#endif /* CJSON_STRING_ZEROCOPY */
            (*thing)->set_service_input_output_data_value_by_identifier(thing, identifier, NULL, string_val);

            dm_lite_free(string_val);
        }
            break;
        case data_type_type_enum:
        case data_type_type_bool:
        case data_type_type_int:
            (*thing)->set_service_input_output_data_value_by_identifier(thing, identifier, &item->valueint, NULL);
            break;
        case data_type_type_float:
        case data_type_type_double:
        case data_type_type_date:
            (*thing)->set_service_input_output_data_value_by_identifier(thing, identifier, &item->valuedouble, NULL);
            break;
        case data_type_type_array:
            arr_size = cJSON_GetArraySize(item);
            for(arr_index = 0; arr_index < arr_size; arr_index++) {
                set_service_array_input(dm_thing_manager, thing, item, arr_index, property, property->data_type.type, identifier);
            }
            break;
        default:
            break;
        }
    }

    if (obj) cJSON_Delete(obj);

    return 0;
}

static void find_and_set_service_input(void* _item, int index, va_list* params)
{
    service_t* service;
    thing_t** thing;
    dm_thing_manager_t* dm_thing_manager;
    iotx_cm_message_info_t* iotx_cm_message_info;
    char* parameter = NULL;

    dm_thing_manager = va_arg(*params, void*);
    iotx_cm_message_info = va_arg(*params, iotx_cm_message_info_t*);
    thing = va_arg(*params, thing_t**);

    service = _item;

    assert(dm_thing_manager && iotx_cm_message_info && service);

    if (strcmp(service->method, iotx_cm_message_info->method) == 0) {
        dm_thing_manager->_service_identifier_requested = service->identifier;

        if (strstr(iotx_cm_message_info->URI, string_method_name_property_set) != NULL || strstr(iotx_cm_message_info->URI, string_method_name_property_get) != NULL) return;

        parameter = iotx_cm_message_info->parameter;
        if (parameter) parse_and_set_service_input(dm_thing_manager, thing, service, parameter);
    }
}

typedef void (*dm_cm_register_uri_handler_fp_t)(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info);

static void dm_cm_register_uri_handler_tsl_get_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    thing_t** thing;

#ifdef _PLATFORM_IS_LINUX_
    char file_name[DEVICE_ID_MAXLEN + 6] = {0};
    FILE* fp_tsl;
#endif

#ifdef SUBDEV_ENABLE

    if (dm_thing_manager->_is_subthing && dm_thing_manager->_sub_thing_id) {
        thing = dm_thing_manager->_sub_thing_id;
        (*thing)->set_tsl_string(thing, cm_message_info->parameter, cm_message_info->parameter_length);

        /* subscribe subjects. */
        if (dm_thing_manager->_cloud_connected) generate_subscribe_uri(dm_thing_manager, thing, 1, 0, NULL, NULL);
#ifdef LOCAL_CONN_ENABLE
        if (dm_thing_manager->_local_connected) generate_subscribe_uri(dm_thing_manager, thing, 0, 0, NULL, NULL);
#endif
    } else {
        thing = dm_thing_manager_generate_new_local_thing(dm_thing_manager, cm_message_info->parameter, cm_message_info->parameter_length);
        if (NULL == thing) {
            dm_log_err("generate new thing failed");
            return;
        }
    }
#else
    thing = dm_thing_manager_generate_new_local_thing(dm_thing_manager, cm_message_info->parameter, cm_message_info->parameter_length);
    if (NULL == thing) {
        dm_log_err("generate new thing failed");
        return;
    }
#endif

#ifdef _PLATFORM_IS_LINUX_
    dm_snprintf(file_name, sizeof(file_name), string_tsl_file_pattern, (*thing)->return_product_key(thing), (*thing)->return_device_name(thing));

    fp_tsl = fopen(file_name, "w+");
    if (fp_tsl) {
        fprintf(fp_tsl, "%s", (char*)cm_message_info->parameter);
        fclose(fp_tsl);
    }
#endif /* _PLATFORM_IS_LINUX_ */
}

static void dm_cm_register_uri_handler_property_set(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    thing_t** thing;

    cJSON* property_set_param_obj;

    thing = dm_thing_manager->_thing_id;
    assert(thing && cm_message_info->parameter);
    property_set_param_obj = cJSON_Parse(cm_message_info->parameter);
    assert(property_set_param_obj && cJSON_IsObject(property_set_param_obj));

    property_iterator(thing, find_and_set_lite_property_for_service_property_set, dm_thing_manager, property_set_param_obj, thing, NULL);

#ifdef RRPC_ENABLED
    dm_thing_manager_answer_service(dm_thing_manager, thing, dm_thing_manager->_service_identifier_requested,
                                    dm_thing_manager->_request_id, dm_thing_manager->_ret == 0 ? 200 : 400, 0);
#else
    dm_thing_manager_answer_service(dm_thing_manager, thing, dm_thing_manager->_service_identifier_requested,
                                    dm_thing_manager->_request_id, dm_thing_manager->_ret == 0 ? 200 : 400);
#endif /* RRPC_ENABLED */

    cJSON_Delete(property_set_param_obj);
}

static void dm_cm_register_uri_handler_property_get(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    thing_t** thing;

    cJSON* property_get_param_obj;
    cJSON* property_get_param_item_obj;
    list_t** list;
    int array_size, index;
    char* property_get_param_identifier;

    thing = dm_thing_manager->_thing_id;

    assert(cm_message_info->parameter);
    list = dm_thing_manager->_service_property_get_identifier_list;
    property_get_param_obj = cJSON_Parse(cm_message_info->parameter);

    assert(property_get_param_obj && cJSON_IsArray(property_get_param_obj));

    if (property_get_param_obj == NULL || cJSON_IsArray(property_get_param_obj) == 0) {
        dm_log_err("UNABLE to resolve %s params format", string_method_name_property_get);
        return;
    }

    array_size = cJSON_GetArraySize(property_get_param_obj);
    for (index = 0; index < array_size; ++index) {
        property_get_param_item_obj = cJSON_GetArrayItem(property_get_param_obj, index);
        assert(cJSON_IsString(property_get_param_item_obj));
#ifndef CJSON_STRING_ZEROCOPY
        property_get_param_identifier = dm_lite_calloc(1, strlen(property_get_param_item_obj->valuestring) + 1);
        strcpy(property_get_param_identifier, property_get_param_item_obj->valuestring);
#else
        property_get_param_identifier = dm_lite_calloc(1, property_get_param_item_obj->valuestring_length + 1);
        strncpy(property_get_param_identifier, property_get_param_item_obj->valuestring,
                property_get_param_item_obj->valuestring_length);
#endif
        list_insert(list, property_get_param_identifier);
    }
    cJSON_Delete(property_get_param_obj);
#ifdef RRPC_ENABLED
    dm_thing_manager_answer_service(dm_thing_manager, thing, dm_thing_manager->_service_identifier_requested,
                                    dm_thing_manager->_request_id, 200, 0);
#else
    dm_thing_manager_answer_service(dm_thing_manager, thing, dm_thing_manager->_service_identifier_requested,
                                    dm_thing_manager->_request_id, 200);
#endif /* RRPC_ENABLED */
}

static void dm_cm_register_uri_handler_thing_enable(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    /* invoke callback funtions. */
#ifdef SUBDEV_ENABLE
    if (dm_thing_manager->_thing_id) {
        invoke_callback_list(dm_thing_manager, dm_callback_type_thing_enabled);
    } else {
        assert(0);
    }
#else
    invoke_callback_list(dm_thing_manager, dm_callback_type_thing_enabled);
#endif
}

static void dm_cm_register_uri_handler_thing_disable(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    /* invoke callback funtions. */
#ifdef SUBDEV_ENABLE
    if (dm_thing_manager->_thing_id) {
        invoke_callback_list(dm_thing_manager, dm_callback_type_thing_disabled);
    } else {
        assert(0);
    }
#else
    invoke_callback_list(dm_thing_manager, dm_callback_type_thing_disabled);
#endif
}

#ifdef RRPC_ENABLED
static void dm_cm_register_uri_handler_rrpc_request(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;

    char* p;
    //  "/sys/productKey/{productKey}/productKey/{deviceName}/rrpc/request/${messageId}"
    //  "/ext/rrpc/${messageId}/sys/productKey/{productKey}/productKey/{deviceName}"
#ifdef RRPC_NEW
    p = strstr(cm_message_info->URI, "/ext/rrpc/");
    if (p) {
        p = p + strlen("/ext/rrpc/");
        dm_thing_manager->_rrpc_message_id = atoll(p);
    }
#else
    p = strrchr(cm_message_info->URI, '/');
    if (p) {
        p++;
        dm_thing_manager->_rrpc_message_id = atoi(p);
    }
#endif /* RRPC_NEW */
    /* invoke callback funtions. */
    dm_thing_manager->_identifier = dm_thing_manager->_service_identifier_requested;
#ifdef SUBDEV_ENABLE
    if (0 == dm_thing_manager->_is_subthing) {
        invoke_callback_list(dm_thing_manager, dm_callback_type_rrpc_requested);
    }
#else
    invoke_callback_list(dm_thing_manager, dm_callback_type_rrpc_requested);
#endif
}
#endif /* RRPC_ENABLED */

#ifdef EXTENDED_INFO_ENABLED

static void dm_cm_register_uri_handler_deviceinfo_update_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{

}

static void dm_cm_register_uri_handler_deviceinfo_delete_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{

}

#endif /* EXTENDED_INFO_ENABLED*/

#ifdef SUBDEV_ENABLE
static void dm_cm_register_uri_handler_topo_add_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    request_info_t* request_info;

    /* invoke callback functions. */
    request_info = find_topo_add_request_info_by_id(dm_thing_manager, cm_message_info->id);

    if (!request_info) {
        dm_log_err("cannot find node from topo_add_request_info, request_id:%d", cm_message_info->id);
    } else {
        dm_thing_manager->_code = cm_message_info->code;
        dm_thing_manager->_sub_thing_id = request_info->thing_id;
        dm_thing_manager->_message = cm_message_info->message;

        if (dm_thing_manager->_code == 200) {
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_topo_add_success);
#if 0
            send_subdev_login_message(dm_thing_manager, request_info->product_key, request_info->device_name, request_info->device_secret, 0, "hmacsha1"); /* "hmacmd5" or "hmacsha1" */
#endif
        } else {
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_topo_add_fail);
        }

        free_sub_request_node(dm_thing_manager, request_info);
        dm_lite_free(request_info);
    }
}

static void dm_cm_register_uri_handler_topo_delete_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    request_info_t* request_info;

    /* invoke callback functions. */
    request_info = find_topo_delete_request_info_by_id(dm_thing_manager, cm_message_info->id);

    if (!request_info) {
        dm_log_err("cannot find node from topo_delete_request_info, request_id:%d", cm_message_info->id);
    } else {
        dm_thing_manager->_code = cm_message_info->code;
        dm_thing_manager->_sub_thing_id = request_info->thing_id;
        dm_thing_manager->_message = cm_message_info->message;

        if (dm_thing_manager->_code == 200) {
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_topo_delete_success);
        } else {
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_topo_delete_fail);
        }
        free_sub_request_node(dm_thing_manager, request_info);
        dm_lite_free(request_info);
    }
}

static void dm_cm_register_uri_handler_permit_join(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    message_info_t** message_info = dm_thing_manager->_message_info;
    cm_abstract_t** cm = dm_thing_manager->_cm;
    char uri_buff[URI_MAX_LENGTH] = {0};
    cJSON* params_obj;
    cJSON* params_pk_obj;
    cJSON* params_time_obj;
    char* pk;
    int time_s;

    params_obj = cJSON_Parse(cm_message_info->parameter);

    assert(params_obj);

    if (!params_obj) return;

    params_pk_obj = cJSON_GetObjectItem(params_obj, "productKey");
    params_time_obj = cJSON_GetObjectItem(params_obj, "time");

    assert(params_pk_obj && params_time_obj);

    if (!params_pk_obj || !params_time_obj) return;

    pk = params_pk_obj->valuestring;
    time_s = params_time_obj->valueint;

    if (pk) {
        strcpy(dm_thing_manager->_permit_pk, pk);
    } else {
        memset(dm_thing_manager->_permit_pk, 0, sizeof(dm_thing_manager->_permit_pk));
    }

    dm_thing_manager->_permit_time_ms = time_s * 1000;
    dm_thing_manager->_uptime_ms = HAL_UptimeMs();

    dm_log_debug("permit_pk:%s, time:%d", dm_thing_manager->_permit_pk ? dm_thing_manager->_permit_pk : "NULL", time_s);


    /* response to permit */
    (*message_info)->clear(message_info);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s_reply", dm_thing_manager->_product_key, dm_thing_manager->_device_name, string_method_name_sub_gw_permit);
    (*message_info)->set_product_key(message_info, dm_thing_manager->_product_key);
    (*message_info)->set_device_name(message_info, dm_thing_manager->_device_name);
    (*message_info)->set_uri(message_info, uri_buff);
    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_RESPONSE);
    (*message_info)->set_id(message_info, cm_message_info->id);
    (*message_info)->set_code(message_info, 200);
    (*message_info)->set_params_data(message_info, "{}");

    (*cm)->send(cm, message_info);
}

static void dm_cm_register_uri_handler_sub_login_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    request_info_t* request_info;
    thing_t** sub_thing;
    dm_thing_t* dm_thing;
#ifdef _PLATFORM_IS_LINUX_
    char file_name[DEVICE_ID_MAXLEN + 6] = {0};
    char* file_content = NULL;
#endif

    request_info = find_subdev_login_request_info_by_id(dm_thing_manager, cm_message_info->id);

    if (!request_info) {
        dm_log_err("cannot find node from subdev_login_request, request_id:%d", cm_message_info->id);
    } else {
        dm_thing_manager->_code = cm_message_info->code;
        dm_thing_manager->_message = cm_message_info->message;
        dm_thing_manager->_thing_id = dm_thing_manager->_sub_thing_id = request_info->thing_id;

        dm_thing = dm_thing_manager->_thing_id;
        if (dm_thing_manager->_code == 200) {
            if (dm_thing) dm_thing->_login = 1;
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_login_success);
        } else {
            if (dm_thing) dm_thing->_login = 0;
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_login_fail);
        }
        free_sub_request_node(dm_thing_manager, request_info);
        dm_lite_free(request_info);

        /* get tsl if need */
        sub_thing = dm_thing_manager->_sub_thing_id;

        if (!sub_thing || !*sub_thing) return;

        if ((*sub_thing)->get_property_number(sub_thing) == 0 && (*sub_thing)->get_service_number(sub_thing) == 0 && (*sub_thing)->get_event_number(sub_thing) == 0) {
            /* get tsl template. */
            dm_thing_manager->_thing_id = sub_thing;
            /* subscribe uris not related to tsl. */
            generate_subscribe_uri(dm_thing_manager, NULL, 1, 1, (*sub_thing)->return_product_key(sub_thing), (*sub_thing)->return_device_name(sub_thing));
#ifdef _PLATFORM_IS_LINUX_
            dm_snprintf(file_name, sizeof(file_name), string_tsl_file_pattern, (*sub_thing)->return_product_key(sub_thing), (*sub_thing)->return_device_name(sub_thing));
            file_content = read_file(file_name);
            if (file_content) {
                (*sub_thing)->set_tsl_string(sub_thing, file_content, strlen(file_content));
                dm_lite_free(file_content);
            } else
#endif /* _PLATFORM_IS_LINUX_ */
            {
                send_request_to_uri(dm_thing_manager, string_method_name_thing_tsl_get);
                return;
            }
        }
        /* subscribe subjects. */
        if (dm_thing_manager->_cloud_connected) generate_subscribe_uri(dm_thing_manager, sub_thing, 1, 0, NULL, NULL);
#ifdef LOCAL_CONN_ENABLE
        if (dm_thing_manager->_local_connected) generate_subscribe_uri(dm_thing_manager, sub_thing, 0, 0, NULL, NULL);
        IOT_CM_Add_Sub_Device((*sub_thing)->return_product_key(sub_thing), (*sub_thing)->return_device_name(sub_thing), NULL);
#endif
    }
}

static void dm_cm_register_uri_handler_sub_logout_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    request_info_t* request_info;
#if 0
    list_t** list;
#endif
    dm_thing_t* dm_thing;

    request_info = find_subdev_logout_request_info_by_id(dm_thing_manager, cm_message_info->id);
    if (!request_info) {
        dm_log_err("cannot find node from subdev_logout_request, request_id:%d", cm_message_info->id);
    } else {
        dm_thing_manager->_code = cm_message_info->code;
        dm_thing_manager->_message = cm_message_info->message;
        dm_thing_manager->_thing_id = dm_thing_manager->_sub_thing_id = request_info->thing_id;

        dm_thing = dm_thing_manager->_thing_id;
        if (dm_thing_manager->_code == 200) {
            if (dm_thing) dm_thing->_login = 0;
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_logout_success);
#if 0
            invoke_callback_list(dm_thing_manager, dm_callback_type_subthing_destroyed);

            list = dm_thing_manager->_sub_thing_list;
            (*list)->remove(list, dm_thing_manager->_thing_id);

            delete_object(dm_thing_manager->_thing_id);

            dm_thing_manager->_sub_thing_id = 0;
#endif
        } else {
            if (dm_thing) dm_thing->_login = 1;
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_logout_fail);
        }

        free_sub_request_node(dm_thing_manager, request_info);
        dm_lite_free(request_info);
    }
}

static void dm_cm_register_uri_handler_sub_register_reply(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    request_info_t* request_info;
    list_t** list;
    char subdev_secret[DEVICE_SECRET_MAXLEN] = {0};

    cJSON* register_response_data_obj;
    cJSON* register_response_data_item_obj;
    cJSON* device_secret_obj;
    cJSON* product_key_obj;
    cJSON* device_namet_obj;

    int index, array_size;

    request_info = find_subdev_register_request_info_by_id(dm_thing_manager, cm_message_info->id);

    if (!request_info) {
        dm_log_err("cannot find node from subdev_register_request, request_id:%d", cm_message_info->id);
    } else {
        dm_thing_manager->_code = cm_message_info->code;
        dm_thing_manager->_message = cm_message_info->message;
        dm_thing_manager->_thing_id = dm_thing_manager->_sub_thing_id = request_info->thing_id;

        if (dm_thing_manager->_code == 200) {
            register_response_data_obj = cJSON_Parse(cm_message_info->parameter);

            assert(register_response_data_obj && cJSON_IsArray(register_response_data_obj));

            if (register_response_data_obj == NULL || cJSON_IsArray(register_response_data_obj) == 0) {
                dm_log_err("UNABLE to resolve %s params format", string_method_name_sub_register);
                return;
            }

            array_size = cJSON_GetArraySize(register_response_data_obj);

            assert(array_size == 1);
            for (index = 0; index < array_size; ++index) {
                register_response_data_item_obj = cJSON_GetArrayItem(register_response_data_obj, index);
                assert(cJSON_IsObject(register_response_data_item_obj));

                product_key_obj = cJSON_GetObjectItem(register_response_data_item_obj, "productKey");

                device_namet_obj = cJSON_GetObjectItem(register_response_data_item_obj, "deviceName");

                assert(product_key_obj && device_namet_obj && strcmp(product_key_obj->valuestring, request_info->product_key) == 0 && strcmp(device_namet_obj->valuestring, request_info->device_name) == 0);

                device_secret_obj = cJSON_GetObjectItem(register_response_data_item_obj, "deviceSecret");

                assert(device_secret_obj);
                if (!device_secret_obj) goto exit;
#ifndef CJSON_STRING_ZEROCOPY
                strcpy(subdev_secret, device_secret_obj->valuestring);
#else
                strncpy(subdev_secret, device_secret_obj->valuestring, device_secret_obj->valuestring_length);
#endif
            }
            cJSON_Delete(register_response_data_obj);

            dm_thing_manager->_subdev_ds_from_register = subdev_secret;
            dm_thing_manager->_subdev_pk_from_register = request_info->product_key;
            dm_thing_manager->_subdev_dn_from_register = request_info->device_name;
            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_register_success);
            invoke_callback_list(dm_thing_manager, dm_callback_type_subthing_registered);

            send_subdev_topo_add_message(dm_thing_manager, request_info->product_key, request_info->device_name, subdev_secret, "hmacsha1");
        } else {
            thing_t** sub_thing;
            int is_sub_thing;
            dm_thing_manager->_subdev_pk_from_register = request_info->product_key;
            dm_thing_manager->_subdev_dn_from_register = request_info->device_name;

            sub_thing = dm_thing_manager_query_thing_id(dm_thing_manager, dm_thing_manager->_subdev_pk_from_register, dm_thing_manager->_subdev_dn_from_register, &is_sub_thing);

            if (sub_thing && is_sub_thing) {
                dm_thing_manager->_thing_id = dm_thing_manager->_sub_thing_id = sub_thing;
            } else {
                dm_thing_manager->_thing_id = dm_thing_manager->_sub_thing_id = NULL;
            }

            invoke_subdev_callback_list(dm_thing_manager, dm_subdev_callback_type_register_fail);
            invoke_callback_list(dm_thing_manager, dm_callback_type_subthing_destroyed);

            if (dm_thing_manager->_thing_id) {
                list = dm_thing_manager->_sub_thing_list;
                (*list)->remove(list, dm_thing_manager->_thing_id);

                delete_object(dm_thing_manager->_thing_id);
            }
            dm_thing_manager->_sub_thing_id = 0;
        }
exit:
        free_sub_request_node(dm_thing_manager, request_info);
        dm_lite_free(request_info);
    }
}

#endif /* SUBDEV_ENABLE */

#ifdef LOCAL_CONN_ENABLE
static void dm_cm_register_uri_handler_property_post(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;

    if (strstr(cm_message_info->URI, string_method_name_property_post_reply)) return;

    invoke_callback_list(dm_thing_manager, dm_callback_type_local_post);
}
#endif /* LOCAL_CONN_ENABLE */

static void dm_cm_register_uri_handler_normal_service(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    /* invoke callback funtions. */
    dm_thing_manager->_identifier = dm_thing_manager->_service_identifier_requested;
#ifdef SUBDEV_ENABLE
    if (0 == dm_thing_manager->_is_subthing) {
        invoke_callback_list(dm_thing_manager, dm_callback_type_service_requested);
    }
#else
    invoke_callback_list(dm_thing_manager, dm_callback_type_service_requested);
#endif
}

static void dm_cm_register_uri_handler_event_response(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{

}

static dm_cm_register_uri_handler_fp_t dm_cm_register_uri_handler_choose(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_cm_register_uri_handler_fp_t dm_cm_register_uri_handler_fp = NULL;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;

    if (!cm_message_info->URI) return NULL;

    if (strstr(cm_message_info->URI, string_method_name_thing_tsl_get_reply)) { /* thing/dsltemplate/get_reply match */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_tsl_get_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_property_set)) { /* thing/service/property/set match */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_property_set;
    } else if (strstr(cm_message_info->URI, string_method_name_property_get)) { /* thing/service/property/get match */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_property_get;
    } else if (strstr(cm_message_info->URI, string_method_name_thing_enable)) { /* thing/enable match */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_thing_enable;
    } else if (strstr(cm_message_info->URI, string_method_name_thing_disable)) { /* thing/disable match */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_thing_disable;
#ifdef RRPC_ENABLED
    } else if (strstr(cm_message_info->URI, string_method_name_rrpc_request) || strstr(cm_message_info->URI, "/ext/rrpc/")) { /* rrpc/request match */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_rrpc_request;
#endif /* RRPC_ENABLED */
#ifdef EXTENDED_INFO_ENABLED
    } else if (strstr(cm_message_info->URI, string_method_name_deviceinfo_update_reply)) { /* deviceinfo/update_reply. */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_deviceinfo_update_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_deviceinfo_delete_reply)) { /* deviceinfo/delete_reply. */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_deviceinfo_delete_reply;
#endif /* EXTENDED_INFO_ENABLED*/
#ifdef SUBDEV_ENABLE
    } else if (strstr(cm_message_info->URI, string_method_name_topo_add_reply)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_topo_add_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_sub_login_reply)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_sub_login_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_sub_logout_reply)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_sub_logout_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_topo_del_reply)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_topo_delete_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_sub_register_reply)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_sub_register_reply;
    } else if (strstr(cm_message_info->URI, string_method_name_sub_gw_permit)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_permit_join;
#endif
#ifdef LOCAL_CONN_ENABLE
    } else if (strstr(cm_message_info->URI, string_method_name_property_post)) {
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_property_post;
#endif
    } else if (cm_message_info->message_type == IOTX_CM_MESSAGE_REQUEST) { /* normal service. */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_normal_service;
    } else if (cm_message_info->message_type == IOTX_CM_MESSAGE_RESPONSE) { /* event resonse. */
        dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_event_response;
    }

    return dm_cm_register_uri_handler_fp;
}

static void dm_cm_register_handler_clear_message_info(iotx_cm_message_info_t* _cm_message_info)
{
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;

    dm_lite_free(cm_message_info->URI);
    cm_message_info->URI = NULL;
		
    if (cm_message_info->method) {
        dm_lite_free(cm_message_info->method);
        cm_message_info->method = NULL;
    }
    if (cm_message_info->parameter) {
        dm_lite_free(cm_message_info->parameter);
        cm_message_info->parameter = NULL;
    }
    if (cm_message_info->message) {
        dm_lite_free(cm_message_info->message);
        cm_message_info->message = NULL;
    }
}

static void dm_cm_register_handler_message_type_process(dm_thing_manager_t* _dm_thing_manager, iotx_cm_message_info_t* _cm_message_info)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    iotx_cm_message_info_t* cm_message_info = _cm_message_info;
    thing_t** thing = dm_thing_manager->_thing_id;

	if (!thing) return;
	
    if (cm_message_info->message_type == IOTX_CM_MESSAGE_REQUEST) {
        dm_thing_manager->_request_id = cm_message_info->id;

        /* find service id trigged. */
        dm_thing_manager->_service_identifier_requested = NULL;
        service_iterator(thing, find_and_set_service_input, dm_thing_manager, cm_message_info, thing);
#ifndef LOCAL_CONN_ENABLE
        assert(dm_thing_manager->_service_identifier_requested);
#endif
#ifdef LOCAL_CONN_ENABLE
        if (dm_thing_manager->_conn_ctx == NULL) {
#endif
            if (dm_thing_manager->_service_identifier_requested == NULL) {
                dm_log_err("method NOT match of service requested");
                return;
            }
#ifdef LOCAL_CONN_ENABLE
        }
#endif
    } else if (cm_message_info->message_type == IOTX_CM_MESSAGE_RAW) {
        dm_thing_manager->_request_id = cm_message_info->id;
        dm_thing_manager->_raw_data = cm_message_info->parameter;
        dm_thing_manager->_raw_data_length = cm_message_info->parameter_length;

        /* invoke callback funtions. */
#ifdef SUBDEV_ENABLE
        if (dm_thing_manager->_thing_id) {
            invoke_callback_list(dm_thing_manager, dm_callback_type_raw_data_arrived);
        } else {
            assert(0);
        }

#else
        invoke_callback_list(dm_thing_manager, dm_callback_type_raw_data_arrived);
#endif

        dm_thing_manager->_raw_data = NULL;
        dm_thing_manager->_raw_data_length = 0;

        return;
    } else if (cm_message_info->message_type == IOTX_CM_MESSAGE_RESPONSE) {
        if (!strstr(cm_message_info->URI, string_method_name_thing_tsl_get_reply)) {
            dm_thing_manager->_response_id_received = cm_message_info->id;
            dm_thing_manager->_code = cm_message_info->code;
            dm_thing_manager->_response_message = cm_message_info->message;
            /* invoke callback funtions. */
            invoke_callback_list(dm_thing_manager, dm_callback_type_response_arrived);
            dm_thing_manager->_response_message = NULL;
        }
    }
}

static void dm_cm_register_handler(iotx_cm_send_peer_t* _source, iotx_cm_message_info_t* _msg, void* user_data)
{
    dm_thing_manager_t* dm_thing_manager = user_data;
    iotx_cm_send_peer_t* iotx_cm_send_peer = _source;
    iotx_cm_message_info_t* cm_message_info = _msg;

    dm_cm_register_uri_handler_fp_t dm_cm_register_uri_handler_fp = NULL;

#ifdef LOCAL_CONN_ENABLE
    dm_thing_manager->_conn_ctx = cm_message_info->conn_ctx;
#endif

    assert(dm_thing_manager && iotx_cm_send_peer && cm_message_info);

    print_iotx_cm_message_info(iotx_cm_send_peer, cm_message_info);

    /* find thing id. */
#ifdef SUBDEV_ENABLE
    check_update_subdev_permit_time(dm_thing_manager);
    dm_thing_manager->_thing_id = query_thing_id_by_uri(dm_thing_manager, cm_message_info->URI);
#else
    dm_thing_manager->_thing_id = NULL;
    list_iterator(dm_thing_manager->_local_thing_list, find_thing_via_product_key_and_device_name, dm_thing_manager, iotx_cm_send_peer);
#endif
    if (dm_thing_manager->_thing_id == NULL && strstr(cm_message_info->URI, string_method_name_thing_tsl_get_reply) == NULL) {
        dm_log_err("thing id NOT match");
        return;
    }

#ifdef SUBDEV_ENABLE
    dm_thing_manager->_param_data = cm_message_info->parameter;
    dm_thing_manager->_param_data_length = cm_message_info->parameter_length;
    if (dm_thing_manager->_thing_id || strstr(cm_message_info->URI, string_method_name_thing_tsl_get_reply)) {
        invoke_callback_list(dm_thing_manager, dm_callback_type_linkkit_data_arrived);
    } else {
        assert(0);
    }

    dm_thing_manager->_param_data = NULL;
    dm_thing_manager->_param_data_length = 0;
#endif

    dm_cm_register_handler_message_type_process(dm_thing_manager, cm_message_info);

    dm_cm_register_uri_handler_fp = dm_cm_register_uri_handler_choose(dm_thing_manager, cm_message_info);

    if (dm_cm_register_uri_handler_fp) dm_cm_register_uri_handler_fp(dm_thing_manager, cm_message_info);

    dm_cm_register_handler_clear_message_info(cm_message_info);
}

static void send_request_to_uri(void* _dm_thing_manager, const char* _uri)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    const char* uri = _uri;
    message_info_t** message_info = dm_thing_manager->_message_info;
    cm_abstract_t** cm = dm_thing_manager->_cm;
    thing_t** thing = dm_thing_manager->_thing_id;
    char uri_buff[URI_MAX_LENGTH] = {0};
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char* p;
    int ret;

    assert(dm_thing_manager && uri && message_info && cm);

    get_product_key_device_name(product_key, device_name, thing, dm_thing_manager);

    strcpy(method_buff, uri);

    /* subtitute '/' by '.' */
    do {
        p = strchr(method_buff, '/');
        if (p) *p = '.';
    } while (p);

    dm_thing_manager->_method = method_buff;
    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", product_key, device_name, uri);

    clear_and_set_message_info(message_info, dm_thing_manager);

    (*message_info)->set_uri(message_info, uri_buff);
    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    ret = (*message_info)->serialize_to_payload_request(message_info);

    if (ret == -1) {
        dm_log_err("serialize_to_payload_request FAIL");
        return;
    }

    (*cm)->send(cm, message_info);
}

#ifdef LOCAL_CONN_ENABLE
static void dm_cm_local_conn_register_handler(iotx_cm_send_peer_t* _source, iotx_cm_message_info_t* _msg, void* user_data)
{
    dm_thing_manager_t* dm_thing_manager = user_data;
    iotx_cm_send_peer_t* iotx_cm_send_peer = _source;
    iotx_cm_message_info_t* cm_message_info = _msg;
    message_info_t** message_info = dm_thing_manager->_message_info;
    cm_abstract_t** cm = dm_thing_manager->_cm;
    char ip_addr[24];
    int port;
    int id;
    int code;
    char response_data[256] = {0};
    char* product_key;
    char* device_name;
    char* uri;

    assert(dm_thing_manager && cm_message_info && cm && *cm);

    id = cm_message_info->id;
    dm_thing_manager->_conn_ctx = cm_message_info->conn_ctx;

    print_iotx_cm_message_info(iotx_cm_send_peer, cm_message_info);

    if (strstr(cm_message_info->URI, string_dev_core_service_dev)) {
        HAL_Wifi_Get_IP(ip_addr, NULL);
        port = 5683;
        code = 200;
        product_key = dm_thing_manager->_product_key;
        device_name = dm_thing_manager->_device_name;

        dm_snprintf(response_data, sizeof(response_data), "{\"deviceModel\":{\"profile\":{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"addr\":\"%s\",\"port\":%d}}}",
                    product_key, device_name, ip_addr, port);

        clear_and_set_message_info(message_info, dm_thing_manager);
        (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_RESPONSE);
        uri = cm_message_info->URI;
        (*message_info)->set_uri(message_info, uri);
        (*message_info)->set_id(message_info, id);
        (*message_info)->set_code(message_info, code);
        (*message_info)->set_params_data(message_info, (char*)response_data);

        (*cm)->send(cm, message_info);
    }

    dm_cm_register_handler_clear_message_info(cm_message_info);
}
#endif
static void* dm_thing_manager_ctor(void* _self, va_list* params)
{
    dm_thing_manager_t* self = _self;
    cm_abstract_t** cm;
    handle_dm_callback_fp_t callback_func;
    list_t** list;

    self->_name = va_arg(*params, char*);
    self->_get_tsl_from_cloud = va_arg(*params, int);
    callback_func = va_arg(*params, void*);
    self->_cloud_domain = va_arg(*params, int);

    assert(self->_name);
	
    self->_local_thing_list = new_object(DM_SLIST_CLASS, string_local_thing_list);
    self->_local_thing_name_list = new_object(DM_SLIST_CLASS, string_local_thing_name_list);
    self->_sub_thing_list = new_object(DM_SLIST_CLASS, string_sub_thing_list);
#ifdef SUBDEV_ENABLE
    self->_sub_thing_name_list = new_object(DM_SLIST_CLASS, string_sub_thing_name_list);
    self->_subdev_callback_list = new_object(DM_SLIST_CLASS, string_subdev_callback_list);
    self->_sub_thing_cnt = 0;
    self->_sub_request_list = new_object(DM_SLIST_CLASS, string_subdev_request_list);
#endif
    self->_callback_list = new_object(DM_SLIST_CLASS, string_callback_list);
    self->_service_property_get_identifier_list = new_object(DM_SLIST_CLASS, string_service_property_get_identifier_list);
    self->_local_thing_id = 0;
    self->_thing_id = NULL;
    self->_message_info = new_object(DM_CM_MSG_INFO_CLASS);
    self->_dm_version = DM_REQUEST_VERSION_STRING;
    self->_id = 0;
    self->_method = NULL;
    self->_cm = new_object(DM_CM_IMPL_CLASS);
    self->_cm_event_handle_func_fp = dm_cm_event_handler;
    self->_cm_register_func_fp = dm_cm_register_handler;
#ifdef LOCAL_CONN_ENABLE
    self->_cm_local_conn_register_func_fp = dm_cm_local_conn_register_handler;
#endif
    self->_cloud_connected = 0;
    self->_property_identifier_set = NULL;
    self->_destructing = 0;

    list = (list_t**)self->_callback_list;
    if (callback_func) list_insert(list, callback_func);

    /* get relative information. */
    HAL_GetProductKey(self->_product_key);
    HAL_GetDeviceName(self->_device_name);
    HAL_GetDeviceSecret(self->_device_secret);
    HAL_GetDeviceID(self->_device_id);

    assert(self->_product_key && self->_device_name && self->_device_secret && self->_device_id);

    cm = self->_cm;

    if (FAIL_RETURN == (*cm)->init(cm, self->_product_key, self->_device_name, self->_device_secret, self->_device_id, self->_cm_event_handle_func_fp, self, self->_cloud_domain)) {
        dm_log_err("cm init failed");
    } else {
#ifdef LOCAL_CONN_ENABLE
        (*cm)->add_service(cm, (char*)string_dev_core_service_dev, self->_cm_local_conn_register_func_fp, self, 0);
#endif
    }

    cJSON_Hooks hook = {
        dm_lite_malloc,
        dm_lite_free_func,
    };

    cJSON_InitHooks(&hook);

#if (defined SUBDEV_ENABLE) && (0)
    /* temp for test. */
    self->_permit_time_ms = 200000;
    self->_uptime_ms = HAL_UptimeMs();
#endif

	self->_usercall_mutex = NULL;
	self->_usercall_mutex = HAL_MutexCreate();
	g_dm_thing_manager_usercall_mutex = self->_usercall_mutex;
	g_dm_thing_manager_callback_mutex = HAL_MutexCreate();
	assert(g_dm_thing_manager_callback_mutex && g_dm_thing_manager_usercall_mutex);
    return self;
}

static void* dm_thing_manager_dtor(void* _self)
{
    dm_thing_manager_t* self = _self;
    list_t** list;

    self->_destructing = 1;

    local_thing_list_iterator(self, free_list_thing, self);

    list = self->_local_thing_name_list;
    list_iterator(list, free_list_string, self);

    list = self->_service_property_get_identifier_list;
    list_iterator(list, free_list_string, self);

    assert(self->_local_thing_list && self->_local_thing_name_list && self->_sub_thing_list && self->_callback_list && self->_message_info);

#ifdef SUBDEV_ENABLE
    sub_thing_list_iterator(self, free_list_thing, self);
    list = self->_sub_thing_name_list;
    list_iterator(list, free_list_string, self);
    delete_object(self->_sub_thing_name_list);
    delete_object(self->_subdev_callback_list);

    list = self->_sub_request_list;
    list_iterator(list, free_list_string, self);
    delete_object(self->_sub_request_list);
#endif

    delete_object(self->_local_thing_list);
    delete_object(self->_local_thing_name_list);
    delete_object(self->_service_property_get_identifier_list);
    delete_object(self->_sub_thing_list);
    delete_object(self->_callback_list);
    delete_object(self->_message_info);
    delete_object(self->_cm);

    self->_dm_version = NULL;
    self->_id = 0;
    self->_method = NULL;

	HAL_MutexDestroy(self->_usercall_mutex);
	self->_usercall_mutex = NULL;
	g_dm_thing_manager_usercall_mutex = NULL;

	HAL_MutexDestroy(g_dm_thing_manager_callback_mutex);
	g_dm_thing_manager_callback_mutex = NULL;
    return self;
}

static void generate_thing_event_service_subscribe_uri(void* _item, int index, va_list* params)
{
    event_t* event = NULL;
    service_t* service = NULL;
    dm_thing_manager_t* dm_thing_manager;
    thing_t** thing;
    cm_abstract_t** cm;

    char method_buff[METHOD_MAX_LENGTH] = {0};
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char delimeter[] = {'.', 0};
    char* uri_buff;
    char* p;
    char* type;
    int cloud;
    char* topic_prefix;

    char normal_service_prefix_pattern[] = "/sys/%s/%s";
#if defined(RRPC_ENABLED) && defined(RRPC_NEW)
    char rrpc_service_prefix_pattern[] = "/ext/rrpc/+/sys/%s/%s";
    int rrpc = 1;
#endif

	if (!_item) return;
	
    dm_thing_manager = va_arg(*params, void*);
    thing = va_arg(*params, void*);
    uri_buff = va_arg(*params, char*);
    type = va_arg(*params, char*);
    cloud = va_arg(*params, int);

    if (strcmp(type, string_event) == 0) {
        event = _item;
    } else if (strcmp(type, string_service) == 0) {
        service = _item;
    } else {
        assert(0);
    }
    assert(dm_thing_manager && uri_buff && _item);

    get_product_key_device_name(product_key, device_name, thing, dm_thing_manager);

    topic_prefix = normal_service_prefix_pattern;
#if defined(RRPC_ENABLED) && defined(RRPC_NEW)
sub_uri:
#endif
    dm_sprintf(uri_buff, topic_prefix, product_key, device_name);

    dm_sprintf(method_buff, (cloud && event) ? "%s_reply" : "%s", event ? event->method : service->method);

    p = strtok(method_buff, delimeter);
    while (p) {
        dm_sprintf(uri_buff + strlen(uri_buff), "/%s", p);
        p = strtok(NULL, delimeter);
    }

    cm = dm_thing_manager->_cm;
    if (cloud) {
        (*cm)->regist(cm, uri_buff, dm_thing_manager->_cm_register_func_fp, dm_thing_manager);
#if defined(RRPC_ENABLED) && defined(RRPC_NEW)
        if (service && rrpc) {
            topic_prefix = rrpc_service_prefix_pattern;
            rrpc = 0;
            goto sub_uri;
        }
#endif
#ifdef LOCAL_CONN_ENABLE
    } else {
        (*cm)->add_service(cm, uri_buff, dm_thing_manager->_cm_register_func_fp, dm_thing_manager, 1);
#endif
    }
}

static void generate_subscribe_uri(void* _dm_thing_manager, void* _thing, int cloud, int subdev,char* pk, char* dn)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    thing_t** thing = _thing;
    cm_abstract_t** cm = dm_thing_manager->_cm;

    int index;
    const char** uri;
    char uri_buff[URI_MAX_LENGTH] = {0};
    const char* cloud_uri_array[] = {string_method_name_thing_tsl_get_reply, string_method_name_down_raw, string_method_name_up_raw_reply,
#ifdef EXTENDED_INFO_ENABLED
                                     string_method_name_deviceinfo_update_reply, string_method_name_deviceinfo_delete_reply,
#endif /* EXTENDED_INFO_ENABLED*/
#if defined(RRPC_ENABLED) && !defined(RRPC_NEW)

                                     string_method_name_rrpc_request_plus,
#endif /* RRPC_ENABLED */
                                    };
#ifdef SUBDEV_ENABLE
    const char* cloud_gw_uri_array[] = {string_method_name_topo_add_reply, string_method_name_topo_del_reply, string_method_name_sub_register_reply, string_method_name_sub_gw_permit};
#endif
    const char* common_uri_array[] = {string_method_name_thing_enable, string_method_name_thing_delete, string_method_name_thing_disable,};

    if (dm_thing_manager->_cloud_connected == 0) {
        dm_log_err("subscribe not allowed when cloud not connected");
        return;
    }

    if (thing) {
        event_iterator(thing, generate_thing_event_service_subscribe_uri, dm_thing_manager, thing, uri_buff, string_event, cloud);
        service_iterator(thing, generate_thing_event_service_subscribe_uri, dm_thing_manager, thing, uri_buff, string_service, cloud);
    } else {
        for (index = 0; index < sizeof(common_uri_array) / sizeof(char*); ++index) {
            uri = common_uri_array + index;
            dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/", pk, dn);
            strcat(uri_buff, *uri);

            if (cloud) {
                (*cm)->regist(cm, uri_buff, dm_thing_manager->_cm_register_func_fp, dm_thing_manager);
#ifdef LOCAL_CONN_ENABLE
            } else {
                (*cm)->add_service(cm, uri_buff, dm_thing_manager->_cm_register_func_fp, dm_thing_manager, 1);
#endif
            }
        }
        if (cloud) {
            for (index = 0; index < sizeof(cloud_uri_array) / sizeof(char*); ++index) {
                uri = cloud_uri_array + index;
                dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/", pk, dn);
                strcat(uri_buff, *uri);

                (*cm)->regist(cm, uri_buff, dm_thing_manager->_cm_register_func_fp, dm_thing_manager);
            }
#ifdef SUBDEV_ENABLE
            if (!subdev) {
                for (index = 0; index < sizeof(cloud_gw_uri_array) / sizeof(char*); ++index) {
                    uri = cloud_gw_uri_array + index;
                    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/", pk, dn);
                    strcat(uri_buff, *uri);

                    (*cm)->regist(cm, uri_buff, dm_thing_manager->_cm_register_func_fp, dm_thing_manager);
                }
            }
#endif
        }
    }
}

static void* dm_thing_manager_generate_new_local_thing(void* _self, const char* tsl, int tsl_len)
{
    dm_thing_manager_t* self = _self;
    thing_t** thing;
    list_t** list;
    char* thing_name;
    size_t name_size;
    char hal_product_key[PRODUCT_KEY_MAXLEN] = {0};
    char hal_device_name[DEVICE_NAME_MAXLEN] = {0};
    char* device_name;
    char* product_key;

    assert(tsl);

    name_size = sizeof(DM_LOCAL_THING_NAME_PATTERN) + 2;
    thing_name = (char*)dm_lite_calloc(1, name_size);

    memset(thing_name, 0, name_size);
    dm_sprintf(thing_name, DM_LOCAL_THING_NAME_PATTERN, self->_local_thing_id++);

    thing = (thing_t**)new_object(DM_THING_CLASS, thing_name);

    if (0 == (*thing)->set_tsl_string(thing, tsl, tsl_len)) {
        list = self->_local_thing_list;
        list_insert(list, thing);

        list = self->_local_thing_name_list;
        list_insert(list, thing_name);

        self->_thing_id = thing;

        dm_log_debug("new thing created@%p", self->_thing_id);

        if (HAL_GetProductKey(hal_product_key) <= 0 || (HAL_GetDeviceName(hal_device_name) <= 0)) {
            dm_log_err("get HAL DeviceInfo failed!");
        }

        product_key = (*thing)->return_product_key(thing);
        device_name = (*thing)->return_device_name(thing);

        if (!product_key || strcmp(product_key, hal_product_key)) {
            dm_log_err("tsl_pk(%s) != hal_pk(%s)", product_key ? product_key : "NULL", hal_product_key);
        }
        if (!device_name || strcmp(device_name, hal_device_name)) {
            dm_log_err("tsl_dn(%s) != hal_dn(%s),using hal_dn", device_name ? device_name : "NULL", hal_device_name);
            (*thing)->set_device_name(thing, hal_device_name);
        }

        /* subscribe subjects. */
        if (self->_cloud_connected) {
            local_thing_list_iterator(self, change_local_thing_login, 1);

            generate_subscribe_uri(self, thing, 1, 0, NULL, NULL);
        }
#ifdef LOCAL_CONN_ENABLE
        if (self->_local_connected) generate_subscribe_uri(self, thing, 0, 0, NULL, NULL);
#endif
        /* invoke callback funtions. */
        invoke_callback_list(self, dm_callback_type_new_thing_created);
    } else {
        delete_object(thing);
        thing = NULL;
    }

    return thing;
}

static int dm_thing_manager_add_callback_function(void* _self, handle_dm_callback_fp_t callback_func)
{
    dm_thing_manager_t* self = _self;
    list_t** list;

    list = (list_t**)self->_callback_list;

    assert(list);

    if (callback_func) list_insert(list, callback_func);

    return 0;
}

static void free_list_thing(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    (void)dm_thing_manager;

    assert(thing && *thing);

    delete_object(thing);
}

static void free_list_string(void* _string, va_list* params)
{
    void* string = _string;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    (void)dm_thing_manager;

    assert(string && dm_thing_manager);

    dm_lite_free(string);

    string = NULL;
}

static void get_service_input_output_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing && (*thing)->get_service_input_output_data_value_by_identifier);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = (*thing)->get_service_input_output_data_value_by_identifier(thing, dm_thing_manager->_identifier,
                                                                                             dm_thing_manager->_get_value, &dm_thing_manager->_get_value_str);
    }
}

static void set_service_input_output_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing && (*thing)->set_service_input_output_data_value_by_identifier);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = (*thing)->set_service_input_output_data_value_by_identifier(thing, dm_thing_manager->_identifier,
                                                                                             dm_thing_manager->_set_value, dm_thing_manager->_set_value_str);
    }
}

static void get_event_output_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing && (*thing)->get_event_value_by_identifier);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = (*thing)->get_event_value_by_identifier(thing, dm_thing_manager->_identifier, dm_thing_manager->_get_value, &dm_thing_manager->_get_value_str);
    }
}

static void set_event_output_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing && (*thing)->set_event_value_by_identifier);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = (*thing)->set_event_value_by_identifier(thing, dm_thing_manager->_identifier,
                                                                         dm_thing_manager->_set_value, dm_thing_manager->_set_value_str);
    }
}

static void invoke_callback_func(void* _callback_fp, va_list* params)
{
    handle_dm_callback_fp_t callback_fp = _callback_fp;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && callback_fp);

    if (callback_fp && dm_thing_manager) {
        callback_fp(dm_thing_manager->_callback_type, dm_thing_manager->_thing_id, dm_thing_manager->_identifier,
                    dm_thing_manager->_callback_type != dm_callback_type_response_arrived ? dm_thing_manager->_request_id : dm_thing_manager->_response_id_received,
                    dm_thing_manager->_raw_data, dm_thing_manager->_raw_data_length,
                    dm_thing_manager->_code, dm_thing_manager->_response_message,
                    dm_thing_manager->_param_data, dm_thing_manager->_param_data_length
#ifdef SUBDEV_ENABLE
                    , dm_thing_manager->_subdev_pk_from_register, dm_thing_manager->_subdev_dn_from_register, dm_thing_manager->_subdev_ds_from_register
#endif
                    );
    }
}

#ifdef SUBDEV_ENABLE
static void invoke_subdev_callback_func(void* _subdev_callback_fp, va_list* params)
{
    handle_dm_subdev_callback_fp_t subdev_callback_fp = _subdev_callback_fp;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && subdev_callback_fp);

    if (subdev_callback_fp && dm_thing_manager) {
        subdev_callback_fp(dm_thing_manager->_subcallback_type, dm_thing_manager->_sub_thing_id,
                           dm_thing_manager->_id, dm_thing_manager->_code, dm_thing_manager->_message,
                           dm_thing_manager->_subdev_pk_from_register,
                           dm_thing_manager->_subdev_dn_from_register,
                           dm_thing_manager->_subdev_ds_from_register);
    }
}
#endif

static void callback_list_iterator(const void* _self, handle_fp_t handle_fp)
{
    const dm_thing_manager_t* self = _self;
    list_t** list = (list_t**)self->_callback_list;

    assert((*list)->iterator);
    list_iterator(list, handle_fp, self);
}
#ifdef SUBDEV_ENABLE
static void subdev_callback_list_iterator(const void* _self, handle_fp_t handle_fp)
{
    const dm_thing_manager_t* self = _self;
    list_t** list = (list_t**)self->_subdev_callback_list;

    assert((*list)->iterator);
    list_iterator(list, handle_fp, self);
}
#endif

static void set_property_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing && (*thing)->set_property_value_by_identifier);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = (*thing)->set_property_value_by_identifier(thing, dm_thing_manager->_identifier, dm_thing_manager->_set_value, dm_thing_manager->_set_value_str);

        /* invoke callback funtions. */
        if (strchr(dm_thing_manager->_identifier, '.') == NULL && strchr(dm_thing_manager->_identifier, '[') == NULL && strchr(dm_thing_manager->_identifier, ']') == NULL ) {
            if (dm_thing_manager->_ret == 0) invoke_callback_list(dm_thing_manager, dm_callback_type_property_value_set);
        }
    }
}

static void get_property_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing && (*thing)->get_property_value_by_identifier);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = (*thing)->get_property_value_by_identifier(thing, dm_thing_manager->_identifier, dm_thing_manager->_get_value, &dm_thing_manager->_get_value_str);
    }
}

static void local_thing_list_iterator(const void* _self, handle_fp_t handle_fp, ...)
{
    va_list params;
    const dm_thing_manager_t* self = _self;
    list_t** list = (list_t**)self->_local_thing_list;

    assert((*list)->iterator);

    va_start(params, handle_fp);
    (*list)->iterator(list, handle_fp, &params);
    va_end(params);
}

static int dm_thing_manager_set_thing_property_value(void* _self, const void* thing_id, const void* identifier,
                                                     const void* value, const char* value_str)
{
    dm_thing_manager_t* self = _self;

    assert(thing_id && identifier && (value || value_str));

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_set_value = (void*)value;
    self->_set_value_str = (char*)value_str;
    self->_ret = -1;

    local_thing_list_iterator(self, set_property_value, self);

#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, set_property_value, self);
    }
#endif
    return self->_ret;
}

static int dm_thing_manager_get_thing_property_value(void* _self, const void* thing_id, const void* identifier,
                                                     void* value, char** value_str)
{
    dm_thing_manager_t* self = _self;

    assert(thing_id && identifier && (value || value_str));

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_get_value = value;
    self->_ret = -1;
    self->_get_value_str = NULL;

    local_thing_list_iterator(self, get_property_value, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, get_property_value, self);
    }
#endif
    if (value_str) *value_str = self->_get_value_str;

    return self->_ret;
}

static int dm_thing_manager_set_thing_event_output_value(void* _self, const void* thing_id, const void* identifier,
                                                         const void* value, const char* value_str)
{
    dm_thing_manager_t* self = _self;

    assert(thing_id && identifier && (value || value_str));

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_set_value = (void*)value;
    self->_set_value_str = (char*)value_str;
    self->_ret = -1;

    local_thing_list_iterator(self, set_event_output_value, self);

#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, set_event_output_value, self);
    }
#endif
    return self->_ret;
}

static int dm_thing_manager_get_thing_event_output_value(void* _self, const void* thing_id, const void* identifier,
                                                         void* value, char** value_str)
{
    dm_thing_manager_t* self = _self;

    assert(thing_id && identifier && (value || value_str));

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_get_value = value;
    self->_ret = -1;

    local_thing_list_iterator(self, get_event_output_value, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, get_event_output_value, self);
    }
#endif

    if (value_str) *value_str = self->_get_value_str;

    return self->_ret;
}

static int dm_thing_manager_get_thing_service_input_value(void* _self, const void* thing_id, const void* identifier,
                                                          void* value, char** value_str)
{
    dm_thing_manager_t* self = _self;

    assert(thing_id && identifier && (value || value_str));

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_get_value = value;
    self->_ret = -1;

    local_thing_list_iterator(self, get_service_input_output_value, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, get_service_input_output_value, self);
    }
#endif
    if (value_str) *value_str = self->_get_value_str;

    return self->_ret;
}

static int dm_thing_manager_get_thing_service_output_value(void* _self, const void* thing_id, const void* identifier,
                                                           void* value, char** value_str)
{
    dm_thing_manager_t* self = _self;

    return dm_thing_manager_get_thing_service_input_value(self, thing_id, identifier, value, value_str);
}

static int dm_thing_manager_set_thing_service_output_value(void* _self, const void* thing_id, const void* identifier,
                                                           const void* value, const char* value_str)
{
    dm_thing_manager_t* self = _self;

    assert(thing_id && identifier && (value || value_str));

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_set_value = (void*)value;
    self->_set_value_str = (char*)value_str;
    self->_ret = -1;

    local_thing_list_iterator(self, set_service_input_output_value, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, set_service_input_output_value, self);
    }
#endif

    return self->_ret;
}

static int install_lite_property_to_message_info(dm_thing_manager_t* _thing_manager, message_info_t** _message_info, lite_property_t* _lite_property)
{
    lite_property_t* lite_property = _lite_property;
    message_info_t** message_info = _message_info;
    dm_thing_manager_t* dm_thing_manager = _thing_manager;
    thing_t** thing = dm_thing_manager->_thing_id;
    char* p = NULL;

    char* q = NULL;
    dm_thing_t* dm_thing;
    size_t params_buffer_len = 0;
    size_t params_val_len = 0;
    int ret, i;
    char property_key_value_buff[PROPERTY_KEY_VALUE_BUFF_MAX_LENGTH] = {0};

    ret = (*thing)->get_lite_property_value(thing, lite_property, NULL, &dm_thing_manager->_get_value_str);

    if ((lite_property->data_type.type == data_type_type_text || lite_property->data_type.type == data_type_type_date) && dm_thing_manager->_get_value_str) {
        p = dm_lite_calloc(1, strlen(dm_thing_manager->_get_value_str) + 3);
        assert(p);

        dm_sprintf(p, "\"%s\"", dm_thing_manager->_get_value_str);
        dm_thing_manager->_get_value_str = p;
    }

    if (ret != 0 && lite_property->data_type.type == data_type_type_array) {
        dm_thing = (dm_thing_t*)thing;
        property_key_value_buff[0] = '[';
        property_key_value_buff[1] = '\0';
        params_buffer_len = sizeof(property_key_value_buff);
        for (i = 0; i < lite_property->data_type.value.data_type_array_t.size; i++) {
            dm_thing->_arr_index = i;
            dm_thing_manager->_get_value_str = NULL;
            ret = (*thing)->get_lite_property_value(thing, lite_property, NULL, &dm_thing_manager->_get_value_str);
            params_val_len = strlen(property_key_value_buff);

            if (ret != 0 || !dm_thing_manager->_get_value_str) {
                dm_thing_manager->_get_value_str = NULL;
            }
            if (lite_property->data_type.value.data_type_array_t.item_type == data_type_type_text) {
                dm_snprintf(property_key_value_buff + params_val_len, params_buffer_len - params_val_len, "\"%s\",", dm_thing_manager->_get_value_str ? dm_thing_manager->_get_value_str : "");
            } else {
                dm_snprintf(property_key_value_buff + params_val_len, params_buffer_len - params_val_len, "%s,", dm_thing_manager->_get_value_str ? dm_thing_manager->_get_value_str : "0");
            }
        }
        q = property_key_value_buff + strlen(property_key_value_buff) - 1;
        if (*q == '[') *(q+1) = ']';
        if (*q == ',') *(q) = ']';

        dm_thing->_arr_index = -1;

        (*message_info)->add_params_data_item(message_info, lite_property->identifier, property_key_value_buff);

        return 0;
    }

    if (ret == 0 && lite_property->identifier && dm_thing_manager->_get_value_str) {
        (*message_info)->add_params_data_item(message_info, lite_property->identifier, dm_thing_manager->_get_value_str);
    }

    if (p) dm_lite_free(p);

    return ret;
}

static void get_product_key_device_name(char* _product_key, char* _device_name, void* _thing, void* _dm_thing_manager)
{
    char* product_key = _product_key;
    char* device_name = _device_name;

    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    if (thing && *thing) {
        dm_thing_manager_install_product_key_device_name(dm_thing_manager, thing, product_key, device_name);
    } else {
        strcpy(product_key, dm_thing_manager->_product_key);
        strcpy(device_name, dm_thing_manager->_device_name);
    }
}

static void clear_and_set_message_info(message_info_t** _message_info, dm_thing_manager_t* _dm_thing_manager)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    message_info_t** message_info = _message_info;
    thing_t** thing;

    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    assert(message_info && *message_info && dm_thing_manager);

    if (message_info == NULL || *message_info == NULL || dm_thing_manager == NULL) return;

    thing = dm_thing_manager->_thing_id;

    get_product_key_device_name(product_key, device_name, thing, dm_thing_manager);
    if (0 == dm_thing_manager->_id) dm_thing_manager->_id++;

    (*message_info)->clear(message_info);

    (*message_info)->set_product_key(message_info, product_key);
    (*message_info)->set_device_name(message_info, device_name);
    (*message_info)->set_version(message_info, dm_thing_manager->_dm_version);
    dm_thing_manager->_current_id = dm_thing_manager->_id;
    (*message_info)->set_id(message_info, dm_thing_manager->_current_id);
    dm_thing_manager->_id = (dm_thing_manager->_id + 1) % INT32_MAX;
    (*message_info)->set_method(message_info, dm_thing_manager->_method);
#ifdef LOCAL_CONN_ENABLE
    (*message_info)->set_conn_ctx(message_info, dm_thing_manager->_conn_ctx);
    dm_thing_manager->_conn_ctx = NULL;
#endif
}

static void install_property_to_message_info(void* _item, int index, va_list* params)
{
    property_t* property = _item;
    lite_property_t* lite_property;
    lite_property_t* struct_lite_property;
    dm_thing_manager_t* dm_thing_manager;
    thing_t** thing;
    message_info_t** message_info;
    size_t params_buffer_len = 0;
    size_t params_val_len = 0;
    char* target_property_identifier;
    int ret, i;
    char property_key_value_buff[PROPERTY_KEY_VALUE_BUFF_MAX_LENGTH] = {0};
    char* p;
    char* q = NULL;
    dm_thing_manager = va_arg(*params, dm_thing_manager_t*);
    thing = va_arg(*params, thing_t**);
    message_info = va_arg(*params, message_info_t**);
    target_property_identifier = va_arg(*params, char*);

    index = index;

    assert(property && dm_thing_manager && thing && *thing && message_info && *message_info);

    lite_property = (lite_property_t*)property;

    /* post all value, or specify identifier. */
    if (property && (target_property_identifier == NULL || (property->identifier && strcmp(property->identifier, target_property_identifier) == 0 ))) {
        ret = install_lite_property_to_message_info(dm_thing_manager, message_info, lite_property);

        params_buffer_len = sizeof(property_key_value_buff);
        if (ret == -1) {
            if (property->data_type.type == data_type_type_struct) {
                property_key_value_buff[0] = '{';
                property_key_value_buff[1] = '\0';
                for (i = 0; i < property->data_type.data_type_specs_number; ++i) {
                    struct_lite_property = (lite_property_t*)property->data_type.specs + i;
                    ret = (*thing)->get_lite_property_value(thing, struct_lite_property, NULL, &dm_thing_manager->_get_value_str);
                    if (ret == 0 && dm_thing_manager->_get_value_str) {
                        params_val_len = strlen(property_key_value_buff);
                        p = property_key_value_buff + params_val_len - 1;

                        /* not the last item, chang from '}' to ','. */
                        if (p && *p == '}') *p = ',';

                        if (struct_lite_property->data_type.type == data_type_type_text && dm_thing_manager->_get_value_str) {
                            q = dm_lite_calloc(1, strlen(dm_thing_manager->_get_value_str) + 3);
                            assert(q);

                            dm_snprintf(q, strlen(dm_thing_manager->_get_value_str) + 3, "\"%s\"", dm_thing_manager->_get_value_str);
                            dm_thing_manager->_get_value_str = q;
                        }

                        dm_snprintf(property_key_value_buff + params_val_len, params_buffer_len - params_val_len, "\"%s\":%s}",
                                    struct_lite_property->identifier, dm_thing_manager->_get_value_str);

                        if (q) {dm_lite_free(q);q = NULL;};
                    }
                }

            }
            (*message_info)->add_params_data_item(message_info, property->identifier, property_key_value_buff);
        }
        dm_thing_manager->_ret = 0;
    }
}

static void handle_event_key_value(void* _item, int index, va_list* params)
{
    event_t* event;
    lite_property_t* lite_property;
    dm_thing_manager_t* dm_thing_manager;
    message_info_t** message_info;
    thing_t** thing;
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char uri_buff[URI_MAX_LENGTH] = {0};
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    char* p;
    int output_data_numb;
    int i;

    (void)index;
    dm_thing_manager = va_arg(*params, void*);
    message_info = va_arg(*params, message_info_t**);
    thing = va_arg(*params, thing_t**);

    assert(dm_thing_manager && message_info && *message_info && (*message_info)->set_message_type && thing && *thing);

    event = _item;
    if (strcmp(event->identifier, dm_thing_manager->_identifier) != 0) return;

    dm_thing_manager->_method = event->method;

    clear_and_set_message_info(message_info, dm_thing_manager);
#ifdef LOCAL_CONN_ENABLE
    (*message_info)->set_product_key(message_info, NULL);
    (*message_info)->set_device_name(message_info, NULL);
#endif
    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    strcpy(method_buff, event->method);
    /* subtitute '.' by '/' */
    do {
        p = strchr(method_buff, '.');
        if (p) *p = '/';
    } while (p);

    dm_thing_manager_install_product_key_device_name(dm_thing_manager, thing, product_key, device_name);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", product_key, device_name, method_buff);
    (*message_info)->set_uri(message_info, uri_buff);

    if (strcmp(dm_thing_manager->_method, string_thing_event_property_post) != 0) {
        output_data_numb = event->event_output_data_num;
        for (i = 0; i < output_data_numb; ++i) {
            lite_property = event->event_output_data + i;
            assert(lite_property);
            install_lite_property_to_message_info(dm_thing_manager, message_info, lite_property);
        }
        dm_thing_manager->_ret = 0;
    } else {
        property_iterator(thing, install_property_to_message_info, dm_thing_manager, thing,
                          dm_thing_manager->_message_info, dm_thing_manager->_property_identifier_post);
    }

}

static void install_service_property_get_to_message_info(void* _string, va_list* params)
{
    char* identifier_to_get = _string;
    dm_thing_manager_t* dm_thing_manager;
    thing_t** thing;
    message_info_t** message_info;

    dm_thing_manager = va_arg(*params, dm_thing_manager_t*);
    thing = va_arg(*params, thing_t**);
    message_info = va_arg(*params, message_info_t**);

    assert(identifier_to_get);

    property_iterator(thing, install_property_to_message_info, dm_thing_manager, thing, message_info, identifier_to_get);
}

static void handle_service_key_value(void* _item, int index, va_list* params)
{
    service_t* service;
    lite_property_t* lite_property;
    dm_thing_manager_t* dm_thing_manager;
    message_info_t** message_info;
    thing_t** thing;
    list_t** list;
    int output_data_numb;
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char uri_buff[URI_MAX_LENGTH] = {0};
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    char* p;
    int i;

#ifdef RRPC_NEW
    char rrpc_message_id[24] = {0};
#endif

	if (!_item) return;

    dm_thing_manager = va_arg(*params, void*);
    message_info = va_arg(*params, message_info_t**);
    thing = va_arg(*params, thing_t**);

    assert(dm_thing_manager && message_info && *message_info && thing && *thing);

    service = _item;
    list = dm_thing_manager->_service_property_get_identifier_list;

    if (service && service->identifier && strcmp(service->identifier, dm_thing_manager->_identifier) != 0) return;

    dm_thing_manager->_method = service->method;

    clear_and_set_message_info(message_info, dm_thing_manager);

    strcpy(method_buff, service->method);
    /* subtitute '.' by '/' */
    do {
        p = strchr(method_buff, '.');
        if (p) *p = '/';
    } while (p);

    dm_thing_manager_install_product_key_device_name(dm_thing_manager, thing, product_key, device_name);
#ifdef RRPC_ENABLED
    if (dm_thing_manager->_rrpc) {
#ifdef RRPC_NEW
        dm_lltoa(dm_thing_manager->_rrpc_message_id, rrpc_message_id, 10);
        dm_snprintf(uri_buff, sizeof(uri_buff), "/ext/rrpc/%s/sys/%s/%s/%s", rrpc_message_id, product_key, device_name, method_buff);
#else
        dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/rrpc/response/%d", product_key, device_name, dm_thing_manager->_rrpc_message_id);
#endif
    } else {
        dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s_reply", product_key, device_name, method_buff);
    }
#else
    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s_reply", product_key, device_name, method_buff);
#endif /* RRPC_ENABLED */

    (*message_info)->set_uri(message_info, uri_buff);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_RESPONSE);
    (*message_info)->set_id(message_info, dm_thing_manager->_usr_response_id);
    (*message_info)->set_code(message_info, dm_thing_manager->_code);

    if (strcmp(dm_thing_manager->_method, string_thing_service_property_set) == 0) {
        /* set property */
    } else if (strcmp(dm_thing_manager->_method, string_thing_service_property_get) == 0) {
        assert((*list)->get_size(list));
        /* get property */
        list_iterator(list, install_service_property_get_to_message_info, dm_thing_manager, thing, dm_thing_manager->_message_info);
        /* clear after use. */
        list_iterator(list, free_list_string, dm_thing_manager);
        (*list)->clear(list);
    } else {
        /* normal service */
        output_data_numb = service->service_output_data_num;
        for (i = 0; i < output_data_numb; ++i) {
            lite_property = service->service_output_data + i;
            assert(lite_property);
            install_lite_property_to_message_info(dm_thing_manager, message_info, lite_property);
        }
    }
}

static void get_event_key_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing);

    if (dm_thing_manager->_thing_id == thing) {
        event_iterator(thing, handle_event_key_value, dm_thing_manager, dm_thing_manager->_message_info, thing);
    }
}

static void get_service_key_value(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing);

    if (dm_thing_manager->_thing_id == thing) {
        service_iterator(thing, handle_service_key_value, dm_thing_manager, dm_thing_manager->_message_info, thing);
    }
}

static int dm_thing_manager_trigger_event(void* _self, const void* thing_id, const void* event_identifier, const char* property_identifier)
{
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    dm_thing_t* dm_thing;
    int ret = -1;

    assert(thing_id && event_identifier && cm && *cm);

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)event_identifier;
    self->_property_identifier_post = (void*)property_identifier;
    self->_ret = -1;
    self->_get_value_str = NULL;

    dm_thing = (dm_thing_t*)thing_id;

    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return ret;
    }

    local_thing_list_iterator(self, get_event_key_value, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, get_event_key_value, self);
    }
#endif
    if (-1 != self->_ret) {
        dm_log_debug("event(%s) triggered, method(%s)", self->_identifier, self->_method);
        ret = (*message_info)->serialize_to_payload_request(message_info);
        if (ret == -1) {
            dm_log_err("serialize_to_payload_request FAIL");
            return ret;
        }

        self->_ret = (*cm)->send(cm, message_info);
    }

    return self->_ret < 0 ? self->_ret : self->_current_id;
}

#if (defined EXTENDED_INFO_ENABLED) || (defined SUBDEV_ENABLE)
static void check_thing_id(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;

    dm_thing_manager = va_arg(*params, void*);

    assert(dm_thing_manager && thing && *thing);

    if (dm_thing_manager->_thing_id == thing) {
        dm_thing_manager->_ret = 0;
    }
}

#endif

#ifdef EXTENDED_INFO_ENABLED
static int check_extended_info_params(const char* params)
{
    cJSON* params_obj;
    int array_size;

    params_obj = cJSON_Parse(params);

    if (!cJSON_IsArray(params_obj)) {
        dm_log_err("input params format errorm, MUST be json array type");
        return -1;
    }

    array_size = cJSON_GetArraySize(params_obj);

    if (array_size <= 0) return -1;

    cJSON_Delete(params_obj);

    return 0;
}

static int dm_thing_manager_trigger_extended_info_update(void* _self, const void* thing_id, const char* params)
{
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    thing_t** thing;
    dm_thing_t* dm_thing;

    char method_buff[METHOD_MAX_LENGTH] = {0};
    char uri_buff[URI_MAX_LENGTH] = {0};

    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    char* p;

    assert(thing_id && cm && *cm && message_info && *message_info);

    self->_thing_id = (void*)thing_id;
    self->_ret = -1;
    self->_get_value_str = NULL;

    dm_thing = (dm_thing_t*)thing_id;
    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return -1;
    }

    check_extended_info_params(params);

    local_thing_list_iterator(self, check_thing_id, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, check_thing_id, self);
    }
#endif
    if (self->_ret != 0) return -1;

    thing = (thing_t**)thing_id;

    strcpy(method_buff, string_method_name_deviceinfo_update);
    /* subtitute '/' by '.' */
    do {
        p = strchr(method_buff, '/');
        if (p) *p = '.';
    } while (p);

    self->_method = method_buff;

    clear_and_set_message_info(message_info, self);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_thing_manager_install_product_key_device_name(self, thing, product_key, device_name);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", product_key, device_name, string_method_name_deviceinfo_update);
    (*message_info)->set_uri(message_info, uri_buff);

    (*message_info)->set_params_data(message_info, (char*)params);

    return (*cm)->send(cm, message_info);
}

static int dm_thing_manager_trigger_extended_info_delete(void* _self, const void* thing_id, const char* params)
{
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    thing_t** thing;
    dm_thing_t* dm_thing;

    char method_buff[METHOD_MAX_LENGTH] = {0};
    char uri_buff[URI_MAX_LENGTH] = {0};
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    char* p;

    assert(thing_id && cm && *cm && message_info && *message_info);

    self->_thing_id = (void*)thing_id;
    self->_ret = -1;
    self->_get_value_str = NULL;

    dm_thing = (dm_thing_t*)thing_id;
    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return -1;
    }

    check_extended_info_params(params);

    local_thing_list_iterator(self, check_thing_id, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret) {
        sub_thing_list_iterator(self, check_thing_id, self);
    }
#endif
    if (self->_ret != 0) return -1;

    thing = (thing_t**)thing_id;

    strcpy(method_buff, string_method_name_deviceinfo_delete);
    /* subtitute '/' by '.' */
    do {
        p = strchr(method_buff, '/');
        if (p) *p = '.';
    } while (p);

    self->_method = method_buff;

    clear_and_set_message_info(message_info, self);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_thing_manager_install_product_key_device_name(self, thing, product_key, device_name);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", product_key, device_name, string_method_name_deviceinfo_delete);
    (*message_info)->set_uri(message_info, uri_buff);

    (*message_info)->set_params_data(message_info, (char*)params);

    return (*cm)->send(cm, message_info);
}
#endif /* EXTENDED_INFO_ENABLED*/
static int generate_raw_message_info(void* _dm_thing_manager, void* _thing, void* _message_info, const char* raw_topic)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    thing_t** thing = _thing;
    message_info_t** message_info = _message_info;
    dm_thing_t* dm_thing;

    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    char uri_buff[URI_MAX_LENGTH] = {0};
    int ret;

    dm_thing = (dm_thing_t*)thing;
    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", thing);
        return -1;
    }

    dm_thing_manager_install_product_key_device_name(dm_thing_manager, thing, product_key, device_name);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", product_key, device_name, raw_topic);

    clear_and_set_message_info(message_info, dm_thing_manager);

    ret = (*message_info)->set_uri(message_info, uri_buff);
    if (ret == -1) return ret;

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_RAW);

    ret = (*message_info)->set_raw_data_and_length(message_info, dm_thing_manager->_raw_data, dm_thing_manager->_raw_data_length);
    if (ret == -1) return ret;

    return 0;
}

static int generate_raw_up_message_info(void* _dm_thing_manager, void* _thing, void* _message_info)
{
    return generate_raw_message_info(_dm_thing_manager, _thing, _message_info, string_method_name_up_raw);
}

static int generate_raw_down_reply_message_info(void* _dm_thing_manager, void* _thing, void* _message_info)
{
    return generate_raw_message_info(_dm_thing_manager, _thing, _message_info, string_method_name_down_raw_reply);
}

#ifdef RRPC_ENABLED
static int dm_thing_manager_answer_service(void* _self, const void* thing_id, const void* identifier, int response_id, int code, int rrpc)
#else
static int dm_thing_manager_answer_service(void* _self, const void* thing_id, const void* identifier, int response_id, int code)
#endif /* RRPC_ENABLED */
{
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    dm_thing_t* dm_thing;
    int ret;

    assert(thing_id && identifier && cm && *cm);

    self->_thing_id = (void*)thing_id;
    self->_identifier = (void*)identifier;
    self->_ret = -1;
    self->_get_value_str = NULL;
    self->_code = code;
    self->_usr_response_id = response_id;
    self->_method = NULL;

    dm_thing = (dm_thing_t*)thing_id;
    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return -1;
    }

    /* response id matches request id. */
    if (self->_usr_response_id == self->_request_id) {
        dm_log_debug("response id(%d) matches request id(%d).", self->_usr_response_id, self->_request_id);
        self->_request_id = 0;
    } else {
        dm_log_warning("response id(%d) not matche request id(%d).", self->_usr_response_id, self->_request_id);
    }
#ifdef RRPC_ENABLED
    self->_rrpc = rrpc;
#endif /* RRPC_ENABLED */
    local_thing_list_iterator(self, get_service_key_value, self);
#ifdef SUBDEV_ENABLE
    if (-1 == self->_ret){
        sub_thing_list_iterator(self, get_service_key_value, self);
    }
#endif
    dm_log_debug("answer normal service(%s), method(%s)", self->_identifier, self->_method ? self->_method : "NULL");

    ret = (*message_info)->serialize_to_payload_response(message_info);

    if (ret == -1) {
        dm_log_err("serialize_to_payload_response FAIL");
        return ret;
    }

    self->_ret = (*cm)->send(cm, message_info);

    return self->_ret;
}

static int dm_thing_manager_invoke_raw_service(void* _self, const void* thing_id, void* raw_data, int raw_data_length)
{
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    dm_thing_t* dm_thing;

    assert(thing_id);

    self->_thing_id = (void*)thing_id;
    self->_ret = 0;
    self->_raw_data = raw_data;
    self->_raw_data_length = raw_data_length;

    dm_log_debug("invoke up raw service");

    dm_thing = (dm_thing_t*)thing_id;
    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return -1;
    }

    if (generate_raw_up_message_info(self, self->_thing_id, message_info) == -1)
    {
        dm_log_err("invoke up raw service FAIL!");
        return -1;
    }

    self->_ret = (*cm)->send(cm, message_info);

    self->_raw_data = NULL;
    self->_raw_data_length = 0;

    return self->_ret;
}

static int dm_thing_manager_answer_raw_service(void* _self, const void* thing_id, void* raw_data, int raw_data_length)
{
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    dm_thing_t* dm_thing;

    assert(thing_id && raw_data && raw_data_length);

    self->_thing_id = (void*)thing_id;
    self->_ret = 0;
    self->_raw_data = raw_data;
    self->_raw_data_length = raw_data_length;

    dm_log_debug("invoke down raw reply service");

    if (generate_raw_down_reply_message_info(self, self->_thing_id, message_info) == -1)
    {
        dm_log_err("invoke down raw reply service FAIL!");
        return -1;
    }

    dm_thing = (dm_thing_t*)thing_id;
    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return -1;
    }

    self->_ret = (*cm)->send(cm, message_info);

    self->_raw_data = NULL;
    self->_raw_data_length = 0;

    return self->_ret;
}
#ifndef CM_SUPPORT_MULTI_THREAD
static int dm_thing_manager_yield(void* _self, int timeout_ms)
{
    dm_thing_manager_t* self = _self;
    cm_abstract_t** cm = self->_cm;

    return (*cm)->yield(cm, timeout_ms);
}
#endif /* CM_SUPPORT_MULTI_THREAD */


static int dm_thing_manager_install_product_key_device_name(void* _self, const void* thing_id, char* product_key, char* device_name)
{
    char* tsl_product_key;
    char* tsl_device_name;
    char hal_product_key[PRODUCT_KEY_MAXLEN] = {0};
    char hal_device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_thing_manager_t* self = _self;
    const thing_t** thing = (const thing_t**)thing_id;

    tsl_product_key = (*thing)->return_product_key(thing);
    tsl_device_name = (*thing)->return_device_name(thing);

    self->_ret = -1;
#ifdef SUBDEV_ENABLE
    sub_thing_list_iterator(self, find_node_by_thing_object, self, thing_id);
    if (0 == self->_ret) {
        strcpy(product_key, tsl_product_key);
        strcpy(device_name, tsl_device_name);
    } else
#endif
    {
        if (HAL_GetProductKey(hal_product_key) <= 0 || (HAL_GetDeviceName(hal_device_name) <= 0)) {
            dm_log_err("get HAL DeviceInfo failed!");
        }

        if (tsl_product_key && (0 == strcmp(tsl_product_key, hal_product_key))) {
            strcpy(product_key, tsl_product_key);
        } else {
            dm_log_err("hal pk(%s) != tsl pk(%s)", hal_product_key, tsl_product_key ? tsl_product_key : "NULL");
            strcpy(product_key, hal_product_key);
        }

        if (tsl_device_name && (0 == strcmp(tsl_device_name, hal_device_name))) {
            strcpy(device_name, tsl_device_name);
        } else {
            dm_log_err("hal dn(%s) != tsl dn(%s)", hal_device_name, tsl_device_name ? tsl_device_name : "NULL");
            strcpy(device_name, hal_device_name);
        }
    }

    self->_ret = 0;

    return self->_ret;
}

#ifdef SUBDEV_ENABLE
static void subdev_request_list_iterator(const void* _dm_thing_manager, handle_fp_t handle_fp)
{
    const dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    list_t** list = (list_t**)dm_thing_manager->_sub_request_list;

    assert((*list)->iterator);
    list_iterator(list, handle_fp, dm_thing_manager);
}

static void get_subdev_request_info(void* node, va_list* params)
{
    request_info_t* request_info = (request_info_t*)node;
    dm_thing_manager_t* dm_thing_manager = NULL;

    dm_thing_manager = va_arg(*params, void*);

    assert(request_info && dm_thing_manager);
    if (request_info->request_id == dm_thing_manager->_request_id) {
        dm_thing_manager->_request_info = request_info;
    }
}

static request_info_t* find_topo_delete_request_info_by_id(void* _dm_thing_manager, int request_id)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    assert(dm_thing_manager);

    dm_thing_manager->_request_id = request_id;
    dm_thing_manager->_request_info = NULL;

    subdev_request_list_iterator(dm_thing_manager, get_subdev_request_info);

    return dm_thing_manager->_request_info;
}

static request_info_t* find_topo_add_request_info_by_id(void* _dm_thing_manager, int request_id)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    assert(dm_thing_manager);

    dm_thing_manager->_request_id = request_id;
    dm_thing_manager->_request_info = NULL;

    subdev_request_list_iterator(dm_thing_manager, get_subdev_request_info);

    return dm_thing_manager->_request_info;
}

static request_info_t* find_subdev_login_request_info_by_id(void* _dm_thing_manager, int request_id)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    assert(dm_thing_manager);

    dm_thing_manager->_request_id = request_id;
    dm_thing_manager->_request_info = NULL;

    subdev_request_list_iterator(dm_thing_manager, get_subdev_request_info);

    return dm_thing_manager->_request_info;
}

static request_info_t* find_subdev_logout_request_info_by_id(void* _dm_thing_manager, int request_id)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    assert(dm_thing_manager);

    dm_thing_manager->_request_id = request_id;
    dm_thing_manager->_request_info = NULL;

    subdev_request_list_iterator(dm_thing_manager, get_subdev_request_info);

    return dm_thing_manager->_request_info;
}

static request_info_t* find_subdev_register_request_info_by_id(void* _dm_thing_manager, int request_id)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    assert(dm_thing_manager);

    dm_thing_manager->_request_id = request_id;
    dm_thing_manager->_request_info = NULL;

    subdev_request_list_iterator(dm_thing_manager, get_subdev_request_info);

    return dm_thing_manager->_request_info;
}

static void free_sub_request_node(void* _dm_thing_manager, request_info_t* node)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    list_t** list = (list_t**)dm_thing_manager->_sub_request_list;
    assert(dm_thing_manager && list && node);

    (*list)->remove(list, node);

    return;
}

static int send_subdev_login_message(void* _self, const char* subdev_pk, const char* subdev_dn, const char* subdev_ds,int _clean_session, char* hmac_method)
{
#define SUB_LOGIN_URI_FMT        "/ext/session/%s/%s/combine/login"
#define SUB_LOGIN_PACKET_FMT     "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\",\"cleanSession\":\"%s\"}"
    dm_thing_manager_t* self = _self;
    message_info_t** message_info;
    thing_t** sub_thing;
    unsigned long long timestamp_long = 0;
    char client_id[128] = {0};
    char timestamp[24] = {0};
    char hmac_source[256];
    char sign[64] = {0};
    char uri_buff[URI_MAX_LENGTH];
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[256] = {0};
    cm_abstract_t** cm;
    request_info_t* request_info;
    int is_sub_thing;

    const char* clean_session;
    const char* sub_product_key = subdev_pk;
    const char* sub_device_name = subdev_dn;
    const char* sub_device_secret = subdev_ds;

    assert(self && subdev_pk && subdev_dn && subdev_ds && hmac_method);

    message_info = self->_message_info;
    cm = self->_cm;

    assert(cm && *cm && message_info);

    clean_session = _clean_session == 0 ? "false" : "true";

    dm_snprintf(client_id, sizeof(client_id), "%s.%s", sub_product_key, sub_device_name);

    timestamp_long = HAL_UptimeMs();

    dm_lltoa(timestamp_long, timestamp, 10);

    dm_snprintf(hmac_source, sizeof(hmac_source), "clientId%s" "deviceName%s" "productKey%s" "timestamp%s", client_id, subdev_dn, subdev_pk, timestamp);

    if (strcmp(hmac_method, "hmacmd5") == 0) {
        utils_hmac_md5(hmac_source, strlen(hmac_source), sign, sub_device_secret, strlen(sub_device_secret));
    } else if (strcmp(hmac_method, "hmacsha1") == 0)  {
        utils_hmac_sha1(hmac_source, strlen(hmac_source), sign, sub_device_secret, strlen(sub_device_secret));
    } else {
        dm_log_err("unsupport sign method");
        return -1;
    }

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_sub_login);

    self->_method = NULL;
    clear_and_set_message_info(message_info, self);
    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), SUB_LOGIN_URI_FMT, self->_product_key, self->_device_name);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), SUB_LOGIN_PACKET_FMT, sub_product_key, sub_device_name, client_id,
                timestamp, strcmp(hmac_method, "hmacmd5") == 0 ? "hmacmd5" : "hmacsha1", sign, clean_session);
    (*message_info)->set_params_data(message_info, params_buff);

    dm_printf("login uri:\t%s\n", uri_buff);
    dm_printf("login params:\t%s\n", params_buff);

    sub_thing = dm_thing_manager_query_thing_id(self, sub_product_key, sub_device_name, &is_sub_thing);

    /* sub_thing may be NULL. */

    request_info = (request_info_t*)dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }

    request_info->thing_id = sub_thing;
    request_info->request_id = (*message_info)->get_id(message_info);
    list_insert(self->_sub_request_list, request_info);

    return (*cm)->send(cm, message_info);
}

static int send_subdev_topo_add_message(void* _self, const char* subdev_pk, const char* subdev_dn, const char* subdev_ds, char* hmac_method)
{
#define TOPO_ADD_PACKET_FMT     "[{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\"}]"

    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    thing_t** sub_thing;
    const char* sub_product_key = subdev_pk;
    const char* sub_device_name = subdev_dn;
    const char* sub_device_secret = subdev_ds;
    unsigned long long timestamp_long = 0;
    char timestamp[24] = {0};
    char client_id[128] = {0};
    char hmac_source[256];
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[320] = {0};
    char uri_buff[URI_MAX_LENGTH];
    char sign[64] = {0};
    char* p;
    request_info_t* request_info;
    int is_sub_thing = 0;

    assert(_self && subdev_pk && subdev_dn && subdev_ds && hmac_method && (strcmp(hmac_method, "hmacmd5") == 0 || strcmp(hmac_method, "hmacsha1") == 0));

    timestamp_long = HAL_UptimeMs();
    dm_lltoa(timestamp_long, timestamp, 10);

    dm_snprintf(client_id, sizeof(client_id), "%s.%s", sub_product_key, sub_device_name);
    dm_snprintf(hmac_source, sizeof(hmac_source), "clientId%s" "deviceName%s" "productKey%s" "timestamp%s", client_id, sub_device_name, sub_product_key, timestamp);
    if (strcmp(hmac_method, "hmacmd5") == 0) {
        utils_hmac_md5(hmac_source, strlen(hmac_source), sign, sub_device_secret, strlen(sub_device_secret));
    } else if (strcmp(hmac_method, "hmacsha1") == 0) {
        utils_hmac_sha1(hmac_source, strlen(hmac_source), sign, sub_device_secret, strlen(sub_device_secret));
    } else {
        dm_log_err("unsupport sign method");
        return -1;
    }

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_topo_add);

    p = method_buff;
    do {
        p = strchr(p, '/');
        if (p) *p = '.';
    } while (p);

    self->_method = method_buff;

    clear_and_set_message_info(message_info, self);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", self->_product_key, self->_device_name, string_method_name_topo_add);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), TOPO_ADD_PACKET_FMT, sub_product_key, sub_device_name, client_id,
                timestamp, strcmp(hmac_method, "hmacmd5") == 0 ? "hmacmd5" : "hmacsha1", sign);
    (*message_info)->set_params_data(message_info, params_buff);

    /* for debug usage. */
    dm_printf("topo add uri:\t%s\n", uri_buff);
    dm_printf("topo add params:\t%s\n", params_buff);

    request_info = dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }

    sub_thing = dm_thing_manager_query_thing_id(self, sub_product_key, sub_device_name, &is_sub_thing);

    /* sub_thing may be NULL. */
    request_info->thing_id = sub_thing;

    request_info->request_id = (*message_info)->get_id(message_info);
    dm_snprintf(request_info->product_key, sizeof(request_info->product_key), "%s", sub_product_key);
    dm_snprintf(request_info->device_name, sizeof(request_info->device_name), "%s", sub_device_name);
    dm_snprintf(request_info->device_secret, sizeof(request_info->device_secret), "%s", subdev_ds);
    list_insert(self->_sub_request_list, request_info);

    return (*cm)->send(cm, message_info);
}

static int send_subdev_register_message(void* _self, const char* subdev_pk, const char* subdev_dn)
{
#define REGISTER_PACKET_FMT     "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]"

    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    thing_t** sub_thing;
    const char* sub_product_key = subdev_pk;
    const char* sub_device_name = subdev_dn;
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[256] = {0};
    char uri_buff[URI_MAX_LENGTH];
    char* p;
    request_info_t* request_info;
    int is_sub_thing = 0;

    assert(_self && subdev_pk && subdev_dn);

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_sub_register);

    p = method_buff;
    do {
        p = strchr(p, '/');
        if (p) *p = '.';
    } while (p);

    self->_method = method_buff;

    clear_and_set_message_info(message_info, self);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", self->_product_key, self->_device_name, string_method_name_sub_register);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), REGISTER_PACKET_FMT, sub_product_key, sub_device_name);
    (*message_info)->set_params_data(message_info, params_buff);

    /* for debug usage. */
    dm_printf("sub register uri:\t%s\n", uri_buff);
    dm_printf("sub register params:\t%s\n", params_buff);

    sub_thing = dm_thing_manager_query_thing_id(self, sub_product_key, sub_device_name, &is_sub_thing);

    request_info = dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }

    request_info->thing_id = sub_thing;
    request_info->request_id = (*message_info)->get_id(message_info);

    dm_snprintf(request_info->product_key, sizeof(request_info->product_key), "%s", sub_product_key);
    dm_snprintf(request_info->device_name, sizeof(request_info->device_name), "%s", sub_device_name);

    list_insert(self->_sub_request_list, request_info);

    return (*cm)->send(cm, message_info);
}
#if 0
static int send_subdev_unregister_message(void* _self, const char* subdev_pk, const char* subdev_dn)
{
#define UNREGISTER_PACKET_FMT     "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]"
    thing_t** sub_thing;
    dm_thing_manager_t* self = _self;
    message_info_t** message_info = self->_message_info;
    cm_abstract_t** cm = self->_cm;
    const char* sub_product_key = subdev_pk;
    const char* sub_device_name = subdev_dn;
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[256] = {0};
    char uri_buff[URI_MAX_LENGTH];
    char* p;
    int is_sub_thing = 0;
#if 0
    request_info_t* request_info;
#endif
    assert(_self && subdev_pk && subdev_dn);

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_sub_unregister);

    p = method_buff;
    do {
        p = strchr(p, '/');
        if (p) *p = '.';
    } while (p);

    self->_method = method_buff;

    clear_and_set_message_info(message_info, self);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", self->_product_key, self->_device_name, string_method_name_sub_unregister);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), UNREGISTER_PACKET_FMT, sub_product_key, sub_device_name);
    (*message_info)->set_params_data(message_info, params_buff);

    /* for debug usage. */
    dm_printf("sub unregister uri:\t%s\n", uri_buff);
    dm_printf("sub unregister params:\t%s\n", params_buff);
#if 0
    request_info = dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }
    sub_thing = dm_thing_manager_query_thing_id(self, sub_product_key, sub_device_name, &is_sub_thing);
    request_info->thing_id = sub_thing;
    request_info->request_id = (*message_info)->get_id(message_info);

    dm_snprintf(request_info->product_key, sizeof(request_info->product_key), "%s", sub_product_key);
    dm_snprintf(request_info->device_name, sizeof(request_info->device_name), "%s", sub_device_name);

    list_insert(self->_sub_request_list, request_info);
#endif
    return (*cm)->send(cm, message_info);
}
#endif
static int check_update_subdev_permit_time(dm_thing_manager_t* _dm_thing_manager)
{
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;
    unsigned long long current_time_ms;
    unsigned long long time_duration_ms;

    if (dm_thing_manager->_permit_time_ms) {
        current_time_ms = HAL_UptimeMs();
        if (current_time_ms == dm_thing_manager->_uptime_ms) {
            time_duration_ms = 0;
        } else {
            time_duration_ms = current_time_ms > dm_thing_manager->_uptime_ms ? current_time_ms - dm_thing_manager->_uptime_ms : current_time_ms + __LONG_LONG_MAX__ * 2ULL + 1ULL - dm_thing_manager->_uptime_ms;
        }

        /* time out of permit time */
        if (time_duration_ms > dm_thing_manager->_permit_time_ms) {
            dm_log_debug("permit join out of time(%d)", dm_thing_manager->_permit_time_ms/1000);
            dm_thing_manager->_permit_time_ms = 0;
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

#if 0
static void* dm_thing_manager_generate_new_sub_thing(void* _self, const char* _product_key, const char* _device_name, const char* _device_secret,
                                                     const char* tsl, int tsl_len)
#else
static void* dm_thing_manager_generate_new_sub_thing(void* _self, const char* _product_key, const char* _device_name, const char* tsl, int tsl_len)
#endif
{
    dm_thing_manager_t* self = _self;
    thing_t** thing;
    list_t** list;
    char* thing_name;
    size_t name_size;
    const char* product_key;
    const char* device_name;
    int ret = -1;

    assert(_product_key && _device_name);

    if (!_product_key || ! _device_name) return NULL;
#if 0
    if (tsl && _device_secret == NULL) return NULL;
#endif
    if (check_update_subdev_permit_time(self) != 0) {
        dm_log_debug("permit join out of time");
        return NULL;
    }

    if (strlen(self->_permit_pk) && strcmp(self->_permit_pk, _product_key) != 0) {
        dm_log_debug("permit pk not match(%s)(%s)", self->_permit_pk, _product_key);
        return NULL;
    }

    name_size = strlen(DM_SUB_THING_NAME_PATTERN) + 2;
    thing_name = (char*)dm_lite_calloc(1, name_size);

    memset(thing_name, 0, name_size);
    dm_sprintf(thing_name, DM_SUB_THING_NAME_PATTERN, self->_sub_thing_cnt++);

    thing = (thing_t**)new_object(DM_THING_CLASS, thing_name);

    if (!thing) return NULL;

    if (tsl && tsl_len) {
        ret = (*thing)->set_tsl_string(thing, tsl, tsl_len);
    }


    list = self->_sub_thing_list;
    list_insert(list, thing);

    list = self->_sub_thing_name_list;
    list_insert(list, thing_name);

    self->_thing_id = self->_sub_thing_id = thing;

    dm_log_debug("new sub thing created@%p", self->_sub_thing_id);

    if (ret != 0) {
        product_key = _product_key;
        device_name = _device_name;

        /* set pk dn before tsl get from cloud. */
        (*thing)->set_product_key(thing, product_key);
        (*thing)->set_device_name(thing, device_name);
    } else {
        if (!(*thing)->return_device_name(thing)) {
            (*thing)->set_device_name(thing, _device_name);
        }

        product_key = (*thing)->return_product_key(thing);
        device_name = (*thing)->return_device_name(thing);

        if (strcmp(product_key, _product_key) != 0) {
            dm_log_err("pk(%s) of tsl != pk(%s) input", product_key, _product_key);
        }
    }

#if 0
    if (_device_secret) {
        send_subdev_topo_add_message(self, product_key, device_name, _device_secret, "hmacsha1"); /* "hmacsha1" or "hmacmd5" */
    } else {
        /* dynamic register */
        send_subdev_register_message(self, product_key, device_name);
    }
#endif
    /* invoke callback funtions. */
    if (self->_thing_id) invoke_callback_list(self, dm_callback_type_new_subthing_created);

    return thing;
}

static void check_thing_in_sub_thing_list(void* _thing, va_list* params)
{
    thing_t** sub_thing = _thing;
    dm_thing_manager_t* dm_thing_manager;
    thing_t** target_sub_thing;

    dm_thing_manager = va_arg(*params, void*);
    target_sub_thing = va_arg(*params, void*);

    assert(dm_thing_manager && target_sub_thing);

    if (sub_thing == target_sub_thing) {
        dm_thing_manager->_sub_thing_id = target_sub_thing;
        dm_thing_manager->_ret = 0;
    }
}

static int send_subdev_topo_delete_message(void* self, const void* sub_thing_id)
{
#define TOPO_DELETE_PACKET_FMT     "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]"
    dm_thing_manager_t* dm_thing_manager = self;
    message_info_t** message_info = dm_thing_manager->_message_info;
    cm_abstract_t** cm = dm_thing_manager->_cm;
    const thing_t** sub_thing = (const thing_t**)sub_thing_id;

    assert(sub_thing_id);

    const char* sub_product_key = (*sub_thing)->return_product_key(sub_thing);
    const char* sub_device_name = (*sub_thing)->return_device_name(sub_thing);

    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[256] = {0};
    char uri_buff[URI_MAX_LENGTH];

    char* p;
    request_info_t* request_info;

    assert(dm_thing_manager && sub_product_key && sub_device_name && message_info && cm);

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_topo_del);

    p = method_buff;
    do {
        p = strchr(p, '/');
        if (p) *p = '.';
    } while (p);

    dm_thing_manager->_method = method_buff;

    clear_and_set_message_info(message_info, dm_thing_manager);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", dm_thing_manager->_product_key, dm_thing_manager->_device_name, string_method_name_topo_del);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), TOPO_DELETE_PACKET_FMT, sub_product_key, sub_device_name);
    (*message_info)->set_params_data(message_info, params_buff);

    /* for debug usage. */
    dm_printf("topo delete uri:\t%s\n", uri_buff);
    dm_printf("topo delete params:\t%s\n", params_buff);

    request_info = dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }
    request_info->thing_id = (void*)sub_thing_id;
    request_info->request_id = (*message_info)->get_id(message_info);

    list_insert(dm_thing_manager->_sub_request_list, request_info);
    return (*cm)->send(cm, message_info);
}
#if 0
static int send_subdev_topo_get_message(void* self, const char* pk, const char* dn)
{
    thing_t** sub_thing;
    dm_thing_manager_t* dm_thing_manager = self;
    message_info_t** message_info = dm_thing_manager->_message_info;
    cm_abstract_t** cm = dm_thing_manager->_cm;
    int is_sub_thing = 0;
    const char* sub_product_key = pk;
    const char* sub_device_name = dn;

    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[256] = {0};
    char uri_buff[URI_MAX_LENGTH];

    char* p;
    request_info_t* request_info;

    assert(dm_thing_manager && sub_product_key && sub_device_name && message_info && cm);

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_topo_get);

    p = method_buff;
    do {
        p = strchr(p, '/');
        if (p) *p = '.';
    } while (p);

    dm_thing_manager->_method = method_buff;

    clear_and_set_message_info(message_info, dm_thing_manager);

    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), "/sys/%s/%s/%s", dm_thing_manager->_product_key, dm_thing_manager->_device_name, string_method_name_topo_get);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), TOPO_DELETE_PACKET_FMT, sub_product_key, sub_device_name);
    (*message_info)->set_params_data(message_info, params_buff);

    /* for debug usage. */
    dm_printf("topo delete uri:\t%s\n", uri_buff);
    dm_printf("topo delete params:\t%s\n", params_buff);

    request_info = dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }
    sub_thing = dm_thing_manager_query_thing_id(self, sub_product_key, sub_device_name, &is_sub_thing);
    request_info->thing_id = sub_thing;
    request_info->request_id = (*message_info)->get_id(message_info);

    list_insert(dm_thing_manager->_sub_request_list, request_info);
    return (*cm)->send(cm, message_info);
}
#endif
static int send_subdev_logout_message(void* _self, const void* sub_thing_id)
{
#define SUB_LOGOUT_URI_FMT        "/ext/session/%s/%s/combine/logout"
#define SUB_LOGOUT_PACKET_FMT     "{\"productKey\":\"%s\",\"deviceName\":\"%s\"}"
    dm_thing_manager_t* self = _self;
    message_info_t** message_info;

    char uri_buff[URI_MAX_LENGTH];
    char method_buff[METHOD_MAX_LENGTH] = {0};
    char params_buff[256] = {0};
    cm_abstract_t** cm;
    request_info_t* request_info;

    const thing_t** sub_thing = (const thing_t**)sub_thing_id;

    assert(sub_thing_id);

    const char* sub_product_key = (*sub_thing)->return_product_key(sub_thing);;
    const char* sub_device_name = (*sub_thing)->return_device_name(sub_thing);;

    message_info = self->_message_info;
    cm = self->_cm;

    assert(cm && *cm && message_info);

    dm_snprintf(method_buff, sizeof(method_buff), "%s", string_method_name_sub_logout);

    self->_method = NULL;
    clear_and_set_message_info(message_info, self);
    (*message_info)->set_message_type(message_info, DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST);

    dm_snprintf(uri_buff, sizeof(uri_buff), SUB_LOGOUT_URI_FMT, self->_product_key, self->_device_name);
    (*message_info)->set_uri(message_info, uri_buff);
    dm_snprintf(params_buff, sizeof(params_buff), SUB_LOGOUT_PACKET_FMT, sub_product_key, sub_device_name);
    (*message_info)->set_params_data(message_info, params_buff);

    dm_printf("logout uri:\t%s\n", uri_buff);
    dm_printf("logout params:\t%s\n", params_buff);

    request_info = (request_info_t*)dm_lite_malloc(sizeof(request_info_t));
    if (!request_info) {
        dm_log_err("malloc %d byte failed!", sizeof(request_info_t));
        return -1;
    }

    request_info->thing_id = sub_thing;
    request_info->request_id = (*message_info)->get_id(message_info);
    list_insert(self->_sub_request_list, request_info);
    return (*cm)->send(cm, message_info);
}

static int dm_thing_manager_remove_sub_thing(void* _self, const void* sub_thing_id)
{
    dm_thing_manager_t* self = _self;
    const thing_t** sub_thing = (const thing_t**)sub_thing_id;
    list_t** list;

    /* check if sub_thing_id in sub thing list */
    self->_ret = -1;
    list = self->_sub_thing_list;

    list_iterator(list, check_thing_in_sub_thing_list, self, sub_thing);

    if (self->_ret == -1) return -1;

    invoke_callback_list(self, dm_callback_type_subthing_destroyed);

    (*list)->remove(list, sub_thing);

    delete_object(sub_thing);

    self->_sub_thing_id = 0;

    return 0;
}

static int dm_thing_manager_topo_delete_sub_thing(void* _self, const void* sub_thing_id)
{
    dm_thing_manager_t* self = _self;
    const thing_t** sub_thing = (const thing_t**)sub_thing_id;
    list_t** list;

    /* check if sub_thing_id in sub thing list */
    self->_ret = -1;
    list = self->_sub_thing_list;

    list_iterator(list, check_thing_in_sub_thing_list, self, sub_thing);

    if (self->_ret == -1) return -1;

    return send_subdev_topo_delete_message(self, self->_sub_thing_id);
}

static int dm_thing_manager_bind_sub_thing(void* _self, const char* pk, const char* dn, const char* ds)
{
    dm_thing_manager_t* self = _self;
    thing_t** sub_thing;
    int is_sub_thing = 0;
    int ret;

    assert(pk && dn);

    sub_thing = dm_thing_manager_query_thing_id(self, pk, dn, &is_sub_thing);

    if (sub_thing && !is_sub_thing) return -1;

    if (ds) {
        ret = send_subdev_topo_add_message(self, pk, dn, ds, "hmacsha1");
    } else {
        ret = send_subdev_register_message(self, pk, dn);
    }

    return ret;
}

static int dm_thing_manager_unbind_sub_thing(void* _self, const char* pk, const char* dn)
{
    dm_thing_manager_t* self = _self;
    thing_t** sub_thing;
    int is_sub_thing = 0;
    int ret;

    assert(pk && dn);

    sub_thing = dm_thing_manager_query_thing_id(self, pk, dn, &is_sub_thing);

    if (!sub_thing || !is_sub_thing) return -1;

    ret = send_subdev_topo_delete_message(self, sub_thing);

    return ret;
}

static int dm_thing_manager_login_sub_thing(void* _self, const void* sub_thing_id, const char* ds)
{
    dm_thing_manager_t* self = _self;
    thing_t** sub_thing;
    dm_thing_t* dm_thing;
    int ret = -1;

    if (!ds) return -1;

    self->_thing_id = (void*)sub_thing_id;
    self->_ret = -1;

    dm_thing = (dm_thing_t*)sub_thing_id;

    if (dm_thing->_login) {
        dm_log_err("ALREADY login of thing@%p", dm_thing);
        return ret;
    }

    sub_thing_list_iterator(self, check_thing_id, self);

    if (self->_ret != 0) return -1;

    sub_thing = (thing_t**)sub_thing_id;

    ret = send_subdev_login_message(self, (*sub_thing)->return_product_key(sub_thing), (*sub_thing)->return_device_name(sub_thing), ds, 1, "hmacsha1");

    return ret;
}

static int dm_thing_manager_logout_sub_thing(void* _self, const void* sub_thing_id)
{
    dm_thing_manager_t* self = _self;
    thing_t** sub_thing;
    dm_thing_t* dm_thing;
    int ret = -1;

    self->_thing_id = (void*)sub_thing_id;
    self->_ret = -1;

    dm_thing = (dm_thing_t*)sub_thing_id;

    if (!dm_thing->_login) {
        dm_log_err("NOT login of thing@%p", dm_thing);
        return ret;
    }

    sub_thing_list_iterator(self, check_thing_id, self);

    if (self->_ret != 0) return -1;

    sub_thing = (thing_t**)sub_thing_id;

    ret = send_subdev_logout_message(self, sub_thing);

    return ret;
}

static int dm_thing_manager_add_subdev_callback_function(void* _self, handle_dm_subdev_callback_fp_t subdev_callback_func)
{
    dm_thing_manager_t* self = _self;
    list_t** list;

    list = (list_t**)self->_subdev_callback_list;

    assert(list);

    if (subdev_callback_func) list_insert(list, subdev_callback_func);

    return 0;
}

void* dm_thing_manager_query_thing_id(void* _self, const char* pk, const char* dn, int* is_subthing)
{
    int is_dev;
    list_t** list;
    iotx_cm_send_peer_t send_peer;
    dm_thing_manager_t* dm_thing_manager = _self;

    assert(_self && pk && dn && is_subthing);

    dm_snprintf(send_peer.product_key, sizeof(send_peer.product_key), "%s", pk);
    dm_snprintf(send_peer.device_name, sizeof(send_peer.device_name), "%s", dn);

    dm_thing_manager->_thing_id = NULL;
    list = dm_thing_manager->_local_thing_list;
    list_iterator(list, find_thing_via_product_key_and_device_name, dm_thing_manager, &send_peer);

    if (dm_thing_manager->_thing_id == NULL) {
        list = dm_thing_manager->_sub_thing_list;
        list_iterator(list, find_thing_via_product_key_and_device_name, dm_thing_manager, &send_peer);
        is_dev = 1;
    }

    if (!dm_thing_manager->_thing_id) {
        is_dev = 0;
        dm_log_debug("cannot find thing by pk(%s),dn(%s)", pk, dn);
    }

    *is_subthing = is_dev;
    return dm_thing_manager->_thing_id;
}

static void sub_thing_list_iterator(const void* _self, handle_fp_t handle_fp, ...)
{
    va_list params;
    const dm_thing_manager_t* self = _self;
    list_t** list = (list_t**)self->_sub_thing_list;

    assert((*list)->iterator);

    va_start(params, handle_fp);
    (*list)->iterator(list, handle_fp, &params);
    va_end(params);
}

static void find_node_by_thing_object(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;
    thing_t** target_thing = NULL;

    dm_thing_manager = va_arg(*params, void*);
    target_thing = va_arg(*params, thing_t**);

    assert(dm_thing_manager && thing && *thing);

    if (target_thing == thing) {
        dm_thing_manager->_ret = 0;
    }
}

static void find_thing_according_uri(void* _thing, va_list* params)
{
    thing_t** thing = _thing;
    dm_thing_manager_t* dm_thing_manager;
    char* uri;
    char* pk;
    char* dn;
    char* p;
    char pk_match = 0;
    char dn_match = 0;
    char uri_buff[URI_MAX_LENGTH] = {0};

    dm_thing_manager = va_arg(*params, void*);
    uri = va_arg(*params, char*);

    assert(thing && dm_thing_manager && uri);

    pk = (*thing)->return_product_key(thing);
    dn = (*thing)->return_device_name(thing);

    dm_snprintf(uri_buff, sizeof(uri_buff), "%s", uri);

    p = strtok(uri_buff, "/");
    while (p) {
        if (pk && 0 == strcmp(p, pk)) {
            pk_match = 1;
        }

        if (dn && 0 == strcmp(p, dn)) {
            dn_match = 1;
        }

        p = strtok(NULL, "/");
    }

    if (pk_match && dn_match) {
        dm_thing_manager->_thing_id = _thing;
    }
}

static void* query_thing_id_by_uri(void* _dm_thing_manager, char* uri)
{
    int is_subthing = 0;
    list_t** list;
    dm_thing_manager_t* dm_thing_manager = _dm_thing_manager;

    assert(dm_thing_manager && uri);

    dm_thing_manager->_thing_id = NULL;
    list = dm_thing_manager->_local_thing_list;

    if ((*list)->get_size(list) == 0) {
        return NULL;
    }

    list_iterator(list, find_thing_according_uri, dm_thing_manager, uri);

    if (dm_thing_manager->_thing_id) {
        is_subthing = 0;
    } else {
        list = dm_thing_manager->_sub_thing_list;
        list_iterator(list, find_thing_according_uri, dm_thing_manager, uri);
        if (dm_thing_manager->_thing_id) is_subthing = 1;
    }

    if (!dm_thing_manager->_thing_id) {
        dm_log_err("cannot find thing by uri(%s)", uri);
    }

    dm_thing_manager->_is_subthing = is_subthing;

    return dm_thing_manager->_thing_id;
}

#endif

static thing_manager_t _dm_thing_manager_class = {
    sizeof(dm_thing_manager_t),
    string_dm_thing_manager_class_name,
    dm_thing_manager_ctor,
    dm_thing_manager_dtor,
    dm_thing_manager_generate_new_local_thing,
#ifdef SUBDEV_ENABLE
    dm_thing_manager_generate_new_sub_thing,
    dm_thing_manager_remove_sub_thing,
    dm_thing_manager_bind_sub_thing,
    dm_thing_manager_unbind_sub_thing,
    dm_thing_manager_login_sub_thing,
    dm_thing_manager_logout_sub_thing,
    dm_thing_manager_topo_delete_sub_thing,
    dm_thing_manager_add_subdev_callback_function,
#endif
    dm_thing_manager_add_callback_function,
    dm_thing_manager_set_thing_property_value,
    dm_thing_manager_get_thing_property_value,
    dm_thing_manager_set_thing_event_output_value,
    dm_thing_manager_get_thing_event_output_value,
    dm_thing_manager_get_thing_service_input_value,
    dm_thing_manager_get_thing_service_output_value,
    dm_thing_manager_set_thing_service_output_value,
    dm_thing_manager_trigger_event,
#ifdef EXTENDED_INFO_ENABLED
    dm_thing_manager_trigger_extended_info_update,
    dm_thing_manager_trigger_extended_info_delete,
#endif /* EXTENDED_INFO_ENABLED*/
    dm_thing_manager_answer_service,
    dm_thing_manager_invoke_raw_service,
    dm_thing_manager_answer_raw_service,
#ifndef CM_SUPPORT_MULTI_THREAD
    dm_thing_manager_yield,
#endif /* CM_SUPPORT_MULTI_THREAD */
    dm_thing_manager_install_product_key_device_name,
};

const void* get_dm_thing_manager_class()
{
    return &_dm_thing_manager_class;
}
