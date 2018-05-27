#ifndef THING_MANAGER_ABSTRACT_H
#define THING_MANAGER_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdarg.h>

#include "iot_export_dm.h"

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    void* (*generate_new_local_thing)(void* _self, const char* tsl, int tsl_len);
#ifdef SUBDEV_ENABLE
    void* (*generate_new_sub_thing)(void* _self, const char* product_key, const char* device_name, const char* tsl, int tsl_len);
    int   (*remove_sub_thing)(void* _self, const void* sub_thing_id);
    int   (*bind_sub_thing)(void* _self, const char* pk, const char* dn, const char* ds);
    int   (*unbind_sub_thing)(void* _self, const char* pk, const char* dn);
    int   (*login_sub_thing)(void* _self, const void* sub_thing_id, const char* ds);
    int   (*logout_sub_thing)(void* _self, const void* sub_thing_id);
    int   (*topo_delete_sub_thing)(void* _self, const void* sub_thing_id);
    int   (*add_subdev_callback_function)(void* _self, handle_dm_subdev_callback_fp_t subdev_callback_func);
#endif
    int   (*add_callback_function)(void* _self, handle_dm_callback_fp_t callback_func);
    int   (*set_thing_property_value)(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
    int   (*get_thing_property_value)(void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*set_thing_event_output_value)(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
    int   (*get_thing_event_output_value)(void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*get_thing_service_input_value)(void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*get_thing_service_output_value)(void* _self, const void* thing_id, const void* identifier, void* value, char** value_str);
    int   (*set_thing_service_output_value)(void* _self, const void* thing_id, const void* identifier, const void* value, const char* value_str);
    int   (*trigger_event)(void* _self, const void* thing_id, const void* event_identifier, const char* property_identifier);
#ifdef EXTENDED_INFO_ENABLED
    int   (*trigger_extended_info_update)(void* _self, const void* thing_id, const char* params);
    int   (*trigger_extended_info_delete)(void* _self, const void* thing_id, const char* params);
#endif /* EXTENDED_INFO_ENABLED*/
#ifdef RRPC_ENABLED
    int   (*answer_service)(void* _self, const void* thing_id, const void* identifier, int response_id, int code, int rrpc);
#else
    int   (*answer_service)(void* _self, const void* thing_id, const void* identifier, int response_id, int code);
#endif /* RRPC_ENABLED */
    int   (*invoke_raw_service)(void* _self, const void* thing_id, void* raw_data, int raw_data_length);
    int   (*answer_raw_service)(void* _self, const void* thing_id, void* raw_data, int raw_data_length);
#ifndef CM_SUPPORT_MULTI_THREAD
    int   (*yield)(void* _self, int timeout);
#endif /* CM_SUPPORT_MULTI_THREAD */
    int   (*install_product_key_device_name)(void* _self, const void* thing_id, char* product_key, char* device_name);
} thing_manager_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THING_MANAGER_ABSTRACT_H */
