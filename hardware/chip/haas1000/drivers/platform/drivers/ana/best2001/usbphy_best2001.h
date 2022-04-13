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
#ifndef __USBPHY_BEST2001_H__
#define __USBPHY_BEST2001_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPIPHY_USB_CS                       0

#define SPIPHY_USB_REG(reg)                 (((reg) & 0xFFF) | (SPIPHY_USB_CS << 12))

void usbphy_div_reset_set();

void usbphy_div_reset_clear(uint32_t div);

void usbphy_div_set(uint32_t div);

#ifdef __cplusplus
}
#endif

#endif

