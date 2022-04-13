/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
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

