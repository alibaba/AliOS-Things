#ifndef __INC_RTL8721D_LP_SYS_ON_BIT_H
#define __INC_RTL8721D_LP_SYS_ON_BIT_H

/* #define REG_AON_PWR_CTRL					0x0000 */
#define BIT_DSLP_SNOOZE_MODE_LSPAD_SHUTDOWN		BIT(24) /* R/W	1		1: Enable LSPAD when SNOOZE, 0: shutdown LSPAD when SNOOZE */

/* REG_AON_ISO_CTRL 0x0004 */
#define BIT_AON_CK32KGEN_FEN				BIT(26)	/* R/W	0		1: Enable SDM32K function,  0: disable */
#define BIT_AON_CTOUCH_FEN				BIT(22)	/* R/W	0		1: Enable Captouch function,  0: disable */
#define BIT_AON_TSF_FEN					BIT(20)	/* R/W	0		1: Enable DLPS TSF function,  0: disable */
#define BIT_AON_RTC_FEN					BIT(19)	/* R/W	0		1: Enable RTC function,  0: disable */
#define BIT_AON_KEYSCAN_FEN				BIT(18)	/* R/W	0		1: Enable Keyscan function, 0:disable */
#define BIT_AON_LPON_FEN					BIT(16)	/* R/W	0		LS Global reset, 1: Enable LS function, 0:disable */
#define BIT_AON_SHIFT_CK32K_CKSEL			12		/* R/W	0		*/
#define BIT_AON_MASK_CK32K_CKSEL			0x03	/* R/W	0		LS 32.768K clock selection, 00: 32.768K SDM, 01: 32.768K XTAL, 10/11: Exter 32.768K From GPIO */
#define BIT_AON_CK32KGEN_CKE				BIT(10)	/* R/W	0		1: Enable SDM32K clock,  0: disable */
#define BIT_AON_RTC_CK_SEL					BIT(8)	/* R/W	0		0: SDM32K clock select, 1:XTAL32K clock select */
#define BIT_AON_CTOUCH_CK_SEL				BIT(7)	/* R/W	0		0: tell CTC APB clock disabled, 1. tell CTC APB clock not disable */
#define BIT_AON_CTOUCH_CKE				BIT(6)	/* R/W	0		1: Enable Captouch clock,  0: disable */
#define BIT_AON_TSF_CKE					BIT(5)	/* R/W	0		1: Enable DLPS TSF clock,  0: disable */
#define BIT_AON_RTC_CKE					BIT(4)	/* R/W	0		1: Enable RTC clock,  0: disable */
#define BIT_AON_KEYSCAN_CKE				BIT(3)	/* R/W	0		1: Enable Keyscan clock, 0:disable */
#define BIT_AUX_FUNC_CLK_SEL				BIT(2)	/* R/W	0		"auxiliary function clock source selection,0: 32.768K, 1: 100k" */
#define BIT_AON_ANA4M_CKE					BIT(1)	/* R/W	0		BIT_AON_ANA4M_CKE	"1: Enable ANA4M clock for LS 0: Disable, This register is used by HW PMC, not for SW*/
#define BIT_AON_GCKANA_CKEN				BIT(0)	/* R/W	1		Enable 100K clock for AON register access */

/* REG_AON_BOOT_REASON1 0x000C */
#define BIT_MASK_AON_CHIP_VERSION_SW		0xF
#define BIT_SHIFT_AON_CHIP_VERSION_SW		8
#define BIT_AON_BOD_RESET_EN				BIT(7)	/* R/W	0		"1:  Enable to reset IC when BOD Reset happened0:  Disable"		Not reset by BOD */
#define BIT_MASK_AON_BOD_DBNC_CYC		0x3
#define BIT_SHIFT_AON_BOD_DBNC_CYC		5	/* R/W   0		Used by HW for BOD reset debounce logic Not reset by BOD */
#define BIT_AON_BOD_RESET_FLAG			BIT(4)	/* R/W	0		"1: BOD reset happenedSW need write 0 to clear this flag"		Not reset by BOD */
#define BIT_DSLP_RETENTION_RAM_PATCH		BIT(2)	/* R/W	0		1: DSLP retention RAM patch exists, need to execute*/
#define BIT_SW_SECURE_BOOT_FLAG			BIT(1)    /* RO	0		Read Only, Write Ignore, 1:  SW secure boot done*/
#define BIT_AON_BOOT_EXIT_DSLP			BIT(0)	/* R/W	0		1: Boot from DSLP */

/* REG_AON_LDO_CTRL0 0x0010 */

#define BIT_AON_V1833_SEL				BIT(26)	/* R/W	1		1:3.3V 0:1.8V */
/* BOD high threshold voltage  */
/* 3.3V RESET INTR	1.8V RESET INTR*/
/* 001 1.887  2.297 	001 1.422  1.498 */
/* 010 1.970 	2.397 	010 1.480  1.560 */
/* 011 2.061 	2.507 	011 1.543  1.627 */
/* 100 2.139 	2.602 	100 1.598  1.684 */
/* 101 2.224 	2.704 	101 1.656  1.746 */
/* 110 2.315 	2.815 	110 1.719  1.812 */
/* 111 2.388 	2.904 	111 1.770  1.865 */
#define BIT_MASK_BOD_THRESHOLD_H			0x7
#define BIT_SHIFT_BOD_THRESHOLD_H			23
/* BOD low threshold voltage  */
/* 3.3V RESET INTR	1.8V RESET INTR*/
/* 001 1.784  2.194 	001 1.345  1.422 */
/* 010 1.863 	2.290 	010 1.400  1.480 */
/* 011 1.949 	2.395 	011 1.460  1.543 */
/* 100 2.023 	2.486	100 1.512  1.598 */
/* 101 2.103 	2.584	101 1.567  1.656 */
/* 110 2.190 	2.690	110 1.627  1.719 */
/* 111 2.260 	2.775	111 1.674  1.770 */
#define BIT_MASK_BOD_THRESHOLD_L			0x7
#define BIT_SHIFT_BOD_THRESHOLD_L			20
#define BIT_POW_BOD_INTERRUPT			BIT(19)
#define BIT_POW_BOD_RESET				BIT(18)
#define BIT_AON_LDO_SLP_EN				BIT(16)
/* Digital LDO voltage  */
/* 0000: 0.600V  */
/* 0001: 0.631V  */
/* 0010: 0.660V  */
/* 0011: 0.692V  */
/* 0100: 0.719V  */
/* 0101: 0.749V  */
/* 0110: 0.782V  */
/* 0111: 0.817V  */
/* 1000: 0.856V  */
/* 1001: 0.899V  (default) */
/* 1010: 0.952V  */
/* 1011: 0.998V  */
/* 1100: 1.049V  */
/* 1101: 1.096V  */
/* 1110: 1.149V  */
/* 1111: 1.196V  */
#define BIT_MASK_AON_LDO_V09_ADJ		0xF
#define BIT_SHIFT_AON_LDO_V09_ADJ		12

/* #define REG_AON_LDO_CTRL1 0x0014 */
/* Psram ldo voltage  */
/* 0000: 1.636V  */
/* 0001: 1.662V  */
/* 0010: 1.6875V  */
/* 0011: 1.714V  */
/* 0100: 1.741V  */
/* 0101: 1.770V  */
/* 0110: 1.785V  */
/* 0111: 1.800V  */
/* 1000: 1.815V  */
/* 1001: 1.830V  */
/* 1010: 1.846V  */
/* 1011: 1.862V  */
/* 1100: 1.895V  */
/* 1101: 1.929V  */
/* 1110: 1.963V  */
/* 1111: 2.000V  */
#define BIT_MASK_LDO_PSRAM_ADJ			0xF
#define BIT_SHIFT_LDO_PSRAM_ADJ			17
#define BIT_LDO_PSRAM_SLEEP_EN			BIT(16)
#define BIT_LDO_PSRAM_EN				BIT(15)
/* lpldo voltage  */
/* 0000: 0.600V  */
/* 0001: 0.631V  */
/* 0010: 0.660V  */
/* 0011: 0.692V  */
/* 0100: 0.719V  */
/* 0101: 0.749V  */
/* 0110: 0.782V  */
/* 0111: 0.817V  */
/* 1000: 0.856V  */
/* 1001: 0.899V  (default) */
/* 1010: 0.952V  */
/* 1011: 0.998V  */
/* 1100: 1.049V  */
/* 1101: 1.096V  */
/* 1110: 1.149V  */
/* 1111: 1.196V  */
#define BIT_MASK_LDO_LP_ADJ				0xF
#define BIT_SHIFT_LDO_LP_ADJ			10
#define BIT_LDO_LP_SLEEP_EN				BIT(9)
#define BIT_LDO_LP_EN					BIT(8)

/* REG_MEM_PMCTRL_DSLP 0x001C */
#define BIT_PSW_ROM_PARA_SEL				BIT(8)	/* R/W	1		"SYSPMC ROM parameter selection under different voltage1: Select 1.1v memory parameter for ROM 0: Select 0.9v memory parameter for ROM"  */
#define BIT_AON_DLSP_RETRAM_SD			BIT(2)	/* R/W	0		Retension RAM SD control under Deep-sleep mode  */
#define BIT_AON_DLSP_RETRAM_DS			BIT(1)	/* R/W	0		Retension RAM DS control under Deep-sleep mode  */
#define BIT_AON_DLSP_RETRAM_LS			BIT(0)	/* R/W	0		Retension RAM LS control under Deep-sleep mode  */

/* REG_XTAL32K_CTRL0 0x0020 */
#define BIT_AON_SHIFT_XTAL32K_GM_3_0				28
#define BIT_AON_MASK_XTAL32K_GM_3_0				0x0F		/* [31:28]	R/W	1111		gm size */
#define BIT_AON_SHIFT_XTAL32K_CUR_REP_1_0		26
#define BIT_AON_MASK_XTAL32K_CUR_REP_1_0		0x03		/* [27:26]	R/W	00		replica buffer current */
#define BIT_AON_SHIFT_XTAL32K_CUR_GM_3_0		22
#define BIT_AON_MASK_XTAL32K_CUR_GM_3_0			0x0F		/* [25:22]	R/W	0100	awake gm current */
#define BIT_AON_SHIFT_XTAL32K_CUR_GM_INI_3_0	18
#define BIT_AON_MASK_XTAL32K_CUR_GM_INI_3_0		0x0F		/* [21:18]	R/W	1000		initial gm current */
#define BIT_AON_SHIFT_XTAL32K_CUR_MAIN_1_0		16
#define BIT_AON_MASK_XTAL32K_CUR_MAIN_1_0		0x03		/* [17:16]	R/W	01		xtal main current */
#define BIT_AON_XTAL32K_EN							BIT(0)		/* [0]	R/W	0		xtal enable  */

/* REG_XTAL32K_CTRL1 0x0024 */
#define BIT_AON_XTAL32K_SC_XO_EXTRA				BIT(21)		/* [21]	R/W	0		0: w/i 4pF cap 1: w/o 4pF cap */
#define BIT_AON_XTAL32K_SC_XI_EXTRA				BIT(20)		/* [20]	R/W	0		0: w/i 4pF cap 1: w/o 4pF cap */
#define BIT_AON_SHIFT_XTAL32K_GM_REP_2_0			17
#define BIT_AON_MASK_XTAL32K_GM_REP_2_0			0x07		/* [19:17]	R/W	0		repilca size */
#define BIT_AON_ENB_XTAL32K_FBRES					BIT(16)		/* [16]	R/W	0		0: enable feedback resistor */
#define BIT_AON_SHIFT_XTAL32K_TOK_1_0			14
#define BIT_AON_MASK_XTAL32K_TOK_1_0				0x03		/* [15:14]	R/W	11	00: 100ms 01:200ms 10:400ms 11: 800ms */
#define BIT_AON_SHIFT_XTAL32K_SC_XO_5_0			8
#define BIT_AON_MASK_XTAL32K_SC_XO_5_0			0x3F		/* [13:8]	R/W	100000		xo cap */
#define BIT_AON_SHIFT_XTAL32K_SC_XI_5_0			2
#define BIT_AON_MASK_XTAL32K_SC_XI_5_0			0x3F		/* [7:2]	R/W	100000		xi cap */
#define BIT_AON_EN_XTAL32K_CAP_AWAKE			BIT(1)		/* [1]	R/W	1		internal cap enable as awake */
#define BIT_AON_EN_XTAL32K_CAP_INITIAL			BIT(0)		/* [0]	R/W	1		internal cap enable as initial */

