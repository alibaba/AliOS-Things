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



#ifndef _IOTX_SHADOW_UPDATE_H_
#define _IOTX_SHADOW_UPDATE_H_

#include "iot_import.h"

#include "shadow.h"
#include "shadow_config.h"
#include "shadow_common.h"


iotx_update_ack_wait_list_pt iotx_shadow_update_wait_ack_list_add(
            iotx_shadow_pt pshadow,
            const char *token,
            size_t token_len,
            iotx_push_cb_fpt cb,
            void *pcontext,
            uint32_t timeout);

void iotx_shadow_update_wait_ack_list_remove(iotx_shadow_pt pshadow, iotx_update_ack_wait_list_pt element);

void iotx_ds_update_wait_ack_list_handle_expire(iotx_shadow_pt pshadow);

void iotx_ds_update_wait_ack_list_handle_response(
            iotx_shadow_pt pshadow,
            const char *json_doc,
            size_t json_doc_len);


#endif /* _IOTX_SHADOW_UPDATE_H_ */
