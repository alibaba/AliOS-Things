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


#include <string.h>

#include "lite-log.h"
#include "lite-system.h"

static iotx_conn_info_t     iotx_conn_info;
static iotx_device_info_t   iotx_device_info;
static int                  iotx_devinfo_inited = 0;

int iotx_device_info_init(void)
{
    if (iotx_devinfo_inited) {
        log_debug("device_info already created, return!");
        return 0;
    }

    memset(&iotx_device_info, 0x0, sizeof(iotx_device_info_t));
    memset(&iotx_conn_info, 0x0, sizeof(iotx_conn_info_t));
    iotx_devinfo_inited = 1;

    log_info("device_info created successfully!");
    return SUCCESS_RETURN;
}

int iotx_device_info_set(
            const char *product_key,
            const char *device_name,
            const char *device_secret)
{
    int ret;
    log_debug("start to set device info!");

    memset(&iotx_device_info, 0x0, sizeof(iotx_device_info));
    strncpy(iotx_device_info.product_key, product_key, PRODUCT_KEY_LEN);
    strncpy(iotx_device_info.device_name, device_name, DEVICE_NAME_LEN);
    strncpy(iotx_device_info.device_secret, device_secret, DEVICE_SECRET_LEN);

    /* construct device-id(@product_key+@device_name) */
    ret = HAL_Snprintf(iotx_device_info.device_id, DEVICE_ID_LEN, "%s.%s", product_key, device_name);
    if ((ret < 0) || (ret >= DEVICE_ID_LEN)) {
        log_err("set device info failed");
        return FAIL_RETURN;
    }

    log_debug("device_info set successfully!");
    return SUCCESS_RETURN;
}

iotx_device_info_pt iotx_device_info_get(void)
{
    return &iotx_device_info;
}

iotx_conn_info_pt iotx_conn_info_get(void)
{
    return &iotx_conn_info;
}

