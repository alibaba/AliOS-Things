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



#ifndef _IOTX_SHADOW_DELTA_H_
#define _IOTX_SHADOW_DELTA_H_

#include "iot_import.h"
#include "shadow.h"
#include "shadow_config.h"
#include "shadow_common.h"
#include "shadow_update.h"


int iotx_shadow_delta_check_existence(iotx_shadow_pt pshadow, const char *attr_name);

void iotx_shadow_delta_entry(
            iotx_shadow_pt pshadow,
            const char *json_doc,
            size_t json_doc_len);

iotx_err_t iotx_shadow_delta_register_attr(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr);

#endif /* _IOTX_SHADOW_DELTA_H_ */
