/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "cJSON.h"
#include "tsl_file.h"
#include "tsl_opt_impl.h"
#include "iot_import.h"

static const char string_thing_service_property_set[] = "thing.service.property.set";
static const char string_thing_service_property_get[] = "thing.service.property.get";
static const char string_thing_event_property_post[]  = "thing.event.property.post";

static int tsl_format_opt_write(const char *filename, const char *buf, int buf_len)
{
    char diagnoal = '\\';
    int ret = -1;
    const char *p;
    FILE *fp = NULL;

    if (!filename || !buf || buf_len < 0) {
        goto do_exit;
    }

    p = buf;
    fp = fopen(filename, "w+");
    if (!fp) {
        TFormat_printf("[err] fopen(%s) failed\n", filename);
        goto do_exit;
    }
    while (p && *p && (p < buf + buf_len)) {
        if (*p == '\"') {
            if (tsl_file_write(&diagnoal, fp, 1) < 0) {
                goto do_exit;
            }
        }
        if (tsl_file_write(p, fp, 1) < 0) {
            goto do_exit;
        }
        p++;
    }

    ret = 0;
do_exit:
    if(fp) fclose(fp);
    return ret;
}

static void tsl_slim_root(cJSON *root)
{
    int i = 0;
    cJSON *obj = root;
    const char *slim_root_identifier[] = {"schema","link"};

    if(!obj) return;

    for(i = 0; i < sizeof(slim_root_identifier) / sizeof(char*); i++) {
        cJSON_DeleteItemFromObject(obj, slim_root_identifier[i]);
    }
}

static void tsl_slim_specs(cJSON *specs)
{
    if(!specs) return;
    cJSON_DeleteItemFromObject(specs, "unit");
    cJSON_DeleteItemFromObject(specs, "unitName");
}

static void tsl_slim_properties_item(cJSON *property_item);

static void tsl_slim_properties(cJSON *properties_obj);

static void tsl_slim_dataType(cJSON *dataType)
{
    cJSON *type;
    cJSON *specs_obj;
    cJSON *specs_item;
    int i = 0;
    int spec_size;
    if(!dataType) return;
    specs_obj = cJSON_GetObjectItem(dataType, "specs");
    type = cJSON_GetObjectItem(dataType, "type");
    if(cJSON_IsString(type) && 0 == strcmp("struct", type->valuestring)) {
        if(cJSON_IsArray(specs_obj)) {
            spec_size = cJSON_GetArraySize(specs_obj);
            for(i = 0; i < spec_size; i++) {
                specs_item = cJSON_GetArrayItem(specs_obj, i);
                tsl_slim_properties_item(specs_item);
            }
        }else {
            TFormat_printf("[err] specs is not array");
        }
    }else {
        tsl_slim_specs(specs_obj);
    }
}

static void tsl_slim_properties_item(cJSON *property_item)
{
    if(!property_item) return;
    cJSON_DeleteItemFromObject(property_item, "name");
    cJSON_DeleteItemFromObject(property_item, "desc");
    tsl_slim_dataType(cJSON_GetObjectItem(property_item,"dataType"));
}
static void tsl_slim_properties(cJSON *properties_obj)
{
    int i;
    int arr_size;
    cJSON *properties_item;
    if(!properties_obj) return;

    arr_size = cJSON_GetArraySize(properties_obj);
    for(i = 0; i < arr_size; i++) {
        properties_item = cJSON_GetArrayItem(properties_obj, i);
        if(properties_item) {
            tsl_slim_properties_item(properties_item);
        }
    }
}

static void tsl_slim_events(cJSON *events_obj)
{
    int i;
    int arr_size;
    int output_data_index;
    int output_data_size;
    cJSON *events_item;
    cJSON *method;
    cJSON *output_data;
    cJSON *property;

    if(!events_obj) return;

    arr_size = cJSON_GetArraySize(events_obj);
    for(i = 0; i < arr_size; i++) {
        events_item = cJSON_GetArrayItem(events_obj, i);
        method = cJSON_GetObjectItem(events_item,"method");
        cJSON_DeleteItemFromObject(events_item, "name");
        cJSON_DeleteItemFromObject(events_item, "desc");
        if(cJSON_IsString(method) && (0 == strcmp(method->valuestring, string_thing_event_property_post))) {
            cJSON_DeleteItemFromObject(events_item, "outputData");
            continue;
        }

        output_data = cJSON_GetObjectItem(events_item, "outputData");
        output_data_size = cJSON_GetArraySize(output_data);
        for(output_data_index = 0; output_data_index < output_data_size; output_data_index++) {
            property = cJSON_GetArrayItem(output_data, output_data_index);
            if(property) {
                tsl_slim_properties_item(property);
            }
        }
    }
}