/* REG_OSC32K_CTRL 0x002C */
#define BIT_AON_1K_SEL_L							BIT(22)		/* [22]	R/W	0		"1K clock source selection0: Divided from OSC128K1: Divided from XTAL32K" */
#define BIT_AON_1K_SEL_H							BIT(21)		/* [21]	R/W	0		0: OSC128K 1:XTAL32K */
#define BIT_AON_SEL_LDO_VREF						BIT(20)		/* [20]	R/W	0		0: LDO low valtage setting 1:high voltage */
#define BIT_AON_SHIFT_OSC32K_RCAL_5_0			8
#define BIT_AON_MASK_OSC32K_RCAL_5_0				0x3F		/* [ 13 : 8 ] R / W 100000 osc frequency is highier as RCAL ' s larger  RG0X_32KOSC[5:0] */
#define BIT_AON_OSC32K_EN							BIT(0)		/* [0]	R/W	0		osc enable XTAL_POW[0] */

/* REG_AON_WAKE_TIMER0 0x0030 */
#define BIT_AON_WAKE_TM0_EN						BIT(31)		/* [31]	R/W	0		AON wake timer0 Enable */
#define BIT_AON_SHIFT_WAKE_TM0_TC				0
#define BIT_AON_MASK_WAKE_TM0_TC					0x3FFFFFFF	/* [29:0]	R/W	0		AON wake timer0 counter value */

/* REG_AON_WAKE_TIMER0_CNT 0x0034 */
#define BIT_AON_TIMER_COUNTER					0x3FFFFFFF	/* [29:0]	R/W	0		AON current timer0 counter value */

/* REG_AON_PAD_CTRL 0x0038 */
#define BIT_PAD_RSTB33								BIT(17)		/* [17]	R/W	1		RSTB33 control of all PADs except AON */
#define BIT_AONPAD_RSTB33							BIT(1)		/* [1]	R/W	1		AON PAD(GPIO_C) RSTB33 control */
#define BIT_AONPAD_SHDN33							BIT(0)		/* [0]	R/W	1		AON PAD(GPIO_C) SHDN33/RSTB33 control */

/* REG_AON_WAKE_GPIO_CTRL1			0x003C  */
#define BIT_AON_SHIFT_WAKE_GPIO_DBNC_CYC		8
#define BIT_AON_MASK_WAKE_GPIO_DBNC_CYC			0x007FFF00	/* [22:8]	R/W	0		The GPIO signal will be filtered by the number of debounce cycles of specified in this register. (100k/128k) */
#define BIT_AON_WAKE_GPIO_DBNC_EN				BIT(7)		/* [7]	R/W	1		1: Enable gpio debounce */
#define BIT_AON_GPIO_WAKE_FEN						BIT(0)		/* [0]	R/W	0		1: Enable GPIO wake function */

/* REG_AON_WAKE_GPIO_CTRL2						0x0040  */
#define BIT_AON_SHIFT_GPIO_WAKE_EVENT			24
#define BIT_AON_MASK_GPIO_WAKE_EVENT			0x0F		/* [27:24]	R/W	0		"Wake event for wake pins,  1:  wake event detected, 0: no wake event bit3:  wake event of  wake pin3 bit2:  wake event of  wake pin2 bit1:  wake event of  wake pin1 bit0:  wake event of  wake pin0" */
#define BIT_AON_SHIFT_GPIO_WAKE_PUPD_EN			8
#define BIT_AON_MASK_GPIO_WAKE_PUPD_EN			0x0F		/* [11:8] 	R/W	0		"1: Enable the GPIO PAD's PU/PD resistence according to the wake pin polarity0: Disable" */
#define BIT_AON_SHIFT_GPIO_WAKE_POLARITY		0
#define BIT_AON_MASK_GPIO_WAKE_POLARITY			0x0F		/* [3:0]	R/W	0		"Wake pin polarity,  0:  high active,   1:  low activebit3:  wake pin3 polaritybit2:  wake pin2 polaritybit1:  wake pin1 polaritybit0:  wake pin0 polarity" */

/* REG_AON_DBNC_CTRL						0x0044  */
#define BIT_AON_CHIPEN_DBNC_FEN					BIT(16)		/* [16]	R/W	1		1: Enable CHIP_EN debounce function;  0: Disable */
#define BIT_AON_SHIFT_CHIPEN_DBNC_CYC			0
#define BIT_AON_MASK_CHIPEN_DBNC_CYC				0x7FFF		/* [14:0]	R/W	0x0100		CHIP_EN debounce cycles,  counted by 100K clock */

/* REG_AON_CK32KGEN_AD 0x0048 */
#define BIT_LS_SHIFT_SDM_ADDR				0			
#define BIT_LS_MASK_SDM_ADDR				0x3F	/* R/W	0		SDM register offset		*/

/* REG_AON_CK32KGEN_RWD 0x004C */
#define BIT_AON_SHIFT_SDM_DATA				0


/* REG_AON_PMC_CTRL					0x0050  */
#define BIT_AON_SHIFT_PMC_STL_LPLDO				16
#define BIT_AON_MASK_PMC_STL_LPLDO				0x3FF		/* [25:16]	R/W	0x0A0		stable time for LPLDO,  in unit of 100K clock cycle */
#define BIT_AON_SHIFT_PMC_PSW_STABLE				0
#define BIT_AON_MASK_PMC_PSW_STABLE				0x3FF		/* [9:0]	R/W	0x200		stable time for power switch between LPLDO and SWR,   in unit of 100K clock cycle */

/* REG_AON_PM_OPT						0x0054  */
#define BIT_AON_PMC_DSLP_ERCK						BIT(9) /* [9]	R/W	0		"1: Enable 100K clock for AON register access when PMC enter deep-sleep mode 0: Disable" */
#define BIT_AON_PMC_DSLP_LDHM						BIT(8) /* [8]	R/W	0		"1: Eanble AON LDO enter into sleep mode when  PMC enter deep-sleep mode0: Disable" */
#define BIT_AON_PMC_EN_SNOZ2NORM					BIT(2) /* [2]	R/W	1		"1: PMC exit Snooze state immediately after receive wakeup event0: Upon receiving wakeup event, PMC switch to Deepsleep from Snooze firstly, then PMC will exit from Deepsleep State to Normal" */
#define BIT_AON_PMC_DIRECT_PDN					BIT(1) /* [1]	R/W	0		1: Direct HW power-down,  0: SW power-down */
#define BIT_AON_PMC_SUPPORT_PDN					BIT(0) /* [0]	R/W	1		1: Aon pmc support power-down;  0: not supported, */

/* REG_AON_WAKE_OPT_MSK				0x0058  */
#define BIT_CHIP_PDB_MSK							BIT(7) /* [7]	R/W	0		1: Enable chip power-down interrupt */
#define BIT_CAPTOUCH_WAKE_MSK					BIT(6) /* [6]	R/W	0	 	1: Enable cap-touch wake */
#define BIT_KEYSCAN_WAKE_MSK						BIT(4) /* [4]	R/W	0		1: enable keyscan wake */
#define BIT_DLPS_TSF_WAKE_MSK						BIT(3) /* [3]	R/W	0		1: enable tsf wake under deep-lps mode */
#define BIT_RTC_WAKE_MSK							BIT(2) /* [2]	R/W	0		1: enable RTC wake */
#define BIT_AON_WAKE_TIM0_MSK					BIT(1) /* [1]	R/W	0		1: enable AON timer wake */
#define BIT_GPIO_WAKE_MSK							BIT(0) /* [0]	R/W	0		1: enable GPIO wake */

/* REG_AON_WAKE_OPT_STS				0x005C  */
#define BIT_CHIP_PDB_STS							BIT(7) /* [7]	R/W	0		1: Indicate chip power-down */
#define BIT_CAPTOUCH_WAKE_STS						BIT(6) /* [6]	R/W	0		1: Indicate captouch wake event */
#define BIT_KEYSCAN_WAKE_STS						BIT(4) /* [4]	R/W	0		1: Indicate keyscan wake */
#define BIT_DLPS_TSF_WAKE_STS						BIT(3) /* [3]	R/W	0		1: Indicate tsf wake under deep-lps mode */
#define BIT_RTC_WAKE_STS							BIT(2) /* [2]	R/W	0		1: Indicate RTC wake */
#define BIT_AON_WAKE_TIM0_STS						BIT(1) /* [1]	R/W	0		1: Indicate AON timer wake */
#define BIT_GPIO_WAKE_STS							BIT(0) /* [0]	R/W	0		1: Indicate GPIO wake */
#define BIT_ALL_WAKE_STS							0xFF

/* REG_AON_SNOOZE_MSK				0x0060  */
#define BIT_XTAL_REQ_SNOOZE_MSK					BIT(1) /* [1]	R/W	0		1: Enable xtal request event to trigger PMC enter snooze mode (for RTC SDM calibration) */
#define BIT_CAPTOUCH_SNOOZE_MSK					BIT(0) /* [0]	R/W	0		1: Enable captouch wake event to trigger PMC enter snooze mode */

/* REG_AON_SNOOZE_STS				0x0064  */
#define BIT_XTAL_REQ_SNOOZE_STS					BIT(1) /* [1]	R/W	0		1: Indicate xtal request event to trigger PMC enter snooze mode */
#define BIT_CAPTOUCH_SNOOZE_STS					BIT(0) /* [0]	R/W	0		1: Indicate captouch wake event to trigger PMC enter snooze mode */

/* REG_AON_CHIP_PWR_DOWN_MSK		0x0068 */
#define BIT_CHIP_PWR_DOWN_MSK					BIT(0) /* [0]	R/W	0		1: Enable chip power down wake event */

/* REG_AON_CHIP_PWR_DOWN_STS		0x006C  */
#define BIT_CHIP_PWR_DOWN_STS						BIT(0) /* [0]	R/W	0		1: Indicate chip power down wake event */

/* REG_AON_TSF_BCN_CFG1				0x0090  */
#define BIT_SHIFT_DLPS_BCN_SPACE					0
#define BIT_MASK_DLPS_BCN_SPACE					0xFFFFFF /* [24:0]	R/W	0		Beacon space for deep-lps, in unit of 32us */

/* REG_AON_TSF_BCN_CFG2				0x0094  */
#define BIT_SYNC_TSF_MAC_EN						BIT(31) /* [31]	R/W	0		1: Enable sync WIFI AON TSF to MAC TSF;  0: Disable, write1 will really sync TSF to MAC, and hardware will clear it automaticly */
#define BIT_SYNC_MAC_TSFT_EN						BIT(24) /* [24]	R/W	0		1: Enable sync WIFI MAC TSF to AON TSF;  0: Disable, write1 is just enable,  REG_TSF_SYNC[10] is used to sync MAC TSF */
#define BIT_SHIFT_MAC_TSF_OFFSET					16
#define BIT_MASK_MAC_TSF_OFFSET					0xFF /* [23:16]	R/W	0		Compensation value for WIFI MAC TSF sync to AON TSF */
#define BIT_SHIFT_DLPS_BCN_EARLY					0
#define BIT_DLPS_BCN_EARLY							0xFFFF /*[15:0]	R/W	0		beacon early interval for deep-lps, in unit of 32us */

