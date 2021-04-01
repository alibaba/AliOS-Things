/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
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

