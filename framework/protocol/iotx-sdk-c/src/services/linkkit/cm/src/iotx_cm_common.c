/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <assert.h>

#include "iotx_cm_common.h"
#include "iotx_cm_connectivity.h"
#ifdef CM_VIA_CLOUD_CONN
#include "iotx_cm_cloud_conn.h"
#endif /* CM_SUPPORT_CLOUD_CONN */
#ifdef CM_SUPPORT_LOCAL_CONN
#include "iotx_cm_local_conn.h"
#endif /* CM_SUPPORT_LOCAL_CONN */
#include "utils_httpc.h"
#include "utils_hmac.h"

#define IOTX_CM_MESSAGE_ID_MAX     (65535)
#define CM_AUTH_TIMEOUT            (10 * 1000)


#ifdef CM_MAPPING_USE_POOL

static iotx_cm_mapping_t g_cm_register_mapping_list[CM_SUPPORT_MAX_MAPPING_SIZE];
static iotx_cm_mapping_t g_cm_service_mapping_list[CM_SUPPORT_MAX_MAPPING_SIZE];

#endif


static const char string_id[] CM_READ_ONLY = "id";
static const char string_code[] CM_READ_ONLY = "code";
static const char string_data[] CM_READ_ONLY = "data";
static const char string_message[] CM_READ_ONLY = "message";
static const char string_params[] CM_READ_ONLY = "params";
static const char string_method[] CM_READ_ONLY = "method";
static const char string_pk[] CM_READ_ONLY = "data.productKey";
static const char string_dn[] CM_READ_ONLY = "data.deviceName";
static const char string_ds[] CM_READ_ONLY = "data.deviceSecret";
static const char string_response_format[] CM_READ_ONLY = "{\"id\":\"%u\",\"code\":%d,\"data\":%s}";
static const char string_request_format[] CM_READ_ONLY = "{\"id\":\"%u\",\"version\":\"1.0\",\"params\":%s,\"method\":\"%s\"}";
static const char string_SYS_URI[] CM_READ_ONLY = "/sys/%s/%s/";
static const char string_EXT_URI[] CM_READ_ONLY = "/ext/%s/%s/";
static const char string_SYS_URI_1[] CM_READ_ONLY = "/sys/%s/%s/%s";
static const char string_EXT_URI_1[] CM_READ_ONLY = "/ext/%s/%s/%s";
static const char string_SHA_METHOD[] CM_READ_ONLY = "hmacsha1";
static const char string_MD5_METHOD[] CM_READ_ONLY = "hmacmd5";
static const char string_TIMESTAMP[] CM_READ_ONLY = "2524608000000";
static const char string_AUTH_URL[] CM_READ_ONLY = "https://iot-auth.cn-shanghai.aliyuncs.com/auth/register/device";
static const char string_AUTH_URL_1[] CM_READ_ONLY = "https://iot-auth.ap-southeast-1.aliyuncs.com/auth/register/device";
static const char string_AUTH_CONTENT_TYPE[] CM_READ_ONLY = "application/x-www-form-urlencoded";
static const char string_hmac_format[] CM_READ_ONLY = "deviceName%s" "productKey%s" "random%s";
static const char string_auth_req_format[] CM_READ_ONLY = "productKey=%s&" "deviceName=%s&" "signMethod=%s&" "sign=%s&" "version=default&" "clientId=%s&" "random=%s&" "resources=mqtt";
static const int  int_random_length = 15;

#if 0
static int iotx_cm_get_next_message_id(iotx_cm_conntext_t* cm_ctx)
{
    if (cm_ctx->cm_message_id == IOTX_CM_MESSAGE_ID_MAX) cm_ctx->cm_message_id = 1;
    cm_ctx->cm_message_id++;
    return cm_ctx->cm_message_id;
}
#endif

#ifdef SUPPORT_PRODUCT_SECRET
static char* genRandomString(int length)
{
    int flag, i;
    char* str;
    if ((str = (char*) LITE_malloc(length)) == NULL )
    {
        CM_ERR(cm_log_error_memory);
        return NULL;
    }

    for (i = 0; i < length - 1; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
            case 0:
                str[i] = 'A' + HAL_Random(26);
                break;
            case 1:
                str[i] = 'a' + HAL_Random(26);
                break;
            case 2:
                str[i] = '0' + HAL_Random(10);
                break;
            default:
                str[i] = 'x';
                break;
        }
    }
    str[length - 1] = '\0';
    return str;
}

