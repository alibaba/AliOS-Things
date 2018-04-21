#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#include "interface/thing_abstract.h"
#include "interface/list_abstract.h"
#include "dm_thing.h"
#include "dm_import.h"
#include "dm_logger.h"
#include "dm_slist.h"

#include "lite-utils.h"

#include "cJSON.h"

#define DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC 1

static const char string_dm_thing_class_name[] __DM_READ_ONLY__ =  "dm_thing_cls";
static const char string_property[] __DM_READ_ONLY__ = "property";
static const char string_service[] __DM_READ_ONLY__ = "service";
static const char string_event[] __DM_READ_ONLY__ = "event";
static const char string_properties[] __DM_READ_ONLY__ = "properties";
static const char string_services[] __DM_READ_ONLY__ = "services";
static const char string_events[] __DM_READ_ONLY__ = "events";
static const char string_text[] __DM_READ_ONLY__ = "text";
static const char string_enum[] __DM_READ_ONLY__ = "enum";
static const char string_bool[] __DM_READ_ONLY__ = "bool";
static const char string_float[] __DM_READ_ONLY__ = "float";
static const char string_double[] __DM_READ_ONLY__ = "double";
static const char string_int[] __DM_READ_ONLY__ = "int";
static const char string_date[] __DM_READ_ONLY__ = "date";
static const char string_struct[] __DM_READ_ONLY__ = "struct";
static const char string_min[] __DM_READ_ONLY__ = "min";
static const char string_max[] __DM_READ_ONLY__ = "max";
static const char string_length[] __DM_READ_ONLY__ = "length";
static const char string_type[] __DM_READ_ONLY__ = "type";
static const char string_identifier[] __DM_READ_ONLY__ = "identifier";
static const char string_method[] __DM_READ_ONLY__ = "method";
static const char string_callType[] __DM_READ_ONLY__ = "callType";
static const char string_name[] __DM_READ_ONLY__ = "name";
static const char string_specs[] __DM_READ_ONLY__ = "specs";
static const char string_accessMode[] __DM_READ_ONLY__ = "accessMode";
static const char string_required[] __DM_READ_ONLY__ = "required";
static const char string_dataType[] __DM_READ_ONLY__ = "dataType";
static const char string_desc[] __DM_READ_ONLY__ = "desc";
static const char string_outputData[] __DM_READ_ONLY__ = "outputData";
static const char string_inputData[] __DM_READ_ONLY__ = "inputData";
static const char string_info[] __DM_READ_ONLY__ = "info";
static const char string_alert[] __DM_READ_ONLY__ = "alert";
static const char string_error[] __DM_READ_ONLY__ = "error";
static const char string_thing_event_property_post[] __DM_READ_ONLY__ = "thing.event.property.post";
static const char string_thing_service_property_set[] __DM_READ_ONLY__ = "thing.service.property.set";
static const char string_thing_service_property_get[] __DM_READ_ONLY__ = "thing.service.property.get";
static const char string_profile_productKey[] __DM_READ_ONLY__ = "profile.productKey";
static const char string_profile_deviceName[] __DM_READ_ONLY__ = "profile.deviceName";
static const char string_input[] __DM_READ_ONLY__ = "input";
static const char string_output[] __DM_READ_ONLY__ = "output";
static const char string_false[] __DM_READ_ONLY__ = "false";
static const char string_true[] __DM_READ_ONLY__ = "true";

#ifdef USING_UTILS_JSON
#define KEY_BUFF_SIZE 32
static int install_cjson_item_string(void** dst, const char* key, const char* src, int src_len);
static void install_cjson_obj_property(property_t* dst, const char* src, int src_len);
static void install_cjson_obj_lite_property(void* dst, const char* src, int src_len);
#ifdef LITE_THING_MODEL
static void install_cjson_item_string_without_malloc(void* dst, const char *key, const char* src, int src_len);
#endif
#else
static int install_cjson_item_string(void** dst, const cJSON* const cjson_obj, const char* const item_name);
static void install_cjson_obj_property(property_t* dst, const cJSON* cjson_obj);
static void install_cjson_obj_lite_property(void* dst, const cJSON* const cjson_obj);
#ifdef LITE_THING_MODEL
static void install_cjson_item_string_without_malloc(void* dst, const cJSON* const cjson_obj, const char* const item_name);
#endif
#endif
#ifdef ENABLE_THING_DEBUG
static void property_print_data_type_info(void* dst);
static void item_print_info(void* _item, int index, va_list* params);
#endif /* ENABLE_THING_DEBUG */

void dm_lltoa(long long n, char* str, int radix)
{
    int i, j;
    long long remain;
    unsigned long long n_abs;
    char tmp_char;

    i = 0;

    if (n < 0) {
        str[i] = '-';
        str++;
        n_abs = -1 * n;
    } else {
        n_abs = n;
    }

    do {
        remain = n_abs % radix;
        if(remain > 9)
            str[i] = remain  - 10 + 'A';
        else
            str[i] = remain + '0';
        i++;
    } while(n_abs /= radix);

    str[i] = '\0';

    for(i-- , j = 0 ; j <= i ; j++ , i--)
    {
        tmp_char = str[j];
        str[j] = str[i];
        str[i] = tmp_char;
    }
    return;
}

static void free_item_memory(void* _item, int index, va_list* params);
static void free_lite_property(void* _lite_property);
static void free_property(void* _property, ...);

static void* dm_thing_ctor(void* _self, va_list* params)
{
    dm_thing_t* self = _self;

    self->_name = va_arg(*params, char*);
    self->_tsl_string_length = 0;
    self->_tsl_string = NULL;

    memset(&self->tsl_template, 0, sizeof(tsl_template_t));

    return self;
}

static void* dm_thing_dtor(void* _self)
{
    dm_thing_t* self = _self;

    property_iterator((thing_t*)self, free_item_memory, string_property, self->tsl_template.property_number);
    event_iterator((thing_t*)self, free_item_memory, string_event, self->tsl_template.event_number);
    service_iterator((thing_t*)self, free_item_memory, string_service, self->tsl_template.service_number);

    if (self->tsl_template.schema) {
        dm_lite_free(self->tsl_template.schema);
    }
    if (self->tsl_template.link) {
        dm_lite_free(self->tsl_template.link);
    }
    if (self->tsl_template.profile.device_name) {
        dm_lite_free(self->tsl_template.profile.device_name);
    }
    if (self->tsl_template.profile.product_key) {
        dm_lite_free(self->tsl_template.profile.product_key);
    }

    return self;
}

static void sprintf_float_double_precise(void* dst, double value, int precise)
{
    char temp_buff[48] = {0};
    char *point = NULL;
    assert(dst);

    dm_snprintf(temp_buff, sizeof(temp_buff), "%.16lf", value);
    point = strchr(temp_buff, '.');

    assert(point);

    if (!point) return;

    *(point + (precise == 0 ? 0 : precise + 1)) = 0;

    strcpy(dst, temp_buff);
}

static data_type_type_t detect_data_type_type(const char* const type_str)
{
    data_type_type_t type;
    if (strcmp(type_str, string_text) == 0) {
        type = data_type_type_text;
    } else if (strcmp(type_str, string_enum) == 0) {
        type = data_type_type_enum;
    } else if (strcmp(type_str, string_bool) == 0) {
        type = data_type_type_bool;
    } else if (strcmp(type_str, string_float) == 0) {
        type = data_type_type_float;
    } else if (strcmp(type_str, string_double) == 0) {
        type = data_type_type_double;
    } else if (strcmp(type_str, string_int) == 0) {
        type = data_type_type_int;
    } else if (strcmp(type_str, string_date) == 0) {
        type = data_type_type_date;
    } else if (strcmp(type_str, string_struct) == 0) {
        type = data_type_type_struct;
    } else {
        dm_printf("%s: unknown data type: %s\n", __func__, type_str);
        assert(0);
    }
    return type;
}

#ifdef USING_UTILS_JSON
static void install_cjson_item_data_type_value(void *dst, data_type_type_t type, const char *src, int src_len)
{
    data_type_x_t *data_type_x = (data_type_x_t *)dst;
    int index = 0;
    char temp_buf[24] = {0};
    char **current_key;
#ifndef LITE_THING_MODEL
    char **current_val;
#endif
    long long long_val;

    if (type == data_type_type_int) {
        /* min */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_min, src, src_len);
        data_type_x->data_type_int_t.min = atoi(temp_buf);
#else
        install_cjson_item_string((void **)&data_type_x->data_type_int_t.min_str, string_min, src, src_len);
        data_type_x->data_type_int_t.min = atoi(data_type_x->data_type_int_t.min_str);
#endif
        /* max */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_max, src, src_len);
        data_type_x->data_type_int_t.max = atoi(temp_buf);
#else
        install_cjson_item_string((void **)&data_type_x->data_type_int_t.max_str, string_max, src, src_len);
        data_type_x->data_type_int_t.max = atoi(data_type_x->data_type_int_t.max_str);
#endif

#if 0
        /* precise */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, "precise", src, src_len);
        if (strlen(temp_buf)) {
            data_type_x->data_type_int_t.precise = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void **)&data_type_x->data_type_int_t.precise_str, "precise", src, src_len);
        if (data_type_x->data_type_int_t.precise_str) {
            data_type_x->data_type_int_t.precise = atoi(data_type_x->data_type_int_t.precise_str);
        }
#endif
#else
        data_type_x->data_type_int_t.precise = 0;
#endif

#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void **)&data_type_x->data_type_int_t.unit, "unit", src, src_len);
        /* unitName */
        install_cjson_item_string((void **)&data_type_x->data_type_int_t.unit_name, "unitName", src, src_len);
#endif

        data_type_x->data_type_int_t.value = (data_type_x->data_type_int_t.min + data_type_x->data_type_int_t.max) / 2; /* default value? */

        long_val = data_type_x->data_type_int_t.max;
        dm_lltoa(long_val, temp_buf, 10);
        data_type_x->data_type_int_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + 2 + data_type_x->data_type_int_t.precise  + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);

        assert(data_type_x->data_type_int_t.value_str);
        dm_sprintf(data_type_x->data_type_int_t.value_str, "%d", data_type_x->data_type_int_t.value);
    } else if (type == data_type_type_float) {
        /* min */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_min, src, src_len);
        data_type_x->data_type_float_t.min = atof(temp_buf);
#else
        install_cjson_item_string((void **)&data_type_x->data_type_float_t.min_str, string_min, src, src_len);
        data_type_x->data_type_float_t.min = atof(data_type_x->data_type_float_t.min_str);
#endif
        /* max */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_max, src, src_len);
        data_type_x->data_type_float_t.max = atof(temp_buf);
#else
        install_cjson_item_string((void **)&data_type_x->data_type_float_t.max_str, string_max, src, src_len);
        data_type_x->data_type_float_t.max = atof(data_type_x->data_type_float_t.max_str);
#endif
#if 0
        /* precise */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, "precise", src, src_len);
        if (strlen(temp_buf)) {
            data_type_x->data_type_float_t.precise = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void **)&data_type_x->data_type_float_t.precise_str, "precise", src, src_len);
        if (data_type_x->data_type_float_t.precise_str) {
            data_type_x->data_type_float_t.precise = atoi(data_type_x->data_type_float_t.precise_str);
        }
#endif
#else
        data_type_x->data_type_float_t.precise = 7;
#endif


#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void **)&data_type_x->data_type_float_t.unit, "unit", src, src_len);
        /* unitName */
        install_cjson_item_string((void **)&data_type_x->data_type_float_t.unit_name, "unitName", src, src_len);
#endif
        data_type_x->data_type_float_t.value = (data_type_x->data_type_float_t.min + data_type_x->data_type_float_t.max) / 2; /* default value? */
        long_val = (long long)data_type_x->data_type_float_t.max;
        dm_lltoa(long_val, temp_buf, 10);
        data_type_x->data_type_float_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + 2 + data_type_x->data_type_float_t.precise + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_float_t.value_str);
        sprintf_float_double_precise(data_type_x->data_type_float_t.value_str, data_type_x->data_type_float_t.value,
                                     data_type_x->data_type_float_t.precise);
    } else if (type == data_type_type_double) {
        /* min */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_min, src, src_len);
        data_type_x->data_type_double_t.min = atof(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.min_str, string_min, src, src_len);
        data_type_x->data_type_double_t.min = atof(data_type_x->data_type_double_t.min_str);
#endif
        /* max */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_max, src, src_len);
        data_type_x->data_type_double_t.max = atof(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.max_str, string_max, src, src_len);
        data_type_x->data_type_double_t.max = atof(data_type_x->data_type_double_t.max_str);
#endif

#if 0
        /* precise */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, "precise");
        if (strlen(temp_buf)) {
            data_type_x->data_type_double_t.precise = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.precise_str, data_type_obj, "precise");
        if (data_type_x->data_type_double_t.precise_str) {
            data_type_x->data_type_double_t.precise = atoi(data_type_x->data_type_double_t.precise_str);
        }
#endif
#else
        data_type_x->data_type_double_t.precise = 16;
#endif

