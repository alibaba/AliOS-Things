#ifndef THING_ABSTRACT_H
#define THING_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>
#include <stdlib.h>

typedef void (*handle_item_t)(void* property, int index, va_list* params);

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    int   (*set_tsl_string)(void* _self, const char* tsl, int tsl_str_len);  /* set tsl to thing model. */
    void* (*get_tsl_string)(void* _self); /* get tsl from thing model. */
    int   (*get_property_number)(const void* _self);
    int   (*get_service_number)(const void* _self);
    int   (*get_event_number)(const void* _self);
    void* (*get_property_by_identifier)(const void* _self, const char* const identifier);
    int   (*get_property_identifier_by_index)(const void* _self, int index, char* identifier);
    void* (*get_service_by_identifier)(const void* _self, const char* const identifier);
    int   (*get_service_identifier_by_index)(const void* _self, int index, char* identifier);
    void* (*get_event_by_identifier)(const void* _self, const char* const identifier);
    int   (*get_event_identifier_by_index)(const void* _self, int index, char* identifier);
    int   (*get_schema)(const void* _self, char* schema);
    int   (*get_link)(const void* _self, char* link);
    int   (*set_product_key)(void* _self, const char* product_key);
    int   (*get_product_key)(const void* _self, char* product_key);
    char* (*return_product_key)(const void* _self);
    int   (*set_device_name)(void* _self, const char* device_name);
    int   (*get_device_name)(const void* _self, char* device_name);
    char* (*return_device_name)(const void* _self);
    int   (*set_property_value_by_identifier)(void* _self, const char* const identifier, const void* value, const char* value_str);
    int   (*set_property_value)(void* _self, void* property, const void* value, const char* value_str);
    int   (*get_property_value_by_identifier)(const void* _self, const char* const identifier, void* value, char** value_str);
    int   (*get_property_value)(const void* _self, const void* const property, void* value, char** value_str);
    void  (*property_iterator)(void* _self, handle_item_t handle_fp, va_list* params);
    void  (*event_iterator)(void* _self, handle_item_t handle_fp, va_list* params);
    void  (*service_iterator)(void* _self, handle_item_t handle_fp, va_list* params);
#ifdef ENABLE_THING_DEBUG
    void  (*print_property_lite_info)(const void* _self);
    void  (*print_property_detail_info)(const void* _self);
    void  (*print_event_lite_info)(const void* _self);
    void  (*print_event_detail_info)(const void* _self);
    void  (*print_service_lite_info)(const void* _self);
    void  (*print_service_detail_info)(const void* _self);
#endif
    /* identifier must be *.* format pointer to identifier of outputData. */
    int   (*set_event_value_by_identifier)(void* _self, const char* const identifier, const void* value, const char* value_str);
    int   (*get_event_value_by_identifier)(void* _self, const char* const identifier, void* value, char** value_str);
    int   (*set_service_input_output_data_value_by_identifier)(void* _self, const char* const identifier, const void* value, const char* value_str);
    int   (*get_service_input_output_data_value_by_identifier)(void* _self, const char* const identifier, void* value, char** value_str);
    int   (*get_lite_property_value)(const void* _self, const void* const property, void* value, char** value_str);
} thing_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // THING_ABSTRACT_H