int _http_response(char *payload,
                   const int payload_len,
                   const char *request_string,
                   const char *url,
                   const int port_num,
                   const char *pkey
                  )
{
#define HTTP_POST_MAX_LEN   (1024)
#define HTTP_RESP_MAX_LEN   (1024)

    int ret = SUCCESS_RETURN;
    char *requ_payload = NULL;
    char *resp_payload = NULL;
    httpclient_t httpc;
    httpclient_data_t httpc_data;

    memset(&httpc, 0, sizeof(httpclient_t));
    memset(&httpc_data, 0, sizeof(httpclient_data_t));

    /* todo */
    httpc.header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";

    requ_payload = (char *)CM_malloc(HTTP_POST_MAX_LEN);
    if (NULL == requ_payload) {
        CM_ERR(cm_log_error_memory);
        return ERROR_MALLOC;
    }
    memset(requ_payload, 0, HTTP_POST_MAX_LEN);

    HAL_Snprintf(requ_payload,
                 HTTP_POST_MAX_LEN,
                 "%s",
                 request_string);
    CM_INFO(cm_log_info_auth_payload_req, payload);

    resp_payload = (char *)CM_malloc(HTTP_RESP_MAX_LEN);
    if (!resp_payload) {
        ret = FAIL_RETURN;
        goto RETURN;
    }
    LITE_ASSERT(resp_payload);
    memset(resp_payload, 0, HTTP_RESP_MAX_LEN);

    httpc_data.post_content_type = (char*)string_AUTH_CONTENT_TYPE;
    httpc_data.post_buf = requ_payload;
    httpc_data.post_buf_len = strlen(requ_payload);
    httpc_data.response_buf = resp_payload;
    httpc_data.response_buf_len = HTTP_RESP_MAX_LEN;

    ret = httpclient_common(&httpc,
                            url,
                            port_num,
                            pkey,
                            HTTPCLIENT_POST,
                            CM_AUTH_TIMEOUT,
                            &httpc_data);
    if (ret != 0) {
        ret = FAIL_RETURN;
        goto RETURN;
    }

    memcpy(payload, httpc_data.response_buf, payload_len);
    CM_INFO(cm_log_info_auth_payload_rsp, payload);

RETURN:
    if (requ_payload) {
        LITE_free(requ_payload);
        requ_payload = NULL;
    }
    if (resp_payload) {
        LITE_free(resp_payload);
        resp_payload = NULL;
    }

    return ret;
}


static char *_set_auth_req_str(const char *product_key, const char *device_name, const char *client_id, const char *sign, const char *ts)
{
#define AUTH_STRING_MAXLEN  (1024)

    char *req = NULL;

    req = CM_malloc(AUTH_STRING_MAXLEN);
    memset(req, 0, AUTH_STRING_MAXLEN);

    HAL_Snprintf(req
                 , AUTH_STRING_MAXLEN
                 , string_auth_req_format
                 , product_key
                 , device_name
#if USING_SHA1_IN_HMAC
                 , string_SHA_METHOD
#else
                 , string_MD5_METHOD
#endif
                 , sign
                 , client_id
                 , ts);
    return req;
}