#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.unit, "unit", src, src_len);
        /* unitName */
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.unit_name, "unitName", src, src_len);
#endif
        data_type_x->data_type_double_t.value = (data_type_x->data_type_double_t.min + data_type_x->data_type_double_t.max) / 2; /* default value? */
        long_val = (long long)data_type_x->data_type_double_t.max;
        dm_lltoa(long_val, temp_buf, 10);
        data_type_x->data_type_double_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + 2 + data_type_x->data_type_double_t.precise + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_double_t.value_str);
        sprintf_float_double_precise(data_type_x->data_type_double_t.value_str, data_type_x->data_type_double_t.value, data_type_x->data_type_double_t.precise);
    } else if (type == data_type_type_enum) {
        /* detect enum item number */

        index = get_json_item_size((char*)src, src_len);

        data_type_x->data_type_enum_t.enum_item_number = index;

        if (data_type_x->data_type_enum_t.enum_item_number == 0) {
            data_type_x->data_type_enum_t.enum_item_key = NULL;
            data_type_x->data_type_enum_t.enum_item_value = NULL;
            return;
        }

#ifdef LITE_THING_MODEL
        data_type_x->data_type_enum_t.enum_item_key = (char**)dm_lite_calloc(1, data_type_x->data_type_enum_t.enum_item_number * sizeof(char **));
        assert(data_type_x->data_type_enum_t.enum_item_key);
        if (data_type_x->data_type_enum_t.enum_item_key == NULL) {
            data_type_x->data_type_enum_t.enum_item_number = 0;
            return;
        }
#else
        data_type_x->data_type_enum_t.enum_item_key = (char**)dm_lite_calloc(1, data_type_x->data_type_enum_t.enum_item_number * sizeof(char **));
        data_type_x->data_type_enum_t.enum_item_value = (char**)dm_lite_calloc(1, data_type_x->data_type_enum_t.enum_item_number * sizeof(char **));

        assert(data_type_x->data_type_enum_t.enum_item_key && data_type_x->data_type_enum_t.enum_item_value);
        if (data_type_x->data_type_enum_t.enum_item_key == NULL || data_type_x->data_type_enum_t.enum_item_value == NULL) {
            data_type_x->data_type_enum_t.enum_item_number = 0;
            return;
        }
#endif
        for (index = 0; index < data_type_x->data_type_enum_t.enum_item_number; ++index) {
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", index);
            current_key = data_type_x->data_type_enum_t.enum_item_key + index;
            *current_key = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
            assert(*current_key);
            strcpy(*current_key, temp_buf);
#ifndef LITE_THING_MODEL
            current_val = data_type_x->data_type_enum_t.enum_item_value + index;
            install_cjson_item_string((void **)current_val, temp_buf, src, src_len);
#endif
        }
        data_type_x->data_type_enum_t.value = atoi(*data_type_x->data_type_enum_t.enum_item_key);
        data_type_x->data_type_enum_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_enum_t.value_str);
        dm_sprintf(data_type_x->data_type_enum_t.value_str, "%d", data_type_x->data_type_enum_t.value);

    } else if (type == data_type_type_bool) {
        data_type_x->data_type_bool_t.bool_item_number = 2;
        data_type_x->data_type_bool_t.bool_item_key = (char**)dm_lite_calloc(1, data_type_x->data_type_bool_t.bool_item_number * sizeof(char **));
#ifndef LITE_THING_MODEL
        data_type_x->data_type_bool_t.bool_item_value = (char**)dm_lite_calloc(1, data_type_x->data_type_bool_t.bool_item_number * sizeof(char **));

        assert(data_type_x->data_type_bool_t.bool_item_key && data_type_x->data_type_bool_t.bool_item_value);
#else
        assert(data_type_x->data_type_bool_t.bool_item_key);
#endif
        for (index = 0; index < data_type_x->data_type_bool_t.bool_item_number; ++index) {
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", index);
            current_key = data_type_x->data_type_bool_t.bool_item_key + index;
            *current_key = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
            assert(*current_key);
            strcpy(*current_key, temp_buf);
#ifndef LITE_THING_MODEL
            current_val = data_type_x->data_type_bool_t.bool_item_value + index;
            install_cjson_item_string((void **)current_val, temp_buf, src, src_len);
#endif
        }

        data_type_x->data_type_bool_t.value = 1; /* default value. */

        data_type_x->data_type_bool_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_bool_t.value_str);
        dm_sprintf(data_type_x->data_type_bool_t.value_str, "%d", data_type_x->data_type_bool_t.value);
    } else if (type == data_type_type_text) {
        /* length */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, string_length, src, src_len);
        if (strlen(temp_buf)) {
            data_type_x->data_type_text_t.length = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void **)&data_type_x->data_type_text_t.length_str, string_length, src, src_len);
        if (data_type_x->data_type_text_t.length_str) {
            data_type_x->data_type_text_t.length = atoi(data_type_x->data_type_text_t.length_str);
        }
#endif
        if (data_type_x->data_type_text_t.length < 1) {
            data_type_x->data_type_text_t.length = 1;
        }
#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void **)&data_type_x->data_type_text_t.unit, "unit", src, src_len);
        /* unitName */
        install_cjson_item_string((void **)&data_type_x->data_type_text_t.unit_name, "unitName", src, src_len);
#endif
        data_type_x->data_type_text_t.value = NULL; /* default value. */
    } else if (type == data_type_type_date) {
        data_type_x->data_type_date_t.value = 1513406265000; /* ms, UTC. default value. */
        dm_lltoa(__LONG_LONG_MAX__, temp_buf, 10);
        data_type_x->data_type_date_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_date_t.value_str);
        dm_lltoa(data_type_x->data_type_date_t.value, data_type_x->data_type_date_t.value_str, 10);
    } else {
        assert(0);
    }
}

static void install_cjson_item_data_type(void *dst, const char *key, const char *src, int src_len)
{
    char *val = NULL, *dm_val = NULL;
    char _key_index[KEY_BUFF_SIZE] = {0};
    int val_len = 0, dm_len = 0;
    data_type_t *data_type = (data_type_t *)dst;
    size_t specs_array_size, index;
    size_t size;
    lite_property_t *lite_property_item;

    val = LITE_json_value_of_ext2((char*)key, (char*)src, src_len, &val_len);
    if (!val) {
        return;
    }

    dm_val = LITE_json_value_of_ext2((char*)string_type, val, val_len, &dm_len);
    if (dm_val) {
        size = dm_len + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
        data_type->type_str = dm_lite_calloc(1, size);
        assert(data_type->type_str);
        memset(data_type->type_str, 0, size);
        strncpy(data_type->type_str, dm_val, size - 1);
        data_type->type = detect_data_type_type(data_type->type_str);
    }

    dm_val = LITE_json_value_of_ext2((char*)string_specs, val, val_len, &dm_len);
    if (!dm_val) {
        return;
    }
    if (data_type->type != data_type_type_struct) {
        if ('{' != *dm_val) {
            return;
        }
        data_type->data_type_specs_number = 0;
        data_type->specs = NULL;
        install_cjson_item_data_type_value((void *)&data_type->value, data_type->type, dm_val, dm_len);
    } else {
        char *spec_val = NULL;
        int spec_len = 0;
        if ('[' != *dm_val) {
            return;
        }
        specs_array_size = get_json_item_size(dm_val, dm_len);

        data_type->data_type_specs_number = specs_array_size;
        if (data_type->data_type_specs_number == 0) {
            data_type->specs = NULL;
        } else {
            data_type->specs = dm_lite_calloc(data_type->data_type_specs_number, sizeof(lite_property_t));
            assert(data_type->specs);
        }

        memset(&data_type->value, 0, sizeof(data_type_x_t));

        for (index = 0; index < data_type->data_type_specs_number; ++index) {
            lite_property_item = (lite_property_t *)data_type->specs + index;
            snprintf(_key_index, KEY_BUFF_SIZE - 1, "[%u]", index + 1);
            spec_val = LITE_json_value_of_ext2(_key_index, dm_val, dm_len, &spec_len);
            if (spec_val) {
                lite_property_item = (lite_property_t *)data_type->specs + index;
                install_cjson_obj_lite_property(lite_property_item, spec_val, spec_len);
            }
        }
    }
}

static void install_cjson_obj_lite_property(void *dst, const char *src, int src_len)
{
    lite_property_t *lite_property_item = (lite_property_t *)dst;

    install_cjson_item_string((void **)&lite_property_item->identifier, string_identifier, src, src_len);
#ifndef LITE_THING_MODEL
    install_cjson_item_string((void **)&lite_property_item->name, string_name, src, src_len);
#endif
    install_cjson_item_data_type(&lite_property_item->data_type, string_dataType, src, src_len);
}

static void install_cjson_item_input_data(void *dst, service_type_t service_type, const char *src, int src_len)
{
    input_data_t *input_data_item = (input_data_t *)dst;
    int property_to_get_name_size = 0;
    if (service_type == service_type_others) {
        install_cjson_obj_lite_property(&input_data_item->lite_property, src, src_len);
    } else if (service_type == service_type_property_set) {
        install_cjson_obj_property(&input_data_item->property_to_set, src, src_len);
    } else if (service_type == service_type_property_get) {
        property_to_get_name_size = src_len + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
        input_data_item->property_to_get_name = (char*)dm_lite_calloc(1, property_to_get_name_size);
        strncpy(input_data_item->property_to_get_name, src, property_to_get_name_size - 1);
    }
}

static int install_cjson_item_string(void **dst, const char *key, const char *src, int src_len)
{
    int val_len = 0;
    char *val = LITE_json_value_of_ext2((char*)key, (char*)src, src_len, &val_len);
    size_t size;

    if (val) {
        size = val_len + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
        *dst = dm_lite_calloc(1, size);
        assert(*dst);
        memset(*dst, 0, size);
        strncpy(*dst, val, val_len);
    } else {
        dm_log_err("parse key:%s from val:%.*s failed", key, src_len, src);
    }

    return val ? 0 : -1;
}

#ifdef LITE_THING_MODEL
static void install_cjson_item_string_without_malloc(void *dst, const char *key, const char *src, int src_len)
{
    int val_len = 0;
    char *val = LITE_json_value_of_ext2((char*)key, (char*)src, src_len, &val_len);
    if (val) {
        strncpy(dst, val, val_len);
    } else {
        dm_log_err("parse key:%s from val:%.*s failed", key, src_len, src);
    }
    return;
}
#endif

static void install_cjson_item_bool(void *dst, const char *key, const char *src, int src_len)
{
    char *bool_val = NULL;
    int bool_val_len = 0;
    int *val = dst;

    *val = 1;
    bool_val = LITE_json_value_of_ext2((char*)key, (char*)src, src_len, &bool_val_len);

    if (bool_val) {
        if (!strncmp(bool_val, string_false, bool_val_len)) {
            *val = 0;
        }
    }

    return;
}

static void install_cjson_obj_property(property_t *dst, const char *src, int src_len)
{
    property_t *property = dst;
    char *temp_str = NULL;

    /* properties.identifier */
    install_cjson_item_string((void **)&property->identifier, string_identifier, src, src_len);
#ifdef LITE_THING_MODEL
    property->name = NULL;
    property->desc = NULL;
#else
    /* properties.name */
    install_cjson_item_string((void **)&property->name, string_name, src, src_len);
    /* properties.desc */
    install_cjson_item_string((void **)&property->desc, string_desc, src, src_len);
#endif
    /* properties.required */
    install_cjson_item_bool((void *)&property->required, string_required, src, src_len);
    /* properties.accessMode */
    install_cjson_item_string((void **)&temp_str, string_accessMode, src, src_len);
    if (temp_str) {
        property->access_mode = strlen(temp_str) == 2 ? property_access_mode_rw :
                                                        (strcmp(temp_str, "r") == 0 ? property_access_mode_r : property_access_mode_w);
        dm_lite_free(temp_str);
    }
    install_cjson_item_data_type(&property->data_type, string_dataType, src, src_len);
    return;
}

static void parse_cjson_obj_to_tsl_template_properties(void *_self, const char *src, int src_len)
{
    char _key[KEY_BUFF_SIZE] = {0};
    char *val = NULL;
    int val_len = 0;

    dm_thing_t *self = _self;
    int properties_array_size;
    int index;
    property_t *property;

    properties_array_size = get_json_item_size((char*)src, src_len);

    self->tsl_template.property_number = properties_array_size;

    if (self->tsl_template.property_number == 0) {
        return;
    }

    self->tsl_template.properties = (property_t *)dm_lite_calloc(self->tsl_template.property_number, sizeof(property_t));

    assert(self->tsl_template.properties);
    if (self->tsl_template.properties == 0) {
        self->tsl_template.property_number = 0;
        return;
    }

    if (src) {
        for (index = 0; index < self->tsl_template.property_number; ++index) {
            snprintf(_key, KEY_BUFF_SIZE - 1, "[%d]", index + 1);
            property = self->tsl_template.properties + index;

            val = LITE_json_value_of_ext2(_key, (char*)src, src_len, &val_len);
            if (val) {
                install_cjson_obj_property(property, val, val_len);
            }

        }
    }
}


static void install_cjson_obj_event(event_t *dst, const char *src, int src_len)
{
    char _key_index[KEY_BUFF_SIZE] = {0};
    char *val = NULL;
    int val_len = 0;
    char *dm_val = NULL;
    int dm_val_len = 0;
    event_t *event = dst;
    int output_data_array_size;
    int output_data_index;
    lite_property_t *output_data_item;

    /* events.identifier */
    install_cjson_item_string((void **)&event->identifier, string_identifier, src, src_len);
#ifndef LITE_THING_MODEL
    /* events.name */
    install_cjson_item_string((void **)&event->name, string_name, src, src_len);
    /* events.desc */
    install_cjson_item_string((void **)&event->desc, string_desc, src, src_len);
#endif
    /* events.method */
    install_cjson_item_string((void **)&event->method, string_method, src, src_len);
    /* events.required */
    install_cjson_item_bool((void *)&event->required, string_required, src, src_len);
    /* events.type */
    install_cjson_item_string((void **)&event->event_type_str, string_type, src, src_len);
    if (strcmp(event->event_type_str, string_info) == 0) {
        event->event_type = event_type_info;
    } else if (strcmp(event->event_type_str, string_alert) == 0) {
        event->event_type = event_type_alert;
    } else if (strcmp(event->event_type_str, string_error) == 0) {
        event->event_type = event_type_error;
    }

#ifdef LITE_THING_MODEL
    if (strcmp(event->method, string_thing_event_property_post) == 0) {
        return;
    }
#endif

    val = LITE_json_value_of_ext2((char*)string_outputData, (char*)src, src_len, &val_len);
    if (val) {
        output_data_array_size = get_json_item_size(val, val_len);
        event->event_output_data_num = output_data_array_size;
        if (event->event_output_data_num == 0) {
            return;
        }

        event->event_output_data = (lite_property_t *)dm_lite_calloc(event->event_output_data_num, sizeof(lite_property_t));
        assert(event->event_output_data);

        if (event->event_output_data == NULL) {
            event->event_output_data_num = 0;
            return;
        }
    }

    for (output_data_index = 0; output_data_index < event->event_output_data_num; ++output_data_index) {
        output_data_item = event->event_output_data + output_data_index;
        snprintf(_key_index, KEY_BUFF_SIZE - 1, "[%d]", output_data_index + 1);
        dm_val = LITE_json_value_of_ext2(_key_index, val, val_len, &dm_val_len);
        if (dm_val) {
            install_cjson_obj_lite_property(output_data_item, dm_val, dm_val_len);
        }
    }
}

static void parse_cjson_obj_to_tsl_template_events(void *_self, const char *src, int src_len)
{
    char *val = NULL;
    char _key[KEY_BUFF_SIZE] = {0};
    dm_thing_t *self = _self;
    int events_array_size;
    int index;
    int val_len = 0;
    event_t *event;

    events_array_size = get_json_item_size((char*)src, src_len);

    self->tsl_template.event_number = events_array_size;

    if (self->tsl_template.event_number == 0) {
        return;
    }

    self->tsl_template.events = (event_t *)dm_lite_calloc(self->tsl_template.event_number, sizeof(event_t));

    assert(self->tsl_template.events);
    if (self->tsl_template.events == NULL) {
        self->tsl_template.event_number = 0;
        dm_log_err("events malloc fail");
        return;
    }

    for (index = 0; index < self->tsl_template.event_number; ++index) {
        event = self->tsl_template.events + index;
        snprintf(_key, KEY_BUFF_SIZE - 1, "[%d]", index + 1);
        val = LITE_json_value_of_ext2(_key, (char*)src, src_len, &val_len);
        if (val) {
            install_cjson_obj_event(event, val, val_len);
        }
    }
}

