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
/*
 * File: lightduer_dev_info.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: Device information
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DEV_INFO_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DEV_INFO_H

#include "lightduer_system_info.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DevInfo {
    char firmware_version[FIRMWARE_VERSION_LEN + 1];
};

struct DevInfoOps {
/*
 * Provide firmware information about the current system
 */
    int (*get_firmware_version)(char *firmware_version);
/*
 * Set firmware information about the current system
 */
    int (*set_firmware_version)(char const *firmware_version);
};

/*
 * Set the firmware version of the current system
 *
 * @param firmware_version:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_set_firmware_version(char const *firmware_version);

/*
 * Get the firmware version of the current system
 *
 * @param firmware_version: size > FIRMWARE_VERSION_LEN
 * @param len:              string length
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_get_firmware_version(char *firmware_version, size_t len);

/*
 * Report device info to Duer Cloud
 *
 * @param info:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_report_device_info(void);

/*
 * Register device info ops
 *
 * @param ops:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_register_device_info_ops(struct DevInfoOps *ops);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DEV_INFO_H
