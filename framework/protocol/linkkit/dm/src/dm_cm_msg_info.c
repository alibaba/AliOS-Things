#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "class_interface.h"
#include "interface/list_abstract.h"
#include "interface/message_info_abstract.h"
#include "interface/log_abstract.h"

#include "dm_cm_msg_info.h"
#include "dm_logger.h"
#include "dm_import.h"
#include "iot_export_dm.h"
#include "dm_slist.h"

#define DM_CM_MSG_INFO_EXTENTED_ROOM_FOR_STRING_MALLOC 1

static const char string_dm_cm_msg_info_class_name[] __DM_READ_ONLY__ = "cm_msg_info_cls";
static const char string_dm_cm_msg_info_param_list_object_name[] __DM_READ_ONLY__ = "cm message info param list";

static void dm_cm_msg_info_clear(void* _self);
static void dm_cm_msg_info_set_params_data(void* _self, char* params_buf);

static void list_insert(void* _list, void* _data)
{
    list_t** list = (list_t**)_list;

    (*list)->insert(list, _data);
}

static void* dm_cm_msg_info_ctor(void* _self, va_list* params)
{
    dm_cm_msg_info_t* self = _self;

    self->uri = NULL;
    self->payload_buf = NULL;
    self->params_data_buf = NULL;
    self->params_data_buf = NULL;
    self->raw_data_length = 0;
    self->product_key = NULL;
    self->device_name = NULL;
    self->id = 0;
    self->code = 0;
    self->version = NULL;
    self->param_list = new_object(DM_SLIST_CLASS, string_dm_cm_msg_info_param_list_object_name);
    self->method = NULL;
    self->message_type = DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST;

    (void)params;

    return self;
}

static int deep_strcpy(char** _dst, const void* _src)
{
    char** dst = _dst;
    const char* src = _src;
    int size = strlen(src) + DM_CM_MSG_INFO_EXTENTED_ROOM_FOR_STRING_MALLOC;

    *dst = dm_lite_calloc(1, size);

    assert(*dst);
    if (*dst == NULL) return -1;

    strcpy(*dst, src);

    return 0;
}

static void* dm_cm_msg_info_dtor(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    dm_cm_msg_info_clear(self);
    delete_object(self->param_list);

    return self;
}

static int dm_cm_msg_info_set_uri(void* _self, char* uri)
{
    dm_cm_msg_info_t* self = _self;

    if (self->uri) {
        dm_lite_free(self->uri);
        self->uri = NULL;
    }
    assert(uri);
    if (uri) {
        return deep_strcpy(&self->uri, uri);
    }

    return -1;
}

static void dm_cm_msg_info_set_payload(void* _self, char* payload_buf, unsigned int payload_len)
{
    dm_cm_msg_info_t* self = _self;

    if (self->payload_buf) {
        dm_lite_free(self->payload_buf);
        self->payload_buf = NULL;
    }
    assert(payload_buf);
    if (payload_buf) deep_strcpy(&self->payload_buf, payload_buf);
}

static void clear_req_rsp_params(void* _req_rsp_param, va_list* params)
{
    req_rsp_param_t* req_rsp_param = _req_rsp_param;
    dm_cm_msg_info_t* cm_message_info;

    cm_message_info = va_arg(*params, void*);

    assert(cm_message_info);

    if (req_rsp_param) {
        if (req_rsp_param->key) dm_lite_free(req_rsp_param->key);
        if (req_rsp_param->value) dm_lite_free(req_rsp_param->value);

        dm_lite_free(req_rsp_param);

        req_rsp_param = NULL;
    }
}

static void dm_cm_msg_info_clear(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    if (self->uri) {
        dm_lite_free(self->uri);
        self->uri = NULL;
    }

    if (self->payload_buf) {
        dm_lite_free(self->payload_buf);
        self->payload_buf = NULL;
    }

    if (self->product_key) {
        dm_lite_free(self->product_key);
        self->product_key = NULL;
    }

    if (self->device_name) {
        dm_lite_free(self->device_name);
        self->device_name = NULL;
    }

    if (self->params_data_buf) {
        dm_lite_free(self->params_data_buf);
        self->params_data_buf = NULL;
    }

    if (self->raw_data_buf) {
        dm_lite_free(self->raw_data_buf);
        self->raw_data_buf = NULL;
        self->raw_data_length = 0;
    }

    if (self->version) {
        dm_lite_free(self->version);
        self->version = NULL;
    }

    self->id = 0;

    if (self->param_list) {
        list_t** list = self->param_list;
        list_iterator(list, clear_req_rsp_params, self);
        (*list)->clear(list);
    }

    if (self->method) {
        dm_lite_free(self->method);
        self->method = NULL;
    }
}

static void* dm_cm_msg_info_get_uri(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->uri;
}

static void* dm_cm_msg_info_get_payload(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->payload_buf;
}

static void dm_cm_msg_info_set_id(void* _self, int id)
{
    dm_cm_msg_info_t* self = _self;
    self->id = id;
}

