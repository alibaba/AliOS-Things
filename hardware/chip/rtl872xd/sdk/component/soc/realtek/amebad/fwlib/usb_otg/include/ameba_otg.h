/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_OTG_H_
#define _RTL8195A_OTG_H_

#define OTG_FAST_INIT   1
#define DWC_EN_ISOC 1
//#define DWC_EN_ISOC 0
#if defined(DWC_EN_ISOC)
#define USB_REQ_ISO_ASAP 1
#endif

#if defined(DWC_DEVICE_ONLY)
//#define USB_DEV_FULL_SPEED
#endif

#define DWC_RM_DEV_RDNT_SRC
#define DWC_RM_HOST_RDNT_SRC
#define HAL_OTG_READ32(addr)            HAL_READ32(USB_OTG_REG_BASE, (u32)addr)
#define HAL_OTG_WRITE32(addr, value)    HAL_WRITE32(USB_OTG_REG_BASE, (u32)addr, value)

#define HAL_OTG_MODIFY32(addr, clrmsk, setmsk)     HAL_WRITE32(USB_OTG_REG_BASE,(u32)addr,\
                                                   ((HAL_READ32(USB_OTG_REG_BASE, (u32)addr) & (~clrmsk)) | setmsk))

#define DWC_READ_REG32(_reg_)   HAL_OTG_READ32((u32)_reg_)
#define DWC_WRITE_REG32(_reg_, _val_)   HAL_OTG_WRITE32((u32)_reg_,_val_)
#define DWC_MODIFY_REG32(_reg_,_cmsk_,_smsk_)   HAL_OTG_MODIFY32((u32)_reg_,_cmsk_,_smsk_)

// USB OTG addon register
#define REG_OTG_PWCSEQ_IP_OFF		0x30004     //This is in OTG IP
#define REG_OTG_PS_INTR_STS			0x30008     //This is in OTG IP
#define REG_OTG_PS_INTR_MSK			0x3000C     //This is in OTG IP

/* #define REG_OTG_PWCSEQ_IP_OFF               0x30004 */
#define BIT_UPLL_CKRDY				BIT(5)	/* R/W	0		1: USB PHY clock ready */
#define BIT_USBOTG_EN				BIT(8)	/* R/W	0		1: Enable USB OTG */
#define BIT_USBPHY_EN				BIT(9)	/* R/W	0		1: Enable USB PHY */
#endif
