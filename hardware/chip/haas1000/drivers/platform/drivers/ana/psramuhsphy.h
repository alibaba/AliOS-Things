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
#ifndef __PSRAMUHSPHY_H__
#define __PSRAMUHSPHY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(psramuhsphy)
#include "hal_phyif.h"

#ifdef SPIPHY_PSRAMUHS0_REG
#define SPI_PSRAMUHSPHY0_REG(reg)       SPIPHY_PSRAMUHS0_REG(reg)
#else
#define SPI_PSRAMUHSPHY0_REG(reg)       (reg)
#endif

#ifdef SPIPHY_PSRAMUHS1_REG
#define SPI_PSRAMUHSPHY1_REG(reg)       SPIPHY_PSRAMUHS1_REG(reg)
#else
#define SPI_PSRAMUHSPHY1_REG(reg)       (reg)
#endif

#define psramuhsphy0_read(reg, val)     hal_phyif_reg_read(SPI_PSRAMUHSPHY0_REG(reg), val)
#define psramuhsphy0_write(reg, val)    hal_phyif_reg_write(SPI_PSRAMUHSPHY0_REG(reg), val)

#define psramuhsphy1_read(reg, val)     hal_phyif_reg_read(SPI_PSRAMUHSPHY1_REG(reg), val)
#define psramuhsphy1_write(reg, val)    hal_phyif_reg_write(SPI_PSRAMUHSPHY1_REG(reg), val)

void psramuhsphy_open(uint32_t clk);

void psramuhsphy_init_calib(void);

void psramuhsphy_calib(uint32_t clk);

void psramuhsphy_sleep(void);

void psramuhsphy_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif

