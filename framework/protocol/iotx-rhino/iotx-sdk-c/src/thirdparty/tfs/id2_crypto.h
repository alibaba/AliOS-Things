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


#ifndef _ID2_CRYPTO_H_
#define _ID2_CRYPTO_H_

#if defined(__cplusplus)
extern "C" {
#endif

int iotx_mqtt_id2_payload_encrypt(char *topic, iotx_mqtt_topic_info_pt topic_msg);
int iotx_mqtt_id2_payload_decrypt(iotx_mqtt_topic_info_pt topic_msg);

#if defined(__cplusplus)
}
#endif
#endif
