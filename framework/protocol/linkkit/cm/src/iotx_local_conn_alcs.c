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


#ifdef CM_SUPPORT_LOCAL_CONN

#include <string.h>

#include "iot_import.h"
#include "lite-log.h"
#include "lite-utils.h"
#include "lite-system.h"
#include "iot_export.h"
#include "iot_export_cm.h"
#include "iot_export_alcs.h"

#include "iotx_cm_common.h"
#include "iotx_cm_connection.h"
#include "iotx_local_conn_alcs.h"

static const int _alcs_port = 5683;
static const int _alcs_send_maxcount = 8;
static const int _alcs_waittime = 2000;
static const int _alcs_obs_maxcount = 8;

static const char string_group[] CM_READ_ONLY = "224.0.1.187";

void _alcs_event_handler(void *pcontext, void *phandle, iotx_alcs_event_msg_t *msg)
{
    iotx_connection_t* connection = (iotx_connection_t*)pcontext;

    CM_INFO(cm_log_info_event_id, msg->event_type);

    switch(msg->event_type) {
    case IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_SUCCESS:
    {
        iotx_alcs_transfer_msg_t* transfer_msg = (iotx_alcs_transfer_msg_t*)msg->msg;
        iotx_connection_msg_rsp_t msg_rsp;

        if (NULL == transfer_msg->uri || NULL == transfer_msg->payload) return;

        log_info("message response arrived: uri=%s, payload=%.*s",
                 transfer_msg->uri,
                 transfer_msg->payload_len,
                 transfer_msg->payload);

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_NEW_DATA;
        msg_rsp.URI = transfer_msg->uri;
        msg_rsp.URI_length = strlen(transfer_msg->uri);
        msg_rsp.payload = transfer_msg->payload;
        msg_rsp.payload_length = transfer_msg->payload_len;
        msg_rsp.conn_ctx = NULL;

        if (connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);
    }
        break;

    case IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_RESP_TIMEOUT:
    {
        iotx_alcs_transfer_msg_t* transfer_msg = (iotx_alcs_transfer_msg_t*)msg->msg;
        iotx_connection_msg_rsp_t msg_rsp;

        if (NULL == transfer_msg->uri) return;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_SEND_FAIL;
        msg_rsp.URI = transfer_msg->uri;
        msg_rsp.URI_length = msg_rsp.URI ? strlen(transfer_msg->uri) : 0;
        msg_rsp.payload = NULL;
        msg_rsp.payload_length = 0;
        msg_rsp.conn_ctx = NULL;

        if (msg_rsp.URI && connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);
    }
        break;

    case IOTX_ALCS_EVENT_MSG_RECV_MESSAGE:
    {
        iotx_alcs_transfer_msg_t* transfer_msg = (iotx_alcs_transfer_msg_t*)msg->msg;
        iotx_connection_msg_rsp_t msg_rsp;
        iotx_alcs_transfer_msg_context_t* context = NULL;

        if (NULL == transfer_msg->ip || NULL == transfer_msg->token || NULL == transfer_msg->uri || NULL == transfer_msg->payload) return;

        log_info("message response arrived: token=%.*s, uri=%s, topic_msg=%.*s",
                 transfer_msg->token_len,
                 transfer_msg->token,
                 transfer_msg->uri,
                 transfer_msg->payload_len,
                 transfer_msg->payload);

        context = CM_malloc(sizeof(iotx_alcs_transfer_msg_context_t));
        memset(context, 0x0, sizeof(iotx_alcs_transfer_msg_context_t));
        context->ip = CM_malloc(strlen(transfer_msg->ip) + 1);
        memset(context->ip, 0x0, strlen(transfer_msg->ip) + 1);
        strncpy((char*)context->ip, (const char*)transfer_msg->ip, strlen(transfer_msg->ip));
        context->port = transfer_msg->port;
        context->token = CM_malloc(transfer_msg->token_len + 1);
        memset(context->token, 0x0, transfer_msg->token_len + 1);
        memcpy((char*)context->token, (const char*)transfer_msg->token, transfer_msg->token_len);
        context->token_len = transfer_msg->token_len;

        memset(&msg_rsp, 0x0, sizeof(iotx_connection_msg_rsp_t));
        msg_rsp.rsp_type = IOTX_CONNECTION_RESPONSE_NEW_DATA;
        msg_rsp.URI = transfer_msg->uri;
        msg_rsp.URI_length = strlen(transfer_msg->uri);
        msg_rsp.payload = transfer_msg->payload;
        msg_rsp.payload_length = transfer_msg->payload_len;
        msg_rsp.conn_ctx = context;

        if (connection->response_handler)
            connection->response_handler(connection->event_pcontext, &msg_rsp);
    }
        break;

    default:
        break;
    }
}

