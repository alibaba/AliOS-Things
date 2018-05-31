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
 * @file     drv_sasc.h
 * @brief    header file for sasc driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CSI_SASC_H_
#define _CSI_SASC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <drv_common.h>

typedef enum {
    SASC_AP_RW = 0,
    SASC_AP_RO = 1,
    SASC_AP_WO = 2,
    SASC_AP_DN = 3
}sasc_ap_e;

#ifdef CONFIG_SASC_CK_V1
typedef enum {
    SASC_CD_DN = 0,
    SASC_CD_DA = 1,
    SASC_CD_IN = 2,
    SASC_CD_DI = 3
}sasc_cd_e;
#else
typedef enum {
    SASC_CD_DI = 0,
    SASC_CD_DA = 1,
    SASC_CD_IN = 2,
    SASC_CD_DN = 3
}sasc_cd_e;
#endif

typedef enum {
    SASC_SRAM_4B = 5,
    SASC_SRAM_8B = 6,
    SASC_SRAM_16B = 7,
    SASC_SRAM_32B = 8,
    SASC_SRAM_64B = 9,
    SASC_SRAM_128B = 10,
    SASC_SRAM_256B = 11,
    SASC_SRAM_512B = 12,
    SASC_SRAM_1KB = 13,
    SASC_SRAM_2KB = 14,
    SASC_SRAM_4KB = 15,
    SASC_SRAM_8KB = 16,
    SASC_SRAM_16KB = 17,
    SASC_SRAM_32KB = 18,
    SASC_SRAM_64KB = 19,
    SASC_SRAM_128KB = 20,
    SASC_EFLASH_1S = 21,
    SASC_EFLASH_2S = 22,
    SASC_EFLASH_4S = 23,
    SASC_EFLASH_8S = 24,
    SASC_EFLASH_16S = 25,
    SASC_EFLASH_32S = 26,
    SASC_EFLASH_64S = 27,
    SASC_EFLASH_128S = 28,
    SASC_EFLASH_256S = 29,
    SASC_EFLASH_512S = 30,
    SASC_EFLASH_1024S = 31,
    SASC_EFLASH_2048S = 32
}sasc_size_e;

/**
  \brief       Config the sasc region attribute.
  \param[in]   handle  sasc handle to operate.
  \param[in]   addr  config region base address.
  \param[in]   size  config region addr.
  \param[in]   sap   super mode ap.
  \param[in]   uap   user mode ap.
  \param[in]   scd   super mode cd.
  \param[in]   ucd   user mode cd.
  \param[in]   is_security  config the region is security or not.
  \return      error code
*/
int32_t csi_sasc_config_region(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security);


#ifdef __cplusplus
}
#endif

#endif /* _CSI_SASC_H_ */
