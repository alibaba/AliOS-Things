#ifndef DM_EXPORT_H
#define DM_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdarg.h>

#define DM_PRODUCT_KEY        "a1AzoSi5TMc"
#define DM_DEVICE_NAME        "test_light_for_iotx_dm_cmp_dev"
#define DM_DEVICE_SECRET      "Hl1UjJIBXo3AzjLhYdtWEGwSllHCyBpm"
#define DM_DEVICE_ID          "IoTxHttpTestDev_001"

typedef enum {
    dm_type_master_device = 0,
    dm_type_subsidiary_device,

    dm_type_number,
} dm_thing_type_t;

typedef enum {
    dm_callback_type_property_value_set = 0,
    dm_callback_type_service_requested,
#ifdef RRPC_ENABLED
    dm_callback_type_rrpc_requested,
#endif /* RRPC_ENABLED */
    dm_callback_type_cloud_connected,
    dm_callback_type_cloud_disconnected,
    dm_callback_type_new_thing_created,
#ifdef SUBDEV_ENABLE
    dm_callback_type_new_subthing_created,
    dm_callback_type_subthing_destroyed,
    dm_callback_type_subthing_registered,
#endif
    dm_callback_type_thing_disabled,
    dm_callback_type_thing_enabled,
    dm_callback_type_raw_data_arrived,
    dm_callback_type_response_arrived,
    dm_callback_type_linkkit_data_arrived,
#ifdef LOCAL_CONN_ENABLE
    dm_callback_type_local_connected,
    dm_callback_type_local_disconnected,
    dm_callback_type_local_post,
#endif
    dm_callback_type_number,
} dm_callback_type_t;

typedef void (*handle_dm_callback_fp_t)(dm_callback_type_t callback_type, void* thing_id,
                                        const char* property_service_identifier, int id,
                                        void* raw_data, int raw_data_length, int code,
                                        char* response_message, void *param, int param_length
#ifdef SUBDEV_ENABLE
                                        , char* sub_pk, char* sub_dn, char* sub_ds
#endif
                                        );

#ifdef SUBDEV_ENABLE
typedef enum {
    dm_subdev_callback_type_topo_add_success,
    dm_subdev_callback_type_topo_add_fail,
    dm_subdev_callback_type_topo_delete_success,
    dm_subdev_callback_type_topo_delete_fail,
    dm_subdev_callback_type_login_success,
    dm_subdev_callback_type_login_fail,
    dm_subdev_callback_type_logout_success,
    dm_subdev_callback_type_logout_fail,
    dm_subdev_callback_type_register_success,
    dm_subdev_callback_type_register_fail,

    dm_subdev_callback_type_number,
} dm_subdev_callback_type_t;

typedef void (*handle_dm_subdev_callback_fp_t)(dm_subdev_callback_type_t callback_type, void* sub_thing_id, int id, int code, char* response_message,
                                               char* subdev_pk_from_register, char* subdev_dn_from_register, char* subdev_ds_from_register);
#endif

/* domain type */
typedef enum {
    /* iot-as-mqtt.cn-shanghai.aliyuncs.com */
    dm_cloud_domain_shanghai,
    /* USA */
    dm_cloud_domain_singapore,

    dm_cloud_domain_max,
} dm_cloud_domain_type_t;

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    void* (*generate_new_thing)(void* _self, const char* tsl, int tsl_len);
#ifdef SUBDEV_ENABLE
    void* (*generate_new_subthing)(void* _self, const char* product_key, const char* device_name, const char* tsl, int tsl_len);
    int   (*remove_subthing)(void* _self, const void* sub_thing_id);
    int   (*bind_sub_thing)(void* _self, const char* pk, const char* dn, const char* ds);
    int   (*unbind_sub_thing)(void* _self, const char* pk, const char* dn);
    int   (*login_sub_thing)(void* _self, const void* sub_thing_id, const char* ds);
    int   (*logout_sub_thing)(void* _self, const void* sub_thing_id);
    int   (*topo_delete_subthing)(void* _self, const void* sub_thing_id);
    int   (*add_subdev_callback_function)(void* _self, handle_dm_subdev_callback_fp_t subdev_callback_func);
#endif
    int   (*set_property_value)(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
    int   (*set_event_output_value)(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
    int   (*set_service_output_value)(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
    int   (*get_property_value)(const void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*get_service_input_value)(const void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*get_service_output_value)(const void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*get_event_output_value)(const void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*install_callback_function)(void* _self, handle_dm_callback_fp_t linkkit_callback_fp);
    int   (*trigger_event)(const void* _self, const void* thing_id, const void* event_identifier, const char* property_identifier);
#ifdef EXTENDED_INFO_ENABLED
    int   (*trigger_extended_info_update)(const void* _self, const void* thing_id, const char* params);
    int   (*trigger_extended_info_delete)(const void* _self, const void* thing_id, const char* params);
#endif /* EXTENDED_INFO_ENABLED*/
#ifdef RRPC_ENABLED
    int   (*answer_service)(const void* _self, const void* thing_id, const void* identifier, int response_id, int code, int rrpc);
#else
    int   (*answer_service)(const void* _self, const void* thing_id, const void* identifier, int response_id, int code);
#endif /* RRPC_ENABLED */
    int   (*invoke_raw_service)(const void* _self, const void* thing_id, void* raw_data, int raw_data_length);
    int   (*answer_raw_service)(const void* _self, const void* thing_id, void* raw_data, int raw_data_length);
#ifndef CM_SUPPORT_MULTI_THREAD
    int   (*yield)(const void* _self, int timeout_ms);
#endif /* CM_SUPPORT_MULTI_THREAD */
} dm_t;

#define DM_IMPL_CLASS get_dm_impl_class()

typedef struct {
    const void*  _;
    void*        _logger;
    char*        _name; /* dm thing manager object name. */
    int          _get_tsl_from_cloud;
    void*        _thing_manager; /* thing manager object */
    int          _log_level;
    int          _domain_type;
} dm_impl_t;

extern const void* get_dm_impl_class();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DM_EXPORT_H */