/* SYSON REG */
/* REG_LP_PWR_ISO_CTRL 0x0200 */
#define BIT_LSYS_ISO_HSYSON				BIT(22) /* R/W	1		1: isolation signls from HS AON domain,   0: disable isolation */
#define BIT_LSYS_ISO_SYSPLL					BIT(21) /* R/W	1		1: isolation signls from SYSPLL domain,   0: disable isolation */
#define BIT_LSYS_ISO_LWLON					BIT(20) /* R/W	1		1: isolation signls from WLAN AON domain,   0: disable isolation */
#define BIT_LSYS_ISO_HSOC					BIT(18) /* R/W	1		1: isolation signls from HS platform domain,   0: disable isolation */
#define BIT_SYS_ISO_HPON 					BIT(17) /* 1: isolation signa from  HS  PERI-ON domain; 0: disable isolation */
#define BIT_SYS_ISO_LPSOC 					BIT(16) /* 1: isolation signls from LS platform domain,   0: disable isolation */

#define BIT_LSYS_LWLON_PWC_EN				BIT(4)	/* R/W	0		"1: Enable WLAN AON  power cut0: Disable" */
#define BIT_MASK_LSYS_HPLAT_PWC_EN_BIT3	BIT(3)	/* R/W	0		"2'b11: Enable HS platform  power cut2'b00: DisableNote:  When enable this power cut,  bit2 should be set to 1'b1 first, then delay 100us set bit 3 to 1'b1" */
#define BIT_MASK_LSYS_HPLAT_PWC_EN_BIT2	BIT(2)
#define BIT_LSYS_HPERI_PWC_EN				BIT(1)	/* R/W	0		"1: Enable HS PERI-ON  power cut0: Disable" */
#define BIT_LSYS_LPLAT_PWC_EN				BIT(0)	/* R/W	0		"1: Enable LS platform power cut0: Disable" */


/* REG_LP_KM0_CTRL 0x0204 */
#define BIT_LSYS_WDT_RESET_HS_ALL			BIT(15)	/* R/W	0		"1: watchdog will reset all the PERIs in HS,	0: watchdog will not reset all the PERIs in HS, uart0 & timer will not be reset and other PERIs will be reset"*/
#define BIT_LSYS_PLFM_WDTRST_OPT			BIT(13)	/*  R/W	0		0:  watchdog reset ls platform only,   1: reset hs/ls/spic platform */
#define BIT_LSYS_PLFM_AUTO_ICG_EN			BIT(12)	/* R/W	0	"1: Enable LS platform auto clock-gating for power save0: Disable" */
#define BIT_LSYS_SPIC_SPUDUP_SIM			BIT(11)	/* R/W	0	*/	
#define BIT_LSYS_PLFM_FLASH_SCE			BIT(10)	/* R/W	0	"1: Need decryption engine for the flash code0: No decryption for the flash code" */	
#define BIT_LSYS_PLFM_MST0_SEL				BIT(9)	/* R/W	0	"1: Enable SIC to access register by bus matrix0: KM0 " */	
#define BIT_KM0_RETENTEION_MODE			BIT(8)	/* R/W	0	"1: enable,  when KM0 reset, Cache will not be clear 0: Disable, when KM0 reset, Cache will be clear by HW" */	
#define BIT_KM0_DBG_RESTARTED				BIT(7)	/* RO	0	km_dbgrestarted_m, used for coresight debug */	
#define BIT_KM0_HALTED						BIT(6)	/* RO	0	km_halted_m, used for coresight debug */	
#define BIT_KM0_LOCKUP						BIT(5)	/* RO	0	km_lockup_m, used for coresight debug */	
#define BIT_KM0_SLEEPSYS					BIT(4)	/* RO	0	km_sl_sleepsys_r, used for __WFE */	


/* REG_LP_FUNC_EN0 0x0208 */
#define BIT_SYS_AMACRO_EN 					BIT(28) /* [28]	R/W	1 XTAL/AUX_ADC shutdown control 1. Enable 0: shutdown */
#define BIT_LSYS_LSOCFUN_FEN 				BIT(16)
#define BIT_LSYS_WLON_FEN 					BIT(12) /* WLON function enable 1: enable,  0: disable */
#define BIT_FLASH_FUN_EN					BIT(8) /* "Flash function enable1: enable flash;? 0: disable flash" */
#define BIT_LSYS_GDMA0_FEN					BIT(7)

#define BIT_SYS_KM0_EN 						BIT(4) /* 1: enable KM0, 0: reset */
#define BIT_SYS_PLATFORM_EN 				BIT(3) /* 1: enable LP platform ; 0: reset */
#define BIT_LSYS_SIC_FEN 					BIT(2)
#define BIT_SYS_FEN_EELDR 					BIT(1) /* 1: enable EE Loader; 0: disable EE loader */

/* REG_LP_FUNC_EN1 0x020C */
#define BIT_LSYS_GPIO0_FEN					BIT(20)
#define BIT_LSYS_QDEC0_FEN					BIT(16)
#define BIT_LSYS_SPGIO0_FEN				BIT(12)
#define BIT_LSYS_I2C0_FEN					BIT(10)
#define BIT_LSYS_ADC_FEN					BIT(8)
#define BIT_LSYS_UART1_FEN_FUN			BIT(7)
#define BIT_LSYS_UART1_FEN_GLB			BIT(6)
#define BIT_LSYS_UART0_FEN_FUN			BIT(5)
#define BIT_LSYS_UART0_FEN_GLB			BIT(4)
#define BIT_LSYS_TIMER0_FEN				BIT(2)
#define BIT_LSYS_IPC_FEN					BIT(1) /*	R/W	0		1: enable LS IPC function, 0: reset */
#define BIT_VENDOR_EN						BIT(0) /*	R/W	0		1: enable Vendor, 0: reset */


/* REG_LP_CLK_CTRL0 0x0210 */
#define BIT_LSYS_WLON_CKE 					BIT(12) /* to be add r26484 */
#define BIT_SHIFT_FLASH_CLK_SEL			9		/* R/W	0		"SPIC clock sel:2'b00: ANA4M2'b01: XTAL2'b10: 400M PLL divison clock"*/
#define BIT_MASK_FLASH_CLK_SEL			0x03
#define BIT_SHIFT_FLASH_CLK_ANA4M			(0x0 << BIT_SHIFT_FLASH_CLK_SEL)
#define BIT_SHIFT_FLASH_CLK_XTAL			(0x1 << BIT_SHIFT_FLASH_CLK_SEL)
#define BIT_SHIFT_FLASH_CLK_PLL			(0x2 << BIT_SHIFT_FLASH_CLK_SEL)
#define BIT_FLASH_CKE						BIT(8)	/* R/W	0		SPIC source clock enable */
#define BIT_LSYS_GDMA0_CKE					BIT(7)	/* R/W	0		*/
#define BIT_SHIFT_LPCPU_CLK_SEL			5
#define BIT_MASK_LPCPU_CLK_SEL				0x03 /*00: XTAL, 01: XTAL/2, 1x: ANA4M */
#define BIT_KM0_CLK_EN						BIT(4)	/* R/W	0		1: enable KM0 clock*/
#define BIT_PLATFORM_CLK_EN				BIT(3)	/* R/W	0		1: enable platform clock*/
#define BIT_SIC_CLK_EN						BIT(2)	/* R/W	0		1: enable SIC clock*/
#define BIT_SYSON_CK_EELDR_EN				BIT(1)	/* R/W	0		1: enable EE LDR clock; 0: gated EE LDR clock*/
#define BIT_SYSON_CK_SYSREG_EN			BIT(0)	/* R/W	0		1: Enable SYSON register clock; 0: gated SYSON Register clock*/




/* REG_LP_CLK_CTRL1 0x0214 */
#define BIT_SHIFT_LPUART0_SCLK_SEL		24
#define BIT_MASK_LPUART0_SCLK_SEL			0x03	/* [25:24] lp uart0(loguart) rx clock, 00: xtal;    01: osc 2m;   10: xtal 2M */
#define BIT_SHIFT_LPUART1_SCLK_SEL		26
#define BIT_MASK_LPUART1_SCLK_SEL			0x03	/* [27:26] lp uart1 rx clock, 00: xtal;    01: osc 2m;   10: xtal 2M */

#define BIT_LSYS_GPIO0_CKSL				BIT(21)	/* R/W	0 	GPIO0 INT clock selecton: 0: LS APB clock 1: 32.768K clock from SDM */		
#define BIT_LSYS_GPIO0_CKE					BIT(20)	/* R/W	0	 */		
#define BIT_LSYS_QDEC0_CKSL_INTR			BIT(18)	/* R/W	0	 QDEC INT clock selecton: 1'b0: LS APB clock 1'b1: division clock from 2M/32K,  */		
#define BIT_LSYS_QDEC0_CKSL32K				BIT(17)	/* R/W	0	 "QDEC system clock sel:1'b0: from 2M OSC1'b1: from 32K" */	
#define BIT_LSYS_QDEC0_CKE					BIT(16)	/* R/W	0	 */		
#define BIT_LSYS_SPGIO0_CKSL_INTR			BIT(13)	/* R/W	0	 I2C system clock selection: 1'b0: LS APB clock 1'b1: OSC2M */		
#define BIT_LSYS_SPGIO0_CKE				BIT(12)	/* R/W	0	*/	
#define BIT_LSYS_I2C0_CKSL					BIT(11)	/* R/W	0	"I2C system clock sel:1'b0: from LS APB clock1'b1: from 2M OSC" */	
#define BIT_LSYS_I2C0_CKE					BIT(10)	/* R/W	0	 */		
#define BIT_LSYS_ADC_CKSL_INTR				BIT(9)	/* R/W	0	 ADC INT clock selection 0: LS APB clock 1: OSC2M */		
#define BIT_LSYS_ADC_CKE					BIT(8)	/* R/W	0	 1: enable LS ADC clock */		
#define BIT_LSYS_UART1_CKE					BIT(6)	/* R/W	0	 */		
#define BIT_LSYS_UART0_CKE					BIT(4)	/* R/W	0	LOGUART */		
#define BIT_LSYS_TIMER0_CKE				BIT(2)	/* R/W	0	 */		
#define BIT_LSYS_IPC_CKE 					BIT(1)
#define BIT_VENDOR_CKE 						BIT(0)

/* REG_LP_FLASH_CTRL 0x0218 */
#define BIT_FLASH_CAL_EN 					BIT(15) /* 15	R/W	1'b0	r_FLASH_CAL_EN	1: flash data will be sample by spic_clk_ps before sending to SPIC, 0: flash data sent to SPIC directly */
#define BIT_FLASH_CK_DIV_EN 				BIT(13) /* 13	R/W	1'b0	r_FLASH_CK_DIV_EN	1: enable to generate flash clock (no phase shift) divided by 400M pll clock, HW detect this signal's rising edge to start the no phase shift division circuit. */
#define BIT_FLASH_CK_PS_DIV_EN 			BIT(12) /* 12	R/W	1'b0	r_FLASH_CK_PS_DIV_EN	1: enable to generate flash clock (with phase shift) divided by 400M pll clock, HW detect this signal's rising edge to start the phase shift clock division circuit. */
#define BIT_SHIFT_DIV_INT					10
#define BIT_MASK_FLASH_DIV_INT			(0x03 << 10) /* 11:10	R/W	2'b0	r_FLASH_DIV_INT	Flash clock division ratio, integrate part, 0: divided by 2 1: divided by 3  2: divided by 4 3: RSVD */
#define BIT_FLASH_DIV_FRAC 					BIT(9) /* 9	R/W	1'b0	r_FLASH_DIV_FRAC	Flash clock division ratio, fractional part, 0: no fraction, only divided by integer set by bit[1:0],  1: 0.5 */
#define BIT_SHIFT_DIV_FRAC					9
#define BIT_FLASH_DIV_HIGH_FRAC 			BIT(8) /* 8	R/W	1'b0	r_FLASH_DIV_HIGH_FRAC	Only valid when r_FLASH_DIV_FRAC= 1, it decides the duty cycle of flash clock when not divided by integer, 1: duty cycle > 50% ; 0: duty cycle < 50% */
#define BIT_SHIFT_FLASH_CK_PS_INT			4
#define BIT_MASK_FLASH_CK_PS_INT			(0x07 << BIT_SHIFT_FLASH_CK_PS_INT) /* 6:4	R/W	3'b0	r_FLASH_CK_PS_INT	Flash clock phase shift in units of 400M pll clock cycels */
#define BIT_FLASH_PS_DIV_RDY 				BIT(3) /* 3	R	1'b0	r_FLASH_PS_DIV_RDY	ready flag of Flash clock with phase shift, Read only */
#define BIT_FLASH_DIV_RDY					BIT(2) /* 2	R	1'b0	r_FLASH_DIV_RDY	ready flag of Flash clock, Read only */

