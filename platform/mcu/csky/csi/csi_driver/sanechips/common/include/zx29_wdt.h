/**
 * File: wdt_si.h
 * Brief: Implementation of Sanechips wdt
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _WDT_SI_H_
#define _WDT_SI_H_

#include <soc.h>

#define FAILURE    -1
#define SUCCESS     0

#define SRC_CLK_19M2    (19200000)
#define SRC_CLK_32K     (32768)

#define SBY_CRM_BASE_ADDR       (SBY_CRPM_BASEADDR)
#define CK_WDT_BASE             (AP_WDT_BASEADDR)
#define CK_CRM_BASE_ADDR        (AP_CRPM_BASEADDR)

#define WDT_RESET_TYPE          (SBY_CRM_BASE_ADDR+0x4)

#define WDT_CRM_REG             (CK_CRM_BASE_ADDR + 0x70)
#define WDT_RESET_REG           (CK_CRM_BASE_ADDR + 0x74)
#define WDT_CRM_INT_CLEAR       (CK_CRM_BASE_ADDR + 0x88)

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
/*wdt reg define*/
typedef struct {
    uint32_t version;     /**< offset:0x0,      R/W  */
    uint32_t config;      /**< offset:0x4,      R/W  */
    uint32_t value_load;  /**< offset:0x8,      R/W  */
    uint32_t count;       /**< offset:0xc,      R/W  */
    uint32_t status;      /**< offset:0x10,     R/W  */
    uint32_t value_int;   /**< offset:0x14,     R/W  */
    uint32_t set_en;      /**< offset:0x18,     R/W  */
    uint32_t start;       /**< offset:0x1c,     R/W  */
} zx29_wdt_reg_t;

#endif  /* _WDT_SI_H_ */
