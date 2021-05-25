/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv/clk.h
 * @brief    Header File for CLK Driver.
 * @version  V1.0
 * @date     18. Mar 2020
 ******************************************************************************/

#ifndef _DRV_CLK_H_
#define _DRV_CLK_H_

#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t module;
    uint16_t dev_tag;
    uint8_t  idx;
} csi_clkmap_t;

void csi_clk_enable(csi_dev_t *dev);
void csi_clk_disable(csi_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_CLK_H_ */

