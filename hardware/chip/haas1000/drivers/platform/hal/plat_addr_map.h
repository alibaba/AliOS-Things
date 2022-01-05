/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __PLAT_ADDR_MAP_H__
#define __PLAT_ADDR_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CHIP_ID_LITERAL                         haas1000

#define _TO_STRING_A(s)                         # s
#define _TO_STRING(s)                           _TO_STRING_A(s)

#define CONCAT_NAME_A(a, b)                     a ## b
#define CONCAT_NAME(a, b)                       CONCAT_NAME_A(a, b)
#define CONCAT_SUFFIX(a, b)                     a.b

#define CONSTRUCT_HDR_NAME_A(d, n)              d/n ## _ ## d
#define CONSTRUCT_HDR_NAME(d, n)                CONSTRUCT_HDR_NAME_A(d, n)
#define CHIP_SPECIFIC_HDR(n)                    _TO_STRING(CONSTRUCT_HDR_NAME(CHIP_ID_LITERAL, n).h)

#include CHIP_SPECIFIC_HDR(plat_addr_map)

#define RAM_TO_RAMX(d)                          ((d) - RAM_BASE + RAMX_BASE)
#define RAMX_TO_RAM(d)                          ((d) - RAMX_BASE + RAM_BASE)

#define FLASH_TO_FLASHX(d)                      ((d) - FLASH_BASE + FLASHX_BASE)
#define FLASHX_TO_FLASH(d)                      ((d) - FLASHX_BASE + FLASH_BASE)

#define FLASH_C_TO_NC(d)                        ((d) - FLASH_BASE + FLASH_NC_BASE)
#define FLASH_NC_TO_C(d)                        ((d) - FLASH_NC_BASE + FLASH_BASE)

#define PSRAM_TO_PSRAMX(d)                      ((d) - PSRAM_BASE + PSRAMX_BASE)
#define PSRAMX_TO_PSRAM(d)                      ((d) - PSRAMX_BASE + PSRAM_BASE)

#define PSRAM_C_TO_NC(d)                        ((d) - PSRAM_BASE + PSRAM_NC_BASE)
#define PSRAM_NC_TO_C(d)                        ((d) - PSRAM_NC_BASE + PSRAM_BASE)

#define BUILD_INFO_MAGIC                        0xBE57341D

#ifdef __cplusplus
}
#endif

#endif
