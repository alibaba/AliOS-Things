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
 * @file     ck_sasc_v1.c
 * @brief    CSI Source File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include "drv_sasc.h"
#include "ck_sasc_v1.h"

static uint8_t region[REGION_MAX_NUM] = {0x0};
static uint32_t s_sram_base;
static uint32_t sram_addr_offset;
//
// Functions
//

static int alloc_region(void)
{
    uint8_t i;
    for (i=0; i<REGION_MAX_NUM; i++) {
        if (region[i] != REGION_USED) {
            return i;
        }
    }
    return -1;
}

static inline void eflash_region_security_config(uint8_t region_idx, uint8_t is_security)
{
    uint32_t temp = *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CAR);
    temp &= ~(1 << region_idx);
    temp |= (is_security << region_idx);
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CAR) = temp;
}

static inline void eflash_region_cd_config(uint8_t region_idx, sasc_cd_e ucd)
{
    uint32_t temp = *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG0);
    temp &= ~(3 << (16 + region_idx*2));
    temp |= (ucd << (16 + region_idx*2));
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG0) = temp;
}

static inline void eflash_region_ap_config(uint8_t region_idx, sasc_ap_e sap, sasc_ap_e uap)
{
    uint32_t temp = *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG0);
    temp &= ~(3 << (region_idx*2));
    temp |= (uap << (region_idx*2));
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG0) = temp;

    temp = *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG1);
    temp &= ~(3 << (region_idx*2));
    temp |= (sap << (region_idx*2));
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG1) = temp;
}

static inline int eflash_region_address_config(uint8_t region_idx, uint32_t addr, sasc_size_e size)
{
    uint32_t region_addr = (addr-EFLASH_ADDR_START) / EFLASH_SECTOR_SIZE;
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_RG0 + region_idx * 4) = region_addr << 23 | size;
    return 0;
}

static inline void sram_region_security_config(uint8_t region_idx, uint8_t is_security)
{
    uint32_t temp = *(volatile uint32_t *)(s_sram_base + SRAM_CAR);
    temp &= ~(1 << region_idx);
    temp |= (is_security << region_idx);
    *(volatile uint32_t *)(s_sram_base + SRAM_CAR) = temp;
}


static inline void sram_region_cd_config(uint8_t region_idx, sasc_cd_e ucd)
{
    uint32_t temp = *(volatile uint32_t *)(s_sram_base + SRAM_CFG0);
    temp &= ~(3 << (16 + region_idx*2));
    temp |= (ucd << (16 + region_idx*2));
    *(volatile uint32_t *)(s_sram_base + SRAM_CFG0) = temp;
}

static inline void sram_region_ap_config(uint8_t region_idx, sasc_ap_e sap, sasc_ap_e uap)
{
    uint32_t temp = *(volatile uint32_t *)(s_sram_base + SRAM_CFG0);
    temp &= ~(3 << (region_idx*2));
    temp |= (uap << (region_idx*2));
    *(volatile uint32_t *)(s_sram_base + SRAM_CFG0) = temp;

    temp = *(volatile uint32_t *)(s_sram_base + SRAM_CFG1);
    temp &= ~(3 << (region_idx*2));
    temp |= (sap << (region_idx*2));
    *(volatile uint32_t *)(s_sram_base + SRAM_CFG1) = temp;
}

static inline void eflash_region_active_config(uint8_t region_idx)
{
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CR) |= (1 << region_idx);
}

static inline void sram_region_active_config(uint8_t region_idx)
{
    *(volatile uint32_t *)(s_sram_base + SRAM_CR) |= (1 << region_idx);
}

static inline int sram_region_address_config(uint8_t region_idx, uint32_t addr, sasc_size_e size)
{

    uint32_t region_addr = (addr-SRAM_ADDR_START) / 4;
    *(volatile uint32_t *)(s_sram_base + SRAM_RG0 + region_idx * 4) = region_addr << sram_addr_offset | size;
    return 0;
}

static int32_t eflash_region_config(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security)
{
    uint8_t region_idx = alloc_region();

    eflash_region_security_config(region_idx, is_security);
    eflash_region_address_config(region_idx, addr, size);
    eflash_region_cd_config(region_idx, ucd);
    eflash_region_ap_config(region_idx, sap, uap);
    eflash_region_active_config(region_idx);
    return 0;

}

static int32_t sram_region_config(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security)
{
    uint8_t region_idx = alloc_region();

    s_sram_base = SRAM_REG_BASE;
    sram_addr_offset = 17;

    sram_region_security_config(region_idx, is_security);
    sram_region_address_config(region_idx, addr, size);
    sram_region_cd_config(region_idx, ucd);
    sram_region_ap_config(region_idx, sap, uap);
    sram_region_active_config(region_idx);
    return 0;
}
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
int32_t csi_sasc_config_region(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security)
{
    uint32_t ret;
    if (IS_EFLASH_ADDR(addr)) {
        ret = eflash_region_config(addr, size-SASC_EFLASH_1S, sap, uap, ucd, is_security);
    } else if (IS_SRAM_ADDR(addr)) {
        ret = sram_region_config(addr, size, sap, uap, ucd, is_security);
    } else {
        return -1;
    }
    return ret;
}
