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

#include "iotx_cm_common.h"


/* text for log */
/* error */
const char cm_log_error_parameter[] CM_READ_ONLY = "parameters error!";
const char cm_log_error_param_1[] CM_READ_ONLY = "param error: lack of product_key or device_name or device_secret";
const char cm_log_error_auth[] CM_READ_ONLY = "auth error";
const char cm_log_error_memory[] CM_READ_ONLY = "memory error";
const char cm_log_error_fail[] CM_READ_ONLY = "failed";
const char cm_log_error_fail_ota[] CM_READ_ONLY = "ota failed";
const char cm_log_error_fail_1[] CM_READ_ONLY = "register fail: add mapping fail.";
const char cm_log_error_fail_rc[] CM_READ_ONLY = "fail: rc = %d.";
const char cm_log_error_status[] CM_READ_ONLY = "status error!";
const char cm_log_error_splice_URI[] CM_READ_ONLY = "splice URI fail!";
const char cm_log_error_parse_URI[] CM_READ_ONLY = "parse URI fail!";
const char cm_log_error_splice_payload[] CM_READ_ONLY = "splice payload fail!";
const char cm_log_error_parse_payload[] CM_READ_ONLY = "parse payload fail!";
const char cm_log_error_get_node[] CM_READ_ONLY = "get list node fail!";
const char cm_log_error_push_node[] CM_READ_ONLY = "push list node fail!";
const char cm_log_error_process[] CM_READ_ONLY = "too many process";
const char cm_log_error_parse_id[] CM_READ_ONLY = "parse id error";
const char cm_log_error_parse_code[] CM_READ_ONLY = "parse code error";
const char cm_log_error_parse_data[] CM_READ_ONLY = "parse data error";
const char cm_log_error_parse_params[] CM_READ_ONLY = "parse params error";
const char cm_log_error_parse_method[] CM_READ_ONLY = "parse method error";
const char cm_log_error_type[] CM_READ_ONLY = "type error";
const char cm_log_error_pk[] CM_READ_ONLY = "product_key is empty";
const char cm_log_error_dn[] CM_READ_ONLY = "device_name is empty";
const char cm_log_error_di[] CM_READ_ONLY = "device_id is empty";
const char cm_log_error_ret_code[] CM_READ_ONLY = "\r\n ret_code = %d (!= 200), abort!\r\n ";
const char cm_log_error_secret_1[] CM_READ_ONLY = "secret type is product_secret, but the product_secret is null";
const char cm_log_error_secret_2[] CM_READ_ONLY = "device_secret is null";

/* warning */
const char cm_log_warning_ota_started[] CM_READ_ONLY = "ota is started";
const char cm_log_warning_cloud_disconnected[] CM_READ_ONLY = "cloud is not connect";
const char cm_log_warning_not_support[] CM_READ_ONLY = "NOT_SUPPORT";
const char cm_log_warning_not_arrived[] CM_READ_ONLY = "Should NOT arrived here";
const char cm_log_warning_not_mapping[] CM_READ_ONLY = "can not find mapping";
const char cm_log_warning_not_func[] CM_READ_ONLY = "can not find func";
const char cm_log_warning_no_list[] CM_READ_ONLY = "no list";
const char cm_log_warning_buffer_overflow[] CM_READ_ONLY = "buffer overflow, %s";

/* info */
const char cm_log_info_init[] CM_READ_ONLY = "cm context initialize";
const char cm_log_info_registered[] CM_READ_ONLY = "URI have been registered, can not register again";
const char cm_log_info_registered_1[] CM_READ_ONLY = "service have been registered, can not register again";
const char cm_log_info_event_id[] CM_READ_ONLY = "event_id %d";
const char cm_log_info_event_type[] CM_READ_ONLY = "event_type %d";
const char cm_log_info_rsp_type[] CM_READ_ONLY = "rsp_type %d";
const char cm_log_info_cloud_disconnect[] CM_READ_ONLY = "cloud disconnect";
const char cm_log_info_cloud_reconnect[] CM_READ_ONLY = "cloud reconnect";
const char cm_log_info_result[] CM_READ_ONLY = "rc = %d";
const char cm_log_info_URI[] CM_READ_ONLY = "URI = %s";
const char cm_log_info_URI_1[] CM_READ_ONLY = "URI: %.*s";
const char cm_log_info_URI_sys[] CM_READ_ONLY = "URI type: /SYS/ ";
const char cm_log_info_URI_ext[] CM_READ_ONLY = "URI type: /EXT/ ";
const char cm_log_info_URI_undefined[] CM_READ_ONLY = "URI type: /UNDEFINE/ ";
const char cm_log_info_URI_length[] CM_READ_ONLY = "URI length = %d";
const char cm_log_info_payload_length[] CM_READ_ONLY = "payload length = %d";
const char cm_log_info_firmware[] CM_READ_ONLY = "The firmware is valid";
const char cm_log_info_MQTT_disconnect[] CM_READ_ONLY = "MQTT disconnect.";
const char cm_log_info_MQTT_reconnect[] CM_READ_ONLY = "MQTT reconnect";
const char cm_log_info_remove_mapping[] CM_READ_ONLY = "find mapping and remove it";
const char cm_log_info_enter_process_1[] CM_READ_ONLY = "enter cloud process";
const char cm_log_info_enter_process_2[] CM_READ_ONLY = "enter local process";
const char cm_log_info_process_type[] CM_READ_ONLY = "process %d";
const char cm_log_info_raw_data[] CM_READ_ONLY = "raw data";
const char cm_log_info_auth_req[] CM_READ_ONLY = "req_str = '%s'";
const char cm_log_info_auth_payload_req[] CM_READ_ONLY = "req_payload: \r\n\r\n%s\r\n";
const char cm_log_info_auth_payload_rsp[] CM_READ_ONLY = "response payload: \r\n\r\n%s\r\n";
const char cm_log_info_auth_rsp[] CM_READ_ONLY = "http response: \r\n\r\n%s\r\n";
const char cm_log_info_add_device[] CM_READ_ONLY = "add device";
const char cm_log_info_found_device[] CM_READ_ONLY = "found device";
const char cm_log_info_remove_device[] CM_READ_ONLY = "remove device";
const char cm_log_info_local_disconnect[] CM_READ_ONLY = "local disconnect";
const char cm_log_info_local_reconnect[] CM_READ_ONLY = "local reconnect";

