#ifndef __INC_RTL8711B_WL_ON_H
#define __INC_RTL8711B_WL_ON_H

/* this is a subset of hal_com_reg.h */

/* BIT_WL_PMC_OFFMAC 0x0020 */
#define BIT_WL_PMC_OFFMAC				(0x00000001 << 1) /*!< Auto FSM to Turn On, include clock, isolation, power control for MAC only
														(auto set by ICFG, and clear when Power Ready) */
#define BIT_WL_APMC_ONMAC				(0x00000001 << 0) /*!< Auto FSM to Turn On, include clock, isolation, power control for MAC only */

/* REG_WL_RF_PSS 0x005c */
#define BIT_AFE_POWER_MODE_SEL		(0x00000001 << 8) /*!< AFE power mode selection:1:  LDO mode 0:  Power-cut mode */
#define BIT_SEL_LDO_RF					(0x00000001 << 2) /*!< Power source selection1: LDO mode (power source is 3.3V,VD33PAD); 0: Power Cut mode (Power source is 1.2V,VDTR). */
#define BIT_SEL_LDO_SYN					(0x00000001 << 1) /*!< Power source selection1: LDO mode (power source is 3.3V,VD33SYN); 0: Power Cut mode (Power source is 1.2V,VDSYN). */
#define BIT_SEL_LDO_BUF					(0x00000001 << 0) /*!< Power source selection1: LDO mode (power source is 3.3V,VD33SYN); 0: Power Cut mode (Power source is 1.2V,VDSYN). */

/* REG_USB_INDIRECT_CTRL 0x006c */
#define BIT_USB_HOST_INT_REQ			(0x00000001 << 31) /*!< For USB doggle mode, USB Host write this bit to 1 will trigger interrupt to CM4. After CM4 finishes handling this interrupt , CM4 will clear this bit to 0 */
//#define BIT_USB_HOST_INT_TYPE			(0x00000001 << 30) /*!< 0: read efuse, 1: write efuse, 2: host cmd */
#define BIT_USB_HOST_CMD				(0x0000001F << 24) /*!< host cmd val */

/* REG_USB_SIE_IMR 0x0078 */
#define BIT_EFUSE_RD_MSK_CM4			(0x00000001 << 23) /*!< USB host indirect read efuse interrupt mask for cm4 */
#define BIT_USB_SUS_MSK_CM4			(0x00000001 << 22) /*!< USB suspend interrupt mask for cm4 */
#define BIT_USB_RES_MSK_CM4			(0x00000001 << 21) /*!< USB resume interrupt mask for cm4 */
#define BIT_SE0_RST_MSK_CM4			(0x00000001 << 20) /*!< SE0 reset interrupt mask for cm4 */
#define BIT_SIE_ACK_DONE_MSK_CM4		(0x00000001 << 19) /*!< SIE ACK done interrupt mask for cm4 */
#define BIT_SIE_NAK_DONE_MSK_CM4		(0x00000001 << 18) /*!< SIE NAK done interrupt mask for cm4 */
#define BIT_LPM_RSM_MSK_CM4			(0x00000001 << 17) /*!< SIE resume from LPM interrupt mask for cm4 */
#define BIT_LPM_ACT_MSK_CM4			(0x00000001 << 16) /*!< SIE enter LPM interrupt mask for cm4 */
#define BIT_EFUSE_RD_MSK_DW8051		(0x00000001 << 7) /*!< USB host indirect read efuse interrupt mask for dw8051 */
#define BIT_USB_SUS_MSK_DW8051		(0x00000001 << 6) /*!< USB suspend interrupt mask for dw8051 */
#define BIT_USB_RES_MSK_DW8051		(0x00000001 << 5) /*!< USB resume interrupt mask for dw8051 */
#define BIT_SE0_RST_MSK_DW8051		(0x00000001 << 4) /*!< SE0 reset interrupt mask for dw8051 */
#define BIT_SIE_ACK_DONE_MSK_DW8051	(0x00000001 << 3) /*!< SIE ACK done interrupt mask for dw8051 */
#define BIT_SIE_NAK_DONE_MSK_DW8051	(0x00000001 << 2) /*!< SIE NAK done interrupt mask for dw8051 */
#define BIT_LPM_RSM_MSK_DW8051		(0x00000001 << 1) /*!< SIE resume from LPM interrupt mask for dw8051 */
#define BIT_LPM_ACT_MSK_DW8051		(0x00000001 << 0) /*!< SIE enter LPM interrupt mask for dw8051 */

