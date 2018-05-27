#ifndef TSL_H
#define TSL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>

typedef enum {
    property_access_mode_rw = 0, /* rw */
    property_access_mode_r, /* r */
    property_access_mode_w, /* w */
} property_access_mode_t;

typedef enum {
    data_type_type_text = 0, /* text */
    data_type_type_enum, /* enum */
    data_type_type_bool, /* bool */
    data_type_type_float, /* float */
    data_type_type_double, /* double */
    data_type_type_int, /* int */
    data_type_type_date, /* date */
    data_type_type_struct, /* struct */
    data_type_type_array, /* array */
} data_type_type_t;

typedef enum {
    event_type_info = 0,
    event_type_alert,
    event_type_error,
} event_type_t;

typedef enum {
    service_type_others = 0, /* other services */
    service_type_property_get, /*  "method": "thing.service.property.get" */
    service_type_property_set, /* "method": "thing.service.property.set" */
} service_type_t;

#ifdef LITE_THING_MODEL
typedef union _data_type_x {
    struct _data_type_int {
        int min; /* min */
        int max; /* max */
        int value;  /* truly value. */
        char* value_str; /* string type value */
        int precise; /* precise */
    } data_type_int_t;

    struct _data_type_float {
        float min; /* min */
        float max; /* max */
        float value;  /* truly value. */
        char* value_str; /* string type value */
        int precise; /* precise */
    } data_type_float_t;

    struct _data_type_double {
        float min; /* min */
        float max; /* max */
        double value;  /* truly value. */
        char* value_str; /* string type value */
        int precise; /* precise */
    } data_type_double_t;

    struct _data_type_enum {
        int enum_item_number; /* number of enum items. */
        int value; /* truly value. */
        char* value_str; /* string type value */
        char** enum_item_key;
        char** enum_item_value;
    } data_type_enum_t;

    struct _data_type_text {
        char* length_str;
        int length; /* text length. */
        char* value; /* truly value. */
    } data_type_text_t;

    struct _data_type_date {
        unsigned long long value; /* truly value. */
        char* value_str; /* string type value */
    } data_type_date_t;

    struct _data_type_bool {
        int bool_item_number; /* number of enum items. */
        int value; /* 0: false; 1: true. */
        char* value_str; /* string type value */
        char** bool_item_key;
        char** bool_item_value;
    } data_type_bool_t;

    struct _data_type_array {
        int size;
        data_type_type_t item_type;
        void* array;
        char** value_str;
    } data_type_array_t;

} data_type_x_t;
#else
typedef union _data_type_x {
    struct _data_type_int {
        int min; /* min */
        char* min_str;
        int max; /* max */
        char* max_str;
        int value;  /* truly value. */
        char* value_str; /* string type value */
        int precise; /* precise */
        char* precise_str;
        char* unit; /* unit */
        char* unit_name; /* unitName */
    } data_type_int_t;

    struct _data_type_float {
        float min; /* min */
        char* min_str;
        float max; /* max */
        char* max_str;
        float value;  /* truly value. */
        char* value_str; /* string type value */
        int precise; /* precise */
        char* precise_str;
        char* unit; /* unit */
        char* unit_name; /* unitName */
    } data_type_float_t;

    struct _data_type_double {
        double min; /* min */
        char* min_str;
        double max; /* max */
        char* max_str;
        double value;  /* truly value. */
        char* value_str; /* string type value */
        int precise; /* precise */
        char* precise_str;
        char* unit; /* unit */
        char* unit_name; /* unitName */
    } data_type_double_t;

    struct _data_type_enum {
        int enum_item_number; /* number of enum items. */
        int value; /* truly value. */
        char* value_str; /* string type value */
        char** enum_item_key;
        char** enum_item_value;
    } data_type_enum_t;

    struct _data_type_text {
        char* length_str;
        int length; /* text length. */
        char* value; /* truly value. */
        char* unit; /* unit */
        char* unit_name; /* unitName */
    } data_type_text_t;

    struct _data_type_date {
        unsigned long long value; /* truly value. */
        char* value_str; /* string type value */
    } data_type_date_t;

    struct _data_type_bool {
        int bool_item_number; /* number of enum items. */
        int value; /* 0: false; 1: true. */
        char* value_str; /* string type value */
        char** bool_item_key;
        char** bool_item_value;
    } data_type_bool_t;

    struct _data_type_array {
        int size;
        data_type_type_t item_type;
        void* array;
        char** value_str;
    } data_type_array_t;
} data_type_x_t;
#endif

typedef struct _dataType {
    data_type_type_t type;  /* type */
    char* type_str; /* type, string format */
    size_t data_type_specs_number; /* useful when type = struct, otherwise 1 as default. */
    void* specs; /* specs, lite_property_t* when type = struct for property and event, otherwise cJSON*. */
    data_type_x_t value; /* actually store property values. used when type != struct */
} data_type_t;

typedef struct _lite_property {
    /* warning: do not move orders of first three items. */
    char* identifier; /* identifier */
    char* name; /* name */
    data_type_t data_type; /* dataType */

} lite_property_t;

typedef struct _property {
    /* warning: do not move orders of first three items. be same as lite_property_t. */
    char* identifier; /* identifier */
    char* name; /* name */
    data_type_t data_type; /* dataType */

    property_access_mode_t access_mode; /* accessMode */
    int required; /* required, 0: false, 1: true */
    char* desc; /* desc */
} property_t;

typedef union _input_data {
    property_t property_to_set; /* "method": "thing.service.property.set" */

    lite_property_t lite_property; /* "method": "thing.service.others" */

    char* property_to_get_name; /* "method": "thing.service.property.get" */
} input_data_t;

typedef struct _event {
    char* identifier; /* identifier */
    char* name; /* name */
    event_type_t event_type; /* type */
    char* event_type_str; /* type, string format */
    int required; /* 1 true or 0 false */
    char* desc; /* desc */
    size_t event_output_data_num; /* outputData number, default value 1 */
    lite_property_t* event_output_data; /* outputData */
    char* method; /* method */
} event_t;

typedef struct _service {
    char* identifier; /* identifier */
    char* name; /* name */
    int required; /* 1 true or 0 false */
    char* desc; /* desc */
    char* call_type; /* callType */

    service_type_t service_type;

    size_t service_input_data_num; /* inputData number */
    input_data_t* service_input_data; /* inputData */

    size_t service_output_data_num; /* outputData number */
    lite_property_t* service_output_data; /* outputData */
    char* method; /* method */
} service_t;

typedef struct _profile {
    char* product_key; /* productKey */
    char* device_name; /* deviceName */
} profile_t;

typedef struct _tsl_template {
    char* schema; /* schema */
    char* link; /* link */
    profile_t profile; /* profile */

    size_t property_number;
    property_t* properties; /* properties */

    size_t event_number;
    event_t* events; /* events */

    size_t service_number;
    service_t* services; /* services */
} tsl_template_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TSL_H */