static void install_cjson_obj_service(service_t *dst, const char *src, int src_len)
{
    char _key_index[KEY_BUFF_SIZE] = {0};
    char *val = NULL;
    int val_len = 0;
    char *dm_val = NULL;
    int dm_val_len = 0;

    service_t *service = dst;
    int output_data_array_size, input_data_array_size;
    int output_data_index, input_data_index;
    lite_property_t *output_data_item;
    input_data_t *input_data_item;

    /* service.identifier */
    install_cjson_item_string((void **)&service->identifier, string_identifier, src, src_len);
#ifndef LITE_THING_MODEL
    /* service.name */
    install_cjson_item_string((void **)&service->name, string_name, src, src_len);
    /* service.desc */
    install_cjson_item_string((void **)&service->desc, string_desc, src, src_len);
#endif
    /* service.method */
    install_cjson_item_string((void **)&service->method, string_method, src, src_len);
    /* service.call_type */
    install_cjson_item_string((void **)&service->call_type, string_callType, src, src_len);
    /* service.required */
    install_cjson_item_bool((void *)&service->required, string_required, src, src_len);

    if (strcmp(service->method, string_thing_service_property_set) == 0) {
        service->service_type = service_type_property_set;
    } else if (strcmp(service->method, string_thing_service_property_get) == 0) {
        service->service_type = service_type_property_get;
    } else {
        service->service_type = service_type_others;
    }

    /* output data process */
    val = LITE_json_value_of_ext2((char*)string_outputData, (char*)src, src_len, &val_len);
#ifdef LITE_THING_MODEL
    if (val && (service->service_type != service_type_property_get))
#else
    if (val)
#endif
    {
        output_data_array_size = get_json_item_size(val, val_len);
        service->service_output_data_num = output_data_array_size;

        if (service->service_output_data_num) {
            if (service->service_output_data_num) {
                service->service_output_data = (lite_property_t *)dm_lite_calloc(service->service_output_data_num,
                                                                                 sizeof(lite_property_t));
                assert(service->service_output_data);
                if (service->service_output_data == NULL) {
                    service->service_output_data_num = 0;
                }
            } else {
                service->service_output_data = NULL;
            }
        }
    }

    for (output_data_index = 0; output_data_index < service->service_output_data_num; ++output_data_index) {
        output_data_item = service->service_output_data + output_data_index;
        snprintf(_key_index, KEY_BUFF_SIZE, "[%d]", output_data_index + 1);
        dm_val = LITE_json_value_of_ext2(_key_index, val, val_len, &dm_val_len);
        install_cjson_obj_lite_property(output_data_item, dm_val, dm_val_len);
    }

    /* input data process */
    service->service_input_data_num = 0;
    val = LITE_json_value_of_ext2((char*)string_inputData, (char*)src, src_len, &val_len);
#ifdef LITE_THING_MODEL
    if (val && (service->service_type != service_type_property_set))
#else
    if (val)
#endif
    {
        input_data_array_size = get_json_item_size(val, val_len);
        service->service_input_data_num = input_data_array_size;
        service->service_input_data = (input_data_t *)dm_lite_calloc(service->service_input_data_num, sizeof(input_data_t));
        if (service->service_input_data == NULL) {
            service->service_input_data_num = 0;
        }
    }
    for (input_data_index = 0; input_data_index < service->service_input_data_num; ++input_data_index) {
        input_data_item = service->service_input_data + input_data_index;
        snprintf(_key_index, KEY_BUFF_SIZE, "[%d]", input_data_index + 1);
        dm_val = LITE_json_value_of_ext2(_key_index, val, val_len, &dm_val_len);
        if (dm_val) {
            install_cjson_item_input_data(input_data_item, service->service_type, dm_val, dm_val_len);
        }
    }
}

static void parse_cjson_obj_to_tsl_template_services(void *_self, const char *src, int src_len)
{
    char _key[KEY_BUFF_SIZE] = {0};
    dm_thing_t *self = _self;

    int sercives_array_size;
    int index;
    service_t *service;

    char *val = NULL;
    int val_len = 0;

    sercives_array_size = get_json_item_size((char*)src, src_len);

    self->tsl_template.service_number = sercives_array_size;
    self->tsl_template.services = (service_t *)dm_lite_calloc(self->tsl_template.service_number, sizeof(service_t));

    if (self->tsl_template.services == NULL) {
        self->tsl_template.service_number = 0;
        dm_log_err("self->tsl_template.events malloc fail");
        return;
    }

    for (index = 0; index < self->tsl_template.service_number; ++index) {
        service = self->tsl_template.services + index;
        snprintf(_key, KEY_BUFF_SIZE, "[%d]", index + 1);
        val = LITE_json_value_of_ext2(_key, (char*)src, src_len, &val_len);
        if (val) {
            install_cjson_obj_service(service, val, val_len);
        }
    }
}

static int parse_cjson_obj_to_tsl_template(void *_self, const char *src, int src_len)
{
    if(!_self || !src || src_len <= 0) {
        return -1;
    }

    dm_thing_t *self = _self;
    char *val = NULL;
    int val_len = 0;
#ifdef LITE_THING_MODEL
    self->tsl_template.schema = NULL;
    self->tsl_template.link = NULL;
#else
    /* schema */
    if(0 != install_cjson_item_string((void **)&self->tsl_template.schema, "schema", src, src_len)) {
        return -1;
    }
    /* link */
    if(0 != install_cjson_item_string((void **)&self->tsl_template.link, "link", src, src_len)) {
        return -1;
    }
#endif

    /* profile */
    if(0 != install_cjson_item_string((void **)&self->tsl_template.profile.product_key, string_profile_productKey, src, src_len)) {
        self->tsl_template.profile.product_key = NULL;
    }
    if(0 != install_cjson_item_string((void **)&self->tsl_template.profile.device_name, string_profile_deviceName, src, src_len)) {
        self->tsl_template.profile.device_name = NULL;
    }

    /* properties */
    val = LITE_json_value_of_ext2((char*)string_properties, (char*)src, src_len, &val_len);
    if (val) {
        parse_cjson_obj_to_tsl_template_properties(self, val, val_len);
    } else {
        return -1;
    }

    /* events */
    val = LITE_json_value_of_ext2((char*)string_events, (char*)src, src_len, &val_len);
    if (val) {
        parse_cjson_obj_to_tsl_template_events(self, val, val_len);
    } else {
        return -1;
    }

    /* services */
    val = LITE_json_value_of_ext2((char*)string_services, (char*)src, src_len, &val_len);
    if (val) {
        parse_cjson_obj_to_tsl_template_services(self, val, val_len);
    } else {
        return -1;
    }

    return 0;
}

static int dm_thing_set_tsl_string(void *_self, const char *src, int src_len)
{
    int ret;
    dm_thing_t *self = _self;
    ret = parse_cjson_obj_to_tsl_template(self, src, src_len);

    return ret;
}

#else
static void install_cjson_item_data_type_value(void* dst, data_type_type_t type, const cJSON* const cjson_obj)
{
    data_type_x_t* data_type_x = (data_type_x_t*)dst;
    const cJSON* const data_type_obj = cjson_obj;
    cJSON* enum_item_obj;
    int index = 0;
    int enum_number = 0;
    int enum_index = 0;
    char temp_buf[24] = {0};
    char** current_key;
#ifndef LITE_THING_MODEL
    char** current_val;
#endif
    long long long_val;

    if (type == data_type_type_int) {
        /* min */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_min);
        data_type_x->data_type_int_t.min = atoi(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_int_t.min_str, data_type_obj, string_min);
        data_type_x->data_type_int_t.min = atoi(data_type_x->data_type_int_t.min_str);
#endif

        /* max */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_max);
        data_type_x->data_type_int_t.max = atoi(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_int_t.max_str, data_type_obj, string_max);
        data_type_x->data_type_int_t.max = atoi(data_type_x->data_type_int_t.max_str);
#endif

#if 0
        /* precise */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, "precise");
        if (strlen(temp_buf)) {
            data_type_x->data_type_int_t.precise = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void**)&data_type_x->data_type_int_t.precise_str, data_type_obj, "precise");
        if (data_type_x->data_type_int_t.precise_str) {
            data_type_x->data_type_int_t.precise = atoi(data_type_x->data_type_int_t.precise_str);
        }
#endif
#else
        data_type_x->data_type_int_t.precise = 0;
#endif

#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void**)&data_type_x->data_type_int_t.unit, data_type_obj, "unit");
        /* unitName */
        install_cjson_item_string((void**)&data_type_x->data_type_int_t.unit_name, data_type_obj, "unitName");
#endif
        data_type_x->data_type_int_t.value = (data_type_x->data_type_int_t.min + data_type_x->data_type_int_t.max) / 2; /* default value? */

        long_val = data_type_x->data_type_int_t.max;
        dm_lltoa(long_val, temp_buf, 10);
        data_type_x->data_type_int_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + 2 + data_type_x->data_type_int_t.precise  + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_int_t.value_str);
        dm_sprintf(data_type_x->data_type_int_t.value_str, "%d", data_type_x->data_type_int_t.value);
    } else if (type == data_type_type_float) {
        /* min */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_min);
        data_type_x->data_type_float_t.min = atof(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_float_t.min_str, data_type_obj, string_min);
        data_type_x->data_type_float_t.min = atof(data_type_x->data_type_float_t.min_str);
#endif
        /* max */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_max);
        data_type_x->data_type_float_t.max = atof(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_float_t.max_str, data_type_obj, string_max);
        data_type_x->data_type_float_t.max = atof(data_type_x->data_type_float_t.max_str);
#endif

#if 0
        /* precise */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, "precise");
        if (strlen(temp_buf)) {
            data_type_x->data_type_float_t.precise = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void**)&data_type_x->data_type_float_t.precise_str, data_type_obj, "precise");
        if (data_type_x->data_type_float_t.precise_str) {
            data_type_x->data_type_float_t.precise = atoi(data_type_x->data_type_float_t.precise_str);
        }
#endif
#else
        data_type_x->data_type_float_t.precise = 7;
#endif

#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void**)&data_type_x->data_type_float_t.unit, data_type_obj, "unit");
        /* unitName */
        install_cjson_item_string((void**)&data_type_x->data_type_float_t.unit_name, data_type_obj, "unitName");
#endif
        data_type_x->data_type_float_t.value = (data_type_x->data_type_float_t.min + data_type_x->data_type_float_t.max) / 2; /* default value? */
        long_val = (long long)data_type_x->data_type_float_t.max;
        dm_lltoa(long_val, temp_buf, 10);
        data_type_x->data_type_float_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + 2 + data_type_x->data_type_float_t.precise + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_float_t.value_str);
        sprintf_float_double_precise(data_type_x->data_type_float_t.value_str, data_type_x->data_type_float_t.value, data_type_x->data_type_float_t.precise);
    } else if (type == data_type_type_double) {
        /* min */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_min);
        data_type_x->data_type_double_t.min = atof(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.min_str, data_type_obj, string_min);
        data_type_x->data_type_double_t.min = atof(data_type_x->data_type_double_t.min_str);
#endif
        /* max */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_max);
        data_type_x->data_type_double_t.max = atof(temp_buf);
#else
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.max_str, data_type_obj, string_max);
        data_type_x->data_type_double_t.max = atof(data_type_x->data_type_double_t.max_str);
#endif

#if 0
        /* precise */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, "precise");
        if (strlen(temp_buf)) {
            data_type_x->data_type_double_t.precise = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.precise_str, data_type_obj, "precise");
        if (data_type_x->data_type_double_t.precise_str) {
            data_type_x->data_type_double_t.precise = atoi(data_type_x->data_type_double_t.precise_str);
        }
#endif
#else
        data_type_x->data_type_double_t.precise = 16;
#endif

#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.unit, data_type_obj, "unit");
        /* unitName */
        install_cjson_item_string((void**)&data_type_x->data_type_double_t.unit_name, data_type_obj, "unitName");
