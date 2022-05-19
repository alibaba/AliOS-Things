/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * File: lightduer_connagent.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Light duer APIS.
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_H

#include "lightduer_types.h"
#include "lightduer_network_defs.h"
#include "lightduer_coap_defs.h"
#include "baidu_json.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_event_id_enum {
    DUER_EVENT_STARTED,
    DUER_EVENT_STOPPED
} duer_event_id;

typedef struct _duer_event_data_s {
    int     _event;     //< Event id, see in @{link duer_event_id}
    int     _code;    //< the return code
    void *  _object;    //< The return content
} duer_event_t;

/**
 * The action result callback
 */
typedef void (*duer_event_callback)(duer_event_t *);

/**
 * Initialize the environment.
 */
void duer_initialize(void);

/**
 * Set all actions event callback
 *
 * @param callback, the event change result callback.
 */
void duer_set_event_callback(duer_event_callback callback);

/**
 * Start the LightDuer egnine.
 *
 * @param data, the configuration data
 * @param size, the data size
 * @return int, the start result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_start(const void *data, size_t size);

/**
 * Add route resources for Server requested, this should be called after CA started.
 *
 * @param msg, const duer_res_t *, the resources list
 * @param length, size_t, the resources length
 * @return int, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_add_resources(const duer_res_t *res, size_t length);

/**
 * Call it when data is ready.
 *
 * @return int, the start result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_data_available(void);

/**
 * Send data to server.
 *
 * @param data, const baidu_json *, the data point values.
 * @return int, the report data result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_data_report(const baidu_json *data);

/**
 * Send data to server.
 *
 * @param data, const baidu_json *, the data point values.
 * @return int, the report data result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_data_report_async(duer_context_t *context, const baidu_json *data);

/**
 * Response the request from Origin Server.
 *
 * @param msg, const duer_msg_t *, the request message structure.
 * @param msg_code, int, the message code, see in @{link duer_msg_code_e}.
 * @param data, const void *, the payload data.
 * @param size, size_t, the payload size.
 * @return int, the send response result, success return DUER_OK, failed return DUER_ERR_FAILED.
 * @Note: if response ACK without response data,
 *        set msg_code = DUER_MSG_EMPTY_MESSAGE, and data = NULL, size = 0
 */
int duer_response(const duer_msg_t *msg, int msg_code, const void *data, duer_size_t size);

/**
 * the seperate response message for an request
 * @param token, const char* , the token got from the corresponding request
 * @param token_len, duer_size_t, the length of the token
 * @param msg_code, int, the message code, see in @{link duer_msg_code_e}
 * @param data, const baidu_json *, the payload of the message
 * @return int, the send response result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_seperate_response(const char *token,
                           duer_size_t token_len,
                           int msg_code,
                           const baidu_json *data);

/**
 * Stop the LightDuer engine.
 *
 * @return int, the stop result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_stop(void);

/**
 * Stop the LightDuer engine.
 * @param reason, int, the stop code which indicate the stop reason
 * @return int, the stop result, success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_stop_with_reason(int reason);

/**
 * clear the queue cache data.
 * @param timestamp, clean the message with create_timestamp before timestamp
 */
int duer_clear_data(duer_u32_t timestamp);

/**
 * the callback to handle coap response message.
 */
void duer_coap_response_callback(duer_context ctx,
                                 duer_msg_t* msg,
                                 duer_addr_t* addr);

/**
 * Finalize the environment.
 */
int duer_finalize(void);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_H*/