static int _get_device_secret(const char *product_key, const char *device_name, const char *client_id, const char *guider_addr, const char *request_string)
{
    char payload[512] = {0};
    int ret = -1;
    int ret_code = 0;
    const char *pvalue;

    /*
        {
            "code" : 200,
            "data" : {
                "productKey" : "42Ze0mk3556498a1AlTP",
                "deviceName" : "0d7fdeb9dc1f4344a2cc0d45edcb0bcb",
                "deviceSecret" : "adsfweafdsf"
        }
    */
    strncpy(payload,request_string,strlen(request_string));
    _http_response(payload,
                   sizeof(payload),
                   request_string,
                   guider_addr,
                   443,
                   iotx_ca_get()
                  );
    CM_INFO(cm_log_info_auth_rsp, payload);

    pvalue = LITE_json_value_of((char*)string_code, payload);
    if (!pvalue) {
        goto do_exit;
    }

    ret_code = atoi(pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    if (200 != ret_code) {
        CM_ERR(cm_log_error_ret_code, ret_code);
        goto do_exit;
    }

    pvalue = LITE_json_value_of((char*)string_pk, payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    if (0 != strncmp(pvalue, product_key, strlen(product_key))) {
        LITE_free(pvalue);
        goto do_exit;
    }
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of((char*)string_dn, payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    if (0 != strncmp(pvalue, device_name, strlen(device_name))) {
        LITE_free(pvalue);
        goto do_exit;
    }
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of((char*)string_ds, payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    HAL_SetDeviceSecret((char*)pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    ret = 0;

do_exit:
    if (pvalue) {
        LITE_free(pvalue);
        pvalue = NULL;
    }

    return ret;
}

static int _calc_hmac_signature(
        const char *product_key,
        const char *device_name,
        char *hmac_sigbuf,
        const int hmac_buflen,
        const char *random)
{
    char signature[64];
    char hmac_source[512];
    int ret = FAIL_RETURN;
    char product_secret[DEVICE_SECRET_LEN + 1] = {0};

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));

    HAL_GetProductSecret(product_secret);

    ret = HAL_Snprintf(hmac_source,
                      sizeof(hmac_source),
                      string_hmac_format,
                      device_name,
                      product_key,
                      random);

#if USING_SHA1_IN_HMAC
    utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    product_secret,
                    strlen(product_secret));
#else
    utils_hmac_md5(hmac_source, strlen(hmac_source),
                   signature,
                   product_secret,
                   strlen(product_secret));
#endif


    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return ret;
}


int iotx_cm_auth(const char *product_key, const char *device_name, const char *client_id)
{
    char *req_str = NULL;
    char guider_sign[40] = {0};
    char *s_random = NULL;
    s_random = genRandomString(int_random_length);
    // todo   string_TIMESTAMP -> random
    _calc_hmac_signature(product_key, device_name, guider_sign, sizeof(guider_sign), s_random);
    req_str = _set_auth_req_str(product_key, device_name, client_id, guider_sign, s_random);
    CM_INFO(cm_log_info_auth_req, req_str);

#ifdef SUPPORT_SINGAPORE_DOMAIN
    if (SUCCESS_RETURN != _get_device_secret(product_key, device_name, client_id, string_AUTH_URL_1, req_str)) {
#else /* SUPPORT_SINGAPORE_DOMAIN */
    if (SUCCESS_RETURN != _get_device_secret(product_key, device_name, client_id, string_AUTH_URL, req_str)) {
#endif /* SUPPORT_SINGAPORE_DOMAIN */
        if (req_str) LITE_free(req_str);
        if (s_random) LITE_free(s_random);
        CM_ERR(cm_log_error_auth);
        return FAIL_RETURN;
    }
    if (s_random) LITE_free(s_random);
    return SUCCESS_RETURN;
}
#endif /* SUPPORT_PRODUCT_SECRET */

#if 0
#ifdef CM_SUPPORT_TOPIC_DISPATCH
int iotx_cm_parse_payload(void* payload,
                          int payload_length,
                          iotx_cm_message_info_t* msg)
{
    char* payload_pt = (char*)payload;
    char* node = NULL;

    if (NULL == payload || NULL == msg || 0 == payload_length) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    switch (msg->message_type) {
    case IOTX_CM_MESSAGE_RAW: {
        CM_INFO(cm_log_info_raw_data);
        msg->code = 0;
        msg->parameter = CM_malloc(payload_length + 1);
        if (NULL == msg->parameter) {
            CM_ERR(cm_log_error_memory);
            LITE_free(node);
            return FAIL_RETURN;
        }
        memset(msg->parameter, 0x0, payload_length);
        memcpy(msg->parameter, payload, payload_length);
        msg->parameter_length = payload_length;
        msg->method = NULL;
        msg->id = 0;
        return SUCCESS_RETURN;
    }

    case IOTX_CM_MESSAGE_RESPONSE: {
        node = LITE_json_value_of((char*)string_id, payload_pt);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_id);
            return FAIL_RETURN;
        }

        msg->id = atoi(node);
        LITE_free(node);
        node = NULL;

        /* parse code */
        node = LITE_json_value_of((char*)string_code, payload_pt);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_code);
            return FAIL_RETURN;
        }
        msg->code = atoi(node);
        LITE_free(node);
        node = NULL;

        /* parse data */
        node = LITE_json_value_of((char*)string_data, payload_pt);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_data);
            return FAIL_RETURN;
        }
        msg->parameter = node;
        msg->parameter_length = strlen(node);
        node = NULL;

        msg->method = NULL;
        return SUCCESS_RETURN;
    }

    case IOTX_CM_MESSAGE_REQUEST: {
        node = LITE_json_value_of((char*)string_id, payload_pt);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_id);
            return FAIL_RETURN;
        }

        msg->id = atoi(node);
        LITE_free(node);
        node = NULL;

        /* parse params */
        node = LITE_json_value_of((char*)string_params, payload_pt);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_params);
            return FAIL_RETURN;
        }

        msg->parameter = node;
        msg->parameter_length = strlen(node);
        node = NULL;

        /* parse method */
        node = LITE_json_value_of((char*)string_method, payload_pt);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_method);
            return FAIL_RETURN;
        }
        msg->method = node;
        node = NULL;

        msg->code = 0;
        return SUCCESS_RETURN;
    }

    default:
        CM_ERR(cm_log_error_type);
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}
#else
int iotx_cm_parse_payload(void* _payload, int payload_length, iotx_cm_message_info_t* msg)
{
#if 0
    char* payload = (char*)_payload;
    char* node = NULL;

    if (NULL == _payload || NULL == msg || 0 == payload_length) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    node = LITE_json_value_of((char*)string_id, payload);
    if (node == NULL) {
        msg->message_type = IOTX_CM_MESSAGE_RAW;
        msg->code = 0;
        msg->parameter = CM_malloc(payload_length + 1);
        if (NULL == msg->parameter) {
            CM_ERR(cm_log_error_memory);
            LITE_free(node);
            return FAIL_RETURN;
        }
        memset(msg->parameter, 0x0, payload_length);
        memcpy(msg->parameter, _payload, payload_length);
        msg->parameter_length = payload_length;
        msg->method = NULL;
        msg->id = 0;
        return SUCCESS_RETURN;
    }

    msg->id = atoi(node);
    LITE_free(node);
    node = NULL;

    /* parse code */
    node = LITE_json_value_of((char*)string_code, payload);
    if (node == NULL) {
        msg->message_type = IOTX_CM_MESSAGE_REQUEST;

        /* parse params */
        node = LITE_json_value_of((char*)string_params, payload);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_params);
            return FAIL_RETURN;
        }

        msg->parameter = node;
        msg->parameter_length = strlen(node);
        node = NULL;

        /* parse method */
        node = LITE_json_value_of((char*)string_method, payload);
        if (node == NULL) {
            CM_ERR(cm_log_error_parse_method);
            return FAIL_RETURN;
        }
        msg->method  = node;
        node = NULL;

        msg->code = 0;
        return SUCCESS_RETURN;
    }
    msg->code = atoi(node);
    LITE_free(node);
    node = NULL;

    /* parse data */
    node = LITE_json_value_of((char*)string_data, payload);
    if (node == NULL) {
        CM_ERR(cm_log_error_parse_data);
        return FAIL_RETURN;
    }
    msg->parameter = node;
    msg->parameter_length = strlen(node);
    node = NULL;

    msg->method = NULL;
    msg->message_type = IOTX_CM_MESSAGE_RESPONSE;

    /* parse message */
    node = LITE_json_value_of((char*)string_message, payload);
    if (node) msg->message = node;