/* REG_LP_KM4_CTRL 0x021C */
#define BIT_SHIFT_LSYS_HCPU_CKSL			25 /* R/W	0		"KM4 CPU clock sel:3'b000: 200MHz3'b001: 100MHz3'b010: 50MHz3'b011: 25MHz3'b100: XTAL" */
#define BIT_MASK_LSYS_HCPU_CKSL			0x7
#define BIT_LSYS_HSYSON_AUX_CKE			BIT(17)	/* R/W	1		1: enable HSYSON aux clock when HS platform is disable, when this bit is set to 1, HSYSON clock switch to ANA4M 0: Disable */
#define BIT_LSYS_HPLAT_CKE					BIT(16)	/* R/W	0		1: enable KM4 & HS platform clock */
#define BIT_HS_SYSCKGEN_EN					BIT(4)	/* R/W	0		HS system clock generator function enable */
#define BIT_LSYS_HCPU_FEN					BIT(3)	/* R/W	0		1: enable KM4 function */
#define BIT_LSYS_HPLAT_FEN					BIT(2)	/* R/W	0		1: enable HS platform */
#define BIT_LSYS_HSOCFUN_FEN				BIT(1)	/* R/W	0		1: enable HS SOC function */
#define BIT_LSYS_HPONFUN_FEN				BIT(0)	/* R/W	0		1: enable HS Peripheral-ON Function ; 0: reset */

//BIT_MASK_SYS_EEROM_SWR_PAR_05_04 SWRLDO_STABLE_TIME
#define SWRLDO_STABLE_0064US					(0x00000000 << 28)
#define SWRLDO_STABLE_0128US					(0x00000001 << 28)
#define SWRLDO_STABLE_0512US					(0x00000002 << 28)
#define SWRLDO_STABLE_2048US					(0x00000003 << 28)

//BIT_MASK_SYS_EEROM_SWR_PAR_03_00 SWRLDO_12V
#define SWRLDO_12V_1P059						(0x00000000 << 24)
#define SWRLDO_12V_1P091						(0x00000001 << 24)
#define SWRLDO_12V_1P125						(0x00000002 << 24)
#define SWRLDO_12V_1P161						(0x00000003 << 24)
#define SWRLDO_12V_1P200						(0x00000004 << 24)
#define SWRLDO_12V_1P220						(0x00000005 << 24)
#define SWRLDO_12V_1P241						(0x00000006 << 24)
#define SWRLDO_12V_1P263						(0x00000007 << 24)
#define SWRLDO_12V_1P286						(0x00000008 << 24)
#define SWRLDO_12V_1P309						(0x00000009 << 24)
#define SWRLDO_12V_1P333						(0x0000000A << 24)
#define SWRLDO_12V_1P385						(0x0000000B << 24)
#define SWRLDO_12V_1P440						(0x0000000C << 24)
#define SWRLDO_12V_1P500						(0x0000000D << 24)
#define SWRLDO_12V_1P565						(0x0000000E << 24)
#define SWRLDO_12V_1P636						(0x0000000F << 24)

//BIT_MASK_SYS_EEROM_SWR_PAR_05_04 LDOH_12V
#define LDOH_12V_0P840V						(0x00000000 << 20)
#define LDOH_12V_0P857V						(0x00000001 << 20)
#define LDOH_12V_0P875V						(0x00000002 << 20)
#define LDOH_12V_0P893V						(0x00000003 << 20)
#define LDOH_12V_0P913V						(0x00000004 << 20)
#define LDOH_12V_0P933V						(0x00000005 << 20)
#define LDOH_12V_0P955V						(0x00000006 << 20)
#define LDOH_12V_0P977V						(0x00000007 << 20)
#define LDOH_12V_1P024V						(0x00000008 << 20)
#define LDOH_12V_1P077V						(0x00000009 << 20)
#define LDOH_12V_1P135V						(0x0000000A << 20)
#define LDOH_12V_1P167V						(0x0000000B << 20)
#define LDOH_12V_1P200V						(0x0000000C << 20)
#define LDOH_12V_1P235V						(0x0000000D << 20)
#define LDOH_12V_1P272V						(0x0000000E << 20)
#define LDOH_12V_1P313V						(0x0000000F << 20)

/* REG_SYS_EFUSE_SYSCFG0 0x0220 */
/* 0x00~0x03 */
#define BIT_PMC_OPT_EN_SWR_XTAL				(0x00000001 << 16) /* default 1, PMC enable SWR&XTAL during power on */

/* REG_SYS_EFUSE_SYSCFG1 0x0224 */
/* 0x04~0x07 */

/* BIT_SWR_REG_ZCDC_H BIT [14:13] */
/* 00:0.1u(PFM) */
/* 01:0.2u */
/* 10:0.7u(PWM) */ /* default */
/* 11:0.8u */
#define BIT_MASK_SWR_REG_ZCDC_H				(0x00000003 << 13) /* [14:13]	R/W	2'b10	BIT_SWR_REG_ZCDC_H	"00:0.1u(PFM)01:0.2u10:0.7u(PWM)11:0.8u" */
#define BIT_SHIFT_SWR_REG_ZCDC_H			13
#define BIT_MASK_SWR_VOL_L1					0x0000000f/* [3:0] */
#define BIT_SWR_FPWM_L1						BIT(6) /* 1: force PWM, 0: force PFM */
/* value 		PWM current(mA)       PFM current (mA) */
/* 000            300                        100  */
/* 001            400                        150  */
/* 010            500                        200  */
/* 011            600                        250  */
/* 100            700                        300  (default) */
/* 101            800                        400  */
/* 110            900                        500  */
/* 111            1000                       VIN/10 */
#define BIT_MASK_SWR_OCP_L1					(0x00000007 << 8) /* [10:8]	R/W	3'b100	BIT_SWR_OCP_L1 */
#define BIT_SHIFT_SWR_OCP_L1				8

/* REG_SYS_EFUSE_SYSCFG2 0x0228 */
/* 0x0A */
#define BIT_MASK_SYS_EEROM_XTAL_FREQ_SEL		(0x0000000f << 16)
#define BIT_SHIFT_SYS_EEROM_XTAL_FREQ_SEL		16

/* REG_SYS_EFUSE_SYSCFG3 0x022C */
/* EFUSE 0xE */
#define BIT_MASK_BOOT_FLASH_BAUD_SEL			(0x00000001 << (7+16))
#define BIT_SHIFT_BOOT_FLASH_BAUD_SEL		(7+16)
#define BIT_SYS_SPIC_ADDR_4BYTE_EN			(0x00000001 << (6+16))
#define BIT_SYS_SPIC_BOOT_SPEEDUP_DIS			(0x00000001 << (5+16))
#define BIT_SYS_DIS_BOOT_LOG_EN				(0x00000001 << (4+16))
#define BIT_SYS_FLASH_DEEP_SLEEP_EN			(0x00000001 << (3+16))
#define BIT_SYS_FLASH_ENCRYPT_EN				(0x00000001 << (2+16))
#define BIT_SYS_RDP_EN							(0x00000001 << (1+16))
#define BIT_SYS_SWD_GP_SEL						(0x00000001 << (0+16)) /*!< SWD_DEFAULT_GPIO,  0: SWD at PB_9,PB_19, 1: SWD at PC_10,PC_11 */

/* EFUSE 0xF */
#define BIT_MASK_OSC4M_FEST					(0xF << 24)	/* 4MHz OSC Frequnce control */
#define BIT_SHIFT_OSC4M_FEST					(24)

/* REG_SWD_PMUX_EN				0x0230 */
#define BIT_LSYS_SWD_PMUX_EN				BIT(0)	/* R/W	1		"1: Eanble SWD pinmux enable function, 0: Disable(SWD pinmux disable by HW). BIT_LSYS_SWD_LOC(0x4800_022C[16]) is valid only when this bit is 1" */

/* REG_LP_SWR_CTRL0 0x0240 */
#define BIT_SHIFT_SWR_FREQ_L_SEL			28
#define BIT_MASK_SWR_FREQ_L_SEL			0xf

#define BIT_SHIFT_SWR_R3_L1_SEL			26
#define BIT_MASK_SWR_R3_L1_SEL			0x3
#define BIT_SHIFT_SWR_R2_L1_SEL			24
#define BIT_MASK_SWR_R2_L1_SEL			0x3
#define BIT_SHIFT_SWR_R1_L1_SEL			22
#define BIT_MASK_SWR_R1_L1_SEL			0x3

#define BIT_SHIFT_SWR_C3_L1_SEL			20
#define BIT_MASK_SWR_C3_L1_SEL			0x3
#define BIT_SHIFT_SWR_C2_L1_SEL			18
#define BIT_MASK_SWR_C2_L1_SEL			0x3
#define BIT_SHIFT_SWR_C1_L1_SEL			16
#define BIT_MASK_SWR_C1_L1_SEL			0x3


#define BIT_SHIFT_SYS_SPSLDO_VOL 			17
#define BIT_MASK_SYS_SPSLDO_VOL 			0x7
#define BIT_SHIFT_SYS_SWR12_IN 			14
#define BIT_MASK_SYS_SWR12_IN 			0x7

#define BIT_SYS_SWR_TYPE_L 				BIT(15)
#define BIT_SYS_FPWM_L1 					BIT(14)
#define BIT_SHIFT_SWR_STD_L1_SEL 			12
#define BIT_MASK_SWR_STD_L1_SEL 			0x3
#define BIT_SHIFT_SWR_VOL_L1_SEL 			8
#define BIT_MASK_SWR_VOL_L1_SEL 			0xf
#define BIT_REGU_LDOD_ENB 					BIT(4)
#define BIT_REGU_LDOM_EN 					BIT(3)
#define BIT_REGU_SWRM_EN 					BIT(2)
#define BIT_REGU_SLDO_EN 					BIT(1)
#define BIT_REGU_BG_EN 						BIT(0)

/* REG_LP_SWR_CTRL1 0x0244 */
#define BIT_SWR_ENFPWMDELAY_H				BIT(8) /* 1: PFM=>PWM Delay */

/* REG_LP_SWR_CTRL2 0x0248 */
#define BIT_MASK_SWR_COT_I_L				0x03 /* [6:5]	R/W	01	*/		
#define BIT_SHIFT_SWR_COT_I_L				5

/* REG_LP_LDO_CTRL0 0x0250 */
#define BIT_CK_100K_EN						BIT(18)
#define BIT_SHIFT_SYS_REGU_LDOE25_ADJ	8
#define BIT_MASK_SYS_REGU_LDOE25_ADJ		0xf
#define BIT_SYS_REGU_LDOE25_EN			BIT(0)

