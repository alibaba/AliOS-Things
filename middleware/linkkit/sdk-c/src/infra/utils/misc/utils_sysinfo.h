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
#ifndef _IOTX_COMMON_SYSINFO_H_
#define _IOTX_COMMON_SYSINFO_H_

#include "iot_import.h"
#ifdef BUILD_AOS
    #include "activation.h"
#endif

#ifndef VERSION_NUM_SIZE
#define VERSION_NUM_SIZE    4
#endif

#ifndef RANDOM_NUM_SIZE
#define RANDOM_NUM_SIZE     4
#endif

#ifndef MAC_ADDRESS_SIZE
#define MAC_ADDRESS_SIZE    8
#endif

#ifndef CHIP_CODE_SIZE
#define CHIP_CODE_SIZE      4
#endif
// aos activation data len
#define AOS_ACTIVE_INFO_LEN (81)


// activation device type
typedef enum
{
    ACTIVE_SUBDEV,              // it's a subDevice
    ACTIVE_SINGLE_GW            // it s a single or gateway device
} active_device_type_t;

// activation system type
typedef enum
{
    ACTIVE_LINKKIT_ONLY,        // only linkkit implement
    ACTIVE_LINKKIT_AOS,         // both linkkit and AOS implement
    ACTIVE_LINKKIT_OTHERS       // linkkit and 3-party OS implement
} active_system_type_t;

/**
 * @brief Get OS versoin from AliOS.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void aos_get_version_hex( unsigned char version[VERSION_NUM_SIZE] );

/**
 * @brief Get MAC hex from AliOS.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void aos_get_mac_hex( unsigned char mac[MAC_ADDRESS_SIZE] );

/**
 * @brief Get Chip code from AliOS.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void aos_get_chip_code( unsigned char chip_code[CHIP_CODE_SIZE] );



#endif /* _IOTX_COMMON_SYSINFO_H_ */
