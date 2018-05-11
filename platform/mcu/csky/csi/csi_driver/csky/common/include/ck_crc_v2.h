/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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
/******************************************************************************
 * @file     ck_crc_v2.h
 * @brief    header file for crc_v2 driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_CRC_v2_H_
#define _CK_CRC_v2_H_

#include "stdint.h"
#include "soc.h"

typedef struct {
    __IOM uint32_t CONFIG_REG;                     /* Offset: 0x000 (W/R) config register */
    __IOM uint32_t INIT_VALUE;                     /* Offset: 0x004 (W/R) initial value register */
    __IOM  uint32_t XOR_OUT;                       /* Offset: 0x008 (W/R) xor this value to crc_v2 result as final crc_v2 result  */
    __IM  uint32_t CRC_RESULT;                     /* Offset: 0x00c (R) crc_v2 result result register */
    uint32_t RESERVED1[12];
    __IOM  uint32_t NEW_DATA;                      /* Offset: 0x040 (W/R) new data register */

} ck_crc_reg_t;
#endif