/* REG_LP_XTAL_CTRL1 0x0264 */
#define BIT_XTAL_AAC_GM_EN				BIT(29)

/* REG_LP_SYSPLL_CTRL0 0x0280 */
#define BIT_SHIFT_PLL_BG_VOL				4
#define BIT_MASK_PLL_BG_VOL				0x1ff
#define BIT_LP_PLL_MBIAS_EN				BIT(2)
#define BIT_LP_PLL_BG_I_EN					BIT(1)
#define BIT_LP_PLL_BG_EN					BIT(0)

/* REG_LP_OSC4M_CTRL				0x0290  */
#define BIT_OSC4M_WAKE_EN					BIT(4)	/* R/W	0		When this bit set to 1,  OSC4M can be OFF when KM0 enter sleep mode and PERI's wakeup INT can power on OSC4M */
#define BIT_OSC4M_EN						BIT(0)	/* R/W	0		Power on 4MHz OSC */

/* REG_LP_OSC2M_CTRL				0x0298  */
#define BIT_SHIFT_OSC2M_IB_SEL				24
#define BIT_MASK_OSC2M_IB_SEL				0xF		/* [27:24]	R/W	1000		iptat tuning for temperature variation. 0000:low freq. 1111:high freq. */
#define BIT_SHIFT_OSC2M_VCM_SEL			20
#define BIT_MASK_OSC2M_VCM_SEL			0xF		/* [23:20]	R/W	0000		comparator vcm */
#define BIT_SHIFT_OSC2M_C_SEL				16
#define BIT_MASK_OSC2M_C_SEL				0xF		/* [19:16]	R/W	0000		no use */
#define BIT_SHIFT_OSC2M_R_SEL				8
#define BIT_MASK_OSC2M_R_SEL				0xFF	/* [15:8]	R/W	10000000		11111111: low freq. 00000000:high freq. */
#define BIT_OSC2M_EN						BIT(0)	/* R/W	0		Enable OSC2M function */

/* REG_PLL_SYS_PS_REG 0x02C0 */
#define BIT_POW_CKGEN_400M			BIT(14)	/*	R/W	0		1: power on CK_400M/CK_400M_PS */
#define BIT_SHIFT_DIV_SEL				5		
#define BIT_DIV_SEL						(0x000001FF << BIT_SHIFT_DIV_SEL) /*!< The period of signal CK500M_SYNC/CK500M_PS_SYNC is: (r_DIV_SEL[8:0] + 2) cycles of 500MHz clock */
#define BIT_SHIFT_EN_CK_400M_PS		4		
#define BIT_EN_CK_400M_PS				(0x00000001 <<  BIT_SHIFT_EN_CK_400M_PS) /*!< 1: enable PLL 400M phase shift clock; 0: disable, Enable this bit when use FLASH_CalibrationPhase */
#define BIT_SHIFT_EN_CK_400M			3
#define BIT_EN_CK_400M					(0x00000001 <<  BIT_SHIFT_EN_CK_400M) /*!< 1: enable PLL 400M clock; 0: disable, HW auto enable this bit */
#define BIT_SHIFT_PHASE_SEL			0		
#define BIT_PHASE_SEL					(0x00000007 << 0) /*!< Phase selection for CK_500M_PS 0: 0,  1: 45, 2: 90, 3: 135, 4: 180,  5: 225, 6: 270, 7: 315 */

/* REG_MEM_CTRL2 */
#define BIT_LSYS_SLEP_RET_RAM_LS			BIT(22)	/* R/W	0		"1: Enable Retention RAM light-sleep when KM0 in sleep state0: Disable" */
#define BIT_LSYS_SLEP_RET_RAM_DS			BIT(21)	/* R/W	0		"1: Enable Retention RAM deep-sleep when KM0 in sleep state0: Disable"  */
#define BIT_LSYS_SLEP_RET_RAM_SD			BIT(20)	/* R/W	0		"1: Shut-down Retention RAM when KM0 in sleep state0: Power on Retention RAM when KM0 in sleep state"  */
#define BIT_LSYS_SLEP_IDCACHE_LS			BIT(19)	/* R/W	0		"1: Enable ICACHE/DCACHE light-sleep when KM0 in sleep state0: Disable"  */
#define BIT_LSYS_SLEP_KM0_RAM_LS			BIT(18)	/* R/W	0		"1: Enable CPU RAM light-sleep when KM0 in sleep state0: Disable"  */
#define BIT_LSYS_SLEP_KM0_RAM_DS			BIT(17)	/* R/W	0		"1: Enable CPU RAM deep-sleep when KM0 in sleep state0: Disable"  */
#define BIT_LSYS_SLEP_KM0_RAM_SD			BIT(16)	/* R/W	0		"1: Shut-down CPU RAM when KM0 in sleep state0: Power on CPU RAM when KM0 in sleep state"  */
#define BIT_LSYS_NORM_RET_RAM_LS			BIT(6)	/* R/W	0		"1: Enable Retention RAM light-sleep when KM0 in active state0: Disable"  */
#define BIT_LSYS_NORM_RET_RAM_DS			BIT(5)	/* R/W	0		"1: Enable Retention RAM deep-sleep when KM0 in active state0: Disable"  */
#define BIT_LSYS_NORM_RET_RAM_SD			BIT(4)	/* R/W	0		"1: Shut-down Retention RAM when KM0 in active state0: Power on Retention RAM when KM0 in active state"  */
#define BIT_LSYS_NORM_IDCACHE_LS			BIT(3)	/* R/W	0		"1: Enable ICACHE/DCACHE light-sleep when KM0 in active state0: Disable"  */
#define BIT_LSYS_NORM_KM0_RAM_LS			BIT(2)	/* R/W	0		"1: Enable CPU RAM light-sleep when KM0 in active state0: Disable"  */
#define BIT_LSYS_NORM_KM0_RAM_DS			BIT(1)	/* R/W	0		"1: Enable CPU RAM deep-sleep when KM0 in active state0: Disable"  */
#define BIT_LSYS_NORM_KM0_RAM_SD			BIT(0)	/* R/W	0		"1: Shut-down CPU RAM when KM0 in active state0: Power on CPU RAM when KM0 in active state" */


/* REG_LP_EFUSE_CTRL0 0x02E4 */
#define BIT_MASK_EFUSE_UNLOCK 			(0xff << 8)
#define BIT_SYS_EFUSE_LDALL 				BIT(1)
#define BIT_SYS_AUTOLOAD_SUS 				BIT(0)

/* REG_LP_EFUSE_CTRL1 0x02E8 */
#define BIT_SYS_EF_RWFLAG 					(0x00000001 << 31)
#define BIT_MASK_SYS_EF_PGPD 				(0x00000007 << 28)
#define BIT_MASK_SYS_EF_RDT 				(0x0000000f << 24)
#define BIT_MASK_SYS_EF_PGTS 				(0x0000000f << 20)
#define BIT_SYS_EF_ALDEN 					(0x00000001 << 18)
#define BIT_SHIFT_SYS_EF_ADDR 				8
#define BIT_MASK_SYS_EF_ADDR 				0x000003ff
#define BIT_MASK_SYS_EF_DATA 				(0x000000ff)

/* REG_LP_EFUSE_TEST 0x02EC */
#define BIT_SW_LDOE25_PC_EN				BIT(31)	/* R/W	0		SW Turn on power cut 25 */
#define BIT_SYS_EF_PD_DIS					BIT(30)	/* R/W	0		0: Enable Efuse power down,  1: Disable */
#define BIT_SYS_EF_CRES_SEL 				(0x00000001 << 29)  /* R/W	0		Efuse resistence selection, Efuse CRES selection, 0: 10k ;  1: 1.5k  */
#define BIT_SYS_EF_FORCE_PGMEN 			(0x00000001 << 10)
#define BIT_SYS_EF_CELL_SEL					(0x00000003 << 8)
#define BIT_SYS_EF_TRPT 					(0x00000001 << 7)
#define BIT_MASK_SYS_EF_SCAN_TTHD 		(0x0000007f)

/* REG_LP_RSIP_KEY_CTRL 0x2F0 */
#define BIT_OTF_KEY_REQ					BIT(0)

/* REG_LP_WL_INTR_CTRL 0x0310 */
#define BIT_WL_OFF_IMR					BIT(17)	
#define BIT_WL_ON_IMR					BIT(16)	
#define BIT_WL_OFF_ISR					BIT(1)
#define BIT_WL_ON_ISR					BIT(0)	

/* REG_LP_WL_POW_CTRL 0x0314 */
#define BIT_KM0_CLR_STATE					BIT(17)	/* R/W	0		1: KM0 CLR ready */
#define BIT_KM0_SET_STATE					BIT(16)	/* R/W	0		1: KM0 set ready */
#define BIT_REAL_STATE						BIT(9)	/* R	0		Ready2active real state */
#define BIT_REQ_STATE						BIT(8)	/* R	0		Req2active real state */
#define BIT_HW_AUTO_RSP_EN				BIT(0)	/* R/W	0		1: enable HW auto response */

/* REG_LP_SLP_WAKE_EVENT_MSK0		0x0300 */
#define BIT_LP_WEVT_HS_MSK			BIT(30)	/* [30]	R/W	0		1: enable HS Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_AON_MSK			BIT(29)	/* [29]	R/W	0		1: enable AON Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_SGPIO_MSK			BIT(28)	/* [28]	R/W	0		"1: enable SGPIO Wakeup event; 0: disable the event to wakeup system" */
#define BIT_LP_WEVT_COMP_MSK			BIT(27)	/* [27]	R/W	0		"1: enable Comparetor Wakeup  event; 0: disable the event to wakeup system" */
#define BIT_LP_WEVT_ADC_MSK			BIT(26)	/* [26]	R/W	0		1: enable ADC Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_I2C_MSK			BIT(24)	/* [24]	R/W	0		1: enable I2C Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_UART_MSK			BIT(20)	/* [20]	R/W	0		"1: enable UART Wakeup event; 0: disable the event to wakeup system" */
#define BIT_LP_WEVT_BOD_MSK			BIT(6)	/* [6]	R/W	0		1: enable BOD Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_WLAN_MSK			BIT(5)	/* [5]	R/W	0		1: enable WLAN Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_GPIO_MSK			BIT(4)	/* [4]	R/W	0		1: enable GPIO Wakeup event; 0: disable the event to wakeup system */
#define BIT_LP_WEVT_SWRD_OCP_MSK		BIT(2)	/* [2]	R/W	0		"1: enable DCORE SWR OCP wakeup  event; 0: disable the event to wakeup system" */
#define BIT_LP_WEVT_TIMER_MSK			BIT(1)	/* [1]	R/W	0		"1: enable timer Wakeup LP event; 0: disable the event to wakeup system" */

/* REG_LP_SLP_WAKE_EVENT_STATUS0	0x0304 */
#define BIT_LP_WEVT_HS_STS				BIT(30)	/* [30]	RO	0		1: Indicate HS Wakeup event */
#define BIT_LP_WEVT_AON_STS			BIT(29)	/* [29]	RO	0		1: Indicate AON Wakeup event (0x128) */
#define BIT_LP_WEVT_SGPIO_STS			BIT(28)	/* [28]	RO	0		1: Indicate SGPIO Wakeup event */
#define BIT_LP_WEVT_COMP_STS			BIT(27)	/* [27]	RO	0		1: Indicate Comparetor Wakeup  event */
#define BIT_LP_WEVT_ADC_STS			BIT(26)	/* [26]	RO	0		1: Indicate  ADC Wakeup  event */
#define BIT_LP_WEVT_I2C_STS			BIT(24)	/* [24]	RO	0		1: Indicate I2C  Wakeup  event */
#define BIT_LP_WEVT_UART_STS			BIT(20)	/* [20]	RO	0		1: Indicate UART  Wakeup  event */
#define BIT_LP_WEVT_BOD_STS			BIT(6)	/* [6]	RO	0		1: Indicate BOD Wakeup  event */
#define BIT_LP_WEVT_WLAN_STS			BIT(5)	/* [5]	RO	0		1: Indicate WLAN Wakeup  event */
#define BIT_LP_WEVT_GPIO_STS			BIT(4)	/* [4]	RO	0		1: Indicate GPIO Wakeup event */
#define BIT_LP_WEVT_SWRD_OCP_STS		BIT(2)	/* [2]	RO	0		1: Indicate DCORE SWR OCP event */
#define BIT_LP_WEVT_TIMER_STS			BIT(1)	/* [1]	RO	0		1: Indicate GTimer Wakeup system event; */