#endif
        data_type_x->data_type_double_t.value = (data_type_x->data_type_double_t.min + data_type_x->data_type_double_t.max) / 2; /* default value? */
        long_val = (long long)data_type_x->data_type_double_t.max;
        dm_lltoa(long_val, temp_buf, 10);
        data_type_x->data_type_double_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + 2 + data_type_x->data_type_double_t.precise + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_double_t.value_str);
        sprintf_float_double_precise(data_type_x->data_type_double_t.value_str, data_type_x->data_type_double_t.value, data_type_x->data_type_double_t.precise);
    } else if (type == data_type_type_enum) {
        /* find enum number */
        for (index = 0; index < 100; ++index) {
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", index);
            enum_item_obj = cJSON_GetObjectItem(data_type_obj, temp_buf);
            if (enum_item_obj) {
                enum_number++;
            }
        }

        data_type_x->data_type_enum_t.enum_item_number = enum_number;
        if (data_type_x->data_type_enum_t.enum_item_number == 0) {
            data_type_x->data_type_enum_t.enum_item_key = NULL;
            data_type_x->data_type_enum_t.enum_item_value = NULL;
            return;
        }
#ifdef LITE_THING_MODEL
        data_type_x->data_type_enum_t.enum_item_key = (char**)dm_lite_calloc(1, data_type_x->data_type_enum_t.enum_item_number * sizeof(char**));

        assert(data_type_x->data_type_enum_t.enum_item_key);
        if (data_type_x->data_type_enum_t.enum_item_key == NULL) {
            data_type_x->data_type_enum_t.enum_item_number = 0;
            return;
        }
#else
        data_type_x->data_type_enum_t.enum_item_key = (char**)dm_lite_calloc(1, data_type_x->data_type_enum_t.enum_item_number * sizeof(char**));
        data_type_x->data_type_enum_t.enum_item_value = (char**)dm_lite_calloc(1, data_type_x->data_type_enum_t.enum_item_number * sizeof(char**));

        assert(data_type_x->data_type_enum_t.enum_item_key && data_type_x->data_type_enum_t.enum_item_value);
        if (data_type_x->data_type_enum_t.enum_item_key == NULL || data_type_x->data_type_enum_t.enum_item_value == NULL) {
            data_type_x->data_type_enum_t.enum_item_number = 0;
            return;
        }
#endif

        for (index = 0; index < 100; ++index) {
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", index);
            enum_item_obj = cJSON_GetObjectItem(data_type_obj, temp_buf);
            if (enum_item_obj) {
                current_key = data_type_x->data_type_enum_t.enum_item_key + enum_index;
                *current_key = dm_lite_calloc(1, strlen(temp_buf) + 1);
                assert(*current_key);
                strcpy(*current_key, temp_buf);
#ifndef LITE_THING_MODEL
                current_val = data_type_x->data_type_enum_t.enum_item_value + enum_index++;
                install_cjson_item_string((void**)current_val, data_type_obj, temp_buf);
#else
                enum_index++;
#endif
            }
        }

        data_type_x->data_type_enum_t.value = atoi(*data_type_x->data_type_enum_t.enum_item_key); /* default value. */
        data_type_x->data_type_enum_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_enum_t.value_str);
        dm_sprintf(data_type_x->data_type_enum_t.value_str, "%d", data_type_x->data_type_enum_t.value);

    } else if (type == data_type_type_bool) {
        data_type_x->data_type_bool_t.bool_item_number = 2;
        data_type_x->data_type_bool_t.bool_item_key = (char**)dm_lite_calloc(1, data_type_x->data_type_bool_t.bool_item_number * sizeof(char**));
#ifndef LITE_THING_MODEL
        data_type_x->data_type_bool_t.bool_item_value = (char**)dm_lite_calloc(1, data_type_x->data_type_bool_t.bool_item_number * sizeof(char**));

        assert(data_type_x->data_type_bool_t.bool_item_key && data_type_x->data_type_bool_t.bool_item_value);
#else
        assert(data_type_x->data_type_bool_t.bool_item_key);
#endif
        for (index = 0; index < data_type_x->data_type_bool_t.bool_item_number; ++index) {
            dm_snprintf(temp_buf, sizeof(temp_buf), "%d", index);
            current_key = data_type_x->data_type_bool_t.bool_item_key + index;
            *current_key = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
            assert(*current_key);
            strcpy(*current_key, temp_buf);
#ifndef LITE_THING_MODEL
            current_val = data_type_x->data_type_bool_t.bool_item_value + index;
            install_cjson_item_string((void**)current_val, data_type_obj, temp_buf);
#endif
        }

        data_type_x->data_type_bool_t.value = 1; /* default value. */

        data_type_x->data_type_bool_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_bool_t.value_str);
        dm_sprintf(data_type_x->data_type_bool_t.value_str, "%d", data_type_x->data_type_bool_t.value);
    } else if (type == data_type_type_text) {
        /* length */
#ifdef LITE_THING_MODEL
        memset(temp_buf, 0, sizeof(temp_buf));
        install_cjson_item_string_without_malloc(temp_buf, data_type_obj, string_length);
        if (strlen(temp_buf)) {
            data_type_x->data_type_text_t.length = atoi(temp_buf);
        }
#else
        install_cjson_item_string((void**)&data_type_x->data_type_text_t.length_str, data_type_obj, string_length);
        if (data_type_x->data_type_text_t.length_str) {
            data_type_x->data_type_text_t.length = atoi(data_type_x->data_type_text_t.length_str);
        }
#endif
        if (data_type_x->data_type_text_t.length < 1) data_type_x->data_type_text_t.length = 1;
#ifndef LITE_THING_MODEL
        /* unit */
        install_cjson_item_string((void**)&data_type_x->data_type_text_t.unit, data_type_obj, "unit");
        /* unitName */
        install_cjson_item_string((void**)&data_type_x->data_type_text_t.unit_name, data_type_obj, "unitName");
#endif
        data_type_x->data_type_text_t.value = NULL; /* default value. */
    } else if (type == data_type_type_date) {
        data_type_x->data_type_date_t.value = 1513406265000; /* ms, UTC. default value. */
        dm_lltoa(__LONG_LONG_MAX__, temp_buf, 10);
        data_type_x->data_type_date_t.value_str = dm_lite_calloc(1, strlen(temp_buf) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC);
        assert(data_type_x->data_type_date_t.value_str);
        dm_lltoa(data_type_x->data_type_date_t.value, data_type_x->data_type_date_t.value_str, 10);
    } else {
        assert(0);
    }
}

static void install_cjson_item_data_type(void* dst, const cJSON* const cjson_obj, const char* const item_name)
{
    const cJSON* const root_obj = cjson_obj;
    data_type_t* data_type = (data_type_t*)dst;
    cJSON* data_type_obj = NULL;
    cJSON* type_obj;
    cJSON* specs_obj;
    cJSON* specs_struct_item_obj;
    size_t size, specs_array_size, index;
    lite_property_t* lite_property_item;

    data_type_obj = cJSON_GetObjectItem(root_obj, item_name);
    if (!data_type_obj || !cJSON_IsObject(data_type_obj)) {
        return;
    }

    type_obj = cJSON_GetObjectItem(data_type_obj, string_type);
    if (type_obj && cJSON_IsString(type_obj)) {
#ifdef CJSON_STRING_ZEROCOPY
        size = type_obj->valuestring_length + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#else
        size = strlen(type_obj->valuestring) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#endif
        data_type->type_str = dm_lite_calloc(1, size);
        assert(data_type->type_str);
        memset(data_type->type_str, 0, size);
#ifdef CJSON_STRING_ZEROCOPY
        strncpy(data_type->type_str, type_obj->valuestring, size - 1);
#else
        strcpy(data_type->type_str, type_obj->valuestring);
#endif
        data_type->type = detect_data_type_type(data_type->type_str);
    }

    specs_obj =  cJSON_GetObjectItem(data_type_obj, string_specs);
    if (!specs_obj) {
        return;
    }
    if (data_type->type != data_type_type_struct) {
        assert(cJSON_IsObject(specs_obj));
        if (!cJSON_IsObject(specs_obj)) {
            return;
        }
        data_type->data_type_specs_number = 0;
        data_type->specs = NULL;
        install_cjson_item_data_type_value((void*)&data_type->value, data_type->type, specs_obj);
    } else {
        assert(cJSON_IsArray(specs_obj));
        if (!cJSON_IsArray(specs_obj)) {
            return;
        }

        specs_array_size = cJSON_GetArraySize(specs_obj);
        data_type->data_type_specs_number = specs_array_size;
        if (data_type->data_type_specs_number == 0) {
            data_type->specs = NULL;
        } else {
            data_type->specs = dm_lite_calloc(data_type->data_type_specs_number, sizeof(lite_property_t));
            assert(data_type->specs);
        }

        memset(&data_type->value, 0, sizeof(data_type_x_t));
        for (index = 0; index < data_type->data_type_specs_number; ++index) {
            specs_struct_item_obj = cJSON_GetArrayItem(specs_obj, index);
            if (specs_struct_item_obj) {
                lite_property_item = (lite_property_t*)data_type->specs + index;
                install_cjson_obj_lite_property(lite_property_item, specs_struct_item_obj);
            }
        }
    }
}

static void install_cjson_obj_lite_property(void* dst, const cJSON* const cjson_obj)
{
    const cJSON* const output_data_item_obj = cjson_obj;
    lite_property_t* lite_property_item = (lite_property_t*)dst;

    install_cjson_item_string((void**)&lite_property_item->identifier, output_data_item_obj, string_identifier);
#ifndef LITE_THING_MODEL
    install_cjson_item_string((void**)&lite_property_item->name, output_data_item_obj, string_name);
#endif
    install_cjson_item_data_type(&lite_property_item->data_type, output_data_item_obj, string_dataType);
}

static void install_cjson_item_input_data(void* dst, const cJSON* const cjson_obj, service_type_t service_type)
{
    const cJSON* const input_output_data_item_obj = cjson_obj;
    input_data_t* input_data_item = (input_data_t*)dst;
    int property_to_get_name_size = 0;

    if (service_type == service_type_others) {
        install_cjson_obj_lite_property(&input_data_item->lite_property, input_output_data_item_obj);
    }

    else if (service_type == service_type_property_set) {
        install_cjson_obj_property(&input_data_item->property_to_set, input_output_data_item_obj);
    }

    else if (service_type == service_type_property_get) {
        assert(cJSON_IsString(input_output_data_item_obj));
#ifdef CJSON_STRING_ZEROCOPY
        property_to_get_name_size = input_output_data_item_obj->valuestring_length + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#else
        property_to_get_name_size = strlen(input_output_data_item_obj->valuestring) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#endif
        input_data_item->property_to_get_name = (char*)dm_lite_calloc(1, property_to_get_name_size);
#ifdef CJSON_STRING_ZEROCOPY
        strncpy(input_data_item->property_to_get_name, input_output_data_item_obj->valuestring, property_to_get_name_size - 1);
#else
        strcpy(input_data_item->property_to_get_name, input_output_data_item_obj->valuestring);
#endif
    }
}

static int install_cjson_item_string(void** dst, const cJSON* const cjson_obj, const char* const item_name)
{
    const cJSON* const root_obj = cjson_obj;
    cJSON* item_obj;
    size_t size;

    int ret = -1;
    item_obj = cJSON_GetObjectItem(root_obj, item_name);
    if (item_obj && cJSON_IsString(item_obj)) {
#ifdef CJSON_STRING_ZEROCOPY
        size = item_obj->valuestring_length + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#else
        size = strlen(item_obj->valuestring) + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#endif
        *dst = dm_lite_calloc(1, size);
        assert(*dst);
        memset(*dst, 0, size);
#ifdef CJSON_STRING_ZEROCOPY
        strncpy(*dst, item_obj->valuestring, size - 1);
#else
        strcpy(*dst, item_obj->valuestring);
#endif
        ret = 0;
    }
    return ret;
}

#ifdef LITE_THING_MODEL
static void install_cjson_item_string_without_malloc(void* dst, const cJSON* const cjson_obj, const char* const item_name)
{
    const cJSON* const root_obj = cjson_obj;
    cJSON* item_obj;
#ifdef CJSON_STRING_ZEROCOPY
    size_t size;
#endif
    item_obj = cJSON_GetObjectItem(root_obj, item_name);
    if (item_obj && cJSON_IsString(item_obj)) {
#ifdef CJSON_STRING_ZEROCOPY
        size = item_obj->valuestring_length + DM_THING_EXTENTED_ROOM_FOR_STRING_MALLOC;
#endif

#ifdef CJSON_STRING_ZEROCOPY
        strncpy(dst, item_obj->valuestring, size - 1);
#else
        strcpy(dst, item_obj->valuestring);
#endif
    }
}
#endif

static void install_cjson_item_bool(void* dst, const cJSON* const cjson_obj, const char* const item_name)
{
    const cJSON* const root_obj = cjson_obj;
    int* val = dst;
    const cJSON* const item_obj = cJSON_GetObjectItem(root_obj, item_name);

    if (item_obj && cJSON_IsBool(item_obj)) {
        *val = cJSON_IsTrue(item_obj) ? 1 : 0 ;
    } else {
        *val = 1;
    }
}

static void install_cjson_obj_property(property_t* dst, const cJSON* const cjson_obj)
{
    property_t* property = dst;
    const cJSON* const property_item_obj = cjson_obj;
    char* temp_str = NULL;

    /* properties.identifier */
    install_cjson_item_string((void**)&property->identifier, property_item_obj, string_identifier);
#ifdef LITE_THING_MODEL
    property->name = NULL;
    property->desc = NULL;
#else
    /* properties.name */
    install_cjson_item_string((void**)&property->name, property_item_obj, string_name);
    /* properties.desc */
    install_cjson_item_string((void**)&property->desc, property_item_obj, string_desc);
#endif
    /* properties.required */
    install_cjson_item_bool((void*)&property->required, property_item_obj, string_required);
    /* properties.accessMode */
    install_cjson_item_string((void**)&temp_str, property_item_obj, string_accessMode);
    if (temp_str) {
        property->access_mode = strlen(temp_str) == 2 ? property_access_mode_rw :
                                                        (strcmp(temp_str, "r") == 0 ? property_access_mode_r : property_access_mode_w);
        dm_lite_free(temp_str);
    }
    install_cjson_item_data_type(&property->data_type, property_item_obj, string_dataType);
}

static void parse_cjson_obj_to_tsl_template_properties(void* _self, const cJSON* cjson_obj)
{
    dm_thing_t* self = _self;
    const cJSON* properties_obj = cjson_obj;
    cJSON* property_item_obj;
    int properties_array_size;
    int index;
    property_t* property;

    properties_array_size = cJSON_GetArraySize(properties_obj);

    self->tsl_template.property_number = properties_array_size;

    if (self->tsl_template.property_number == 0) return;

    self->tsl_template.properties = (property_t*)dm_lite_calloc(self->tsl_template.property_number, sizeof(property_t));

    assert(self->tsl_template.properties);

    if (self->tsl_template.properties == 0) {
        self->tsl_template.property_number = 0;
        return;
    }

    for (index = 0; index < self->tsl_template.property_number; ++index) {
        property_item_obj = cJSON_GetArrayItem(properties_obj, index);
        if (property_item_obj) {
            property = self->tsl_template.properties + index;
            install_cjson_obj_property(property, property_item_obj);
        }
    }
}

static void install_cjson_obj_event(event_t* dst, const cJSON* const cjson_obj)
{
    event_t* event = dst;
    const cJSON* const event_item_obj = cjson_obj;
    const cJSON* output_data_array_obj;
    const cJSON* output_data_item_obj;
    int output_data_array_size;
    int output_data_index;
    lite_property_t* output_data_item;

    /* events.identifier */
    install_cjson_item_string((void**)&event->identifier, event_item_obj, string_identifier);
#ifndef LITE_THING_MODEL
    /* events.name */
    install_cjson_item_string((void**)&event->name, event_item_obj, string_name);
    /* events.desc */
    install_cjson_item_string((void**)&event->desc, event_item_obj, string_desc);
#endif
    /* events.method */
    install_cjson_item_string((void**)&event->method, event_item_obj, string_method);
    /* events.required */
    install_cjson_item_bool((void*)&event->required, event_item_obj, string_required);
    /* events.type */
    install_cjson_item_string((void**)&event->event_type_str, event_item_obj, string_type);
    if (strcmp(event->event_type_str, string_info) == 0) {
        event->event_type = event_type_info;
    }

    else if (strcmp(event->event_type_str, string_alert) == 0) {
        event->event_type = event_type_alert;
    }

    else if (strcmp(event->event_type_str, string_error) == 0) {
        event->event_type = event_type_error;
    }
#ifdef LITE_THING_MODEL
    if (strcmp(event->method, string_thing_event_property_post) == 0) return;
#endif
    output_data_array_obj =  cJSON_GetObjectItem(event_item_obj, string_outputData);
    if (output_data_array_obj) {
        assert(cJSON_IsArray(output_data_array_obj));
        output_data_array_size = cJSON_GetArraySize(output_data_array_obj);
        event->event_output_data_num = output_data_array_size;
        if (event->event_output_data_num == 0) {
            return;
        }
        event->event_output_data = (lite_property_t*)dm_lite_calloc(event->event_output_data_num, sizeof(lite_property_t));
        assert(event->event_output_data);

        if (event->event_output_data == NULL) {
            event->event_output_data_num = 0;
            return;
        }
    }

    for (output_data_index = 0; output_data_index < event->event_output_data_num; ++output_data_index) {
        output_data_item = event->event_output_data + output_data_index;
        output_data_item_obj = cJSON_GetArrayItem(output_data_array_obj, output_data_index);
        install_cjson_obj_lite_property(output_data_item, output_data_item_obj);
    }
}

