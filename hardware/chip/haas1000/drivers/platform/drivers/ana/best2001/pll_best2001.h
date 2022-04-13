/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
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