#endif
    return SUCCESS_RETURN;
}
#endif


int iotx_cm_splice_payload(void* payload, int* payload_length, int id, iotx_cm_message_info_t* msg)
{
#if 0
    if (NULL == payload || NULL == payload_length || NULL == msg) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    switch(msg->message_type) {
    case IOTX_CM_MESSAGE_RESPONSE:
        HAL_Snprintf(payload, *payload_length, string_response_format, id, msg->code, msg->parameter);
        (*payload_length) = strlen(payload);
        break;
    case IOTX_CM_MESSAGE_REQUEST:
        HAL_Snprintf(payload, *payload_length, string_request_format, id, msg->parameter, msg->method);
        (*payload_length) = strlen(payload);
        break;
    case IOTX_CM_MESSAGE_RAW:
        memcpy(payload, msg->parameter, msg->parameter_length);
        (*payload_length) = msg->parameter_length;
        break;
    default:
        break;
    }
#endif

    return SUCCESS_RETURN;
}

void iotx_cm_free_message_info(iotx_cm_message_info_t* message_info)
{
    if (message_info->URI) LITE_free(message_info->URI);
    if (message_info->payload) LITE_free(message_info->payload);
//    if (message_info->parameter) LITE_free(message_info->parameter);
//    if (message_info->method) LITE_free(message_info->method);
//    if (message_info->message) LITE_free(message_info->message);
}
#endif