static int dm_cm_msg_info_get_id(void* _self)
{
    dm_cm_msg_info_t* self = _self;
    return self->id;
}

static void dm_cm_msg_info_set_version(void* _self, const char* version)
{
    dm_cm_msg_info_t* self = _self;

    assert(version);

    if (self->version) {
        dm_lite_free(self->version);
        self->version = NULL;
    }
    assert(version);
    if (version) deep_strcpy(&self->version, version);
}

static char* dm_cm_msg_info_get_version(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->version;
}

static void dm_cm_msg_info_add_params_data_item(void* _self, const char* key, const char* value)
{
    dm_cm_msg_info_t* self = _self;
    list_t** list = self->param_list;
    req_rsp_param_t* req_rsp_param;

    req_rsp_param = (req_rsp_param_t*)dm_lite_calloc(1, sizeof(req_rsp_param_t));

    assert(req_rsp_param);

    if (req_rsp_param == NULL) return;

    if (key && value) {
        deep_strcpy(&req_rsp_param->key, key);
        deep_strcpy(&req_rsp_param->value, value);
    }

    list_insert(list, req_rsp_param);
}

static void dm_cm_msg_info_set_method(void* _self, const char* method)
{
    dm_cm_msg_info_t* self = _self;

    if (self->method) {
        dm_lite_free(self->method);
        self->method = NULL;
    }

    if (method) deep_strcpy(&self->method, method);
}

static char* dm_cm_msg_info_get_method(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->method;
}

static void serialize_params_data(void* _req_rsp_param, va_list* _params)
{
    req_rsp_param_t* req_rsp_param = _req_rsp_param;
    dm_cm_msg_info_t* cm_message_info;
    char* params;
    int len = 6; /* "key":"value"\0 */

    cm_message_info = va_arg(*_params, void*);
    params = va_arg(*_params, char*);

    assert(req_rsp_param && cm_message_info && req_rsp_param->key && req_rsp_param->value && params);

    if (strcmp(params, "{}") == 0) {
        *(params + 1) = 0;
    }
    else {
        *(params + strlen(params) - 1) = ','; /* change from '}' to ',' */
    }

    if (req_rsp_param && cm_message_info && req_rsp_param->key && req_rsp_param->value) {
        len += strlen(req_rsp_param->key);
        len += strlen(req_rsp_param->value);

        /* check if there is enough room for new key-value. */
        if (len > (DM_CM_MSG_INFO_PARAMS_LENGTH_MAX - strlen(params))) {
            assert(0);
            return;
        }

        dm_sprintf(params + strlen(params), "\"%s\":%s}", req_rsp_param->key, req_rsp_param->value);
    }
}

static int dm_cm_msg_info_serialize_to_payload_request(void* _self)
{
    dm_cm_msg_info_t* self = _self;
    const list_t** list = self->param_list;
    char params[DM_CM_MSG_INFO_PARAMS_LENGTH_MAX] = {'{', '}', 0};
#if 0
    char request[CM_MESSAGE_INFO_PARAMS_LENGTH_MAX + 32] = {0};
#endif
    int ret = -1;

    assert(self->version && self->method && list && (*list));
    if (self->version && self->method && list && (*list)) {
        list_iterator(list, serialize_params_data, self, params);
#if 0
        dm_snprintf(request, CM_MESSAGE_INFO_PARAMS_LENGTH_MAX + 32, "{\"id\":%d,\"version\":\"%s\",\"params\":%s,\"method\":\"%s\"}",
                    self->id, self->version, params, self->method);
        dm_cm_msg_info_set_payload(self, request, strlen(request));
#endif
        dm_cm_msg_info_set_params_data(self, params);

        /* for debug only. */
        dm_printf("\nrequest params:\n%s\n\n", params);
        //        dm_printf("\nrequest:\n%s\n\n", request);

        ret = 0;
    }

    return ret;
}

static int dm_cm_msg_info_serialize_to_payload_response(void* _self)
{
    dm_cm_msg_info_t* self = _self;
    const list_t** list = self->param_list;
    char data[DM_CM_MSG_INFO_PARAMS_LENGTH_MAX] = {'{', '}', 0};
#if 0
    char response[CM_MESSAGE_INFO_PARAMS_LENGTH_MAX + 32] = {0};
#endif
    int ret = -1;

    assert(list && (*list));
    if (list && (*list)) {
        list_iterator(list, serialize_params_data, self, data);
#if 0
        dm_snprintf(response, CM_MESSAGE_INFO_PARAMS_LENGTH_MAX + 32, "{\"id\":%d,\"code\":%d,\"data\":%s}",
                    self->id, self->code, data);
        dm_cm_msg_info_set_payload(self, response, strlen(response));
#endif
        dm_cm_msg_info_set_params_data(self, data);

        /* for debug only. */
        dm_printf("\nresponse data:\n%s\n\n", data);
        //        dm_printf("\nresponse:\n%s\n\n", response);

        ret = 0;
    }

    return ret;
}