static void tsl_slim_services(cJSON *services_obj)
{
    int i;
    int arr_size;
    int data_size;
    int data_index;
    cJSON *service_item;
    cJSON *method;
    cJSON *output_data;
    cJSON *input_data;
    cJSON *property;

    if(!services_obj) return;
    arr_size = cJSON_GetArraySize(services_obj);
    for(i = 0; i < arr_size; i++) {
        service_item = cJSON_GetArrayItem(services_obj, i);
        if(service_item) {
            method = cJSON_GetObjectItem(service_item,"method");
            cJSON_DeleteItemFromObject(service_item, "name");
            cJSON_DeleteItemFromObject(service_item, "desc");

            if(cJSON_IsString(method) && (0 == strcmp(method->valuestring, string_thing_service_property_get))) {
                cJSON_DeleteItemFromObject(service_item, "outputData");
            }else if(cJSON_IsString(method) && (0 == strcmp(method->valuestring, string_thing_service_property_set))) {
                cJSON_DeleteItemFromObject(service_item, "inputData");
            }else {
                output_data = cJSON_GetObjectItem(service_item, "outputData");
                data_size = cJSON_GetArraySize(output_data);
                for(data_index = 0; data_index < data_size; data_index++) {
                    property = cJSON_GetArrayItem(output_data, data_index);
                    if(property) {
                        tsl_slim_properties_item(property);
                    }
                }
                input_data = cJSON_GetObjectItem(service_item, "inputData");
                data_size = cJSON_GetArraySize(input_data);
                for(data_index = 0; data_index < data_size; data_index++) {
                    property = cJSON_GetArrayItem(input_data, data_index);
                    if(property) {
                        tsl_slim_properties_item(property);
                    }
                }
            }
        }
    }
}


static int tsl_form_slim(cJSON *tsl_obj)
{
    cJSON *properties_obj;
    cJSON *services_obj;
    cJSON *events_obj;

    int ret = -1;
    if(!tsl_obj) goto do_exit;

    tsl_slim_root(tsl_obj);
    properties_obj = cJSON_GetObjectItem(tsl_obj, "properties");
    if(properties_obj) {
        tsl_slim_properties(properties_obj);
    }
    events_obj = cJSON_GetObjectItem(tsl_obj, "events");
    if(events_obj) {
        tsl_slim_events(events_obj);
    }
    services_obj = cJSON_GetObjectItem(tsl_obj, "services");
    if(services_obj) {
        tsl_slim_services(services_obj);
    }
    ret = 0;
do_exit:
    return ret;
}

int tsl_format_opt_convert(const char *src_filename, const char *dst_filename, int is_slim)
{
    cJSON *cjson = NULL;
    int ret = -1;
    char *src_buf = NULL;
    int src_buf_len = 0;
    char *p_unformat_json = NULL;

    if (!src_filename || !dst_filename) {
        TFormat_printf("[err] invalid params\n");
        goto do_exit;
    }

    src_buf = tsl_read_from_file(src_filename, &src_buf_len);
    if (!src_buf) {
        TFormat_printf("[err] read file(%s) failed\n", src_filename);
        goto do_exit;
    }
    cjson = cJSON_Parse(src_buf);
    if(!cjson) {
        TFormat_printf("[err] parse json failed\n");
        goto do_exit;
    }
    if(is_slim) {
        tsl_form_slim(cjson);
    }
    p_unformat_json = cJSON_PrintUnformatted(cjson);
    if(!p_unformat_json) {
        TFormat_printf("[err] invalid params");
        goto do_exit;
    }
    if (0 != tsl_format_opt_write(dst_filename, p_unformat_json, strlen(p_unformat_json))) {
        TFormat_printf("[err] write file(%s) failed\n", dst_filename);
        goto do_exit;
    }
    ret = 0;
do_exit:
    if (p_unformat_json) {
        HAL_Free(p_unformat_json);
    }
    if (cjson) {
        cJSON_Delete(cjson);
    }
    if (src_buf) {
        HAL_Free(src_buf);
    }
    return ret;
}

int tsl_format_opt_dump(const char *filename)
{
    int ret = -1;
    char *buf = NULL;
    int size;
    if(!filename) {
        goto do_exit;
    }
    buf = tsl_read_from_file(filename, &size);
    if(buf) {
        TFormat_printf("[OK] conv tsl:\n%s\n", buf);
        HAL_Free(buf);
        ret = 0;
    }else {
        TFormat_printf("[ERR] fopen(%s) failed\n", filename);
    }
do_exit:
    return ret;
}