void iotx_cm_response_func(void* context, iotx_cm_mapping_type_t mapping_type, iotx_cm_message_info_t* message_info)
{
    iotx_cm_send_peer_t peer;
    iotx_cm_conntext_t* cm_ctx = (iotx_cm_conntext_t*)context;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
    iotx_cm_mapping_t* mapping = NULL;
#else
    iotx_cm_event_msg_t msg_event = {0};
    iotx_cm_new_data_t new_data = {0};
#endif
    iotx_device_info_t* device_info = iotx_device_info_get();

    if (NULL == cm_ctx || NULL == message_info) {
        CM_ERR(cm_log_error_parameter);
        return;
    }

    memset(&peer, 0x0, sizeof(iotx_cm_send_peer_t));
    strncpy(peer.product_key, device_info->product_key, strlen(device_info->product_key));
    strncpy(peer.device_name, device_info->device_name, strlen(device_info->device_name));

#ifdef CM_SUPPORT_TOPIC_DISPATCH
    /* find mapping */
    mapping = iotx_cm_find_mapping(cm_ctx, mapping_type, message_info->URI, strlen(message_info->URI));

    if (NULL == mapping) {
        CM_WARNING(cm_log_warning_not_mapping);
//        iotx_cm_free_message_info(message_info);
        return;
    }

    if (NULL == mapping->func && NULL== mapping->mail_box) {
        CM_WARNING(cm_log_warning_not_func);
//        iotx_cm_free_message_info(message_info);
        return;
    }
    if (mapping->func) mapping->func(&peer, message_info, mapping->user_data);
//    iotx_cm_free_message_info(message_info);
#else
    new_data.message_info = message_info;
    new_data.peer = &peer;
    msg_event.event_id = IOTX_CM_EVENT_NEW_DATA_RECEIVED;
    msg_event.msg = &new_data;

    iotx_cm_trigger_event_callback(cm_ctx, &msg_event);

//    iotx_cm_free_message_info(message_info);
#endif
}


iotx_cm_mapping_t* iotx_cm_get_mapping_node(iotx_cm_mapping_type_t mapping_type)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return NULL;

#else /* CM_SUPPORT_TOPIC_DISPATCH */

#ifdef CM_MAPPING_USE_POOL
    int i = 0;
    iotx_cm_mapping_t* mapping_list = NULL;

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type)
        mapping_list = g_cm_register_mapping_list;
    else if(IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type)
        mapping_list = g_cm_service_mapping_list;
    else
        return NULL;

    for (i = 0; i < CM_SUPPORT_MAX_MAPPING_SIZE; i++) {
        if (mapping_list[i].is_used == 0) {
            mapping_list[i].is_used = 1;
            return &mapping_list[i];
        }
    }
    return NULL;
#else
    iotx_cm_mapping_t* node = NULL;

    node = CM_malloc(sizeof(iotx_cm_mapping_t));
    if (NULL == node) return NULL;

    memset(node, 0x0, sizeof(iotx_cm_mapping_t));
    node->mapping_type = mapping_type;
    return node;
#endif

#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}

int iotx_cm_free_mapping_node(iotx_cm_mapping_type_t mapping_type, iotx_cm_mapping_t* node)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    if (NULL == node) return FAIL_RETURN;

#ifdef CM_MAPPING_USE_POOL
    if (node->is_used == 1) {
        node->is_used = 0;
        memset(node, 0x0, sizeof(iotx_cm_mapping_t));
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
#else /* CM_MAPPING_USE_POOL */
    LITE_free(node);
    return SUCCESS_RETURN;
#endif /* CM_MAPPING_USE_POOL */

#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}


int iotx_cm_add_mapping(iotx_cm_conntext_t* cm_ctx, iotx_cm_mapping_type_t mapping_type, char* URI, iotx_cm_register_fp_t func, void* user_data, void* mail_box)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t* mapping = NULL;

    if (NULL == cm_ctx || NULL == URI) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    mapping = iotx_cm_get_mapping_node(mapping_type);
    if (NULL == mapping) return FAIL_RETURN;

    mapping->URI = CM_malloc(strlen(URI) + 1);
    if (NULL == mapping->URI) {
        CM_ERR(cm_log_error_memory);
        iotx_cm_free_mapping_node(mapping_type, mapping);
        return FAIL_RETURN;
    }
    memset(mapping->URI, 0x0, strlen(URI) + 1);
    strncpy(mapping->URI, URI, strlen(URI));
    mapping->func = func;
    mapping->user_data = user_data;
    mapping->mail_box = mail_box;

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type) {
        mapping->next = cm_ctx->register_mapping_list;
        cm_ctx->register_mapping_list = mapping;
    } else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type) {
        mapping->next = cm_ctx->service_mapping_list;
        cm_ctx->service_mapping_list = mapping;
    } else {
        LITE_free(mapping->URI);
        iotx_cm_free_mapping_node(mapping_type, mapping);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}