/*  alcs */
void* iotx_local_conn_alcs_init(void* handle)
{
    iotx_alcs_param_t alcs_param;
    iotx_alcs_event_handle_t event_handle;
    void *pclient;

    memset(&alcs_param, 0x0, sizeof(iotx_alcs_param_t));
    memset(&event_handle, 0x0, sizeof(iotx_alcs_event_handle_t));

    alcs_param.group = (char*)string_group;
    alcs_param.port = _alcs_port;
    alcs_param.send_maxcount = _alcs_send_maxcount;
    alcs_param.waittime = _alcs_waittime;
    alcs_param.obs_maxcount = _alcs_obs_maxcount;
    alcs_param.role = IOTX_ALCS_ROLE_CLIENT | IOTX_ALCS_ROLE_SERVER;
    event_handle.h_fp = _alcs_event_handler;
    event_handle.pcontext = handle;

    alcs_param.handle_event = &event_handle;

    if (NULL == (pclient = IOT_ALCS_Construct(&alcs_param))) {
        CM_ERR(cm_log_error_fail);
        return NULL;
    }

    return pclient;
}

int iotx_local_conn_alcs_add_service(void* handle,
                                     const char *topic_filter,
                                     iotx_cm_message_ack_types_t ack_type,
                                     iotx_cm_message_auth_types_t auth_type)
{
    iotx_alcs_res_t alcs_res = {0};
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    alcs_res.uri = (char*)topic_filter;
    alcs_res.msg_ct = IOTX_ALCS_MESSAGE_CT_APP_JSON;
    alcs_res.msg_perm = IOTX_ALCS_MESSAGE_PERM_GET;
    alcs_res.maxage = 60;
    alcs_res.need_auth = auth_type;
    return IOT_ALCS_Register_Resource(connection->context, &alcs_res);
}


int iotx_local_conn_alcs_remove_service(void* handle,
                                        const char *topic_filter)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    return IOT_ALCS_Unregister_Resource(connection->context, (char*)topic_filter);
}


int iotx_local_conn_alcs_send(void* handle,
                              void* _context,
                              iotx_connection_msg_t* message)
{
    int ret = FAIL_RETURN;
    iotx_connection_t* connection = (iotx_connection_t*)handle;
    iotx_alcs_transfer_msg_context_t* context = (iotx_alcs_transfer_msg_context_t*)_context;
    iotx_alcs_msg_t alcs_msg;

    if (NULL == message) {
        CM_ERR(cm_log_error_parameter);
        return FAIL_RETURN;
    }

    memset(&alcs_msg, 0x0, sizeof(iotx_alcs_msg_t));

    alcs_msg.group_id = 0;  // TODO

    alcs_msg.ip = context ? context->ip : NULL;
    alcs_msg.port = context ? context->port : 0;
    alcs_msg.msg_code = (context && context->token_len && context->token) ? ITOX_ALCS_COAP_MSG_CODE_205_CONTENT : ITOX_ALCS_COAP_MSG_CODE_GET;
    alcs_msg.msg_type = IOTX_ALCS_MESSAGE_TYPE_CON;
    alcs_msg.uri = message->URI;
    alcs_msg.payload = message->payload;
    alcs_msg.payload_len = message->payload_length;

    // there is no target call IOT_ALCS_Observe_Notify
    // there is target call IOT_ALCS_Send
    // there is context call IOT_ALCS_Send_Response
    if (NULL == context) {
        ret = IOT_ALCS_Observe_Notify(connection->context, alcs_msg.uri, alcs_msg.payload_len, alcs_msg.payload);
    } else if (context->ip && context->port && NULL == context->token) {
        ret = IOT_ALCS_Send(connection->context, &alcs_msg);
    } else if (context->ip && context->port && context->token_len && context->token) {
        ret = IOT_ALCS_Send_Response(connection->context, &alcs_msg, (uint8_t)context->token_len, (uint8_t*)context->token);
    }

    if (context && context->ip) LITE_free(context->ip);
    if (context && context->token) LITE_free(context->token);
    if (context)  LITE_free(context);

    return ret;
}


int iotx_local_conn_alcs_deinit(void* handle)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    return IOT_ALCS_Destroy(&connection->context);
}


int iotx_local_conn_alcs_yield(void* handle, int timeout_ms)
{
    iotx_connection_t* connection = (iotx_connection_t*)handle;

    return IOT_ALCS_Yield(connection->context);
}

#endif /* CM_SUPPORT_LOCAL_CONN */