/* REG_USB_SIE_INT 0x007c */
#define BIT_USB_CMD_INT				((u32)(0x00000001 << 7)) /*!<  USB host indirect read/write efuse or host cmd interrupt */
#define BIT_USB_SUS_INT					((u32)(0x00000001 << 6)) /*!<  USB suspend interrupt */
#define BIT_USB_RES_INT					((u32)(0x00000001 << 5)) /*!<  USB resume interrupt */
#define BIT_SE0_RST_INT					((u32)(0x00000001 << 4)) /*!<  SE0 reset interrupt */
#define BIT_SIE_ACK_DONE_INT			((u32)(0x00000001 << 3)) /*!<  SIE ACK done interrupt */ 
#define BIT_SIE_NAK_DONE_INT			((u32)(0x00000001 << 2)) /*!<  SIE NAK done interrupt */
#define BIT_LPM_RSM_INT				((u32)(0x00000001 << 1)) /*!<  SIE resume from LPM interrupt */ 
#define BIT_LPM_ACT_INT					((u32)(0x00000001 << 0)) /*!<  IE enter LPM interrupt */

/* REG_USB_PWR_OPT 0x0088 */
#define BIT_CM4_WAKE_USB				((u32)(0x00000001 << 6)) /*!<  R/W	0 		cm4 wakeup usb device,  1: wakeup  0: not wakeup */
#define BIT_HOST_WAKE_DEV_EN			((u32)(0x00000001 << 5)) /*!<  R/W	0		usb host wake device function enable,  1: Enable,  0:Disable */
#define BIT_HOST_WAKE_DEV				((u32)(0x00000001 << 4)) /*!<  R/W	0		usb host wake device, 1: wake */
#define BIT_USB_LPS_BLOCK				((u32)(0x00000001 << 3)) /*!<  R/W	0		Block USB RX for wlan is in LPS,  1: Block  RX ;   0: Not block */
#define BIT_USB_LPM_NY					((u32)(0x00000001 << 2)) /*!<  R/W	0		USB LPM Not Yet */
#define BIT_USB_SUS_DIS					((u32)(0x00000001 << 1)) /*!<  R/W	0		Disable USB enter suspend,  1: Disable,  0: enable */
#define BIT_USB_LPMACT_EN				((u32)(0x00000001 << 0)) /*!<  R/W	0		Enable USB enter LPM ,   1:  Enable,   0: Disable */

/* REG_SYS_CFG_8710B 0xF0 */
#define BIT_USB_DOGGLE_MODE			((u32)(0x00000001 << 1)) /*!< 1: enable usb host access wifi mac, this bit should set by host driver */
#define BIT_MCLK_VLD					((u32)(0x00000001 << 0)) /*!< 1: MAC clock ready flag */


#define REG_WL_PMC_CTRL			0x0020

#define REG_WL_RF_PSS				0x005C /*!< select RF power source: LDO:3.3V, PC: 1.2V*/

#define REG_SYS_CFG_8710B			0x00F0

#define REG_USB_INDIRECT_CTRL		0x006C
#define REG_USB_SIE_IMR			0x0078
#define REG_USB_SIE_INT				0x007c
#define REG_WL_PMC_ISR_8711B		0x0084
#define REG_USB_PWR_OPT			0x0088

#define REG_USB_HOST_RW_DATA		0x009C
#define REG_USB_HOST_RW_ADDR	0x00F8
#endif //__INC_RTL8711B_WL_ON_H