/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
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

