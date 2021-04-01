/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __PSRAMUHSPHY_BEST2001_H__
#define __PSRAMUHSPHY_BEST2001_H__

#include "plat_types.h"

#define SPIPHY_PSRAMUHS0_CS                 1
#define SPIPHY_PSRAMUHS1_CS                 2

#define SPIPHY_PSRAMUHS0_REG(reg)           (((reg) & 0xFFF) | (SPIPHY_PSRAMUHS0_CS << 12))
#define SPIPHY_PSRAMUHS1_REG(reg)           (((reg) & 0xFFF) | (SPIPHY_PSRAMUHS1_CS << 12))

#ifdef __cplusplus
extern "C" {
#endif

void psramuhs_read_reg(uint32_t reg, uint32_t *val);
void hal_psramuhsip_mc_busy_wait(void);
void hal_psramuhsip_wb_busy_wait(void);

#ifdef __cplusplus
}
#endif

#endif

