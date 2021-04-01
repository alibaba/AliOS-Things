/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __PLL_BEST2001_H__
#define __PLL_BEST2001_H__

#ifdef __cplusplus
extern "C" {
#endif

void usb_pll_enable(int enable);

void usb_pll_config(uint64_t freq);

void ddr_pll_config();

void dsp_pll_config(uint64_t freq);

void bb_pll_config(uint64_t freq);

#ifdef __cplusplus
}
#endif

#endif

