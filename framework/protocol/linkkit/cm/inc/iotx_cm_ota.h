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


#ifndef __CM_OTA_H__
#define __CM_OTA_H__


#ifdef SERVICE_OTA_ENABLED
#include "iot_import.h"

#include "lite-utils.h"
#include "lite-system.h"
#include "iotx_cm_common.h"

void* iotx_cm_ota_init(iotx_cm_conntext_t* cm_ctx, const char* version);

int iotx_cm_ota_yield(iotx_cm_conntext_t* cm_ctx, iotx_cm_ota_t* cm_ota);

int iotx_cm_ota_deinit(iotx_cm_conntext_t* cm_ctx);

int iotx_cm_ota_request_image(iotx_cm_conntext_t* cm_ctx, const char* version);

int iotx_cm_ota_get_config(iotx_cm_conntext_t* cm_ctx, const char* configScope, const char* getType, const char* attributeKeys);

#endif /* SERVICE_OTA_ENABLED */

#endif /* __CM_OTA_H__ */


