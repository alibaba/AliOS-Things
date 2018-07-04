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

#include <stdio.h>
#include "lite-system.h"
#include "iot_import.h"
#include "iot_export.h"

#include "lite-utils.h"

int iotx_midreport_reqid(char *requestId, char *product_key, char *device_name)
{
    int ret;
    /* requestId = pk+devicename+mid */
    ret = HAL_Snprintf(requestId,
                       MIDREPORT_REQID_LEN,
                       "%s_%s_mid",
                       product_key,
                       device_name);
    return ret;
}

int iotx_midreport_payload(char *msg, char *requestId, char *mid, char *pid)
{
    int ret;
    /*topic's json data: {"id":"requestId" ,"params":{"_sys_device_mid":mid,"_sys_device_pid":pid }}*/
    ret = HAL_Snprintf(msg,
                       MIDREPORT_PAYLOAD_LEN,
                       "{\"id\":\"%s\",\"params\":{\"_sys_device_mid\":\"%s\",\"_sys_device_pid\":\"%s\"}}",
                       requestId,
                       mid,
                       pid);
    return ret;
}

int iotx_midreport_topic(char *topic_name, char *topic_head, char *product_key, char *device_name)
{
    int ret;
    /* reported topic name: "/sys/${productKey}/${deviceName}/thing/status/update" */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "%s/sys/%s/%s/thing/status/update",
                       topic_head,
                       product_key,
                       device_name);
    return ret;
}


