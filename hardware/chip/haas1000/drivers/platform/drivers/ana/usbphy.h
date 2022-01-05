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
#ifndef __USBPHY_H__
#define __USBPHY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(usbphy)
#include "hal_analogif.h"
#include "hal_phyif.h"

#ifdef SPIPHY_USB_REG
#define SPI_USBPHY_REG(reg)             SPIPHY_USB_REG(reg)
#elif defined(ISPI_USBPHY_REG)
#define SPI_USBPHY_REG(reg)             ISPI_USBPHY_REG(reg)
#else
#define SPI_USBPHY_REG(reg)             (reg)
#endif

#ifdef CHIP_HAS_SPIPHY
#define usbphy_read(reg, val)           hal_phyif_reg_read(SPI_USBPHY_REG(reg), val)
#define usbphy_write(reg, val)          hal_phyif_reg_write(SPI_USBPHY_REG(reg), val)
#else
#define usbphy_read(reg, val)           hal_analogif_reg_read(SPI_USBPHY_REG(reg), val)
#define usbphy_write(reg, val)          hal_analogif_reg_write(SPI_USBPHY_REG(reg), val)
#endif

void usbphy_open(void);

void usbphy_close(void);

void usbphy_sleep(void);

void usbphy_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif

