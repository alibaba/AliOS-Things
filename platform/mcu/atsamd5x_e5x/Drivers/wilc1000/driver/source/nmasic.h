/**
 *
 * \file
 *
 * \brief This module contains WILC1000 ASIC specific internal APIs.
 *
 * Copyright (c) 2016-2018 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef _NMASIC_H_
#define _NMASIC_H_

#include "../../common/include/nm_common.h"

#define NMI_PERIPH_REG_BASE     0x1000
#define NMI_CHIPID	            (NMI_PERIPH_REG_BASE)
#define rNMI_GP_REG_0			(0x149c)
#define NMI_GP_REG_1   0x14a0

#define NMI_VMM_BASE 0x150000
#define NMI_VMM_CORE_CFG (NMI_VMM_BASE+0x14)

#define WIFI_PERIPH_BASE							0x00000000
#define rPA_CONTROL									(WIFI_PERIPH_BASE+0x9804)

#ifdef CONF_WILC_USE_3000_REV_A
#ifdef CONF_WILC_USE_SDIO
	#define WILC_WAKEUP_REG 					0xf0
	#define WILC_CLK_STATUS_REG 				0xf0
	#define WILC_WAKEUP_BIT 					NBIT0
	#define WILC_CLK_STATUS_BIT 				NBIT4
	#define WILC_FROM_INTERFACE_TO_WF_REG		0xFA
	#define WILC_FROM_INTERFACE_TO_WF_BIT		NBIT0
	#define WILC_TO_INTERFACE_FROM_WF_REG		0xFC
	#define WILC_TO_INTERFACE_FROM_WF_BIT		NBIT0

	#define WILC_INT_STATUS_REG 				0xFE
	#define WILC_INT_CLEAR_REG 					0xFE
#else
	#define WILC_WAKEUP_REG 					0x1
	#define WILC_CLK_STATUS_REG 				0x13
	#define WILC_WAKEUP_BIT 					NBIT1
	#define WILC_CLK_STATUS_BIT 				NBIT2
	#define WILC_FROM_INTERFACE_TO_WF_REG		0x0E
	#define WILC_FROM_INTERFACE_TO_WF_BIT		NBIT0
	#define WILC_TO_INTERFACE_FROM_WF_REG		0x14 
	#define WILC_TO_INTERFACE_FROM_WF_BIT		NBIT0

	#define WILC_INT_STATUS_REG 				0x40
	#define WILC_INT_CLEAR_REG 					0x44

#endif /* WILC_SDIO */

#elif defined CONF_WILC_USE_1000_REV_B
#ifdef CONF_WILC_USE_SDIO
	#define WILC_WAKEUP_REG 					0xf0
	#define WILC_CLK_STATUS_REG 				0xf1
	#define WILC_WAKEUP_BIT  					NBIT0
	#define WILC_CLK_STATUS_BIT 				NBIT0
	#define WILC_FROM_INTERFACE_TO_WF_REG		0xFA
	#define WILC_FROM_INTERFACE_TO_WF_BIT		NBIT0
	#define WILC_TO_INTERFACE_FROM_WF_REG		0xFC
	#define WILC_TO_INTERFACE_FROM_WF_BIT		NBIT0 

	#define WILC_INT_STATUS_REG 				0xF7
	#define WILC_INT_CLEAR_REG 					0xF8
#else
	#define WILC_WAKEUP_REG 					0x1
	#define WILC_CLK_STATUS_REG					0xf /* Assume initially it is B0 chip */ 
	#define WILC_WAKEUP_BIT 					NBIT1
	#define WILC_CLK_STATUS_BIT 				NBIT2

	#define WILC_INT_STATUS_REG 				0x40
	#define WILC_INT_CLEAR_REG 					0x44
	#define WILC_FROM_INTERFACE_TO_WF_REG		0x0B
	#define WILC_FROM_INTERFACE_TO_WF_BIT		NBIT0
	#define WILC_TO_INTERFACE_FROM_WF_REG		0x10
	#define WILC_TO_INTERFACE_FROM_WF_BIT		NBIT0

#endif /* WILC_SDIO */
#endif /* CONF_WILC_USE_3000_REV_A*/


#define NMI_STATE_REG			(0x108c)
#define BOOTROM_REG				(0xc000c)
#define NMI_REV_REG  			(0x207ac)
#define M2M_WAIT_FOR_HOST_REG 	(0x207bc)
#define M2M_FINISH_INIT_STATE 	0x02532636UL
#define M2M_FINISH_BOOT_ROM   	 0x10add09eUL
#define M2M_START_FIRMWARE   	 0xef522f61UL
#define M2M_START_PS_FIRMWARE    0x94992610UL

#ifdef CONF_WILC_USE_SPI
#define IRG_FLAGS_OFFSET	16
#else
#define IRG_FLAGS_OFFSET	0
#endif

#define REV_B0        (0x2B0)
#define GET_CHIPID()	nmi_get_chipid()
#define ISNMC1000(id)   (((id & 0xfffff000) == 0x100000) ? 1 : 0)
#define ISNMC1500(id)   (((id & 0xfffff000) == 0x150000) ? 1 : 0)
#define REV(id)         ( ((id) & 0x00000fff ) )
#define EFUSED_MAC(value) (value & 0xffff0000)

#ifdef CONF_WILC_USE_3000_REV_A

/* Coexistence Block */
#define rCOE_TOP_CTL 								(WIFI_PERIPH_BASE+0x1124)
#define rCOEXIST_CTL 								(WIFI_PERIPH_BASE+0x161E00)
#define rCOEXIST_TIMER 								(WIFI_PERIPH_BASE+0x161E04)
#define rBT_WIRE_SAMPLE_TIME 						(WIFI_PERIPH_BASE+0x161E08)
#define rBT_SLOT_LUMP_CTL1 							(WIFI_PERIPH_BASE+0x161E10)
#define rBT_CNT_THR 								(WIFI_PERIPH_BASE+0x161E28)
#define rBT_CNT_INT 								(WIFI_PERIPH_BASE+0x161E78)
#define rCOE_AUTO_PS_ON_NULL_PKT 					(WIFI_PERIPH_BASE+0x160468)
#define rCOE_AUTO_PS_OFF_NULL_PKT 					(WIFI_PERIPH_BASE+0x16046C)
#define rTX_ABORT_NULL_FRM_DURATION_TIMEOUT_VALUE (WIFI_PERIPH_BASE+0x16045C)
#define rTX_ABORT_NULL_FRM_RATE_POWER_LEVEL 		(WIFI_PERIPH_BASE+0x160460)
#define rTX_ABORT_NULL_FRM_PHY_TX_MODE_SETTING (WIFI_PERIPH_BASE+0x160464)
#define rCOE_AUTO_CTS_PKT 							(WIFI_PERIPH_BASE+0x160470)

#define BT_REJECTION_TIMER			0x800
#define BT_ACCEPTANCE_TIMER			0x700
#define BT_REJ_ACPT_TIMER_TIME_BASE	4
#define BT_REG_TIMER_NULL_THRE		0x783	/* Threshold for generating NULL packet enabling wifi power-save mode  */
#define BT_ACPT_TIMER_THRE			0x000	/* Interrupt will be generated when rej/acc arb counter reach this value */
#define COUNT_TO_ONE_US				39		/* Counts to generate 1 microsecond ticks from system clock  */

#define TX_RATE_1_MBPS		0x0 
#define TX_RATE_2_MBPS		0x1
#define TX_RATE_5_5_MBPS	0x2
#define TX_RATE_11_MBPS		0x3
#define TX_RATE_6_MBPS		0xb
#define TX_RATE_9_MBPS		0xf
#define TX_RATE_12_MBPS		0xa
#define TX_RATE_18_MBPS		0xe
#define TX_RATE_24_MBPS		0x9
#define TX_RATE_36_MBPS		0xd
#define TX_RATE_48_MBPS		0x8
#define TX_RATE_54_MBPS		0xc
#define TX_RATE_MCS0_MBPS	0x80
#define TX_RATE_MCS1_MBPS	0x81
#define TX_RATE_MCS2_MBPS	0x82
#define TX_RATE_MCS3_MBPS	0x83
#define TX_RATE_MCS4_MBPS	0x84
#define TX_RATE_MCS5_MBPS	0x85
#define TX_RATE_MCS6_MBPS	0x86
#define TX_RATE_MCS7_MBPS	0x87

#define PHY_MODE_1_MBPS		0x10141 //802.11b
#define PHY_MODE_2_MBPS		0x10141 //802.11b
#define PHY_MODE_5_5_MBPS	0x10141 //802.11b
#define PHY_MODE_11_MBPS	0x10141 //802.11b
#define PHY_MODE_6_MBPS		0x10142 //802.11a
#define PHY_MODE_9_MBPS		0x10142 //802.11a
#define PHY_MODE_12_MBPS	0x10142 //802.11a
#define PHY_MODE_18_MBPS	0x10142 //802.11a
#define PHY_MODE_24_MBPS	0x10142 //802.11a
#define PHY_MODE_36_MBPS	0x10142 //802.11a
#define PHY_MODE_48_MBPS	0x10142 //802.11a
#define PHY_MODE_54_MBPS	0x10142 //802.11a
#define PHY_MODE_MCS0_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS1_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS2_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS3_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS4_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS5_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS6_MBPS	0x10146 //HT-Mixed
#define PHY_MODE_MCS7_MBPS	0x10146 //HT-Mixed
#endif

/**
*  @struct		tstrM2mWifiGetRevision
*  @brief		Structure holding firmware version parameters
*  @sa			M2M_WIFI_AUTH_WEB, M2M_WIFI_AUTH_WPA, M2M_WIFI_AUTH_WPA2
*/
typedef struct {
	uint8 u8FirmwareMajor; /* Version Major Number which represents the official release base */
	uint8 u8FirmwareMinor; /* Version Minor Number which represents the engineering release base */
	uint8 u8FirmwarePatch;	/* Version pathc Number which represents the pathces release base */
	uint8 u8DriverMajor; /* Version Major Number which represents the official release base */
	uint8 u8DriverMinor; /* Version Minor Number which represents the engineering release base */
	uint8 u8DriverPatch; /* Version Patch Number which represents the pathces release base */
	uint8 BuildDate[sizeof(__DATE__)];
	uint8 BuildTime[sizeof(__TIME__)];
	uint32 u32Chipid; /* HW revision which will be basically the chip ID */
} tstrM2mRev;

#ifdef __cplusplus
     extern "C" {
 #endif
 

sint8 chip_wake(void);
/*
*	@fn		chip_sleep
*	@brief	
*/
sint8 chip_sleep(void);
void chip_idle(void);


sint8 enable_interrupts(void);

sint8 cpu_start(void);

uint32 nmi_get_chipid(void);

uint32 nmi_get_rfrevid(void);

void restore_pmu_settings_after_global_reset(void);

void nmi_update_pll(void);

void nmi_set_sys_clk_src_to_xo(void);

sint8 chip_reset(void);

sint8 firmware_download(void);

#ifdef CONF_WILC_USE_3000_REV_A
sint8 cpu_start_bt(void);
sint8 firmware_download_bt(void);
#endif


sint8 wait_for_firmware_start(void);
sint8 wait_for_bootrom(void);

sint8 chip_deinit(void);

sint8 chip_reset_and_cpu_halt(void);

sint8 set_gpio_dir(uint8 gpio, uint8 dir);

sint8 set_gpio_val(uint8 gpio, uint8 val);

sint8 get_gpio_val(uint8 gpio, uint8* val);

sint8 pullup_ctrl(uint32 pinmask, uint8 enable);

sint8 nmi_get_otp_mac_address(uint8 *pu8MacAddr, uint8 * pu8IsValid);

sint8 nmi_get_mac_address(uint8 *pu8MacAddr);

#ifdef __cplusplus
	 }
 #endif

#endif	/*_NMASIC_H_*/