/* remove */
int iotx_cm_remove_mapping(iotx_cm_conntext_t* cm_ctx, iotx_cm_mapping_type_t mapping_type, char* URI)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t* mapping = NULL;
    iotx_cm_mapping_t* pre_mapping = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type)
        pre_mapping = mapping = cm_ctx->register_mapping_list;
    else if(IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type)
        pre_mapping = mapping = cm_ctx->service_mapping_list;
    else
        return FAIL_RETURN;

    if (NULL == mapping) return FAIL_RETURN;

    if (0 == strncmp(URI, mapping->URI, strlen(URI))) {
        if (NULL == mapping->next) {
            if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type)
                cm_ctx->register_mapping_list = NULL;
            else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type)
                cm_ctx->service_mapping_list = NULL;
            else
                return FAIL_RETURN;
        } else {
            if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type)
                cm_ctx->register_mapping_list = mapping->next;
            else if (IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type)
                cm_ctx->service_mapping_list = mapping->next;
            else
                return FAIL_RETURN;
        }
    }

    while (mapping) {
        if (0 == strncmp(URI, mapping->URI, strlen(URI))) {
            CM_INFO(cm_log_info_remove_mapping);
            pre_mapping->next = mapping->next;

            LITE_free(mapping->URI);
            iotx_cm_free_mapping_node(mapping_type, mapping);
            return SUCCESS_RETURN;
        }

        pre_mapping = mapping;
        mapping = mapping->next;
    }

    return FAIL_RETURN;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}


/* remove all */
int iotx_cm_remove_mapping_all(iotx_cm_conntext_t* cm_ctx)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return SUCCESS_RETURN;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t* mapping = NULL;
    iotx_cm_mapping_t* next_mapping = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    next_mapping = mapping = cm_ctx->register_mapping_list;

    while (mapping) {
        next_mapping = mapping->next;

        LITE_free(mapping->URI);
        iotx_cm_free_mapping_node(IOTX_CM_MAPPING_TYPE_REGISTER, mapping);

        mapping = next_mapping;
    }
    cm_ctx->register_mapping_list = NULL;

    next_mapping = mapping = cm_ctx->service_mapping_list;

    while (mapping) {
        next_mapping = mapping->next;

        LITE_free(mapping->URI);
        iotx_cm_free_mapping_node(IOTX_CM_MAPPING_TYPE_SERVICE, mapping);

        mapping = next_mapping;
    }

    cm_ctx->service_mapping_list = NULL;

    return FAIL_RETURN;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}

iotx_cm_mapping_t* iotx_cm_find_mapping(iotx_cm_conntext_t* cm_ctx, iotx_cm_mapping_type_t mapping_type, char* URI, int URI_length)
{
#ifndef CM_SUPPORT_TOPIC_DISPATCH
    return NULL;
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    iotx_cm_mapping_t* mapping = NULL;

    if (NULL == cm_ctx) {
        CM_ERR(cm_log_error_parameter);
        return NULL;
    }

    if (IOTX_CM_MAPPING_TYPE_REGISTER == mapping_type)
        mapping = cm_ctx->register_mapping_list;
    else if(IOTX_CM_MAPPING_TYPE_SERVICE == mapping_type)
        mapping = cm_ctx->service_mapping_list;
    else
        return NULL;


    while (mapping) {
        if ((0 == strncmp(URI, mapping->URI, URI_length)) || (0 != strstr(URI, mapping->URI))) {
            return mapping;
        }

        mapping = mapping->next;
    }

    return NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */
}

static void cm_connectivity_iterator_action_handler(void* list_node, va_list* params)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)list_node;
    iotx_cm_conntext_t* cm_ctx;
    char* uri;
    const char* pk;
    const char* dn;
    iotx_cm_message_auth_types_t auth_type;
    cm_iterator_action_t action;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t*);
    action = va_arg(*params, int);

    assert(cm_ctx && action < cm_iterator_action_max);

    if (connectivity && IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)) {
        switch (action) {
        case cm_iterator_action_register:{
            iotx_cm_register_param_t* register_param = va_arg(*params, iotx_cm_register_param_t*);
            int count = va_arg(*params, int);
            connectivity->register_func(cm_ctx, connectivity, register_param, count);
        }
            break;

        case cm_iterator_action_unregister:
            uri = va_arg(*params, char*);
            connectivity->unregister_func(cm_ctx, connectivity, uri);
            break;

        case cm_iterator_action_add_service:
            uri = va_arg(*params, char*);
            auth_type = va_arg(*params, int);
            connectivity->add_service_func(cm_ctx, connectivity, uri, auth_type);
            break;

        case cm_iterator_action_remove_service:
            uri = va_arg(*params, char*);
            connectivity->remove_service_func(cm_ctx, connectivity, uri);
            break;

        case cm_iterator_action_add_subdevice:
            pk = va_arg(*params, char*);
            dn = va_arg(*params, char*);
            connectivity->add_subdevice_func(cm_ctx, connectivity, pk, dn);
            break;

        case cm_iterator_action_remove_subdevice:
            pk = va_arg(*params, char*);
            dn = va_arg(*params, char*);
            connectivity->remove_subdevice_func(cm_ctx, connectivity, pk, dn);
            break;

        default:
            break;
        }
    }
}


