/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"

//#define IOTX_PRODUCT_KEY        "21ESeH6RYT7"
//#define IOTX_DEVICE_NAME        "ota_demo"
//#define IOTX_DEVICE_SECRET      "HTVEzMoQrNLV5CjmjKs2l16aDpmXi1Aa"
//#define IOTX_DEVICE_ID          "21ESeH6RYT7.ota_demo"

#define IOTX_PRODUCT_KEY        "stMRPCR0yQu"
#define IOTX_DEVICE_NAME        "xikan_ota"
#define IOTX_DEVICE_SECRET      "XZjhoJclBJpvcEoa8eLYSxnb3ksRVd3W"
#define IOTX_DEVICE_ID          "stMRPCR0yQu.xikan_ota"

//#define IOTX_PRE_DTLS_SERVER_URI "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI "coap://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5683"


//#define IOTX_PRE_NOSEC_SERVER_URI "coap://iot-as-coap.alibaba.net:5683"

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


#define OTA_BUF_LEN        (5000)

extern int iotx_get_well_known(iotx_coap_context_t *p_context);

static int fetch_ota(void *h_ota, void *h_coap)
{
    int rc = 1;
    FILE *fp;
    uint32_t last_percent = 0, percent = 0;
    char version[128], md5sum[33];
    int32_t len, size_downloaded, size_file;
    char buf_ota[OTA_BUF_LEN];
    int32_t firmware_valid;

    if (NULL == (fp = fopen("ota.bin", "wb+"))) {
        EXAMPLE_TRACE("open file failed");
        return -1;
    }

    do {
        len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1);
        if (len > 0) {
            if (1 != fwrite(buf_ota, len, 1, fp)) {
                EXAMPLE_TRACE("write data to file failed");
                rc = -1;
                break;
            }
        }else {
            IOT_OTA_ReportProgress(h_ota, IOT_OTAP_FETCH_FAILED, NULL);
            EXAMPLE_TRACE("ota fetch fail");
        }

        /* get OTA information */
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);

        last_percent = percent;
        percent = (size_downloaded * 100) / size_file;
        if (percent - last_percent > 0) {
            IOT_OTA_ReportProgress(h_ota, percent, NULL);
            IOT_OTA_ReportProgress(h_ota, percent, "hello");
        }

        IOT_CoAP_Yield(h_coap);

    } while (!IOT_OTA_IsFetchFinish(h_ota));

    while (1 == rc) {
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
        if (0 == firmware_valid) {
            EXAMPLE_TRACE("The firmware is invalid");
            rc = -1;
            break;
        } else {
            EXAMPLE_TRACE("The firmware is valid");
            rc = -1;
            break;
        }
    }

    if (NULL != fp) {
        fclose(fp);
    }

    return rc;
}


//-1, fetch failed
//0, no any ota firmware
//1, fetch successfully
static int try_fetch_ota(void *h_ota, void *h_coap)
{
    if (IOT_OTA_IsFetching(h_ota)) {
        return fetch_ota(h_ota, h_coap);
    }

    return 0;
}

int iotx_set_devinfo(iotx_deviceinfo_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));
    /**< get device info*/
    HAL_GetProductKey(p_devinfo->product_key);
    HAL_GetDeviceSecret(p_devinfo->device_secret);
    HAL_GetDeviceName(p_devinfo->device_name);
    HAL_GetDeviceID(p_devinfo->device_id);
    /**< end*/

    return IOTX_SUCCESS;
}


int main(int argc, char **argv)
{
    int rc = 0;
    void *h_ota = NULL;
    iotx_coap_config_t config;
    iotx_deviceinfo_t deviceinfo;
    iotx_coap_context_t *h_coap = NULL;

    IOT_OpenLog("coap-ota");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /**< set device info */
    HAL_SetProductKey(IOTX_PRODUCT_KEY);
    HAL_SetDeviceName(IOTX_DEVICE_NAME);
    HAL_SetDeviceSecret(IOTX_DEVICE_SECRET);
    /**< end */
    iotx_set_devinfo(&deviceinfo);

    memset(&config, 0x00, sizeof(iotx_coap_config_t));
    config.p_devinfo = (iotx_deviceinfo_t *)&deviceinfo;
    config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
    h_coap = IOT_CoAP_Init(&config);
    if (NULL == h_coap) {
        rc = -1;
        EXAMPLE_TRACE("initialize CoAP failed");
        return -1;
    }

    IOT_CoAP_DeviceNameAuth(h_coap);

    h_ota = IOT_OTA_Init(deviceinfo.product_key, deviceinfo.device_name, h_coap);
    if (NULL == h_ota) {
        rc = -1;
        EXAMPLE_TRACE("initialize OTA failed");
        goto do_exit;
    }

    int ota_code = 0;
    do {

        IOT_CoAP_Yield(h_coap);

        IOT_OTA_ReportVersion(h_ota, "iotx_ver_1.0.0");

        HAL_SleepMs(2000);

        ota_code = try_fetch_ota(h_ota, h_coap);

    } while (1 != ota_code);

    EXAMPLE_TRACE("OTA success");

do_exit:
    if (NULL != h_ota) {
        IOT_OTA_Deinit(h_ota);
    }
    if (NULL != h_coap) {
        IOT_CoAP_Deinit(&h_coap);
    }

    return rc;
}

