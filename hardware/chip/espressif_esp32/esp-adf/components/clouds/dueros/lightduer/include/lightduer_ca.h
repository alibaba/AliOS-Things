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
// Author: Su Hao (suhao@baidu.com)
//
// Desc: Provide the API for external applications.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_INCLUDE_BAIDU_CA_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_INCLUDE_BAIDU_CA_H

#include "lightduer_types.h"
#include "lightduer_coap.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Acquire the handler
 *
 * @Param soc_ctx, in socket context
 * @Return duer_handler, the global context handler
 */
DUER_EXT duer_handler baidu_ca_acquire(duer_transevt_func soc_ctx);

/*
 * Acquire the resources for response
 *
 * @Param hdlr, in, the handler will be operated
 * @Param list_res, in, resource list
 * @Param list_res_size, in resource list length
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_add_resources(duer_handler hdlr,
                                              const duer_res_t list_res[],
                                              duer_size_t list_res_size);
/*
 * Load the configuration infomation
 *
 * @Param hdlr, in, the handler will be operated
 * @Param data, in, the configuration data
 * @Param size, in, the data size
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_load_configuration(duer_handler hdlr, const void *data, duer_size_t size);

DUER_EXT duer_status_t baidu_ca_unload_configuration(duer_handler hdlr);

/*
 * Start run the Baidu CA, prepare the environment.
 *
 * @Param hdlr, in, the handler will be operated
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_start(duer_handler hdlr);

/*
 *
 * Determine the CA is started.
 *
 * @Param hdlr, in, the handler will be operated
 * @Return duer_bool, DUER_TRUE if started, else return DUER_FALSE.
 */
DUER_EXT duer_bool baidu_ca_is_started(duer_handler hdlr);

/*
 *
 * Determine the CA is stopped.
 *
 * @Param hdlr, in, the handler will be operated
 * @Return duer_bool, DUER_TRUE if stopped, else return DUER_FALSE.
 */
DUER_EXT duer_bool baidu_ca_is_stopped(duer_handler hdlr);

/*
 * Set the Reporter report response callback.
 *
 * @Param hdlr, in, the handler will be operated
 * @Param f_response, in, the callback for notify user the report data response
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_report_set_response_callback(duer_handler hdlr,
                                                             duer_notify_f f_response,
                                                             duer_context context);

/*
 * Set the transmit data callback.
 *
 * @Param hdlr, in, the handler will be operated
 * @Param f_transmit, in, the callback for transmit encoded data.
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_report_set_data_tx_callback(duer_handler hdlr, duer_transmit_f f_transmit);

/*
 * Build the message body that will be reported.
 *
 * @Param hdlr, in, the handler will be operated
 * @Param data, in, the message report data
 * @Param size, in, the data size
 * @Param confirmable, in, the report data QoS
 * @Return duer_msg_t *, in, the generated message body,
 *         it SHOULD be released by ${link baidu_ca_release_message}
 */
DUER_EXT duer_msg_t* baidu_ca_build_report_message(duer_handler hdlr,
                                                   duer_bool confirmable);

/*
 * Build the message body that will be responsed to remote.
 *
 * @Param hdlr, in, the handler will be operated
 * @Param msg, in, the message that remote requested
 * @Param msg_code, out, the response message code
 * @Return duer_msg_t *, in, the generated message body,
 *         it SHOULD be released by ${link baidu_ca_release_message}
 */
DUER_EXT duer_msg_t* baidu_ca_build_response_message(duer_handler hdlr,
                                                     const duer_msg_t* msg,
                                                     duer_u8_t msg_code);

/*
 * Build the seperate response to the request with the token.
 *
 * @Param hdlr, in, the handler will be operated
 * @Param ptoken, in, the token used in the message
 * @Param token_len, in, the length of the token
 * @Param msg_code, in, the response message code
 * @Param confirmable, in,
 * @Return duer_msg_t *, out, the generated message body,
 *         it SHOULD be released by ${link baidu_ca_release_message}
 */
DUER_EXT duer_msg_t* baidu_ca_build_seperate_response_message(duer_handler hdlr,
                                                              const char *ptoken,
                                                              duer_size_t token_len,
                                                              int msg_code,
                                                              duer_bool confirmable);

/*
 * Release the message that generated by baidu_ca_build_XXXX_message.
 *
 * @Param hdlr, in, the handler will be operated
 * @Param msg, in, the message that remote requested
 */
DUER_EXT void baidu_ca_release_message(duer_handler hdlr, duer_msg_t* msg);

/*
 * Send the message
 *
 * @Param hdlr, in, the handler will be operated
 * @Param msg, in, the msg will be sent
 * @Param addr, in, the remote addr
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_send_data(duer_handler hdlr,
                                          const duer_msg_t* msg,
                                          const duer_addr_t* addr);

/*
 * Send the user data
 *
 * @Param hdlr, in, the handler will be operated
 * @Param data, in, the user data
 * @Param size, in, the data size
 * @Param addr, in, the remote addr
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_send_data_directly(duer_handler hdlr,
                                          const void *data,
                                          duer_size_t szie,
                                          const duer_addr_t* addr);

/*
 * When the message data has ready to be received
 *
 * @Param hdlr, in, the handler will be operated
 * @Param addr, in, the remote addr
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_data_available(duer_handler hdlr,
                                               const duer_addr_t* addr);

/*
 * Execute the cached CoAP data, such as blockwise, resending...
 *
 * @Param hdlr, in, the handler will be operated
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_exec(duer_handler hdlr);

/*
 * Stop the Baidu CA.
 *
 * @Param hdlr, in, the handler will be operated
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_stop(duer_handler hdlr);

/*
 * Release the handler
 *
 * @Param hdlr, in, the handler will be operated
 * @Return duer_status_t, in, the operation result
 */
DUER_EXT duer_status_t baidu_ca_release(duer_handler hdlr);

/*
 * Obtain the uuid
 *
 * @Param hdlr, in, the handler will be operated
 * @Return const char *, The UUID string
 */
DUER_EXT const char *baidu_ca_get_uuid(duer_handler hdlr);

/*
 * Obtain the rsa cacrt
 *
 * @Param hdlr, in, the handler will be operated
 * @Return const char *, The RSA CACRT string
 */
DUER_EXT const char *baidu_ca_get_rsa_cacrt(duer_handler hdlr);

/*
 * Obtain the bindToken
 *
 * @Param hdlr, in, the handler will be operated
 * @Return const char *, The bindToken string
 */
DUER_EXT const char *baidu_ca_get_bind_token(duer_handler hdlr);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_INCLUDE_BAIDU_CA_H
