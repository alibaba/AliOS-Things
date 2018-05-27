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


#ifdef CM_VIA_CLOUD_CONN

#ifndef __CM_VIA_CLOUD_CONN_H__
#define __CM_VIA_CLOUD_CONN_H__

#include "iot_import.h"

#include "lite-utils.h"
#include "lite-system.h"
#include "iotx_cm_common.h"

int iotx_cm_cloud_conn_disconnect_handler(iotx_cm_conntext_t* cm_ctx);
int iotx_cm_cloud_conn_reconnect_handler(iotx_cm_conntext_t* cm_ctx);
int iotx_cm_cloud_conn_register_handler(iotx_cm_conntext_t* cm_ctx, char* URI, int result, int is_register);
int iotx_cm_cloud_conn_response_handler(iotx_cm_conntext_t* cm_ctx, iotx_cm_message_info_t* message_info);

void* iotx_cm_cloud_conn_init(void* handler, iotx_cm_init_param_t* pparam);
int iotx_cm_cloud_conn_connect(void* handler, void* _connectivity);
int iotx_cm_cloud_conn_trigger_connected(void* handler, void* _connectivity, iotx_cm_event_handle_fp_t event_fp, void *user_data);
int iotx_cm_cloud_conn_register(void* handler, void* _connectivity, const char* topic_filter);
int iotx_cm_cloud_conn_unregister(void* handler, void* _connectivity, const char* topic_filter);
int iotx_cm_cloud_conn_add_service(void* handler, void* _connectivity, const char* topic_filter, iotx_cm_message_auth_types_t auth_type);
int iotx_cm_cloud_conn_remove_service(void* handler, void* connectivity, const char* topic_filter);
int iotx_cm_cloud_conn_send(void* handler, void* connectivity, iotx_cm_send_peer_t* target, const char* topic_filter,
                            iotx_cm_message_ack_types_t ack_type, const void* payload, int payload_length, void* context);
int iotx_cm_cloud_conn_send_sync(void* handler, void* connectivity, iotx_cm_send_peer_t* target, const char* topic_filter,
                                 iotx_cm_message_ack_types_t ack_type, const void* payload, int payload_length, void* context);
int iotx_cm_cloud_conn_add_subdevice(void* handler, void* connectivity, const char* pk, const char* dn);
int iotx_cm_cloud_conn_remove_subdevice(void* handler, void* connectivity, const char* pk, const char* dn);
int iotx_cm_cloud_conn_yield(void* connectivity, int timeout_ms);
int iotx_cm_cloud_conn_deinit(void* connectivity);

iotx_cm_send_peer_t* iotx_cm_cloud_conn_get_target();

#ifdef CM_SUPPORT_MULTI_THREAD
int iotx_cm_cloud_conn_add_send(void* handler,
                                iotx_cm_send_peer_t* target,
                                iotx_cm_message_info_t* message_info);
void* iotx_cm_cloud_conn_process(void *pclient);
#endif /* CM_SUPPORT_MULTI_THREAD */

#endif /* __CM_VIA_CLOUD_CONN_H__ */
#endif /* CM_VIA_CLOUD_CONN */


