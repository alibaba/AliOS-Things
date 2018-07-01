#ifndef LINKKIT_EXPORT_H
#define LINKKIT_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "iot_export_dm.h"
#ifdef SERVICE_OTA_ENABLED
#include "iot_export_fota.h"
#endif /* SERVICE_OTA_ENABLED */

typedef void (*handle_post_cb_fp_t)(const void* thing_id, int respons_id, int code, const char* response_message, void* ctx);
typedef void (*handle_subdev_cb_fp_t)(const void* thing_id, int code, const char* response_message, int success, void* ctx);

typedef struct _linkkit_ops {
#ifdef LOCAL_CONN_ENABLE
    int (*on_connect)(void* ctx, int cloud); /* true: cloud connection; false: local connection */
    int (*on_disconnect)(void* ctx, int cloud); /* true: cloud connection; false: local connection */
#else
    int (*on_connect)(void* ctx); /* true: cloud connection; false: local connection */
    int (*on_disconnect)(void* ctx); /* true: cloud connection; false: local connection */
#endif
    int (*raw_data_arrived)(const void* thing_id, const void* data, int len, void* ctx);
#ifdef SUBDEV_ENABLE
    int (*thing_create)(const void* thing_id, void* ctx, int sub);
    int (*sub_thing_destroy)(const void* sub_thing_id, void* ctx);
    int (*sub_thing_registered)(const void* sub_thing_id, const char* sub_thing_pk, const char* sub_thing_dn, const char* sub_thing_ds, void* ctx);
#else
    int (*thing_create)(const void* thing_id, void* ctx);
#endif
    int (*thing_enable)(const void* thing_id, void* ctx);
    int (*thing_disable)(const void* thing_id, void* ctx);
#ifdef RRPC_ENABLED
    int (*thing_call_service)(const void* thing_id, const char* service, int request_id, int rrpc, void* ctx);
#else
    int (*thing_call_service)(const void* thing_id, const char* service, int request_id, void* ctx);
#endif /* RRPC_ENABLED */
    int (*thing_prop_changed)(const void* thing_id, const char* property, void* ctx);
    int (*linkit_data_arrived)(const void* thing_id, const void* data, int len, void* ctx);
} linkkit_ops_t;

#ifdef SUBDEV_ENABLE
typedef struct _linkkit_subdev_ops {
    handle_subdev_cb_fp_t topo_add_notify;
    handle_subdev_cb_fp_t topo_delete_notify;
    handle_subdev_cb_fp_t login_notify;
    handle_subdev_cb_fp_t logout_notify;
} linkkit_subdev_ops_t;
#endif

typedef enum _linkkit_loglevel {
    linkkit_loglevel_emerg = 0,
    linkkit_loglevel_crit,
    linkkit_loglevel_error,
    linkkit_loglevel_warning,
    linkkit_loglevel_info,
    linkkit_loglevel_debug,
} linkkit_loglevel_t;

/* domain type */
/* please sync with dm_cloud_domain_type_t */
typedef enum {
    /* iot-as-mqtt.cn-shanghai.aliyuncs.com */
    linkkit_cloud_domain_shanghai,
    /* USA */
    linkkit_cloud_domain_singapore,

    linkkit_cloud_domain_max,
} linkkit_cloud_domain_type_t;

/* device info related operation */
typedef enum {
    linkkit_extended_info_operate_update,
    linkkit_extended_info_operate_delete,

    linkkit_deviceinfo_operate_max,
} linkkit_extended_info_operate_t;

/**
 * @brief dispatch message of queue for further process.
 *
 * @return void*
 */
#ifdef CM_SUPPORT_MULTI_THREAD
void* linkkit_dispatch(void* params);
#else
void* linkkit_dispatch();
#endif

/**
 * @brief start linkkit routines, and install callback funstions(async type for cloud connecting).
 *
 * @param max_buffered_msg, specify max buffered message size.
 * @param ops, callback function struct to be installed.
 * @param get_tsl_from_cloud, config if device need to get tsl from cloud(!0) or local(0), if local selected, must invoke linkkit_set_tsl to tell tsl to dm after start complete.
 * @param log_level, config log level.
 * @param user_context, user context pointer.
 * @param domain_type, specify the could server domain.
 *
 * @return int, 0 when success, -1 when fail.
 */
int linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_ops_t *ops, linkkit_cloud_domain_type_t domain_type, void *user_context);

#ifdef SERVICE_OTA_ENABLED
/**
 * @brief init fota service routines, and install callback funstions.
 *
 * @param callback_fp, callback function to be installed.
 *
 * @return int, 0 when success, -1 when fail.
 */
int linkkit_ota_init(handle_service_fota_callback_fp_t callback_fp);
#endif /* SERVICE_OTA_ENABLED */

/**
 * @brief stop linkkit routines.
 *
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_end();

/**
 * @brief install user tsl.
 *
 * @param tsl, tsl string that contains json description for thing object.
 * @param tsl_len, tsl string length.
 *
 * @return pointer to thing object, NULL when fails.
 */
extern void* linkkit_set_tsl(const char* tsl, int tsl_len);

/* patterns: */
/* method:
 * set_property_/event_output_/service_output_value:
 * method_set, thing_id, identifier, value */

typedef enum {
    linkkit_method_set_property_value = 0,
    linkkit_method_set_event_output_value,
    linkkit_method_set_service_output_value,

    linkkit_method_set_number,
} linkkit_method_set_t;

/**
 * @brief set value to property, event output, service output items.
 *        if identifier is struct type or service output type or event output type, use '.' as delimeter like "identifier1.ientifier2"
 *        to point to specific item.
 *        value and value_str could not be NULL at the same time;
 *        if value and value_str both as not NULL, value shall be used and value_str will be ignored.
 *        if value is NULL, value_str not NULL, value_str will be used.
 *        in brief, value will be used if not NULL, value_str will be used only if value is NULL.
 *
 * @param method_set, specify set value type.
 * @param thing_id, pointer to thing object, specify which thing to set.
 * @param identifier, property, event output, service output identifier.
 * @param value, value to set.(input int* if target value is int type or enum or bool, float* if float type,
 *        long long* if date type, char* if text type).
 * @param value_str, value to set in string format if value is null.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_set_value(linkkit_method_set_t method_set, const void* thing_id, const char* identifier,
                             const void* value, const char* value_str);

typedef enum {
    linkkit_method_get_property_value = 0,
    linkkit_method_get_event_output_value,
    linkkit_method_get_service_input_value,
    linkkit_method_get_service_output_value,

    linkkit_method_get_number,
} linkkit_method_get_t;

/**
 * @brief get value from property, event output, service input/output items.
 *        if identifier is struct type or service input/output type or event output type, use '.' as delimeter like "identifier1.ientifier2"
 *        to point to specific item.
 *        value and value_str could not be NULL at the same time;
 *        if value and value_str both as not NULL, value shall be used and value_str will be ignored.
 *        if value is NULL, value_str not NULL, value_str will be used.
 *        in brief, value will be used if not NULL, value_str will be used only if value is NULL.
 * @param method_get, specify get value type.
 * @param thing_id, pointer to thing object, specify which thing to get.
 * @param identifier, property, event output, service input/output identifier.
 * @param value, value to get(input int* if target value is int type or enum or bool, float* if float type,
 *        long long* if date type, char* if text type).
 * @param value_str, value to get in string format. DO NOT modify this when function returns,
 *        user should copy to user's own buffer for further process.
 *        user should NOT free the memory.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_get_value(linkkit_method_get_t method_get, const void* thing_id, const char* identifier,
                             void* value, char** value_str);


/**
 * @brief answer to a service when a service requested by cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param service_identifier, service identifier to answer, user should get this identifier from handle_dm_callback_fp_t type callback
 *        report that "dm_callback_type_service_requested" happened, use this function to generate response to the service sender.
 * @param response_id, id value in response payload. its value is from "dm_callback_type_service_requested" type callback function.
 *        use the same id as the request to send response as the same communication session.
 * @param code, code value in response payload. for example, 200 when service successfully executed, 400 when not successfully executed.
 * @param rrpc, specify rrpc service call or not.
 *
 * @return 0 when success, -1 when fail.
 */
#ifdef RRPC_ENABLED
extern int linkkit_answer_service(const void* thing_id, const char* service_identifier, int response_id, int code, int rrpc);
#else
extern int linkkit_answer_service(const void* thing_id, const char* service_identifier, int response_id, int code);
#endif /* RRPC_ENABLED */