/* REG_LP_SLP_WAKE_EVENT_MSK1		0x0308 */
#define BIT_LP_BOD_MSK			BIT(0)	/* [0]	R/W	0		1: Enable BOD interrupt,  0: Disable */

/* REG_LP_SLP_WAKE_EVENT_STATUS1	0x030C */
#define BIT_LP_BOD_STS			BIT(0)	/* [0]	R/W	0		1: Indicate BOD interrupt event */

/* REG_LP_PWRMGT_CTRL				0x0318 */
#define BIT_SHIFT_LSYS_PMC_TUTIME		16		/* [21:16]	R/W	0x3F		"PMC timeunit setting,  in cycles of ANA4M, e.g, default 0x3F,  timeunit = 16us" */
#define BIT_MASK_LSYS_PMC_TUTIME		0x3F	/* [21:16]	R/W			"PMC timeunit setting,  in cycles of ANA4M, e.g, default 0x3F,  timeunit = 16us" */
#define BIT_LSYS_PMC_OPT_BKPOFIMK1	BIT(9)	/* [9]	R/W	0		1: Enable to break PMC power-off flow when receiving wakeup event at pre-define stage1 */
#define BIT_LSYS_PMC_OPT_BKPOFIMK0	BIT(8)	/* [8]	R/W	0		1: Enable to break PMC power-off flow when receiving wakeup event at pre-define stage0 */
#define BIT_LSYS_SYSIRQ_MASK_POLL	BIT(4)	/* [4]	WO	0		1: Mask SYSIRQ INT before enter sleep mode, after write BIT_LSYS_PMC_PMEN_SLEP, SYSIRQ INT will be enabled by HW, 0: Invalid*/
#define BIT_LSYS_PMC_PMEN_SWR2LDO	BIT(3)	/* [3]	R/W	0		"1: Request PMC to Switch LS power from SWR to LPLDO auto clear to 0 after PMC finishes" */
#define BIT_LSYS_PMC_PMEN_LDO2SWR	BIT(2)	/* [2]	R/W	0		"1: Request PMC to Switch LS power from LPLDO to SWR auto clear to 0 after PMC finishes" */
#define BIT_LSYS_PMC_PMEN_SLEP		BIT(1)	/* [1]	R/W	0		"1: Request PMC to enter sleep state, auto clear to 0 after PMC finishes" */
#define BIT_LSYS_PMC_PMEN_DSLP		BIT(0)	/* [0]	R/W	0		"1: Request PMC to enter deep sleep state,auto clear to 0 after PMC finishes" */

/* REG_LP_PWRMGT_OPTION				0x0320 */
#define BIT_LSYS_PST_NORM_EXTL		BIT(17)	/* [17]	R/W	1		"1:  Enable XTAL when PMC exit from sleep mode0:  XTAL ON/OFF state keeps unchange when PMC exit from sleep mode" */
#define BIT_LSYS_PST_NORM_EPSW		BIT(16)	/* [16]	R/W	0		"" Whether to enable power switch when exit from sleep mode1:LS power switch to SWR when PMC exit from sleep mode0:LS power keeps unchange when PMC exit from sleep mode" */
#define BIT_LSYS_PST_DSLP_EACK			BIT(15)	/* [15]	R/W	0		"1: Enable ANA4M when PMC enter into deep sleep mode0: Disable ANA4M when PMC enter into deep sleep mode" */
#define BIT_LSYS_PST_DSLP_DPSW		BIT(14)	/* [14]	R/W	0		"Whether to disable power switch when enter into deepsleep mode1: Keep LS power unchange when PMC enter into deepsleep mode0: LS power switch to LPLDO when PMC enter into deepsleep mode" */
#define BIT_LSYS_PST_SLEP_XACT			BIT(10)	/* [10]	R/W	1		"1: XTAL keeps at active mode when PMC enter into sleep mode0: XTAL switch to LPS mode when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_EXTL			BIT(9)	/* [9]	R/W	0		"1: Enable XTAL when PMC enter into sleep mode0: Disable XTAL when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_ESWR		BIT(8)	/* [8]	R/W	0		"1: Enable SWR when PMC enter into sleep mode0: Disable SWR when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_EPWM		BIT(7)	/* [7]	R/W	1		"1: SWR keeps at PWM mode when PMC enter into sleep mode0: SWR switch to PFM when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_DPSW		BIT(6)	/* [6]	R/W	1		"Whether to disable power switch when enter into sleep mode1: Keep LS power unchange when PMC enter into sleep mode0: LS power switch to LPLDO when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_ERCK			BIT(5)	/* [5]	R/W	0		"1: Enable LS SYSON register clock when PMC enter into sleep mode0: Gate LS SYSON register clock when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_LDLM			BIT(4)	/* [4]	R/W	0		"1: LPLDO keeps at active mode when PMC enter into sleep mode0: LPLDO switch to sleep mode when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_EMPM		BIT(3)	/* [3]	R/W	0		"1: Memory power control setting don't change when PMC enter into sleep mode0: Memory power control setting will change(0xD4[1]) when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_EBUS			BIT(2)	/* [2]	R/W	0		"1: Enable platform clock when PMC entro into sleep mode0: Gate platform clock when PMC entro into sleep mode" */
#define BIT_LSYS_PST_SLEP_EACK			BIT(1)	/* [1]	R/W	1		"1: Enable ANA4M CLK when PMC enter into sleep mode 0: Disable ANA4M CLK when PMC enter into sleep mode" */
#define BIT_LSYS_PST_SLEP_ESOC			BIT(0)	/* [0]	R/W	0		"1: power enable SOC platform when PMC enter into sleep mode (clock-gating)0: power off SoC domain when PMC enter into sleep mode (power-gating)" */

/* REG_LP_RTC_XTAL_DIV_CTRL 0x0330 */
#define BIT_LP_SHIFT_RTC_DIV_XTAL32K		8
#define BIT_LP_MASK_RTC_DIV_XTAL32K		0x7FF//1221 [18:8]
#define BIT_LP_RTC_DIV_XTAL32K_EN			BIT(0)

/* REG_LP_UART_XTAL_DIV_CTRL			0x0334 */
#define BIT_LSYS_UART_XTAL_DIV_EN				BIT(16)	/*[16]	R/W			1: enable XTAL division circuit for UART */
#define BIT_LSYS_SHIFT_UART_XTAL_DIV_FREQ	0
#define BIT_LSYS_MASK_UART_XTAL_DIV_FREQ		0x7FFF	/* [14:0]	R/W	0x7D0		"output divided clock frequency for UART:BIT_LSYS_UART_XTAL_DIV_FREQ/1000  (MHz)" */


/* REG_SWR_PSW_CTRL					0x0338 */
#define BIT_LSYS_SHIFT_SWR_PSW_WAIT_CYC	24
#define BIT_LSYS_NASK_SWR_PSW_WAIT_CYC	0xFF	/* [31:24]	R/W	0xFF		"Wait time of each step during SWR voltage adjust process, the timeunit is control by 0x13C[21:16]: BIT_LSYS_ANAIP_CTL_TUTIMEDecrease voltage:  Step0 ->  Step1 -> Step2 -> Step3 -> Step4Increase voltage: Step4 ->  Step3 -> Step2 -> Step1 -> Step0" */
#define BIT_LSYS_SHIFT_SWR_REQ_DBNC_CYC	16
#define BIT_LSYS_MASK_SWR_REQ_DBNC_CYC	0xFF	/* [23:16]	R/W	0x1F		De-bounce time of SWR voltage adjust request from WIFI & SYSON PMC,   the timeunit is control by 0x13C[21:16]: BIT_LSYS_ANAIP_CTL_TUTIME */
#define BIT_LSYS_SHIFT_SWR_STEP1_VOL		12
#define BIT_LSYS_MASK_SWR_STEP1_VOL		0x0F	/* [15:12]	R/W	0x7		Step 1 voltage setting for SWR voltage adjust, default 1.043v */
#define BIT_LSYS_SHIFT_SWR_STEP2_VOL		8
#define BIT_LSYS_MASK_SWR_STEP2_VOL		0x0F	/* [11:8]	R/W	0x5		Step 2 voltage setting for SWR voltage adjust, default 1.000v */
#define BIT_LSYS_SHIFT_SWR_STEP3_VOL		4
#define BIT_LSYS_MASK_SWR_STEP3_VOL		0x0F	/* [7:4]	R/W	0x3		Step 3 voltage setting for SWR voltage adjust, default 0.941v */
#define BIT_LSYS_SHIFT_SWR_STEP4_VOL		0
#define BIT_LSYS_MASK_SWR_STEP4_VOL		0x0F	/* [3:0]	R/W	0x1		Step 4 voltage setting for SWR voltage adjust, default 0.906v */

/* REG_XMODE_SW_CTRL					0x033C */
#define BIT_LSYS_SHIFT_ANAIP_CTL_TUTIME	16
#define BIT_LSYS_MASK_ANAIP_CTL_TUTIME	0x3F	/* [21:16]	R/W	0xF		"Timeunit of  SWR/XTAL arbitration circuit, in unit of ANA4M,Default timeunit:  4us" */
#define BIT_LSYS_SHIFT_XTAL_BG2HPMOD_CYC	8
#define BIT_LSYS_MASK_XTAL_BG2HPMOD_CYC	0xFF	/* [15:8]	R/W	0x1F		The time interval between BG enable to XTAL switch to HP Mode,  the timeunit is control by 0x13C[21:16]: BIT_LSYS_ANAIP_CTL_TUTIME */
#define BIT_LSYS_SHIFT_XTAL_DBNC_CYC		0
#define BIT_LSYS_MASK_XTAL_DBNC_CYC		0x0F	/* [7:0]	R/W	0x1F		De-bounce time of XTAL enable request from WIFI & SYSON PMC,   the timeunit is control by 0x13C[21:16]: BIT_LSYS_ANAIP_CTL_TUTIME */

/* REG_GPIO_SHDN33_CTRL				0x0340 */
#define BIT_LSYS_SHIFT_WAKE_GPIO_VALUE	28
#define BIT_LSYS_MASK_WAKE_GPIO_VALUE	0x0F	/* [31:28]	R	0		Input value of AON wake GPIO */
#define BIT_LSYS_GPIO_GRP6_SHDN33			BIT(6)	/* [6]	R/W	1		" 1:  Enable GPIO Group6 0:  Shutdown GPIO Group6" */
#define BIT_LSYS_GPIO_GRP5_SHDN33			BIT(5)	/* [5]	R/W	1		" 1:  Enable GPIO Group5 0:  Shutdown GPIO Group5" */
#define BIT_LSYS_GPIO_GRP4_SHDN33			BIT(4)	/* [4]	R/W	1		" 1:  Enable GPIO Group4 0:  Shutdown GPIO Group4" */
#define BIT_LSYS_GPIO_GRP3_SHDN33			BIT(3)	/* [3]	R/W	1		" 1:  Enable GPIO Group3 0:  Shutdown GPIO Group3" */
#define BIT_LSYS_GPIO_GRP2_SHDN33			BIT(2)	/* [2]	R/W	1		" 1:  Enable GPIO Group2 0:  Shutdown GPIO Group2" */
#define BIT_LSYS_GPIO_GRP1_SHDN33			BIT(1)	/* [1]	R/W	1		" 1:  Enable GPIO Group1 0:  Shutdown GPIO Group1" */
#define BIT_LSYS_GPIO_GRP0_SHDN33			BIT(0)	/* [0]	R/W	1		" 1:  Enable GPIO Group0 0:  Shutdown GPIO Group0" */