static void parse_cjson_obj_to_tsl_template_events(void* _self, const cJSON* cjson_obj)
{
    dm_thing_t* self = _self;
    const cJSON* events_obj = cjson_obj;
    cJSON* event_item_obj;
    int events_array_size;
    int index;
    event_t* event;

    events_array_size = cJSON_GetArraySize(events_obj);

    self->tsl_template.event_number = events_array_size;

    if (self->tsl_template.event_number == 0) return;

    self->tsl_template.events = (event_t*)dm_lite_calloc(self->tsl_template.event_number, sizeof(event_t));

    assert(self->tsl_template.events);

    if (self->tsl_template.events == NULL) {
        self->tsl_template.event_number = 0;
        dm_log_err("self->tsl_template.events malloc fail");
        return;
    }

    for (index = 0; index < self->tsl_template.event_number; ++index) {
        event_item_obj = cJSON_GetArrayItem(events_obj, index);
        if (event_item_obj) {
            event = self->tsl_template.events + index;

            install_cjson_obj_event(event, event_item_obj);
        }
    }
}

static void install_cjson_obj_service(service_t* dst, const cJSON* const cjson_obj)
{
    const cJSON* service_item_obj = cjson_obj;
    service_t* service = dst;
    cJSON* output_data_array_obj;
    cJSON* output_data_item_obj;
    cJSON* input_data_array_obj;
    cJSON* input_data_item_obj;
    int output_data_array_size, input_data_array_size;
    int output_data_index, input_data_index;
    lite_property_t* output_data_item;
    input_data_t* input_data_item;

    /* service.identifier */
    install_cjson_item_string((void**)&service->identifier, service_item_obj, string_identifier);
#ifndef LITE_THING_MODEL
    /* service.name */
    install_cjson_item_string((void**)&service->name, service_item_obj, string_name);
    /* service.desc */
    install_cjson_item_string((void**)&service->desc, service_item_obj, string_desc);
#endif
    /* service.method */
    install_cjson_item_string((void**)&service->method, service_item_obj, string_method);
    /* service.call_type */
    install_cjson_item_string((void**)&service->call_type, service_item_obj, "callType");
    /* service.required */
    install_cjson_item_bool((void*)&service->required, service_item_obj, string_required);

    if (strcmp(service->method, string_thing_service_property_set) == 0) {
        service->service_type = service_type_property_set;
    }

    else if (strcmp(service->method, string_thing_service_property_get) == 0) {
        service->service_type = service_type_property_get;
    }

    else {
        service->service_type = service_type_others;
    }

    /* output data process */
    output_data_array_obj =  cJSON_GetObjectItem(service_item_obj, string_outputData);
#ifdef LITE_THING_MODEL
    if (output_data_array_obj && service->service_type != service_type_property_get) {
#else
    if (output_data_array_obj) {
#endif
        assert(cJSON_IsArray(output_data_array_obj));
        output_data_array_size = cJSON_GetArraySize(output_data_array_obj);
        service->service_output_data_num = output_data_array_size;
        if (service->service_output_data_num) {
            service->service_output_data = (lite_property_t*)dm_lite_calloc(service->service_output_data_num, sizeof(lite_property_t));
            assert(service->service_output_data);
            if (service->service_output_data == NULL) {
                service->service_output_data_num = 0;
            }
        } else {
            service->service_output_data = NULL;
        }
    }

    for (output_data_index = 0; output_data_index < service->service_output_data_num; ++output_data_index) {
        output_data_item = service->service_output_data + output_data_index;
        output_data_item_obj = cJSON_GetArrayItem(output_data_array_obj, output_data_index);
        if (output_data_item_obj) {
            install_cjson_obj_lite_property(output_data_item, output_data_item_obj);
        }
    }
    /* input data process */
    input_data_array_obj =  cJSON_GetObjectItem(service_item_obj, string_inputData);
    service->service_input_data_num = 0;
#ifdef LITE_THING_MODEL
    if (input_data_array_obj && service->service_type != service_type_property_set) {
#else
    if (input_data_array_obj) {
#endif
        assert(cJSON_IsArray(input_data_array_obj));
        input_data_array_size = cJSON_GetArraySize(input_data_array_obj);
        service->service_input_data_num = input_data_array_size;
        service->service_input_data = (input_data_t*)dm_lite_calloc(service->service_input_data_num, sizeof(input_data_t));
        if (service->service_input_data == NULL) {
            service->service_input_data_num = 0;
        }
    }
    for (input_data_index = 0; input_data_index < service->service_input_data_num; ++input_data_index) {
        input_data_item = service->service_input_data + input_data_index;
        input_data_item_obj = cJSON_GetArrayItem(input_data_array_obj, input_data_index);
        if (input_data_item_obj) {
            install_cjson_item_input_data(input_data_item, input_data_item_obj, service->service_type);
        }
    }
}

static void parse_cjson_obj_to_tsl_template_services(void* _self, const cJSON* cjson_obj)
{
    dm_thing_t* self = _self;
    const cJSON* services_obj = cjson_obj;
    cJSON* service_item_obj;

    int sercives_array_size;
    int index;
    service_t* service;

    sercives_array_size = cJSON_GetArraySize(services_obj);

    self->tsl_template.service_number = sercives_array_size;
    self->tsl_template.services = (service_t*)dm_lite_calloc(self->tsl_template.service_number, sizeof(service_t));

    if (self->tsl_template.services == NULL) {
        self->tsl_template.service_number = 0;
        dm_log_err("self->tsl_template.events malloc fail");
        return;
    }

    for (index = 0; index < self->tsl_template.service_number; ++index) {
        service_item_obj = cJSON_GetArrayItem(services_obj, index);
        if (service_item_obj) {
            service = self->tsl_template.services + index;
            install_cjson_obj_service(service, service_item_obj);
        }
    }
}

static int parse_cjson_obj_to_tsl_template(void* _self, const cJSON* cjson_obj)
{
    if(!_self || !cjson_obj) {
        return -1;
    }

    dm_thing_t* self = _self;
    const cJSON* root_obj = cjson_obj;
    cJSON* profile_obj;
    cJSON* properties_obj;
    cJSON* events_obj;
    cJSON* sercives_obj;

    /* schema */
#ifdef LITE_THING_MODEL
    self->tsl_template.schema = NULL;
    self->tsl_template.link = NULL;
#else
    if(0 != install_cjson_item_string((void**)&self->tsl_template.schema, root_obj, "schema")) {
        return -1;
    }

    /* link */
    if(0 != install_cjson_item_string((void**)&self->tsl_template.link, root_obj, "link")) {
        return -1;
    }
#endif
    /* profile */
    profile_obj = cJSON_GetObjectItem(root_obj, "profile");
    if (profile_obj) {
        install_cjson_item_string((void**)&self->tsl_template.profile.product_key, profile_obj, "productKey");
        install_cjson_item_string((void**)&self->tsl_template.profile.device_name, profile_obj, "deviceName");
    } else {
        self->tsl_template.profile.product_key = NULL;
        self->tsl_template.profile.device_name = NULL;
    }

    /* properties */
    properties_obj = cJSON_GetObjectItem(root_obj, string_properties);
    if (properties_obj) {
        assert(cJSON_IsArray(properties_obj));
        parse_cjson_obj_to_tsl_template_properties(self, properties_obj);
    }
    else {
        return -1;
    }
    /* events */
    events_obj = cJSON_GetObjectItem(root_obj, string_events);
    if (events_obj) {
        assert(cJSON_IsArray(events_obj));
        parse_cjson_obj_to_tsl_template_events(self, events_obj);
    } else {
        return -1;
    }
    /* events */
    sercives_obj = cJSON_GetObjectItem(root_obj, string_services);
    if (sercives_obj) {
        assert(cJSON_IsArray(sercives_obj));
        parse_cjson_obj_to_tsl_template_services(self, sercives_obj);
    } else {
        return -1;
    }

    return 0;
}

/* set tsl to thing model. */
static int dm_thing_set_tsl_string(void* _self, const char* tsl, int tsl_str_len)
{
    int ret = -1;
    dm_thing_t* self = _self;

    self->_json_object = cJSON_Parse(tsl);

    assert(self->_json_object);
    ret = parse_cjson_obj_to_tsl_template(self, self->_json_object);

    cJSON_Delete(self->_json_object);
    self->_json_object = 0;

    return ret;
}

#endif

/* get tsl from thing model. */
static void* dm_thing_get_tsl_string(void* _self)
{
    dm_thing_t* self = _self;

    /* TODO */

    return self->_tsl_string;
}

static int dm_thing_get_property_number(const void* _self)
{
    const dm_thing_t* self = _self;

    return self->tsl_template.property_number;
}

static int dm_thing_get_service_number(const void* _self)
{
    const dm_thing_t* self = _self;

    return self->tsl_template.service_number;
}

static int dm_thing_get_event_number(const void* _self)
{
    const dm_thing_t* self = _self;

    return self->tsl_template.event_number;
}

static void* dm_thing_get_property_by_identifier(const void* _self, const char* const identifier)
{
    const dm_thing_t* self = _self;
    property_t* property = NULL;
    lite_property_t* lite_property = NULL;
    size_t property_number = self->tsl_template.property_number;
    size_t data_type_specs_number;
    int index;
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    char* p;
    char id_buff[DM_THING_MAX_IDENTIFIER_LENGTH] = {0};

    if (property_number == 0 || strlen(identifier) >= DM_THING_MAX_IDENTIFIER_LENGTH) {
        return NULL;
    }

    strcpy(id_buff, identifier);

    if (strchr(identifier, DEFAULT_TSL_DELIMITER)) {
        p = strtok(id_buff, delimeter);

        for (index = 0; index < property_number; ++index) {
            property = self->tsl_template.properties + index;
            if (property) {
                if (strcmp(property->identifier, p) == 0) {
                    break;
                }
            }
        }
        if (property && index < property_number) {
            p = strtok(NULL, delimeter);
            data_type_specs_number = property->data_type.data_type_specs_number;
            for (index = 0; index < data_type_specs_number; ++index) {
                lite_property = (lite_property_t*)property->data_type.specs + index;
                if (strcmp(lite_property->identifier, p) == 0) {
                    return lite_property;
                }
            }
        }
    } else {
        for (index = 0; index < property_number; ++index) {
            property = self->tsl_template.properties + index;
            if (property) {
                if (strcmp(property->identifier, identifier) == 0) {
                    return property;
                }
            }
        }
    }

    return NULL;
}

static int dm_thing_get_property_identifier_by_index(const void* _self, int index, char* identifier)
{
    const dm_thing_t* self = _self;
    property_t* property = NULL;
    int ret = -1;

    if (index < self->tsl_template.property_number) {
        property = self->tsl_template.properties + index;
        strcpy(identifier, property->identifier);
        ret = 0;
    }

    return ret;
}

static void* dm_thing_get_service_by_identifier(const void* _self, const char* const identifier)
{
    const dm_thing_t* self = _self;
    service_t* service = NULL;
    lite_property_t* lite_property = NULL;
    input_data_t* input_data;
    size_t service_number = self->tsl_template.service_number;
    size_t service_output_data_num;
    size_t service_input_data_num;
    int index;
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    char* p;
    char* p_m;
    char* p_l;
    char id_buff[DM_THING_MAX_IDENTIFIER_LENGTH] = {0};

    if (service_number == 0 || strlen(identifier) >= DM_THING_MAX_IDENTIFIER_LENGTH) return NULL;

    strcpy(id_buff, identifier);

    if (strchr(identifier, DEFAULT_TSL_DELIMITER)) {
        p = strtok(id_buff, delimeter);

        for (index = 0; index < service_number; ++index) {
            service = self->tsl_template.services + index;
            if (service) {
                if (strcmp(service->identifier, p) == 0) {
                    break;
                }
            }
        }
        p_m = strtok(NULL, delimeter);
        p_l = strtok(NULL, delimeter);
        if (p_m && service && index < service_number) {
            service_input_data_num = service->service_input_data_num;
            for (index = 0; index < service_input_data_num; ++index) {
                input_data = service->service_input_data + index;
                lite_property = &input_data->lite_property;
                if (strcmp(lite_property->identifier, p_m) == 0) {
                    if (p_l && strcmp(p_l, string_input) != 0) {
                        return NULL;
                    }
                    return lite_property;
                }
            }
        }
        if (p_m && service && index < service_number) {
            service_output_data_num = service->service_output_data_num;
            for (index = 0; index < service_output_data_num; ++index) {
                lite_property = service->service_output_data + index;
                if (strcmp(lite_property->identifier, p_m) == 0) {
                    if (p_l && strcmp(p_l, string_output) != 0) {
                        return NULL;
                    }
                    return lite_property;
                }
            }
        }
    } else {
        for (index = 0; index < service_number; ++index) {
            service = self->tsl_template.services + index;
            if (service) {
                if (strcmp(service->identifier, identifier) == 0) {
                    return service;
                }
            }
        }
    }

    return NULL;
}

static int dm_thing_get_service_identifier_by_index(const void* _self, int index, char* identifier)
{
    const dm_thing_t* self = _self;
    service_t* service = NULL;
    int ret = -1;

    assert(index < self->tsl_template.service_number);

    if (index < self->tsl_template.service_number) {
        service = self->tsl_template.services + index;
        strcpy(identifier, service->identifier);
        ret = 0;
    }

    return ret;
}

static void* dm_thing_get_event_by_identifier(const void* _self, const char* const identifier)
{
    const dm_thing_t* self = _self;
    event_t* event = NULL;
    lite_property_t* lite_property = NULL;
    size_t event_number = self->tsl_template.event_number;
    size_t event_output_data_num;
    int index;
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    char* p;
    char id_buff[DM_THING_MAX_IDENTIFIER_LENGTH] = {0};

    if (event_number == 0 || strlen(identifier) >= DM_THING_MAX_IDENTIFIER_LENGTH) return NULL;

    strcpy(id_buff, identifier);

    if (strchr(identifier, DEFAULT_TSL_DELIMITER)) {
        p = strtok(id_buff, delimeter);

        for (index = 0; index < event_number; ++index) {
            event = self->tsl_template.events + index;
            if (event) {
                if (strcmp(event->identifier, p) == 0) {
                    break;
                }
            }
        }
        if (event && index < event_number) {
            p = strtok(NULL, delimeter);
            event_output_data_num = event->event_output_data_num;
            for (index = 0; index < event_output_data_num; ++index) {
                lite_property = event->event_output_data + index;
                if (strcmp(lite_property->identifier, p) == 0) {
                    return lite_property;
                }
            }
        }
    } else {
        for (index = 0; index < event_number; ++index) {
            event = self->tsl_template.events + index;
            if (event) {
                if (strcmp(event->identifier, identifier) == 0) {
                    return event;
                }
            }
        }
    }

    return NULL;
}

static int dm_thing_get_event_identifier_by_index(const void* _self, int index, char* identifier)
{
    const dm_thing_t* self = _self;
    event_t* event = NULL;
    int ret = -1;

    if (index < self->tsl_template.event_number) {
        event = self->tsl_template.events + index;
        strcpy(identifier, event->identifier);
        ret = 0;
    }

    return ret;
}

static int dm_thing_get_schema(const void* _self, char* schema)
{
    const dm_thing_t* self = _self;
    int ret = -1;

    assert(self->tsl_template.schema);

    if (self->tsl_template.schema) {
        strcpy(schema, self->tsl_template.schema);
        ret = 0;
    }
    return ret;
}

static int dm_thing_get_link(const void* _self, char* link)
{
    const dm_thing_t* self = _self;
    int ret = -1;

    assert(self->tsl_template.link);

    if (self->tsl_template.link) {
        strcpy(link, self->tsl_template.link);
        ret = 0;
    }
    return ret;
}

static int dm_thing_set_product_key(void* _self, const char* product_key)
{
    dm_thing_t* self = _self;
    int ret = -1;
    int length = 0;

    assert(product_key);

    if (self->tsl_template.profile.product_key) {
        dm_lite_free(self->tsl_template.profile.product_key);
    }

    length = strlen(product_key) + 1;
    self->tsl_template.profile.product_key = dm_lite_calloc(1, length);

    assert(self->tsl_template.profile.product_key);
    if (!self->tsl_template.profile.product_key) return -1;

    strcpy(self->tsl_template.profile.product_key, product_key);

    ret = 0;

    return ret;
}

static int dm_thing_get_product_key(const void* _self, char* product_key)
{
    const dm_thing_t* self = _self;
    int ret = -1;

    assert(self->tsl_template.profile.product_key);

    if (self->tsl_template.profile.product_key) {
        strcpy(product_key, self->tsl_template.profile.product_key);
        ret = 0;
    }

    return ret;
}

static char* dm_thing_return_product_key(const void* _self)
{
    const dm_thing_t* self = _self;

    if (self->tsl_template.profile.product_key) {
        return self->tsl_template.profile.product_key;
    }

    return NULL;
}

static int dm_thing_set_device_name(void* _self, const char* device_name)
{
    dm_thing_t* self = _self;
    int ret = -1;
    int length = 0;

    assert(device_name);

    if (self->tsl_template.profile.device_name) {
        dm_lite_free(self->tsl_template.profile.device_name);
    }

    length = strlen(device_name) + 1;
    self->tsl_template.profile.device_name = dm_lite_calloc(1, length);

    assert(self->tsl_template.profile.device_name);
    if (!self->tsl_template.profile.device_name) return -1;

    strcpy(self->tsl_template.profile.device_name, device_name);

    ret = 0;

    return ret;
}


static int dm_thing_get_device_name(const void* _self, char* device_name)
{
    const dm_thing_t* self = _self;
    int ret = -1;

    if (self->tsl_template.profile.device_name) {
        strcpy(device_name, self->tsl_template.profile.device_name);
        ret = 0;
    }
    return ret;
}

static char* dm_thing_return_device_name(const void* _self)
{
    const dm_thing_t* self = _self;

    if (self->tsl_template.profile.device_name) {
        return self->tsl_template.profile.device_name;
    }

    return NULL;
}

static int set_lite_property_value(void* _self, void* property, const void* value, const char* value_str)
{
    dm_thing_t* self = _self;
    lite_property_t* lite_property = NULL;
    lite_property_t* lite_sub_property = NULL;
    data_type_type_t type;
    data_type_x_t* data_type_x;
    int val_int;
    float val_float;
    double val_double;
    unsigned long long val_long;
    const char* val_char_p;
    int text_length;
    int index;
    char* enum_item_key_str;
    const char* type_str[] = {
        string_text, string_enum, string_bool, string_float, string_double, string_int, string_date, string_struct
    };

    assert(property);

    if (property == NULL) return -1;

    lite_property = property;
    data_type_x = &lite_property->data_type.value;
    type = lite_property->data_type.type;

    dm_log_debug("set property@%p\ttype = %d(%s)\n", lite_property, type, type_str[type]);

    /* at least one input is not null. */
    if (value == NULL && value_str == NULL) {
        return -1;
    }

    switch (type) {
    case data_type_type_int:
        val_int = value ? *(const int*)value : atoi(value_str);
        val_int = val_int > data_type_x->data_type_int_t.max ? data_type_x->data_type_int_t.max :
                                                               (val_int < data_type_x->data_type_int_t.min ? data_type_x->data_type_int_t.min : val_int);
        data_type_x->data_type_int_t.value = val_int;
        dm_sprintf(data_type_x->data_type_int_t.value_str, "%d", data_type_x->data_type_int_t.value);
        break;
    case data_type_type_float:
        val_float = value ? *(const float*)value : atof(value_str);
        val_float = val_float > data_type_x->data_type_float_t.max ? data_type_x->data_type_float_t.max :
                                                                     (val_float < data_type_x->data_type_float_t.min ? data_type_x->data_type_float_t.min : val_float);
        data_type_x->data_type_float_t.value = val_float;
        sprintf_float_double_precise(data_type_x->data_type_float_t.value_str, data_type_x->data_type_float_t.value,
                                     data_type_x->data_type_float_t.precise);
        break;
    case data_type_type_double:
        val_double = value ? *(const double*)value : atof(value_str);
        val_double = val_double > data_type_x->data_type_double_t.max ? data_type_x->data_type_double_t.max :
                                                                        (val_double < data_type_x->data_type_double_t.min ? data_type_x->data_type_double_t.min : val_double);
        data_type_x->data_type_double_t.value = val_double;
        sprintf_float_double_precise(data_type_x->data_type_double_t.value_str, data_type_x->data_type_double_t.value,
                                     data_type_x->data_type_double_t.precise);
        break;
    case data_type_type_bool:
        val_int = value ? *(const int*)value : atoi(value_str);
        if (val_int) {
            data_type_x->data_type_bool_t.value = 1;
            dm_sprintf(data_type_x->data_type_bool_t.value_str, "%d", 1);
        } else {
            data_type_x->data_type_bool_t.value = 0;
            dm_sprintf(data_type_x->data_type_bool_t.value_str, "%d", 0);
        }

        break;
    case data_type_type_enum:
        val_int = value ? *(const int*)value : atoi(value_str);
        /* check if match predefined keys. */
        for (index = 0; index < data_type_x->data_type_enum_t.enum_item_number; ++index) {
            enum_item_key_str = *(data_type_x->data_type_enum_t.enum_item_key + index);
            if (val_int == atoi(enum_item_key_str)) {
                data_type_x->data_type_enum_t.value = val_int;
                dm_sprintf(data_type_x->data_type_enum_t.value_str, "%d", data_type_x->data_type_enum_t.value);
                return 0;
            }
        }
        return -1;
        break;
    case data_type_type_text:
        val_char_p = value ? (const char*)value : value_str;
        text_length = strlen(val_char_p);
        /* check if length restrict satisfied. */
        if (text_length <= data_type_x->data_type_text_t.length) {
            if (data_type_x->data_type_text_t.value) {
                dm_lite_free(data_type_x->data_type_text_t.value);
                data_type_x->data_type_text_t.value = NULL;
            }

            data_type_x->data_type_text_t.value = dm_lite_calloc(1, text_length + 1);
            assert(data_type_x->data_type_text_t.value);
            if (data_type_x->data_type_text_t.value == NULL) {
                return -1;
            }

            strcpy(data_type_x->data_type_text_t.value, val_char_p);
        } else {
            return -1;
        }
        break;
    case data_type_type_date:
        val_long = value ? *(const unsigned long long*)value : atoll(value_str);
        data_type_x->data_type_date_t.value = val_long;
        dm_lltoa(data_type_x->data_type_date_t.value, data_type_x->data_type_date_t.value_str, 10);
        break;
    case data_type_type_struct:
        for(index = 0; index < lite_property->data_type.data_type_specs_number; ++index) {
            lite_sub_property = (lite_property_t*)lite_property->data_type.specs + index;

#ifdef CM_SUPPORT_MEMORY_MAGIC
            val_char_p = LITE_json_value_of_ext(lite_sub_property->identifier, (char*)value_str, MEM_MAGIC, DM_MODULE_NAME);
#else
            val_char_p = LITE_json_value_of(lite_sub_property->identifier, (char*)value_str);
#endif
            if(!val_char_p) return -1;
            set_lite_property_value(self, lite_sub_property, NULL, val_char_p);
            LITE_free(val_char_p);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int dm_thing_set_property_value(void* _self, void* _property, const void* value, const char* value_str)
{
    dm_thing_t* self = _self;
    property_t* property = _property;
    lite_property_t* lite_property = NULL;

    assert(property);
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    if (property->access_mode == property_access_mode_r) {
        dm_log_err("try to set value to readonly property, id:%s\n", property->identifier);

        return -1;
    }
#endif
    lite_property = (lite_property_t*)property;

    set_lite_property_value(self, lite_property, value, value_str);

    return 0;
}

static int dm_thing_set_property_value_by_identifier(void* _self, const char* const identifier, const void* value, const char* value_str)
{
    dm_thing_t* self = _self;
    lite_property_t* lite_property;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    property_t* property;

    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    const char* p;
    char id_buff[MAX_IDENTIFIER_LENGTH] = {0};

    if (strchr(identifier, DEFAULT_TSL_DELIMITER)) {
        strcpy(id_buff, identifier);
        p = strtok(id_buff, delimeter);
    } else {
        p = identifier;
    }

    /* do some checks. */
    property = dm_thing_get_property_by_identifier(self, p);
    if (property->access_mode == property_access_mode_r) {
        dm_log_err("try to set value to readonly property, id:%s\n", property->identifier);
        return -1;
    }
#endif

    lite_property = dm_thing_get_property_by_identifier(self, identifier);

    if (lite_property == NULL) {
        dm_log_err("property(%s) not find", identifier);
        return -1;
    }

    return set_lite_property_value(self, lite_property, value, value_str);
}

static int dm_thing_get_lite_property_value(const void* _self, const void* const property, void* value, char** value_str)
{
    const dm_thing_t* self = _self;
    const lite_property_t* lite_property = NULL;
    data_type_type_t type;
    const data_type_x_t* data_type_x;
    int val_int;
    float val_float;
    double val_double;
    int ret = -1;
    const char* type_str[] = {
        string_text, string_enum, string_bool, string_float, string_double, string_int, string_date, string_struct
    };

    self = self;
    if (property == NULL) {
        dm_log_err("property(%p) not exit", property);
        return -1;
    }

    lite_property = property;
    data_type_x = &lite_property->data_type.value;
    type = lite_property->data_type.type;

    dm_log_debug("get property@%p\ttype:%d(%s)\n", lite_property, type, type_str[type]);
    switch (type) {
    case data_type_type_int:
        val_int = data_type_x->data_type_int_t.value;
        if (value) {
            *(int*)value = val_int;
        }
        if (value_str) {
            *value_str = data_type_x->data_type_int_t.value_str;
        }

        ret = 0;
        break;
    case data_type_type_float:
        val_float = data_type_x->data_type_float_t.value;
        if (value) {
            *(float*)value = val_float;
        }
        if (value_str) {
            *value_str = data_type_x->data_type_float_t.value_str;
        }

        ret = 0;
        break;
    case data_type_type_double:
        val_double = data_type_x->data_type_double_t.value;
        if (value) {
            *(double*)value = val_double;
        }
        if (value_str) {
            *value_str = data_type_x->data_type_double_t.value_str;
        }

        ret = 0;
        break;
    case data_type_type_bool:
        if (value) {
            *(int*)value = 0;
            if (data_type_x->data_type_bool_t.value) {
                *(int*)value = 1;
            }
        }
        if (value_str) {
            *value_str = data_type_x->data_type_bool_t.value_str;
        }
        ret = 0;
        break;
    case data_type_type_enum:
        if (value) {
            *(int*)value = data_type_x->data_type_enum_t.value;
        }
        if (value_str) {
            *value_str = data_type_x->data_type_enum_t.value_str;
        }
        ret = 0;
        break;
    case data_type_type_text:
        if (value) {
            if (data_type_x->data_type_text_t.value) {
                strcpy(value, data_type_x->data_type_text_t.value);
            }
        }
        if (value_str) {
            *value_str = data_type_x->data_type_text_t.value;
        }
        ret = 0;
        break;
    case data_type_type_date:
        if (value) {
            *(unsigned long long*)value = data_type_x->data_type_date_t.value;
        }
        if (value_str) {
            if (data_type_x->data_type_date_t.value_str) {
                *value_str = data_type_x->data_type_date_t.value_str;
            }
        }
        ret = 0;
        break;
    default:
        *value_str = NULL;
        break;
    }

    return ret;

}

static int dm_thing_get_property_value(const void* _self, const void* const _property, void* value, char** value_str)
{
    const dm_thing_t* self = _self;
    const lite_property_t* lite_property = NULL;
    const property_t* property = _property;

    assert(property);
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    if (property->access_mode == property_access_mode_w) {
        dm_log_err("try to set value to write only property, id:%s\n", property->identifier);

        return -1;
    }
#endif
    lite_property = (lite_property_t*)property;

    return dm_thing_get_lite_property_value(self, lite_property, value, value_str);
}

static int dm_thing_get_property_value_by_identifier(const void* _self, const char* const identifier, void* value, char** value_str)
{
    const dm_thing_t* self = _self;
    const lite_property_t* lite_property;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    property_t* property;
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    const char* p;
    char id_buff[MAX_IDENTIFIER_LENGTH] = {0};

    if (strchr(identifier, DEFAULT_TSL_DELIMITER)) {
        strcpy(id_buff, identifier);
        p = strtok(id_buff, delimeter);
    } else {
        p = identifier;
    }

    /* do some checks. */
    property = dm_thing_get_property_by_identifier(self, p);
    if (property->access_mode == property_access_mode_w) {
        dm_log_err("try to set value to write only property, id:%s\n", property->identifier);
        return -1;
    }
#endif
    lite_property = dm_thing_get_property_by_identifier(self, identifier);

    if (lite_property == NULL) {
        dm_log_err("property(%s) not find", identifier);
        return -1;
    }

    return dm_thing_get_lite_property_value(self, lite_property, value, value_str);
}

static void dm_thing_property_iterator(void* _self, handle_item_t handle_fp, va_list* params)
{
    dm_thing_t* self = _self;
    property_t* property = NULL;
    size_t property_number = self->tsl_template.property_number;
    int index;

    for (index = 0; index < property_number; ++index) {
        property = self->tsl_template.properties + index;
        if (property) {
            va_list args;
            va_copy(args, *params);
            handle_fp(property, index, &args);
            va_end(args);
        }
    }
}

static void dm_thing_event_iterator(void* _self, handle_item_t handle_fp, va_list* params)
{
    dm_thing_t* self = _self;
    event_t* event = NULL;
    size_t event_number = self->tsl_template.event_number;
    int index;

    for (index = 0; index < event_number; ++index) {
        event = self->tsl_template.events + index;
        if (event) {
            va_list args;
            va_copy(args, *params);
            handle_fp(event, index, &args);
            va_end(args);
        }
    }
}

static void dm_thing_service_iterator(void* _self, handle_item_t handle_fp, va_list* params)
{
    dm_thing_t* self = _self;
    service_t* service = NULL;
    size_t service_number = self->tsl_template.service_number;
    int index;

    for (index = 0; index < service_number; ++index) {
        service = self->tsl_template.services + index;
        if (service) {
            va_list args;
            va_copy(args, *params);
            handle_fp(service, index, &args);
            va_end(args);
        }
    }
}

void property_iterator(void* _self, handle_item_t handle_fp, ...)
{
    dm_thing_t* self = _self;

    va_list params;
    va_start(params, handle_fp);

    dm_thing_property_iterator(self, handle_fp, &params);

    va_end(params);
}

void event_iterator(void* _self, handle_item_t handle_fp, ...)
{
    dm_thing_t* self = _self;

    va_list params;
    va_start(params, handle_fp);

    dm_thing_event_iterator(self, handle_fp, &params);

    va_end(params);
}

void service_iterator(void* _self, handle_item_t handle_fp, ...)
{
    dm_thing_t* self = _self;

    va_list params;
    va_start(params, handle_fp);

    dm_thing_service_iterator(self, handle_fp, &params);

    va_end(params);
}

static void property_free_data_type_info(void* dst)
{
    data_type_t* data_type = dst;
    data_type_x_t* data_type_x;
    lite_property_t* lite_property;
    int index;
    assert(data_type);

    data_type_x = &data_type->value;

    switch (data_type->type) {
    case data_type_type_int:
#ifdef LITE_THING_MODEL
        if (data_type_x->data_type_int_t.value_str) {
            dm_lite_free(data_type_x->data_type_int_t.value_str);
        }
#else
        if (data_type_x->data_type_int_t.min_str) {
            dm_lite_free(data_type_x->data_type_int_t.min_str);
        }
        if (data_type_x->data_type_int_t.max_str) {
            dm_lite_free(data_type_x->data_type_int_t.max_str);
        }
        if (data_type_x->data_type_int_t.value_str) {
            dm_lite_free(data_type_x->data_type_int_t.value_str);
        }
        if (data_type_x->data_type_int_t.precise_str) {
            dm_lite_free(data_type_x->data_type_int_t.precise_str);
        }
        if (data_type_x->data_type_int_t.unit) {
            dm_lite_free(data_type_x->data_type_int_t.unit);
        }
        if (data_type_x->data_type_int_t.unit_name) {
            dm_lite_free(data_type_x->data_type_int_t.unit_name);
        }
#endif
        break;
    case data_type_type_float:
#ifdef LITE_THING_MODEL
        if (data_type_x->data_type_float_t.value_str) {
            dm_lite_free(data_type_x->data_type_float_t.value_str);
        }
#else
        if (data_type_x->data_type_float_t.min_str) {
            dm_lite_free(data_type_x->data_type_float_t.min_str);
        }
        if (data_type_x->data_type_float_t.max_str) {
            dm_lite_free(data_type_x->data_type_float_t.max_str);
        }
        if (data_type_x->data_type_float_t.value_str) {
            dm_lite_free(data_type_x->data_type_float_t.value_str);
        }
        if (data_type_x->data_type_float_t.precise_str) {
            dm_lite_free(data_type_x->data_type_float_t.precise_str);
        }
        if (data_type_x->data_type_float_t.unit) {
            dm_lite_free(data_type_x->data_type_float_t.unit);
        }
        if (data_type_x->data_type_float_t.unit_name) {
            dm_lite_free(data_type_x->data_type_float_t.unit_name);
        }
#endif
        break;
    case data_type_type_double:
#ifdef LITE_THING_MODEL
        if (data_type_x->data_type_double_t.value_str) {
            dm_lite_free(data_type_x->data_type_double_t.value_str);
        }
#else
        if (data_type_x->data_type_double_t.min_str) {
            dm_lite_free(data_type_x->data_type_double_t.min_str);
        }
        if (data_type_x->data_type_double_t.max_str) {
            dm_lite_free(data_type_x->data_type_double_t.max_str);
        }
        if (data_type_x->data_type_double_t.value_str) {
            dm_lite_free(data_type_x->data_type_double_t.value_str);
        }
        if (data_type_x->data_type_double_t.precise_str) {
            dm_lite_free(data_type_x->data_type_double_t.precise_str);
        }
        if (data_type_x->data_type_double_t.unit) {
            dm_lite_free(data_type_x->data_type_double_t.unit);
        }
        if (data_type_x->data_type_double_t.unit_name) {
            dm_lite_free(data_type_x->data_type_double_t.unit_name);
        }
#endif
        break;
    case data_type_type_enum:
        for (index = 0; index < data_type_x->data_type_enum_t.enum_item_number; ++index) {
            dm_lite_free(*(data_type_x->data_type_enum_t.enum_item_key + index));
            if (data_type_x->data_type_enum_t.enum_item_value && *(data_type_x->data_type_enum_t.enum_item_value + index)) {
                dm_lite_free(*(data_type_x->data_type_enum_t.enum_item_value + index));
            }
        }
        if (data_type_x->data_type_enum_t.value_str) {
            dm_lite_free(data_type_x->data_type_enum_t.value_str);
        }

        if (data_type_x->data_type_enum_t.enum_item_number) {
            dm_lite_free(data_type_x->data_type_enum_t.enum_item_key);
            if (data_type_x->data_type_enum_t.enum_item_value) {
                dm_lite_free(data_type_x->data_type_enum_t.enum_item_value);
            }
        }
        break;
    case data_type_type_bool:
        for (index = 0; index < data_type_x->data_type_bool_t.bool_item_number; ++index) {
            dm_lite_free(*(data_type_x->data_type_bool_t.bool_item_key + index));
            if (data_type_x->data_type_bool_t.bool_item_value && *(data_type_x->data_type_bool_t.bool_item_value + index)) {
                dm_lite_free(*(data_type_x->data_type_bool_t.bool_item_value + index));
            }
        }
        if (data_type_x->data_type_bool_t.value_str) {
            dm_lite_free(data_type_x->data_type_bool_t.value_str);
        }

        if (data_type_x->data_type_bool_t.bool_item_number) {
            dm_lite_free(data_type_x->data_type_bool_t.bool_item_key);
            if (data_type_x->data_type_bool_t.bool_item_value) {
                dm_lite_free(data_type_x->data_type_bool_t.bool_item_value);
            }
        }
        break;
    case data_type_type_text:
#ifdef LITE_THING_MODEL
        if (data_type_x->data_type_text_t.length_str) {
            dm_lite_free(data_type_x->data_type_text_t.length_str);
        }
        if (data_type_x->data_type_text_t.value) {
            dm_lite_free(data_type_x->data_type_text_t.value);
        }
#else
        if (data_type_x->data_type_text_t.length_str) {
            dm_lite_free(data_type_x->data_type_text_t.length_str);
        }
        if (data_type_x->data_type_text_t.value) {
            dm_lite_free(data_type_x->data_type_text_t.value);
        }
        if (data_type_x->data_type_text_t.unit) {
            dm_lite_free(data_type_x->data_type_text_t.unit);
        }
        if (data_type_x->data_type_text_t.unit_name) {
            dm_lite_free(data_type_x->data_type_text_t.unit_name);
        }
#endif
        break;
    case data_type_type_struct:
        for (index = 0; index < data_type->data_type_specs_number; ++index) {
            lite_property = (lite_property_t*)data_type->specs + index;
            free_lite_property(lite_property);
        }
        if (data_type->data_type_specs_number) {
            dm_lite_free(data_type->specs);
        }
        break;
    case data_type_type_date:
        if (data_type_x->data_type_date_t.value_str) {
            dm_lite_free(data_type_x->data_type_date_t.value_str);
        }
        break;
    default:
        dm_printf("unsupported type:%s\n", data_type->type_str);
        break;
    }

    if (data_type->type_str) {
        dm_lite_free(data_type->type_str);
    }
}

static void free_lite_property(void* _lite_property)
{
    lite_property_t* lite_property = _lite_property;

    assert(lite_property);

    if (lite_property) {
        if (lite_property->identifier) {
            dm_lite_free(lite_property->identifier);
        }
        if (lite_property->name) {
            dm_lite_free(lite_property->name);
        }
    }
    property_free_data_type_info(&lite_property->data_type);
}

static void free_input_data(void* _input_data, service_type_t service_type)
{
    input_data_t* input_data = _input_data;

    assert(input_data);

    if (input_data) {
        switch (service_type) {
        case service_type_property_set:
            free_property(&input_data->property_to_set, string_property, 2);
            break;
        case service_type_others:
            free_lite_property(&input_data->lite_property);
            break;
        case service_type_property_get:
            if (input_data->property_to_get_name) {
                dm_lite_free(input_data->property_to_get_name);
            }
            break;
        default:
            break;
        }
    }
}

static void free_property(void* _property, ...)
{
    property_t* property = _property;

    assert(property);

    va_list params;

    va_start(params, _property);
    free_item_memory(property, 0, &params);
    va_end(params);
}

static void free_item_memory(void* _item, int index, va_list* params)
{
    property_t* property = NULL;
    event_t* event = NULL;
    service_t* service = NULL;
    lite_property_t* lite_property;
    input_data_t* input_data;
    char* type_str; /* 0: property, 1: event, 2: service. */
    int i, j;
    int output_data_numb;
    int input_data_numb;
    int number;
    void* p;

    type_str = va_arg(*params, char*);
    number = va_arg(*params, int);

    assert(type_str);

    if (strcmp(type_str, string_property) == 0) {
        property = _item;
    }

    else if (strcmp(type_str, string_event) == 0) {
        event = _item;
    }

    else if (strcmp(type_str, string_service) == 0) {
        service = _item;
    }

    else {
        assert(0);
    }

    if (property) {
        if (property->identifier) {
            dm_lite_free(property->identifier);
        }
        if (property->name) {
            dm_lite_free(property->name);
        }
        if (property->desc) {
            dm_lite_free(property->desc);
        }

        property_free_data_type_info(&property->data_type);
        if (index + 1 == number) {
            p = property - index;
            dm_lite_free(p);
        }

    } else if (event) {
        if (event->identifier) {
            dm_lite_free(event->identifier);
        }
        if (event->name) {
            dm_lite_free(event->name);
        }
        if (event->desc) {
            dm_lite_free(event->desc);
        }
        if (event->event_type_str) {
            dm_lite_free(event->event_type_str);
        }
        if (event->method) {
            dm_lite_free(event->method);
        }

        output_data_numb = event->event_output_data_num;
        for (i = 0 ; i < output_data_numb; ++i) {
            lite_property = event->event_output_data + i;
            free_lite_property(lite_property);
        }
        if (output_data_numb) {
            dm_lite_free(event->event_output_data);
        }

        if (index + 1 == number) {
            p = event - index;
            dm_lite_free(p);
        }
    } else if (service) {
        if (service->identifier) {
            dm_lite_free(service->identifier);
        }
        if (service->name) {
            dm_lite_free(service->name);
        }
        if (service->desc) {
            dm_lite_free(service->desc);
        }
        if (service->call_type) {
            dm_lite_free(service->call_type);
        }
        if (service->method) {
            dm_lite_free(service->method);
        }

        output_data_numb = service->service_output_data_num;
        for (i = 0 ; i < output_data_numb; ++i) {
            lite_property = service->service_output_data + i;
            free_lite_property(lite_property);
        }
        if (output_data_numb) {
            dm_lite_free(service->service_output_data);
        }

        input_data_numb = service->service_input_data_num;
        for (j = 0 ; j < input_data_numb; ++j) {
            input_data = service->service_input_data + j;
            free_input_data(input_data, service->service_type);
        }
        if (input_data_numb) {
            dm_lite_free(service->service_input_data);
        }
        if (index + 1 == number) {
            p = service - index;
            dm_lite_free(p);
        }
    }
}
#ifdef ENABLE_THING_DEBUG
static void print_lite_property(void* dst)
{
    lite_property_t* lite_property = (lite_property_t*)dst;

    assert(lite_property);

    dm_printf("\tidentifier:%s, name:%s\n", lite_property->identifier, lite_property->name);

    property_print_data_type_info(&lite_property->data_type);
}

static void print_property(void* dst, ...)
{
    property_t* property = dst;

    va_list params;

    va_start(params, dst);

    item_print_info(property, -1, &params);

    va_end(params);
}

static void print_input_data(void* dst, service_type_t service_type)
{
    input_data_t* input_data = (input_data_t*)dst;

    assert(input_data);

    switch (service_type) {
    case service_type_property_set:
        print_property(&input_data->property_to_set, 6, string_property, string_identifier, string_name, string_accessMode, string_required, string_dataType);
        break;
    case service_type_others:
        print_lite_property(&input_data->lite_property);
        break;
    case service_type_property_get:
        dm_printf("\t\"%s\"\n", input_data->property_to_get_name);
        break;
    default:
        break;
    }
}

static void property_print_data_type_info(void* dst)
{
    data_type_t* data_type = dst;
    data_type_x_t* data_type_x;
    lite_property_t* lite_property;
    int index;
    assert(data_type);

    data_type_x = &data_type->value;
    switch (data_type->type) {
    case data_type_type_int:
#ifdef LITE_THING_MODEL
        dm_printf("\tdataType:{type:%s,min:%d,max:%d\n\tvalue:%d}\n",
                  data_type->type_str,
                  data_type_x->data_type_int_t.min,
                  data_type_x->data_type_int_t.max,
                  data_type_x->data_type_int_t.value);
        break;
#else
        dm_printf("\tdataType:{type:%s,min:%d,max:%d,precise:%d,unit:%s,unitName:%s\n\tvalue:%d}\n",
                  data_type->type_str,
                  data_type_x->data_type_int_t.min,
                  data_type_x->data_type_int_t.max,
                  data_type_x->data_type_int_t.precise,
                  data_type_x->data_type_int_t.unit,
                  data_type_x->data_type_int_t.unit_name,
                  data_type_x->data_type_int_t.value);
        break;
#endif
    case data_type_type_float:
#ifdef LITE_THING_MODEL
        dm_printf("\tdataType:{type:%s,min:%.2f,max:%.2f,precise:%d\n\tvalue:%.2f}\n",
                  data_type->type_str,
                  data_type_x->data_type_float_t.min,
                  data_type_x->data_type_float_t.max,
                  data_type_x->data_type_float_t.precise,
                  data_type_x->data_type_float_t.value);
        break;
#else
        dm_printf("\tdataType:{type:%s,min:%.2f,max:%.2f,precise:%d,unit:%s,unitName:%s\n\tvalue:%.2f}\n",
                  data_type->type_str,
                  data_type_x->data_type_float_t.min,
                  data_type_x->data_type_float_t.max,
                  data_type_x->data_type_float_t.precise,
                  data_type_x->data_type_float_t.unit,
                  data_type_x->data_type_float_t.unit_name,
                  data_type_x->data_type_float_t.value);
        break;
#endif

    case data_type_type_double:
#ifdef LITE_THING_MODEL
        dm_printf("\tdataType:{type:%s,min:%.2f,max:%.2f,precise:%d\n\tvalue:%.7f}\n",
                  data_type->type_str,
                  data_type_x->data_type_double_t.min,
                  data_type_x->data_type_double_t.max,
                  data_type_x->data_type_double_t.precise,
                  data_type_x->data_type_double_t.value);
        break;
#else
        dm_printf("\tdataType:{type:%s,min:%.2f,max:%.2f,precise:%d,unit:%s,unitName:%s\n\tvalue:%.7f}\n",
                  data_type->type_str,
                  data_type_x->data_type_double_t.min,
                  data_type_x->data_type_double_t.max,
                  data_type_x->data_type_double_t.precise,
                  data_type_x->data_type_double_t.unit,
                  data_type_x->data_type_double_t.unit_name,
                  data_type_x->data_type_double_t.value);
        break;
#endif
    case data_type_type_enum:
        dm_printf("\tdataType:{type:%s,", data_type->type_str);
        for (index = 0; index < data_type_x->data_type_enum_t.enum_item_number; ++index) {
            dm_printf("key:%s|value:%s\t", *(data_type_x->data_type_enum_t.enum_item_key + index),
#ifdef LITE_THING_MODEL
                      "NULL");
#else
                      *(data_type_x->data_type_enum_t.enum_item_value + index));
#endif

        }

        dm_printf("\n\tvalue:%d}\n", data_type_x->data_type_enum_t.value);
        break;
    case data_type_type_bool:
        dm_printf("\tdataType:{type:%s,", data_type->type_str);
        for (index = 0; index < data_type_x->data_type_bool_t.bool_item_number; ++index) {
            dm_printf("key:%s|value:%s\t",
                      *(data_type_x->data_type_bool_t.bool_item_key + index),
#ifdef LITE_THING_MODEL
                      "NULL");
#else
                      *(data_type_x->data_type_bool_t.bool_item_value + index));
#endif

        }

        dm_printf("\n\tvalue:%s}\n", data_type_x->data_type_bool_t.value ? string_true : string_false);
        break;
    case data_type_type_text:
#ifdef LITE_THING_MODEL
        dm_printf("\tdataType:{type:%s,length:%d\n\tvalue:%s}\n",
                  data_type->type_str,
                  data_type_x->data_type_text_t.length,
                  data_type_x->data_type_text_t.value);
#else
        dm_printf("\tdataType:{type:%s,length:%d,unit:%s,unitName:%s\n\tvalue:%s}\n",
                  data_type->type_str,
                  data_type_x->data_type_text_t.length,
                  data_type_x->data_type_text_t.unit,
                  data_type_x->data_type_text_t.unit_name,
                  data_type_x->data_type_text_t.value);
#endif
        break;
    case data_type_type_struct:
        dm_printf("\tdataType:%s[\n", data_type->type_str);
        for (index = 0; index < data_type->data_type_specs_number; ++index) {
            lite_property = (lite_property_t*)data_type->specs + index;
            print_lite_property(lite_property);
        }
        dm_printf("\t]\n");
        break;
    case data_type_type_date:
        dm_printf("\tdataType:{type:%s\n\tvalue:%lld}\n",
                  data_type->type_str,
                  data_type_x->data_type_date_t.value);
        break;
    default:
        dm_printf("unsupported type:%s\n", data_type->type_str);
        break;
    }
}

static void item_print_info(void* _item, int index, va_list* params)
{
    property_t* property = NULL;
    event_t* event = NULL;
    service_t* service = NULL;
    lite_property_t* lite_property;
    input_data_t* input_data;
    char* type_str; /* 0: property, 1: event, 2: service. */
    int type;
    char* control_str;
    int length;
    int i, j;
    int output_data_numb;
    int input_data_numb;

    length = va_arg(*params, int);

    assert(length > 1);  /* at least type info and one control type. */

    type_str = control_str = va_arg(*params, char*);

    assert(type_str);

    if (strcmp(type_str, string_property) == 0) {
        type = 0;
        property = _item;
    } else if (strcmp(type_str, string_event) == 0) {
        type = 1;
        event = _item;
    } else if (strcmp(type_str, string_service) == 0) {
        type = 2;
        service = _item;
    } else {
        assert(0);
    }

    if (type_str && index != -1) {
        dm_printf("%s information print strart\n", type_str);
        dm_printf("index: %d\n", index);
    }

    for (i = 1, control_str = va_arg(*params, char*); i < length ; ++i, control_str = va_arg(*params, char*)) {
        if (strcmp(control_str, string_identifier) == 0) {
            dm_printf("\tidentifier:%s\n", 0 == type ? property->identifier : (1 == type ? event->identifier : service->identifier));
        }

        else if (strcmp(control_str, string_name) == 0) {
            dm_printf("\tname:%s\n", 0 == type ? property->name : (1 == type ? event->name : service->name));
        }

        else if (strcmp(control_str, string_accessMode) == 0) {
            dm_printf("\taccessMode:%s\n",
                      property->access_mode == property_access_mode_rw ? "rw" : (property->access_mode == property_access_mode_r ? "r" : "w"));
        } else if (strcmp(control_str, string_required) == 0) {
            dm_printf("\trequired:%s\n", (0 == type ? property->required : (1 == type ? event->required : service->required)) ? string_true : string_false);
        }

        else if (strcmp(control_str, string_desc) == 0) {
            dm_printf("\tdesc:%s\n", 0 == type ? property->desc : (1 == type ? event->desc : service->desc));
        }

        else if (strcmp(control_str, string_dataType) == 0) {
            property_print_data_type_info(&property->data_type);
        }

        else if (strcmp(control_str, string_method) == 0) {
            dm_printf("\tmethod:%s\n", 1 == type ? event->method : service->method);
        }

        else if (strcmp(control_str, string_type) == 0) {
            dm_printf("\ttype:%s\n", event->event_type_str);
        }

        else if (strcmp(control_str, string_outputData) == 0) {
            dm_printf("\toutputData:[\n");
            output_data_numb = 1 == type ? event->event_output_data_num : service->service_output_data_num;
            for (j = 0 ; j < output_data_numb; ++j) {
                lite_property = (1 == type ? event->event_output_data : service->service_output_data) + j;
                print_lite_property(lite_property);
            }
            dm_printf("\t]\n");
        } else if (strcmp(control_str, string_inputData) == 0) {
            dm_printf("\tinputData:[\n");
            input_data_numb = service->service_input_data_num;
            for (j = 0 ; j < input_data_numb; ++j) {
                input_data = service->service_input_data + j;
                print_input_data(input_data, service->service_type);
            }
            dm_printf("\t]\n");
        } else {
            break;
        }
    }

    if (type_str && index != -1) {
        dm_printf("%s information print end\n\n", type_str);
    }

}

static void dm_thing_print_property_lite_info(const void* _self)
{
    const thing_t* self = _self;

    property_iterator((thing_t*)self, item_print_info, 3, string_property, string_identifier, string_accessMode);
}

static void dm_thing_print_property_detail_info(const void* _self)
{
    const thing_t* self = _self;

    property_iterator((thing_t*)self, item_print_info, 6, string_property, string_identifier, string_name, string_accessMode, string_required, string_dataType);
}

static void dm_thing_print_event_lite_info(const void* _self)
{
    const thing_t* self = _self;

    event_iterator((thing_t*)self, item_print_info, 2, string_event, string_identifier);
}

static void dm_thing_print_event_detail_info(const void* _self)
{
    const thing_t* self = _self;

    event_iterator((thing_t*)self, item_print_info, 8, string_event, string_identifier, string_name, string_desc, string_required, string_type, string_method, string_outputData);
}

static void dm_thing_print_service_lite_info(const void* _self)
{
    const thing_t* self = _self;

    service_iterator((thing_t*)self, item_print_info, 2, string_service, string_identifier);
}

static void dm_thing_print_service_detail_info(const void* _self)
{
    const thing_t* self = _self;

    service_iterator((thing_t*)self, item_print_info, 8, string_service, string_identifier, string_name, string_desc, string_required, string_method, string_inputData, string_outputData);
}
#endif /* ENABLE_THING_DEBUG */
static int dm_thing_set_event_value_by_identifier(void* _self, const char* const identifier, const void* value, const char* value_str)
{
    thing_t* self = _self;
    lite_property_t* lite_property;
    event_t* event;
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    char* p;
    char id_buff[DM_THING_MAX_IDENTIFIER_LENGTH] = {0};

    assert(strchr(identifier, DEFAULT_TSL_DELIMITER)); /* set lite property value of outputData. */
    if (strchr(identifier, DEFAULT_TSL_DELIMITER)  == NULL || strlen(identifier) >= DM_THING_MAX_IDENTIFIER_LENGTH) return -1;
    /* do some checks. */
    strcpy(id_buff, identifier);
    p = strtok(id_buff, delimeter);

    event = dm_thing_get_event_by_identifier(self, p);
    /* not allowed to set value to this type of event. */
    if (event == NULL || strcmp(event->method, string_thing_event_property_post) == 0) return -1;

    lite_property = dm_thing_get_event_by_identifier(self, identifier);

    assert(lite_property);
    if (lite_property == NULL) return -1;

    return set_lite_property_value(self, lite_property, value, value_str);
}

static int dm_thing_get_event_value_by_identifier(void* _self, const char* const identifier, void* value, char** value_str)
{
    thing_t* self = _self;
    lite_property_t* lite_property;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    event_t* event;
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
    char* p;
    char id_buff[MAX_IDENTIFIER_LENGTH] = {0};
#endif
    assert(strchr(identifier, DEFAULT_TSL_DELIMITER)); /* set lite property value of outputData. */
    if (strchr(identifier, DEFAULT_TSL_DELIMITER) == NULL) return -1;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    /* do some checks. */
    strcpy(id_buff, identifier);
    p = strtok(id_buff, delimeter);
    event = dm_thing_get_event_by_identifier(self, p);
    assert(strcmp(event->method, string_thing_event_property_post) != 0); /* not allowed to set value to this type of event. */
#endif
    lite_property = dm_thing_get_event_by_identifier(self, identifier);

    assert(lite_property);

    if (lite_property == NULL) return -1;

    return dm_thing_get_lite_property_value(self, lite_property, value, value_str);
}

static int dm_thing_set_service_input_output_data_value_by_identifier(void* _self, const char* const identifier,
                                                                      const void* value, const char* value_str)
{
    thing_t* self = _self;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    service_t* service;
    char* p;
    char id_buff[MAX_IDENTIFIER_LENGTH] = {0};
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
#endif
    lite_property_t* lite_property;

    assert(strchr(identifier, DEFAULT_TSL_DELIMITER)); /* set lite property value of inputData. */
    if (strchr(identifier, DEFAULT_TSL_DELIMITER) == NULL) return -1;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    /* do some checks. */
    strcpy(id_buff, identifier);
    p = strtok(id_buff, delimeter);

    service = dm_thing_get_service_by_identifier(self, p);

    assert(strcmp(service->method, string_thing_service_property_get) != 0 &&
            strcmp(service->method, string_thing_service_property_set) != 0); /* not allowed to set value to this type of service. */
#endif
    lite_property = dm_thing_get_service_by_identifier(self, identifier);
    if (lite_property == NULL) return -1;

    return set_lite_property_value(self, lite_property, value, value_str);
}

static int dm_thing_get_service_input_output_data_value_by_identifier(const void* _self, const char* const identifier,
                                                                      void* value, char** value_str)
{
    const thing_t* self = _self;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    service_t* service;
    char* p;
    char id_buff[MAX_IDENTIFIER_LENGTH] = {0};
    char delimeter[] = {DEFAULT_TSL_DELIMITER, 0};
#endif
    lite_property_t* lite_property;

    assert(strchr(identifier, DEFAULT_TSL_DELIMITER)); /* set lite property value of inputData. */
    if (strchr(identifier, DEFAULT_TSL_DELIMITER) == NULL) return -1;
#ifdef PROPERTY_ACCESS_MODE_ENABLED
    /* do some checks. */
    strcpy(id_buff, identifier);
    p = strtok(id_buff, delimeter);

    service = dm_thing_get_service_by_identifier(self, p);
    assert(strcmp(service->method, string_thing_service_property_get) != 0 &&
            strcmp(service->method, string_thing_service_property_set) != 0); /* not allowed to set value to this type of service. */
#endif
    lite_property = dm_thing_get_service_by_identifier(self, identifier);

    if (lite_property == NULL) return -1;

    return dm_thing_get_lite_property_value(self, lite_property, value, value_str);
}

static thing_t _dm_thing_class = {
    sizeof(dm_thing_t),
    string_dm_thing_class_name,
    dm_thing_ctor,
    dm_thing_dtor,
    dm_thing_set_tsl_string,
    dm_thing_get_tsl_string,
    dm_thing_get_property_number,
    dm_thing_get_service_number,
    dm_thing_get_event_number,
    dm_thing_get_property_by_identifier,
    dm_thing_get_property_identifier_by_index,
    dm_thing_get_service_by_identifier,
    dm_thing_get_service_identifier_by_index,
    dm_thing_get_event_by_identifier,
    dm_thing_get_event_identifier_by_index,
    dm_thing_get_schema,
    dm_thing_get_link,
    dm_thing_set_product_key,
    dm_thing_get_product_key,
    dm_thing_return_product_key,
    dm_thing_set_device_name,
    dm_thing_get_device_name,
    dm_thing_return_device_name,
    dm_thing_set_property_value_by_identifier,
    dm_thing_set_property_value,
    dm_thing_get_property_value_by_identifier,
    dm_thing_get_property_value,
    dm_thing_property_iterator,
    dm_thing_event_iterator,
    dm_thing_service_iterator,
    #ifdef ENABLE_THING_DEBUG
    dm_thing_print_property_lite_info,
    dm_thing_print_property_detail_info,
    dm_thing_print_event_lite_info,
    dm_thing_print_event_detail_info,
    dm_thing_print_service_lite_info,
    dm_thing_print_service_detail_info,
    #endif /* ENABLE_THING_DEBUG */
    dm_thing_set_event_value_by_identifier,
    dm_thing_get_event_value_by_identifier,
    dm_thing_set_service_input_output_data_value_by_identifier,
    dm_thing_get_service_input_output_data_value_by_identifier,
    dm_thing_get_lite_property_value,
};

const void* get_dm_thing_class()
{
    return &_dm_thing_class;
}