static void dm_cm_msg_info_set_message_type(void* _self, int message_type)
{
    dm_cm_msg_info_t* self = _self;

    self->message_type = message_type;
}

static int dm_cm_msg_info_get_message_type(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->message_type;
}

static char* dm_cm_msg_info_get_params_data(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->params_data_buf;
}


/* malloc mem and copy payload. */
static void dm_cm_msg_info_set_params_data(void* _self, char* params_data_buf)
{
    dm_cm_msg_info_t* self = _self;

    assert(params_data_buf);

    if (self->params_data_buf) {
        dm_lite_free(self->params_data_buf);
        self->params_data_buf = NULL;
    }

    if (params_data_buf) deep_strcpy(&self->params_data_buf, params_data_buf);
}

/* malloc mem and copy payload. */
static int dm_cm_msg_info_set_raw_data_and_length(void* _self, void* raw_data, int raw_data_length)
{
    dm_cm_msg_info_t* self = _self;

    if (self->raw_data_buf) {
        dm_lite_free(self->params_data_buf);
        self->raw_data_buf = NULL;
    }

    self->raw_data_buf = dm_lite_calloc(1, raw_data_length);
    assert(self->raw_data_buf);
    if (self->raw_data_buf == NULL) return -1;
    self->raw_data_length = raw_data_length;

    memcpy(self->raw_data_buf, raw_data, raw_data_length);

    return 0;
}


static void* dm_cm_msg_info_get_raw_data(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->raw_data_buf;
}

static int dm_cm_msg_info_get_raw_data_length(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->raw_data_length;
}

static char* dm_cm_msg_info_get_product_key(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->product_key;
}

/* malloc mem and copy payload. */
static void dm_cm_msg_info_set_product_key(void* _self, char* product_key)
{
    dm_cm_msg_info_t* self = _self;

    if (self->product_key) {
        dm_lite_free(self->product_key);
        self->product_key = NULL;
    }
    if (product_key) deep_strcpy(&self->product_key, product_key);
}

static char* dm_cm_msg_info_get_device_name(void* _self)
{
    dm_cm_msg_info_t* self = _self;

    return self->device_name;
}

/* malloc mem and copy payload. */
static void dm_cm_msg_info_set_device_name(void* _self, char* device_name)
{
    dm_cm_msg_info_t* self = _self;

    if (self->device_name) {
        dm_lite_free(self->device_name);
        self->device_name = NULL;
    }

    if (device_name) deep_strcpy(&self->device_name, device_name);
}

static void dm_cm_msg_info_set_code(void* _self, int code)
{
    dm_cm_msg_info_t* self = _self;
    self->code = code;
}

static int dm_cm_msg_info_get_code(void* _self)
{
    dm_cm_msg_info_t* self = _self;
    return self->code;
}

#ifdef LOCAL_CONN_ENABLE
static void* dm_cm_msg_info_get_conn_ctx(void* _self)
{
    dm_cm_msg_info_t* self = _self;
    return self->conn_ctx;
}

static void dm_cm_msg_info_set_conn_ctx(void* _self, void* _conn_ctx)
{
    dm_cm_msg_info_t* self = _self;

    self->conn_ctx = _conn_ctx;
}
#endif

static const message_info_t _dm_cm_msg_info_class = {
    sizeof(dm_cm_msg_info_t),
    string_dm_cm_msg_info_class_name,
    dm_cm_msg_info_ctor,
    dm_cm_msg_info_dtor,
    dm_cm_msg_info_set_uri,
    dm_cm_msg_info_set_payload,
    dm_cm_msg_info_clear,
    dm_cm_msg_info_get_uri,
    dm_cm_msg_info_get_payload,
    dm_cm_msg_info_set_id,
    dm_cm_msg_info_get_id,
    dm_cm_msg_info_set_version,
    dm_cm_msg_info_get_version,
    dm_cm_msg_info_add_params_data_item,
    dm_cm_msg_info_set_method,
    dm_cm_msg_info_get_method,
    dm_cm_msg_info_serialize_to_payload_request,
    dm_cm_msg_info_serialize_to_payload_response,
    dm_cm_msg_info_set_message_type,
    dm_cm_msg_info_get_message_type,
    dm_cm_msg_info_get_params_data,
    dm_cm_msg_info_set_params_data,
    dm_cm_msg_info_set_raw_data_and_length,
    dm_cm_msg_info_get_raw_data,
    dm_cm_msg_info_get_raw_data_length,
    dm_cm_msg_info_get_product_key,
    dm_cm_msg_info_set_product_key,
    dm_cm_msg_info_get_device_name,
    dm_cm_msg_info_set_device_name,
    dm_cm_msg_info_set_code,
    dm_cm_msg_info_get_code,
#ifdef LOCAL_CONN_ENABLE
    dm_cm_msg_info_get_conn_ctx,
    dm_cm_msg_info_set_conn_ctx,
#endif
};

const void* get_dm_cm_msg_info_class()
{
    return &_dm_cm_msg_info_class;
}