int iotx_cm_register(iotx_cm_conntext_t* cm_ctx, void* _connectivity, iotx_cm_register_param_t* register_param, int count)
{
    linked_list_t* list = NULL;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;

    assert(cm_ctx && register_param);
    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;
        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_register, register_param, count);
    } else {
        return connectivity->register_func(cm_ctx, connectivity, register_param, count);
    }

    return SUCCESS_RETURN;
}


int iotx_cm_unregister(iotx_cm_conntext_t* cm_ctx, void* _connectivity, char* URI)
{
    linked_list_t* list = NULL;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;

    assert(cm_ctx && URI);

    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;

        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_unregister, URI);
    } else {
        connectivity->unregister_func(cm_ctx, connectivity, URI);
    }

    if (FAIL_RETURN == iotx_cm_remove_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_REGISTER, URI)) {
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int iotx_cm_add_service(iotx_cm_conntext_t* cm_ctx, void* _connectivity, char* _URI, iotx_cm_message_auth_types_t auth_type,
                        iotx_cm_register_fp_t register_func, void* user_data, void* mail_box)
{
    linked_list_t* list = NULL;
    char* URI = _URI;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;
#ifdef CM_SUPPORT_TOPIC_DISPATCH
    iotx_cm_mapping_t* mapping = NULL;
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

    assert(cm_ctx && URI);

#ifdef CM_SUPPORT_TOPIC_DISPATCH
    if (FAIL_RETURN == iotx_cm_add_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI, register_func, user_data, mail_box)) {
        iotx_cm_event_msg_t event;
        iotx_cm_event_result_t result_pt = {0};

        result_pt.result = -1;
        result_pt.URI = URI;
        event.event_id = IOTX_CM_EVENT_ADD_SERVICE_RESULT;
        event.msg = (void*)&result_pt;

        CM_ERR(cm_log_error_fail_1);

        iotx_cm_trigger_event_callback(cm_ctx, &event);

        return FAIL_RETURN;
    }

    mapping = iotx_cm_find_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI, strlen(URI));

    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;
        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_add_service, mapping->URI, auth_type);
    } else {
        return connectivity->add_service_func(cm_ctx, connectivity, mapping->URI, auth_type);
    }
#else /* CM_SUPPORT_TOPIC_DISPATCH */
    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;
        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_add_service, URI, auth_type);
    } else {
        return connectivity->add_service_func(cm_ctx, connectivity, URI, auth_type);
    }
#endif /* CM_SUPPORT_TOPIC_DISPATCH */

    return SUCCESS_RETURN;
}


int iotx_cm_remove_service(iotx_cm_conntext_t* cm_ctx, void* _connectivity, char* URI)
{
    linked_list_t* list;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;

    assert(cm_ctx && URI);

    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;
        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_remove_service, URI);
    } else {
        connectivity->remove_service_func(cm_ctx, connectivity, URI);
    }

    if (FAIL_RETURN == iotx_cm_remove_mapping(cm_ctx, IOTX_CM_MAPPING_TYPE_SERVICE, URI)) {
        CM_ERR(cm_log_error_fail);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


int iotx_cm_add_subdevice(iotx_cm_conntext_t* cm_ctx, void* _connectivity, const char* pk, const char* dn)
{
    linked_list_t* list = NULL;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;

    assert(cm_ctx && pk && dn);

    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;
        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_add_subdevice, pk, dn);
    } else {
        connectivity->add_subdevice_func(cm_ctx, connectivity, pk, dn);
    }

    return SUCCESS_RETURN;
}


int iotx_cm_remove_subdevice(iotx_cm_conntext_t* cm_ctx, void* _connectivity, const char* pk, const char* dn)
{
    linked_list_t* list = NULL;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;

    assert(cm_ctx && pk && dn);

    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;
        linked_list_iterator(list, cm_connectivity_iterator_action_handler, cm_ctx, cm_iterator_action_remove_subdevice, pk, dn);
    } else {
        connectivity->remove_subdevice_func(cm_ctx, connectivity, pk, dn);
    }

    return SUCCESS_RETURN;
}