/* REG_AUDIO_SHARE_PAD_CTRL			0x0344 */
#define BIT_LSYS_AC_ANA_PORB				BIT(9)	/* [9]	R/W	0		1: Enable Audio pad function, 0: disable */
#define BIT_LSYS_AC_MBIAS_POW				BIT(8)	/* [8]	R/W	0		1: Enable Audio bandgap,  0: disable */
#define BIT_LSYS_SHIFT_AC_LDO_REG			1
#define BIT_LSYS_MASK_AC_LDO_REG			0x7F	/* [7:1]	R/W	0		Audio LDO voltage setting */
#define BIT_LSYS_AC_LDO_POW				BIT(0)

/* REG_BOD_DBNC_CTRL					0x0348 */
#define BIT_LSYS_BOD_DBNC_FEN				BIT(16)	/* [16]	R/W	1		1: Enable BOD de-bounce function */
#define BIT_LSYS_SHIFT_BOD_DBNC_CYC		0
#define BIT_LSYS_MASK_BOD_DBNC_CYC		0x7FFF	/* [14:0]	R/W	0x1000		BOD de-dounce time, in unit of ANA4M clock cycles */

/* REG_SYSON_PMC_PATCH_GRPx 0x0350~0x037F, x=1~6 */
#define BIT_LSYS_SHIFT_PATCH_GRPx_INSTR3	44
#define BIT_LSYS_MASK_PATCH_GRPx_INSTR3	0x3FFFF	/* [61:44]	R/W	0		instruction3 of PMC patch groupx */
#define BIT_LSYS_SHIFT_PATCH_GRPx_INSTR2	26
#define BIT_LSYS_MASK_PATCH_GRPx_INSTR2	0x3FFFF	/* [43:26]	R/W	0		instruction2 of PMC patch groupx */
#define BIT_LSYS_SHIFT_PATCH_GRPx_INSTR1	8
#define BIT_LSYS_MASK_PATCH_GRPx_INSTR1	0x3FFFF	/* R/W	0		instruction1 of PMC patch groupx */
#define BIT_LSYS_SHIFT_PATCH_GRPx_SADDR	0
#define BIT_LSYS_MASK_PATCH_GRPx_SADDR	0xFF	/* R/W	0		Start address of the first instruction of PMC patch groupx */

/* REG_LP_GDMA_CTRL 0x0380 */
#define BIT_LSYS_GDMA_CLK_ALWAYS_DISABLE	BIT(5)	/* R/W	0		Disable dmac clk all the time. if clk_always_enable=0 and clk_always_disable=1,  the whole dmac¡¯s clk will be shut down to save power. */
#define BIT_LSYS_GDMA_CLK_ALWAYS_ENABLE		BIT(4)	/* R/W	0		Enable dmac clk all the time. It has the highest priority. If clk_always_enable=0 and clk_always_disable=0, dmac can shut any channel¡¯s clk when this channel has no job to save power. If clk_always_enable=1, all the channel¡¯s clk will be valid. */

/* REG_LP_EFUSE_PROTECTION 0x03E0 */
#define EFUSE_PHYSICAL_SBOOT_ON		BIT_SECURE_BOOT_DIS
#define BIT_CPU_PC_DBGEN				BIT(14)	/* R	1		"1: Enable to get KM4/KM0 PC value through debug port0: Disable" */
#define BIT_SECURE_BOOT_DIS			BIT(13)	/* R	1		1: Disable Secure boot ,  0: Enable */
#define BIT_LSYS_INVA_DBGEN			BIT(12)	/* R	1		1: Enable invasive debug */
#define BIT_LSYS_NONINVA_DBGEN		BIT([11)	/* R	1		1: Enable non-invasive debug */
#define BIT_LSYS_SEC_INVA_DBGEN		BIT(10)	/* R	1		1: Enable secure invasive debug */
#define BIT_LSYS_SEC_NONINVA_DBGEN	BIT(9)	/* R	1		1: Enable secure non-invasive debug */
#define BIT_LSYS_SWD_PROTECT_DIS		BIT(8)	/* R	1		1: Disable SWD password protection;  0: Enable */
#define BIT_LSYS_SIC_SECURE_EN			BIT(7)	/* R	1		1: Permit SIC to access Secure space; 0: forbidden */
#define BIT_LSYS_SBOOT_PUBKEY_WREN	BIT(6)	/* R	1		1: Permit to PG secure boot public key; 0: fobidden */
#define BIT_LSYS_RSIP_KEY_WREN		BIT(5)	/* R	1		1: Permit to PG RSIP key; 0: fobidden */
#define BIT_LSYS_SWD_PWD_WREN		BIT(3)	/* R	1		1: Permit to PG swd password; 0: fobidden */
#define BIT_LSYS_RSIP_KEY_RDEN			BIT(2)	/* R	1		1: Permit to Read RSIP key from Efuse; 0: fobidden */
#define BIT_LSYS_SWD_PWD_RDEN		BIT(0)	/* R	1		1: Permit to Read swd password from Efuse; 0: fobidden */


/*  REG_LP_SYSTEM_CFG0 0x03F0 */
#define BIT_SYSCFG_BD_PKG_SEL			(0x00000001 << 31) /*!< 0: normal package; 1: engineering mode */
#define BIT_SysCFG_TRP_UART_IMAGE		(0x00000001 << 30)
#define BIT_SysCFG_TRP_SPSLDO_SEL		(0x00000001 << 26) /*!< Trapped Selection for SPS 0: SWR mode; 1: LDO mode */
#define BIT_SysCFG_TRP_TESTMODE_EN	(0x00000001 << 25) /*!< Trapped Selection for Test Mode 0: Normal mode; 1: Test mode */
#define BIT_MASK_SYSCFG_ALF_EN		(0x0000000F << 24) /* autoload fail */
#define BIT_MASK_SYSCFG_CHIP_TYPE		(0x00000001 << 7) /*!< TestChip ID, 1?: Test (RLE)?; 0?: MP (RL), BUT AMebaD always is 1 */
#define BIT_MASK_SYSCFG_VENDOR_ID	(0x00000003 << BIT_SHIFT_SYSCFG_VENDOR_ID) /*!<[5:4] 00: TSMC , 01: SMIC, 10: UMC */
#define BIT_SHIFT_SYSCFG_VENDOR_ID	4
#define BIT_MASK_SYSCFG_CUT_VER		(0x0000000F << BIT_SHIFT_SYSCFG_CUT_VER) /*!<[3:0] Chip version */
#define BIT_SHIFT_SYSCFG_CUT_VER		0

/*  REG_LP_SYSTEM_CFG1 0x03F4 */
#define BIT_SHIFT_SYSCFG_TRP_ICFG		28 /* "Trapped ICFG data value0: normall modeXXXX: the other mode (Test Mode)" */
#define BIT_MASK_SYSCFG_TRP_ICFG		(u32)(0x0000000f << BIT_SHIFT_SYSCFG_TRP_ICFG)
#define BIT_MCM_FLASH_FT_ICFG			(u32)(0x00000004 << BIT_SHIFT_SYSCFG_TRP_ICFG)
#define BIT_FORCE_USE_S0_ICFG			(u32)(0x0000000e << BIT_SHIFT_SYSCFG_TRP_ICFG)
#define BIT_MCM_FLASH_PG_ICFG			(u32)(0x0000000c << BIT_SHIFT_SYSCFG_TRP_ICFG)
#define BIT_SYSPLL_CLKRDY				BIT(12)	/* R	0		system pll ready flag,  1: ready */
#define BIT_SPS_BG_VLD					BIT(11)	/* R	0		SWR Bandgap valid flag,  1: valid */
#define BIT_OSC128K_CLKRDY				BIT(10)	/* R	0		OSC 128k ready flag,  1: ready */
#define BIT_XTAL32K_CLKRDY				BIT(9)	/* R	0		XTAL 32k ready flag,  1: ready */
#define BIT_SYS_XCLK_VLD				BIT(8) /*Xtal Clock Stable, 1: Clock Stable */
#define BIT_SYSCFG_ALDN_STS			BIT(0) /*!< 1: SYS CFG autoload done; 0: SYSCFG autoload not ready (if done, system reg can not write) */

/*  REG_LP_SYSTEM_CFG2 0x03F8 */
#define BIT_SOC_BOOT_FROM_JTAG			BIT(31)
#define BIT_SOC_BOOT_RSVD_BIT30			BIT(30) /* reseved */
#define BIT_SOC_BOOT_WAKE_FROM_PS		BIT(29)
#define BIT_SOC_BOOT_PATCH_FUNC0			BIT(28)
#define BIT_SOC_BOOT_PATCH_FUNC1			BIT(27)
#define BIT_SOC_BOOT_PATCH_FUNC2			BIT(26)
#define BIT_SOC_BOOT_PATCH_KM4_RUN		BIT(25)
#define BIT_SOC_BOOT_WAKE_FROM_PS_HS		BIT(24)
#define BIT_BOOT_IMG3_EXIST				BIT(23)
#define BIT_BOOT_DSLP_RESET_HAPPEN		BIT(21)	/*!<  software backup from REG_AON_BOOT_REASON1-BIT_AON_BOOT_EXIT_DSLP */
#define BIT_BOOT_KM4WDG_RESET_HAPPEN	BIT(20)	/*!<  watchdog reset, software backup from BIT4 */
#define BIT_BOOT_KM4SYS_RESET_HAPPEN		BIT(19)	/*!<  system reset, software backup from BIT3 */
#define BIT_BOOT_BOD_RESET_HAPPEN			BIT(18)	/*!<  software backup from REG_AON_BOOT_REASON1-BIT_AON_BOD_RESET_FLAG */
#define BIT_BOOT_WDG_RESET_HAPPEN		BIT(17)	/*!<  watchdog reset, software backup from BIT1 */
#define BIT_BOOT_SYS_RESET_HAPPEN			BIT(16)	/*!<  system reset, software backup from BIT0 */
#define BIT_BOOT_REASON_SHIFT				16
#define BIT_BOOT_REASON_MASK				0x3F

#define BIT_MASK_SYSCFG_ROM_INFO			(0x0000FFFF)



/* VENDOR REG */
/* #define REG_OSC_CAL							0x2840 */
#define BIT_CAL_START					BIT(31)	/* R/W	0		Write 1 to Start calibration,  auto clear to 0 when calibration done	*/
#define BIT_SHIFT_CAL_CLK_SEL			24
#define BIT_MASK_CAL_CLK_SEL			0x03	/* [25:24]	R/W	0		"Calbiration source clock selection: 2'b00:  ANA4M2'b01:  100K 2'b10/2'b11:  OSC131K"	*/	
#define BIT_SHIFT_CAL_RPT				0
#define BIT_MASK_CAL_RPT				0xFFFF	/* [15:0]	R			"Read only, Here XTAL clock is used to count N cycles of calibration clock, the total used XTAL clock cycles can be read by this register When calibration clock is ANA4M,  N = 32otherwise N= 8"	*/	