/**
 * @brief answer a down raw service when a raw service requested by cloud, or invoke a up raw service to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param is_up_raw, specify up raw(not 0) or down raw reply(0).
 * @param raw_data, raw data that sent to cloud.
 * @param raw_data_length, raw data length that sent to cloud.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_invoke_raw_service(const void* thing_id, int is_up_raw, void* raw_data, int raw_data_length);

#ifdef SERVICE_OTA_ENABLED
/**
 * @brief perform ota service when "new version detected" reported.
 *
 * @param is_up_raw, specify up raw(not 0) or down raw reply(0).
 * @param data_buf, data buf that used to do ota. ota service will use this buffer to download bin.
 * @param data_buf_length, data buf length that used to do ota.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_invoke_ota_service(void* data_buf, int data_buf_length);
#endif /* SERVICE_OTA_ENABLED */

#ifdef EXTENDED_INFO_ENABLED
/**
 * @brief trigger extended info update procedure.
 *
 * @param thing_id, pointer to thing object.
 * @param params, json type string that user to send to cloud.
 * @param linkkit_extended_info_operation, specify update type or delete type.
 *
 * @return 0 when success, -1 when fail.
 */

int linkkit_trigger_extended_info_operate(const void* thing_id, const char* params, linkkit_extended_info_operate_t linkkit_extended_info_operation);
#endif /* EXTENDED_INFO_ENABLED */

/**
 * @brief trigger a event to post to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param event_identifier, event identifier to trigger.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_trigger_event(const void* thing_id, const char* event_identifier, handle_post_cb_fp_t cb);
/**
 * @brief post property to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param property_identifier, used when trigger event with method "event.property.post", if set, post specified property, if NULL, post all.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_post_property(const void* thing_id, const char* property_identifier, handle_post_cb_fp_t cb);
#ifdef SUBDEV_ENABLE

/**
 * @brief trigger a event to post to cloud.
 *
 * @param product_key, device_name of sub-thing.
 * @param tsl, tsl of sub-thing, could be NULL to get tsl from cloud
 * @param tsl_len, tsl length
 *
 * @return void*, sub thing id when success, NULL when fail.
 */
void* linkkit_subdev_create(const char* product_key, const char* device_name, const char* tsl, int tsl_len);

/**
 * @brief remove a sub device from gateway.
 *
 * @param sub_thing_id, pointer to sub-thing object.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_subdev_destroy(const void* sub_thing_id);

/**
 * @brief bind subdev to gateway.
 *
 * @param product_key, device_name and device secret of subdev.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_bind_subdev(const void* subdev_product_key, const char* subdev_device_name, const char* subdev_device_secret, handle_subdev_cb_fp_t cb);

/**
 * @brief unbind subdev from gateway.
 *
 * @param product_key, device_name of subdev.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_unbind_subdev(const void* subdev_product_key, const void* subdev_device_name, handle_subdev_cb_fp_t cb);

/**
 * @brief subdev login to cloud.
 *
 * @param sub_thing_id, pointer to sub-thing object.
 * @param subdev_device_secret, device secret of subdev
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_subdev_login(const void* sub_thing_id, const char* subdev_device_secret, handle_subdev_cb_fp_t cb);

/**
 * @brief subdev logout from cloud.
 *
 * @param sub_thing_id, pointer to sub-thing object.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_subdev_logout(const void* sub_thing_id, handle_subdev_cb_fp_t cb);

#endif

/**
 * @brief get time from cloud using ntp protocol.
 *
 * @param callback, when device get reply from cloud, it will call callback to trigger user.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_ntp_time_request(void (*)(const char *ntp_offset_time_ms));

#ifndef CM_SUPPORT_MULTI_THREAD
/**
 * @brief this function used to yield when want to receive or send data.
 *        if multi-thread enabled, user should NOT call this function.
 *
 * @param timeout_ms, timeout value in ms.
 *
 * @return 0 when success, -1 when fail.
 */
extern int linkkit_yield(int timeout_ms);
#endif /* CM_SUPPORT_MULTI_THREAD */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINKKIT_EXPORT_H */