static void cm_connectivity_iterator_send_data_handler(void* list_node, va_list* params)
{
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)list_node;
    iotx_cm_connectivity_t* target_connectivity;
    iotx_cm_send_peer_t* target;
    iotx_cm_conntext_t* cm_ctx;
    iotx_cm_message_ack_types_t ack_type;
    char* uri;
    void* payload;
    int payload_length;
    void* context;
    int* ret;

    cm_ctx = va_arg(*params, iotx_cm_conntext_t*);
    target_connectivity = va_arg(*params, void*);
    target = va_arg(*params, void*);
    uri = va_arg(*params, char*);
    ack_type = va_arg(*params, int);
    payload = va_arg(*params, void*);
    payload_length = va_arg(*params, int);
    context = va_arg(*params, void*);
    ret = va_arg(*params, int*);

    assert(cm_ctx && uri && payload && payload_length && ret);

    if ((target_connectivity && connectivity == target_connectivity && (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity))) ||
        (!target_connectivity && connectivity && (IOTX_CM_CONNECTIVITY_STATUS_CONNECTED == iotx_cm_get_connectivity_status(connectivity)))) {
        if (FAIL_RETURN == connectivity->send_func(cm_ctx, connectivity, target, uri, ack_type, payload, payload_length, context, NULL)) {
            *ret = FAIL_RETURN;
        }
    }
}

int iotx_cm_send_data(iotx_cm_conntext_t* cm_ctx, void* _connectivity, iotx_cm_send_peer_t* target, char* URI,
                      iotx_cm_message_ack_types_t ack_type, void* payload, int payload_length, void* context, void* sem)
{
    linked_list_t* list;
    int ret = 0;
    iotx_cm_connectivity_t* connectivity = (iotx_cm_connectivity_t*)_connectivity;

    assert(cm_ctx && URI && payload && payload_length);

    if (NULL == connectivity) {
        list = cm_ctx->list_connectivity;

        if (list->_size == 0) ret = FAIL_RETURN;
        else
            linked_list_iterator(list, cm_connectivity_iterator_send_data_handler, cm_ctx, connectivity,
                         target, URI, ack_type, payload, payload_length, context, &ret);
    } else {
        ret = connectivity->send_func(cm_ctx, connectivity, target, URI, ack_type, payload, payload_length, context, sem);
    }

    if (SUCCESS_RETURN != ret) {
        iotx_cm_event_msg_t event_msg;
        iotx_cm_event_result_t event_result = {0};

        event_result.result = -1;
        event_result.URI = URI;
        event_msg.event_id = IOTX_CM_EVENT_SEND_RESULT;
        event_msg.msg = &event_result;

        CM_ERR(cm_log_error_fail_rc, ret);

        /* todo: send fail is not a event */
        iotx_cm_trigger_event_callback(cm_ctx, &event_msg);
    }
    return ret;
}

static void invoke_event_callback_func(void* _cb_usr_ctx, va_list* params)
{
    iotx_cm_event_cb_usr_ctx_t* cb_usr_ctx = _cb_usr_ctx;
    iotx_cm_conntext_t* cm_ctx;
    iotx_cm_event_msg_t* msg;

    cm_ctx = va_arg(*params, void*);
    msg = va_arg(*params, void*);

    assert(cm_ctx && cb_usr_ctx && msg);

    if (cb_usr_ctx && cm_ctx &&  cb_usr_ctx->event_func) {
        cb_usr_ctx->event_func(cm_ctx, msg, cb_usr_ctx->user_data);
    }
}

static void callback_list_iterator(void* _cm_ctx, linked_list_handle_fp_t handle_fp, void* msg)
{
    iotx_cm_conntext_t* cm_ctx = _cm_ctx;
    linked_list_t* list = cm_ctx->list_event_callback;

    linked_list_iterator(list, handle_fp, cm_ctx, msg);
}

int iotx_cm_trigger_event_callback(iotx_cm_conntext_t* _cm_ctx, iotx_cm_event_msg_t* msg)
{
    iotx_cm_conntext_t* cm_ctx = _cm_ctx;
    linked_list_t* list;

    /* invoke callback funtions. */
    list = cm_ctx->list_event_callback;

    if (list && !linked_list_empty(list)) {
        callback_list_iterator(cm_ctx, invoke_event_callback_func, msg);
    }

    return SUCCESS_RETURN;
}