/* #define REG_OSC2M_CAL						0x2844 */						
#define BIT_SHIFT_OSC2M_CAL_RPT		16
#define BIT_MASK_OSC2M_CAL_RPT		0xFFFF	/* [31:16]	R			"Read only, Here XTAL clock is used to count N cycles of OSC2M clock, the total used XTAL clock cycles can be read by this register N is decided by 0x44[15:14]"	*/	
#define BIT_SHIFT_OSC2M_CAL_CYC_SEL	12
#define BIT_MASK_OSC2M_CAL_CYC_SEL	0x03	/* [13:12]	R/W	0		"Target cycles of OSC2M2'b00: 16 2'b01: 322'b10: 642'b11: 128"	*/	
#define BIT_SHIFT_OSC2M_CAL_DLY		8
#define BIT_MASK_OSC2M_CAL_DLY		0xF		/* [11:8]	R/W	0		HW will wait (BIT_OSC2M_CAL_DLY+1) OSC2M cycles before start to count N cycles of OSC2M clock by XTAL		*/
#define BIT_OSC2M_CAL_FEN				BIT(1)	/* R/W	0		"OSC2M Calibration function enable1: Enable,  0: Disable"		*/
#define BIT_OSC2M_CAL_START			BIT(0)	/* R/W	0		Write 1 to Start calibration,  auto clear to 0 when calibration done	*/	

/* SDM REG */
/* #define REG_SDM_CTRL0						0x000 */
#define BIT_AON_SDM_ONE_CAL_EN				BIT(31)		/* [31]	R/W	0		1: Open SDM32K calibration once when first enable */
#define BIT_AON_SDM_FUNC_EN					BIT(30)		/* [30]	R/W	0		1: Enable SDM32K function*/
#define BIT_AON_SDM_DONE						BIT(27)		/* [27]	R/W	0	*/
#define BIT_AON_SDM_ALWAYS_CAL_EN			BIT(18)		/* [18]	R/W	0		1: Enable SDM32K always calibration */
#define BIT_AON_SDM_RTC_CAL_EN				BIT(17)		/* [17]	R/W	0		1: Enable SDM32K RTC trigger calibration */
#define BIT_AON_SDM_AUTO_CAL_EN				BIT(16)		/* [16]	R/W	0		1: Enable SDM32K auto calibration */

#define BIT_MASK_AUTO_CAL_MAX_ERROR			0xFFFFFFFF	/* [31:0]	R/W	0		automatic calibration error threshold, will re-calibration when source clock frequcy overrange the threshold		*/


//================= LP AON Register Address Definition =====================//
#define REG_AON_PWR_CTRL					0x0000
#define REG_AON_ISO_CTRL					0x0004
#define REG_AON_BOOT_REASON1				0x000C
#define REG_AON_LDO_CTRL0					0x0010
#define REG_AON_LDO_CTRL1					0x0014
#define REG_MEM_PMCTRL_DSLP				0x001C
#define REG_XTAL32K_CTRL0					0x0020
#define REG_XTAL32K_CTRL1					0x0024
#define REG_OSC32K_CTRL					0x002C
#define REG_AON_WAKE_TIMER0				0x0030
#define REG_AON_WAKE_TIMER0_CNT			0x0034
#define REG_AON_PAD_CTRL					0x0038
#define REG_AON_WAKE_GPIO_CTRL1			0x003C
#define REG_AON_WAKE_GPIO_CTRL2			0x0040
#define REG_AON_DBNC_CTRL					0x0044
#define REG_AON_CK32KGEN_AD				0x0048
#define REG_AON_CK32KGEN_RWD				0x004C
#define REG_AON_PMC_CTRL					0x0050
#define REG_AON_PM_OPT						0x0054
#define REG_AON_WAKE_OPT_MSK				0x0058
#define REG_AON_WAKE_OPT_STS				0x005C
#define REG_AON_SNOOZE_MSK				0x0060
#define REG_AON_SNOOZE_STS				0x0064
#define REG_AON_CHIP_PWR_DOWN_MSK		0x0068
#define REG_AON_CHIP_PWR_DOWN_STS		0x006C
#define REG_AON_TSF_BCN_CFG1				0x0090
#define REG_AON_TSF_BCN_CFG2				0x0094
#define REG_AON_TSF_CNT_LOW				0x0098
#define REG_AON_TSF_CNT_HIGH				0x009C

#define REG_AON_XTAL_CTRL					0x00C8

//================= LP SYSON Register Address Definition =====================//
#define REG_LP_SYSON_PMC_PATCH_REG		0x0150 /*!< LS_SYSON_PMC patch, autoload from LEFUSE[0x200~] */

#define REG_LP_PWR_ISO_CTRL				0x0200 /*!< 0x00: REG_SYS_PWR_CTRL, 0x02: REG_SYS_ISO_CTRL */
#define REG_LP_KM0_CTRL					0x0204
#define REG_LP_FUNC_EN0					0x0208
#define REG_LP_FUNC_EN1					0x020C
#define REG_LP_CLK_CTRL0					0x0210
#define REG_LP_CLK_CTRL1					0x0214
#define REG_LP_FLASH_CTRL					0x0218

#define REG_LP_KM4_CTRL					0x021C

#define REG_SYS_EFUSE_SYSCFG0				0x0220 /*!< BYTE0x00~0x03 */
#define REG_SYS_EFUSE_SYSCFG1				0x0224 /*!< BYTE0x04~0x07 */
#define REG_SYS_EFUSE_SYSCFG2				0x0228 /*!< BYTE0x08~0x0B: XTAL */
#define REG_SYS_EFUSE_SYSCFG3				0x022C /*!< BYTE0x0C~0x0F */
#define REG_SYS_EFUSE_SYSCFG4				0x05e0 /*!< BYTE0x10~0x13 */
#define REG_SYS_EFUSE_SYSCFG5				0x05e4 /*!< BYTE0x14~0x17 */
#define REG_SYS_EFUSE_SYSCFG6				0x05e8 /*!< BYTE0x18~0x1B */
#define REG_SYS_EFUSE_SYSCFG7				0x05eC /*!< BYTE0x1C~0x1F */

#define REG_SWD_PMUX_EN					0x0230

#define REG_LP_SWR_CTRL0					0x0240
#define REG_LP_SWR_CTRL1					0x0244
#define REG_LP_SWR_CTRL2					0x0248
#define REG_LP_SWR_CTRL3					0x024C

#define REG_LP_LDO_CTRL0					0x0250

#define REG_LP_XTAL_CTRL0					0x0260
#define REG_LP_XTAL_CTRL1					0x0264
#define REG_LP_XTAL_CTRL2					0x0268
#define REG_LP_XTAL_CTRL3					0x026C
#define REG_LP_XTAL_CTRL4					0x0270
#define REG_LP_XTAL_CTRL5					0x0274
#define REG_LP_XTAL_CTRL6					0x0278

#define REG_LP_SYSPLL_CTRL0				0x0280

#define REG_LP_OSC4M_CTRL					0x0290
#define REG_LP_OSC2M_CTRL					0x0298

#define REG_LP_DEBUG_REG					0x02A0
#define REG_LP_PINMUX_CTRL					0x02A4

#define REG_PLL_SYS_REG0					0x02B0
#define REG_PLL_SYS_REG1					0x02B4
#define REG_PLL_SYS_REG2					0x02B8
#define REG_PLL_SYS_REG3					0x02BC
#define REG_PLL_SYS_PS_REG					0x02C0

#define REG_MEM_CTRL2						0x02D8

#define REG_LP_EFUSE_CTRL0					0x02E4
#define REG_LP_EFUSE_CTRL1					0x02E8	/* 0x480002E8, is for non-secure efuse access, can only access NS_EFUSE ~[0x150~0x17F] */
#define REG_HS_EFUSE_CTRL1_S				0x0810	/* 0x50000810, is for secure efuse access, can only access TZ_EFUSE[0x150~0x17F] */
#define REG_LP_EFUSE_TEST					0x02EC

/* request RSIP Key autoload */
#define REG_LP_RSIP_KEY_CTRL				0x02F0

#define REG_LP_DSLP_INFO_SW				0x02F4 /* just software use */
#define REG_LP_SLP_WAKE_EVENT_MSK0		0x0300
#define REG_LP_SLP_WAKE_EVENT_STATUS0	0x0304
#define REG_LP_SLP_WAKE_EVENT_MSK1		0x0308
#define REG_LP_SLP_WAKE_EVENT_STATUS1	0x030C
#define REG_LP_PWRMGT_CTRL				0x0318
#define REG_LP_PWRMGT_OPTION				0x0320

#define REG_LP_WL_INTR_CTRL				0x0310
#define REG_LP_WL_POW_CTRL				0x0314
#define REG_LP_RTC_XTAL_DIV_CTRL			0x0330
#define REG_LP_UART_XTAL_DIV_CTRL			0x0334
#define REG_SWR_PSW_CTRL					0x0338
#define REG_XMODE_SW_CTRL					0x033C
#define REG_GPIO_SHDN33_CTRL				0x0340
#define REG_AUDIO_SHARE_PAD_CTRL			0x0344
#define REG_BOD_DBNC_CTRL					0x0348
/* PMC ROM Patch */
#define REG_SYSON_PMC_PATCH_GRP1_L		0x0350
#define REG_SYSON_PMC_PATCH_GRP1_H		0x0354
#define REG_SYSON_PMC_PATCH_GRP2_L		0x0358
#define REG_SYSON_PMC_PATCH_GRP2_H		0x035C
#define REG_SYSON_PMC_PATCH_GRP3_L		0x0360
#define REG_SYSON_PMC_PATCH_GRP3_H		0x0364
#define REG_SYSON_PMC_PATCH_GRP4_L		0x0368
#define REG_SYSON_PMC_PATCH_GRP4_H		0x036C
#define REG_SYSON_PMC_PATCH_GRP5_L		0x0370
#define REG_SYSON_PMC_PATCH_GRP5_H		0x0374
#define REG_SYSON_PMC_PATCH_GRP6_L		0x0378
#define REG_SYSON_PMC_PATCH_GRP6_H		0x037C

#define REG_LP_GDMA_CTRL					0x0380

#define REG_LP_BOOT_REASON0				0x03C0
#define REG_LP_BACKUP_REG1					0x03C4
#define REG_LP_BACKUP_REG2					0x03C8
#define REG_LP_BACKUP_REG3					0x03CC
#define REG_LP_BACKUP_REG4					0x03D0
#define REG_LP_BACKUP_REG5					0x03D4
#define REG_LP_BACKUP_REG6					0x03D8
#define REG_LP_BACKUP_REG7					0x03DC

#define REG_LP_EFUSE_PROTECTION			0x03E0 /* RO, Autoload From EFUSE 0x1C0/0x1C1 */

#define REG_LP_SYSTEM_CFG0					0x03F0
#define REG_LP_SYSTEM_CFG1					0x03F4
#define REG_LP_SYSTEM_CFG2					0x03F8
#define REG_LP_BOOT_CFG					REG_LP_SYSTEM_CFG2
#define REG_SOC_FUNC_EN					REG_LP_SYSTEM_CFG2

#define REG_GPIO_F9_PAD_CTRL				0x0508

//================= LP VENDOR REG Register Address Definition =====================//
#define REG_OSC_CAL							0x2840
#define REG_OSC2M_CAL						0x2844

//================= LP SDM Register Address Definition =====================//
#define REG_SDM_CTRL0						0x000
#define REG_SDM_CTRL1						0x001
#define REG_SDM_CTRL2						0x002
#define REG_SDM_CTRL3						0x003
#define REG_SDM_CTRL4						0x004
#define AUTO_CAL_MAX_ERROR				0x007

static inline u32
is_power_supply18(void)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);

	if (temp & BIT_AON_V1833_SEL)
		return FALSE;
	else
		return TRUE;
}


#endif //__INC_RTL8721D_LP_SYS_ON_BIT_H
